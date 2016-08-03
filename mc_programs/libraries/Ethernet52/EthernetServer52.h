#ifndef ethernetserver52_h
#define ethernetserver52_h

#define ACH_INSERTION

#include "Server.h"

class EthernetClient52;

class EthernetServer52 : 
public Server {
private:
  uint16_t _port;
  void accept();
public:
	uint8_t _clientIP[4];
  EthernetServer52(uint16_t);
  EthernetClient52 available();
  virtual void begin();
	virtual void begin(int16_t sock_f); // specify desired socket
  virtual size_t write(uint8_t);
  virtual size_t write(const uint8_t *buf, size_t size);
  using Print::write;
};

#endif
