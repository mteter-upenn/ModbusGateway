bool getModbus(uint8_t *in_mb_f, uint16_t msg_lgth, uint8_t *out_mb_f, uint16_t &out_len)
{
  uint16_t strt_reg, lgth_reg, adj_strt_reg, adj_lgth_reg;
  uint16_t i, j;
  uint8_t exp_lgth = 0;
  uint8_t result = 0xFF;
  uint16_t rgstr1 = 0;
  uint8_t reg_flags = 0;
  bool fnd_reg = false;
  uint8_t func_reg = 0;
  uint8_t mberror = 0;
  bool wc_req = false;
  bool mb_stat = false;
  uint8_t adj_reg_flags = 0;
  uint8_t mtr_typ = 0;
  uint8_t act_dev;
  
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

  union convertUnion
  {
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

  if (msg_lgth == 12);  // typical modbus/tcp message is 12 bytes long
  {
    out_mb_f[0] = in_mb_f[0]; // copy first 2 bytes to outbound message
    out_mb_f[1] = in_mb_f[1];
    out_mb_f[2] = 0;
    out_mb_f[3] = 0;
    
    out_mb_f[6] = in_mb_f[6]; // set device
    
    //node.setSlave(in_mb_f[6]);
    act_dev = in_mb_f[6];  // let id given by incoming packet be default, it will change if it matches with something on the list in isMeterEth
    // search for slave id in eeprom, if there check if ip exists
    if (isMeterEth(in_mb_f[6], mtr_typ, act_dev)){  // isMeterEth set clientIP
      node.setSerialEthernet(false);  // false means ethernet
      node.setClientIP(clientIP);
      //Serial.println(F("ETHER!"));
    }
    else{
      node.setSerialEthernet(true);  // true means serial
    }
    
    node.setSlave(act_dev);
    
    
//        node.setTransmitBuffer(0, in_mb[0]);  // necessary?
//        node.setTransmitBuffer(1, in_mb[1]);
    strt_reg = (in_mb_f[8] << 8) | (in_mb_f[9]);
    lgth_reg = (in_mb_f[10] << 8) | (in_mb_f[11]);
    exp_lgth = lgth_reg * 2;
    func_reg = in_mb_f[7];
    
      
    if (strt_reg < 10000) {
      adj_strt_reg = strt_reg;
      adj_lgth_reg = lgth_reg;
    }
    else if ((strt_reg > 9999) && (strt_reg < 20000)) {
      wc_req = true;  // 10k request
      adj_strt_reg = strt_reg - 10000;
      fnd_reg = findRegister(adj_strt_reg, reg_flags, mtr_typ);
      
  //    Serial.println(adj_strt_reg);
      if (0x01 & lgth_reg) // if odd number, then can't return float (must be even # of regs)
      {
//        Serial.println("odd");
        mberror = 0x02;
      }
      else
      {
        if (fnd_reg)  // found registers
        {
          adj_reg_flags = (0x7f & reg_flags); // ignore ws bit for comparisons
          
          // adj_lgth_reg is the length request sent to modbus device
          // all requests made through 10k registers must be with expectation of float
          // this means the unadjusted length request has 2 registers for every unique data point
          switch (adj_reg_flags){
            case 1:  // u16
            case 2:  // s16
              adj_lgth_reg = lgth_reg / 2;  // single register values
              break;
            case 7:  // m20k
              adj_lgth_reg = lgth_reg * 3 / 2;  // 3 register values
              break;
            case 8:  // m30k
            case 9:  // u64
            case 10:  // engy
            case 11:  // dbl
              adj_lgth_reg = lgth_reg * 2;  // 4 register values
              break;
            default:  // float, u32, s32, m10k, m1k
              adj_lgth_reg = lgth_reg;  // 2 register values
              break;              
          }
        }
        else
        {
//          Serial.println("no flags");
          mberror = 0x02;
        }  // end if else check if address in block
      }  // end if else check lgth  
    }  // end if 10k request
    else
    {
//      Serial.println("reg outside exp");
      mberror = 0x02;
    }
    
    if ((!(func_reg == 3) || (func_reg == 4)))
    {
      mberror = 0x01;
    }
    
    if (mberror) {  // error occurred prior to actual modbus request, return error
//      Serial.println("error in gateway"); 
//      Serial.println(mberror, HEX);
      out_mb_f[7] = (func_reg | 0x80); // return function  + 128
      out_mb_f[8] = mberror; // modbus error function
      out_mb_f[4] = 0;  // expected tcp length
      out_mb_f[5] = 3;  // expected tcp length

      out_len = 9;
      /*Serial.print("error: ");
      Serial.println(mberror, DEC);*/
     
      return false;
    }  // end if error
    else {  // no error yet, handle code
      //if (!wc_req) {
        switch (func_reg) {
//          case 1:
//            node.readCoils(adj_strt_reg, adj_lgth_reg);
//            break;
//          case 2:
//            node.readDiscreteInputs(adj_strt_reg, adj_lgth_reg);
//            break;
          case 3:
//            Serial.println(F("modbus request"));
            result = node.readHoldingRegisters(adj_strt_reg, adj_lgth_reg);
//            Serial.print(F("sent request: "));
//            Serial.println(result, HEX);
            break;
          case 4:
            result = node.readInputRegisters(adj_strt_reg, adj_lgth_reg);
            break;
//          case 5:
//            result = node.writeSingleCoil(adj_strt_reg, adj_lgth_reg);
//            break;
//          case 6:
//            result = node.writeSingleRegister(adj_strt_reg, adj_lgth_reg);
//            break;
          default:
            result = 0x01;
            break;
        }  // end switch function
//      }  // end if typical register request
//      else {
//        /*Serial.println("requesting...");
//        Serial.println(adj_strt_reg, DEC);
//        Serial.println(adj_lgth_reg, DEC);*/
//        switch (func_reg) {
//          case 3:
//            //Serial.println(F("modbus 10k request"));
//            result = node.readHoldingRegisters(adj_strt_reg, adj_lgth_reg);
////            Serial.print(F("sent request: "));
////            Serial.println(result, HEX);
//            break;
//          case 4:
//            result = node.readInputRegisters(adj_strt_reg, adj_lgth_reg);
//            break;
//          default:
//            result = 0x01;
//            break;
//        }  // end switch function
////        Serial.print("result: ");
////        Serial.println(result, HEX);
//      }// end else if 10k request
      
      switch (result) {
        case 0:  // node.ku8MBSuccess
          if (!wc_req) {  // no adjustments to data
            for (j = 0, i = 9; j < adj_lgth_reg; j++, i+=2) {
              rgstr1 = node.getResponseBuffer(j);
              
              out_mb_f[i] = highByte(rgstr1);
              out_mb_f[i + 1] = lowByte(rgstr1);
            }
          }
          else {
//            Serial.println("handling 10k data");
//            Serial.println((reg_flags & 0x7F), DEC);
          
            switch (adj_reg_flags) {  // ((reg_flags & 0x7F))
              case 0:  // float
                if (reg_flags & 0x80){  // if ws
                  for (j = 0, i = 9; j < adj_lgth_reg; j+=2, i+=4){  // adj_lgth_reg is # of registers - divide since handling by two
                    rgstr1 = node.getResponseBuffer(j);
                    out_mb_f[i + 2] = (rgstr1 >> 8);  // would memcpy work better here?
                    out_mb_f[i + 3] = rgstr1;

                    rgstr1 = node.getResponseBuffer(j + 1);
                    out_mb_f[i] = (rgstr1 >> 8);
                    out_mb_f[i + 1] = rgstr1;
                  }
                }
                else{  // no ws, no adjustments needed
                  for (j = 0, i = 9; j < adj_lgth_reg; j++, i+=2){
                    rgstr1 = node.getResponseBuffer(j);
                    out_mb_f[i] = (rgstr1 >> 8);
                    out_mb_f[i + 1] = rgstr1;
                  }
                }
                break;
              case 1: // u16 to float
                for (j = 0, i = 9; j < adj_lgth_reg; j++, i+=4){  // adj_lgth_reg is # of registers - divide since handling by two
                  rgstr1 = node.getResponseBuffer(j);
      
                  int2flt.f = (float)rgstr1;
                  
                  out_mb_f[i] = (int2flt.u32 >> 8);
                  out_mb_f[i + 1] = int2flt.u32;
                  
                  out_mb_f[i + 2] = (int2flt.u32 >> 24);
                  out_mb_f[i + 3] = (int2flt.u32 >> 16); 
                }
                break;
              case 2: // s16 to float
                for (j = 0, i = 9; j < adj_lgth_reg; j++, i+=4){  // adj_lgth_reg is # of registers - divide since handling by two
                  int2flt.s16 = node.getResponseBuffer(j);  // may require (int) cast
                  
                  int2flt.f = (float) int2flt.s16;
                  
                  out_mb_f[i] = (int2flt.u32 >> 8);
                  out_mb_f[i + 1] = int2flt.u32;
                  
                  out_mb_f[i + 2] = (int2flt.u32 >> 24);
                  out_mb_f[i + 3] = (int2flt.u32 >> 16); 
                }
                break;
              case 3: // u32 to float
                for (j = 0, i = 9; j < adj_lgth_reg; j+=2, i+=4){  // adj_lgth_reg is # of registers - divide since handling by two
                  if((reg_flags & 0x80)){  // WORDSWAP
                    int2flt.u16[1] = node.getResponseBuffer(j);
                    int2flt.u16[0] = node.getResponseBuffer(j + 1);
                  }
                  else{
                    int2flt.u16[0] = node.getResponseBuffer(j);
                    int2flt.u16[1] = node.getResponseBuffer(j + 1);
                  }
                  
                  int2flt.f = (float) int2flt.u32;
                  
                  out_mb_f[i] = (int2flt.u32 >> 8);
                  out_mb_f[i + 1] = int2flt.u32;
                  
                  out_mb_f[i + 2] = (int2flt.u32 >> 24);
                  out_mb_f[i + 3] = (int2flt.u32 >> 16); 
                }
                break;
              case 4: // s32 to float
                for (j = 0, i = 9; j < adj_lgth_reg; j+=2, i+=4)  // adj_lgth_reg is # of registers - divide since handling by two
                { 
                  if((reg_flags & 0x80))
                  {
                    int2flt.u16[1] = node.getResponseBuffer(j);  // high word
                    int2flt.u16[0] = node.getResponseBuffer(j + 1);  // low word
                  }
                  else
                  {
                    int2flt.u16[0] = node.getResponseBuffer(j);  // low word
                    int2flt.u16[1] = node.getResponseBuffer(j + 1);  // high word
                  }
                  
                  int2flt.f = (float) int2flt.s32;
                  
                  out_mb_f[i] = (int2flt.u32 >> 8);
                  out_mb_f[i + 1] = int2flt.u32;
                  
                  out_mb_f[i + 2] = (int2flt.u32 >> 24);
                  out_mb_f[i + 3] = (int2flt.u32 >> 16); 
                }
                break;
              case 5: // mod1k to float
                for (j = 0, i = 9; j < adj_lgth_reg; j+=2, i+=4)  // adj_lgth_reg is # of registers - divide since handling by two
                { 
                  if((reg_flags & 0x80))  // if word swapped
                  {
                    int2flt.u32 = node.getResponseBuffer(j + 1);  // low word
                    rgstr1 = node.getResponseBuffer(j);
                  }
                  else
                  {
                    int2flt.u32 = node.getResponseBuffer(j);  // low word
                    rgstr1 = node.getResponseBuffer(j + 1);
                  }

                  int2flt.u32 = int2flt.u32 + ((uint32_t)(rgstr1 & 0x7f)) * 1000;  // high word
                  if (rgstr1 >> 15){ // if negative bit
                    int2flt.s32 *= (-1);
                  }
                    
                  int2flt.f = (float) int2flt.s32;
                  
                  out_mb_f[i] = (int2flt.u32 >> 8);
                  out_mb_f[i + 1] = int2flt.u32;
                  
                  out_mb_f[i + 2] = (int2flt.u32 >> 24);
                  out_mb_f[i + 3] = (int2flt.u32 >> 16); 
                }
                break;
              case 6: // mod10k to float
                for (j = 0, i = 9; j < adj_lgth_reg; j+=2, i+=4)  // adj_lgth_reg is # of registers - divide since handling by two
                { 
                  if((reg_flags & 0x80))  // if word swapped
                  {
                    int2flt.u32 = node.getResponseBuffer(j + 1);  // low word
                    rgstr1 = node.getResponseBuffer(j);     
                  }
                  else
                  {
                    int2flt.u32 = node.getResponseBuffer(j);  // low word
                    rgstr1 = node.getResponseBuffer(j + 1);
                  }

                  int2flt.u32 = int2flt.u32 + ((uint32_t)(rgstr1 & 0x7f)) * 10000;  // high word
                  if (rgstr1 >> 15){ // if negative bit
                    int2flt.s32 *= (-1);
                  }
                    
                  int2flt.f = (float) int2flt.s32;
                  
                  out_mb_f[i] = (int2flt.u32 >> 8);
                  out_mb_f[i + 1] = int2flt.u32;
                  
                  out_mb_f[i + 2] = (int2flt.u32 >> 24);
                  out_mb_f[i + 3] = (int2flt.u32 >> 16); 
                }
                break;
              case 7: // mod20k to float
                for (j = 0, i = 9; j < adj_lgth_reg; j+=3, i += 4)  // adj_lgth_reg is # of registers - divide since handling by two
                { 
                  if((reg_flags & 0x80))  // if word swapped
                  {
                    int2flt.f = (float)node.getResponseBuffer(j + 2);  // low word
                    int2flt.f = int2flt.f + ((float)(node.getResponseBuffer(j + 1))) * pow(10, 4);  // middle word
                    rgstr1 = node.getResponseBuffer(j);
                  }
                  else
                  {
                    int2flt.f = (float)node.getResponseBuffer(j);  // low word
                    int2flt.f = int2flt.f + ((float)(node.getResponseBuffer(j + 1))) * pow(10, 4);  //middle word
                    rgstr1 = node.getResponseBuffer(j + 2);
                  }

                  int2flt.f = int2flt.f + ((float)(rgstr1 & 0x7f)) * pow(10, 8);  // high word
                  if (rgstr1 >> 15){ // if negative bit
                    int2flt.f *= (-1);
                  }
                  
                  out_mb_f[i] = (int2flt.u32 >> 8);
                  out_mb_f[i + 1] = int2flt.u32;
                  
                  out_mb_f[i + 2] = (int2flt.u32 >> 24);
                  out_mb_f[i + 3] = (int2flt.u32 >> 16); 
                }
                break;
              case 8: // mod30k to float
                for (j = 0; j < adj_lgth_reg; j+=4)  // adj_lgth_reg is # of registers - divide since handling by two
                { 
                  if((reg_flags & 0x80))  // if word swapped
                  {
                    int2flt.f = (float)node.getResponseBuffer(j + 3);  // low word
                    int2flt.f = int2flt.f + ((float)(node.getResponseBuffer(j + 2))) * pow(10, 4);  // high word
                    int2flt.f = int2flt.f + ((float)(node.getResponseBuffer(j + 1))) * pow(10, 8);  // high word
                    rgstr1 = node.getResponseBuffer(j);
                  }
                  else
                  {
                    int2flt.f = (float)node.getResponseBuffer(j);  // low word
                    int2flt.f = int2flt.f + ((float)(node.getResponseBuffer(j + 1))) * pow(10, 4);  // high word
                    int2flt.f = int2flt.f + ((float)(node.getResponseBuffer(j + 2))) * pow(10, 8);  // high word
                    rgstr1 = node.getResponseBuffer(j + 3);
                  }

                  int2flt.f = int2flt.f + ((float)(rgstr1 & 0x7f)) * pow(10, 12);  // high word
                  if (rgstr1 >> 15){ // if negative bit
                    int2flt.f *= (-1);
                  }
                  
                  out_mb_f[j + 9] = (int2flt.u32 >> 8);
                  out_mb_f[j + 10] = int2flt.u32;
                  
                  out_mb_f[j + 11] = (int2flt.u32 >> 24);
                  out_mb_f[j + 12] = (int2flt.u32 >> 16); 
                }
                break;
              case 9: // u64 to float
                for (j = 0; j < adj_lgth_reg; j+=4)  // adj_lgth_reg is # of registers - divide since handling by two
                { 
                  if((reg_flags & 0x80))  // if word swapped
                  {
                    int2flt.f = (float)node.getResponseBuffer(j + 3);  // low word
                    int2flt.f = int2flt.f + ((float)(node.getResponseBuffer(j + 2))) * pow(2, 16);  // high word
                    int2flt.f = int2flt.f + ((float)(node.getResponseBuffer(j + 1))) * pow(2, 32);  // high word
                    rgstr1 = node.getResponseBuffer(j);
                  }
                  else
                  {
                    int2flt.f = (float)node.getResponseBuffer(j);  // low word
                    int2flt.f = int2flt.f + ((float)(node.getResponseBuffer(j + 1))) * pow(2, 16);  // high word
                    int2flt.f = int2flt.f + ((float)(node.getResponseBuffer(j + 2))) * pow(2, 32);  // high word
                    rgstr1 = node.getResponseBuffer(j + 3);
                  }

                  int2flt.f = int2flt.f + ((float)(rgstr1 & 0x7f)) * pow(2, 48);  // high word
                  if (rgstr1 >> 15){ // if negative bit
                    int2flt.f *= (-1);
                  }
                  
                  out_mb_f[j + 9] = (int2flt.u32 >> 8);
                  out_mb_f[j + 10] = int2flt.u32;
                  
                  out_mb_f[j + 11] = (int2flt.u32 >> 24);
                  out_mb_f[j + 12] = (int2flt.u32 >> 16); 
                }
                break;
              case 10:  // energy units  (eaton designed unit)
                for (j = 0; j < adj_lgth_reg; j+=4)  // adj_lgth_reg is # of registers - divide since handling by two
                { 
                  if((reg_flags & 0x80))  // if word swapped
                  {
                    int2flt.f = (float)node.getResponseBuffer(j + 3);  // low word
                    int2flt.f = int2flt.f + ((float)(node.getResponseBuffer(j + 2))) * pow(2, 16);  // high word
                    int2flt.f = int2flt.f + ((float)(node.getResponseBuffer(j + 1))) * pow(2, 32);  // high word
                    rgstr1 = node.getResponseBuffer(j);
                  }
                  else
                  {
                    int2flt.f = (float)node.getResponseBuffer(j);  // low word
                    int2flt.f = int2flt.f + ((float)(node.getResponseBuffer(j + 1))) * pow(2, 16);  // high word
                    int2flt.f = int2flt.f + ((float)(node.getResponseBuffer(j + 2))) * pow(2, 32);  // high word
                    rgstr1 = node.getResponseBuffer(j + 3);
                  }

                  int2flt.f *= pow(10, (int8_t)(rgstr1 >> 8));
                  
                  out_mb_f[j + 9] = (int2flt.u32 >> 8);
                  out_mb_f[j + 10] = int2flt.u32;
                  
                  out_mb_f[j + 11] = (int2flt.u32 >> 24);
                  out_mb_f[j + 12] = (int2flt.u32 >> 16); 
                }
                break;
              case 11:  // energy units  (eaton designed unit)
#if defined(CORE_TEENSY)  // if teensy3.0 or greater
                for (j = 0; j < adj_lgth_reg; j += 4)  // adj_lgth_reg is # of registers - divide since handling by two
                {
                  if ((reg_flags & 0x80)) {  // WORDSWAP
                    int2flt.u16[3] = node.getResponseBuffer(j);
                    int2flt.u16[2] = node.getResponseBuffer(j + 1);
                    int2flt.u16[1] = node.getResponseBuffer(j + 2);
                    int2flt.u16[0] = node.getResponseBuffer(j + 3);
                  }
                  else {
                    int2flt.u16[0] = node.getResponseBuffer(j);
                    int2flt.u16[1] = node.getResponseBuffer(j + 1);
                    int2flt.u16[2] = node.getResponseBuffer(j + 2);
                    int2flt.u16[3] = node.getResponseBuffer(j + 3);
                  }

                  int2flt.f = (float)int2flt.dbl;

                  out_mb_f[j + 9] = (int2flt.u32 >> 8);
                  out_mb_f[j + 10] = int2flt.u32;

                  out_mb_f[j + 11] = (int2flt.u32 >> 24);
                  out_mb_f[j + 12] = (int2flt.u32 >> 16);
                }
#else
                for (j = 0; j < adj_lgth_reg; j += 4)  // adj_lgth_reg is # of registers - divide since handling by two
                {
                  if ((reg_flags & 0x80)) {  // WORDSWAP
                    dblC.u16[3] = node.getResponseBuffer(j);
                    dblC.u16[2] = node.getResponseBuffer(j + 1);
                    dblC.u16[1] = node.getResponseBuffer(j + 2);
                    dblC.u16[0] = node.getResponseBuffer(j + 3);
                  }
                  else {
                    dblC.u16[0] = node.getResponseBuffer(j);
                    dblC.u16[1] = node.getResponseBuffer(j + 1);
                    dblC.u16[2] = node.getResponseBuffer(j + 2);
                    dblC.u16[3] = node.getResponseBuffer(j + 3);
                  }

                  int e = dblC.sD.e - 1023 + 127;
                  if (e >= 0 && e <= 255) {
                    int2flt.sF.s = dblC.sD.s;
                    int2flt.sF.e = e;
                    int2flt.sF.m = dblC.sD.m;
                  }
                  else int2flt.f = NAN;

                  out_mb_f[j + 9] = (int2flt.u32 >> 8);
                  out_mb_f[j + 10] = int2flt.u32;

                  out_mb_f[j + 11] = (int2flt.u32 >> 24);
                  out_mb_f[j + 12] = (int2flt.u32 >> 16);
                }
                break;
              default: // pass on data with no adjustments
                for (j = 0, i = 9; j < adj_lgth_reg; j++, i+=2){
                  rgstr1 = node.getResponseBuffer(j);
                  out_mb_f[i] = (rgstr1 >> 8);
                  out_mb_f[i + 1] = rgstr1;
                }
#endif
                break;
              
            } // end special switch
          }
          
    
        case 1:  // node.ku8MBIllegalFunction
        case 2:  // node.ku8MBIllegalDataAddress
        case 3:  // node.ku8MBIllegalDataValue
        case 4:  // node.ku8MBSlaveDeviceFailure
        default:
  //        Serial.println("error returned");
          if (result > 0){
            exp_lgth = 0; // length of modbus message for MB/TCP
            out_mb_f[7] = (func_reg | 0x80); // return function  + 128
            out_mb_f[8] = result; // modbus error function
          }
          else{
            out_mb_f[7] = func_reg; // return function since no error occured
            out_mb_f[8] = exp_lgth; // expected modbus length
            mb_stat = true;
          }
          
          out_mb_f[4] = ((exp_lgth + 3) >> 8);  // expected tcp length
          out_mb_f[5] = exp_lgth + 3;  // expected tcp length
  
          out_len = (exp_lgth + 9);
          
          return mb_stat;        
          break;
      }  // end switch (result)
    }  // end else if no error    
  }  // end if (msg_lgth == 12
  
  return false;
}  // end getModbus()


void handle_modbus() {
  uint8_t in_mb[MB_ARR_SIZE] = {0};
  uint8_t out_mb[MB_ARR_SIZE];
  //uint16_t i = 0;
  uint16_t initLen;
  uint16_t givenLen;
  uint16_t out_len = 0;
  uint32_t u32MB_Cl_To_old; // , u32MB_Cl_To_cur;

  EthernetClient client = serv_mb.available();
  
  if (client)
  {
    while (client.connected())
    {
      //      client.getRemoteIP(rip); // get client IP
      if (client.available())
      {
        //uint8_t c = client.read();
        //Serial.println(c, DEC);
        //in_mb[i] = c;
        
        initLen = client.read(in_mb, MB_ARR_SIZE);
        u32MB_Cl_To_old = millis();

        givenLen = word(in_mb[4], in_mb[5]) + 6;

        if ((initLen > givenLen) || (givenLen > MB_ARR_SIZE)) {
          client.stop();  // grabbed too much, just exit without worrying.  It  shouldn't happen, any other connection will have dift sockets
          return;         //     or incoming packet larger than array (this should not happen, modbus/tcp deals with pretty small stuff overall
        }

        while (initLen < givenLen) {  // make sure to grab the full packet
          initLen += client.read(in_mb + initLen, MB_ARR_SIZE - initLen);

          if ((millis() - u32MB_Cl_To_old) > 10) {
            client.stop();  // could not get full message, exit
            return;
          }
        }
        
        getModbus(in_mb, initLen, out_mb, out_len);

        if (out_len > 0) {
          client.write(out_mb, out_len);
          client.flush();
          break;
        }
        //i++;
      }  // end while

      //if (i > 6) {
      //  if (i == in_mb[5] + 6) {
      //    //Serial.print(F("time: "));
      //    //Serial.println(millis() - u32MB_Cl_To_old, DEC);

      //    getModbus(in_mb, i, out_mb, out_len);

      //    if (out_len > 0) {
      //      //uint16_t j;
      //      //Serial.println(F("out: "));
      //      //Serial.println(out_len, DEC);
      //      //for (j = 0; j < out_len; j++) {
      //      //  Serial.println(out_mb[j]);
      //      //}

      //      client.write(out_mb, out_len);
      //      client.flush();
      //      break;
      //    }
      //  }

      //  u32MB_Cl_To_cur = millis();

      //  if (u32MB_Cl_To_cur - u32MB_Cl_To_old > 10) {
      //    break;
      //  }
      //}
      //getModbus(in_mb, i, out_mb, out_len);    

      //if (out_len > 0){
      //  uint16_t j;
      //  Serial.println(F("out: "));
      //  //Serial.println(out_len, DEC);
      //  for (j = 0; j < out_len; j++) {
      //    Serial.println(out_mb[j]);
      //  }
      // 
      //  client.write(out_mb, out_len);
      //  client.flush();
      //}
    }  // end if (client.connected())
  client.stop();
  }  // end if (client)
  
}
