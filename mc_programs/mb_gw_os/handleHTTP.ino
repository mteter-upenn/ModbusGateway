void handle_http() {
  bool post_fl = false;                              // set true if http request is POST
  bool fnd_len = false;                              // set true to find length of POST message
  uint16_t post_len = 0;                             // length of POST message
  uint16_t i;                                        // tickers
  uint8_t meter;                                     // type of meter, identifies register mapping in eeprom -> X.x.x
  char *mtr_ind;                                   // index of 'METER' in GET request
  //uint8_t reqFlag = 0;                               // flag for different http requests [live.xml, mtrsetup.xml]
  char HTTP_req[REQ_BUF_SZ] = {0};                   // buffered HTTP request stored as null terminated string
  uint8_t req_index = 0;                             // index into HTTP_req buffer
  char ch;                                           // used for reading GET request with extra information
  char c;
#if DISP_TIMING_DEBUG == 1
  uint32_t gotClient, doneHttp, doneFind, doneSend;  // times for debugging
#endif
//  bool testBool = true;                              // flag used for debug
  
  EthernetClient client = serv_web.available();  // try to get client

  if (client) {  // got client?
      boolean currentLineIsBlank = true;
#if DISP_TIMING_DEBUG == 1
      gotClient = millis();
#endif
      HTTP_req[REQ_BUF_SZ - 1] = 0;

      while (client.connected()) {
          if (client.available()) {   // client data available to read
//            if (testBool){
//              Serial.println(client.available(), DEC);
//              testBool = false;
//            }

            c = client.read(); // read 1 byte (character) from client
            
            // buffer first part of HTTP request in HTTP_req array (string)
            // leave last element in array as 0 to null terminate string (REQ_BUF_SZ - 1)
            if (req_index < (REQ_BUF_SZ - 1)) {
                HTTP_req[req_index] = c;          // save HTTP request character
                req_index++;
            }

            //Serial.print(c);    // print HTTP request character to serial monitor
            
            if ((req_index == 4) && (strstr(HTTP_req, "POST"))){
              post_fl = true;  // http request is POST
            }
            if (post_fl == true){  // http request is POST
              if (post_cont.searchString("Content-Length: ", 16)){
                fnd_len = true;
                post_fl = false;
                post_cont.clear();
              }
              
              post_cont.put(c);
            }

            if (fnd_len){
              if (!((c == '\n') || (c == '\r'))){
                post_len = post_len * 10 + (c - '0');
              }
              else{
                fnd_len = false;
              }
            }

            
              // last line of client request is blank and ends with \n
              // respond to client only after last line received
              if (c == '\n' && currentLineIsBlank) {
#if DISP_TIMING_DEBUG == 1
                doneHttp = millis();
#endif
                  // send a standard http response header
                if (sdInit){ 
                  if (strstr(HTTP_req, "GET")){
                    if (strstr(HTTP_req, ".css")){
                      sendWebFile(client, "/ep_style.css");
                      //webFile = SD.open("ep_style.css");        // open style file
                    }
                    else if (strstr(HTTP_req, ".gif") != NULL) {  // will need to expand if more pictures are desired
                      sendGifHdr(client);
                      sendWebFile(client, "/images/logo_let.gif");

                      if (bReset) {
                        client.stop();
                        resetArd();
                        return;
                      }
                    }
                    else if (strstr(HTTP_req, ".htm") || strstr(HTTP_req, "GET / ")){ // if html or index request
                      if (strstr(HTTP_req, "GET / ") || strstr(HTTP_req, "GET /index.htm")) {
                        sendWebFile(client, "/index.htm");
                        //webFile = SD.open("index.htm");        // open web page file
                      }
                      else if (strstr(HTTP_req, "GET /gensetup.htm")) {
                        sendWebFile(client, "/gensetup.htm");
                        //webFile = SD.open("gensetup.htm");        // open web page file
                      }
                      else if (strstr(HTTP_req, "GET /mtrsetup.htm")) {
                        sendWebFile(client, "/mtrsetup.htm");
                        //webFile = SD.open("mtrsetup.htm");        // open web page file
                      }
                      else if (strstr(HTTP_req, "live.htm")) {
                        mtr_ind = strstr(HTTP_req, "METER=");

                        if (mtr_ind){
                          
                          selSlv = 0;
                          for (i = 0; i < 3; i++){  // search all of HTTP_req
                            ch = HTTP_req[i + (mtr_ind - HTTP_req) + 6];

                            if ((ch == '&') || (ch == ' ')){
                              break;
                            }
                            else{
                              selSlv = selSlv * 10 + (ch - '0');
                            }
                          }
                          
                          if (selSlv > EEPROM.read(mtr_strt)){  // if greater than number of meters listed
                            selSlv = 1;
                          }
                        }  

                        meter = slv_typs[(selSlv - 1)][0];
                        //meter = EEPROM.read(mtr_strt + selSlv * 8 - 7);  // turn meter from slave index to meter type X.x.x
                        
                        if (meter == 11){
                          sendWebFile(client, "/clive.htm");
                           //webFile = SD.open("clive.htm");        // chilled water
                        }
                        else if (meter == 12){
                          sendWebFile(client, "/slive.htm");
                           //webFile = SD.open("slive.htm");        // steam
                        }
                        else{
                          sendWebFile(client, "/elive.htm");
                           //webFile = SD.open("elive.htm");        // electric
                        }
                      }
                      else if (strstr(HTTP_req, "GET /pastdown.htm")) {
#if DISP_TIMING_DEBUG
                        time1 = millis();
#endif
                        sendWebFile(client, "/pstdown1.htm");
                        sendDownLinks(client, HTTP_req);
                        sendWebFile(client, "/pstdown2.htm");
#if DISP_TIMING_DEBUG
                        time2 = millis();
#endif
                        //webFile = SD.open("pstdown1.htm");        // open web page file
                        //reqFlag = 4;
                      }
                      else if (strstr(HTTP_req, "GET /pastview.htm")) {  // need to flesh out ideas for this (mimic pastdown?)
                        // graph data?
                        sendWebFile(client, "/pastview.htm");
                        //webFile = SD.open("pastview.htm");        // open web page file
                      }
                      else if (strstr(HTTP_req, "GET /reset.htm")) {
                        sendWebFile(client, "/reset.htm");
                        //webFile = SD.open("reset.htm");        // open web page file
                      }
                      else if (strstr(HTTP_req, "GET /redirect.htm")) {
                        sendWebFile(client, "/rdct1.htm");
                        sendIP(client);
                        sendWebFile(client, "/rdct2.htm");

                        bReset = true;
                        /*client.stop();
                        resetArd();
                        return;*/
                      }
                      else{
                        sendWebFile(client, "/nopage.htm");
                        //webFile = SD.open("nopage.htm");
//                        send404(client);
//                        break;
                      }
                    }
                    else if (strstr(HTTP_req, ".xml")){
                      if (strstr(HTTP_req, "GET /gensetup.xml")) {
                        sendWebFile(client, "/gensetup.xml");
                        sendXmlEnd(client, 3);
                        //webFile = SD.open("gensetup.xml");
                      }
                      else if (strstr(HTTP_req, "GET /mtrsetup.xml")) {
                        //webFile = SD.open("mtrsetup.xml");
                        sendWebFile(client, "/mtrsetup.xml");  // AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
                        sendXmlEnd(client, 2);
                        //reqFlag = 2;
                      }
                      else if (strstr(HTTP_req, "GET /data.xml")) {
                        mtr_ind = strstr(HTTP_req, "METER=");
                         
                        if (mtr_ind){
                          selSlv = 0;
                          for (i = 0; i < 3; i++){  // search all of HTTP_req
                            ch = HTTP_req[i = (mtr_ind - HTTP_req) + 6];

                            if ((ch == '&') || (ch == ' ')){
                              break;
                            }
                            else{
                              selSlv = selSlv * 10 + (ch - '0');
                            }
                          }
                          
                          if (selSlv > EEPROM.read(mtr_strt)){  // if greater than number of meters listed
                            selSlv = 1;
                          }
                        }
                        liveXML(client);  // handles http in function
                        break;
                      }
                      else if (strstr(HTTP_req, "GET /info.xml")) {
                        sendWebFile(client, "/mtrsetup.xml");  // AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
                        sendXmlEnd(client, 1);
                        //webFile = SD.open("mtrsetup.xml");
                        //reqFlag = 1;
                      }
                      else{  // could not find xml file
                        send404(client);  // handles http in function
                        break;
                      }   
                    }
                    else if (strstr(HTTP_req, ".TXT") || strstr(HTTP_req, ".CSV")) {
                      if (strstr(HTTP_req, "GET /TEST.CSV")) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Disposition: attachment; filename=test.txt");
                        client.println("Connection: close\n");

                        sendWebFile(client, "/test.CSV");
                        //webFile = SD.open("test.CSV");
                        //reqFlag = 3;
                      }
                      else if (strstr(HTTP_req, "GET /TEST.TXT")) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Disposition: attachment; filename=test.txt");
                        client.println("Connection: close\n");

                        sendWebFile(client, "test.txt");
                        //webFile = SD.open("test.txt");
                        //reqFlag = 3;
                      }
                      else {  
                        char filename[36] = {0};
                        
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
                        /*client.println("HTTP/1.1 200 OK");
                        client.println("Content-Disposition: attachment; filename=test.txt");
                        client.println("Connection: close\n");*/
                        sendWebFile(client, filename);
                        break;
                      }
                    }
                    else{  // not htm, css, or xml
                      send404(client);
                      break;
                    }

#if DISP_TIMING_DEBUG == 1
                    doneFind = millis();
#endif
                    //sendWebFile(client, reqFlag);  // reset buffer index and all buffer elements to 0 in function

#if DISP_TIMING_DEBUG == 1
                    doneSend = millis();

                    Serial.print(F("HTTP Header: "));
                    Serial.println((doneHttp - gotClient), DEC);
                    Serial.print(F("Find: "));
                    Serial.println((doneFind - doneHttp), DEC);
                    Serial.print(F("Message: "));
                    Serial.println((doneSend - doneFind), DEC);
                    Serial.print(F("time2 - time1: "));
                    Serial.println((time2 - time1), DEC);
#endif
                                   
                    break;
                  } // end else if GET
                  else if (strstr(HTTP_req, "POST")){
                    if (strstr(HTTP_req, "setup.htm")){
                      digitalWrite(epWriteLed, HIGH);
                      getPostSetupData(client, post_len);  // reads and stores POST data to EEPROM
                      digitalWrite(epWriteLed, LOW);

                      digitalWrite(sdWriteLed, HIGH);
                      if (strstr(HTTP_req, "mtrsetup.htm")){
                        writeMtrSetupFile();
                      }
                      else if (strstr(HTTP_req, "gensetup.htm")){
                        writeGenSetupFile();
                      }
                      digitalWrite(sdWriteLed, LOW);
                            
                      sendPostResp(client);

                      /*client.stop();
                      resetArd();
                      return;*/
                      break;
                    }
                    else{
                      sendWebFile(client, "nopage.htm");
                      //webFile = SD.open("nopage.htm");
                      //sendWebFile(client, reqFlag);  // reset buffer index and all buffer elements to 0 in function
                      break;
                    }
                  } // end else if POST
                  else{  // not GET or POST
                    send404(client);
                    break;
                  }
                }
                else{  // sd card could not be initialized, send back standard response
                  sendBadSD(client);
                  break;
                }
              } // end blank line
              // every line of text received from the client ends with \r\n
              if (c == '\n') {
                  // last character on line of received text
                  // starting new line with next character read
                  currentLineIsBlank = true;
              } 
              else if (c != '\r') {
                  // a text character was received from client
                  currentLineIsBlank = false;
              }
          } // end if (client.available())
      } // end while (client.connected())
      delay(1);      // give the web browser time to receive the data
      client.stop(); // close the connection
  } // end if (client)
} // end handle_http
