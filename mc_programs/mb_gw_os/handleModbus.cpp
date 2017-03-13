#include "handleModbus.h"
#include "globals.h"
#include <ModbusStructs.h>
#include "secondaryModbus.h"
#include <MeterLibrary.h>
#include <Ethernet52.h>

uint8_t getModbus(uint8_t u8a_mbReq[MB_ARRAY_SIZE], uint16_t u16_mbReqLen, uint8_t u8a_mbResp[MB_ARRAY_SIZE], 
  uint16_t &u16_mbRespLen, bool b_byteSwap) {

  uint16_t u16_reqReg, u16_numRegs, u16_adjReqReg(0), u16_adjNumRegs(0);
  uint8_t u8_mbRespNumBytes(0);
  uint8_t u8_mbResult(0x0A);
  bool b_foundReg(false);
  uint8_t u8_mbReqFunc(0);
  uint8_t u8_mbError(0);
  bool b_reqRegManip(false);
  FloatConv fltConvFlg(FloatConv::FLOAT);
  uint8_t u8_mtrType(0);
  //uint8_t u8_mtrId;
  

  return 0x0a;

  if (u16_mbReqLen == 12) { // typical modbus/tcp message is 12 bytes long
    u8a_mbResp[0] = u8a_mbReq[0]; // copy first 2 bytes to outbound message
    u8a_mbResp[1] = u8a_mbReq[1];
    u8a_mbResp[2] = 0;  // reserved, does nothing
    u8a_mbResp[3] = 0;  // reserved, does nothing
    
    u8a_mbResp[6] = u8a_mbReq[6]; // set device
    
    //uint8_t u8_mtrVid = u8a_mbReq[6];
    //uint8_t u8a_clientIp[4];
    //u8_mtrId = u8a_mbReq[6];  // let id given by incoming packet be default, it will change if it matches with something on the list in isMeterEth
    // search for slave id in eeprom, if there check if ip exists
    // isMeterEth changes u8_mtrType and u8_mtrId
    //if (isMeterEth(u8a_clientIp, u8_mtrVid, u8_mtrType, u8_mtrId)){  // isMeterEth set g_u8a_clientIP
    //  //g_mm_node.setSerialEthernet(false);  // <--false means ethernet
    //  //g_mm_node.setClientIP(u8a_clientIp);
    //}
    //else{
    //  //g_mm_node.setSerialEthernet(true);  // true means serial
    //}
    
    //g_mm_node.setSlave(u8_mtrId);
    
    
    u16_reqReg = (u8a_mbReq[8] << 8) | (u8a_mbReq[9]);
    u16_numRegs = (u8a_mbReq[10] << 8) | (u8a_mbReq[11]);
    u8_mbRespNumBytes = u16_numRegs * 2;
    u8_mbReqFunc = u8a_mbReq[7];
    
      
    if (u16_reqReg < 10000) {
      u16_adjReqReg = u16_reqReg;
      u16_adjNumRegs = u16_numRegs;
    }
    else if ((u16_reqReg > 9999) && (u16_reqReg < 20000) && ((u8_mbReqFunc == 3) || (u8_mbReqFunc == 4))) {
      
      b_reqRegManip = true;  // 10k request
      u16_adjReqReg = u16_reqReg - 10000;
      b_foundReg = findRegister(u16_adjReqReg, fltConvFlg, u8_mtrType);

      
      if (0x01 & u16_numRegs) { // if odd number, then can't return float (must be even # of regs)
        u8_mbError = 0x02;
      }
      else if (b_foundReg) { // found registers
        // u16_adjNumRegs is the length request sent to modbus device
        // all requests made through 10k registers must be with expectation of float
        // this means the unadjusted length request has 2 registers for every unique data point
        //switch (u8_mskdDataTypeFlags){
        switch (fltConvFlg) {
          case FloatConv::UINT16:  // u16
          case FloatConv::INT16:  // s16
            u16_adjNumRegs = u16_numRegs / 2;  // single register values
            break;
          case FloatConv::MOD20K:  // m20k
          case FloatConv::MOD20K_WS:  // m20k
            u16_adjNumRegs = u16_numRegs * 3 / 2;  // 3 register values
            break;
          case FloatConv::MOD30K:  // m30k
          case FloatConv::MOD30K_WS:  // m30k
          case FloatConv::UINT64:  // u64
          case FloatConv::UINT64_WS:  // u64
          case FloatConv::ENERGY:  // engy
          case FloatConv::ENERGY_WS:  // engy
          case FloatConv::DOUBLE:  // dbl
          case FloatConv::DOUBLE_WS:  // dbl
            u16_adjNumRegs = u16_numRegs * 2;  // 4 register values
            break;
          default:  // float, u32, s32, m10k, m1k
            u16_adjNumRegs = u16_numRegs;  // 2 register values
            break;              
        }
      }
      else {
//          Serial.println("no flags");
        u8_mbError = 0x02;
      }  // end if else check if address in block
      //}  // end if else check lgth  
    }  // end if 10k request
    else {
//      Serial.println("reg outside exp");
      u8_mbError = 0x02;
    }
    
    //if ((!(u8_mbReqFunc == 3) || (u8_mbReqFunc == 4))) {
    if (u8_mbReqFunc > 4) {
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
      return u8_mbError;
    }  // end if error
    else {  // no error yet, handle code
      switch (u8_mbReqFunc) {
        case 1:
          //u8_mbResult = g_mm_node.readCoils(u16_adjReqReg, u16_adjNumRegs);
          break;
        case 2:
          //u8_mbResult = g_mm_node.readDiscreteInputs(u16_adjReqReg, u16_adjNumRegs);
          break;
        case 3:
          //u8_mbResult = g_mm_node.readHoldingRegisters(u16_adjReqReg, u16_adjNumRegs);
          break;
        case 4:
          //u8_mbResult = g_mm_node.readInputRegisters(u16_adjReqReg, u16_adjNumRegs);
          break;
//          case 5:
//            u8_mbResult = g_mm_node.writeSingleCoil(u16_adjReqReg, u16_adjNumRegs);
//            break;
//          case 6:
//            u8_mbResult = g_mm_node.writeSingleRegister(u16_adjReqReg, u16_adjNumRegs);
//            break;
        default:
          u8_mbResult = 0x10;
          break;
      }  // end switch function

      
      switch (u8_mbResult) {  // why even bother with this switch?
        case 0:  // g_mm_node.ku8MBSuccess
          if (!b_reqRegManip) {  // no adjustments to data
            if (b_byteSwap || (u8_mbReqFunc < 3)) {  // if byteswap or coil request
              // would like to use this method, but bytes need to be swapped MSB for network
              //g_mm_node.copyResponseBuffer(&u8a_mbResp[9]);
            }
            else {
              uint16_t u16_tempReg;
              for (int jj = 0, ii = 9; jj < u16_adjNumRegs; ++jj, ii += 2) {
                //u16_tempReg = g_mm_node.getResponseBuffer(jj);

                u8a_mbResp[ii] = highByte(u16_tempReg);
                u8a_mbResp[ii + 1] = lowByte(u16_tempReg);
              }
            }
          }
          else { // 10k request
            // create MeterLibrary class which can take the meter type and register to convert and dump requested values
            MeterLibBlocks mtrBlks(u16_adjReqReg, u8_mtrType);
            //mtrBlks.convertToFloat(g_mm_node, &u8a_mbResp[9]);
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
          }
          
          u8a_mbResp[4] = ((u8_mbRespNumBytes + 3) >> 8);  // expected tcp length
          u8a_mbResp[5] = u8_mbRespNumBytes + 3;  // expected tcp length
  
          u16_mbRespLen = (u8_mbRespNumBytes + 9);
          
          return u8_mbResult;
          break;
      }  // end switch (u8_mbResult)
    }  // end else if no error    
  }  // end if (u16_mbReqLen == 12
  
  return 10;  // Gateway path unavailable
}  // end getModbus()


