void handle_data() {
  uint32_t u32_curDataTime;
  const uint32_t k_u32_period(300000);  // time in ms 300'000UL wait every 5 minutes

  u32_curDataTime = millis();
  
  if ((u32_curDataTime - g_u32_lastDataRequest) > k_u32_period) {  
    

    
    time_t t_time = now();
    uint8_t u8_numSlvsRcd;
    uint8_t u8a_mbReq[12];
    File dataFile;
    char ca_fileName[31] = {0};

    //union convertUnion
    //{
    //  float f;
    //  uint8_t u8[4];
    //} u8a2flt;

    g_u32_lastDataRequest = u32_curDataTime;
    u8_numSlvsRcd = g_u8_numSlaves > g_u8_maxRecordSlaves ? g_u8_maxRecordSlaves : g_u8_numSlaves;
    memset(u8a_mbReq, 0, 5);
    u8a_mbReq[5] = 6;     // length of modbus half

    ca_fileName[30] = 0;
    digitalWrite(gk_s16_sdWriteLed, HIGH);
    getFileName(t_time, ca_fileName);  // gets ca_fileName and prints header (if needed) and first column (date)

    dataFile = SD.open(ca_fileName, FILE_WRITE);
    //printTime(t_time);

    if (dataFile) {
      //Serial.println(F("opened file"));
      const int k_i_maxNumElecVals(32);
      const int k_i_maxNumStmChwVals(10);
      int i_numMtrVals;
      int i_totStrtInd;
      MeterLibrary mtrLib(1);
      uint16_t u16_numGrps;
      uint16_t u16_reqReg, u16_numRegs;
      float fa_data[k_i_maxNumElecVals];  // 32 is max number of value types that can be sent
      int8_t s8a_dataFlags[k_i_maxNumElecVals];
      char ca_fileBuffer[650] = { 0 };

      uint8_t u8_slvVid;
      uint8_t u8_slvMbFunc;
      uint8_t u8_slvMtrType;
      //uint16_t u16_mtrLibStart;
      //uint16_t u16_mtrGrpStart;
      //PwrEgyRegs elecReg;
      bool b_mbStatus;
      uint8_t u8a_mbResp[gk_u16_mbArraySize];
      uint16_t u16_mbRespLen;

      for (int ii = 0; ii < u8_numSlvsRcd; ++ii) {
        u8_slvMtrType = g_u8a_slaveTypes[ii][0];
        u8_slvVid = g_u8a_slaveVids[ii];
        u8_slvMbFunc = EEPROM.read(g_u16_regBlkStart + 4 * u8_slvMtrType + 2);

        mtrLib.changeInputs(0, 0, u8_slvMtrType, true);
        u16_numGrps = mtrLib.getNumGrps();

        u8a_mbReq[6] = u8_slvVid;   // u8_slvVidice id
        u8a_mbReq[7] = u8_slvMbFunc;  // modbus u8_slvMbFunction

        /*
        u16_mtrLibStart = word(EEPROM.read(g_u16_regBlkStart + 4 * u8_slvMtrType - 1), EEPROM.read(g_u16_regBlkStart + 4 * u8_slvMtrType));

        //u16_mtrGrpStart = word(EEPROM.read(u16_mtrLibStart + 3), EEPROM.read(u16_mtrLibStart + 4));

        //Serial.print("type: "); Serial.println(u8_slvMtrType, DEC);
        //Serial.print("regBlkStart: "); Serial.println(g_u16_regBlkStart, DEC);
        //Serial.print("lib start: "); Serial.println(u16_mtrLibStart, DEC);
        //Serial.print("group start: "); Serial.println(u16_mtrGrpStart, DEC);

        u8a_mbReq[6] = u8_slvVid;   // u8_slvVidice id
        u8a_mbReq[7] = u8_slvMbFunc;  // modbus u8_slvMbFunction
        u8a_mbReq[10] = 0;  // assume no length higher than 255
        u8a_mbReq[11] = 2;  // ask for float conversion = 2*num for registers
        */


        // collect all data in float array
        for (int jj = 1; jj < u16_numGrps; ++jj) {
          mtrLib.setGroup(jj);
          u16_reqReg = mtrLib.getReqReg();
          u16_numRegs = mtrLib.getNumRegs();

          u8a_mbReq[8] = highByte(u16_reqReg);
          u8a_mbReq[9] = lowByte(u16_reqReg);
          u8a_mbReq[10] = highByte(u16_numRegs);  // assume no length higher than 255
          u8a_mbReq[11] = lowByte(u16_numRegs);  // ask for float conversion = 2*num for registers

          delay(5); // ensure long enough delay between polls

          b_mbStatus = getModbus(u8a_mbReq, 12, u8a_mbResp, u16_mbRespLen, true);  // getModbus uses MB/TCP as inputs and outputs

          if (b_mbStatus) {
            // record data for group's values
            mtrLib.groupToFloat(&u8a_mbResp[9], fa_data, s8a_dataFlags);
          }
          else if (jj == 1) {
            // set all flags to error - assume that since the first group failed, others will fail as well and cut losses
            memset(s8a_dataFlags, -1, k_i_maxNumElecVals);
          }
          else {
            // set groups values to fail state
            mtrLib.groupMbErr(s8a_dataFlags);
          }
        } // end for loop through groups

        // last group full of duds (if any) ********************************************************************
        mtrLib.setGroup(u16_numGrps);  //                                                                      *
        mtrLib.groupLastFlags(s8a_dataFlags);  //                                                              *
        // end handling of last group **************************************************************************


        // dump data to character array

        i_numMtrVals = ((u8_slvMtrType == 11) || (u8_slvMtrType == 12)) ? k_i_maxNumStmChwVals : k_i_maxNumElecVals;
        i_totStrtInd  = ((u8_slvMtrType == 11) || (u8_slvMtrType == 12)) ? 8 : 30;  // set value to change # of sig figs

        for (int jj = 0; jj < i_numMtrVals; ++jj) {
          strcat_P(ca_fileBuffer, PSTR(","));

          switch (s8a_dataFlags[jj]) {
            case 1:
              if (jj < i_totStrtInd) {  // i_totStrtInd is where totalizer values start
                dtostrf(fa_data[jj], 1, 3, (ca_fileBuffer + strlen(ca_fileBuffer)));
              }
              else {  // have totalizer use less digits after decimal point
                dtostrf(fa_data[jj], 1, 1, (ca_fileBuffer + strlen(ca_fileBuffer)));
              }
              break;
            
            case (-1):
              strcat_P(ca_fileBuffer, PSTR("error"));
              break;
            default:  // data is N/A
              break;  // nothing to write
          }
        }

        /*
        switch (u8_slvMtrType) {
          case 11:  // chw
            u8a_mbReq[8] = 0; // highByte(chwRegs[j]);  // should always be 0
            u8a_mbReq[9] = 0; // lowByte(chwRegs[j]);
            u8a_mbReq[11] = 34;  // ask for float conversion = 2*num for registers

            //delay(1); // ensure long enough delay between polls
            
            u8_mbStatus = getModbus(u8a_mbReq, 12, u8a_mbResp, u16_mbRespLen, false);
            
            // {0, 6, 8, 10, 30};
            for (int jj = 0; jj < 5; ++jj) {
              uint16_t u16a_chwRegs[5] = { 0, 6, 8, 10, 30 };  // tons, gpm, Ts, Tr, tonhrs

              if (u8_mbStatus) {  // good message
                u8a2flt.u8[3] = u8a_mbResp[2 * u16a_chwRegs[jj] + 11];
                u8a2flt.u8[2] = u8a_mbResp[2 * u16a_chwRegs[jj] + 12];
                u8a2flt.u8[1] = u8a_mbResp[2 * u16a_chwRegs[jj] + 9]; // high word
                u8a2flt.u8[0] = u8a_mbResp[2 * u16a_chwRegs[jj] + 10];  // low word

                strcat_P(ca_fileBuffer, PSTR(","));
                if (jj < 4) {
                  dtostrf(u8a2flt.f, 1, 3, (ca_fileBuffer + strlen(ca_fileBuffer)));
                }
                else {  // have totalizer use less digits after decimal point
                  dtostrf(u8a2flt.f, 1, 1, (ca_fileBuffer + strlen(ca_fileBuffer)));
                }
              }
              else if (jj == 0) {  // bad message on first try, just skip everything to avoid wasting time
                strcat_P(ca_fileBuffer, PSTR(",error,error,error,error,error"));
                break;
              }
              else {  // bad message
                strcat_P(ca_fileBuffer, PSTR(",error"));
              }
            }
            break;
          case 12:  // stm
            u8a_mbReq[8] = 0; // highByte(stmRegs[j]);
            u8a_mbReq[9] = 0; // lowByte(stmRegs[j]);
            u8a_mbReq[11] = 34;  // ask for float conversion = 2*num for registers

            //delay(1); // ensure long enough delay between polls
            //Serial.println("sending steam modbus");
            u8_mbStatus = getModbus(u8a_mbReq, 12, u8a_mbResp, u16_mbRespLen, false);
            //Serial.println("got steam modbus");

            if (u8_mbStatus) {  // good message
              uint16_t u16a_stmRegs[4] = { 2, 8, 14, 32 };  // lbs/hr, T, P, lbs

              for (int jj = 0; jj < 4; ++jj) {
                u8a2flt.u8[3] = u8a_mbResp[2 * u16a_stmRegs[jj] + 11];
                u8a2flt.u8[2] = u8a_mbResp[2 * u16a_stmRegs[jj] + 12];
                u8a2flt.u8[1] = u8a_mbResp[2 * u16a_stmRegs[jj] + 9]; // high word
                u8a2flt.u8[0] = u8a_mbResp[2 * u16a_stmRegs[jj] + 10];  // low word

                strcat(ca_fileBuffer, ",");
                if (jj < 3) {
                  dtostrf(u8a2flt.f, 1, 3, (ca_fileBuffer + strlen(ca_fileBuffer)));
                }
                else {  // have totalizer use less digits after decimal point
                  dtostrf(u8a2flt.f, 1, 1, (ca_fileBuffer + strlen(ca_fileBuffer)));
                }
              }
            }
            else {  // bad message on first try, just skip everything to avoid wasting time
              strcat_P(ca_fileBuffer, PSTR(",error,error,error,error"));
            }
            
            
            break;
          default:
            elecReg = getElecRegs(u16_mtrLibStart);
            //Serial.print(elecReg.u16_pwr);
            //Serial.print(F(", "));
            //Serial.println(elecReg.u16_egy);

            u8a_mbReq[8] = highByte(elecReg.u16_pwr);
            u8a_mbReq[9] = lowByte(elecReg.u16_pwr);
            

            //delay(5); // ensure long enough delay between polls
            //Serial.println("sending pwr modbus");
            u8_mbStatus = getModbus(u8a_mbReq, 12, u8a_mbResp, u16_mbRespLen, false);
            //Serial.println("got pwr modbus");
            
            if (u8_mbStatus) {
              u8a2flt.u8[3] = u8a_mbResp[11];
              u8a2flt.u8[2] = u8a_mbResp[12];
              u8a2flt.u8[1] = u8a_mbResp[9]; // high word
              u8a2flt.u8[0] = u8a_mbResp[10];  // low word

              //Serial.print(ii + 1, DEC);
              //Serial.print(F(" power: "));
              //Serial.println(int2flt.f);

              strcat_P(ca_fileBuffer, PSTR(","));
              dtostrf(u8a2flt.f, 1, 3, (ca_fileBuffer + 1));

    // get energy now
              u8a_mbReq[8] = highByte(elecReg.u16_egy);
              u8a_mbReq[9] = lowByte(elecReg.u16_egy);

              //delay(1); // ensure long enough delay between polls
              //Serial.println("sending egy modbus");
              u8_mbStatus = getModbus(u8a_mbReq, 12, u8a_mbResp, u16_mbRespLen, false);
              //Serial.println("got egy modbus");

              if (u8_mbStatus) {
                u8a2flt.u8[3] = u8a_mbResp[11];
                u8a2flt.u8[2] = u8a_mbResp[12];
                u8a2flt.u8[1] = u8a_mbResp[9]; // high word
                u8a2flt.u8[0] = u8a_mbResp[10];  // low word

                //Serial.print(i + 1, DEC);
                //Serial.print(F(" energy: "));
                //Serial.println(int2flt.f);

                strcat_P(ca_fileBuffer, PSTR(","));
                dtostrf(u8a2flt.f, 1, 1, (ca_fileBuffer + strlen(ca_fileBuffer)));
              }
              else {
                strcat_P(ca_fileBuffer, PSTR(",error"));
              }
            }
            else {
              // print failures for both 
              strcat_P(ca_fileBuffer, PSTR(",error,error"));
            }
            break;
        }  // end switch
        */


        //Serial.println(ca_fileBuffer);
        //Serial.println(strlen(ca_fileBuffer), DEC);
        dataFile.write(ca_fileBuffer);
        
        ca_fileBuffer[0] = 0;
      }  // end for rotate through slaves

      dataFile.print(F("\n"));
      dataFile.close();
    }  // end if dataFile

    digitalWrite(gk_s16_sdWriteLed, LOW);
  }  // end if time
}
