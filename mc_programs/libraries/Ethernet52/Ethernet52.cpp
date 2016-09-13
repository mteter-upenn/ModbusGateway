#include "w5200.h"
#include "Ethernet52.h"
#include "EthernetServer52.h"
#include "socket52.h"

// #include "Dhcp.h"


// XXX: don't make assumptions about the value of MAX_SOCK_NUM.
// MJT: unsure if {0} will compile correctly
uint8_t EthernetClass52::_state[MAX_SOCK_NUM];
uint16_t EthernetClass52::_server_port[MAX_SOCK_NUM];

uint16_t EthernetClass52::_server_port_mask[MAX_SOCK_NUM];
// #ifdef UPENN_TEENSY_MBGW
// uint16_t EthernetClass52::_server_port_mask[MAX_SOCK_NUM] = {80, 80, 502, 502, 502, 502, 0};
// #else
// uint16_t EthernetClass52::_server_port_mask[MAX_SOCK_NUM] = {0};
// #endif

#ifdef ACH_INSERTION
// // ACH - added
uint16_t EthernetClass52::_client_port[MAX_SOCK_NUM]; // ACH
// uint16_t EthernetClass52::_client_port[MAX_SOCK_NUM] = {0}; // ACH
#endif

uint8_t EthernetClass52::_u8MaxUsedSocks = 4;


// #if MAX_SOCK_NUM == 8  // keep
// uint8_t EthernetClass52::_state[MAX_SOCK_NUM] = {0, 0, 0, 0, 0, 0, 0, 0 };
// uint16_t EthernetClass52::_server_port[MAX_SOCK_NUM] = {0, 0, 0, 0, 0, 0, 0, 0 };
// // ACH - added
// uint16_t EthernetClass52::_client_port[MAX_SOCK_NUM] = {0, 0, 0, 0, 0, 0, 0, 0}; // ACH
// #else
// uint8_t EthernetClass52::_state[MAX_SOCK_NUM] = {0, 0, 0, 0};
// uint16_t EthernetClass52::_server_port[MAX_SOCK_NUM] = {0, 0, 0, 0};
// // ACH - added
// uint16_t EthernetClass52::_client_port[MAX_SOCK_NUM] = {0, 0, 0, 0}; // ACH
// #endif

// int EthernetClass52::begin(uint8_t *mac_address)
// {
	// uint16_t u16pSocketSizes[4] = {4, 4, 4, 4};
	// uint8_t i;
	
  // static DhcpClass s_dhcp;
  // _dhcp = &s_dhcp;
  
	// // MJT start
	// // standard block
  // _u8MaxUsedSocks = 4;
	
	// for (i = 0; i < _u8MaxUsedSocks; i++) {
		// _state[i] = 0;
		// _server_port[i] = 0;
	
		// _server_port_mask[i] = 0;
		
// #ifdef ACH_INSERTION
		// // ACH - added
		// _client_port[_u8MaxUsedSocks] = {0}; // ACH
// #endif
	// }
// // MJT end

  // // Initialise the basic info
  // W5200.init(_u8MaxUsedSocks, u16pSocketSizes);
  // SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
  // W5200.setMACAddress(mac_address);
	// W5200.setRetransmissionCount(3);
  // W5200.setIPAddress(IPAddress(0,0,0,0).raw_address());
  // SPI.endTransaction();

  // // Now try to get our config info from a DHCP server
  // int ret = _dhcp->beginWithDHCP(mac_address);
  // if(ret == 1)
  // {
    // // We've successfully found a DHCP server and got our configuration info, so set things
    // // accordingly
    // SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
    // W5200.setIPAddress(_dhcp->getLocalIp().raw_address());
    // W5200.setGatewayIp(_dhcp->getGatewayIp().raw_address());
    // W5200.setSubnetMask(_dhcp->getSubnetMask().raw_address());
    // SPI.endTransaction();
    // _dnsServerAddress = _dhcp->getDnsServerIp();
  // }

  // return ret;
// }

void EthernetClass52::begin(uint8_t *mac_address, IPAddress local_ip)
{
  // Assume the DNS server will be the machine on the same network as the local IP
  // but with last octet being '1'
  IPAddress dns_server = local_ip;
  dns_server[3] = 1;
  begin(mac_address, local_ip, dns_server);
}

