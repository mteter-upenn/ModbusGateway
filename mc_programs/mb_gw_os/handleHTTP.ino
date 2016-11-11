void handle_http(uint8_t u8_socket) {
#if DISP_TIMING_DEBUG == 1
  uint32_t gotClient, doneHttp, doneFind, time1 = 0, time2 = 0, lineTime = 0;  // times for debugging
  //uint32_t totBytes = 0;
#endif
  
  //EthernetClient52 ec_client = g_es_webServ.available();  // try to get client

  //if (ec_client) {  // got client?
#if DISP_TIMING_DEBUG == 1
    gotClient = millis();
#endif
    while (g_eca_socks[u8_socket].connected()) {
      uint8_t u8_meterType;                                     // type of meter, identifies register mapping in eeprom -> X.x.x
      char *cp_meterInd;                                     // index of 'METER' in GET request
      char ca_firstLine[gk_u16_requestLineSize] = { 0 };                 // buffer for first line of HTTP request stored as null terminated string
      char ca_remHeader[gk_u16_requestBuffSize] = { 0 };                // buffer for remaining HTTP request

      if (g_eca_socks[u8_socket].available()) {   // client data available to read
        char *cp_dumPtr;
        uint16_t u16_lenRead;
        uint32_t u32_msgRecvTime;

        u16_lenRead = g_eca_socks[u8_socket].read((uint8_t*)ca_firstLine, gk_u16_requestLineSize - 1);

        u32_msgRecvTime = millis();
        while (u16_lenRead < gk_u16_requestLineSize - 1) {  // make sure enough is read
          uint32_t k_u32_mesgTimeout(50);

          u16_lenRead += g_eca_socks[u8_socket].read((uint8_t*)ca_firstLine + u16_lenRead, gk_u16_requestLineSize - u16_lenRead - 1);

          if ((millis() - u32_msgRecvTime) > k_u32_mesgTimeout) {  // stop trying to read message after 50 ms - assume it's never coming
            //g_eca_socks[u8_socket].stop();
            //Ethernet52.cleanSockets(80);
            return;
          }
        }

#if DISP_TIMING_DEBUG == 1
        lineTime = millis();
#endif

        ca_firstLine[gk_u16_requestLineSize - 1] = 0;  // this will replace a character, though I don't think it is important
        ca_remHeader[gk_u16_requestBuffSize - 1] = 0;

        //Serial.println(ca_firstLine);

        if (strncmp(ca_firstLine, "GET", 3) == 0) {
#if DISP_TIMING_DEBUG == 1
          doneHttp = millis();
#endif
          if (g_b_sdInit) {
            if (strstr(ca_firstLine, ".css") != nullptr) {
              sendWebFile(g_eca_socks[u8_socket], "/ep_style.css", FileType::CSS);  // only css available
            }
            else if (strstr(ca_firstLine, ".gif") != nullptr) {  // will need to expand if more pictures are desired
              sendWebFile(g_eca_socks[u8_socket], "/images/logo_let.gif", FileType::GIF);  // only gif available

              // assuming gif is last request and reset is required, reset
              //if (g_b_reset) {
              //  g_eca_socks[u8_socket].stop();
              //  resetArd();  // this will restart the gateway
              //  return;  // this should never fire
              //}
              break;
            }
// html requests
            else if ((strstr(ca_firstLine, ".htm") != nullptr) || (strncmp(ca_firstLine, "GET / ", 6) == 0)) { // if html or index request
              if ((strncmp(ca_firstLine, "GET / ", 6) == 0) || (strncmp(ca_firstLine, "GET /index.htm", 14) == 0)) {
#if DISP_TIMING_DEBUG
                time1 = millis();
#endif
                sendWebFile(g_eca_socks[u8_socket], "/index.htm", FileType::HTML);
#if DISP_TIMING_DEBUG
                time2 = millis();
#endif
              }
              else if (strncmp(ca_firstLine, "GET /gensetup.htm", 17) == 0) {
                sendWebFile(g_eca_socks[u8_socket], "/gensetup.htm", FileType::HTML);
              }
              else if (strncmp(ca_firstLine, "GET /mtrsetup.htm", 17) == 0) {
                sendWebFile(g_eca_socks[u8_socket], "/mtrsetup.htm", FileType::HTML);
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
                  sendWebFile(g_eca_socks[u8_socket], "/clive.htm", FileType::HTML);        // chilled water
                }
                else if (u8_meterType == 12) {
                  sendWebFile(g_eca_socks[u8_socket], "/slive.htm", FileType::HTML);        // steam
                }
                else {
                  sendWebFile(g_eca_socks[u8_socket], "/elive.htm", FileType::HTML);        // electric 
                }
              }
              else if (strncmp(ca_firstLine, "GET /pastdown.htm", 17) == 0) {

                sendWebFile(g_eca_socks[u8_socket], "/pstdown1.htm", FileType::HTML);
                sendDownLinks(g_eca_socks[u8_socket], ca_firstLine);
                sendWebFile(g_eca_socks[u8_socket], "/pstdown2.htm", FileType::NONE);
              }
              else if (strncmp(ca_firstLine, "GET /pastview.htm", 17) == 0) {  // need to flesh out ideas for this (mimic pastdown?)
                                                                  // graph data?
                sendWebFile(g_eca_socks[u8_socket], "/pastview.htm", FileType::HTML);
              }
              else if (strncmp(ca_firstLine, "GET /reset.htm", 14) == 0) {
                sendWebFile(g_eca_socks[u8_socket], "/reset.htm", FileType::HTML);
              }
              //else if (strncmp(ca_firstLine, "GET /redirect.htm", 17) == 0) {
              //  sendWebFile(g_eca_socks[u8_socket], "/rdct1.htm", FileType::HTML);
              //  sendIP(g_eca_socks[u8_socket]);
              //  sendWebFile(g_eca_socks[u8_socket], "/rdct2.htm", FileType::NONE);

              //  //g_b_reset = true;  // must wait for css and gif requests before restarting
              //}
              else {
                sendWebFile(g_eca_socks[u8_socket], "/nopage.htm", FileType::HTML);
              }
            }
//  xml requests
            else if (strstr(ca_firstLine, ".xml") != nullptr) {
              if (strncmp(ca_firstLine, "GET /gensetup.xml", 17) == 0) {
                sendWebFile(g_eca_socks[u8_socket], "/gensetup.xml", FileType::XML);
                sendXmlEnd(g_eca_socks[u8_socket], XmlFile::GENERAL);
              }
              else if (strncmp(ca_firstLine, "GET /mtrsetup.xml", 17) == 0) {
                sendWebFile(g_eca_socks[u8_socket], "/mtrsetup.xml", FileType::XML);
                sendXmlEnd(g_eca_socks[u8_socket], XmlFile::METER);
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
                liveXML(g_eca_socks[u8_socket]);  // handles http in function
              }
              else if (strncmp(ca_firstLine, "GET /info.xml", 13) == 0) {
                sendWebFile(g_eca_socks[u8_socket], "/mtrsetup.xml", FileType::XML);
                sendXmlEnd(g_eca_socks[u8_socket], XmlFile::INFO);
              }
              else if (strncmp(ca_firstLine, "GET /restart.xml", 13) == 0) {
                sendWebFile(g_eca_socks[u8_socket], "/gensetup.xml", FileType::XML);
                sendXmlEnd(g_eca_socks[u8_socket], XmlFile::GENERAL);

                g_eca_socks[u8_socket].stop();
                resetArd();  // this will restart the gateway
                return;  // this should never fire
              }
              else {  // could not find xml file
                send404(g_eca_socks[u8_socket]);  // handles http in function
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
                sendWebFile(g_eca_socks[u8_socket], filename, FileType::NONE);
              //}
            }
// not htm, css, or xml
            else {  
              send404(g_eca_socks[u8_socket]);
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
            break;  // break from while loop to kill client and end func
          }
          else {
            sendBadSD(g_eca_socks[u8_socket]);
            break;  // don't bother trying to keep connection open - there's no reason to if there's no sd card
          }
        }
// POST http
        else if (strstr(ca_firstLine, "POST") != nullptr) {
          if (strstr(ca_firstLine, "setup.htm")) {
            digitalWrite(gk_s16_epWriteLed, HIGH);
            getPostSetupData(g_eca_socks[u8_socket], ca_remHeader);  // reads and stores POST data to EEPROM
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

            sendPostResp(g_eca_socks[u8_socket]);
         
            break;
          }
        }

      } // end if (ec_client.available())
      //else if (b_idleModbus) {
      //  //handle_modbus(false);
      //}
    } // end while (ec_client.connected() && < timeout)


    //delay(1);      // give the web browser time to receive the data
    //g_eca_socks[u8_socket].stop(); // close the connection

    //Ethernet52.cleanSockets(80);


  //} // end if (ec_client)
} // end handle_http
