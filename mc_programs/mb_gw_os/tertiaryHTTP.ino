/*
* void sendPostResp
* char * preprocPost
* void getPostSetupData
*/



void sendPostResp(EthernetClient52 client) {
  char postResp[76]; // = "HTTP/1.1 303 See Other\nLocation: http://";

  strcpy_P(postResp, PSTR("HTTP/1.1 303 See Other\nLocation: /redirect.htm\nConnection: close\n\n"));
  client.write(postResp, 66);

  client.flush();
}


char * preprocPost(EthernetClient52 client, char * headHttp, uint16_t &postLen) {
  char * msgPtr;
  int16_t lenRead;
  uint16_t i;
  bool foundPtr = false;
  const char contLen[] = "Content-Length: ";
  const char escHead[] = "\r\n\r\n";
  uint16_t charMatches = 0;

  postLen = 0;

  lenRead = client.read((uint8_t*)headHttp, POST_BUF_SZ); // read out to POST_BUF_SZ
  headHttp[lenRead] = 0;

  while (!foundPtr) {
    msgPtr = strstr(headHttp, "Content-Length: ");  // Find 'Content-Length: '

    if (msgPtr != NULL) {
      msgPtr += 16;  // add 16 to get to end of phrase
      foundPtr = true;  // exit condition
    }
    else {  // check to see if phrase was cut in 2
      msgPtr = headHttp + lenRead - 15;  // stick msgPtr at end of read message

      for (; msgPtr < headHttp + lenRead; msgPtr++) {
        for (i = 0; i < 16; i++) {
          if ((msgPtr + i) == (headHttp + lenRead)) {  
            // end of msgPtr, // match up to previous point
            charMatches = i;
          }
          else if (*(msgPtr + i) == contLen[i]) {
            // continue to match headHttp and contLen
          }
          else {
            break;  // mismatch
          }
        }

        if (charMatches) {  // a match has been found, no reason to keep looking
          break;
        }
      }

      lenRead = client.read((uint8_t*)headHttp, POST_BUF_SZ); // read out to POST_BUF_SZ
      headHttp[lenRead] = 0;

      if (charMatches) {
        // try to match beginning of headHttp with remaining chars of contLen
        for (i = charMatches; i < 16; i++) {
          if (contLen[i] == headHttp[i - charMatches]) {
            if (i == 15) {
              foundPtr = true;  // escape condition
              msgPtr = headHttp + 16 - charMatches;  // end of phrase
            }
          }
          else {
            charMatches = 0;
            break;
          }
        }
      }  // if charMatches
    }  // else pointer is NULL
  }  // while haven't found pointer

  // after 'Content-Length: ' is found, determine the postLen
  for (i = 0; i < 2; i++) {  // used for loop instead just to be sure it breaks free even if I get nothing out of it
    while (isdigit(*msgPtr)) {
      postLen = postLen * 10 + ((*msgPtr) - '0');
      msgPtr++;
    }

    if ((*msgPtr) == '\0') {
      client.read((uint8_t*)headHttp, POST_BUF_SZ); // length was cut off, load next portion into headHttp
      msgPtr = headHttp;  // reset postLenPtr to start of headHttp
    }
    else {
      break;  // breaks for loop
    }
  }

  // now find '\r\n\r\n'  -  this escapes from the http header
  foundPtr = false;
  charMatches = false;

  while (!foundPtr) {
    msgPtr = strstr(headHttp, escHead);

    if (msgPtr != NULL) {  
      msgPtr += 4;  // add 4 to get to end of phrase
      foundPtr = true;  // exit condition
    }
    else {  // check to see if phrase was cut in 2
      msgPtr = headHttp + lenRead - 3;  // stick msgPtr at end of read message

      for (; msgPtr < headHttp + lenRead; msgPtr++) {
        for (i = 0; i < 4; i++) {
          if ((msgPtr + i) == (headHttp + lenRead)) {
            // end of msgPtr, // match up to previous point
            charMatches = i;
          }
          else if (*(msgPtr + i) == escHead[i]) {
            // continue to match headHttp and contLen
          }
          else {
            break;  // mismatch
          }
        }

        if (charMatches) {  // a match has been found, no reason to keep looking
          break;
        }
      }

      lenRead = client.read((uint8_t*)headHttp, POST_BUF_SZ); // read out to POST_BUF_SZ
      headHttp[lenRead] = 0;

      if (charMatches) {
        // try to match beginning of headHttp with remaining chars of escHead
        for (i = charMatches; i < 4; i++) {
          if (escHead[i] == headHttp[i - charMatches]) {
            if (i == 3) {
              foundPtr = true;  // escape condition
              msgPtr = headHttp + 3 - charMatches;  // end of phrase
            }
          }
          else {
            charMatches = 0;
            break;
          }
        }
      }  // if charMatches
    }  // else pointer is NULL
  }  // while haven't found pointer

  headHttp[lenRead + client.read((uint8_t*)headHttp + lenRead, REQ_ARR_SZ - lenRead)] = 0;
  return msgPtr;
}


