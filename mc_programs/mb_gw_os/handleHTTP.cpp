#include "handleHTTP.h"
#include "mb_names.h"
#include "globals.h"
#include <Ethernet52.h>
#include "secondaryHTTP.h"
#include <MeterLibrary.h>
#include <ModbusStructs.h>
#include <ModbusStack.h>
#include "tertiaryHTTP.h"
#include <ModbusServer.h>
#include "miscFuncs.h"


SockFlag readHttp(const uint8_t u8_socket, FileReq &u16_fileReq, FileType &s16_fileType, uint8_t &u8_selSlv, 
                  char ca_fileReq[REQUEST_LINE_SIZE]) {
  char ca_httpReqDummy[REQUEST_LINE_SIZE] = { 0 };

  uint16_t u16_lenRead(0);
  uint32_t u32_msgRecvTime;

  if (g_eca_socks[u8_socket].available()) {
    u16_lenRead = g_eca_socks[u8_socket].read((uint8_t*)ca_fileReq, REQUEST_LINE_SIZE - 1);
  }

  u32_msgRecvTime = millis();
  uint32_t k_u32_mesgTimeout(50);
  while (u16_lenRead < (REQUEST_LINE_SIZE - 1)) {  // make sure enough is read
    if (g_eca_socks[u8_socket].available()) {
      u16_lenRead += g_eca_socks[u8_socket].read((uint8_t*)ca_fileReq + u16_lenRead, REQUEST_LINE_SIZE - u16_lenRead - 1);
    }

    if ((u16_lenRead < (REQUEST_LINE_SIZE - 1)) && ((millis() - u32_msgRecvTime) > k_u32_mesgTimeout)) {  // stop trying to read message after 50 ms - assume it's never coming
      return SockFlag_LISTEN;
    }
  }
  ca_fileReq[REQUEST_LINE_SIZE - 1] = 0;

  strcpy(ca_httpReqDummy, ca_fileReq);

  if (strncmp(ca_fileReq, "GET", 3) == 0) {
    convertToFileName(ca_fileReq);

    Serial.print("GET: ");  Serial.println(ca_fileReq);

    if (strstr(ca_fileReq, ".css") != nullptr) {
      s16_fileType = FileType::CSS;
      u16_fileReq = FileReq_EPSTYLE;
    }
    else if (strstr(ca_fileReq, ".gif") != nullptr) {  // will need to expand if more pictures are desired
      s16_fileType = FileType::GIF;
      u16_fileReq = FileReq_LOGO_LET;
    }
    else if ((strstr(ca_fileReq, ".htm") != nullptr) || (strcmp(ca_fileReq, "/") == 0)) { // if html or index request
      s16_fileType = FileType::HTML;
      if (strcmp(ca_fileReq, "/") == 0) {
        u16_fileReq = FileReq_INDEX;
        strcpy(ca_fileReq, "/index.htm");
      }
      else if (strcmp(ca_fileReq, "/index.htm") == 0) {
        u16_fileReq = FileReq_INDEX;
      }
      else if (strcmp(ca_fileReq, "/gensetup.htm") == 0) {
        u16_fileReq = FileReq_GENSETUP;
      }
      else if (strcmp(ca_fileReq, "/mtrsetup.htm") == 0) {
        u16_fileReq = FileReq_MTRSETUP;
      }
      else if (strcmp(ca_fileReq, "/live.htm") == 0) {
        u16_fileReq = FileReq_LIVE;
      }
      else if (strstr(ca_fileReq, "/pastdown.htm") != nullptr) {  // strstr since there will be more afterwards
        u16_fileReq = FileReq_PASTDOWN;
      }
      else if (strcmp(ca_fileReq, "/pastview.htm") == 0) {  // need to flesh out ideas for this (mimic pastdown?)
                                                                 // graph data?
        u16_fileReq = FileReq_PASTVIEW;
      }
      else if (strcmp(ca_fileReq, "/reset.htm") == 0) {
        u16_fileReq = FileReq_RESET;
      }
      else {
        u16_fileReq = FileReq_NOPAGE;
        strcpy(ca_fileReq, "/nopage.htm");
      }
    }
    else if (strstr(ca_fileReq, ".xml") != nullptr) {
      s16_fileType = FileType::XML;
      if (strcmp(ca_fileReq, "/gensetup.xml") == 0) {
        u16_fileReq = FileReq_GENSETUP;
      }
      else if (strcmp(ca_fileReq, "/mtrsetup.xml") == 0) {
        u16_fileReq = FileReq_MTRSETUP;
      }
      else if (strcmp(ca_fileReq, "/data.xml") == 0) {
        char *cp_meterInd;                                     // index of 'METER' in GET request

        u8_selSlv = 0;
        cp_meterInd = strstr(ca_httpReqDummy, "METER=");

        if (cp_meterInd != nullptr) {
          char *cp_dumPtr;
          cp_meterInd += 6;  // move pointer to end of phrase
          cp_dumPtr = cp_meterInd + 3;

          for (; cp_meterInd < cp_dumPtr; ++cp_meterInd) {
            if (isdigit(*cp_meterInd)) {
              u8_selSlv = u8_selSlv * 10 + ((*cp_meterInd) - '0');
            }
            else {
              break;
            }
          }

          if (u8_selSlv > SlaveData.getNumSlvs()) {  // if greater than number of meters listed
            u8_selSlv = 0;
          }
        }

        u16_fileReq = FileReq_DATA;
      }
      else if (strcmp(ca_fileReq, "/info.xml") == 0) {
        u16_fileReq = FileReq_INFO;
        strcpy(ca_fileReq, "/mtrsetup.xml");
      }
      else if (strcmp(ca_fileReq, "/restart.xml") == 0) {
        u16_fileReq = FileReq_RESTART;
      }
      else {  // could not find xml file
        u16_fileReq = FileReq_404;
      }
    }
    else if (strstr(ca_fileReq, ".TXT") != nullptr) {
      s16_fileType = FileType::TXT;
      u16_fileReq = FileReq_404;
    }
    else if (strstr(ca_fileReq, ".CSV") != nullptr) {
      s16_fileType = FileType::CSV;
      u16_fileReq = FileReq_404;
    }
    // not htm, css, or xml
    else {
      s16_fileType = FileType::NONE;
      u16_fileReq = FileReq_404;


    }

    return SockFlag_GET;
  }
  else if (strncmp(ca_fileReq, "POST", 4) == 0) {
    convertToFileName(ca_fileReq);
    Serial.print("POST: "); Serial.println(ca_fileReq);

    if (strcmp(ca_fileReq, "/gensetup.htm") == 0) {
      s16_fileType = FileType::HTML;
      u16_fileReq = FileReq_GENSETUP;
    }
    else if (strcmp(ca_fileReq, "/mtrsetup.htm") == 0) {
      s16_fileType = FileType::HTML;
      u16_fileReq = FileReq_MTRSETUP;
    }
    else {
      s16_fileType = FileType::NONE;
      u16_fileReq = FileReq_404;
    }

    return SockFlag_POST;
  }
  return SockFlag_LISTEN;
}



