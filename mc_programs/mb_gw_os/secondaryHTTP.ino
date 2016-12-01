/*
 * void flushEthRx
 * void send404
 * void sendBadSD
 * void sendWebFile
 * void LiveXML
 */


//void flushEthRx(EthernetClient52 &ec_client, uint8_t *u8p_buffer, uint16_t u16_length) {
//  while (ec_client.available()) {
//    ec_client.read(u8p_buffer, u16_length);
//  }
//}

// converts first line of http request to the file requested- overwrites char array, BE CAREFUL
void convertToFileName(char ca_fileReq[gk_u16_requestLineSize]) {
  uint16_t u16_strStart(0);
  char ca_dummy[gk_u16_requestLineSize];

  for (int ii = 0; ii < gk_u16_requestLineSize; ++ii) {
    if (ca_fileReq[ii] == 32) {
      u16_strStart = ii + 1;
      break;
    }
  }


  for (int ii = u16_strStart; ii < gk_u16_requestLineSize; ++ii) {
    switch (ca_fileReq[ii]) {
    case 32:  // space
    case 38:  // &
      ca_fileReq[ii] = 0;
      goto exitConvertForLoop;
      break;
    case 0:  // null char, string is done
      goto exitConvertForLoop;
      break;
    }
  }
exitConvertForLoop:

  strcpy(ca_dummy, ca_fileReq + u16_strStart);
  strcpy(ca_fileReq, ca_dummy);
  return;
}


void send404(EthernetClient52 &ec_client){
  char ca_resp404[44];

  strcpy_P(ca_resp404, PSTR("HTTP/1.1 404 Not Found\nConnnection: close\n\n"));
  ec_client.write(ca_resp404);

  ec_client.flush();
}


void sendBadSD(EthernetClient52 &ec_client){
  char ca_respBadSD[220];

  strcpy_P(ca_respBadSD, PSTR("HTTP/1.1 200 OK\nContent-Type: text/html\n")); // 41 with \0
  strcat_P(ca_respBadSD, PSTR("Connection: close\n\n"));  // 20 with \0

  ec_client.write(ca_respBadSD);

  strcpy_P(ca_respBadSD, PSTR("<!DOCTYPE html><html><head><title>"));// 35 with \0
  strcat(ca_respBadSD, g_c_gwName);
  strcat_P(ca_respBadSD, PSTR("</title></head><body><p>The SD card for this gateway did not initialize properly.  "));// 84 with \0
  strcat_P(ca_respBadSD, PSTR("Website functionality is not available at this time.</p></body></html>"));// 71 with \0

  ec_client.write(ca_respBadSD);

  ec_client.flush();
}


