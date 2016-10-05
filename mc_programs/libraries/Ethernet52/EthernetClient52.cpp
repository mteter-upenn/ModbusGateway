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


int EthernetClient52::connect(const char* host, uint16_t port) {
	return 0;
}

int EthernetClient52::connect(IPAddress ip, uint16_t port) {
	if (_sock < MAX_SOCK_NUM) {
		if (socketStatus(_sock) != SnSR::CLOSED) socketDisconnect(_sock);
		_sock = MAX_SOCK_NUM;
	}
	if (ip == IPAddress(0ul) || ip == IPAddress(0xFFFFFFFFul)) return 0;
	
	_sock = socketBegin(SnMR::TCP, 0);  // sending 0 for port means it will be a global ticker (good)
	
	if (_sock >= MAX_SOCK_NUM) return 0;
	socketConnect(_sock, rawIPAddress(ip), port);  // the destination port is important for server peers
	
	while (1) {
		uint8_t stat = socketStatus(_sock);
		if (stat == SnSR::ESTABLISHED) return 1;
		if (stat == SnSR::CLOSED) return 0;
		delay(1);
	}
}

size_t EthernetClient52::write(uint8_t b) {
  return write(&b, 1);
}

size_t EthernetClient52::write(const uint8_t *buf, size_t size) {
  if (_sock == MAX_SOCK_NUM) {
    setWriteError();
    return 0;
  }
  if (socketSend(_sock, buf, size)) {
    return size;
  }
	setWriteError();
  return 0;
  
}

int EthernetClient52::available() {
  if (_sock < MAX_SOCK_NUM)
    return socketRecvAvailable(_sock);
  return 0;
}

int EthernetClient52::read() {
  uint8_t b;
	
	if (socketRecv(_sock, &b, 1) > 0) return b;
	return -1;
}

int EthernetClient52::read(uint8_t *buf, size_t size) {
	if (_sock >= MAX_SOCK_NUM) return 0;
  return socketRecv(_sock, buf, size);
}

int EthernetClient52::peek() {
	if (_sock >= MAX_SOCK_NUM) return -1;
	if (!available()) return -1;
	return socketPeek(_sock);
}


void EthernetClient52::flush()
{
	// ::flush(_sock);  // not sure if this is right
	// TODO: Wait for transmission to complete
}

void EthernetClient52::stop() {
  if (_sock >= MAX_SOCK_NUM)
    return;

  // attempt to close the connection gracefully (send a FIN to other side)
  socketDisconnect(_sock);
  unsigned long start = millis();

  // wait a second for the connection to close
  while (socketStatus(_sock) != SnSR::CLOSED && millis() - start < 1000)
    delay(1);

  forceClose();
}


void EthernetClient52::forceClose() {
	// if it hasn't closed, close it forcefully
  if (socketStatus(_sock) != SnSR::CLOSED)
    socketClose(_sock);

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
  if (_sock >= MAX_SOCK_NUM) return 0;
  
  uint8_t s = socketStatus(_sock);
  return !(s == SnSR::LISTEN || s == SnSR::CLOSED || s == SnSR::FIN_WAIT ||
    (s == SnSR::CLOSE_WAIT && !available()));
}

bool EthernetClient52::setSocket(uint8_t u8_sock) {
	if (u8_sock >= MAX_SOCK_NUM) {
		return false;
	}
	else {
		_sock = u8_sock;
		return true;
	}
}

uint8_t EthernetClient52::status() {
  if (_sock >= MAX_SOCK_NUM) return SnSR::CLOSED;
  return socketStatus(_sock);
}

// the next function allows us to use the client returned by
// EthernetServer52::available() as the condition in an if-statement.

bool EthernetClient52::operator==(const EthernetClient52& rhs) {
	if (_sock != rhs._sock) return false;
	if (_sock >= MAX_SOCK_NUM) return false;
	if (rhs._sock >= MAX_SOCK_NUM) return false;
	return true;
}

#ifdef ACH_INSERTION
// ACH - added
void EthernetClient52::getRemoteIP(uint8_t remoteIP[4]) { // ACH
	// if (_sock >= MAX_SOCK_NUM) return SnSR::CLOSED;
	socketRemoteIP(_sock, remoteIP);
	return; // remoteIP;
}
 
uint16_t EthernetClient52::getRemotePort() { // ACH
	if (_sock >= MAX_SOCK_NUM) return 0;
	return socketRemotePort(_sock);
}
#endif

uint16_t EthernetClient52::getSourcePort() { // ACH
	if (_sock >= MAX_SOCK_NUM) return 0;
	return socketSourcePort(_sock);
}