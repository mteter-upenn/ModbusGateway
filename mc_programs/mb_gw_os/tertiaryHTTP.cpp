/*
* void sendPostResp
* char * preprocPost
* void getPostSetupData
*/

#include "tertiaryHTTP.h"
#include <Ethernet52.h>
#include "globals.h"
#include <EEPROM.h>
#include <Time.h>
#include "miscFuncs.h"
#include <ModbusStructs.h>
#include "handleRTC.h"

void sendPostResp(EthernetClient52 &ec_client) {
  char ca_postResp[67]; // = "HTTP/1.1 303 See Other\nLocation: http://";

  strcpy_P(ca_postResp, PSTR("HTTP/1.1 303 See Other\nLocation: /reset.htm\nConnection: close\n\n"));
  ec_client.write(ca_postResp);

  ec_client.flush();
}


char* preprocPost(EthernetClient52 &ec_client, char *cp_httpHdr, uint16_t &u16_postLen) {
  char *cp_srchPtr;
  int16_t s16_lenRead;
  bool b_foundPtr = false;
  const char cca_contLen[] = "Content-Length: ";
  const char cca_escHead[] = "\r\n\r\n";
  uint16_t u16_charMatches = 0;

  u16_postLen = 0;

  s16_lenRead = ec_client.read((uint8_t*)cp_httpHdr, POST_BUFFER_SIZE - 1); // read out to g_POST_BUF_SZ
  cp_httpHdr[s16_lenRead] = 0;

  while (!b_foundPtr) {
    cp_srchPtr = strstr(cp_httpHdr, "Content-Length: ");  // Find 'Content-Length: '

    if (cp_srchPtr != nullptr) {
      cp_srchPtr += 16;  // add 16 to get to end of phrase
      b_foundPtr = true;  // exit condition
    }
    else {  // check to see if phrase was cut in 2
      cp_srchPtr = cp_httpHdr + s16_lenRead - 15;  // stick msgPtr at end of read message

      for (; cp_srchPtr < cp_httpHdr + s16_lenRead; ++cp_srchPtr) {
        for (uint32_t ii = 0; ii < 16; ++ii) {  // 16 is length of 'Content-Length: '
          if ((cp_srchPtr + ii) == (cp_httpHdr + s16_lenRead)) {
            // end of msgPtr, // match up to previous point
            u16_charMatches = ii;
          }
          else if (*(cp_srchPtr + ii) == cca_contLen[ii]) {
            // continue to match headHttp and contLen
          }
          else {
            break;  // mismatch
          }
        }

        if (u16_charMatches) {  // a match has been found, no reason to keep looking
          break;
        }
      }

      s16_lenRead = ec_client.read((uint8_t*)cp_httpHdr, POST_BUFFER_SIZE - 1); // read out to POST_BUF_SZ
      cp_httpHdr[s16_lenRead] = 0;

      if (u16_charMatches) {
        // try to match beginning of headHttp with remaining chars of contLen
        for (int ii = u16_charMatches; ii < 16; ++ii) {
          if (cca_contLen[ii] == cp_httpHdr[ii - u16_charMatches]) {
            if (ii == 15) {
              b_foundPtr = true;  // escape condition
              cp_srchPtr = cp_httpHdr + 16 - u16_charMatches;  // end of phrase
            }
          }
          else {
            u16_charMatches = 0;
            break;
          }
        }
      }  // if charMatches
    }  // else pointer is NULL
  }  // while haven't found pointer

  // after 'Content-Length: ' is found, determine the postLen
  for (int ii = 0; ii < 2; ++ii) {  // used for loop instead just to be sure it breaks free even if I get nothing out of it
    while (isdigit(*cp_srchPtr)) {
      u16_postLen = u16_postLen * 10 + ((*cp_srchPtr) - '0');
      cp_srchPtr++;
    }

    if ((*cp_srchPtr) == '\0') {
      ec_client.read((uint8_t*)cp_httpHdr, POST_BUFFER_SIZE - 1); // length was cut off, load next portion into headHttp
      cp_srchPtr = cp_httpHdr;  // reset postLenPtr to start of headHttp
    }
    else {
      break;  // breaks for loop
    }
  }

  // now find '\r\n\r\n'  -  this escapes from the http header
  b_foundPtr = false;
  u16_charMatches = 0;

  while (!b_foundPtr) {
    cp_srchPtr = strstr(cp_httpHdr, cca_escHead);

    if (cp_srchPtr != nullptr) {
      cp_srchPtr += 4;  // add 4 to get to end of phrase
      b_foundPtr = true;  // exit condition
    }
    else {  // check to see if phrase was cut in 2
      cp_srchPtr = cp_httpHdr + s16_lenRead - 3;  // stick msgPtr at end of read message

      for (; cp_srchPtr < cp_httpHdr + s16_lenRead; ++cp_srchPtr) {
        for (int ii = 0; ii < 4; ++ii) {
          if ((cp_srchPtr + ii) == (cp_httpHdr + s16_lenRead)) {
            // end of msgPtr, // match up to previous point
            u16_charMatches = ii;
          }
          else if (*(cp_srchPtr + ii) == cca_escHead[ii]) {
            // continue to match headHttp and contLen
          }
          else {
            break;  // mismatch
          }
        }

        if (u16_charMatches) {  // a match has been found, no reason to keep looking
          break;
        }
      }

      s16_lenRead = ec_client.read((uint8_t*)cp_httpHdr, POST_BUFFER_SIZE - 1); // read out to POST_BUFFER_SIZE
      cp_httpHdr[s16_lenRead] = 0;

      if (u16_charMatches) {
        // try to match beginning of headHttp with remaining chars of escHead
        for (int ii = u16_charMatches; ii < 4; ++ii) {
          if (cca_escHead[ii] == cp_httpHdr[ii - u16_charMatches]) {
            if (ii == 3) {
              b_foundPtr = true;  // escape condition
              cp_srchPtr = cp_httpHdr + 3 - u16_charMatches;  // end of phrase
            }
          }
          else {
            u16_charMatches = 0;
            break;
          }
        }
      }  // if charMatches
    }  // else pointer is NULL
  }  // while haven't found pointer

  cp_httpHdr[s16_lenRead + ec_client.read((uint8_t*)cp_httpHdr + s16_lenRead, REQUEST_BUFFER_SIZE - s16_lenRead - 1) + 1] = 0;
  return cp_srchPtr;
}