void sendWebFile(EthernetClient52 &ec_client, const char* k_cp_fileName, FileType en_fileType, bool b_addFileLength) {
  File streamFile = SD.open(k_cp_fileName);

  if (streamFile) {
    uint32_t u32_fileSize;                                   // size of file being sent over tcp
    char ca_streamBuf[gk_u16_respBuffSize];                       // buffer for moving data between sd card and ethernet

    u32_fileSize = streamFile.size();

    if (en_fileType != FileType::NONE) {
      uint32_t hdrLength;

      strcpy_P(ca_streamBuf, PSTR("HTTP/1.1 200 OK\nContent-Type: "));  // text / html\n"));
      switch (en_fileType) {
        case FileType::HTML:  // html
          strcat_P(ca_streamBuf, PSTR("text/html\n"));
          break;
        case FileType::CSS:  // css
          strcat_P(ca_streamBuf, PSTR("text/css\n"));
          break;
        case FileType::GIF:  // gif
          strcat_P(ca_streamBuf, PSTR("image/gif\n"));
          break;
        case FileType::XML:  // xml
          strcat_P(ca_streamBuf, PSTR("text/xml\n"));
          break;
        case FileType::CSV:  // csv - note, csvs for download are treated as NONE
          strcat_P(ca_streamBuf, PSTR("text/csv\n"));
          break;
        default:
          break;
      }

      if (b_addFileLength) {  // xml files have extra bits tacked on 
        strcat_P(ca_streamBuf, PSTR("Connection: close\nContent - Length: "));
        hdrLength = strlen(ca_streamBuf);
        sprintf(ca_streamBuf + hdrLength, "%lu\n\n", u32_fileSize);
      }
      else {
        strcat_P(ca_streamBuf, PSTR("Connection: close\n\n"));
      }
      

      //strcat_P(ca_streamBuf, PSTR("Connection: close\n\n"));
      
      ec_client.write(ca_streamBuf);
    }

    uint16_t u16_maxIter;                                    // number of chunks file must be split into to send via tcp

    u16_maxIter = (u32_fileSize / gk_u16_respBuffSize) + 1;

    for (int ii = 0; ii < u16_maxIter; ++ii) {
      uint32_t u32_remBytes;                                 // number of bytes remaining in last chunk (wfSize Mod STRM_BUF_SZ)
      uint16_t u16_bytesToSend;                                 // size of file buffer - min(remBytes, lclBufSz)

      u32_remBytes = u32_fileSize - (ii * gk_u16_respBuffSize);  // might be able to get rid of this as well, just use STRM_BUF_SZ, read should spit out early

      u16_bytesToSend = (u32_remBytes < gk_u16_respBuffSize) ? u32_remBytes : gk_u16_respBuffSize;
      
      streamFile.read(ca_streamBuf, u16_bytesToSend);  // expect speed increase
      
      ec_client.write(ca_streamBuf, u16_bytesToSend);
    }

    ec_client.flush();
    streamFile.close();
  }
  else {
    send404(ec_client);
  }

#if SHOW_FREE_MEM
  Serial.print(F("sendWebFile mem: "));
  Serial.println(getFreeMemory());
#endif
}


void sendDownLinks(EthernetClient52 &ec_client, const char *const k_ckp_firstLine) {
  uint16_t u16_dirNameLen;
  const uint16_t k_u16_truncReqLineSz = gk_u16_requestLineSize - 4;
  char ca_dirName[k_u16_truncReqLineSz];
  
  // request for folder contents looks like this:
  // GET /pastdown.htm/PASTDATA/2016/07   or
  // GET /pastdown.htm

  strcpy(ca_dirName, k_ckp_firstLine);  // +4 removed ("GET ")

  // cut down httpReq  (subtract out " HTTP/1.1...")
  /*for (int ii = 0; ii < k_u16_truncReqLineSz; ++ii) {
    if (ca_dirName[ii] == 32) {
      ca_dirName[ii] = 0;
      break;
    }
    else if (ca_dirName[ii] == 0) {
      break;
    }
  }*/
  
  if (strstr(ca_dirName, "PASTDATA") == nullptr) {  // can't find PASTDATA
    // add /PASTDATA, don't want to look in higher folders than this
    strcat_P(ca_dirName, PSTR("/PASTDATA"));
  }

  u16_dirNameLen = strlen(ca_dirName);

  if (ca_dirName[u16_dirNameLen - 1] != '/') {  // if the last character is not a /, add one
    strcat(ca_dirName, "/");  // not sure if trailing '/' will screw up .open()
    u16_dirNameLen++;
  }

  File dir = SD.open(ca_dirName + 13);  // do not include "/pastdown.htm" which is 13 chars long

  if (dir) {
    char ca_streamBuf[gk_u16_respBuffSize] = { 0 };                       // buffer for moving data to ethernet

    dir.rewindDirectory();  // resets library to top of directory

    if (u16_dirNameLen > 24) {  // if in directory lower than /PASTDATA, allow ../ option
      char ca_dirNameRed[k_u16_truncReqLineSz];

      strcpy(ca_dirNameRed, ca_dirName);
      for (int ii = (u16_dirNameLen - 2); ii > 1; --ii) {
        if (ca_dirNameRed[ii] == '/') {
          ca_dirNameRed[ii + 1] = 0;
          break;
        }
      }
      strcat_P(ca_streamBuf, PSTR("<a href=\""));
      strcat(ca_streamBuf, ca_dirNameRed);
      strcat_P(ca_streamBuf, PSTR("\">../</a></br>"));
    }
    
    while (true) {
      const uint16_t k_u16_minRemBytes(150);  // longest line should be 71 bytes, give bit more than twice padding
      File entry = dir.openNextFile();
      
      if (!entry) {
        break; 
      }

      if (entry.isDirectory()) {
        // write <a href="/pastdown.htm/CUR_DIR_NAME/DIR_NAME" download>DIR_NAME</a></br>
        strcat_P(ca_streamBuf, PSTR("<a href=\""));  // 9
        strcat(ca_streamBuf, ca_dirName);
        strcat(ca_streamBuf, entry.name());
        strcat_P(ca_streamBuf, PSTR("\">"));  // 2
        strcat(ca_streamBuf, entry.name());
        strcat_P(ca_streamBuf, PSTR("/</a></br>"));  // 10
      }
      else {
        // write <a href="CUR_DIR_NAME/FILE_NAME" download>FILE_NAME</a></br>
        strcat_P(ca_streamBuf, PSTR("<a href=\""));  // 9
        strcat(ca_streamBuf, ca_dirName + 13);  // current directory
        strcat(ca_streamBuf, entry.name());  // file
        strcat_P(ca_streamBuf, PSTR("\" download>"));  // 11
        strcat(ca_streamBuf, entry.name());  // file
        strcat_P(ca_streamBuf, PSTR("</a></br>"));  // 9
      }

      entry.close();

      // if remaining room is less than desired, send message 
      if ((gk_u16_respBuffSize - strlen(ca_streamBuf)) < k_u16_minRemBytes) {  
        ec_client.write(ca_streamBuf);
        ec_client.flush();

        ca_streamBuf[0] = 0;
      }
    }
    ec_client.write(ca_streamBuf);
    dir.close();
  }
}


