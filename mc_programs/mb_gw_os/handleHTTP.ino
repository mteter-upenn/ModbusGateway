void handle_http() {
  bool post_fl = false;                              // set true if http request is POST
  bool fnd_len = false;                              // set true to find length of POST message
  uint16_t post_len = 0;                             // length of POST message
  uint32_t i;                                        // tickers
  uint8_t meter;                                     // type of meter, identifies register mapping in eeprom -> X.x.x
  char *mtr_ind;                                     // index of 'METER' in GET request
  //uint8_t reqFlag = 0;                               // flag for different http requests [live.xml, mtrsetup.xml]
  char HTTP_req[REQ_ARR_SZ] = { 0 };                 // buffered HTTP request stored as null terminated string
  //uint8_t readBytes = 4;
  uint8_t req_index = 0;                             // index into HTTP_req buffer
  char ch;                                           // used for reading GET request with extra information
  char c;
#if DISP_TIMING_DEBUG == 1
  uint32_t gotClient, doneHttp, doneFind, time1 = 0, time2 = 0, lineTime = 0;  // times for debugging
  uint32_t totBytes = 0;
#endif
//  bool testBool = true;                              // flag used for debug
  
  EthernetClient client = serv_web.available();  // try to get client

  if (client) {  // got client?
    boolean currentLineIsBlank = true;
#if DISP_TIMING_DEBUG == 1
    gotClient = millis();
#endif

    while (client.connected()) {
      if (client.available() > 3) {   // client data available to read
        client.read((uint8_t*)HTTP_req, REQ_ARR_SZ - 1);  // dump as much as possible

#if DISP_TIMING_DEBUG == 1
        lineTime = millis();
#endif

        HTTP_req[REQ_BUF_SZ - 1] = 0;  // this will replace a character, though I don't think it is important
        HTTP_req[REQ_ARR_SZ - 1] = 0;

        Serial.println(HTTP_req);
        Serial.println(HTTP_req + REQ_BUF_SZ);

        if (strncmp(HTTP_req, "GET", 3) == 0) {
          flushEthRx(client, (uint8_t*)HTTP_req + REQ_BUF_SZ, REQ_ARR_SZ - REQ_BUF_SZ - 1);

#if DISP_TIMING_DEBUG == 1
          doneHttp = millis();
#endif

          if (sdInit) {
            if (strstr(HTTP_req, ".css") != NULL) {
              sendWebFile(client, "/ep_style.css");  // only css available
            }
            else if (strstr(HTTP_req, ".gif") != NULL) {  // will need to expand if more pictures are desired
              sendGifHdr(client);
              sendWebFile(client, "/images/logo_let.gif");  // only gif available

              // assuming gif is last request and reset is required, reset
              if (bReset) {
                client.stop();
                resetArd();
                return;
              }
            }
// html requests
            else if ((strstr(HTTP_req, ".htm") != NULL) || (strncmp(HTTP_req, "GET / ", 6) == 0)) { // if html or index request
              if ((strncmp(HTTP_req, "GET / ", 6) == 0) || (strncmp(HTTP_req, "GET /index.htm", 14) == 0)) {
#if DISP_TIMING_DEBUG
                time1 = millis();
#endif
                sendWebFile(client, "/index.htm");
#if DISP_TIMING_DEBUG
                time2 = millis();
#endif
              }
              else if (strncmp(HTTP_req, "GET /gensetup.htm", 17) == 0) {
                sendWebFile(client, "/gensetup.htm");
              }
              else if (strncmp(HTTP_req, "GET /mtrsetup.htm", 17) == 0) {
                sendWebFile(client, "/mtrsetup.htm");
              }
              else if (strstr(HTTP_req, "live.htm") != NULL) {
                mtr_ind = strstr(HTTP_req, "METER=");

                if (mtr_ind != NULL) {
                  selSlv = 0;
                  mtr_ind += 6;  // move pointer to end of phrase

                  for (i = 0; i < 3; i++) {  // search all of HTTP_req
                    ch = HTTP_req[i + (mtr_ind - HTTP_req)];
                    Serial.print("ch: ");
                    Serial.println(ch);
                    Serial.println(*mtr_ind);
                    //mtr_ind += i;
                    if ((ch == '&') || (ch == ' ')) {
                    //if (((*mtr_ind) == '&') || ((*mtr_ind) == ' ')) {
                      break;
                    }
                    else {
                      selSlv = selSlv * 10 + (ch - '0');
                    }
                  }

                  if (selSlv > EEPROM.read(mtr_strt)) {  // if greater than number of meters listed
                    selSlv = 1;
                  }
                }
                else {
                  selSlv = 1;
                }

                meter = slv_typs[(selSlv - 1)][0];  // turn meter from slave index to meter type X.x.x

                if (meter == 11) {
                  sendWebFile(client, "/clive.htm");        // chilled water
                }
                else if (meter == 12) {
                  sendWebFile(client, "/slive.htm");        // steam
                }
                else {
                  sendWebFile(client, "/elive.htm");        // electric 
                }
              }
              else if (strncmp(HTTP_req, "GET /pastdown.htm", 17) == 0) {

                sendWebFile(client, "/pstdown1.htm");
                sendDownLinks(client, HTTP_req);
                sendWebFile(client, "/pstdown2.htm");
              }
              else if (strncmp(HTTP_req, "GET /pastview.htm", 17) == 0) {  // need to flesh out ideas for this (mimic pastdown?)
                                                                  // graph data?
                sendWebFile(client, "/pastview.htm");
              }
              else if (strncmp(HTTP_req, "GET /reset.htm", 14) == 0) {
                sendWebFile(client, "/reset.htm");
              }
              else if (strncmp(HTTP_req, "GET /redirect.htm", 17) == 0) {
                sendWebFile(client, "/rdct1.htm");
                sendIP(client);
                sendWebFile(client, "/rdct2.htm");

                bReset = true;  // must wait for css and gif requests before restarting
              }
              else {
                sendWebFile(client, "/nopage.htm");
              }
            }
//  xml requests
            else if (strstr(HTTP_req, ".xml") != NULL) {
              if (strncmp(HTTP_req, "GET /gensetup.xml", 17) == 0) {
                sendWebFile(client, "/gensetup.xml");
                sendXmlEnd(client, 3);
              }
              else if (strncmp(HTTP_req, "GET /mtrsetup.xml", 17) == 0) {
                sendWebFile(client, "/mtrsetup.xml");
                sendXmlEnd(client, 2);
              }
              else if (strncmp(HTTP_req, "GET /data.xml", 13) == 0) {
                mtr_ind = strstr(HTTP_req, "METER=");

                if (mtr_ind != NULL) {
                  selSlv = 0;
                  mtr_ind += 6;  // move pointer to end of phrase

                  for (i = 0; i < 3; i++) {  // search all of HTTP_req
                    ch = HTTP_req[i + (mtr_ind - HTTP_req)];
                    //mtr_ind += i;
                    if ((ch == '&') || (ch == ' ')) {
                    //if (((*mtr_ind) == '&') || ((*mtr_ind) == ' ')) {
                      break;
                    }
                    else {
                      selSlv = selSlv * 10 + (ch - '0');
                    }
                  }

                  if (selSlv > EEPROM.read(mtr_strt)) {  // if greater than number of meters listed
                    selSlv = 1;
                  }
                }
                liveXML(client);  // handles http in function
              }
              else if (strncmp(HTTP_req, "GET /info.xml", 13) == 0) {
                sendWebFile(client, "/mtrsetup.xml");
                sendXmlEnd(client, 1);
              }
              else {  // could not find xml file
                send404(client);  // handles http in function
              }
            }
// txt and csv requests
            else if ((strstr(HTTP_req, ".TXT") != NULL) || (strstr(HTTP_req, ".CSV") != NULL)) {
              //if (strncmp(HTTP_req, "GET /TEST.CSV", 13) == 0) {
              //  client.println("HTTP/1.1 200 OK");
              //  client.println("Content-Disposition: attachment; filename=test.txt");
              //  client.println("Connection: close\n");

              //  sendWebFile(client, "/test.CSV");
              //}
              //else if (strstr(HTTP_req, "GET /TEST.TXT")) {
              //  client.println("HTTP/1.1 200 OK");
              //  client.println("Content-Disposition: attachment; filename=test.txt");
              //  client.println("Connection: close\n");

              //  sendWebFile(client, "test.txt");
              //}
              //else {
                char filename[36] = { 0 };

                strcpy(filename, HTTP_req + 4);
                filename[35] = 0;
                for (i = 0; i < 36; i++) {
                  if (filename[i] == 32) {
                    filename[i] = 0;
                    break;
                  }
                  else if (filename[i] == 0) {
                    break;
                  }
                }
                sendWebFile(client, filename);
              //}
            }
// not htm, css, or xml
            else {  
              send404(client);
            }

#if DISP_TIMING_DEBUG == 1
            doneFind = millis();
#endif

#if DISP_TIMING_DEBUG == 1
            //doneSend = millis();
            //Serial.write(HTTP_req, REQ_BUF_SZ - 1);
            //Serial.println();
            //Serial.print(F("Message length: "));
            //Serial.println(totBytes, DEC);
            Serial.print(F("HTTP Header: "));
            Serial.println((doneHttp - gotClient), DEC);
            Serial.print(F("time to grab first 40: "));
            Serial.println((lineTime - gotClient), DEC);
            Serial.print(F("Find: "));
            Serial.println((doneFind - doneHttp), DEC);
            Serial.print(F("time to send page: "));
            //Serial.println((doneSend - doneFind), DEC);
            //Serial.print(F("time2 - time1: "));
            Serial.println((time2 - time1), DEC);
            Serial.println();
#endif

            break;  // break from while loop to kill client and end func
          }
        }
// POST http
        else if (strstr(HTTP_req, "POST") != NULL) {
          char * postLenPtr;

          postLenPtr = strstr(HTTP_req + REQ_BUF_SZ, "Content-Length");  // Find 'Content-Length: '

          if (postLenPtr != NULL) {  // found 'Content-Length'
            if (postLenPtr < (HTTP_req + REQ_ARR_SZ - 23)) {  // make sure not at end of array
              postLenPtr += 16;

              //char c = HTTP_req[postLenPtr - HTTP_req];
              //char c = *postLenPtr;

              for (postLenPtr; postLenPtr < postLenPtr + 7; postLenPtr++) {
                if ((*postLenPtr) == '\r' || (*postLenPtr) == '\n') {
                  post_len = post_len * 10 + ((*postLenPtr) - '0');
                }
                else {
                  break;
                }
              }
            }
            else {  // phrase occurs near end of array
              // not sure how to handle this except for pray it doesn't happen
            }
          }
          else {
            // phrase not found, need to check end of array in case it got split
            // for now, hope it doesn't happen, but it does need to get fixed
          }

          flushEthRx(client, (uint8_t*)HTTP_req + REQ_BUF_SZ, REQ_ARR_SZ - REQ_BUF_SZ - 1);


          if (strstr(HTTP_req, "setup.htm")) {
            digitalWrite(epWriteLed, HIGH);
            getPostSetupData(client, post_len);  // reads and stores POST data to EEPROM
            digitalWrite(epWriteLed, LOW);

            // write xml files
            digitalWrite(sdWriteLed, HIGH);
            if (strncmp(HTTP_req, "POST /mtrsetup.htm", 18)) {
              writeMtrSetupFile();
            }
            else if (strncmp(HTTP_req, "POST /gensetup.htm", 18)) {
              writeGenSetupFile();
            }
            digitalWrite(sdWriteLed, LOW);

            sendPostResp(client);

            /*client.stop();
            resetArd();
            return;*/
            break;
          }
          else {
            sendWebFile(client, "nopage.htm");
            break;
          }
        }

      } // end if (client.available())
      else {
        // delay and check if we can get more
        delay(1);
        if (client.available() < 4) {
          break;
        }
      }
    } // end while (client.connected())
    delay(1);      // give the web browser time to receive the data
    client.stop(); // close the connection
  } // end if (client)
} // end handle_http
