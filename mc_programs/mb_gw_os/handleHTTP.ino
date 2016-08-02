void handle_http(bool b_idleModbus) {
#if DISP_TIMING_DEBUG == 1
  uint32_t gotClient, doneHttp, doneFind, time1 = 0, time2 = 0, lineTime = 0;  // times for debugging
  //uint32_t totBytes = 0;
#endif
  
  EthernetClient52 ec_client = g_es_webServ.available();  // try to get client

  if (ec_client) {  // got client?
#if DEBUG_HTTP_TCP_TIMEOUT == 1
    uint32_t u32Http_TCP_to_old;                      // time keeper for tcp timeout
    const uint32_t k_u32_httpTcpTimeout(3000);        // HTTP_TCP_TIMEOUT timeout for device to hold on to tcp connection after http request
    u32Http_TCP_to_old = millis();
#endif

#if DISP_TIMING_DEBUG == 1
    gotClient = millis();
#endif
    


    //while (ec_client.connected() && ((millis() - u32Http_TCP_to_old) < k_u32_httpTcpTimeout)) {
    while (ec_client.connected()) {
      uint8_t u8_meterType;                                     // type of meter, identifies register mapping in eeprom -> X.x.x
      char *cp_meterInd;                                     // index of 'METER' in GET request
      char ca_firstLine[gk_u16_requestLineSize] = { 0 };                 // buffer for first line of HTTP request stored as null terminated string
      char ca_remHeader[gk_u16_requestBuffSize] = { 0 };                // buffer for remaining HTTP request

      if (ec_client.available()) {   // client data available to read
        char *cp_dumPtr;
        uint16_t u16_lenRead;
        uint32_t u32_msgRecvTime;

        u16_lenRead = ec_client.read((uint8_t*)ca_firstLine, gk_u16_requestLineSize - 1);

        u32_msgRecvTime = millis();
        while (u16_lenRead < gk_u16_requestLineSize - 1) {  // make sure enough is read
          uint32_t k_u32_mesgTimeout(50);

          u16_lenRead += ec_client.read((uint8_t*)ca_firstLine + u16_lenRead, gk_u16_requestLineSize - u16_lenRead - 1);

          if ((millis() - u32_msgRecvTime) > k_u32_mesgTimeout) {  // stop trying to read message after 50 ms - assume it's never coming
            ec_client.stop();
            return;
          }
        }

#if DISP_TIMING_DEBUG == 1
        lineTime = millis();
#endif

        ca_firstLine[gk_u16_requestLineSize - 1] = 0;  // this will replace a character, though I don't think it is important
        ca_remHeader[gk_u16_requestBuffSize - 1] = 0;

        Serial.println(ca_firstLine);
        //Serial.println(HTTP_req + gk_u16_requestLineSize);

        if (strncmp(ca_firstLine, "GET", 3) == 0) {
          flushEthRx(ec_client, (uint8_t*)ca_remHeader, gk_u16_requestBuffSize - 1);

#if DISP_TIMING_DEBUG == 1
          doneHttp = millis();
#endif

          if (g_b_sdInit) {
            if (strstr(ca_firstLine, ".css") != nullptr) {
              sendWebFile(ec_client, "/ep_style.css", FileType::CSS);  // only css available
            }
            else if (strstr(ca_firstLine, ".gif") != nullptr) {  // will need to expand if more pictures are desired
              //sendGifHdr(ec_client);
              sendWebFile(ec_client, "/images/logo_let.gif", FileType::GIF);  // only gif available

              // assuming gif is last request and reset is required, reset
              if (g_b_reset) {
                ec_client.stop();
                resetArd();  // this will restart the gateway
                return;  // this should never fire
              }
              break;
            }
// html requests
            else if ((strstr(ca_firstLine, ".htm") != nullptr) || (strncmp(ca_firstLine, "GET / ", 6) == 0)) { // if html or index request
              if ((strncmp(ca_firstLine, "GET / ", 6) == 0) || (strncmp(ca_firstLine, "GET /index.htm", 14) == 0)) {
#if DISP_TIMING_DEBUG
                time1 = millis();
#endif
                sendWebFile(ec_client, "/index.htm", FileType::HTML);
#if DISP_TIMING_DEBUG
                time2 = millis();
#endif
              }
              else if (strncmp(ca_firstLine, "GET /gensetup.htm", 17) == 0) {
                sendWebFile(ec_client, "/gensetup.htm", FileType::HTML);
              }
              else if (strncmp(ca_firstLine, "GET /mtrsetup.htm", 17) == 0) {
                sendWebFile(ec_client, "/mtrsetup.htm", FileType::HTML);
              }
              else if (strstr(ca_firstLine, "live.htm") != nullptr) {
                cp_meterInd = strstr(ca_firstLine, "METER=");

                if (cp_meterInd != nullptr) {
                  g_u8a_selectedSlave = 0;
                  cp_meterInd += 6;  // move pointer to end of phrase
                  cp_dumPtr = cp_meterInd + 3;

                  for (; cp_meterInd < cp_dumPtr; ++cp_meterInd) {
                    if (isdigit(*cp_meterInd)) {
                      g_u8a_selectedSlave = g_u8a_selectedSlave * 10 + ((*cp_meterInd) - '0');
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

                u8_meterType = g_u8a_slaveTypes[(g_u8a_selectedSlave - 1)][0];  // turn meter from slave index to meter type X.x.x

                if (u8_meterType == 11) {
                  sendWebFile(ec_client, "/clive.htm", FileType::HTML);        // chilled water
                }
                else if (u8_meterType == 12) {
                  sendWebFile(ec_client, "/slive.htm", FileType::HTML);        // steam
                }
                else {
                  sendWebFile(ec_client, "/elive.htm", FileType::HTML);        // electric 
                }
              }
              else if (strncmp(ca_firstLine, "GET /pastdown.htm", 17) == 0) {

                sendWebFile(ec_client, "/pstdown1.htm", FileType::HTML);
                sendDownLinks(ec_client, ca_firstLine);
                sendWebFile(ec_client, "/pstdown2.htm", FileType::NONE);
              }
              else if (strncmp(ca_firstLine, "GET /pastview.htm", 17) == 0) {  // need to flesh out ideas for this (mimic pastdown?)
                                                                  // graph data?
                sendWebFile(ec_client, "/pastview.htm", FileType::HTML);
              }
              else if (strncmp(ca_firstLine, "GET /reset.htm", 14) == 0) {
                sendWebFile(ec_client, "/reset.htm", FileType::HTML);
              }
              else if (strncmp(ca_firstLine, "GET /redirect.htm", 17) == 0) {
                sendWebFile(ec_client, "/rdct1.htm", FileType::HTML);
                sendIP(ec_client);
                sendWebFile(ec_client, "/rdct2.htm", FileType::NONE);

                g_b_reset = true;  // must wait for css and gif requests before restarting
              }
              else {
                sendWebFile(ec_client, "/nopage.htm", FileType::HTML);
              }
            }
//  xml requests
            else if (strstr(ca_firstLine, ".xml") != nullptr) {
              if (strncmp(ca_firstLine, "GET /gensetup.xml", 17) == 0) {
                sendWebFile(ec_client, "/gensetup.xml", FileType::XML);
                sendXmlEnd(ec_client, XmlFile::GENERAL);
              }
              else if (strncmp(ca_firstLine, "GET /mtrsetup.xml", 17) == 0) {
                sendWebFile(ec_client, "/mtrsetup.xml", FileType::XML);
                sendXmlEnd(ec_client, XmlFile::METER);
              }
              else if (strncmp(ca_firstLine, "GET /data.xml", 13) == 0) {
                cp_meterInd = strstr(ca_firstLine, "METER=");

                if (cp_meterInd != nullptr) {
                  g_u8a_selectedSlave = 0;
                  cp_meterInd += 6;  // move pointer to end of phrase
                  cp_dumPtr = cp_meterInd + 3;

                  for (; cp_meterInd < cp_dumPtr; ++cp_meterInd) {
                    if (isdigit(*cp_meterInd)) {
                      g_u8a_selectedSlave = g_u8a_selectedSlave * 10 + ((*cp_meterInd) - '0');
                    }
                    else {
                      break;
                    }
                  }

                  if (g_u8a_selectedSlave > EEPROM.read(g_u16_mtrBlkStart)) {  // if greater than number of meters listed
                    g_u8a_selectedSlave = 1;
                  }
                }
                liveXML(ec_client);  // handles http in function
              }
              else if (strncmp(ca_firstLine, "GET /info.xml", 13) == 0) {
                sendWebFile(ec_client, "/mtrsetup.xml", FileType::XML);
                sendXmlEnd(ec_client, XmlFile::INFO);
              }
              else {  // could not find xml file
                send404(ec_client);  // handles http in function
              }
            }
// txt and csv requests
            else if ((strstr(ca_firstLine, ".TXT") != nullptr) || (strstr(ca_firstLine, ".CSV") != nullptr)) {
              //if (strncmp(HTTP_req, "GET /TEST.CSV", 13) == 0) {
              //  ec_client.println("HTTP/1.1 200 OK");
              //  ec_client.println("Content-Disposition: attachment; filename=test.txt");
              //  ec_client.println("Connection: close\n");

              //  sendWebFile(ec_client, "/test.CSV");
              //}
              //else if (strstr(HTTP_req, "GET /TEST.TXT")) {
              //  ec_client.println("HTTP/1.1 200 OK");
              //  ec_client.println("Content-Disposition: attachment; filename=test.txt");
              //  ec_client.println("Connection: close\n");

              //  sendWebFile(ec_client, "test.txt");
              //}
              //else {
                char filename[36] = { 0 };

                strcpy(filename, ca_firstLine + 4);
                filename[35] = 0;
                for (int ii = 0; ii < 36; ++ii) {
                  if (filename[ii] == 32) {
                    filename[ii] = 0;
                    break;
                  }
                  else if (filename[ii] == 0) {
                    break;
                  }
                }
                sendWebFile(ec_client, filename, FileType::NONE);
              //}
            }
// not htm, css, or xml
            else {  
              send404(ec_client);
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
            sendBadSD(ec_client);
            break;  // don't bother trying to keep connection open - there's no reason to if there's no sd card
          }
        }
// POST http
        else if (strstr(ca_firstLine, "POST") != nullptr) {
          if (strstr(ca_firstLine, "setup.htm")) {
            digitalWrite(gk_s16_epWriteLed, HIGH);
            getPostSetupData(ec_client, ca_remHeader);  // reads and stores POST data to EEPROM
            digitalWrite(gk_s16_epWriteLed, LOW);

            // rewrite xml files
            digitalWrite(gk_s16_sdWriteLed, HIGH);
            if (strncmp(ca_firstLine, "POST /mtrsetup.htm", 18)) {
              writeMtrSetupFile();
            }
            else if (strncmp(ca_firstLine, "POST /gensetup.htm", 18)) {
              writeGenSetupFile();
            }
            digitalWrite(gk_s16_sdWriteLed, LOW);

            sendPostResp(ec_client);
         
#if DEBUG_HTTP_TCP_TIMEOUT == 1
            u32Http_TCP_to_old = millis();
#else
            break;
#endif
          }
        }

      } // end if (ec_client.available())
      else if (b_idleModbus) {
        //handle_modbus(false);
      }
    } // end while (ec_client.connected() && < timeout)
    delay(1);      // give the web browser time to receive the data
    ec_client.stop(); // close the connection
  } // end if (ec_client)
} // end handle_http
