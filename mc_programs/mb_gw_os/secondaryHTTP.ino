/*
 * void flushEthRx
 * void send404
 * void sendBadSD
 * void sendWebFile
 * void LiveXML
 */


void flushEthRx(EthernetClient client, uint8_t * buffer, uint16_t len) {
  while (client.available()) {
    client.read(buffer, len);
  }
}


void send404(EthernetClient client){
  char resp404[44];

  strcpy_P(resp404, PSTR("HTTP/1.1 404 Not Found\nConnnection: close\n\n"));
  client.write(resp404);

  //post_cont.clear();
  client.flush();
}


void sendBadSD(EthernetClient client){
  char respBadSD[220];

  strcpy_P(respBadSD, PSTR("HTTP/1.1 200 OK\nContent-Type: text/html\n")); // 41 with \0
  strcat_P(respBadSD, PSTR("Connection: close\n\n"));  // 20 with \0

  client.write(respBadSD);

  
  strcpy_P(respBadSD, PSTR("<!DOCTYPE html><html><head><title>"));// 35 with \0
  strcat(respBadSD, meter_nm);
  strcat_P(respBadSD, PSTR("</title></head><body><p>The SD card for this gateway did not initialize properly.  "));// 84 with \0
  strcat_P(respBadSD, PSTR("Website functionality is not available at this time.</p></body></html>"));// 71 with \0

  client.write(respBadSD);

  //post_cont.clear();
  client.flush();
}

void sendGifHdr(EthernetClient client) {
  char respBadSD[100];

  strcpy_P(respBadSD, PSTR("HTTP/1.1 200 OK\nContent-Type: image/gif\n")); 
  strcat_P(respBadSD, PSTR("Connection: close\n\n"));
  client.write(respBadSD);
  client.flush();
}

void sendWebFile(EthernetClient client, const char* filename) {
  uint16_t i;                                        // tickers
  uint32_t wfSize;                                   // size of file being sent over tcp
  uint16_t max_i;                                    // number of chunks file must be split into to send via tcp
  uint32_t remBytes;                                 // number of bytes remaining in last chunk (wfSize Mod STRM_BUF_SZ)
  uint16_t lclBufSz;                                 // size of file buffer - min(remBytes, lclBufSz)
  char streamBuf[RESP_BUF_SZ];                       // buffer for moving data between sd card and ethernet
  File streamFile;

  streamFile = SD.open(filename);

  if (streamFile) {
    wfSize = streamFile.size();

    max_i = (wfSize / RESP_BUF_SZ) + 1;

    for (i = 0; i < max_i; i++) {
      remBytes = wfSize - (i * RESP_BUF_SZ);  // might be able to get rid of this as well, just use STRM_BUF_SZ, read should spit out early
      lclBufSz = min(remBytes, RESP_BUF_SZ);

      //for (j = 0; j < lclBufSz; j++) {
      //  streamBuf[j] = streamFile.read();
      //}
      streamFile.read(streamBuf, lclBufSz);  // expect speed increase
      
      client.write(streamBuf, lclBufSz);

    }

    client.flush();
    streamFile.close();
  }
  else {
    send404(client);
  }

#if SHOW_FREE_MEM
  Serial.print(F("sendWebFile mem: "));
  Serial.println(getFreeMemory());
#endif
}


