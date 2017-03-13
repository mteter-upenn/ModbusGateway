#include "globals.h"
#include "handleData.h"
#include <SD.h>
#include <EEPROM.h>
#include <MeterLibrary.h>
#include "secondaryData.h"
#include <Time.h>

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


    g_u32_lastDataRequest = u32_curDataTime;
    u8_numSlvsRcd = SlaveData.getNumSlvs() > g_u8_maxRecordSlaves ? g_u8_maxRecordSlaves : SlaveData.getNumSlvs();
    memset(u8a_mbReq, 0, 5);
    u8a_mbReq[5] = 6;     // length of modbus half

    ca_fileName[30] = 0;
    digitalWrite(SD_WRITE_LED_PIN, HIGH);
    getFileName(t_time, ca_fileName);  // gets ca_fileName and prints header (if needed) and first column (date)

    dataFile = SD.open(ca_fileName, FILE_WRITE);

    if (dataFile) {
      const int k_i_maxNumElecVals(32);
      const int k_i_maxNumStmChwVals(10);
      int i_numMtrVals;
      int i_totStrtInd;
      MeterLibGroups mtrGrps(1);
      uint16_t u16_numGrps;
      uint16_t u16_reqReg, u16_numRegs;
      float fa_data[k_i_maxNumElecVals];  // 32 is max number of value types that can be sent
      int8_t s8a_dataFlags[k_i_maxNumElecVals];
      char ca_fileBuffer[650] = { 0 };

      uint8_t u8_slvVid;
      uint8_t u8_slvMbFunc;
      uint8_t u8_slvMtrType;
      uint8_t u8_mbStatus = 1;
//      uint8_t u8a_mbResp[MB_ARRAY_SIZE];
//      uint16_t u16_mbRespLen;

      for (int ii = 0; ii < u8_numSlvsRcd; ++ii) {
        u8_slvMtrType = SlaveData[ii].u8a_mtrType[0];
        u8_slvVid = SlaveData[ii].u8_vid;
//        u8_slvMbFunc = EEPROM.read(g_u16_regBlkStart + 4 * u8_slvMtrType + 2);
        EEPROM.get(g_u16_regBlkStart + 4 * u8_slvMtrType + 2, u8_slvMbFunc);

        mtrGrps.setMeterType(u8_slvMtrType);
        u16_numGrps = mtrGrps.getNumGrps();

        u8a_mbReq[6] = u8_slvVid;   // u8_slvVidice id
        u8a_mbReq[7] = u8_slvMbFunc;  // modbus u8_slvMbFunction

        // collect all data in float array
        for (int jj = 1; jj < u16_numGrps; ++jj) {
          mtrGrps.setGroup(jj);
          u16_reqReg = mtrGrps.getReqReg();
          u16_numRegs = mtrGrps.getNumRegs();

          u8a_mbReq[8] = highByte(u16_reqReg);
          u8a_mbReq[9] = lowByte(u16_reqReg);
          u8a_mbReq[10] = highByte(u16_numRegs);  // assume no length higher than 255
          u8a_mbReq[11] = lowByte(u16_numRegs);  // ask for float conversion = 2*num for registers

          delay(5); // ensure long enough delay between polls

//          u8_mbStatus = getModbus(u8a_mbReq, 12, u8a_mbResp, u16_mbRespLen, true);  // getModbus uses MB/TCP as inputs and outputs

          if (u8_mbStatus == 0) {
            // record data for group's values
            //mtrGrps.groupToFloat(&u8a_mbResp[9], fa_data, s8a_dataFlags);
          }
          else if (jj == 1) {
            // set all flags to error - assume that since the first group failed, others will fail as well and cut losses
            memset(s8a_dataFlags, -1, k_i_maxNumElecVals);
          }
          else {
            // set groups values to fail state
            mtrGrps.groupMbErr(s8a_dataFlags);
          }
        } // end for loop through groups

        // last group full of duds (if any) ********************************************************************
        mtrGrps.setGroup(u16_numGrps);  //                                                                     *
        mtrGrps.groupLastFlags(s8a_dataFlags);  //                                                             *
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

        dataFile.write(ca_fileBuffer);
        
        ca_fileBuffer[0] = 0;
      }  // end for rotate through slaves

      dataFile.print(F("\n"));
      dataFile.close();
    }  // end if dataFile

    digitalWrite(SD_WRITE_LED_PIN, LOW);
  }  // end if time
}
