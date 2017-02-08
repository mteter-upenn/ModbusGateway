
#if DEBUG_PRINT == 1
#include "w5200.h"
#endif
#include "socket52.h"
// extern "C" {
// #include "string.h"
// }

#include "Ethernet52.h"
#include "EthernetClient52.h"
#include "EthernetServer52.h"

EthernetServer52::EthernetServer52(uint16_t port) {
  _port = port;
	memset(_clientIP, 0, 4);
}

void EthernetServer52::begin() {
	for (int16_t sock = 0; sock < EthernetClass52::_u8MaxUsedSocks; sock++) {
		if (EthernetClass52::_server_port_mask[sock] == _port || !EthernetClass52::_server_port_mask[sock]){ 
			// if mask matches port of this server or mask is 0 (catchall)

			if (socketStatus(sock) == SnSR::CLOSED) {
				socketBegin(SnMR::TCP, _port, sock);
				socketListen(sock);
				break;
			}
		}
	}  
}

void EthernetServer52::begin(int16_t sock_f) {  // specify desired socket
	if (EthernetClass52::_server_port_mask[sock_f] == _port || !EthernetClass52::_server_port_mask[sock_f]){ 
		// if mask matches port of this server or mask is 0 (catchall)

		if (socketStatus(sock_f) == SnSR::CLOSED) {
			socketBegin(SnMR::TCP, _port, sock_f);
			socketListen(sock_f);
		}
	}
	else {  // desired socket didn't match port
		for (int16_t sock = 0; sock < EthernetClass52::_u8MaxUsedSocks; sock++) {
			if (EthernetClass52::_server_port_mask[sock] == _port || !EthernetClass52::_server_port_mask[sock]){ 
				// if mask matches port of this server or mask is 0 (catchall)

				if (socketStatus(sock) == SnSR::CLOSED) {
					socketBegin(SnMR::TCP, _port, sock);
					socketListen(sock);
					break;
				}
			}
		}  
	}
}


void EthernetServer52::accept() { // make sure all sockets on _port are clean
  bool listening = false;

  for (int sock = 0; sock < EthernetClass52::_u8MaxUsedSocks; ++sock) {
	  if (EthernetClass52::_server_port_mask[sock] == _port || !EthernetClass52::_server_port_mask[sock]) {
			// if ports match or port mask is 0 (all ports available)

			if (EthernetClass52::_server_port[sock] == _port) {
				uint8_t u8_sockStatus = socketStatus(sock);
				
				switch (u8_sockStatus) {
					case SnSR::LISTEN:
					case SnSR::ESTABLISHED:
					case SnSR::SYNSENT:
					case SnSR::SYNRECV:
						listening = true;  // should we return here?
						return; // kick once we found a socket that is listening to the desired port
						break;
					case SnSR::CLOSE_WAIT:
						if (socketRecvAvailable(sock) < 1) socketDisconnect(sock);
						break;
					default:
						// if (u8_sockStatus != SnSR::CLOSED) {
							// Serial.print("sock "); Serial.print(sock, DEC); Serial.print(" stat: 0x"); 
							// Serial.println(u8_sockStatus, HEX);
						// }
						break;
				}

			} 
	  }
  }

  if (!listening) begin();
}

EthernetClient52 EthernetServer52::available() {
  accept();

  for (int sock = 0; sock < EthernetClass52::_u8MaxUsedSocks; ++sock) {
	  if (EthernetClass52::_server_port_mask[sock] == _port || !EthernetClass52::_server_port_mask[sock]){
			uint8_t u8_sockStatus = socketStatus(sock);
			
			if (EthernetClass52::_server_port[sock] == _port &&
					(u8_sockStatus == SnSR::ESTABLISHED || u8_sockStatus == SnSR::CLOSE_WAIT)) {

#ifdef ACH_INSERTION
				 // ACH - added
				// See if we have identified this one before
				uint16_t u16_rmtPort = socketRemotePort(sock);
				// client._dstport = client.getRemotePort();
				
				if (EthernetClass52::_client_port[sock] == 0 ) {  // if _clien_port[] is 0, then we haven't looked at this client yet
					EthernetClass52::_client_port[sock] = u16_rmtPort;
				}
				else if (EthernetClass52::_client_port[sock] != u16_rmtPort) {
					// Not us!
					continue;
				}  // otherwise, check if client is sending data
				
				if (socketRecvAvailable(sock) > 0) {  // only return if client has data, otherwise could get caught with the previously opened
																	 //   one
					EthernetClient52 client(sock);
					return client;
				}
				// ACH - end of additions
#else		
				if (socketRecvAvailable(sock) > 0) { 
					EthernetClient52 client(sock);
					// XXX: don't always pick the lowest numbered socket.
					return client;
				}
#endif
			}
	  }
  }

  return EthernetClient52(MAX_SOCK_NUM);  // 
}

size_t EthernetServer52::write(uint8_t b) 
{
  return write(&b, 1);
}

size_t EthernetServer52::write(const uint8_t *buffer, size_t size)  {
  accept();

  for (int sock = 0; sock < EthernetClass52::_u8MaxUsedSocks; sock++) {
	  if (EthernetClass52::_server_port_mask[sock] == _port || !EthernetClass52::_server_port_mask[sock]){
			uint16_t u16_rmtPort = socketRemotePort(sock);
			uint8_t u8_sockStatus = socketStatus(sock);
			
			if (EthernetClass52::_server_port[sock] == _port &&
#ifdef ACH_INSERTION
				// ACH - added
				EthernetClass52::_client_port[sock] == u16_rmtPort && // ACH
#endif
				u8_sockStatus == SnSR::ESTABLISHED) {
					
				socketSend(sock, buffer, size);
				return size;
			}
	  }
  }
  
  return 0;
}

#if DEBUG_PRINT == 1
void EthernetServer52::printAll() {
	for (int sock = 0; sock < EthernetClass52::_u8MaxUsedSocks; sock++) {
		EthernetClient52 client(sock);
		
		Serial.print("socket "); Serial.print(sock, DEC); Serial.print(":  ");
		Serial.print("port mask: "); Serial.print(EthernetClass52::_server_port_mask[sock], DEC);
		Serial.print(", port arr: "); Serial.print(EthernetClass52::_server_port[sock], DEC);
		Serial.print(", port 5200: "); Serial.print(W5200.readSnPORT(sock), DEC);
		Serial.print(", cl port: "); Serial.print(EthernetClass52::_client_port[sock], DEC);
		Serial.print(", cl stat: 0x"); Serial.print(client.status(), HEX);
		
		if (client.status() == SnSR::ESTABLISHED || client.status() == SnSR::CLOSE_WAIT) {
			Serial.print(", avail: "); Serial.print(client.available(), DEC);
		}
		Serial.println();
	}
}
#endif