bool getModbus(uint8_t u8a_mbReq[gk_u16_mbArraySize], uint16_t u16_mbReqLen, uint8_t u8a_mbResp[gk_u16_mbArraySize], 
  uint16_t &u16_mbRespLen) {

  uint16_t u16_reqReg, u16_numRegs, u16_adjReqReg = 0, u16_adjNumRegs = 0;
  uint8_t u8_mbRespNumBytes = 0;
  uint8_t u8_mbResult = 0xFF;
  uint16_t u16_tempReg = 0;
  uint8_t u8_dataTypeFlags = 0;
  bool b_foundReg = false;
  uint8_t u8_mbReqFunc = 0;
  uint8_t u8_mbError = 0;
  bool b_reqRegManip = false;
  bool b_mbStatus = false;
  uint8_t u8_mskdDataTypeFlags = 0;
  uint8_t u8_mtrType = 0;
  uint8_t u8_mtrId;
  
#if defined(__arm__) && defined(CORE_TEENSY)  // if teensy3.0 or greater
#else
  struct stFloat {
    uint32_t m : 23;
    uint8_t e : 8;
    uint8_t s : 1;
  };

  struct stDbl {
    uint32_t filler : 29;
    uint32_t m : 23;
    uint16_t e : 11;
    uint8_t s : 1;
  };

  union dblConv {
    stDbl sD;
    uint16_t u16[4];
  }  dblC;
#endif

  union convertUnion {
    float f;
    uint32_t u32;
    int32_t s32;
    uint16_t u16[4];
    int16_t s16;
    uint64_t u64;
#if defined(CORE_TEENSY)  // if teensy3.0 or greater
    double dbl;
#else
    stFloat sF;
#endif
  } int2flt;
  
  int2flt.u32 = 0;  // unsure if this needs to be done

  if (u16_mbReqLen == 12);  // typical modbus/tcp message is 12 bytes long
  {
    u8a_mbResp[0] = u8a_mbReq[0]; // copy first 2 bytes to outbound message
    u8a_mbResp[1] = u8a_mbReq[1];
    u8a_mbResp[2] = 0;
    u8a_mbResp[3] = 0;
    
    u8a_mbResp[6] = u8a_mbReq[6]; // set device
    
    uint8_t u8_mtrVid = u8a_mbReq[6];
    uint8_t u8a_clientIp[4];
    u8_mtrId = u8a_mbReq[6];  // let id given by incoming packet be default, it will change if it matches with something on the list in isMeterEth
    // search for slave id in eeprom, if there check if ip exists
    // isMeterEth changes u8_mtrType and u8_mtrId
    if (isMeterEth(u8a_clientIp, u8_mtrVid, u8_mtrType, u8_mtrId)){  // isMeterEth set g_u8a_clientIP
      g_mm_node.setSerialEthernet(false);  // <--false means ethernet
      g_mm_node.setClientIP(u8a_clientIp);
      //Serial.println(F("ETHER!"));
    }
    else{
      g_mm_node.setSerialEthernet(true);  // true means serial
    }
    
    g_mm_node.setSlave(u8_mtrId);
    
    
    u16_reqReg = (u8a_mbReq[8] << 8) | (u8a_mbReq[9]);
    u16_numRegs = (u8a_mbReq[10] << 8) | (u8a_mbReq[11]);
    u8_mbRespNumBytes = u16_numRegs * 2;
    u8_mbReqFunc = u8a_mbReq[7];
    
      
    if (u16_reqReg < 10000) {
      u16_adjReqReg = u16_reqReg;
      u16_adjNumRegs = u16_numRegs;
    }
    else if ((u16_reqReg > 9999) && (u16_reqReg < 20000)) {
      b_reqRegManip = true;  // 10k request
      u16_adjReqReg = u16_reqReg - 10000;
      b_foundReg = findRegister(u16_adjReqReg, u8_dataTypeFlags, u8_mtrType);
      
  //    Serial.println(u16_adjReqReg);
      if (0x01 & u16_numRegs) // if odd number, then can't return float (must be even # of regs)
      {
//        Serial.println("odd");
        u8_mbError = 0x02;
      }
      else
      {
        if (b_foundReg)  // found registers
        {
          u8_mskdDataTypeFlags = (0x7f & u8_dataTypeFlags); // ignore ws bit for comparisons
          
          // u16_adjNumRegs is the length request sent to modbus device
          // all requests made through 10k registers must be with expectation of float
          // this means the unadjusted length request has 2 registers for every unique data point
          switch (u8_mskdDataTypeFlags){
            case 1:  // u16
            case 2:  // s16
              u16_adjNumRegs = u16_numRegs / 2;  // single register values
              break;
            case 7:  // m20k
              u16_adjNumRegs = u16_numRegs * 3 / 2;  // 3 register values
              break;
            case 8:  // m30k
            case 9:  // u64
            case 10:  // engy
            case 11:  // dbl
              u16_adjNumRegs = u16_numRegs * 2;  // 4 register values
              break;
            default:  // float, u32, s32, m10k, m1k
              u16_adjNumRegs = u16_numRegs;  // 2 register values
              break;              
          }
        }
        else
        {
//          Serial.println("no flags");
          u8_mbError = 0x02;
        }  // end if else check if address in block
      }  // end if else check lgth  
    }  // end if 10k request
    else
    {
//      Serial.println("reg outside exp");
      u8_mbError = 0x02;
    }
    
    if ((!(u8_mbReqFunc == 3) || (u8_mbReqFunc == 4)))
    {
      u8_mbError = 0x01;
    }
    
    if (u8_mbError) {  // error occurred prior to actual modbus request, return error
//      Serial.println("error in gateway"); 
//      Serial.println(u8_mbError, HEX);
      u8a_mbResp[7] = (u8_mbReqFunc | 0x80); // return function  + 128
      u8a_mbResp[8] = u8_mbError; // modbus error function
      u8a_mbResp[4] = 0;  // expected tcp length
      u8a_mbResp[5] = 3;  // expected tcp length

      u16_mbRespLen = 9;
      /*Serial.print("error: ");
      Serial.println(u8_mbError, DEC);*/
     
      return false;
    }  // end if error
    else {  // no error yet, handle code
      //if (!b_reqRegManip) {
        switch (u8_mbReqFunc) {
//          case 1:
//            g_mm_node.readCoils(u16_adjReqReg, u16_adjNumRegs);
//            break;
//          case 2:
//            g_mm_node.readDiscreteInputs(u16_adjReqReg, u16_adjNumRegs);
//            break;
          case 3:
            //Serial.println(F("modbus request"));
            u8_mbResult = g_mm_node.readHoldingRegisters(u16_adjReqReg, u16_adjNumRegs);
            //Serial.print(F("sent request: "));
            //Serial.println(u8_mbResult, HEX);
            break;
          case 4:
            u8_mbResult = g_mm_node.readInputRegisters(u16_adjReqReg, u16_adjNumRegs);
            break;
//          case 5:
//            u8_mbResult = g_mm_node.writeSingleCoil(u16_adjReqReg, u16_adjNumRegs);
//            break;
//          case 6:
//            u8_mbResult = g_mm_node.writeSingleRegister(u16_adjReqReg, u16_adjNumRegs);
//            break;
          default:
            u8_mbResult = 0x01;
            break;
        }  // end switch function
//      }  // end if typical register request
//      else {
//        /*Serial.println("requesting...");
//        Serial.println(u16_adjReqReg, DEC);
//        Serial.println(u16_adjNumRegs, DEC);*/
//        switch (u8_mbReqFunc) {
//          case 3:
//            //Serial.println(F("modbus 10k request"));
//            u8_mbResult = g_mm_node.readHoldingRegisters(u16_adjReqReg, u16_adjNumRegs);
////            Serial.print(F("sent request: "));
////            Serial.println(u8_mbResult, HEX);
//            break;
//          case 4:
//            u8_mbResult = g_mm_node.readInputRegisters(u16_adjReqReg, u16_adjNumRegs);
//            break;
//          default:
//            u8_mbResult = 0x01;
//            break;
//        }  // end switch function
////        Serial.print("u8_mbResult: ");
////        Serial.println(u8_mbResult, HEX);
//      }// end else if 10k request
      
      switch (u8_mbResult) {
        case 0:  // g_mm_node.ku8MBSuccess
          if (!b_reqRegManip) {  // no adjustments to data
            for (int jj = 0, ii = 9; jj < u16_adjNumRegs; ++jj, ii+=2) {
              u16_tempReg = g_mm_node.getResponseBuffer(jj);
              
              u8a_mbResp[ii] = highByte(u16_tempReg);
              u8a_mbResp[ii + 1] = lowByte(u16_tempReg);
            }
          }
          else {
//            Serial.println("handling 10k data");
//            Serial.println((u8_dataTypeFlags & 0x7F), DEC);
          
            switch (u8_mskdDataTypeFlags) {  // ((u8_dataTypeFlags & 0x7F))
              case 0:  // float
                if (u8_dataTypeFlags & 0x80){  // if ws
                  for (int jj = 0, ii = 9; jj < u16_adjNumRegs; jj+=2, ii+=4){  // u16_adjNumRegs is # of registers - divide since handling by two
                    u16_tempReg = g_mm_node.getResponseBuffer(jj);
                    u8a_mbResp[ii + 2] = (u16_tempReg >> 8);  // would memcpy work better here?
                    u8a_mbResp[ii + 3] = u16_tempReg;

                    u16_tempReg = g_mm_node.getResponseBuffer(jj + 1);
                    u8a_mbResp[ii] = (u16_tempReg >> 8);
                    u8a_mbResp[ii + 1] = u16_tempReg;
                  }
                }
                else{  // no ws, no adjustments needed
                  for (int jj = 0, ii = 9; jj < u16_adjNumRegs; ++jj, ii+=2){
                    u16_tempReg = g_mm_node.getResponseBuffer(jj);
                    u8a_mbResp[ii] = (u16_tempReg >> 8);
                    u8a_mbResp[ii + 1] = u16_tempReg;
                  }
                }
                break;
              case 1: // u16 to float
                for (int jj = 0, ii = 9; jj < u16_adjNumRegs; ++jj, ii+=4){  // u16_adjNumRegs is # of registers - divide since handling by two
                  u16_tempReg = g_mm_node.getResponseBuffer(jj);
      
                  int2flt.f = (float)u16_tempReg;
                  
                  u8a_mbResp[ii] = (int2flt.u32 >> 8);
                  u8a_mbResp[ii + 1] = int2flt.u32;
                  
                  u8a_mbResp[ii + 2] = (int2flt.u32 >> 24);
                  u8a_mbResp[ii + 3] = (int2flt.u32 >> 16); 
                }
                break;
              case 2: // s16 to float
                for (int jj = 0, ii = 9; jj < u16_adjNumRegs; ++jj, ii+=4){  // u16_adjNumRegs is # of registers - divide since handling by two
                  int2flt.s16 = g_mm_node.getResponseBuffer(jj);  // may require (int) cast
                  
                  int2flt.f = (float) int2flt.s16;
                  
                  u8a_mbResp[ii] = (int2flt.u32 >> 8);
                  u8a_mbResp[ii + 1] = int2flt.u32;
                  
                  u8a_mbResp[ii + 2] = (int2flt.u32 >> 24);
                  u8a_mbResp[ii + 3] = (int2flt.u32 >> 16); 
                }
                break;
              case 3: // u32 to float
                for (int jj = 0, ii = 9; jj < u16_adjNumRegs; jj+=2, ii+=4){  // u16_adjNumRegs is # of registers - divide since handling by two
                  if((u8_dataTypeFlags & 0x80)){  // WORDSWAP
                    int2flt.u16[1] = g_mm_node.getResponseBuffer(jj);
                    int2flt.u16[0] = g_mm_node.getResponseBuffer(jj + 1);
                  }
                  else{
                    int2flt.u16[0] = g_mm_node.getResponseBuffer(jj);
                    int2flt.u16[1] = g_mm_node.getResponseBuffer(jj + 1);
                  }
                  
                  int2flt.f = (float) int2flt.u32;
                  
                  u8a_mbResp[ii] = (int2flt.u32 >> 8);
                  u8a_mbResp[ii + 1] = int2flt.u32;
                  
                  u8a_mbResp[ii + 2] = (int2flt.u32 >> 24);
                  u8a_mbResp[ii + 3] = (int2flt.u32 >> 16); 
                }
                break;
              case 4: // s32 to float
                for (int jj = 0, ii = 9; jj < u16_adjNumRegs; jj+=2, ii+=4)  // u16_adjNumRegs is # of registers - divide since handling by two
                { 
                  if((u8_dataTypeFlags & 0x80))
                  {
                    int2flt.u16[1] = g_mm_node.getResponseBuffer(jj);  // high word
                    int2flt.u16[0] = g_mm_node.getResponseBuffer(jj + 1);  // low word
                  }
                  else
                  {
                    int2flt.u16[0] = g_mm_node.getResponseBuffer(jj);  // low word
                    int2flt.u16[1] = g_mm_node.getResponseBuffer(jj + 1);  // high word
                  }
                  
                  int2flt.f = (float) int2flt.s32;
                  
                  u8a_mbResp[ii] = (int2flt.u32 >> 8);
                  u8a_mbResp[ii + 1] = int2flt.u32;
                  
                  u8a_mbResp[ii + 2] = (int2flt.u32 >> 24);
                  u8a_mbResp[ii + 3] = (int2flt.u32 >> 16); 
                }
                break;
              case 5: // mod1k to float
                for (int jj = 0, ii = 9; jj < u16_adjNumRegs; jj+=2, ii+=4)  // u16_adjNumRegs is # of registers - divide since handling by two
                { 
                  if((u8_dataTypeFlags & 0x80))  // if word swapped
                  {
                    int2flt.u32 = g_mm_node.getResponseBuffer(jj + 1);  // low word
                    u16_tempReg = g_mm_node.getResponseBuffer(jj);
                  }
                  else
                  {
                    int2flt.u32 = g_mm_node.getResponseBuffer(jj);  // low word
                    u16_tempReg = g_mm_node.getResponseBuffer(jj + 1);
                  }

                  int2flt.u32 = int2flt.u32 + ((uint32_t)(u16_tempReg & 0x7fff)) * 1000;  // high word
                  if (u16_tempReg >> 15){ // if negative bit
                    int2flt.s32 *= (-1);
                  }
                    
                  int2flt.f = (float) int2flt.s32;
                  
                  u8a_mbResp[ii] = (int2flt.u32 >> 8);
                  u8a_mbResp[ii + 1] = int2flt.u32;
                  
                  u8a_mbResp[ii + 2] = (int2flt.u32 >> 24);
                  u8a_mbResp[ii + 3] = (int2flt.u32 >> 16); 
                }
                break;
              case 6: // mod10k to float
                for (int jj = 0, ii = 9; jj < u16_adjNumRegs; jj+=2, ii+=4)  // u16_adjNumRegs is # of registers - divide since handling by two
                { 
                  if((u8_dataTypeFlags & 0x80))  // if word swapped
                  {
                    int2flt.u32 = g_mm_node.getResponseBuffer(jj + 1);  // low word
                    u16_tempReg = g_mm_node.getResponseBuffer(jj);     
                  }
                  else
                  {
                    int2flt.u32 = g_mm_node.getResponseBuffer(jj);  // low word
                    u16_tempReg = g_mm_node.getResponseBuffer(jj + 1);
                  }

                  int2flt.u32 = int2flt.u32 + ((uint32_t)(u16_tempReg & 0x7fff)) * 10000;  // high word
                  if (u16_tempReg >> 15){ // if negative bit
                    int2flt.s32 *= (-1);
                  }
                    
                  int2flt.f = (float) int2flt.s32;
                  
                  u8a_mbResp[ii] = (int2flt.u32 >> 8);
                  u8a_mbResp[ii + 1] = int2flt.u32;
                  
                  u8a_mbResp[ii + 2] = (int2flt.u32 >> 24);
                  u8a_mbResp[ii + 3] = (int2flt.u32 >> 16); 
                }
                break;
              case 7: // mod20k to float
                for (int jj = 0, ii = 9; jj < u16_adjNumRegs; jj+=3, ii += 4)  // u16_adjNumRegs is # of registers - divide since handling by two
                { 
                  if((u8_dataTypeFlags & 0x80))  // if word swapped
                  {
                    int2flt.f = (float)g_mm_node.getResponseBuffer(jj + 2);  // low word
                    int2flt.f = int2flt.f + ((float)(g_mm_node.getResponseBuffer(jj + 1))) * pow(10, 4);  // middle word
                    u16_tempReg = g_mm_node.getResponseBuffer(jj);
                  }
                  else
                  {
                    int2flt.f = (float)g_mm_node.getResponseBuffer(jj);  // low word
                    int2flt.f = int2flt.f + ((float)(g_mm_node.getResponseBuffer(jj + 1))) * pow(10, 4);  //middle word
                    u16_tempReg = g_mm_node.getResponseBuffer(jj + 2);
                  }

                  int2flt.f = int2flt.f + ((float)(u16_tempReg & 0x7fff)) * pow(10, 8);  // high word
                  if (u16_tempReg >> 15){ // if negative bit
                    int2flt.f *= (-1);
                  }
                  
                  u8a_mbResp[ii] = (int2flt.u32 >> 8);
                  u8a_mbResp[ii + 1] = int2flt.u32;
                  
                  u8a_mbResp[ii + 2] = (int2flt.u32 >> 24);
                  u8a_mbResp[ii + 3] = (int2flt.u32 >> 16); 
                }
                break;
              case 8: // mod30k to float
                for (int jj = 0; jj < u16_adjNumRegs; jj+=4)  // u16_adjNumRegs is # of registers - divide since handling by two
                { 
                  if((u8_dataTypeFlags & 0x80))  // if word swapped
                  {
                    int2flt.f = (float)g_mm_node.getResponseBuffer(jj + 3);  // low word
                    int2flt.f = int2flt.f + ((float)(g_mm_node.getResponseBuffer(jj + 2))) * pow(10, 4);  // high word
                    int2flt.f = int2flt.f + ((float)(g_mm_node.getResponseBuffer(jj + 1))) * pow(10, 8);  // high word
                    u16_tempReg = g_mm_node.getResponseBuffer(jj);
                  }
                  else
                  {
                    int2flt.f = (float)g_mm_node.getResponseBuffer(jj);  // low word
                    int2flt.f = int2flt.f + ((float)(g_mm_node.getResponseBuffer(jj + 1))) * pow(10, 4);  // high word
                    int2flt.f = int2flt.f + ((float)(g_mm_node.getResponseBuffer(jj + 2))) * pow(10, 8);  // high word
                    u16_tempReg = g_mm_node.getResponseBuffer(jj + 3);
                  }

                  int2flt.f = int2flt.f + ((float)(u16_tempReg & 0x7fff)) * pow(10, 12);  // high word
                  if (u16_tempReg >> 15){ // if negative bit
                    int2flt.f *= (-1);
                  }
                  
                  u8a_mbResp[jj + 9] = (int2flt.u32 >> 8);
                  u8a_mbResp[jj + 10] = int2flt.u32;
                  
                  u8a_mbResp[jj + 11] = (int2flt.u32 >> 24);
                  u8a_mbResp[jj + 12] = (int2flt.u32 >> 16); 
                }
                break;
              case 9: // u64 to float
                for (int jj = 0; jj < u16_adjNumRegs; jj+=4)  // u16_adjNumRegs is # of registers - divide since handling by two
                { 
                  if((u8_dataTypeFlags & 0x80))  // if word swapped
                  {
                    int2flt.f = (float)g_mm_node.getResponseBuffer(jj + 3);  // low word
                    int2flt.f = int2flt.f + ((float)(g_mm_node.getResponseBuffer(jj + 2))) * pow(2, 16);  // high word
                    int2flt.f = int2flt.f + ((float)(g_mm_node.getResponseBuffer(jj + 1))) * pow(2, 32);  // high word
                    u16_tempReg = g_mm_node.getResponseBuffer(jj);
                  }
                  else
                  {
                    int2flt.f = (float)g_mm_node.getResponseBuffer(jj);  // low word
                    int2flt.f = int2flt.f + ((float)(g_mm_node.getResponseBuffer(jj + 1))) * pow(2, 16);  // high word
                    int2flt.f = int2flt.f + ((float)(g_mm_node.getResponseBuffer(jj + 2))) * pow(2, 32);  // high word
                    u16_tempReg = g_mm_node.getResponseBuffer(jj + 3);
                  }

                  int2flt.f = int2flt.f + ((float)(u16_tempReg & 0x7fff)) * pow(2, 48);  // high word
                  if (u16_tempReg >> 15){ // if negative bit
                    int2flt.f *= (-1);
                  }
                  
                  u8a_mbResp[jj + 9] = (int2flt.u32 >> 8);
                  u8a_mbResp[jj + 10] = int2flt.u32;
                  
                  u8a_mbResp[jj + 11] = (int2flt.u32 >> 24);
                  u8a_mbResp[jj + 12] = (int2flt.u32 >> 16); 
                }
                break;
              case 10:  // energy units  (eaton designed unit)
                for (int jj = 0; jj < u16_adjNumRegs; jj+=4)  // u16_adjNumRegs is # of registers - divide since handling by two
                { 
                  if((u8_dataTypeFlags & 0x80))  // if word swapped
                  {
                    int2flt.f = (float)g_mm_node.getResponseBuffer(jj + 3);  // low word
                    int2flt.f = int2flt.f + ((float)(g_mm_node.getResponseBuffer(jj + 2))) * pow(2, 16);  // high word
                    int2flt.f = int2flt.f + ((float)(g_mm_node.getResponseBuffer(jj + 1))) * pow(2, 32);  // high word
                    u16_tempReg = g_mm_node.getResponseBuffer(jj);
                  }
                  else
                  {
                    int2flt.f = (float)g_mm_node.getResponseBuffer(jj);  // low word
                    int2flt.f = int2flt.f + ((float)(g_mm_node.getResponseBuffer(jj + 1))) * pow(2, 16);  // high word
                    int2flt.f = int2flt.f + ((float)(g_mm_node.getResponseBuffer(jj + 2))) * pow(2, 32);  // high word
                    u16_tempReg = g_mm_node.getResponseBuffer(jj + 3);
                  }

                  int2flt.f *= pow(10, (int8_t)(u16_tempReg >> 8));
                  
                  u8a_mbResp[jj + 9] = (int2flt.u32 >> 8);
                  u8a_mbResp[jj + 10] = int2flt.u32;
                  
                  u8a_mbResp[jj + 11] = (int2flt.u32 >> 24);
                  u8a_mbResp[jj + 12] = (int2flt.u32 >> 16); 
                }
                break;
              case 11:  // double to float
#if defined(CORE_TEENSY)  // if teensy3.0 or greater
                for (int jj = 0; jj < u16_adjNumRegs; jj += 4)  // u16_adjNumRegs is # of registers - divide since handling by two
                {
                  if ((u8_dataTypeFlags & 0x80)) {  // WORDSWAP
                    int2flt.u16[3] = g_mm_node.getResponseBuffer(jj);
                    int2flt.u16[2] = g_mm_node.getResponseBuffer(jj + 1);
                    int2flt.u16[1] = g_mm_node.getResponseBuffer(jj + 2);
                    int2flt.u16[0] = g_mm_node.getResponseBuffer(jj + 3);
                  }
                  else {
                    int2flt.u16[0] = g_mm_node.getResponseBuffer(jj);
                    int2flt.u16[1] = g_mm_node.getResponseBuffer(jj + 1);
                    int2flt.u16[2] = g_mm_node.getResponseBuffer(jj + 2);
                    int2flt.u16[3] = g_mm_node.getResponseBuffer(jj + 3);
                  }

                  int2flt.f = (float)int2flt.dbl;

                  u8a_mbResp[jj + 9] = (int2flt.u32 >> 8);
                  u8a_mbResp[jj + 10] = int2flt.u32;

                  u8a_mbResp[jj + 11] = (int2flt.u32 >> 24);
                  u8a_mbResp[jj + 12] = (int2flt.u32 >> 16);
                }
#else
                for (int jj = 0; jj < u16_adjNumRegs; jj += 4)  // u16_adjNumRegs is # of registers - divide since handling by two
                {
                  if ((u8_dataTypeFlags & 0x80)) {  // WORDSWAP
                    dblC.u16[3] = g_mm_node.getResponseBuffer(jj);
                    dblC.u16[2] = g_mm_node.getResponseBuffer(jj + 1);
                    dblC.u16[1] = g_mm_node.getResponseBuffer(jj + 2);
                    dblC.u16[0] = g_mm_node.getResponseBuffer(jj + 3);
                  }
                  else {
                    dblC.u16[0] = g_mm_node.getResponseBuffer(jj);
                    dblC.u16[1] = g_mm_node.getResponseBuffer(jj + 1);
                    dblC.u16[2] = g_mm_node.getResponseBuffer(jj + 2);
                    dblC.u16[3] = g_mm_node.getResponseBuffer(jj + 3);
                  }

                  int e = dblC.sD.e - 1023 + 127;
                  if (e >= 0 && e <= 255) {
                    int2flt.sF.s = dblC.sD.s;
                    int2flt.sF.e = e;
                    int2flt.sF.m = dblC.sD.m;
                  }
                  else int2flt.f = NAN;

                  u8a_mbResp[jj + 9] = (int2flt.u32 >> 8);
                  u8a_mbResp[jj + 10] = int2flt.u32;

                  u8a_mbResp[jj + 11] = (int2flt.u32 >> 24);
                  u8a_mbResp[jj + 12] = (int2flt.u32 >> 16);
                }
                break;
              default: // pass on data with no adjustments
                for (int jj = 0, ii = 9; jj < u16_adjNumRegs; ++jj, ii+=2){
                  u16_tempReg = g_mm_node.getResponseBuffer(jj);
                  u8a_mbResp[ii] = (u16_tempReg >> 8);
                  u8a_mbResp[ii + 1] = u16_tempReg;
                }
#endif
                break;
              
            } // end special switch
          }
          
    
        case 1:  // g_mm_node.ku8MBIllegalFunction
        case 2:  // g_mm_node.ku8MBIllegalDataAddress
        case 3:  // g_mm_node.ku8MBIllegalDataValue
        case 4:  // g_mm_node.ku8MBSlaveDeviceFailure
        default:
  //        Serial.println("error returned");
          if (u8_mbResult > 0){
            u8_mbRespNumBytes = 0; // length of modbus message for MB/TCP
            u8a_mbResp[7] = (u8_mbReqFunc | 0x80); // return function  + 128
            u8a_mbResp[8] = u8_mbResult; // modbus error function
          }
          else{
            u8a_mbResp[7] = u8_mbReqFunc; // return function since no error occured
            u8a_mbResp[8] = u8_mbRespNumBytes; // expected modbus length
            b_mbStatus = true;
          }
          
          u8a_mbResp[4] = ((u8_mbRespNumBytes + 3) >> 8);  // expected tcp length
          u8a_mbResp[5] = u8_mbRespNumBytes + 3;  // expected tcp length
  
          u16_mbRespLen = (u8_mbRespNumBytes + 9);
          
          return b_mbStatus;        
          break;
      }  // end switch (u8_mbResult)
    }  // end else if no error    
  }  // end if (u16_mbReqLen == 12
  
  return false;
}  // end getModbus()


