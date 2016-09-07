#include "w5200.h"
#include "socket52.h"
extern "C" {
#include "string.h"
}

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
			EthernetClient52 client(sock);  // why make client?  only thing needed is status which could be run
																		//   as socketStatus(sock).  Does this create unnecessary overhead
																		//   with the creation and destruction of many clients?
			if (client.status() == SnSR::CLOSED) {
				// Serial.print(F("socket: "));
				// Serial.println(sock, DEC);
				socket(sock, SnMR::TCP, _port, 0);  // initializes this socket on the desired port
				listen(sock);
				EthernetClass52::_server_port[sock] = _port;
				break;
			}
		}
	}  
}

void EthernetServer52::begin(int16_t sock_f) {  // specify desired socket
	if (EthernetClass52::_server_port_mask[sock_f] == _port || !EthernetClass52::_server_port_mask[sock_f]){ 
		// if mask matches port of this server or mask is 0 (catchall)
		EthernetClient52 client(sock_f);
		if (client.status() == SnSR::CLOSED) {
			// Serial.print(F("socket: "));
			// Serial.println(sock_f, DEC);
			socket(sock_f, SnMR::TCP, _port, 0);  // initializes this socket on the desired port
			listen(sock_f);
			EthernetClass52::_server_port[sock_f] = _port;
		}
	}
	else {
		for (int16_t sock = 0; sock < EthernetClass52::_u8MaxUsedSocks; sock++) {
			if (EthernetClass52::_server_port_mask[sock] == _port || !EthernetClass52::_server_port_mask[sock]){ 
				// if mask matches port of this server or mask is 0 (catchall)
				EthernetClient52 client(sock);
				if (client.status() == SnSR::CLOSED) {
					// Serial.print(F("socket: "));
					// Serial.println(sock, DEC);
					socket(sock, SnMR::TCP, _port, 0);  // initializes this socket on the desired port
					listen(sock);
					EthernetClass52::_server_port[sock] = _port;
					break;
				}
			}
		}  
	}
}


void EthernetServer52::accept() { // make sure all sockets on _port are clean
  int listening = 0;

  for (int sock = 0; sock < EthernetClass52::_u8MaxUsedSocks; sock++) {
	  if (EthernetClass52::_server_port_mask[sock] == _port || !EthernetClass52::_server_port_mask[sock]) {
			// if ports match or port mask is 0 (all ports available)
			EthernetClient52 client(sock);

			if (EthernetClass52::_server_port[sock] == _port) {
				uint8_t clStatus = client.status();
				
				if (clStatus == SnSR::LISTEN || clStatus == SnSR::ESTABLISHED) {  // MJT: not sure about adding established
					listening = 1;
				} 
				else if (clStatus == SnSR::CLOSE_WAIT && !client.available()) {
					// Serial.println(F("close wait"));
					client.stop();
				}
				// else {
					// Serial.print(F("extra: "));
					// Serial.println(clStatus);
				// }
			} 
	  }
  }

  if (!listening) {
	  // Serial.println(F(" problem here: "));
    begin();
  }
}

EthernetClient52 EthernetServer52::available() {
  accept();

  for (int sock = 0; sock < EthernetClass52::_u8MaxUsedSocks; sock++) {
	  if (EthernetClass52::_server_port_mask[sock] == _port || !EthernetClass52::_server_port_mask[sock]){
			EthernetClient52 client(sock);
			if (EthernetClass52::_server_port[sock] == _port &&
				(client.status() == SnSR::ESTABLISHED ||
				 client.status() == SnSR::CLOSE_WAIT)) {

#ifdef ACH_INSERTION
				 // ACH - added
				// See if we have identified this one before
				client._dstport = client.getRemotePort();
				
				if (EthernetClass52::_client_port[sock] == 0 ) {  // if _clien_port[] is 0, then we haven't looked at this client yet
					EthernetClass52::_client_port[sock] = client._dstport;
					// client.getRemoteIP(_client_ip);
					// return client; commented out to make sure that client.available() is checked
				}
				else if (EthernetClass52::_client_port[sock] != client._dstport) {
					// Not us!
					continue;
				}  // otherwise, check if client is sending data
				
				if (client.available()) {  // only return if client has data, otherwise could get caught with the previously opened
																	 //   one
					return client;
				}
				// ACH - end of additions
#else		
				if (client.available()) { 
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

size_t EthernetServer52::write(const uint8_t *buffer, size_t size) 
{
  size_t n = 0;
  
  accept();

  for (int sock = 0; sock < EthernetClass52::_u8MaxUsedSocks; sock++) {
	  if (EthernetClass52::_server_port_mask[sock] == _port || !EthernetClass52::_server_port_mask[sock]){
		EthernetClient52 client(sock);

		if (EthernetClass52::_server_port[sock] == _port &&
#ifdef ACH_INSERTION
			// ACH - added
			EthernetClass52::_client_port[sock] == client._srcport && // ACH
#endif
			client.status() == SnSR::ESTABLISHED) {
			n += client.write(buffer, size);
		}
	  }
  }
  
  return n;
}

#if DEBUG_PRINT == 1
void EthernetServer52::printAll() {
	for (int sock = 0; sock < EthernetClass52::_u8MaxUsedSocks; sock++) {
		EthernetClient52 client(sock);
		
		Serial.print("socket "); Serial.print(sock, DEC); Serial.print(":  ");
		Serial.print("port mask: "); Serial.print(EthernetClass52::_server_port_mask[sock], DEC);
		Serial.print(", port: "); Serial.print(EthernetClass52::_server_port[sock], DEC);
		Serial.print(", cl port: "); Serial.print(EthernetClass52::_client_port[sock], DEC);
		Serial.print(", cl stat: 0x"); Serial.print(client.status(), HEX);
		
		if (client.status() == SnSR::ESTABLISHED || client.status() == SnSR::CLOSE_WAIT) {
			Serial.print(", avail: "); Serial.print(client.available(), DEC);
		}
		Serial.println();
	}
}
#endif