void sendDownLinks(EthernetClient client, char* httpReq) {
  uint8_t i;
  uint16_t dirNameLen;
  char * pdPtr;
  char dirName[36];
  char streamBuf[RESP_BUF_SZ] = {0};                       // buffer for moving data to ethernet
  File dir;

  strcpy(dirName, httpReq + 4);
  dirName[35] = 0;


  // cut down httpReq  (subtract out " HTTP/1.1...")
  for (i = 0; i < 36; i++) {
    if (dirName[i] == 32) {
      dirName[i] = 0;
      break;
    }
    else if (dirName[i] == 0) {
      break;
    }
  }

  pdPtr = strstr(dirName, "PASTDATA");
  
  if (!pdPtr) {  // can't find PASTDATA
    strcat_P(dirName, PSTR("/PASTDATA"));
  }

  dirNameLen = strlen(dirName);
  if (dirName[dirNameLen - 1] != '/') {
    strcat(dirName, "/");  // not sure if trailing '/' will screw up .open()
    dirNameLen++;
  }

  dir = SD.open(dirName + 13);  // do not include "/pastdown.htm"
  /*Serial.write(dirName + 13);
  Serial.println();
  Serial.println();*/

  if (dir) {
    dir.rewindDirectory();

    if (dirNameLen > 24) {
      char dirNameRed[36];

      strcpy(dirNameRed, dirName);
      for (i = (dirNameLen - 2); i > 1; i--) {
        if (dirNameRed[i] == '/') {
          dirNameRed[i + 1] = 0;
          break;
        }
      }
      strcat_P(streamBuf, PSTR("<a href=\""));
      strcat(streamBuf, dirNameRed);
      strcat_P(streamBuf, PSTR("\">../</a></br>"));
    }
    
    i = 0;

    while (true) {
      File entry = dir.openNextFile();
      i++;
      if (!entry) {
        break; 
      }

      if (entry.isDirectory()) {
        strcat_P(streamBuf, PSTR("<a href=\""));
        strcat(streamBuf, dirName);
        strcat(streamBuf, entry.name());
        strcat_P(streamBuf, PSTR("\">"));
        strcat(streamBuf, entry.name());
        strcat_P(streamBuf, PSTR("/</a></br>"));
      }
      else {
        strcat_P(streamBuf, PSTR("<a href=\""));
        strcat(streamBuf, dirName + 13);
        strcat(streamBuf, entry.name());
        strcat_P(streamBuf, PSTR("\" download>"));
        strcat(streamBuf, entry.name());
        strcat_P(streamBuf, PSTR("</a></br>"));
      }

      entry.close();

      if (i > 9) {
        client.write(streamBuf);
        client.flush();

        streamBuf[0] = 0;
        i = 0;
      }
    }
    client.write(streamBuf);
    dir.close();
  }

}


void sendXmlEnd(EthernetClient client, uint8_t reqFlag) {
  char extraBuf[32];

  extraBuf[0] = 0;

  switch (reqFlag) {
    case 1:  // info.xml
      strcat_P(extraBuf, PSTR("<selMtr>"));
      sprintf(extraBuf + 8, "%u", selSlv);
      strcat_P(extraBuf, PSTR("</selMtr>"));
    case 2:  // mtrsetup.xml
      strcat_P(extraBuf, PSTR("</meterList>"));
      client.write(extraBuf);
      break;
    case 3: // gensetup.xml
      if (bGoodRTC) {
        time_t t = now();

        strcat_P(extraBuf, PSTR("<rtc>"));
        sprintf(extraBuf + 5, "%lu", t);
        strcat_P(extraBuf, PSTR("</rtc>"));
      }

      strcat_P(extraBuf, PSTR("</setup>"));
      client.write(extraBuf);
    default:
      break;
  }
}

void sendIP(EthernetClient client) {
  int16_t i;
  uint16_t j, k;
  uint8_t ipOct;
  char postResp[15];

  j = 0;
  ipOct = ip[0];
  for (i = log10(ipOct); i > -1; i--) {
    postResp[j] = ipOct / pow(10, i) + '0';
    ipOct -= (postResp[j] - '0') * pow(10, i);
    j++;
  }

  for (k = 1; k < 4; k++) {
    postResp[j] = '.';
    j++;

    ipOct = ip[k];
    for (i = log10(ipOct); i > -1; i--) {
      postResp[j] = ipOct / pow(10, i) + '0';
      ipOct -= (postResp[j] - '0') * pow(10, i);
      j++;
    }
  }

  client.write(postResp, j);
}


