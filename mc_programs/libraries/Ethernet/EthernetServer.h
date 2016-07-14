#ifndef ethernetserver_h
#define ethernetserver_h

#define ACH_INSERTION

#include "Server.h"

class EthernetClient;

class EthernetServer : 
public Server {
private:
  uint16_t _port;
  void accept();
public:
  EthernetServer(uint16_t);
  EthernetClient available();
  virtual void begin();
	virtual void begin(int16_t sock_f); // specify desired socket
  virtual size_t write(uint8_t);
  virtual size_t write(const uint8_t *buf, size_t size);
  using Print::write;
};

#endif
