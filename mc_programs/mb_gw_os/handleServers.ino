
// need set of global arrays for clients, timings, flags, etc
// will need new modbus library, likely just accept arrays and spit out info on success

// start by transforming 80 server, it will make it easier
void handleServers() {
  bool b_allFreeSocks = true;  // assume there are open sockets - don't worry we'll check first to make sure
  bool b_485avail = true;  // can assume 485 is open at init
  bool ba_clientSocksAvail[2] = { false, false };  // assume both socks used at init
  
  while (b_allFreeSocks) {
    b_allFreeSocks = true;  // set true, if anything is active, set false to avoid escape

    for (int ii = 0; ii < 8; ++ii) {
      if (g_u16a_socketFlags[ii] & SockFlag_ESTABLISHED) {  
        b_allFreeSocks = false;  // this socket is being used!
      }
      else {// if we haven't started with this socket yet
        uint8_t u8_sockStatus = socketStatus(ii);

        if (!((u8_sockStatus == SnSR::CLOSED) || (u8_sockStatus == SnSR::LISTEN))) {  //
          uint16_t u16_srcPort = socketSourcePort(ii);

          b_allFreeSocks = false;  // this socket is being used!

          g_eca_socks[ii] = 

        }
      }
    }

    if (b_allFreeSocks) {
      break;
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