bool respondHttp(const uint8_t u8_socket, const SockFlag u16_sockFlag, const FileReq u16_fileReq, const FileType s16_fileType, 
                 const uint8_t u8_selSlv, const char ca_fileReq[REQUEST_LINE_SIZE], ModbusStack &mbStack, 
                 uint8_t &u8_curGrp, float fa_liveXmlData[MAX_NUM_ELEC_VALS], int8_t s8a_dataFlags[MAX_NUM_ELEC_VALS]) {
#if DISP_TIMING_DEBUG == 1
  uint32_t gotClient, doneHttp, doneFind, time1 = 0, time2 = 0, lineTime = 0;  // times for debugging
  //uint32_t totBytes = 0;
#endif

  if (u16_sockFlag & SockFlag_GET) {
    if (g_b_sdInit) {
      switch (s16_fileType) {
      case FileType::XML:
        switch (u16_fileReq) {
        case FileReq_GENSETUP:
          sendWebFile(g_eca_socks[u8_socket], ca_fileReq, FileType::XML, false);
          sendXmlEnd(g_eca_socks[u8_socket], XmlFile::GENERAL);
          break;
        case FileReq_MTRSETUP:
          sendWebFile(g_eca_socks[u8_socket], ca_fileReq, FileType::XML, false);
          //sendXmlEnd(g_eca_socks[u8_socket], XmlFile::METER);
          break;
        case FileReq_DATA: {
          MeterLibGroups mtrGrp(SlaveData[u8_selSlv].u8a_mtrType[0]);

          if (u8_curGrp == 0) { // first time, must add to stack
            
            ModbusRequest mbReq;
            u8_curGrp = 1;
            mtrGrp.setGroup(u8_curGrp);
            
            mbReq = mtrGrp.getGroupRequest(SlaveData.isSlaveTcpByInd(u8_selSlv), SlaveData[u8_selSlv].u8_id, SlaveData[u8_selSlv].u8_vid);
            g_u16a_mbReqUnqId[u8_socket] = mbStack.add(mbReq, 2);  // PRIORITY 2!!
            g_u32a_socketTimeoutStart[u8_socket] = millis();  // this is reset for activity
            return false;
          }
          else {
            uint8_t u8_mbReqInd = mbStack.getReqInd(g_u16a_mbReqUnqId[u8_socket]);
            if (!(u8_mbReqInd < mbStack.k_u8_maxSize)) {  // NOTHING EXISTS IN STACK!
              send404(g_eca_socks[u8_socket]);
              u8_curGrp = 0;
              return true;
            }
            if (mbStack[u8_mbReqInd].u8_flags & (MRFLAG_goodData | MRFLAG_timeout)) {
              uint16_t u16a_respData[128] = { 0 };
              uint8_t u8_numBytes(0);
              uint8_t u8_mbStatus(0);

              if (mbStack[u8_mbReqInd].u8_flags & MRFLAG_timeout) {  // protocol has timed out, no message from slave device
                u8_mbStatus = g_modbusServer.k_u8_MBGatewayTargetFailed;
              }
              else if (mbStack[u8_mbReqInd].u8_flags & MRFLAG_isTcp) {  // there is a good message over tcp
                u8_mbStatus = g_modbusServer.recvTcpResponse(mbStack[u8_mbReqInd], u16a_respData, u8_numBytes);
              }
              else {  // there is a good message over serial
                u8_mbStatus = g_modbusServer.recvSerialResponse(mbStack[u8_mbReqInd], u16a_respData, u8_numBytes);
              }

              mtrGrp.setGroup(u8_curGrp);
              if (!u8_mbStatus) {
                mtrGrp.groupToFloat(u16a_respData, fa_liveXmlData, s8a_dataFlags);
              }
              else {
                mtrGrp.groupMbErr(s8a_dataFlags);
              }
              g_u32a_socketTimeoutStart[u8_socket] = millis();

              if (mbStack[u8_mbReqInd].u8_flags & MRFLAG_isTcp) {  // tcp used
                uint8_t u8_dumSck = (mbStack[u8_mbReqInd].u8_flags & MRFLAG_sckMask);

                g_eca_socks[u8_dumSck].stop();
                g_eca_socks[u8_dumSck].setSocket(u8_dumSck);

                g_ba_clientSocksAvail[u8_dumSck - 6] = true;
              }
              else {  // serial used
                g_b_485avail = true;
              }
              mbStack.removeByInd(u8_mbReqInd);
              g_u16a_mbReqUnqId[u8_socket] = 0;

              // go to next group
              
              u8_curGrp++;
              mtrGrp.setGroup(u8_curGrp);

              if (u8_curGrp < mtrGrp.getNumGrps()) {
                ModbusRequest mbReq;
                mbReq = mtrGrp.getGroupRequest(SlaveData.isSlaveTcpByInd(u8_selSlv), SlaveData[u8_selSlv].u8_id, SlaveData[u8_selSlv].u8_vid);
                g_u16a_mbReqUnqId[u8_socket] = mbStack.add(mbReq, 2);  // PRIORITY 2!!
                Serial.print("group "); Serial.print(u8_curGrp, DEC);
                Serial.print(" request unique id: "); Serial.println(g_u16a_mbReqUnqId[u8_socket], DEC);
                return false;
              }
              else {  // this is the last group, hooray! - no modbus message here
                mtrGrp.groupLastFlags(s8a_dataFlags);

                liveXML(u8_socket, u8_selSlv, fa_liveXmlData, s8a_dataFlags);
                u8_curGrp = 0;
                return true;
              }
              
            }
            else {  // data has not yet been returned
              return false;
              break;
            }
          }
          break;
        }
        case FileReq_INFO:
          sendWebFile(g_eca_socks[u8_socket], ca_fileReq, FileType::XML, false);
          break;
        case FileReq_RESTART:
          sendWebFile(g_eca_socks[u8_socket], ca_fileReq, FileType::XML, false);

          g_eca_socks[u8_socket].stop();  // gently close socket for client
          resetArd();  // this will restart the gateway
          break;
        default:
          send404(g_eca_socks[u8_socket]);
          break;
        }
        break;
      case (FileType::TXT):
      case (FileType::CSV):
        sendWebFile(g_eca_socks[u8_socket], ca_fileReq, FileType::NONE);
        break;
      case (FileType::NONE):
        send404(g_eca_socks[u8_socket]);
        break;
      default:  // html, css, gif
        switch (u16_fileReq) {
        case (FileReq_EPSTYLE):
        case (FileReq_LOGO_LET):
        case (FileReq_INDEX):
        case (FileReq_GENSETUP):
        case (FileReq_MTRSETUP):
        case (FileReq_LIVE):
        case (FileReq_PASTVIEW):
        case (FileReq_RESET):
        case (FileReq_NOPAGE):
          sendWebFile(g_eca_socks[u8_socket], ca_fileReq, s16_fileType);
          break;
        case (FileReq_PASTDOWN):
          sendWebFile(g_eca_socks[u8_socket], "/pstdown1.htm", FileType::HTML, false);
          sendDownLinks(g_eca_socks[u8_socket], ca_fileReq);
          sendWebFile(g_eca_socks[u8_socket], "/pstdown2.htm", FileType::NONE, false);
          break;
        default:
          if (s16_fileType == FileType::HTML) {  // html page requested that it can't find
            sendWebFile(g_eca_socks[u8_socket], "/nopage.htm", FileType::HTML);
          }
          else {
            send404(g_eca_socks[u8_socket]);
          }
          break;
        }
        break;
      }
    }
    else {
      sendBadSD(g_eca_socks[u8_socket]);
    }
  }
  // POST http
  else if (u16_sockFlag & SockFlag_POST) {
    switch (u16_fileReq) {
    case (FileReq_GENSETUP):
    case (FileReq_MTRSETUP):
      Serial.println("post");
      getPostSetupData(g_eca_socks[u8_socket]);  // reads and stores POST data to EEPROM
      Serial.println("get data");
      sendPostResp(g_eca_socks[u8_socket]);
      Serial.println("send resp");
      break;
    default:
      send404(g_eca_socks[u8_socket]);
      break;
    }
  }
  else {
    send404(g_eca_socks[u8_socket]);
  }

  return true;
} // end handle_http
