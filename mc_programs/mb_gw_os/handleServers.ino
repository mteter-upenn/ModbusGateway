
// need set of global arrays for clients, timings, flags, etc
// will need new modbus library, likely just accept arrays and spit out info on success

// start by transforming 80 server, it will make it easier
void handleServers() {
  bool b_allFreeSocks = true;  // assume there are open sockets - don't worry we'll check first to make sure
  bool b_485avail = true;  // can assume 485 is open at init
  bool ba_clientSocksAvail[2] = { false, false };  // assume both socks used at init
  ModbusStack mbStack;

  while (b_allFreeSocks) {
    b_allFreeSocks = true;  // set true, if anything is active, set false to avoid escape

    // loop through sockets to see if any new ones are available
    //   this is a separate loop so that no sockets get caught hanging - might not actually need it, but it won't hurt
    for (int ii = 0; ii < 8; ++ii) {
      if (g_u16a_socketFlags[ii] & SockFlag_ESTABLISHED) {  
        b_allFreeSocks = false;  // this socket is being used!
      }
      else { // if we haven't started with this socket yet
        //uint8_t u8_sockStatus = socketStatus(ii);
        uint8_t u8_sockStatus = g_eca_socks[ii].status();

        if (!((u8_sockStatus == SnSR::CLOSED) || (u8_sockStatus == SnSR::LISTEN))) {  //
          //uint16_t u16_srcPort = socketSourcePort(ii);
          uint16_t u16_srcPort = g_eca_socks[ii].getSourcePort();
          //Serial.print("not closed or listen but 0x"); Serial.println(u8_sockStatus, HEX);

          if (u16_srcPort == 502) {
            b_allFreeSocks = false;  // this socket is being used!
            g_u16a_socketFlags[ii] = (SockFlag_ESTABLISHED | SockFlag_MODBUS);
          }
          else if (u16_srcPort == 80) {
            b_allFreeSocks = false;  // this socket is being used!
            g_u16a_socketFlags[ii] = (SockFlag_ESTABLISHED | SockFlag_HTTP);
            //Serial.print("socket "); Serial.print(ii, DEC); Serial.println(" active on 80");
          }
          else {
            // close socket!
            g_eca_socks[ii].stop();
            g_eca_socks[ii].setSocket(ii);
            //socketDisconnect(ii);  // this does not check or force if actually closed
          }
        }
      }
    }

    if (b_allFreeSocks) {  // nothing being used
      break;
    }

    for (int ii = 0; ii < 8; ++ii) {  // loop through sockets and handle them
      if (g_u16a_socketFlags[ii] & SockFlag_ESTABLISHED) { // only look at sockets in use
        if (g_u16a_socketFlags[ii] & SockFlag_MODBUS) {  // if port 502
          if (g_u16a_socketFlags[ii] & SockFlag_SENT_MSG) {

          }
          else {  // have not relayed message to modbus device
            if (!(g_u16a_socketFlags[ii] & SockFlag_READ_MSG)) {  // haven't read modbus request yet

            }

          }
        }
        else if (g_u16a_socketFlags[ii] & SockFlag_HTTP) {  // if port 80
          //THIS ALL NEEDS TO CHANGE TO HANDLE LIVEXML REQUESTS IF A MODBUS STACK IS GOING TO BE USED
          handle_http(ii);
          
          g_eca_socks[ii].stop();
          g_eca_socks[ii].setSocket(ii);

          Ethernet52.cleanSockets(80);  // makes sure desired sockets are listening
          g_u16a_socketFlags[ii] = SockFlag_LISTEN;  // reset flag
        }
      }
    }

    // loop through all available modbus requests
    if (mbStack.getLength() > 0) {
      uint8_t u8_stkInd;

      if (b_485avail) {
        u8_stkInd = mbStack.getNext485();
        if (u8_stkInd != 255) {
          // SEND REQUEST
          mbStack[u8_stkInd].b_sentReq = true;
          // START TIMER
          b_485avail = false;
        }
        else {  // no 485 requests exist

        }
      }
      else {
        // CHECK IF DATA HAS BEEN RETURNED
        
        //b_485avail = true;
      }

      for (int ii = 0; ii < 2; ++ii) {
        if (ba_clientSocksAvail[ii]) {  // if socket is available for use
          u8_stkInd = mbStack.getNextTcp();
          if (u8_stkInd != 255) {
            // SEND REQUEST
            mbStack[u8_stkInd].b_sentReq = true;
            // START TIMER
            ba_clientSocksAvail[ii] = false;
          }
        }
        else {  // socket is in use
          // CHECK IF DATA HAS BEEN RETURNED

          //ba_clientSocksAvail[ii] = true;
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