void liveXML(EthernetClient cl) {  // sends xml file of live meter data
  float data[32];
  int8_t data_b[32];
  uint8_t in_mb[12];  // can make this smaller 
  uint8_t out_mb[MB_ARR_SIZE];
  uint16_t in_len = 12;
  uint16_t out_len = 0;
  uint16_t lclmtr_strt, grp_strt, grp_adr, mb_strt, j, grp_len, i, clc_num, clc_typ;
  uint8_t meter, dev;
  bool mb_stat;
  char respXml[1024] = {0};  // 68
  uint8_t func;
  
  union convertUnion
  {
    float f;
    uint8_t u8[4];
  } int2flt;

  strcpy_P(respXml, PSTR("HTTP/1.1 200 OK\nContent-Type: text/xml\nConnnection: close\n\n"));  // create http response
  //Serial.print(F("selSlv: "));
  //Serial.println(selSlv);
  meter = slv_typs[(selSlv - 1)][0];
  //Serial.print(F("meter type: "));
  //Serial.println(meter);
  //meter = EEPROM.read(mtr_strt + selSlv * 8 - 7);
//  Serial.print(F("Mtr type: "));
//  Serial.println(meter, DEC);
  if ((meter > EEPROM.read(reg_strt + 2)) || (meter == 0)){  // check if meter higher than number of meter registers listed
    //cl.write(respXml);

    strcat_P(respXml, PSTR("<?xml version = \"1.0\" ?><inputs><has_data>false</has_data></inputs>"));
    cl.write(respXml);

    //post_cont.clear();
    cl.flush();
    return;  // no registers in eeprom
  }
  dev = slv_vids[(selSlv - 1)];  // getModbus accounts for vids
  //Serial.print(F("vid: "));
  //Serial.println(dev);
  func = EEPROM.read(reg_strt + 4 * meter + 2);
  lclmtr_strt = word(EEPROM.read(reg_strt + 4 * meter - 1), EEPROM.read(reg_strt + 4 * meter));
  
  grp_strt = word(EEPROM.read(lclmtr_strt + 3), EEPROM.read(lclmtr_strt + 4));
  grp_len = EEPROM.read(lclmtr_strt + 5);
  grp_adr = grp_strt;

  memset(in_mb, 0, 5);
  in_mb[5] = 6;     // length of modbus half
  in_mb[6] = dev;   // device id
  in_mb[7] = func;  // modbus function

  for (i = 0; i < (grp_len - 1); i++){  // the last group is filled with those data requests that cannot be filled
    clc_num = EEPROM.read(grp_adr);

    mb_strt = word(EEPROM.read(grp_adr + 1), EEPROM.read(grp_adr + 2)) + 10000;  // ask for float conversion
    in_mb[8] = highByte(mb_strt);
    in_mb[9] = lowByte(mb_strt);
//    Serial.print(F("Start"));
//    Serial.println(mb_strt, DEC);
    in_mb[10] = 0;  // assume no length higher than 255
    in_mb[11] = clc_num * 2;  // ask for float conversion = 2*num for registers

    delay(5); // ensure long enough delay between polls
    mb_stat = getModbus(in_mb, in_len, out_mb, out_len);

//    Serial.print(F("group: "));
//    Serial.print(i, DEC);
    if (mb_stat){
//      Serial.println(F(", has had successful modbus"));
      for (j = 0; j < clc_num; j++){  // shift 2 to get to collection type
        clc_typ = EEPROM.read(j + grp_adr + 3) - 1;

        int2flt.u8[3] = out_mb[(4 * j) + 11];
        int2flt.u8[2] = out_mb[(4 * j) + 12];
        int2flt.u8[1] = out_mb[(4 * j) + 9]; // high word
        int2flt.u8[0] = out_mb[(4 * j) + 10];  // low word

        data[clc_typ] = int2flt.f;
//        if (i == 0){
//          Serial.print("at ");
//          Serial.print(clc_typ, DEC);
//          Serial.print(" stored ");
//          Serial.print(int2flt.f);
//          Serial.print(", ");
//          Serial.print(int2flt.u8[3], BIN);
//          Serial.print(", ");
//          Serial.print(int2flt.u8[2], BIN);
//          Serial.print(", ");
//          Serial.print(int2flt.u8[1], BIN);
//          Serial.print(", ");
//          Serial.println(int2flt.u8[0], BIN);
//        }
        
        data_b[clc_typ] = 1;  // successful read
      }  // end for
    }  // end if
    else{
      //Serial.print(F(", has had MB failure: "));
      //Serial.print(mb_stat, HEX);
      //Serial.print(F("  "));
      if (i == 0){ // if first message and timeout, just dump failures
        //Serial.println(F("Sending false xml"));

        //cl.write(respXml);
        
        strcat_P(respXml, PSTR("<?xml version = \"1.0\" ?><inputs><has_data>false</has_data></inputs>"));
        cl.write(respXml);

        //post_cont.clear();
        cl.flush();
        return;  // if modbus timeout error on first loop, kill further data requests
      }
//      Serial.println(F("Continuing..."));
      for (j = 0; j < clc_num; j++){ 
        clc_typ = EEPROM.read(j + grp_adr + 3) - 1;
//        Serial.print("for group adr ");
//        Serial.print(grp_adr, DEC);
//        Serial.print(" clc_typ = ");
//        Serial.println(clc_typ);
        data_b[clc_typ] = -1;  // unsuccessful read
      }  // end for
    } // end if

    grp_adr = 3 + 2 * (clc_num) + grp_adr;  // starting address of next group

  }  // end for
  // last group full of duds (if any)
  clc_num = EEPROM.read(grp_adr);  

  for (j = 0; j < clc_num; j++){  // shift 2 to get to collection type
    clc_typ = EEPROM.read(j + grp_adr + 3) - 1;

    data_b[clc_typ] = 0;  // data does not exist on this hardware
  }  // end for

  //cl.write(respXml);

  strcat_P(respXml, PSTR("<?xml version = \"1.0\" ?><inputs><has_data>true</has_data>"));
  //cl.write(respXml);

  //respXml[0] = 0;  // set first byte to 0 
  if ((meter == 11) || (meter == 12)){
    for (i = 0; i < 10; i++)
    {
      if (i < 3){strcat_P(respXml, PSTR("<flow>"));}
      else if (i < 4){strcat_P(respXml, PSTR("<tmp1>"));}
      else if (i < 5){strcat_P(respXml, PSTR("<tmp2>"));}
      else if (i < 6){strcat_P(respXml, PSTR("<dlta>"));}
      else if (i < 7){strcat_P(respXml, PSTR("<pres>"));}
      else{strcat_P(respXml, PSTR("<engy>"));}

      switch (data_b[i]){
        case 1:  // good data
          dtostrf(data[i], 1, 2, (respXml + strlen(respXml)));
          break;
        case 0:  // data does not exist on hardware
//          strcat(respXml, "-");
          strcat_P(respXml, PSTR("&#8212;"));
          break;
        case (-1):  // no modbus return
          strcat_P(respXml, PSTR("error"));
          break;
        default:
          break;
      }    

      if (i < 3){strcat_P(respXml, PSTR("</flow>"));}
      else if (i < 4){strcat_P(respXml, PSTR("</tmp1>"));}
      else if (i < 5){strcat_P(respXml, PSTR("</tmp2>"));}
      else if (i < 6){strcat_P(respXml, PSTR("</dlta>"));}
      else if (i < 7){strcat_P(respXml, PSTR("</pres>"));}
      else{strcat_P(respXml, PSTR("</engy>"));}

      if (strlen(respXml) > 984) {
      //if ((i % 2) == 1){
        cl.write(respXml);  // write line of xml response every other time (stacks 2 together)
        respXml[0] = 0;  // reset respXml to 0 length so strcat starts at beginning
      }
    }
  }
  else{
    for (i = 0; i < 32; i++)
    {
      if (i < 5){strcat_P(respXml, PSTR("<curr>"));}
      else if (i < 9){strcat_P(respXml, PSTR("<v_LN>"));}
      else if (i < 13){strcat_P(respXml, PSTR("<v_LL>"));}
      else if (i < 17){strcat_P(respXml, PSTR("<rl_p>"));}
      else if (i < 21){strcat_P(respXml, PSTR("<rc_p>"));}
      else if (i < 25){strcat_P(respXml, PSTR("<ap_p>"));}
      else if (i < 29){strcat_P(respXml, PSTR("<p_fc>"));}
      else{strcat_P(respXml, PSTR("<engy>"));}
      
      switch (data_b[i]){
        case 1:  // good data
          dtostrf(data[i], 1, 2, (respXml + strlen(respXml)));
          break;
        case 0:  // data does not exist on hardware
//          strcat(respXml, "-");
          strcat_P(respXml, PSTR("&#8212;"));
          break;
        case (-1):  // no modbus return
          strcat_P(respXml, PSTR("error"));
          break;
        default:
          break;
      }    

      if (i < 5){strcat_P(respXml, PSTR("</curr>"));}
      else if (i < 9){strcat_P(respXml, PSTR("</v_LN>"));}
      else if (i < 13){strcat_P(respXml, PSTR("</v_LL>"));}
      else if (i < 17){strcat_P(respXml, PSTR("</rl_p>"));}
      else if (i < 21){strcat_P(respXml, PSTR("</rc_p>"));}
      else if (i < 25){strcat_P(respXml, PSTR("</ap_p>"));}
      else if (i < 29){strcat_P(respXml, PSTR("</p_fc>"));}
      else{strcat_P(respXml, PSTR("</engy>"));}

      if (strlen(respXml) > 984) {
        //if ((i % 2) == 1){
        cl.write(respXml);  // write line of xml response
        respXml[0] = 0;  // reset respXml to 0 length
      }
    }
  }

  strcat_P(respXml, PSTR("</inputs>"));
  cl.write(respXml);
  
  //post_cont.clear();
  cl.flush();
}


