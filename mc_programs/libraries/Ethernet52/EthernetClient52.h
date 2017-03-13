#ifndef ethernetclient52_h
#define ethernetclient52_h

#define ACH_INSERTION

#include "Arduino.h"	
#include "Print.h"
#include "Client.h"
#include "IPAddress.h"
#include "socket52.h"

class EthernetClient52 : public Client {

public:
  EthernetClient52(): _sock(MAX_SOCK_NUM) {}
  EthernetClient52(uint8_t sock): _sock(sock) {}

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
  virtual operator bool() { return _sock < MAX_SOCK_NUM; }
	virtual bool operator==(const bool value) { return bool() == value; }
  virtual bool operator!=(const bool value) { return bool() != value; }
  virtual bool operator==(const EthernetClient52&);
  virtual bool operator!=(const EthernetClient52& rhs) { return !this->operator==(rhs); }
	
  uint8_t getSocketNumber() const { return _sock; }
	bool setSocket(uint8_t u8_sock);
	
  friend class EthernetServer52;
  
  using Print::write;

private:
  static uint16_t _srcport;
  uint8_t _sock;
  
#ifdef ACH_INSERTION
  // ACH - added
  // uint16_t _dstport; // ACH
  
  // ACH - added
public:
  void getRemoteIP(uint8_t remoteIP[4]); // ACH
  uint16_t getRemotePort(); // ACH
#endif

public:
	uint16_t getSourcePort();
};

#endif
