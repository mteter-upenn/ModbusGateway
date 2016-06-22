/*
* void sendPostResp
* char * preprocPost
* void getPostSetupData
*/



void sendPostResp(EthernetClient client) {
  char postResp[76]; // = "HTTP/1.1 303 See Other\nLocation: http://";

  strcpy_P(postResp, PSTR("HTTP/1.1 303 See Other\nLocation: /redirect.htm\nConnection: close\n\n"));
  client.write(postResp, 66);

  //post_cont.clear();
  client.flush();
}


char * preprocPost(EthernetClient client, char * headHttp, uint16_t &postLen) {
  char * msgPtr;
  uint16_t lenRead;
  uint16_t i;
  bool foundPtr = false;
  const char contLen[] = "Content-Length: ";
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

  Serial.print(F("index: "));
  Serial.println(msgPtr - headHttp);
  Serial.write(msgPtr, 3);
  Serial.println();
  Serial.println(headHttp);

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
  
  Serial.print(F("postLen: "));
  Serial.println(postLen, DEC);

  // now find '\n\n'  -  this escapes from the http header
  foundPtr = false;
  charMatches = false;

  while (!foundPtr) {
    msgPtr = strstr(headHttp, "\n\n");  // Find '\n\n'

    if (msgPtr != NULL) {
      msgPtr += 2;  // add 2 to get to end of phrase
      foundPtr = true;  // exit condition
    }
    else {  // check to see if phrase was cut in 2
      if (headHttp[lenRead - 1] == '\n') {
        charMatches = true;
      }

      lenRead = client.read((uint8_t*)headHttp, POST_BUF_SZ); // read out to POST_BUF_SZ
      headHttp[lenRead] = 0;
      Serial.print(headHttp);

      if (charMatches) {
        if (headHttp[0] == '\n') {
          foundPtr = true;
          msgPtr = headHttp + 1;
        }
        else {
          charMatches = false;
        }
      }  // if charMatches
    }  // else pointer is NULL
  }  // while haven't found pointer

  lenRead = client.read((uint8_t*)headHttp + lenRead, REQ_ARR_SZ - lenRead);
  headHttp[lenRead] = 0;
  return msgPtr;
}

//char * postLenPtr;

//client.read((uint8_t*)headHttp, POST_BUF_SZ); // read
//headHttp[POST_BUF_SZ] = 0;

//postLenPtr = strstr(headHttp, "Content-Length: ");  // Find 'Content-Length: '

//if (postLenPtr != NULL) {  // found 'Content-Length'
//  postLenPtr += 16;

//  for (i = 0; i < 2; i++) {  // used for loop instead just to be sure it breaks free even if I get nothing out of it
//    while (isdigit(*postLenPtr)) {
//      post_len = post_len * 10 + ((*postLenPtr) - '0');
//      postLenPtr++;
//    }

//    if ((*postLenPtr) == '\0') {
//      client.read((uint8_t*)headHttp, POST_BUF_SZ); // length was cut off, load next portion into headHttp
//      postLenPtr = headHttp;  // reset postLenPtr to start of headHttp
//    }
//    else {
//      break;
//    }
//  }
//  // now need to find end of message, continue to rotate through POST_BUF_SZ segments until \n\n found


//  //if (postLenPtr < (HTTP_req + REQ_ARR_SZ - 23)) {  // make sure not at end of array
//  //  postLenPtr += 16;

//  //  for (postLenPtr; postLenPtr < postLenPtr + 7; postLenPtr++) {
//  //    if (isdigit(*postLenPtr)) {
//  //      post_len = post_len * 10 + ((*postLenPtr) - '0');
//  //    }
//  //    else {
//  //      break;
//  //    }
//  //  }
//  //}
//  //else {  // phrase occurs near end of array
//  //  // not sure how to handle this except for pray it doesn't happen
//  //}
//}
//else {
//  // phrase not found, need to check end of array in case it got split
//  // for now, hope it doesn't happen, but it does need to get fixed
//}

//flushEthRx(client, (uint8_t*)headHttp, REQ_ARR_SZ - 1);