void sendXmlEnd(EthernetClient52 &ec_client, XmlFile en_xmlType) {
  char ca_buffer[32];

  ca_buffer[0] = 0;

  switch (en_xmlType) {
    //case XmlFile::INFO:  // info.xml needs to know which slave to look at
    //  //strcat_P(ca_buffer, PSTR("<selMtr>"));  // 8
    //  //sprintf(ca_buffer + 8, "%u", g_u8a_selectedSlave);
    //  //strcat_P(ca_buffer, PSTR("</selMtr>"));  // 9
    //  // fall-through
    //case XmlFile::METER:  // mtrsetup.xml
    //  strcat_P(ca_buffer, PSTR("</meterList>"));
    //  ec_client.write(ca_buffer);
    //  break;
    case XmlFile::GENERAL: // gensetup.xml
      if (g_b_rtcGood) {
        time_t t = now();

        strcat_P(ca_buffer, PSTR("<rtc>"));
        sprintf(ca_buffer + 5, "%lu", t);
        strcat_P(ca_buffer, PSTR("</rtc>"));
      }

      strcat_P(ca_buffer, PSTR("</setup>"));
      ec_client.write(ca_buffer);
    default:
      break;
  }
}

void sendIP(EthernetClient52 &ec_client) {
  int jj;
  uint8_t u8_ipByte;
  char ca_postResp[15];  // largest that a ip address could be in string form

  jj = 0;
  u8_ipByte = g_ip_ip[0];
  for (int ii = log10(u8_ipByte); ii > -1; --ii) {
    ca_postResp[jj] = u8_ipByte / pow(10, ii) + '0';
    u8_ipByte -= (ca_postResp[jj] - '0') * pow(10, ii);
    jj++;
  }

  for (int kk = 1; kk < 4; ++kk) {
    ca_postResp[jj] = '.';
    jj++;

    u8_ipByte = g_ip_ip[kk];
    for (int ii = log10(u8_ipByte); ii > -1; --ii) {
      ca_postResp[jj] = u8_ipByte / pow(10, ii) + '0';
      u8_ipByte -= (ca_postResp[jj] - '0') * pow(10, ii);
      jj++;
    }
  }

  ec_client.write(ca_postResp, jj);
}


