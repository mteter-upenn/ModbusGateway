void handle_http(bool bMain) {
  uint32_t i;                                        // tickers
  uint8_t meter;                                     // type of meter, identifies register mapping in eeprom -> X.x.x
  char *mtr_ind;                                     // index of 'METER' in GET request
  char *chPtr;
  char reqHttp[REQ_BUF_SZ] = { 0 };                 // buffer for first line of HTTP request stored as null terminated string
  char headHttp[REQ_ARR_SZ] = { 0 };                // buffer for remaining HTTP request
  uint16_t initLen;
  uint32_t strtMsg;
  uint32_t u32Http_TCP_to_old;                      // time keeper for tcp timeout

#if DISP_TIMING_DEBUG == 1
  uint32_t gotClient, doneHttp, doneFind, time1 = 0, time2 = 0, lineTime = 0;  // times for debugging
  //uint32_t totBytes = 0;
#endif
  
  EthernetClient52 client = serv_web.available();  // try to get client

  if (client) {  // got client?
#if DISP_TIMING_DEBUG == 1
    gotClient = millis();
#endif
    u32Http_TCP_to_old = millis();


    while (client.connected() && ((millis() - u32Http_TCP_to_old) < HTTP_TCP_TIMEOUT)) {
    //while (client.connected()) {
      if (client.available()) {   // client data available to read
        initLen = client.read((uint8_t*)reqHttp, REQ_BUF_SZ - 1);

        strtMsg = millis();
        while (initLen < REQ_BUF_SZ - 1) {  // make sure enough is read
          initLen += client.read((uint8_t*)reqHttp + initLen, REQ_BUF_SZ - initLen - 1);

          if ((millis() - strtMsg) > 50) {  // stop trying to read message after 50 ms - assume it's never coming
            client.stop();
            return;
          }
        }

#if DISP_TIMING_DEBUG == 1
        lineTime = millis();
#endif

        reqHttp[REQ_BUF_SZ - 1] = 0;  // this will replace a character, though I don't think it is important
        headHttp[REQ_ARR_SZ - 1] = 0;

        Serial.println(reqHttp);
        //Serial.println(HTTP_req + REQ_BUF_SZ);

        if (strncmp(reqHttp, "GET", 3) == 0) {
          flushEthRx(client, (uint8_t*)headHttp, REQ_ARR_SZ - 1);

#if DISP_TIMING_DEBUG == 1
          doneHttp = millis();
#endif

          if (sdInit) {
            if (strstr(reqHttp, ".css") != NULL) {
              sendWebFile(client, "/ep_style.css", 2);  // only css available
            }
            else if (strstr(reqHttp, ".gif") != NULL) {  // will need to expand if more pictures are desired
              sendGifHdr(client);
              sendWebFile(client, "/images/logo_let.gif", 0);  // only gif available

              // assuming gif is last request and reset is required, reset
              if (bReset) {
                client.stop();
                resetArd();  // this will restart the gateway
                return;  // this should never fire
              }
              break;
            }
// html requests
            else if ((strstr(reqHttp, ".htm") != NULL) || (strncmp(reqHttp, "GET / ", 6) == 0)) { // if html or index request
              if ((strncmp(reqHttp, "GET / ", 6) == 0) || (strncmp(reqHttp, "GET /index.htm", 14) == 0)) {
#if DISP_TIMING_DEBUG
                time1 = millis();
#endif
                sendWebFile(client, "/index.htm", 1);
#if DISP_TIMING_DEBUG
                time2 = millis();
#endif
              }
              else if (strncmp(reqHttp, "GET /gensetup.htm", 17) == 0) {
                sendWebFile(client, "/gensetup.htm", 1);
              }
              else if (strncmp(reqHttp, "GET /mtrsetup.htm", 17) == 0) {
                sendWebFile(client, "/mtrsetup.htm", 1);
              }
              else if (strstr(reqHttp, "live.htm") != NULL) {
                mtr_ind = strstr(reqHttp, "METER=");

                if (mtr_ind != NULL) {
                  selSlv = 0;
                  mtr_ind += 6;  // move pointer to end of phrase
                  chPtr = mtr_ind + 3;

                  for (; mtr_ind < chPtr; mtr_ind++) {
                    if (isdigit(*mtr_ind)) {
                      selSlv = selSlv * 10 + ((*mtr_ind) - '0');
                    }
                    else {
                      break;
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
                  sendWebFile(client, "/clive.htm", 1);        // chilled water
                }
                else if (meter == 12) {
                  sendWebFile(client, "/slive.htm", 1);        // steam
                }
                else {
                  sendWebFile(client, "/elive.htm", 1);        // electric 
                }
              }
              else if (strncmp(reqHttp, "GET /pastdown.htm", 17) == 0) {

                sendWebFile(client, "/pstdown1.htm", 1);
                sendDownLinks(client, reqHttp);
                sendWebFile(client, "/pstdown2.htm", 0);
              }
              else if (strncmp(reqHttp, "GET /pastview.htm", 17) == 0) {  // need to flesh out ideas for this (mimic pastdown?)
                                                                  // graph data?
                sendWebFile(client, "/pastview.htm", 1);
              }
              else if (strncmp(reqHttp, "GET /reset.htm", 14) == 0) {
                sendWebFile(client, "/reset.htm", 1);
              }
              else if (strncmp(reqHttp, "GET /redirect.htm", 17) == 0) {
                sendWebFile(client, "/rdct1.htm", 1);
                sendIP(client);
                sendWebFile(client, "/rdct2.htm", 0);

                bReset = true;  // must wait for css and gif requests before restarting
              }
              else {
                sendWebFile(client, "/nopage.htm", 1);
              }
            }
//  xml requests
            else if (strstr(reqHttp, ".xml") != NULL) {
              if (strncmp(reqHttp, "GET /gensetup.xml", 17) == 0) {
                sendWebFile(client, "/gensetup.xml", 4);
                sendXmlEnd(client, 3);
              }
              else if (strncmp(reqHttp, "GET /mtrsetup.xml", 17) == 0) {
                sendWebFile(client, "/mtrsetup.xml", 4);
                sendXmlEnd(client, 2);
              }
              else if (strncmp(reqHttp, "GET /data.xml", 13) == 0) {
                mtr_ind = strstr(reqHttp, "METER=");

                if (mtr_ind != NULL) {
                  selSlv = 0;
                  mtr_ind += 6;  // move pointer to end of phrase
                  chPtr = mtr_ind + 3;

                  for (; mtr_ind < chPtr; mtr_ind++) {
                    if (isdigit(*mtr_ind)) {
                      selSlv = selSlv * 10 + ((*mtr_ind) - '0');
                    }
                    else {
                      break;
                    }
                  }

                  if (selSlv > EEPROM.read(mtr_strt)) {  // if greater than number of meters listed
                    selSlv = 1;
                  }
                }
                liveXML(client);  // handles http in function
              }
              else if (strncmp(reqHttp, "GET /info.xml", 13) == 0) {
                sendWebFile(client, "/mtrsetup.xml", 4);
                sendXmlEnd(client, 1);
              }
              else {  // could not find xml file
                send404(client);  // handles http in function
              }
            }
// txt and csv requests
            else if ((strstr(reqHttp, ".TXT") != NULL) || (strstr(reqHttp, ".CSV") != NULL)) {
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

                strcpy(filename, reqHttp + 4);
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
                sendWebFile(client, filename, 0);
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

#if DEBUG_HTTP_TCP_TIMEOUT == 1
            u32Http_TCP_to_old = millis();  // continue loop until timeout
#else
            break;  // break from while loop to kill client and end func
#endif
          }
          else {
            sendBadSD(client);
            break;  // don't bother trying to keep connection open - there's no reason to if there's no sd card
          }
        }
// POST http
        else if (strstr(reqHttp, "POST") != NULL) {
          if (strstr(reqHttp, "setup.htm")) {
            digitalWrite(epWriteLed, HIGH);
            getPostSetupData(client, headHttp);  // reads and stores POST data to EEPROM
            digitalWrite(epWriteLed, LOW);

            // rewrite xml files
            digitalWrite(sdWriteLed, HIGH);
            if (strncmp(reqHttp, "POST /mtrsetup.htm", 18)) {
              writeMtrSetupFile();
            }
            else if (strncmp(reqHttp, "POST /gensetup.htm", 18)) {
              writeGenSetupFile();
            }
            digitalWrite(sdWriteLed, LOW);

            sendPostResp(client);
         
#if DEBUG_HTTP_TCP_TIMEOUT == 1
            u32Http_TCP_to_old = millis();
#else
            break;
#endif
          }
        }

      } // end if (client.available())
      else if (bMain) {
        //handle_modbus(false);
      }
    } // end while (client.connected() && < timeout)
    delay(1);      // give the web browser time to receive the data
    client.stop(); // close the connection
  } // end if (client)
} // end handle_http
