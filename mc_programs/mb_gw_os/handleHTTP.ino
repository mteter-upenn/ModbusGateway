void handle_http(bool bMain) {
  uint32_t i;                                        // tickers
  uint8_t meter;                                     // type of meter, identifies register mapping in eeprom -> X.x.x
  char *mtr_ind;                                     // index of 'METER' in GET request
  char *chPtr;
  char reqHttp[gk_u16_requestLineSize] = { 0 };                 // buffer for first line of HTTP request stored as null terminated string
  char headHttp[gk_u16_requestBuffSize] = { 0 };                // buffer for remaining HTTP request
  uint16_t initLen;
  uint32_t strtMsg;
  uint32_t u32Http_TCP_to_old;                      // time keeper for tcp timeout
  const uint32_t k_u32_httpTcpTimeout(3000);        // HTTP_TCP_TIMEOUT timeout for device to hold on to tcp connection after http request

#if DISP_TIMING_DEBUG == 1
  uint32_t gotClient, doneHttp, doneFind, time1 = 0, time2 = 0, lineTime = 0;  // times for debugging
  //uint32_t totBytes = 0;
#endif
  
  EthernetClient52 client = g_es_webServ.available();  // try to get client

  if (client) {  // got client?
#if DISP_TIMING_DEBUG == 1
    gotClient = millis();
#endif
    u32Http_TCP_to_old = millis();


    while (client.connected() && ((millis() - u32Http_TCP_to_old) < k_u32_httpTcpTimeout)) {
    //while (client.connected()) {
      if (client.available()) {   // client data available to read
        initLen = client.read((uint8_t*)reqHttp, gk_u16_requestLineSize - 1);

        strtMsg = millis();
        while (initLen < gk_u16_requestLineSize - 1) {  // make sure enough is read
          initLen += client.read((uint8_t*)reqHttp + initLen, gk_u16_requestLineSize - initLen - 1);

          if ((millis() - strtMsg) > 50) {  // stop trying to read message after 50 ms - assume it's never coming
            client.stop();
            return;
          }
        }

#if DISP_TIMING_DEBUG == 1
        lineTime = millis();
#endif

        reqHttp[gk_u16_requestLineSize - 1] = 0;  // this will replace a character, though I don't think it is important
        headHttp[gk_u16_requestBuffSize - 1] = 0;

        Serial.println(reqHttp);
        //Serial.println(HTTP_req + gk_u16_requestLineSize);

        if (strncmp(reqHttp, "GET", 3) == 0) {
          flushEthRx(client, (uint8_t*)headHttp, gk_u16_requestBuffSize - 1);

#if DISP_TIMING_DEBUG == 1
          doneHttp = millis();
#endif

          if (g_b_sdInit) {
            if (strstr(reqHttp, ".css") != NULL) {
              sendWebFile(client, "/ep_style.css", 2);  // only css available
            }
            else if (strstr(reqHttp, ".gif") != NULL) {  // will need to expand if more pictures are desired
              sendGifHdr(client);
              sendWebFile(client, "/images/logo_let.gif", 0);  // only gif available

              // assuming gif is last request and reset is required, reset
              if (g_b_reset) {
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
                  g_u8a_selectedSlave = 0;
                  mtr_ind += 6;  // move pointer to end of phrase
                  chPtr = mtr_ind + 3;

                  for (; mtr_ind < chPtr; mtr_ind++) {
                    if (isdigit(*mtr_ind)) {
                      g_u8a_selectedSlave = g_u8a_selectedSlave * 10 + ((*mtr_ind) - '0');
                    }
                    else {
                      break;
                    }
                  }

                  if (g_u8a_selectedSlave > EEPROM.read(g_u16_mtrBlkStart)) {  // if greater than number of meters listed
                    g_u8a_selectedSlave = 1;
                  }
                }
                else {
                  g_u8a_selectedSlave = 1;
                }

                meter = g_u8a_slaveTypes[(g_u8a_selectedSlave - 1)][0];  // turn meter from slave index to meter type X.x.x

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

                g_b_reset = true;  // must wait for css and gif requests before restarting
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
                  g_u8a_selectedSlave = 0;
                  mtr_ind += 6;  // move pointer to end of phrase
                  chPtr = mtr_ind + 3;

                  for (; mtr_ind < chPtr; mtr_ind++) {
                    if (isdigit(*mtr_ind)) {
                      g_u8a_selectedSlave = g_u8a_selectedSlave * 10 + ((*mtr_ind) - '0');
                    }
                    else {
                      break;
                    }
                  }

                  if (g_u8a_selectedSlave > EEPROM.read(g_u16_mtrBlkStart)) {  // if greater than number of meters listed
                    g_u8a_selectedSlave = 1;
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
            //Serial.write(HTTP_req, gk_u16_requestLineSize - 1);
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
            digitalWrite(gk_s16_epWriteLed, HIGH);
            getPostSetupData(client, headHttp);  // reads and stores POST data to EEPROM
            digitalWrite(gk_s16_epWriteLed, LOW);

            // rewrite xml files
            digitalWrite(gk_s16_sdWriteLed, HIGH);
            if (strncmp(reqHttp, "POST /mtrsetup.htm", 18)) {
              writeMtrSetupFile();
            }
            else if (strncmp(reqHttp, "POST /gensetup.htm", 18)) {
              writeGenSetupFile();
            }
            digitalWrite(gk_s16_sdWriteLed, LOW);

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