void liveXML(EthernetClient52 &ec_client, uint8_t u8a_selectedSlave) {  // sends xml file of live meter data
  const int k_i_maxNumElecVals(32);
  const int k_i_maxNumStmChwVals(10);
  const uint16_t k_u16_minRemBytes(50);
  float fa_data[k_i_maxNumElecVals];  // 32 is max number of value types that can be sent
  int8_t s8a_dataFlags[k_i_maxNumElecVals];
  uint8_t u8a_mbReq[12];  // can make this smaller 
  uint8_t u8a_mbResp[gk_u16_mbArraySize];
  uint16_t u16_reqLen(12), u16_respLen(0);
  uint16_t u16_numGrps;
  uint16_t u16_reqReg, u16_numRegs;
  uint8_t u8_mtrType, u8_mbVid, u8_mtrMbFunc;
  uint8_t u8_mbReqStat;
  char ca_respXml[gk_u16_respBuffSize] = {0};  // 68

  strcpy_P(ca_respXml, PSTR("HTTP/1.1 200 OK\nContent-Type: text/xml\nConnnection: close\n\n"));  // create http response

  u8_mtrType = g_u8a_slaveTypes[(u8a_selectedSlave - 1)][0];

  if ((u8_mtrType > EEPROM.read(g_u16_regBlkStart + 2)) || (u8_mtrType == 0)){  // check if meter higher than number of meter registers listed
    strcat_P(ca_respXml, PSTR("<?xml version = \"1.0\" ?><inputs><has_data>false</has_data></inputs>"));
    ec_client.write(ca_respXml);

    ec_client.flush();
    return;  // no registers in eeprom
  }
  u8_mbVid = g_u8a_slaveVids[(u8a_selectedSlave - 1)];  // getModbus accounts for vids

  u8_mtrMbFunc = EEPROM.read(g_u16_regBlkStart + 4 * u8_mtrType + 2);

  MeterLibGroups mtrGrps(u8_mtrType);
  u16_numGrps = mtrGrps.getNumGrps();

  memset(u8a_mbReq, 0, 5);
  u8a_mbReq[5] = 6;     // length of modbus half
  u8a_mbReq[6] = u8_mbVid;   // device id
  u8a_mbReq[7] = u8_mtrMbFunc;  // modbus function
  

  for (int ii = 1; ii < u16_numGrps; ++ii){  // the last group is filled with those data requests that cannot be filled
    mtrGrps.setGroup(ii);
    u16_reqReg = mtrGrps.getReqReg();
    u16_numRegs = mtrGrps.getNumRegs();

    u8a_mbReq[8] = highByte(u16_reqReg);
    u8a_mbReq[9] = lowByte(u16_reqReg);
    u8a_mbReq[10] = highByte(u16_numRegs);  // assume no length higher than 255
    u8a_mbReq[11] = lowByte(u16_numRegs);  // ask for float conversion = 2*num for registers

    delay(5); // ensure long enough delay between polls
    u8_mbReqStat = getModbus(u8a_mbReq, u16_reqLen, u8a_mbResp, u16_respLen, true);  // getModbus uses MB/TCP as inputs and outputs

    if (u8_mbReqStat == 0) {
      mtrGrps.groupToFloat(&u8a_mbResp[9], fa_data, s8a_dataFlags);
    }  // end if
    else {
      if (ii == 1){ // if first message and error, just dump failures
        strcat_P(ca_respXml, PSTR("<?xml version = \"1.0\" ?><inputs><has_data>false</has_data></inputs>"));
        ec_client.write(ca_respXml);
        ec_client.flush();
        return;  // if modbus timeout error on first loop, kill further data requests
      }
      mtrGrps.groupMbErr(s8a_dataFlags);
    } // end if
  }  // end for

  // last group full of duds (if any) ********************************************************************
  mtrGrps.setGroup(u16_numGrps);
  mtrGrps.groupLastFlags(s8a_dataFlags);
  // end handling of last group **************************************************************************

  // add to xml string, indicate that data is contained
  strcat_P(ca_respXml, PSTR("<?xml version = \"1.0\" ?><inputs><has_data>true</has_data>"));

  if ((u8_mtrType == 11) || (u8_mtrType == 12)){
    for (int ii = 0; ii < k_i_maxNumStmChwVals; ++ii) { 
      if (ii < 3) strcat_P(ca_respXml, PSTR("<flow>"));
      else if (ii < 4) strcat_P(ca_respXml, PSTR("<tmp1>"));
      else if (ii < 5) strcat_P(ca_respXml, PSTR("<tmp2>"));
      else if (ii < 6) strcat_P(ca_respXml, PSTR("<dlta>"));
      else if (ii < 7) strcat_P(ca_respXml, PSTR("<pres>"));
      else strcat_P(ca_respXml, PSTR("<engy>"));

      switch (s8a_dataFlags[ii]){
        case 1:  // good data
          dtostrf(fa_data[ii], 1, 2, (ca_respXml + strlen(ca_respXml)));
          break;
        case 0:  // data does not exist on hardware
//          strcat(respXml, "-");
          strcat_P(ca_respXml, PSTR("&#8212;"));
          break;
        case (-1):  // no modbus return
          strcat_P(ca_respXml, PSTR("error"));
          break;
        default:
          break;
      }    

      if (ii < 3) strcat_P(ca_respXml, PSTR("</flow>")); 
      else if (ii < 4) strcat_P(ca_respXml, PSTR("</tmp1>"));
      else if (ii < 5) strcat_P(ca_respXml, PSTR("</tmp2>"));
      else if (ii < 6) strcat_P(ca_respXml, PSTR("</dlta>"));
      else if (ii < 7) strcat_P(ca_respXml, PSTR("</pres>"));
      else strcat_P(ca_respXml, PSTR("</engy>"));

      if ((gk_u16_respBuffSize - strlen(ca_respXml)) < k_u16_minRemBytes) {
        ec_client.write(ca_respXml);  // write line of xml response every other time (stacks 2 together)
        ca_respXml[0] = 0;  // reset respXml to 0 length so strcat starts at beginning
      }
    }
  }
  else {  // electric meter
    for (int ii = 0; ii < k_i_maxNumElecVals; ++ii) {
      if (ii < 5){strcat_P(ca_respXml, PSTR("<curr>"));}
      else if (ii < 9){strcat_P(ca_respXml, PSTR("<v_LN>"));}
      else if (ii < 13){strcat_P(ca_respXml, PSTR("<v_LL>"));}
      else if (ii < 17){strcat_P(ca_respXml, PSTR("<rl_p>"));}
      else if (ii < 21){strcat_P(ca_respXml, PSTR("<rc_p>"));}
      else if (ii < 25){strcat_P(ca_respXml, PSTR("<ap_p>"));}
      else if (ii < 29){strcat_P(ca_respXml, PSTR("<p_fc>"));}
      else{strcat_P(ca_respXml, PSTR("<engy>"));}

      switch (s8a_dataFlags[ii]){
        case 1:  // good data
          dtostrf(fa_data[ii], 1, 2, (ca_respXml + strlen(ca_respXml)));
          break;
        case 0:  // data does not exist on hardware
//          strcat(respXml, "-");
          strcat_P(ca_respXml, PSTR("&#8212;"));  // long dash
          break;
        case (-1):  // no modbus return
          strcat_P(ca_respXml, PSTR("error"));
          break;
        default:
          break;
      }    


      if (ii < 5){strcat_P(ca_respXml, PSTR("</curr>"));}
      else if (ii < 9){strcat_P(ca_respXml, PSTR("</v_LN>"));}
      else if (ii < 13){strcat_P(ca_respXml, PSTR("</v_LL>"));}
      else if (ii < 17){strcat_P(ca_respXml, PSTR("</rl_p>"));}
      else if (ii < 21){strcat_P(ca_respXml, PSTR("</rc_p>"));}
      else if (ii < 25){strcat_P(ca_respXml, PSTR("</ap_p>"));}
      else if (ii < 29){strcat_P(ca_respXml, PSTR("</p_fc>"));}
      else{strcat_P(ca_respXml, PSTR("</engy>"));}


      if ((gk_u16_respBuffSize - strlen(ca_respXml)) < k_u16_minRemBytes) {
        ec_client.write(ca_respXml);  // write line of xml response
        ca_respXml[0] = 0;  // reset respXml to 0 length
      }
    }
  }

  strcat_P(ca_respXml, PSTR("</inputs>"));
  ec_client.write(ca_respXml);
  
  ec_client.flush();
}