void EthernetClass52::begin(uint8_t *mac_address, IPAddress local_ip, IPAddress dns_server)
{
  // Assume the gateway will be the machine on the same network as the local IP
  // but with last octet being '1'
  IPAddress gateway = local_ip;
  gateway[3] = 1;
  begin(mac_address, local_ip, dns_server, gateway);
}

void EthernetClass52::begin(uint8_t *mac_address, IPAddress local_ip, IPAddress dns_server, IPAddress gateway) {
  IPAddress subnet(255, 255, 255, 0);
  begin(mac_address, local_ip, dns_server, gateway, subnet);
}

void EthernetClass52::begin(uint8_t *mac_address, IPAddress local_ip, IPAddress dns_server, IPAddress gateway, IPAddress subnet) {
  begin(mac_address, local_ip, dns_server, gateway, subnet, 4);
}

void EthernetClass52::begin(uint8_t *mac_address, IPAddress local_ip, IPAddress dns_server, IPAddress gateway, IPAddress subnet, 
		uint8_t u8MaxUsedSocks) {
	uint16_t u16pSocketSizes[MAX_SOCK_NUM]; // create as large as possible
	uint8_t i;
	uint8_t ivar;
	
	
	_u8MaxUsedSocks = ((u8MaxUsedSocks > 8) || (u8MaxUsedSocks < 1)) ? 4 : u8MaxUsedSocks;
	
	for (i = 0; i < _u8MaxUsedSocks; i++) {
		ivar = i + _u8MaxUsedSocks;
		
		if (ivar > 7) {
			u16pSocketSizes[i] = 2;
		}
		else if (ivar > 3) {
			u16pSocketSizes[i] = 4;
		}
		else if (ivar > 1) {
			u16pSocketSizes[i] = 8;
		}
		else {
			u16pSocketSizes[i] = 16;
		}
	}
	
	begin(mac_address, local_ip, dns_server, gateway, subnet, _u8MaxUsedSocks, u16pSocketSizes);
}

void EthernetClass52::begin(uint8_t *mac_address, IPAddress local_ip, IPAddress dns_server, IPAddress gateway, IPAddress subnet, 
		uint8_t u8MaxUsedSocks, uint16_t* u16pSocketSizes) {
	uint16_t u16pSocketPorts[MAX_SOCK_NUM]; // create as large as possible
	uint8_t i;
	
	_u8MaxUsedSocks = ((u8MaxUsedSocks > 8) || (u8MaxUsedSocks < 1)) ? 4 : u8MaxUsedSocks;
	
	for (i = 0; i < _u8MaxUsedSocks; i++) {
		u16pSocketPorts[i] = 0;
	}
	
	begin(mac_address, local_ip, dns_server, gateway, subnet, _u8MaxUsedSocks, u16pSocketSizes, u16pSocketPorts);
}

void EthernetClass52::begin(uint8_t *mac, IPAddress local_ip, IPAddress dns_server, IPAddress gateway, IPAddress subnet, 
		uint8_t u8MaxUsedSocks, uint16_t* u16pSocketSizes, uint16_t* u16pSocketPorts) {
			
	uint16_t memSizeTotal = 0;
	
	_u8MaxUsedSocks = ((u8MaxUsedSocks > 8) || (u8MaxUsedSocks < 1)) ? 4 : u8MaxUsedSocks;
	
	for (uint8_t ii = 0; ii < _u8MaxUsedSocks; ++ii) {
		memSizeTotal += u16pSocketSizes[ii];
		
		if (memSizeTotal > 16) {
			// serious problem
			_u8MaxUsedSocks = ii;
			
			if (ii == 0) {
				u16pSocketSizes[ii] = 16;
				_u8MaxUsedSocks = 1;
			}
			
			break;			
		}
	}
	
	
	memcpy(_server_port_mask, u16pSocketPorts, _u8MaxUsedSocks * 2);  // ports to be used
	
	W5200.init(_u8MaxUsedSocks, u16pSocketSizes);
  SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
  W5200.setMACAddress(mac);
	W5200.setRetransmissionCount(3);
	

	_ethAddress.dword = (uint32_t)local_ip;
  W5200.setIPAddress(_ethAddress.bytes);
	_ethAddress.dword = (uint32_t)gateway;
  W5200.setGatewayIp(_ethAddress.bytes);
	_ethAddress.dword = (uint32_t)subnet;
  W5200.setSubnetMask(_ethAddress.bytes);
  SPI.endTransaction();
  // _dnsServerAddress = dns_server;
}

