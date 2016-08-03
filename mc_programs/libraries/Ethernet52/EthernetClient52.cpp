#include "w5200.h"
#include "socket52.h"

extern "C" {
  #include "string.h"
}

#include "Arduino.h"

#include "Ethernet52.h"
#include "EthernetClient52.h"
#include "EthernetServer52.h"
// #include "Dns52.h"

uint16_t EthernetClient52::_srcport = 1024;

EthernetClient52::EthernetClient52() : _sock(MAX_SOCK_NUM) {
}

EthernetClient52::EthernetClient52(uint8_t sock) : _sock(sock) {
}


int EthernetClient52::connect(const char* host, uint16_t port) {
	return 0;
}
// int EthernetClient52::connect(const char* host, uint16_t port) {
  // // Look up the host first
  // int ret = 0;
  // DNSClient52 dns;
  // IPAddress remote_addr;

  // dns.begin(Ethernet52.dnsServerIP());
  // ret = dns.getHostByName(host, remote_addr);
  // if (ret == 1) {
    // return connect(remote_addr, port);
  // } else {
    // return ret;
  // }
// }

int EthernetClient52::connect(IPAddress ip, uint16_t port) {
  if (_sock != MAX_SOCK_NUM)
    return 0;

  for (int i = 0; i < EthernetClass52::_u8MaxUsedSocks; i++) {
		if (EthernetClass52::_server_port_mask[i] == port || !EthernetClass52::_server_port_mask[i]){ 
			uint8_t s = socketStatus(i);
			if (s == SnSR::CLOSED || s == SnSR::FIN_WAIT || s == SnSR::CLOSE_WAIT) {
				_sock = i;
				break;
			}
		}
  }

  if (_sock == MAX_SOCK_NUM)
    return 0;

  _srcport++;
  if (_srcport == 0) _srcport = 1024;
  socket(_sock, SnMR::TCP, _srcport, 0);

  if (!::connect(_sock, rawIPAddress(ip), port)) {
    _sock = MAX_SOCK_NUM;
    return 0;
  }

  while (status() != SnSR::ESTABLISHED) {
    delay(1);
    if (status() == SnSR::CLOSED) {
      _sock = MAX_SOCK_NUM;
      return 0;
    }
  }

  return 1;
}

size_t EthernetClient52::write(uint8_t b) {
  return write(&b, 1);
}

size_t EthernetClient52::write(const uint8_t *buf, size_t size) {
  if (_sock == MAX_SOCK_NUM) {
    setWriteError();
    return 0;
  }
  if (!send(_sock, buf, size)) {
    setWriteError();
    return 0;
  }
  return size;
}

int EthernetClient52::available() {
  if (_sock != MAX_SOCK_NUM)
    return recvAvailable(_sock);
  return 0;
}

int EthernetClient52::read() {
  uint8_t b;
  if ( recv(_sock, &b, 1) > 0 )
  {
    // recv worked
    return b;
  }
  else
  {
    // No data available
    return -1;
  }
}

int EthernetClient52::read(uint8_t *buf, size_t size) {
  return recv(_sock, buf, size);
}

int EthernetClient52::peek() {
  uint8_t b;
  // Unlike recv, peek doesn't check to see if there's any data available, so we must
  if (!available())
    return -1;
  ::peek(_sock, &b);
  return b;
}

void EthernetClient52::flush() {
  while (available())
    read();
}

void EthernetClient52::stop() {
  if (_sock == MAX_SOCK_NUM)
    return;

  // attempt to close the connection gracefully (send a FIN to other side)
  disconnect(_sock);
  unsigned long start = millis();

  // wait a second for the connection to close
  while (status() != SnSR::CLOSED && millis() - start < 1000)
    delay(1);

  forceClose();
}


void EthernetClient52::forceClose() {
	// if it hasn't closed, close it forcefully
  if (status() != SnSR::CLOSED)
    close(_sock);

// #ifdef UPENN_TEENSY_MBGW
	// if (_sock == MAX_SOCK_NUM - 1) {
		// EthernetClass52::_server_port[_sock] = 0;
	// }
// #else
  // EthernetClass52::_server_port[_sock] = 0;
// #endif

	if (EthernetClass52::_server_port_mask[_sock] == 0) {
		EthernetClass52::_server_port[_sock] = 0;
	}
	
#ifdef ACH_INSERTION
  // ACH - added
  EthernetClass52::_client_port[_sock] = 0; // ACH
#endif
  _sock = MAX_SOCK_NUM;
}


uint8_t EthernetClient52::connected() {
  if (_sock == MAX_SOCK_NUM) return 0;
  
  uint8_t s = status();
  return !(s == SnSR::LISTEN || s == SnSR::CLOSED || s == SnSR::FIN_WAIT ||
    (s == SnSR::CLOSE_WAIT && !available()));
}

uint8_t EthernetClient52::status() {
  if (_sock == MAX_SOCK_NUM) return SnSR::CLOSED;
  return socketStatus(_sock);
}

// the next function allows us to use the client returned by
// EthernetServer52::available() as the condition in an if-statement.

EthernetClient52::operator bool() {
  return _sock != MAX_SOCK_NUM;
}

#ifdef ACH_INSERTION
// ACH - added
void EthernetClient52::getRemoteIP(uint8_t remoteIP[]) { // ACH
	W5200.readSnDIPR(_sock, remoteIP);
	return; // remoteIP;
}
 
uint16_t EthernetClient52::getRemotePort() { // ACH
	return W5200.readSnDPORT(_sock);
}
#endif
