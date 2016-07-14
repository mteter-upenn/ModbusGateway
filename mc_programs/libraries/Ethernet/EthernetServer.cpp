#include "w5100.h"
#include "socket.h"
extern "C" {
#include "string.h"
}

#include "Ethernet.h"
#include "EthernetClient.h"
#include "EthernetServer.h"

EthernetServer::EthernetServer(uint16_t port)
{
  _port = port;
}

void EthernetServer::begin()
{
// #ifdef UPENN_TEENSY_MBGW
// Serial.println(F("defined"));
// #else
	// Serial.println(F("undefined"));
// #endif
	for (int16_t sock = 0; sock < EthernetClass::_u8MaxUsedSocks; sock++) {
		if (EthernetClass::_server_port_mask[sock] == _port || !EthernetClass::_server_port_mask[sock]){ 
			// if mask matches port of this server or mask is 0 (catchall)
			EthernetClient client(sock);
			if (client.status() == SnSR::CLOSED) {
				// Serial.print(F("socket: "));
				// Serial.println(sock, DEC);
				socket(sock, SnMR::TCP, _port, 0);  // initializes this socket on the desired port
				listen(sock);
				EthernetClass::_server_port[sock] = _port;
				break;
			}
		}
	}  
}

void EthernetServer::begin(int16_t sock_f) {  // specify desired socket
	if (EthernetClass::_server_port_mask[sock_f] == _port || !EthernetClass::_server_port_mask[sock_f]){ 
		// if mask matches port of this server or mask is 0 (catchall)
		EthernetClient client(sock_f);
		if (client.status() == SnSR::CLOSED) {
			// Serial.print(F("socket: "));
			// Serial.println(sock_f, DEC);
			socket(sock_f, SnMR::TCP, _port, 0);  // initializes this socket on the desired port
			listen(sock_f);
			EthernetClass::_server_port[sock_f] = _port;
		}
	}
	else {
		for (int16_t sock = 0; sock < EthernetClass::_u8MaxUsedSocks; sock++) {
			if (EthernetClass::_server_port_mask[sock] == _port || !EthernetClass::_server_port_mask[sock]){ 
				// if mask matches port of this server or mask is 0 (catchall)
				EthernetClient client(sock);
				if (client.status() == SnSR::CLOSED) {
					// Serial.print(F("socket: "));
					// Serial.println(sock, DEC);
					socket(sock, SnMR::TCP, _port, 0);  // initializes this socket on the desired port
					listen(sock);
					EthernetClass::_server_port[sock] = _port;
					break;
				}
			}
		}  
	}
}


void EthernetServer::accept()
{
  int listening = 0;

  for (int sock = 0; sock < EthernetClass::_u8MaxUsedSocks; sock++) {
	  if (EthernetClass::_server_port_mask[sock] == _port || !EthernetClass::_server_port_mask[sock]){
		EthernetClient client(sock);

		if (EthernetClass::_server_port[sock] == _port) {
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

EthernetClient EthernetServer::available() {
  accept();

  for (int sock = 0; sock < EthernetClass::_u8MaxUsedSocks; sock++) {
	  if (EthernetClass::_server_port_mask[sock] == _port || !EthernetClass::_server_port_mask[sock]){
		EthernetClient client(sock);
		if (EthernetClass::_server_port[sock] == _port &&
			(client.status() == SnSR::ESTABLISHED ||
			 client.status() == SnSR::CLOSE_WAIT)) {

#ifdef ACH_INSERTION
			 // ACH - added
			// See if we have identified this one before
			if (EthernetClass::_client_port[sock] == 0 ) {
				client._dstport = client.getRemotePort();
				EthernetClass::_client_port[sock] = client._dstport;
				return client;
			}
			if (EthernetClass::_client_port[sock] != client._dstport) {
				// Not us!
				continue;
			}
			// ACH - end of additions
			
			return client;
#else		
			if (client.available()) { 
			// XXX: don't always pick the lowest numbered socket.
			return client;
			}
#endif
		}
	  }
  }

  return EthernetClient(MAX_SOCK_NUM);  // 
}

size_t EthernetServer::write(uint8_t b) 
{
  return write(&b, 1);
}

size_t EthernetServer::write(const uint8_t *buffer, size_t size) 
{
  size_t n = 0;
  
  accept();

  for (int sock = 0; sock < EthernetClass::_u8MaxUsedSocks; sock++) {
	  if (EthernetClass::_server_port_mask[sock] == _port || !EthernetClass::_server_port_mask[sock]){
		EthernetClient client(sock);

		if (EthernetClass::_server_port[sock] == _port &&
#ifdef ACH_INSERTION
			// ACH - added
			EthernetClass::_client_port[sock] == client._srcport && // ACH
#endif
			client.status() == SnSR::ESTABLISHED) {
			n += client.write(buffer, size);
		}
	  }
  }
  
  return n;
}