void getPostSetupData(EthernetClient52 cl, char * headHttp) {
  uint16_t  j;
  char * id_strt, *id_end, *val_strt, *val_end;        // pointers used for setting barriers around values and identifiers
  char * chPtr;                                        // dummy pointer for loops
  uint8_t u8dum, lcl_meter;
  uint8_t num_mtrs = 0;
  uint16_t u16dum;
  uint32_t u32dum;
  bool readingMsg = true;
  uint16_t postLen;                                    // length of message given in header
  uint16_t totLen;                                     // length of message actually in headHttp
  char * postMsgPtr;                                   // pointer to beginning of message

  //Serial.println("start preproc");
  postMsgPtr = preprocPost(cl, headHttp, postLen);     // get length of post message and place pointer at its start
  //Serial.println("exited preproc");
  
  totLen = strlen(headHttp) - (postMsgPtr - headHttp);

  id_strt = postMsgPtr;

  digitalWrite(epWriteLed, HIGH);

  while (readingMsg) {
    if ((*postMsgPtr) != '\0') {
      if ((*postMsgPtr) == '=') {
        id_end = postMsgPtr;
        postMsgPtr++;
        val_strt = postMsgPtr;

        while (((*postMsgPtr) != '&') && ((*postMsgPtr) != '\0')) {  // not sure how to handle if '\0' pops up with more message to be read
          postMsgPtr++;
        }

        val_end = postMsgPtr;

        if (strncmp(id_strt, "mip", 3) == 0) {  //  ****************************************** METER IP *************************************************
          lcl_meter = 0;
          for (chPtr = (id_strt + 3); chPtr < id_end; chPtr++) {
            lcl_meter = lcl_meter * 10 + ((*chPtr) - '0');
          }

          if (lcl_meter < num_mtrs) {  // make sure to only record necessary number of meters
            if (val_strt == val_end) {  // blank values for ips translate to 0's
              for (j = 0; j < 4; j++) {
                EEPROM.write((j + mtr_strt + 9 * lcl_meter + 4), 0);
              }
            }
            else {
              chPtr = val_strt;
              for (j = 0; j < 4; j++) {
                u8dum = 0;
                while (((*chPtr) != '.') && (chPtr < val_end)) {
                  u8dum = u8dum * 10 + ((*chPtr) - '0');
                  chPtr++;
                }
                chPtr++;
                EEPROM.write((j + mtr_strt + 9 * lcl_meter + 4), u8dum);
              }
            }
          }
        }
        else if (strncmp(id_strt, "id", 2) == 0) {  //  ****************************************** MODBUS ID *************************************************
          lcl_meter = 0;
          for (chPtr = (id_strt + 2); chPtr < id_end; chPtr++) {
            lcl_meter = lcl_meter * 10 + ((*chPtr) - '0');
          }

          if (lcl_meter < num_mtrs) {  // make sure to only record necessary number of meters
            u8dum = 0;
            for (chPtr = val_strt; chPtr < val_end; chPtr++) {
              u8dum = u8dum * 10 + ((*chPtr) - '0');
            }
            EEPROM.write((mtr_strt + 9 * lcl_meter + 8), u8dum);
            //          Serial.print(lcl_meter, DEC);
            //          Serial.print(F(": "));
            //          Serial.println(u8dum, DEC);
          }
        }
        else if (strncmp(id_strt, "vid", 3) == 0) {  //  ************************************ VIRTUAL MODBUS ID *************************************************
          lcl_meter = 0;
          for (chPtr = (id_strt + 3); chPtr < id_end; chPtr++) {
            lcl_meter = lcl_meter * 10 + ((*chPtr) - '0');
          }

          if (lcl_meter < num_mtrs) {  // make sure to only record necessary number of meters
            u8dum = 0;
            for (chPtr = val_strt; chPtr < val_end; chPtr++) {
              u8dum = u8dum * 10 + ((*chPtr) - '0');
            }
            EEPROM.write((mtr_strt + 9 * lcl_meter + 9), u8dum);
            //          Serial.print(lcl_meter, DEC);
            //          Serial.print(F(": "));
            //          Serial.println(u8dum, DEC);
          }
        }
        else if (strncmp(id_strt, "mtr", 3) == 0) {  //  ****************************************** METER TYPE *************************************************
          lcl_meter = 0;
          for (chPtr = (id_strt + 3); chPtr < id_end; chPtr++) {
            lcl_meter = lcl_meter * 10 + ((*chPtr) - '0');
          }

          if (lcl_meter < num_mtrs) {  // make sure to only record necessary number of meters
            chPtr = val_strt;
            for (j = 0; j < 3; j++) {
              u8dum = 0;
              while (((*chPtr) != '.') && (chPtr < val_end)) {
                u8dum = u8dum * 10 + ((*chPtr) - '0');
                chPtr++;
              }
              chPtr++;
              EEPROM.write((j + mtr_strt + 9 * lcl_meter + 1), u8dum);
            }
          }
        }
        else if (strncmp(id_strt, "numMtrs", 7) == 0) {  //  ****************************************** NUM METERS *************************************************
          for (chPtr = val_strt; chPtr < val_end; chPtr++) {
            num_mtrs = num_mtrs * 10 + ((*chPtr) - '0');
          }

          if (num_mtrs > 20) {
            num_mtrs = 20;
          }

          EEPROM.write(mtr_strt, num_mtrs);
        }
        else if (strncmp(id_strt, "nm", 2) == 0) {  // ******************************** NAME ****************************************
          //Serial.println(F("nm"));

          if ((val_end - val_strt) > 30) {
            val_end = val_strt + 30;
          }
          for (chPtr = val_strt; chPtr < val_end; chPtr++) {  // limited to 30 characters
            if ((*chPtr) == 43) {  // filter out '+' as html concatenator
              (*chPtr) = 32;  // replace with blank space
            }
            EEPROM.write((chPtr - val_strt + nm_strt), (*chPtr));
          }
          if ((val_end - val_strt) != 30) {
            EEPROM.write((val_end - val_strt + nm_strt), 0);
          }
        }
        else if (strncmp(id_strt, "rd", 2) == 0) {  // ***************************************** Record Data ************************************
          //Serial.println(F("record data"));

          u16dum = 0;
          for (chPtr = val_strt; chPtr < val_end; chPtr++) {
            u16dum = u16dum * 10 + ((*chPtr) - '0');
          }

          if (u16dum) {
            EEPROM.write(nm_strt + 31, true);
          }
          else {
            EEPROM.write(nm_strt + 31, false);
          }
        }
        else if (strncmp(id_strt, "ms", 2) == 0) {  // ***************************** Max Number of Slaves to Record ************************************
          //Serial.println(F("num slaves to record"));

          u8dum = 0;
          for (chPtr = val_strt; chPtr < val_end; chPtr++) {
            u8dum = u8dum * 10 + ((*chPtr) - '0');
          }

          EEPROM.write(nm_strt + 32, u8dum);
        }
        else if (strncmp(id_strt, "ip", 2) == 0) {  //  ***************************************** IP ************************************************
          //Serial.println(F("ip"));

          chPtr = val_strt;
          for (j = 0; j < 4; j++) {
            u8dum = 0;
            while (((*chPtr) != '.') && (chPtr < val_end)) {
              u8dum = u8dum * 10 + ((*chPtr) - '0');
              chPtr++;
            }
            chPtr++;
            EEPROM.write((j + ip_strt + 6), u8dum);
          }
        }
        else if (strncmp(id_strt, "sm", 2) == 0) {  //  ****************************************** SUBNET MASK *************************************************
          //Serial.println(F("sm"));

          chPtr = val_strt;
          for (j = 0; j < 4; j++) {
            u8dum = 0;
            while (((*chPtr) != '.') && (chPtr < val_end)) {
              u8dum = u8dum * 10 + ((*chPtr) - '0');
              chPtr++;
            }
            chPtr++;
            EEPROM.write((j + ip_strt + 10), u8dum);
          }
        }
        else if (strncmp(id_strt, "gw", 2) == 0) {  //  ****************************************** DEFAULT GATEWAY *************************************************
          //Serial.println(F("gw"));
          chPtr = val_strt;
          for (j = 0; j < 4; j++) {
            u8dum = 0;
            while (((*chPtr) != '.') && (chPtr < val_end)) {
              u8dum = u8dum * 10 + ((*chPtr) - '0');
              chPtr++;
            }
            chPtr++;
            EEPROM.write((j + ip_strt + 14), u8dum);
          }
        }
        else if (strncmp(id_strt, "ntp", 3) == 0) {  // ***************************************** USE NTP? ************************************
          //Serial.println(F("use ntp?"));

          u16dum = 0;
          for (chPtr = val_strt; chPtr < val_end; chPtr++) {
            u16dum = u16dum * 10 + ((*chPtr) - '0');
          }

          if (u16dum) {
            EEPROM.write(ip_strt + 18, true);
          }
          else {
            EEPROM.write(ip_strt + 18, false);
          }
        }
        else if (strncmp(id_strt, "nip", 3) == 0) {  //  ************************************ NTP SERVER IP *****************************************
          //Serial.println(F("ntp ip"));

          chPtr = val_strt;
          for (j = 0; j < 4; j++) {
            u8dum = 0;
            while (((*chPtr) != '.') && (chPtr < val_end)) {
              u8dum = u8dum * 10 + ((*chPtr) - '0');
              chPtr++;
            }
            chPtr++;
            EEPROM.write((j + ip_strt + 19), u8dum);
          }
        }
        else if (strncmp(id_strt, "br", 2) == 0) {  //  ****************************************** BAUDRATE *************************************************
          //Serial.println(F("br"));

          u32dum = 0;

          for (chPtr = val_strt; chPtr < val_end; chPtr++) {
            u32dum = u32dum * 10 + ((*chPtr) - '0');
          }

          EEPROM.write(ip_strt + 23, (u32dum >> 16));
          EEPROM.write(ip_strt + 24, (u32dum >> 8));
          EEPROM.write(ip_strt + 25, u32dum);
        }
        else if (strncmp(id_strt, "to", 2) == 0) {  //  ****************************************** MB TIMEOUT *************************************************
          //Serial.println(F("to"));

          u16dum = 0;
          for (chPtr = val_strt; chPtr < val_end; chPtr++) {
            u16dum = u16dum * 10 + ((*chPtr) - '0');
          }

          EEPROM.write(ip_strt + 26, highByte(u16dum));
          EEPROM.write(ip_strt + 27, lowByte(u16dum));
        }
        else if (strncmp(id_strt, "tm", 2) == 0) {  //  ****************************************** TIME *************************************************
          //Serial.println(F("time"));

          u32dum = 0;

          for (chPtr = val_strt; chPtr < val_end; chPtr++) {
            u32dum = u32dum * 10 + ((*chPtr) - '0');
          }

          if (u32dum > 1451606400UL) {
            bGoodRTC = true;
#if defined(CORE_TEENSY)
            Teensy3Clock.set(u32dum);
#endif
            setTime(u32dum);
          }
        }

        postMsgPtr++;
        id_strt = postMsgPtr;  
      }  // end if *postMsgPtr == '='
      else {  // doesn't equal '='
        postMsgPtr++;
      }  // end if ch == '='
    }  // if not '\0'
    else {
      if (totLen < postLen) {
        if (cl.available()) {
          uint16_t lenRead;
          lenRead = cl.read((uint8_t*)headHttp, REQ_ARR_SZ - 1);
          headHttp[lenRead] = 0;
          totLen += lenRead;
        }
        else {
          readingMsg = false;  // exit if no more can be found
        }
      }
      else {
        readingMsg = false;  // read everything already, exit
      }
    }
  }  // end while
  flushEthRx(cl, (uint8_t*)headHttp, REQ_ARR_SZ - 1);  // this shouldn't need to be used, here just in case

  digitalWrite(epWriteLed, LOW);
  setConstants();
}