void handle_modbus(bool b_idleHttp) {
  uint8_t u8a_mbReq[MB_ARRAY_SIZE] = {0};
  uint8_t u8a_mbResp[MB_ARRAY_SIZE];
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
        u16_lenRead = ec_client.read(u8a_mbReq, MB_ARRAY_SIZE);
        u32_mbReqStart = millis();

				// if client hasn't read 6 bytes, then there is a huge problem here
				if (u16_lenRead < 6) {
					u16_givenLen = 65535;
				}
				else {
					u16_givenLen = word(u8a_mbReq[4], u8a_mbReq[5]) + 6;
				}

        if ((u16_lenRead > u16_givenLen) || (u16_givenLen > MB_ARRAY_SIZE)) {
          ec_client.stop();               // grabbed too much, just exit without worrying.  It  shouldn't happen, any other 
          Ethernet52.cleanSockets(502);   //   connection will have dift sockets or incoming packet larger than array (this 
          return;                         //   should not happen, modbus/tcp deals with pretty small stuff overall)
        }

        while (u16_lenRead < u16_givenLen) {  // make sure to grab the full packet
          u16_lenRead += ec_client.read(u8a_mbReq + u16_lenRead, MB_ARRAY_SIZE - u16_lenRead);

          if ((millis() - u32_mbReqStart) > k_u32_mbReqTimeout) {  // 10 ms might be too quick, but not really sure
            ec_client.stop();  // could not get full message, exit
            Ethernet52.cleanSockets(502);
            return;
          }

					if (u16_lenRead < 6) {
						u16_givenLen = 65535;
					}
					else {
						u16_givenLen = word(u8a_mbReq[4], u8a_mbReq[5]) + 6;
					}
        }
        
        getModbus(u8a_mbReq, u16_lenRead, u8a_mbResp, u16_mbRespLen, false);

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
    Ethernet52.cleanSockets(502);
  }  // end if (client)
  
}
