#ifndef ethernet52_h
#define ethernet52_h

#define ACH_INSERTION

#include <Arduino.h>
#include "IPAddress.h"
#include "EthernetClient52.h"
#include "EthernetServer52.h"
#include <ModbusStructs.h>

#define MAX_SOCK_NUM 8  // let MAX_SOCK_NUM be the max number of sockets possible for the device

class EthernetClass52 {
private:
	static uint8_t _state[MAX_SOCK_NUM];
  static uint16_t _server_port[MAX_SOCK_NUM];
  static uint16_t _server_port_mask[MAX_SOCK_NUM];
#ifdef ACH_INSERTION
  // ACH - added
  static uint16_t _client_port[MAX_SOCK_NUM]; // ACH
	
#endif

  static uint8_t _u8MaxUsedSocks;
	
public:
  // Initialise the Ethernet shield to use the provided MAC address and gain the rest of the
  // configuration through DHCP.
  // Returns 0 if the DHCP configuration failed, and 1 if it succeeded

  void begin(MacArray mac, IpArray local_ip, IpArray dns_server, IpArray gateway, IpArray subnet,
    uint8_t u8MaxUsedSocks, uint16_t* u16pSocketSizes, uint16_t* u16pSocketPorts);

	void cleanSockets(uint16_t port);
	
  IPAddress localIP();
  IPAddress subnetMask();
  IPAddress gatewayIP();
	
  friend class EthernetClient52;
  friend class EthernetServer52;
	friend class EthernetUDP52;
	
	friend uint8_t socketBegin(uint8_t protocol, uint16_t port);
	friend uint8_t socketBegin(uint8_t protocol, uint16_t port, uint8_t sock);

  static uint8_t getMaxUsedSocks() {return _u8MaxUsedSocks;}
  static uint16_t getServerPort(uint8_t u8_sock) {return _server_port[u8_sock];}
  static uint16_t getServerPortMask(uint8_t u8_sock) {return _server_port_mask[u8_sock];}
  static uint16_t getClientPort(uint8_t u8_sock) {return _client_port[u8_sock];}
};

extern EthernetClass52 Ethernet52;

#endif
