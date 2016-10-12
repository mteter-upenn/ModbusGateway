
// need set of global arrays for clients, timings, flags, etc
// will need new modbus library, likely just accept arrays and spit out info on success

// start by transforming 80 server, it will make it easier
void handleServers() {
  bool b_allFreeSocks = true;  // assume there are open sockets - don't worry we'll check first to make sure
  bool b_485avail = true;  // can assume 485 is open at init
  bool ba_clientSocksAvail[2] = { false, false };  // assume both socks used at init
  ModbusStack mbStack;
  const uint32_t k_u32_mbTcpTimeout(3000);              // timeout for device to hold on to tcp connection after modbus request

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

        // SOCKET IS NOT CLOSED OR LISTENING- ASSUME DATA AVAILABLE
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
          if (!(g_u16a_socketFlags[ii] & SockFlag_READ_REQ)) {  // haven't read modbus request yet or added to stack
            // CHECK IF MESSAGE AVAILABLE TO READ
            // READ, ADD TO STACK
            // SET SOCKFLAGS
            // RESET TCP TIMER
          }
          else if (mbStack[g_u8a_mbReqInd[ii]].u8_tcp485Req & 0x10) {  // if we have, then check for good message return
            // TRANSFER MESSAGE FROM PROTOCOL BUFFER TO CURRENT SOCKET OUTBOUND
            // SEND THIS MESSAGE THEN RESET TIMEOUT
            // CLOSE PROTOCOL USED TO TALK TO DEVICE
            // CLEAN SOCKETS?
            // REMOVE FROM STACK
            // RESET SOCKFLAGS SO HAVEN'T READ MESSAGE
          }
          else if (mbStack[g_u8a_mbReqInd[ii]].u8_tcp485Req & 0x20) {  // check for timeout
            // SEND TIMEOUT MESSAGE TO REQUESTOR
            // RESET TCP TIMEOUT
            // CLOSE PROTOCOL USED TO TALK TO DEVICE (MIGHT BE DONE IN STACK LOOP)
            // CLEAN SOCKETS?
            // REMOVE FROM STACK
            // RESET SOCKFLAGS
          }
          else if ((millis() - g_u32a_socketTimeoutStart[ii]) > k_u32_mbTcpTimeout) {  // check for time out
            // IF STACK MEMBER PRESENT
            //   CLOSE DEVICE SOCKET
            //   SEND TIMEOUT MESSAGE TO REQUESTOR
            //   CLOSE THIS SOCKET
            //   CLEAN SOCKETS?
            //   REMOVE FROM STACK
            //   RESET SOCKFLAGS
            // ELSE
            //   CLOSE THIS SOCKET
            //   CLEAN SOCKETS?
            //   RESET SOCKFLAGS

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
        if (u8_stkInd < mbStack.mk_maxSize) { // 255 is none in stack
          g_modbusServer.sendSerialRequest(mbStack[u8_stkInd]);  // SEND REQUEST
          // START TIMER, timer in ModbusServer class

          //mbStack[u8_stkInd].b_sentReq = true;
          mbStack[u8_stkInd].u8_tcp485Req |= 0x40;  // mark sent flag

          b_485avail = false;
        }
        else {  // no 485 requests exist
          // don't need to do anything
        }
      }
      else {
        // need ModbusRequest to set flags
        // CHECK IF DATA HAS BEEN RETURNED
        u8_stkInd = mbStack.getLive485();  // find index of request that is currently active on this protocol
        if (u8_stkInd < mbStack.mk_maxSize) { // 255 is none in stack
          if (g_modbusServer.serialAvailable()) {
            mbStack[u8_stkInd].u8_tcp485Req |= 0x10;  // mark received actual msg flag
          }
          else if (g_modbusServer.serialTimedOut()) {
            mbStack[u8_stkInd].u8_tcp485Req |= 0x20;  // mark device timed out
          }
        }
        else {  // serial active, but nothing in stack, set serial to open and pray
          b_485avail = true;
        }
      }  // end if/else serial

      for (int ii = 0; ii < 2; ++ii) {
        if (ba_clientSocksAvail[ii]) {  // if socket is available for use
          u8_stkInd = mbStack.getNextTcp();
          if (u8_stkInd < mbStack.mk_maxSize) {
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