void handle_modbus(bool b_idleHttp) {
  uint8_t u8a_mbReq[gk_u16_mbArraySize] = {0};
  uint8_t u8a_mbResp[gk_u16_mbArraySize];
  uint16_t u16_lenRead;
  uint16_t u16_givenLen;
  uint16_t u16_mbRespLen = 0;
  uint32_t u32_mbReqStart; // , u32MB_Cl_To_cur;
  uint32_t u32_mbTcpConnStart;
  const uint32_t k_u32_mbTcpTimeout(3000);              // timeout for device to hold on to tcp connection after modbus request
  const uint32_t k_u32_mbReqTimeout(10);
  EthernetClient52 ec_client = g_es_mbServ.available();
  
  if (ec_client)
  {
    u32_mbTcpConnStart = millis();

    while (ec_client.connected() && ((millis() - u32_mbTcpConnStart) < k_u32_mbTcpTimeout)) {  // check to see if client is connected and hasn't to'd
      //      ec_client.getRemoteIP(rip); // get client IP
      if (ec_client.available()) {
        u16_lenRead = ec_client.read(u8a_mbReq, gk_u16_mbArraySize);
        u32_mbReqStart = millis();

        u16_givenLen = word(u8a_mbReq[4], u8a_mbReq[5]) + 6;

        if ((u16_lenRead > u16_givenLen) || (u16_givenLen > gk_u16_mbArraySize)) {
          ec_client.stop();  // grabbed too much, just exit without worrying.  It  shouldn't happen, any other connection will have dift sockets
          return;         //     or incoming packet larger than array (this should not happen, modbus/tcp deals with pretty small stuff overall
        }

        while (u16_lenRead < u16_givenLen) {  // make sure to grab the full packet
          u16_lenRead += ec_client.read(u8a_mbReq + u16_lenRead, gk_u16_mbArraySize - u16_lenRead);

          if ((millis() - u32_mbReqStart) > k_u32_mbReqTimeout) {  // 10 ms might be too quick, but not really sure
            ec_client.stop();  // could not get full message, exit
            return;
          }
        }
        
        getModbus(u8a_mbReq, u16_lenRead, u8a_mbResp, u16_mbRespLen);

        if (u16_mbRespLen > 0) {
          ec_client.write(u8a_mbResp, u16_mbRespLen);
          ec_client.flush();
          //break;
          u32_mbTcpConnStart = millis();
        }
      }  // end if client available
      else if (b_idleHttp) {
        //handle_data(false);
      }
    }  // end while (client.connected())
    ec_client.stop();
  }  // end if (client)
  
}
