#ifndef ethernet52_h
#define ethernet52_h

#define ACH_INSERTION

#include <inttypes.h>
//#include "w5100.h"  // commented out by someone else
#include "IPAddress.h"
#include "EthernetClient52.h"
#include "EthernetServer52.h"
// #include "Dhcp.h"

#define MAX_SOCK_NUM 8  // let MAX_SOCK_NUM be the max number of sockets possible for the device

class EthernetClass52 {
private:
	union {  // ugly way of avoiding a friend class designation in IPAddress.h, trying to keep this library possible 
					 //   without modifications to generic libraries (could always make IPAddress52 lib)
		uint8_t bytes[4]; // IPv4 address
		uint32_t dword;
	} _ethAddress;
	
  // IPAddress _dnsServerAddress;
  // DhcpClass* _dhcp;
	
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
  int begin(uint8_t *mac_address);
  void begin(uint8_t *mac_address, IPAddress local_ip);
  void begin(uint8_t *mac_address, IPAddress local_ip, IPAddress dns_server);
  void begin(uint8_t *mac_address, IPAddress local_ip, IPAddress dns_server, IPAddress gateway);
  void begin(uint8_t *mac_address, IPAddress local_ip, IPAddress dns_server, IPAddress gateway, IPAddress subnet);
	void begin(uint8_t *mac_address, IPAddress local_ip, IPAddress dns_server, IPAddress gateway, IPAddress subnet, 
		uint8_t u8MaxUsedSocks);
	void begin(uint8_t *mac_address, IPAddress local_ip, IPAddress dns_server, IPAddress gateway, IPAddress subnet, 
		uint8_t u8MaxUsedSocks, uint16_t* u16pSocketSizes);
  void begin(uint8_t *mac, IPAddress local_ip, IPAddress dns_server, IPAddress gateway, IPAddress subnet, 
		uint8_t u8MaxUsedSocks, uint16_t* u16pSocketSizes, uint16_t* u16pSocketPorts);
  // int maintain();

	void cleanSockets(uint16_t port);
	
  IPAddress localIP();
  IPAddress subnetMask();
  IPAddress gatewayIP();
  // IPAddress dnsServerIP();

	
  friend class EthernetClient52;
  friend class EthernetServer52;
	friend class EthernetUDP52;
	
	friend uint8_t socketBegin(uint8_t protocol, uint16_t port);
	friend uint8_t socketBegin(uint8_t protocol, uint16_t port, uint8_t sock);
};

extern EthernetClass52 Ethernet52;

#endif