void getPostSetupData(EthernetClient cl, char * headHttp) {
  uint16_t  j; // i, k
  //uint16_t id_strt, id_end, val_strt, val_end;
  char * id_strt, *id_end, *val_strt, *val_end;
  char * chPtr;
  //char post_str[pst_len];
  //char ch;
  uint8_t u8dum, lcl_meter;
  uint8_t num_mtrs = 0;
  uint16_t u16dum;
  uint32_t u32dum;

  uint16_t postLen;
  char * postMsgPtr;

  Serial.println("start preproc");
  postMsgPtr = preprocPost(cl, headHttp, postLen);
  Serial.println("exited preproc");

  //for (i = 0; i < pst_len; i++) {
  //  if (cl.available()) {
  //    post_str[i] = cl.read();
  //    //Serial.print(post_str[i]);
  //  }
  //  else {
  //    return;  // false
  //  }
  //}
  //Serial.println();

  //id_strt = 0;
  id_strt = postMsgPtr;
  //i = 0;
  digitalWrite(epWriteLed, HIGH);
  //  Serial.println(F("post setup"));
  while ((*postMsgPtr) != '\0') {
    //ch = (*postMsgPtr);
    //    Serial.print(F("ch: "));
    //    Serial.println(ch);
    if ((*postMsgPtr) == '=') {
      //id_end = i;
      //i++;
      //val_strt = i;

      //ch = post_str[i];

      id_end = postMsgPtr;
      postMsgPtr++;
      val_strt = postMsgPtr;


      //while ((i < pst_len) && (ch != '&')) {
      //  i++;
      //  ch = post_str[i];
      //}
      //val_end = i;

      while (((*postMsgPtr) != '&') && ((*postMsgPtr) != '\0')) {
        postMsgPtr++;
      }
      val_end = postMsgPtr;

      //if (strncmp(post_str + id_strt, "mip", 3) == 0) {  //  ****************************************** METER IP *************************************************
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
                                                             //        Serial.println(F("nm"));
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
        u8dum = 0;
        for (chPtr = val_strt; chPtr < val_end; j++) {
          u8dum = u8dum * 10 + ((*chPtr) - '0');
        }

        EEPROM.write(nm_strt + 32, u8dum);
      }
      else if (strncmp(id_strt, "ip", 2) == 0) {  //  ***************************************** IP ************************************************
                                                             //        Serial.println(F("ip"));
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
                                                             //        Serial.println(F("sm"));
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
                                                             //        Serial.println(F("gw"));
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
                                                             //        Serial.println(F("br"));
        u32dum = 0;

        for (chPtr = val_strt; chPtr < val_end; chPtr++) {
          u32dum = u32dum * 10 + ((*chPtr) - '0');
        }

        EEPROM.write(ip_strt + 23, (u32dum >> 16));
        EEPROM.write(ip_strt + 24, (u32dum >> 8));
        EEPROM.write(ip_strt + 25, u32dum);
      }
      else if (strncmp(id_strt, "to", 2) == 0) {  //  ****************************************** MB TIMEOUT *************************************************
                                                             //        Serial.println(F("to"));
        u16dum = 0;
        for (chPtr = val_strt; chPtr < val_end; chPtr++) {
          u16dum = u16dum * 10 + ((*chPtr) - '0');
        }

        EEPROM.write(ip_strt + 26, highByte(u16dum));
        EEPROM.write(ip_strt + 27, lowByte(u16dum));
      }
      else if (strncmp(id_strt, "tm", 2) == 0) {  //  ****************************************** TIME *************************************************
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
      //i++;
      //id_strt = i;
      postMsgPtr++;
      id_strt = postMsgPtr;
    }  // end if ch == '='
    else {
      //i++;
      postMsgPtr++;
    }  // end if ch == '='
  }  // while loop

  flushEthRx(cl, (uint8_t*)headHttp, REQ_ARR_SZ - 1);  // this shouldn't need to be used, here just in case

  digitalWrite(epWriteLed, LOW);
  setConstants();
}

