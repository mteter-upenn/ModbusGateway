#include "w5200.h"
#include "Ethernet52.h"
#include "EthernetServer52.h"
#include "socket52.h"


// XXX: don't make assumptions about the value of MAX_SOCK_NUM.
// MJT: unsure if {0} will compile correctly
uint8_t EthernetClass52::_state[MAX_SOCK_NUM];
uint16_t EthernetClass52::_server_port[MAX_SOCK_NUM];

uint16_t EthernetClass52::_server_port_mask[MAX_SOCK_NUM];

#ifdef ACH_INSERTION
// // ACH - added
uint16_t EthernetClass52::_client_port[MAX_SOCK_NUM]; // ACH
#endif

uint8_t EthernetClass52::_u8MaxUsedSocks = 4;

// COMMENTED OUT TO EXPERIMENT WITH DIFFFERENT ARGUMENT DATA TYPES
//void EthernetClass52::begin(uint8_t *mac_address, IPAddress local_ip)
//{
//  // Assume the DNS server will be the machine on the same network as the local IP
//  // but with last octet being '1'
//  IPAddress dns_server = local_ip;
//  dns_server[3] = 1;
//  begin(mac_address, local_ip, dns_server);
//}

//void EthernetClass52::begin(uint8_t *mac_address, IPAddress local_ip, IPAddress dns_server)
//{
//  // Assume the gateway will be the machine on the same network as the local IP
//  // but with last octet being '1'
//  IPAddress gateway = local_ip;
//  gateway[3] = 1;
//  begin(mac_address, local_ip, dns_server, gateway);
//}

//void EthernetClass52::begin(uint8_t *mac_address, IPAddress local_ip, IPAddress dns_server, IPAddress gateway) {
//  IPAddress subnet(255, 255, 255, 0);
//  begin(mac_address, local_ip, dns_server, gateway, subnet);
//}

//void EthernetClass52::begin(uint8_t *mac_address, IPAddress local_ip, IPAddress dns_server, IPAddress gateway, IPAddress subnet) {
//  begin(mac_address, local_ip, dns_server, gateway, subnet, 4);
//}

//void EthernetClass52::begin(uint8_t *mac_address, IPAddress local_ip, IPAddress dns_server, IPAddress gateway, IPAddress subnet,
//		uint8_t u8MaxUsedSocks) {
//	uint16_t u16pSocketSizes[MAX_SOCK_NUM]; // create as large as possible
//	uint8_t i;
//	uint8_t ivar;
	
	
//	_u8MaxUsedSocks = ((u8MaxUsedSocks > 8) || (u8MaxUsedSocks < 1)) ? 4 : u8MaxUsedSocks;
	
//	for (i = 0; i < _u8MaxUsedSocks; i++) {
//		ivar = i + _u8MaxUsedSocks;
		
//		if (ivar > 7) {
//			u16pSocketSizes[i] = 2;
//		}
//		else if (ivar > 3) {
//			u16pSocketSizes[i] = 4;
//		}
//		else if (ivar > 1) {
//			u16pSocketSizes[i] = 8;
//		}
//		else {
//			u16pSocketSizes[i] = 16;
//		}
//	}
	
//	begin(mac_address, local_ip, dns_server, gateway, subnet, _u8MaxUsedSocks, u16pSocketSizes);
//}

//void EthernetClass52::begin(uint8_t *mac_address, IPAddress local_ip, IPAddress dns_server, IPAddress gateway, IPAddress subnet,
//		uint8_t u8MaxUsedSocks, uint16_t* u16pSocketSizes) {
//	uint16_t u16pSocketPorts[MAX_SOCK_NUM]; // create as large as possible
//	uint8_t i;
	
//	_u8MaxUsedSocks = ((u8MaxUsedSocks > 8) || (u8MaxUsedSocks < 1)) ? 4 : u8MaxUsedSocks;
	
//	for (i = 0; i < _u8MaxUsedSocks; i++) {
//		u16pSocketPorts[i] = 0;
//	}
	
//	begin(mac_address, local_ip, dns_server, gateway, subnet, _u8MaxUsedSocks, u16pSocketSizes, u16pSocketPorts);
//}

void EthernetClass52::begin(MacArray mac, IpArray local_ip, IpArray dns_server, IpArray gateway, IpArray subnet,
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
  W5200.setMACAddress(mac.u8a_mac);
	W5200.setRetransmissionCount(3);
	

  W5200.setIPAddress(local_ip.u8a_ip);
  W5200.setGatewayIp(gateway.u8a_ip);
  W5200.setSubnetMask(subnet.u8a_ip);
  SPI.endTransaction();
}



// make sure no sockets on port are closed
// if all such sockets are busy, then open emergency socket (if available) to listen
// if not, then make sure any non active (set to listen) emergency sockets are closed
void EthernetClass52::cleanSockets(uint16_t port) {
	bool b_busySocks = true;
	
	if (port == 0) return;  // bad things could happen
	
	for (int ii = 0; ii < _u8MaxUsedSocks; ++ii) {
		if (_server_port_mask[ii] == port){
			uint8_t u8_sockStat = socketStatus(ii);
			// Serial.print("socket "); Serial.print(ii, DEC); Serial.print(" stat: 0x"); Serial.println(u8_sockStat, HEX);
			
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
	
	if (b_busySocks) {  // if there all sockets under this port are busy (ESTABLISHED), then open emergency socket 
		for (int ii = 0; ii < _u8MaxUsedSocks; ++ii) {
			if (_server_port_mask[ii] == 0) {  // if emergency socket
				uint8_t u8_sockStat = socketStatus(ii);
				
				if (u8_sockStat == SnSR::CLOSED) {  // if not currently in use, listen
					socketBegin(SnMR::TCP, port, ii);
					socketListen(ii);
				}
			}
		}
	}
	else {  // close all emergency sockets on given port that are acting as servers (LISTEN)
		for (int ii = 0; ii < _u8MaxUsedSocks; ++ii) {
			if (_server_port_mask[ii] == 0 && _server_port[ii] == port) {
				uint8_t u8_sockStat = socketStatus(ii);
				
				if (u8_sockStat == SnSR::LISTEN) socketClose(ii);
			}
		}
	}
}


IPAddress EthernetClass52::localIP() {
  IpArray ipStruct;

  SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
  W5200.getIPAddress(ipStruct.u8a_ip);  // ret.raw_address()
  SPI.endTransaction();

  return IPAddress(ipStruct.u8a_ip);
}

IPAddress EthernetClass52::subnetMask() {
  IpArray ipStruct;

  SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
  W5200.getIPAddress(ipStruct.u8a_ip);  // ret.raw_address()
  SPI.endTransaction();
	
  return IPAddress(ipStruct.u8a_ip);
}

IPAddress EthernetClass52::gatewayIP() {
  IpArray ipStruct;

  SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
  W5200.getIPAddress(ipStruct.u8a_ip);  // ret.raw_address()
  SPI.endTransaction();
	
  return IPAddress(ipStruct.u8a_ip);
}

EthernetClass52 Ethernet52;