void getPostSetupData(EthernetClient52 &ec_client) {
  char *cp_paramStart, *cp_paramEnd, *cp_argStart, *cp_argEnd;        // pointers used for setting barriers around values and identifiers
  char *cp_iterPtr;                                        // dummy pointer for loops
  uint8_t u8_dum, u8_mtrInd;
  uint8_t u8_numGivenMtrs = 0;
  uint16_t u16_dum;
  uint32_t u32_dum;
  bool b_readingMsg = true;
  uint16_t u16_postLen;                                    // length of message given in header
  uint16_t u16_totLenRead;                                     // length of message actually in headHttp
  char *cp_postMsgPtr;                                   // pointer to beginning of message
  char cp_httpHdr[REQUEST_BUFFER_SIZE] = { 0 };

  cp_httpHdr[REQUEST_BUFFER_SIZE - 1] = 0;
  cp_postMsgPtr = preprocPost(ec_client, cp_httpHdr, u16_postLen);     // get length of post message and place pointer at its start

  u16_totLenRead = strlen(cp_httpHdr) - (cp_postMsgPtr - cp_httpHdr);

  cp_paramStart = cp_postMsgPtr;

  digitalWrite(EEPROM_WRITE_LEN_PIN, HIGH);
  Serial.println(cp_postMsgPtr);

  while (b_readingMsg) {
    if ((*cp_postMsgPtr) != '\0') {
      if ((*cp_postMsgPtr) == '=') {
        cp_paramEnd = cp_postMsgPtr;
        cp_postMsgPtr++;
        cp_argStart = cp_postMsgPtr;

        while (((*cp_postMsgPtr) != '&') && ((*cp_postMsgPtr) != '\0')) {  // not sure how to handle if '\0' pops up with more message to be read
          cp_postMsgPtr++;
        }

        cp_argEnd = cp_postMsgPtr;

        if (strncmp(cp_paramStart, "mip", 3) == 0) {  //  ****************************************** METER IP *************************************************
          u8_mtrInd = 0;
          for (cp_iterPtr = (cp_paramStart + 3); cp_iterPtr < cp_paramEnd; ++cp_iterPtr) {
            u8_mtrInd = u8_mtrInd * 10 + ((*cp_iterPtr) - '0');
          }

          IpArray ipStruct = {{0, 0, 0, 0}};

          if (u8_mtrInd < u8_numGivenMtrs) {  // make sure to only record necessary number of meters
            if (cp_argStart == cp_argEnd) {  // blank values for ips translate to 0's
              // pass
            }
            else {
              cp_iterPtr = cp_argStart;
              for (int jj = 0; jj < 4; ++jj) {
                u8_dum = 0;
                while (((*cp_iterPtr) != '.') && (cp_iterPtr < cp_argEnd)) {
                  u8_dum = u8_dum * 10 + ((*cp_iterPtr) - '0');
                  cp_iterPtr++;
                }
                cp_iterPtr++;
                ipStruct.u8a_ip[jj] = u8_dum;
              }
            }
            EEPROM.put(g_u16_mtrBlkStart + 9 * u8_mtrInd + 4, ipStruct);
          }
        }
        else if (strncmp(cp_paramStart, "id", 2) == 0) {  //  ****************************************** MODBUS ID *************************************************
          u8_mtrInd = 0;
          for (cp_iterPtr = (cp_paramStart + 2); cp_iterPtr < cp_paramEnd; ++cp_iterPtr) {
            u8_mtrInd = u8_mtrInd * 10 + ((*cp_iterPtr) - '0');
          }

          if (u8_mtrInd < u8_numGivenMtrs) {  // make sure to only record necessary number of meters
            u8_dum = 0;
            for (cp_iterPtr = cp_argStart; cp_iterPtr < cp_argEnd; ++cp_iterPtr) {
              u8_dum = u8_dum * 10 + ((*cp_iterPtr) - '0');
            }
            EEPROM.put(g_u16_mtrBlkStart + 9 * u8_mtrInd + 8, u8_dum);
          }
        }
        else if (strncmp(cp_paramStart, "vid", 3) == 0) {  //  ************************************ VIRTUAL MODBUS ID *************************************************
          u8_mtrInd = 0;
          for (cp_iterPtr = (cp_paramStart + 3); cp_iterPtr < cp_paramEnd; ++cp_iterPtr) {
            u8_mtrInd = u8_mtrInd * 10 + ((*cp_iterPtr) - '0');
          }

          if (u8_mtrInd < u8_numGivenMtrs) {  // make sure to only record necessary number of meters
            u8_dum = 0;
            for (cp_iterPtr = cp_argStart; cp_iterPtr < cp_argEnd; ++cp_iterPtr) {
              u8_dum = u8_dum * 10 + ((*cp_iterPtr) - '0');
            }
            EEPROM.put(g_u16_mtrBlkStart + 9 * u8_mtrInd + 9, u8_dum);
          }
        }
        else if (strncmp(cp_paramStart, "mtr", 3) == 0) {  //  ****************************************** METER TYPE *************************************************
          u8_mtrInd = 0;
          for (cp_iterPtr = (cp_paramStart + 3); cp_iterPtr < cp_paramEnd; ++cp_iterPtr) {
            u8_mtrInd = u8_mtrInd * 10 + ((*cp_iterPtr) - '0');
          }
          TypeArray typeStruct = {{0, 0, 0}};
          if (u8_mtrInd < u8_numGivenMtrs) {  // make sure to only record necessary number of meters
            cp_iterPtr = cp_argStart;
            for (int jj = 0; jj < 3; ++jj) {
              u8_dum = 0;
              while (((*cp_iterPtr) != '.') && (cp_iterPtr < cp_argEnd)) {
                u8_dum = u8_dum * 10 + ((*cp_iterPtr) - '0');
                cp_iterPtr++;
              }
              cp_iterPtr++;

              typeStruct.u8a_type[jj] = u8_dum;
            }
            EEPROM.put(g_u16_mtrBlkStart + 9 * u8_mtrInd + 1, typeStruct);
          }
        }
        else if (strncmp(cp_paramStart, "numMtrs", 7) == 0) {  //  ****************************************** NUM METERS *************************************************
          for (cp_iterPtr = cp_argStart; cp_iterPtr < cp_argEnd; ++cp_iterPtr) {
            u8_numGivenMtrs = u8_numGivenMtrs * 10 + ((*cp_iterPtr) - '0');
          }

          if (u8_numGivenMtrs > 20) {
            u8_numGivenMtrs = 20;
          }

          EEPROM.put(g_u16_mtrBlkStart, u8_numGivenMtrs);
        }
        else if (strncmp(cp_paramStart, "nm", 2) == 0) {  // ******************************** NAME ****************************************
          NameArray nameStruct = {{0}};

          if ((cp_argEnd - cp_argStart) > 31) {
            cp_argEnd = cp_argStart + 31;
          }
          for (cp_iterPtr = cp_argStart; cp_iterPtr < cp_argEnd; ++cp_iterPtr) {  // limited to 30 characters
            if ((*cp_iterPtr) == 43) {  // filter out '+' as html concatenator
              (*cp_iterPtr) = 32;  // replace with blank space
            }

            nameStruct.ca_name[cp_iterPtr - cp_argStart] = *cp_iterPtr;
          }
          if ((cp_argEnd - cp_argStart) != 31) {
            nameStruct.ca_name[cp_iterPtr - cp_argStart] = 0;
          }
          EEPROM.put(g_u16_nameBlkStart, nameStruct);
        }
        else if (strncmp(cp_paramStart, "rd", 2) == 0) {  // ***************************************** Record Data ************************************
          u16_dum = 0;
          for (cp_iterPtr = cp_argStart; cp_iterPtr < cp_argEnd; ++cp_iterPtr) {
            u16_dum = u16_dum * 10 + ((*cp_iterPtr) - '0');
          }

          if (u16_dum) {
            EEPROM.put(g_u16_nameBlkStart + 32, true);
          }
          else {
            EEPROM.put(g_u16_nameBlkStart + 32, false);
          }
        }
        else if (strncmp(cp_paramStart, "ms", 2) == 0) {  // ***************************** Max Number of Slaves to Record ************************************
          u8_dum = 0;
          for (cp_iterPtr = cp_argStart; cp_iterPtr < cp_argEnd; ++cp_iterPtr) {
            u8_dum = u8_dum * 10 + ((*cp_iterPtr) - '0');
          }

          EEPROM.put(g_u16_nameBlkStart + 33, u8_dum);
        }
        else if (strncmp(cp_paramStart, "ip", 2) == 0) {  //  ***************************************** IP ************************************************
          IpArray ipStruct = {{0, 0, 0, 0}};
          cp_iterPtr = cp_argStart;
          for (int jj = 0; jj < 4; ++jj) {
            u8_dum = 0;
            while (((*cp_iterPtr) != '.') && (cp_iterPtr < cp_argEnd)) {
              u8_dum = u8_dum * 10 + ((*cp_iterPtr) - '0');
              cp_iterPtr++;
            }
            cp_iterPtr++;

            ipStruct.u8a_ip[jj] = u8_dum;
          }
          EEPROM.put(g_u16_ipBlkStart + 6, ipStruct);
        }
        else if (strncmp(cp_paramStart, "sm", 2) == 0) {  //  ****************************************** SUBNET MASK *************************************************
          IpArray ipStruct = {{0, 0, 0, 0}};
          cp_iterPtr = cp_argStart;
          for (int jj = 0; jj < 4; ++jj) {
            u8_dum = 0;
            while (((*cp_iterPtr) != '.') && (cp_iterPtr < cp_argEnd)) {
              u8_dum = u8_dum * 10 + ((*cp_iterPtr) - '0');
              cp_iterPtr++;
            }
            cp_iterPtr++;
            ipStruct.u8a_ip[jj] = u8_dum;
          }
          EEPROM.put(g_u16_ipBlkStart + 10, ipStruct);
        }
        else if (strncmp(cp_paramStart, "gw", 2) == 0) {  //  ****************************************** DEFAULT GATEWAY *************************************************
          IpArray ipStruct = {{0, 0, 0, 0}};
          cp_iterPtr = cp_argStart;
          for (int jj = 0; jj < 4; ++jj) {
            u8_dum = 0;
            while (((*cp_iterPtr) != '.') && (cp_iterPtr < cp_argEnd)) {
              u8_dum = u8_dum * 10 + ((*cp_iterPtr) - '0');
              cp_iterPtr++;
            }
            cp_iterPtr++;
            ipStruct.u8a_ip[jj] = u8_dum;
          }
          EEPROM.put(g_u16_ipBlkStart + 14, ipStruct);
        }
        else if (strncmp(cp_paramStart, "tcpto", 5) == 0) {  //  ************************************ TCP SOCKET TIMEOUT *************************************************
          u32_dum = 0;

          for (cp_iterPtr = cp_argStart; cp_iterPtr < cp_argEnd; ++cp_iterPtr) {
            u32_dum = u32_dum * 10 + ((*cp_iterPtr) - '0');
          }

          EEPROM.put(g_u16_ipBlkStart + 18, u32_dum);
        }
        else if (strncmp(cp_paramStart, "ntp", 3) == 0) {  // ***************************************** USE NTP? ************************************
          u16_dum = 0;
          for (cp_iterPtr = cp_argStart; cp_iterPtr < cp_argEnd; ++cp_iterPtr) {
            u16_dum = u16_dum * 10 + ((*cp_iterPtr) - '0');
          }

          if (u16_dum) {
            EEPROM.put(g_u16_ipBlkStart + 22, true);
          }
          else {
            EEPROM.put(g_u16_ipBlkStart + 22, false);
          }
        }
        else if (strncmp(cp_paramStart, "nip", 3) == 0) {  //  ************************************ NTP SERVER IP *****************************************
          IpArray ipStruct = {{0, 0, 0, 0}};
          cp_iterPtr = cp_argStart;
          for (int jj = 0; jj < 4; ++jj) {
            u8_dum = 0;
            while (((*cp_iterPtr) != '.') && (cp_iterPtr < cp_argEnd)) {
              u8_dum = u8_dum * 10 + ((*cp_iterPtr) - '0');
              cp_iterPtr++;
            }
            cp_iterPtr++;

            ipStruct.u8a_ip[jj] = u8_dum;
          }
          EEPROM.put(g_u16_ipBlkStart + 23, ipStruct);
        }
        else if (strncmp(cp_paramStart, "br", 2) == 0) {  //  ****************************************** BAUDRATE *************************************************
          u32_dum = 0;

          for (cp_iterPtr = cp_argStart; cp_iterPtr < cp_argEnd; ++cp_iterPtr) {
            u32_dum = u32_dum * 10 + ((*cp_iterPtr) - '0');
          }

          EEPROM.put(g_u16_ipBlkStart + 27, u32_dum);
        }
        else if (strncmp(cp_paramStart, "db", 2) == 0) {  //  ************************************ DATA BITS *************************************************
          u8_dum = 0;

          for (cp_iterPtr = cp_argStart; cp_iterPtr < cp_argEnd; ++cp_iterPtr) {
            u8_dum = u8_dum * 10 + ((*cp_iterPtr) - '0');
          }

          EEPROM.put(g_u16_ipBlkStart + 31, u8_dum);
        }
        else if (strncmp(cp_paramStart, "par", 3) == 0) {  //  ************************************ PARITY *************************************************
          u8_dum = 0;

          for (cp_iterPtr = cp_argStart; cp_iterPtr < cp_argEnd; ++cp_iterPtr) {
            u8_dum = u8_dum * 10 + ((*cp_iterPtr) - '0');
          }

          EEPROM.put(g_u16_ipBlkStart + 32, u8_dum);
        }
        else if (strncmp(cp_paramStart, "sb", 2) == 0) {  //  ************************************ STOP BITS *************************************************
          u8_dum = 0;

          for (cp_iterPtr = cp_argStart; cp_iterPtr < cp_argEnd; ++cp_iterPtr) {
            u8_dum = u8_dum * 10 + ((*cp_iterPtr) - '0');
          }

          EEPROM.put(g_u16_ipBlkStart + 33, u8_dum);
        }
        else if (strncmp(cp_paramStart, "to", 2) == 0) {  //  ****************************************** MB TIMEOUT *************************************************
          u16_dum = 0;
          for (cp_iterPtr = cp_argStart; cp_iterPtr < cp_argEnd; ++cp_iterPtr) {
            u16_dum = u16_dum * 10 + ((*cp_iterPtr) - '0');
          }

          EEPROM.put(g_u16_ipBlkStart + 34, u16_dum);
        }
        else if (strncmp(cp_paramStart, "prtsd", 5) == 0) {  // ******************************** PRINT MB COMMS TO SD? ************************************
          u16_dum = 0;
          for (cp_iterPtr = cp_argStart; cp_iterPtr < cp_argEnd; ++cp_iterPtr) {
            u16_dum = u16_dum * 10 + ((*cp_iterPtr) - '0');
          }

          if (u16_dum) {
            EEPROM.put(g_u16_ipBlkStart + 36, true);
          }
          else {
            EEPROM.put(g_u16_ipBlkStart + 36, false);
          }
        }
        else if (strncmp(cp_paramStart, "tm", 2) == 0) {  //  ****************************************** TIME *************************************************
          //Serial.println(F("time"));
//          const int32_t k_s32_timeZone = -5;
          u32_dum = 0;

          for (cp_iterPtr = cp_argStart; cp_iterPtr < cp_argEnd; ++cp_iterPtr) {
            u32_dum = u32_dum * 10 + ((*cp_iterPtr) - '0');
          }
          u32_dum += TIME_ZONE_DIFF * SECS_PER_HOUR;

          Serial.print("time from POST: "); printTime(u32_dum);

          if (u32_dum > 1451606400UL) {
            g_b_rtcGood = true;
            Teensy3Clock.set(u32_dum);
            setTime(u32_dum);
            Serial.print("update time and retrieve "); getRtcTime();
            Serial.print("via now"); printTime(now());
          }
        }

        cp_postMsgPtr++;
        cp_paramStart = cp_postMsgPtr;
      }  // end if *cp_postMsgPtr == '='
      else {  // doesn't equal '='
        cp_postMsgPtr++;
      }  // end if ch == '='
    }  // if not '\0'
    else {  // ((*cp_postMsgPtr) == '\0')
      if (u16_totLenRead < u16_postLen) {
        if (ec_client.available()) {
          uint16_t lenRead;
          lenRead = ec_client.read((uint8_t*)cp_httpHdr, REQUEST_BUFFER_SIZE - 1);
          cp_httpHdr[lenRead] = 0;
          u16_totLenRead += lenRead;
        }
        else {
          b_readingMsg = false;  // exit if no more can be found
        }
      }
      else {
        b_readingMsg = false;  // read everything already, exit
      }
    }
  }  // end while

  digitalWrite(EEPROM_WRITE_LEN_PIN, LOW);
  setConstants();
  writeRestartFile();
}

