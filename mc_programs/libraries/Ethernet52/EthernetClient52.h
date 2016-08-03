#ifndef ethernetclient52_h
#define ethernetclient52_h

#define ACH_INSERTION

#include "Arduino.h"	
#include "Print.h"
#include "Client.h"
#include "IPAddress.h"

class EthernetClient52 : public Client {

public:
  EthernetClient52();
  EthernetClient52(uint8_t sock);

  uint8_t status();
  virtual int connect(IPAddress ip, uint16_t port);
  virtual int connect(const char *host, uint16_t port);
  virtual size_t write(uint8_t);
  virtual size_t write(const uint8_t *buf, size_t size);
  virtual int available();
  virtual int read();
  virtual int read(uint8_t *buf, size_t size);
  virtual int peek();
  virtual void flush();
  virtual void stop();
	virtual void forceClose();
  virtual uint8_t connected();
  virtual operator bool();

  friend class EthernetServer52;
  
  using Print::write;

private:
  static uint16_t _srcport;
  uint8_t _sock;
  
#ifdef ACH_INSERTION
  // ACH - added
  uint16_t _dstport; // ACH
  
  // ACH - added
  void getRemoteIP(uint8_t remoteIP[]); // ACH
  uint16_t getRemotePort(); // ACH
#endif
};

#endif