// int EthernetClass52::maintain(){
  // int rc = DHCP_CHECK_NONE;
  // if(_dhcp != NULL){
    // //we have a pointer to dhcp, use it
    // rc = _dhcp->checkLease();
    // switch ( rc ){
      // case DHCP_CHECK_NONE:
        // //nothing done
        // break;
      // case DHCP_CHECK_RENEW_OK:
      // case DHCP_CHECK_REBIND_OK:
        // //we might have got a new IP.
        // SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
        // W5200.setIPAddress(_dhcp->getLocalIp().raw_address());
        // W5200.setGatewayIp(_dhcp->getGatewayIp().raw_address());
        // W5200.setSubnetMask(_dhcp->getSubnetMask().raw_address());
        // SPI.endTransaction();
        // _dnsServerAddress = _dhcp->getDnsServerIp();
        // break;
      // default:
        // //this is actually a error, it will retry though
        // break;
    // }
  // }
  // return rc;
// }


// make sure no sockets on port are closed
// if all such sockets are busy, then open emergency socket (if available) to listen
// if not, then make sure any non established emergency sockets are closed
void EthernetClass52::cleanSockets(uint16_t port) {
	bool b_busySocks = true;
	
	if (port == 0) return;  // bad things could happen
	
	for (int ii = 0; ii < _u8MaxUsedSocks; ++ii) {
		if (_server_port_mask[ii] == port){
			uint8_t u8_sockStat = socketStatus(ii);
			
			if (u8_sockStat == SnSR::CLOSED) {
				socketBegin(SnMR::TCP, port, ii);
				socketListen(ii);
				b_busySocks = false;
			}
			else if (u8_sockStat == SnSR::LISTEN) {
				b_busySocks = false;
			}
			// for any other status, do nothing to the socket and don't change b_busySocks to false			
		}
		else if (_server_port_mask[ii] == 0 && _server_port[ii] == port && b_busySocks) {
			// if any emergency sockets are already listening, then don't bother opening up more
			uint8_t u8_sockStat = socketStatus(ii);
			
			if (u8_sockStat == SnSR::LISTEN) {
				b_busySocks = false;
			}
		}
	}
	
	if (b_busySocks) {
		for (int ii = 0; ii < _u8MaxUsedSocks; ++ii) {
			if (_server_port_mask[ii] == 0) {
				uint8_t u8_sockStat = socketStatus(ii);
				
				if (u8_sockStat == SnSR::CLOSED) {
					socketBegin(SnMR::TCP, port, ii);
					socketListen(ii);
				}
			}
		}
	}
	else {  // close all emergency sockets on given port
		for (int ii = 0; ii < _u8MaxUsedSocks; ++ii) {
			if (_server_port_mask[ii] == 0 && _server_port[ii] == port) {
				uint8_t u8_sockStat = socketStatus(ii);
				
				if (u8_sockStat == SnSR::LISTEN) socketClose(ii);
			}
		}
	}
}


IPAddress EthernetClass52::localIP() {
  SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
  W5200.getIPAddress(_ethAddress.bytes);  // ret.raw_address()
  SPI.endTransaction();

	return IPAddress(_ethAddress.bytes);
}

IPAddress EthernetClass52::subnetMask() {
  SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
  W5200.getSubnetMask(_ethAddress.bytes);
  SPI.endTransaction();
	
	return IPAddress(_ethAddress.bytes);
}

IPAddress EthernetClass52::gatewayIP() {
  SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
  W5200.getGatewayIp(_ethAddress.bytes);
  SPI.endTransaction();
	
	return IPAddress(_ethAddress.bytes);
}

// IPAddress EthernetClass52::dnsServerIP() {
  // return _dnsServerAddress;
// }

EthernetClass52 Ethernet52;
