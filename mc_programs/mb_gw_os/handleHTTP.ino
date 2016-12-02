SockFlag readHttp(const uint8_t u8_socket, FileReq &u16_fileReq, FileType &s16_fileType, uint8_t &u8_selSlv, 
                  char ca_fileReq[gk_u16_requestLineSize]) {
  char ca_httpReqDummy[gk_u16_requestLineSize] = { 0 };
  //char ca_httpFirstLine[gk_u16_requestLineSize] = { 0 };
  //char ca_httpFullReq[gk_u16_requestBuffSize] = { 0 };

  uint16_t u16_lenRead(0);
  uint32_t u32_msgRecvTime;

  if (g_eca_socks[u8_socket].available()) {
    u16_lenRead = g_eca_socks[u8_socket].read((uint8_t*)ca_fileReq, gk_u16_requestLineSize - 1);
  }

  u32_msgRecvTime = millis();
  uint32_t k_u32_mesgTimeout(50);
  while (u16_lenRead < (gk_u16_requestLineSize - 1)) {  // make sure enough is read
    if (g_eca_socks[u8_socket].available()) {
      u16_lenRead += g_eca_socks[u8_socket].read((uint8_t*)ca_fileReq + u16_lenRead, gk_u16_requestLineSize - u16_lenRead - 1);
    }

    if ((u16_lenRead < (gk_u16_requestLineSize - 1)) && ((millis() - u32_msgRecvTime) > k_u32_mesgTimeout)) {  // stop trying to read message after 50 ms - assume it's never coming
      //g_eca_socks[u8_socket].stop();
      //Ethernet52.cleanSockets(80);
      return SockFlag_LISTEN;
    }
  }
  ca_fileReq[gk_u16_requestLineSize - 1] = 0;

  strcpy(ca_httpReqDummy, ca_fileReq);

  if (strncmp(ca_fileReq, "GET", 3) == 0) {
    // make sure string has zero at end for searches
    //for (int ii = 4; ii < gk_u16_requestLineSize; ++ii) {
    //  if (ca_fileReq[ii] == 32) {
    //    ca_fileReq[ii] = 0;
    //    break;
    //  }
    //  else if (ca_fileReq[ii] == 0) {
    //    break;
    //  }
    //}
    convertToFileName(ca_fileReq);

    Serial.print("GET: ");  Serial.println(ca_fileReq);
    //Serial.println(ca_httpReqDummy);

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
      else if (strstr(ca_fileReq, "live.htm") != nullptr) {
        uint8_t u8_meterType;                                     // type of meter, identifies register mapping in eeprom -> X.x.x
        char *cp_meterInd;                                     // index of 'METER' in GET request

        u8_selSlv = 0;
        cp_meterInd = strstr(ca_httpReqDummy, "METER=");  // the other string strips everything else off

        if (cp_meterInd != nullptr) {
          Serial.println("found METER=");
          char *cp_dumPtr;
          cp_meterInd += 6;  // move pointer to end of phrase 'METER='
          cp_dumPtr = cp_meterInd + 3;

          for (; cp_meterInd < cp_dumPtr; ++cp_meterInd) {
            if (isdigit(*cp_meterInd)) {
              u8_selSlv = u8_selSlv * 10 + ((*cp_meterInd) - '0');
            }
            else {
              break;
            }
          }

          if (u8_selSlv > EEPROM.read(g_u16_mtrBlkStart)) {  // if greater than number of meters listed
            u8_selSlv = 0;
          }
        }
        else {
          Serial.println("did not find METER=");
          u8_selSlv = 0;
        }

        u8_meterType = g_u8a_slaveTypes[u8_selSlv][0];  // turn meter from slave index to meter type X.x.x

        switch (u8_meterType) {
        case 11:
          u16_fileReq = FileReq_CLIVE;
          strcpy(ca_fileReq, "/clive.htm");
          break;
        case 12:
          u16_fileReq = FileReq_SLIVE;        // steam
          strcpy(ca_fileReq, "/slive.htm");
          break;
        default:
          u16_fileReq = FileReq_ELIVE;        // electric 
          strcpy(ca_fileReq, "/elive.htm");
        }
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
        //uint8_t u8_meterType;                                     // type of meter, identifies register mapping in eeprom -> X.x.x
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

          if (u8_selSlv > g_u8_numSlaves) {  // if greater than number of meters listed
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
        strcpy(ca_fileReq, "/gensetup.xml");
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

    //strcpy(ca_httpReq, ca_httpFirstLine + 4);  // account for "GET "
    return SockFlag_GET;
  }
  else if (strncmp(ca_fileReq, "POST", 4) == 0) {
    //for (int ii = 5; ii < gk_u16_requestLineSize; ++ii) {
    //  if (ca_fileReq[ii] == 32) {
    //    ca_fileReq[ii] = 0;
    //    break;
    //  }
    //  else if (ca_fileReq[ii] == 0) {
    //    break;
    //  }
    //}
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

    //if (strstr(ca_httpReq, "setup.htm")) {
    //  
    //}
    //strcpy(ca_httpReq, ca_httpFirstLine + 5);
    return SockFlag_POST;
  }
  return SockFlag_LISTEN;
}



bool respondHttp(const uint8_t u8_socket, const SockFlag u16_sockFlag, const FileReq u16_fileReq, const FileType s16_fileType, 
                 const uint8_t u8_selSlv, const char ca_fileReq[gk_u16_requestLineSize], ModbusStack &mbStack) {
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
        case FileReq_DATA:
          // LIVE XML - NEED MODBUS HANDLING HERE

          MeterLibGroups mtrGrp();

          // return false;  // return false so it knows nothing has been sent back yet
          break;
        case FileReq_INFO:
          sendWebFile(g_eca_socks[u8_socket], ca_fileReq, FileType::XML, false);
          //sendXmlEnd(g_eca_socks[u8_socket], XmlFile::INFO);
          break;
        case FileReq_RESTART:
          sendWebFile(g_eca_socks[u8_socket], ca_fileReq, FileType::XML, false);
          sendXmlEnd(g_eca_socks[u8_socket], XmlFile::GENERAL);

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
        case (FileReq_ELIVE):
        case (FileReq_SLIVE):
        case (FileReq_CLIVE):
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
    //if (strstr(ca_httpReq, "setup.htm")) {
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
