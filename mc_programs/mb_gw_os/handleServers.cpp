#include "handleServers.h"
#include <ModbusStack.h>
#include "globals.h"
#include "mb_names.h"
#include <Ethernet52.h>
#include <ModbusStructs.h>
#include <ModbusServer.h>
#include "handleHTTP.h"
#include "handleRTC.h"

// need set of global arrays for clients, timings, flags, etc
// will need new modbus library, likely just accept arrays and spit out info on success
//ModbusStack mbStack;
// start by transforming 80 server, it will make it easier
void handleServers() {
  bool b_allFreeSocks = false;  // assume there are used sockets - don't worry we'll check first to make sure
  ModbusStack mbStack;
  const uint32_t k_u32_mbTcpTimeout(500);              // timeout for device to hold on to tcp connection after modbus request
  uint8_t u8a_mbSrtBytes[8][2];
  char ca_fileReq[8][REQUEST_LINE_SIZE] = { {0}, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 } };
  
  
  float fa_liveXmlData[9][MAX_NUM_ELEC_VALS];  // 
  int8_t s8a_dataFlags[9][MAX_NUM_ELEC_VALS] = { 0 };
  FileType s16_fileTypes[8];
  FileReq u16_fileReqs[8];
  uint8_t u8_selSlvs[8];
  uint8_t u8_curGrp[8] = { 0 };

  while (!b_allFreeSocks) {
    if (g_b_useNtp && ((millis() - g_u32_rtcNtpLastReset) > NTP_RESET_DELAY)) {
      if (g_ba_clientSocksAvail[0] || g_ba_clientSocksAvail[1]) {  // check if emergency socket is available for udp use
        // if enough time has elapsed and we want to use ntp
        time_t t_localTime(0);

        t_localTime = getNtpTime();

        if (t_localTime != 0) {  // set clock to time gotten from ntp
          Teensy3Clock.set(t_localTime);  // just need to set Teensy3 time, class defined time updates from here
          //setTime(t_localTime);  // this should not be strictly necessary, though update will be delayed
          g_b_rtcGood = true;

          digitalWrite(RTC_FAIL_LED_PIN, LOW);
        }

        g_u32_rtcNtpLastReset = millis();  // reset timer
      }
    }


    b_allFreeSocks = true;  // set true, if anything is active, set false to avoid escape
    // loop through sockets to see if any new ones are available
    //   this is a separate loop so that no sockets get caught hanging - might not actually need it, but it won't hurt
    for (int ii = 0; ii < 8; ++ii) {
      if (g_u16a_socketFlags[ii] & SockFlag_ESTABLISHED) {  
        b_allFreeSocks = false;  // this socket is being used!
      }
      else { // if we haven't started with this socket yet
        uint8_t u8_sockStatus = g_eca_socks[ii].status();

        // SOCKET IS NOT CLOSED OR LISTENING- ASSUME DATA AVAILABLE
        if (!((u8_sockStatus == SnSR::CLOSED) || (u8_sockStatus == SnSR::LISTEN))) {
          uint16_t u16_srcPort = g_eca_socks[ii].getSourcePort();
          char ca_msg[40] = {0};
          g_u32a_socketTimeoutStart[ii] = millis();

          snprintf(ca_msg, 40, "FOUND MESSAGE ON SOCKET %d, PORT %d", ii, u16_srcPort);
          Serial.println(ca_msg);
//          g_es_webServ.printAll(ca_msg);  // doesn't matter what server class instance is used

          if (u16_srcPort == 502) {
            b_allFreeSocks = false;  // this socket is being used!
            g_u16a_socketFlags[ii] = (SockFlag_ESTABLISHED | SockFlag_MODBUS);
          }
          else if (u16_srcPort == 80) {
            b_allFreeSocks = false;  // this socket is being used!
            g_u16a_socketFlags[ii] = (SockFlag_ESTABLISHED | SockFlag_HTTP);
          }
          else {
            // close socket!
            g_eca_socks[ii].stop();
            g_eca_socks[ii].setSocket(ii);
          }
        }
        else if ((ii > 5) && (u8_sockStatus == SnSR::CLOSED)) {  // if emergency/client socket and closed
          g_ba_clientSocksAvail[ii - 6] = true;  // make sure socket declared available for modbus-slave use
        }
      }
    }

    if (b_allFreeSocks) {  // nothing being used
      break;
    }

    for (int ii = 0; ii < 8; ++ii) {  // loop through sockets and handle them
      if ((g_u16a_socketFlags[ii] & SockFlag_ESTABLISHED) && !(g_u16a_socketFlags[ii] & 
          SockFlag_CLIENT)) { // only look at sockets in use and aren't clients to slave devices
        if (g_eca_socks[ii].status() == SnSR::CLOSE_WAIT) { // other side closed socket
//          Serial.println("requestor closed connection");

          uint8_t u8_mbReqInd = mbStack.getReqInd(g_u16a_mbReqUnqId[ii]);
          if (u8_mbReqInd < mbStack.k_u8_maxSize) { 
            if (mbStack[u8_mbReqInd].u8_flags & MRFLAG_isTcp) {  // tcp used
              uint8_t u8_dumSck = (mbStack[u8_mbReqInd].u8_flags & MRFLAG_sckMask);

              g_eca_socks[u8_dumSck].stop();
              g_eca_socks[u8_dumSck].setSocket(u8_dumSck);

              g_ba_clientSocksAvail[u8_dumSck - 6] = true;
            }
            else {  // serial used
              g_b_485avail = true;
            }
            // CLEAN SOCKETS? - main socket should just keep ticking, no reason to here
            // REMOVE FROM STACK
            mbStack.removeByInd(u8_mbReqInd);
            g_u16a_mbReqUnqId[ii] = 0;
          }

          g_eca_socks[ii].stop();
          g_eca_socks[ii].setSocket(ii);

          Ethernet52.cleanSockets(502);
          g_u16a_socketFlags[ii] = SockFlag_LISTEN;
          continue;
        }

        if (g_u16a_socketFlags[ii] & SockFlag_MODBUS) {  // if port 502
          /*uint8_t u8_mbReqInd = mbStack.getReqInd(g_u16a_mbReqUnqId[ii]);*/

          if (!(g_u16a_socketFlags[ii] & SockFlag_READ_REQ)) {  // haven't read modbus request yet or added to stack
            // CHECK IF MESSAGE AVAILABLE TO READ

            if (g_eca_socks[ii].available()) {
//              Serial.println("available > 0");
              // READ, ADD TO STACK
              ModbusRequest mbReq;
              uint8_t u8_mbStatus = g_modbusServer.parseRequest(g_eca_socks[ii], mbReq, u8a_mbSrtBytes[ii]);
              if (!u8_mbStatus) {  // no error in message
                // ADD TO STACK
                g_u16a_mbReqUnqId[ii] = mbStack.add(mbReq, 1);

                uint8_t u8a_respBuf[1] = { 0 };
                char ca_remIp[16] = { 0 };
                char ca_errStr[30] = "ip is ";
                g_eca_socks[ii].remoteIp2Char(ca_remIp);
                strcpy(ca_errStr, ca_remIp);
                storeStringAndArr(ca_errStr, u8a_respBuf , 0, g_u16a_mbReqUnqId[ii], false);

                uint8_t u8_stkInd = mbStack.getReqInd(g_u16a_mbReqUnqId[ii]);
//                Serial.print("added this to "); Serial.print(mbStack.getLength(), DEC);
//                Serial.print(" long stack: "); Serial.println(g_u16a_mbReqUnqId[ii], DEC);

//                mbStack.printReqByInd(u8_stkInd);

                // SET SOCKFLAGS
                if (mbReq.u8_flags & MRFLAG_isTcp) {
                  g_u16a_socketFlags[ii] |= (SockFlag_MBTCP | SockFlag_READ_REQ);
                }
                else {
                  g_u16a_socketFlags[ii] |= (SockFlag_MB485 | SockFlag_READ_REQ);
                }
                // SET TCP TIMER
                g_u32a_socketTimeoutStart[ii] = millis();  // this is reset for activity
              }
              else if (u8_mbStatus == g_modbusServer.k_u8_MBResponseTimedOut) {
//                Serial.print("error: "); Serial.println(u8_mbStatus, DEC);
                g_eca_socks[ii].stop();
                g_eca_socks[ii].setSocket(ii);

                Ethernet52.cleanSockets(502);
                g_u16a_socketFlags[ii] = SockFlag_LISTEN;
                g_u16a_mbReqUnqId[ii] = 0;
                continue;
              }
              else {  // invalid request
                uint8_t u8a_respBuf[9] = { 0 };
                u8a_respBuf[0] = u8a_mbSrtBytes[ii][0];
                u8a_respBuf[1] = u8a_mbSrtBytes[ii][1];
                u8a_respBuf[5] = 3;
                u8a_respBuf[6] = mbReq.u8_vid;
                u8a_respBuf[7] = mbReq.u8_func | 0x80;
                u8a_respBuf[8] = u8_mbStatus;

                g_eca_socks[ii].write(u8a_respBuf, 9);
                g_eca_socks[ii].flush();  // do anything?

                g_u32a_socketTimeoutStart[ii] = millis();
              }
            }
            else if ((millis() - g_u32a_socketTimeoutStart[ii]) > k_u32_mbTcpTimeout) {
//              Serial.println("tcp timeout");

              g_eca_socks[ii].stop();
              g_eca_socks[ii].setSocket(ii);

              Ethernet52.cleanSockets(502);
              g_u16a_socketFlags[ii] = SockFlag_LISTEN;
              g_u16a_mbReqUnqId[ii] = 0;
              continue;
            }
          }

          if (g_u16a_socketFlags[ii] & SockFlag_READ_REQ) {  // request has been read
            g_u32a_socketTimeoutStart[ii] = millis();  // reset timer
          }

          uint8_t u8_mbReqInd = mbStack.getReqInd(g_u16a_mbReqUnqId[ii]);

          // don't use else- otherwise will need to wait for next loop to check everything
          if (mbStack[u8_mbReqInd].u8_flags & (MRFLAG_goodData | MRFLAG_timeout)) {  // if we have, then check for good message return
            // TRANSFER MESSAGE FROM PROTOCOL BUFFER TO CURRENT SOCKET OUTBOUND
//            Serial.println("sending response...");
            g_modbusServer.sendResponse(g_eca_socks[ii], mbStack[u8_mbReqInd], u8a_mbSrtBytes[ii]);
            // SEND THIS MESSAGE THEN RESET TCP TIMEOUT
            g_u32a_socketTimeoutStart[ii] = millis();
            // CLOSE PROTOCOL USED TO TALK TO DEVICE
            if (mbStack[u8_mbReqInd].u8_flags & MRFLAG_isTcp) {  // tcp used
              uint8_t u8_dumSck = (mbStack[u8_mbReqInd].u8_flags & MRFLAG_sckMask);

              g_eca_socks[u8_dumSck].stop();
              g_eca_socks[u8_dumSck].setSocket(u8_dumSck);

              g_ba_clientSocksAvail[u8_dumSck - 6] = true;
            }
            else {  // serial used
              g_b_485avail = true;
            }
            // CLEAN SOCKETS? - main socket should just keep ticking, no reason to here
            // REMOVE FROM STACK
            mbStack.removeByInd(u8_mbReqInd);
            g_u16a_mbReqUnqId[ii] = 0;

            // RESET SOCKFLAGS SO HAVEN'T READ MESSAGE
            g_u16a_socketFlags[ii] &= ~(SockFlag_MB485 | SockFlag_MBTCP | SockFlag_READ_REQ | SockFlag_CLIENT);  // should reset bits 3, 4, and 5, bet this doesn't work
          }  // end if good message or mb timeout
          else if ((millis() - g_u32a_socketTimeoutStart[ii]) > k_u32_mbTcpTimeout) {  // check for time out
            // THIS SHOULD NEVER TRIGGER WITH TIMER RESET DIRECTLY ABOVE, RELY ON MODBUS TIMEOUT TO GET OUT OF LOOP

//            Serial.println("tcp timeout");
            // IF STACK MEMBER PRESENT
            if (u8_mbReqInd < mbStack.k_u8_maxSize) {
            //   CLOSE DEVICE SOCKET
              mbStack[u8_mbReqInd].u8_flags |= MRFLAG_timeout;  // mark device timed out
            //   SEND TIMEOUT MESSAGE TO REQUESTOR
              g_modbusServer.sendResponse(g_eca_socks[ii], mbStack[u8_mbReqInd], u8a_mbSrtBytes[ii]);

              if (mbStack[u8_mbReqInd].u8_flags & MRFLAG_sentMsg) {  // only need to reset protocols if message has been sent
                if (mbStack[u8_mbReqInd].u8_flags & MRFLAG_isTcp) {  // tcp used
                  uint8_t u8_dumSck = (mbStack[u8_mbReqInd].u8_flags & MRFLAG_sckMask);

                  g_eca_socks[u8_dumSck].stop();
                  g_eca_socks[u8_dumSck].setSocket(u8_dumSck);

                  g_ba_clientSocksAvail[u8_dumSck - 6] = true;
                }
                else {  // serial used
                  g_b_485avail = true;
                }
              }
              mbStack.removeByInd(u8_mbReqInd);
              g_u16a_mbReqUnqId[ii] = 0;
            }  // end if request in stack
            //   CLOSE THIS SOCKET
            //   CLEAN SOCKETS?
            //   RESET SOCKFLAGS
            g_eca_socks[ii].stop();
            g_eca_socks[ii].setSocket(ii);

            Ethernet52.cleanSockets(502);
            g_u16a_socketFlags[ii] = SockFlag_LISTEN;
            continue;
          }  // end if else tcp timeout
        }  // end if port 502
        else if (g_u16a_socketFlags[ii] & SockFlag_HTTP) {  // if port 80
          // if haven't read yet
          // if available
          if (!(g_u16a_socketFlags[ii] & SockFlag_READ_REQ)) {  // haven't read request yet
            if (g_eca_socks[ii].available()) {
              s16_fileTypes[ii] = FileType::NONE;
              u16_fileReqs[ii] = FileReq_404;
              u8_selSlvs[ii] = 0;

              g_u16a_socketFlags[ii] |= readHttp(ii, u16_fileReqs[ii], s16_fileTypes[ii], u8_selSlvs[ii], ca_fileReq[ii]);
              g_u16a_socketFlags[ii] |= SockFlag_READ_REQ;
              g_u32a_socketTimeoutStart[ii] = millis();
            }
            else if ((millis() - g_u32a_socketTimeoutStart[ii]) > k_u32_mbTcpTimeout) {  // nothing every available
//              Serial.println("tcp timeout");

              g_eca_socks[ii].stop();
              g_eca_socks[ii].setSocket(ii);

              Ethernet52.cleanSockets(80);
              g_u16a_socketFlags[ii] = SockFlag_LISTEN;
              continue;  // move on to next socket
            }
          }
          
          if (g_u16a_socketFlags[ii] & SockFlag_READ_REQ) {
            if (respondHttp(ii, g_u16a_socketFlags[ii], u16_fileReqs[ii], s16_fileTypes[ii], u8_selSlvs[ii], ca_fileReq[ii], mbStack, u8_curGrp[ii], fa_liveXmlData[ii], s8a_dataFlags[ii])) {
              // good
              g_eca_socks[ii].stop();
              g_eca_socks[ii].setSocket(ii);

              Ethernet52.cleanSockets(80);  // makes sure desired sockets are listening
              g_u16a_socketFlags[ii] = SockFlag_LISTEN;  // reset flag
              continue;
            }
            else {  // need to keep loop running for whatever reason (like waiting for live MB data)
              // need to do anything here?
              g_u32a_socketTimeoutStart[ii] = millis();
            }
          }
          if ((millis() - g_u32a_socketTimeoutStart[ii]) > k_u32_mbTcpTimeout) {  // tcp timeout
            // MUST TAKE CARE OF ANYTHING ADDED TO THE MODBUS STACK HERE!!!!!!!!!!
            uint8_t u8_mbReqInd = mbStack.getReqInd(g_u16a_mbReqUnqId[ii]);
            if (u8_mbReqInd < mbStack.k_u8_maxSize) {
              if (mbStack[u8_mbReqInd].u8_flags & MRFLAG_sentMsg) {  // only need to reset protocols if message has been sent
                if (mbStack[u8_mbReqInd].u8_flags & MRFLAG_isTcp) {  // tcp used
                  uint8_t u8_dumSck = (mbStack[u8_mbReqInd].u8_flags & MRFLAG_sckMask);

                  g_eca_socks[u8_dumSck].stop();
                  g_eca_socks[u8_dumSck].setSocket(u8_dumSck);

                  g_ba_clientSocksAvail[u8_dumSck - 6] = true;
                }
                else {  // serial used
                  g_b_485avail = true;
                }
              }
              mbStack.removeByInd(u8_mbReqInd);
              g_u16a_mbReqUnqId[ii] = 0;
            }  // end if request in stack


            g_eca_socks[ii].stop();
            g_eca_socks[ii].setSocket(ii);

            Ethernet52.cleanSockets(80);  // makes sure desired sockets are listening
            g_u16a_socketFlags[ii] = SockFlag_LISTEN;  // reset flag
            continue;
          }
        }
      }
    }  // end for loop through sockets



    // loop through available protocols and check against stack of requests

    if (mbStack.getLength() > 0) {
      uint8_t u8_stkInd;

      if (g_b_485avail) {
        u8_stkInd = mbStack.getNext485();
        if (u8_stkInd < mbStack.k_u8_maxSize) { // 255 is none in stack
          mbStack[u8_stkInd].u8_flags |= MRFLAG_sentMsg;  // mark sent flag

//          Serial.println("found this serial req in stack:");
//          mbStack.printReqByInd(u8_stkInd);

          g_modbusServer.sendSerialRequest(mbStack[u8_stkInd]);  // SEND REQUEST
          // START TIMER, timer in ModbusServer class

          g_b_485avail = false;
        }
        else {  // no 485 requests exist
          // don't need to do anything
        }
      }
      else {  // serial is flagged as active
        // need ModbusRequest to set flags
        // CHECK IF DATA HAS BEEN RETURNED
        u8_stkInd = mbStack.getLive485();  // find index of request that is currently active on this protocol

        if (u8_stkInd < mbStack.k_u8_maxSize) { // 255 is none in stack
          if (g_modbusServer.serialAvailable()) {
            mbStack[u8_stkInd].u8_flags |= MRFLAG_goodData;  // mark received actual msg flag
//            Serial.println("good data");
          }
          else if (g_modbusServer.serialTimedOut()) {
            mbStack[u8_stkInd].u8_flags |= MRFLAG_timeout;  // mark device timed out
//            Serial.println("timeout");
          }
        }
        else {  // serial active, but nothing in stack, set serial to open and pray
          g_b_485avail = true;
        }
      }  // end if/else serial

      // HAVE NOT COMPLETED CHECK FOR TCP REQUESTS
      for (int ii = 6; ii < 8; ++ii) {
        if (g_ba_clientSocksAvail[ii - 6]) {  // if socket is available for use
          u8_stkInd = mbStack.getNextTcp();
          if (u8_stkInd < mbStack.k_u8_maxSize) {
            mbStack[u8_stkInd].u8_flags |= (MRFLAG_sentMsg | (ii & 0xff));  // mark sent flag
            g_u16a_socketFlags[ii] = SockFlag_ESTABLISHED | SockFlag_CLIENT;

//            Serial.print("found this tcp req in stack for socket "); Serial.println(ii, DEC);
//            mbStack.printReqByInd(u8_stkInd);
            
            // SEND REQUEST
            g_modbusServer.sendTcpRequest(g_eca_socks[ii], mbStack[u8_stkInd]);

            // START TIMER
            g_ba_clientSocksAvail[ii - 6] = false;
          }
          else {
            //Serial.print("nothing in tcp stack for socket "); Serial.println(ii, DEC);
          }
        }
        else {  // socket is in use
          // CHECK IF DATA HAS BEEN RETURNED
          u8_stkInd = mbStack.getLiveTcp(ii);  // aaarrghh on needing to add 6 for ii
          if (u8_stkInd < mbStack.k_u8_maxSize) {
            if (g_modbusServer.tcpAvailable(g_eca_socks[ii])) {
              mbStack[u8_stkInd].u8_flags |= MRFLAG_goodData;
//              Serial.print("good tcp data on socket "); Serial.println(ii, DEC);
            }
            else if (g_modbusServer.tcpTimedOut(g_eca_socks[ii])) {
              mbStack[u8_stkInd].u8_flags |= MRFLAG_timeout;
//              Serial.print("modbus timeout on socket "); Serial.println(ii, DEC);
            }
          }
          else {  // client active, but 
//            Serial.println("socket active, but nothing in stack");

            g_eca_socks[ii].stop();
            g_eca_socks[ii].setSocket(ii);

            g_ba_clientSocksAvail[ii - 6] = true;
          }
        }
      }
    }


    // Loop through all sockets via a server class
    //   avoid sockets known to be currently open
    //   if find new socket, set client array
    //   if there are no new sockets, then set b_openSocks to false or break from here to end this loop

    // loop through all sockets from global array (use flags)
    //   if port 502
    //     if sent_msg
    //       check if msg available from desired protocol source
    //       if msg_ret
    //         handle msg from device, modify as needed
    //         send return to original requester
    //         reset timeout clock?
    //         if tcp, close socket used
    //         update flags to show closed client socket/closed 485
    //         clean sockets
    //     elif not sent_msg
    //       determine protocol (tcp or 485)
    //       if protocol unavailable
    //         continue with next socket
    //       else
    //         adjust request appropriately
    //       note flags
    //       set timeout clock - set every time protocol is unavailable?
    //       send msg
    //
    //     if timeout
    //       close socket gracefully
    //       set flags
    //       clean sockets
    //   if port 80
    //     send to trimmed down version of handleHttp
    //       just need to respond to request
    //     close socket here or in handleHttp?
    //   else
    //     close socket

    // clean sockets?

  }


}
