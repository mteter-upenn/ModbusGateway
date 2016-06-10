/*
 * void send404
 * void sendBadSD
 * void sendWebFile
 * void sendPostResp
 * void LiveXML
 * void getPostSetupData
 */



void send404(EthernetClient client){
  char resp404[44];

  strcpy_P(resp404, PSTR("HTTP/1.1 404 Not Found\nConnnection: close\n\n"));
  client.write(resp404);

  post_cont.clear();
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

  post_cont.clear();
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
  uint16_t i, j;                                     // tickers
  uint32_t wfSize;                                   // size of file being sent over tcp
  uint16_t max_i;                                    // number of chunks file must be split into to send via tcp
  uint32_t remBytes;                                 // number of bytes remaining in last chunk (wfSize Mod STRM_BUF_SZ)
  uint16_t lclBufSz;                                 // size of file buffer - min(remBytes, lclBufSz)
  char streamBuf[STRM_BUF_SZ];                       // buffer for moving data between sd card and ethernet
  File streamFile;

  streamFile = SD.open(filename);

  if (streamFile) {
    wfSize = streamFile.size();

    max_i = (wfSize / STRM_BUF_SZ) + 1;

    for (i = 0; i < max_i; i++) {
      remBytes = wfSize - (i * STRM_BUF_SZ);  // might be able to get rid of this as well, just use STRM_BUF_SZ, read should spit out early
      lclBufSz = min(remBytes, STRM_BUF_SZ);

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
}


void sendDownLinks(EthernetClient client, char* httpReq) {
  uint8_t i;
  uint16_t dirNameLen;
  char * pdPtr;
  char dirName[36];
  char streamBuf[STRM_BUF_SZ] = {0};                       // buffer for moving data to ethernet
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


void sendPostResp(EthernetClient client){
  //int16_t i;
  //uint16_t j, k;
  //uint8_t ipOct;
  char postResp[76]; // = "HTTP/1.1 303 See Other\nLocation: http://";
//  char postClose[] = "\nConnection: close\n\n";

  strcpy_P(postResp, PSTR("HTTP/1.1 303 See Other\nLocation: /redirect.htm\nConnection: close\n\n"));
  client.write(postResp, 66);

  /*strcpy_P(postResp, PSTR("HTTP/1.1 303 See Other\nLocation: http://"));
  j = 40;
  ipOct = ip[0];
  for (i = log10(ipOct); i > -1; i--){
    postResp[j] = ipOct / pow(10, i) + '0';
    ipOct -= (postResp[j] - '0') * pow(10, i);
    j++;
  }

  for (k = 1; k < 4; k++){
    postResp[j] = '.';
    j++;

    ipOct = ip[k];
    for (i = log10(ipOct); i > -1; i--){
      postResp[j] = ipOct / pow(10, i) + '0';
      ipOct -= (postResp[j] - '0') * pow(10, i);
      j++;
    }
  }

  strcat(postResp, PSTR("\nConnection: close\n\n"));
  
  client.write(postResp, j);*/
  
  post_cont.clear();
  client.flush();
}


void liveXML(EthernetClient cl){
  float data[32];
  int8_t data_b[32];
  uint8_t in_mb[12];  // can make this smaller 
  uint8_t out_mb[ARR_SIZE];
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

    post_cont.clear();
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

        post_cont.clear();
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
  
  post_cont.clear();
  cl.flush();
}


void getPostSetupData(EthernetClient cl, uint16_t pst_len){
  uint16_t i, j, k;
  uint16_t id_strt, id_end, val_strt, val_end;
  char post_str[pst_len];
  char ch;
  uint8_t u8dum, lcl_meter;
  uint8_t num_mtrs = 0;
  uint16_t u16dum;
  uint32_t u32dum;

  for (i = 0; i < pst_len; i++){
    if (cl.available()){
      post_str[i] = cl.read();
      //Serial.print(post_str[i]);
    }
    else{
      return;  // false
    }
  }
  //Serial.println();

  id_strt = 0;
  i = 0;
  digitalWrite(epWriteLed, HIGH);
//  Serial.println(F("post setup"));
  while (i < pst_len){
    ch = post_str[i];
//    Serial.print(F("ch: "));
//    Serial.println(ch);
    if (ch == '='){
      id_end = i;
      i++;
      val_strt = i;

      ch = post_str[i];

      while ((i < pst_len) && (ch != '&')){
        i++;
        ch = post_str[i];
      }
      val_end = i;

      
      if (strncmp(post_str + id_strt, "mip", 3) == 0){  //  ****************************************** METER IP *************************************************
        lcl_meter = 0;
        for (j = (id_strt + 3); j < id_end; j++){
          lcl_meter = lcl_meter * 10 + (post_str[j] - '0');
        }

        if (lcl_meter < num_mtrs){  // make sure to only record necessary number of meters
          if (val_strt == val_end){  // blank values for ips translate to 0's
            for (j = 0; j < 4; j++){
              EEPROM.write((j + mtr_strt + 9 * lcl_meter + 4), 0);
            }
          }
          else{
            k = val_strt;
            for (j = 0; j < 4; j++){
              u8dum = 0;
              while ((post_str[k] != '.') && (k < val_end)){
                u8dum = u8dum * 10 + (post_str[k] - '0');
                k++;
              }
              k++;
              EEPROM.write((j + mtr_strt + 9 * lcl_meter + 4), u8dum);
            }
          }
        }
      }
      else if (strncmp(post_str + id_strt, "id", 2) == 0){  //  ****************************************** MODBUS ID *************************************************
        lcl_meter = 0;
        for (j = (id_strt + 2); j < id_end; j++){
          lcl_meter = lcl_meter * 10 + (post_str[j] - '0');
        }

        if (lcl_meter < num_mtrs){  // make sure to only record necessary number of meters
          u8dum = 0;
          for (j = val_strt; j < val_end; j++){
            u8dum = u8dum * 10 + (post_str[j] - '0');
          }
          EEPROM.write((mtr_strt + 9 * lcl_meter + 8), u8dum);
//          Serial.print(lcl_meter, DEC);
//          Serial.print(F(": "));
//          Serial.println(u8dum, DEC);
        }
      }
      else if (strncmp(post_str + id_strt, "vid", 3) == 0) {  //  ************************************ VIRTUAL MODBUS ID *************************************************
        lcl_meter = 0;
        for (j = (id_strt + 3); j < id_end; j++) {
          lcl_meter = lcl_meter * 10 + (post_str[j] - '0');
        }

        if (lcl_meter < num_mtrs) {  // make sure to only record necessary number of meters
          u8dum = 0;
          for (j = val_strt; j < val_end; j++) {
            u8dum = u8dum * 10 + (post_str[j] - '0');
          }
          EEPROM.write((mtr_strt + 9 * lcl_meter + 9), u8dum);
          //          Serial.print(lcl_meter, DEC);
          //          Serial.print(F(": "));
          //          Serial.println(u8dum, DEC);
        }
      }
      else if (strncmp(post_str + id_strt, "mtr", 3) == 0){  //  ****************************************** METER TYPE *************************************************
        lcl_meter = 0;
        for (j = (id_strt + 3); j < id_end; j++){
          lcl_meter = lcl_meter * 10 + (post_str[j] - '0');
        }

        if (lcl_meter < num_mtrs){  // make sure to only record necessary number of meters
          k = val_strt;
          for (j = 0; j < 3; j++){
            u8dum = 0;
            while ((post_str[k] != '.') && (k < val_end)){
              u8dum = u8dum * 10 + (post_str[k] - '0');
              k++;
            }
            k++;
            EEPROM.write((j + mtr_strt + 9 * lcl_meter + 1), u8dum);
          }
        }
      }
      else if (strncmp(post_str + id_strt, "numMtrs", 7) == 0){  //  ****************************************** NUM METERS *************************************************
        for (j = val_strt; j < val_end; j++){
          num_mtrs = num_mtrs * 10 + (post_str[j] - '0');
        }

        if (num_mtrs > 20){
          num_mtrs = 20;
        }

        EEPROM.write(mtr_strt, num_mtrs);
      }
      else if (strncmp(post_str + id_strt, "nm", 2) == 0){  // ******************************** NAME ****************************************
//        Serial.println(F("nm"));
        if ((val_end - val_strt) > 30){
          val_end = val_strt + 30;
        }
        for (j = val_strt; j < val_end; j++){  // limited to 30 characters
          if (post_str[j] == 43){  // filter out '+' as html concatenator
            post_str[j] = 32;  // replace with blank space
          }
          EEPROM.write((j - val_strt + nm_strt), post_str[j]);
        }
        if ((val_end - val_strt) != 30){
          EEPROM.write((val_end - val_strt + nm_strt), 0);
        }
      }
      else if (strncmp(post_str + id_strt, "rd", 2) == 0) {  // ***************************************** Record Data ************************************
        u16dum = 0;
        for (j = val_strt; j < val_end; j++){
          u16dum = u16dum * 10 + (post_str[j] - '0');
        }

        if (u16dum) {
          EEPROM.write(nm_strt + 31, true);
        }
        else {
          EEPROM.write(nm_strt + 31, false);
        }
      }
      else if (strncmp(post_str + id_strt, "ms", 2) == 0) {  // ***************************** Max Number of Slaves to Record ************************************
        u8dum = 0;
        for (j = val_strt; j < val_end; j++) {
          u8dum = u8dum * 10 + (post_str[j] - '0');
        }

        EEPROM.write(nm_strt + 32, u8dum);
      }      else if (strncmp(post_str + id_strt, "ip", 2) == 0){  //  ***************************************** IP ************************************************
//        Serial.println(F("ip"));
        k = val_strt;
        for (j = 0; j < 4; j++){
          u8dum = 0;
          while ((post_str[k] != '.') && (k < val_end)){
            u8dum = u8dum * 10 + (post_str[k] - '0');
            k++;
          }
          k++;
          EEPROM.write((j + ip_strt + 6), u8dum);
        }
      }
      else if (strncmp(post_str + id_strt, "sm", 2) == 0){  //  ****************************************** SUBNET MASK *************************************************
//        Serial.println(F("sm"));
        k = val_strt;
        for (j = 0; j < 4; j++){
          u8dum = 0;
          while ((post_str[k] != '.') && (k < val_end)){
            u8dum = u8dum * 10 + (post_str[k] - '0');
            k++;
          }
          k++;
          EEPROM.write((j + ip_strt + 10), u8dum);
        }
      }
      else if (strncmp(post_str + id_strt, "gw", 2) == 0){  //  ****************************************** DEFAULT GATEWAY *************************************************
//        Serial.println(F("gw"));
        k = val_strt;
        for (j = 0; j < 4; j++){
          u8dum = 0;
          while ((post_str[k] != '.') && (k < val_end)){
            u8dum = u8dum * 10 + (post_str[k] - '0');
            k++;
          }
          k++;
          EEPROM.write((j + ip_strt + 14), u8dum);
        }
      }
      else if (strncmp(post_str + id_strt, "ntp", 3) == 0) {  // ***************************************** USE NTP? ************************************
        u16dum = 0;
        for (j = val_strt; j < val_end; j++) {
          u16dum = u16dum * 10 + (post_str[j] - '0');
        }

        if (u16dum) {
          EEPROM.write(ip_strt + 18, true);
        }
        else {
          EEPROM.write(ip_strt + 18, false);
        }
      }
      else if (strncmp(post_str + id_strt, "nip", 3) == 0) {  //  ************************************ NTP SERVER IP *****************************************
        k = val_strt;
        for (j = 0; j < 4; j++) {
          u8dum = 0;
          while ((post_str[k] != '.') && (k < val_end)) {
            u8dum = u8dum * 10 + (post_str[k] - '0');
            k++;
          }
          k++;
          EEPROM.write((j + ip_strt + 19), u8dum);
        }
      }
      else if (strncmp(post_str + id_strt, "br", 2) == 0){  //  ****************************************** BAUDRATE *************************************************
//        Serial.println(F("br"));
        u32dum = 0;
        
        for (j = val_strt; j < val_end; j++){
          u32dum = u32dum * 10 + (post_str[j] - '0');
        }

        EEPROM.write(ip_strt + 23, (u32dum >> 16));
        EEPROM.write(ip_strt + 24, (u32dum >> 8));
        EEPROM.write(ip_strt + 25, u32dum);
      }
      else if (strncmp(post_str + id_strt, "to", 2) == 0){  //  ****************************************** MB TIMEOUT *************************************************
//        Serial.println(F("to"));
        u16dum = 0;
        for (j = val_strt; j < val_end; j++){
          u16dum = u16dum * 10 + (post_str[j] - '0');
        }

        EEPROM.write(ip_strt + 26,highByte(u16dum));
        EEPROM.write(ip_strt + 27, lowByte(u16dum));
      }
      else if (strncmp(post_str + id_strt, "tm", 2) == 0){  //  ****************************************** TIME *************************************************
         u32dum = 0;
        
        for (j = val_strt; j < val_end; j++){
          u32dum = u32dum * 10 + (post_str[j] - '0');
        }

        if (u32dum > 1451606400UL) {
          bGoodRTC = true;
#if defined(CORE_TEENSY)
          Teensy3Clock.set(u32dum);
#endif
          setTime(u32dum);
        }
      }
      i++;
      id_strt = i;
    }  // end if ch == '='
    else{
      i++;
    }  // end if ch == '='
  }  // while loop

  digitalWrite(epWriteLed, LOW);
  setConstants();
}
