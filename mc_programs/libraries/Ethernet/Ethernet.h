#ifndef ethernet_h
#define ethernet_h

// #define UPENN_TEENSY_MBGW  shouldn't need this
#define ACH_INSERTION

#include <inttypes.h>
//#include "w5100.h"
#include "IPAddress.h"
#include "EthernetClient.h"
#include "EthernetServer.h"
#include "Dhcp.h"

// #ifdef UPENN_TEENSY_MBGW
// #define MAX_SOCK_NUM 7
// #else
// #define MAX_SOCK_NUM 4
// #endif

#define MAX_SOCK_NUM 8  // let MAX_SOCK_NUM be the max number of sockets possible for the device

class EthernetClass {
private:
  IPAddress _dnsServerAddress;
  DhcpClass* _dhcp;
public:
  static uint8_t _state[MAX_SOCK_NUM];
  static uint16_t _server_port[MAX_SOCK_NUM];
  static uint16_t _server_port_mask[MAX_SOCK_NUM];
#ifdef ACH_INSERTION
  // ACH - added
  static uint16_t _client_port[MAX_SOCK_NUM]; // ACH
#endif

  static uint8_t _u8MaxUsedSocks;

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
  int maintain();

  IPAddress localIP();
  IPAddress subnetMask();
  IPAddress gatewayIP();
  IPAddress dnsServerIP();

  friend class EthernetClient;
  friend class EthernetServer;
};

extern EthernetClass Ethernet;

#endif
