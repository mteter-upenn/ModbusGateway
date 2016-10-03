
// need set of global arrays for clients, timings, flags, etc
// will need new modbus library, likely just accept arrays and spit out info on success

// start by transforming 80 server, it will make it easier
void handleServers() {
  bool b_openSocks = true;  // assume there are open sockets - don't worry we'll check first to make sure

  while (b_openSocks) {
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
    //   if port 80
    //     send to trimmed down version of handleHttp
    //       just need to respond to request
    //     close socket here or in handleHttp?
    //   else
    //     close socket

    // clean sockets?

  }


}