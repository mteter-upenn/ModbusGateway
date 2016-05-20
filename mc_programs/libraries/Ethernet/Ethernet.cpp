#include "w5100.h"
#include "Ethernet.h"
#include "Dhcp.h"

// XXX: don't make assumptions about the value of MAX_SOCK_NUM.
// MJT: unsure if {0} will compile correctly
uint8_t EthernetClass::_state[MAX_SOCK_NUM] = {0};
uint16_t EthernetClass::_server_port[MAX_SOCK_NUM] = {0};

#ifdef UPENN_TEENSY_MBGW
uint16_t EthernetClass::_server_port_mask[MAX_SOCK_NUM] = {80, 80, 502, 502, 502, 502, 0};
#else
uint16_t EthernetClass::_server_port_mask[MAX_SOCK_NUM] = {0};
#endif

#ifdef ACH_INSERTION
// ACH - added
uint16_t EthernetClass::_client_port[MAX_SOCK_NUM] = {0}; // ACH
#endif

// #if MAX_SOCK_NUM == 8  // keep
// uint8_t EthernetClass::_state[MAX_SOCK_NUM] = {0, 0, 0, 0, 0, 0, 0, 0 };
// uint16_t EthernetClass::_server_port[MAX_SOCK_NUM] = {0, 0, 0, 0, 0, 0, 0, 0 };
// // ACH - added
// uint16_t EthernetClass::_client_port[MAX_SOCK_NUM] = {0, 0, 0, 0, 0, 0, 0, 0}; // ACH
// #else
// uint8_t EthernetClass::_state[MAX_SOCK_NUM] = {0, 0, 0, 0};
// uint16_t EthernetClass::_server_port[MAX_SOCK_NUM] = {0, 0, 0, 0};
// // ACH - added
// uint16_t EthernetClass::_client_port[MAX_SOCK_NUM] = {0, 0, 0, 0}; // ACH
// #endif

int EthernetClass::begin(uint8_t *mac_address)
{
  static DhcpClass s_dhcp;
  _dhcp = &s_dhcp;


  // Initialise the basic info
  W5100.init();
  SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
  W5100.setMACAddress(mac_address);
  W5100.setIPAddress(IPAddress(0,0,0,0).raw_address());
  SPI.endTransaction();

  // Now try to get our config info from a DHCP server
  int ret = _dhcp->beginWithDHCP(mac_address);
  if(ret == 1)
  {
    // We've successfully found a DHCP server and got our configuration info, so set things
    // accordingly
    SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
    W5100.setIPAddress(_dhcp->getLocalIp().raw_address());
    W5100.setGatewayIp(_dhcp->getGatewayIp().raw_address());
    W5100.setSubnetMask(_dhcp->getSubnetMask().raw_address());
    SPI.endTransaction();
    _dnsServerAddress = _dhcp->getDnsServerIp();
  }

  return ret;
}

void EthernetClass::begin(uint8_t *mac_address, IPAddress local_ip)
{
  // Assume the DNS server will be the machine on the same network as the local IP
  // but with last octet being '1'
  IPAddress dns_server = local_ip;
  dns_server[3] = 1;
  begin(mac_address, local_ip, dns_server);
}

void EthernetClass::begin(uint8_t *mac_address, IPAddress local_ip, IPAddress dns_server)
{
  // Assume the gateway will be the machine on the same network as the local IP
  // but with last octet being '1'
  IPAddress gateway = local_ip;
  gateway[3] = 1;
  begin(mac_address, local_ip, dns_server, gateway);
}

void EthernetClass::begin(uint8_t *mac_address, IPAddress local_ip, IPAddress dns_server, IPAddress gateway)
{
  IPAddress subnet(255, 255, 255, 0);
  begin(mac_address, local_ip, dns_server, gateway, subnet);
}

void EthernetClass::begin(uint8_t *mac, IPAddress local_ip, IPAddress dns_server, IPAddress gateway, IPAddress subnet)
{
  W5100.init();
  SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
  W5100.setMACAddress(mac);
#if ARDUINO > 106 || TEENSYDUINO > 121
  W5100.setIPAddress(local_ip._address.bytes);
  W5100.setGatewayIp(gateway._address.bytes);
  W5100.setSubnetMask(subnet._address.bytes);
#else
  W5100.setIPAddress(local_ip._address);
  W5100.setGatewayIp(gateway._address);
  W5100.setSubnetMask(subnet._address);
#endif
  SPI.endTransaction();
  _dnsServerAddress = dns_server;
}

int EthernetClass::maintain(){
  int rc = DHCP_CHECK_NONE;
  if(_dhcp != NULL){
    //we have a pointer to dhcp, use it
    rc = _dhcp->checkLease();
    switch ( rc ){
      case DHCP_CHECK_NONE:
        //nothing done
        break;
      case DHCP_CHECK_RENEW_OK:
      case DHCP_CHECK_REBIND_OK:
        //we might have got a new IP.
        SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
        W5100.setIPAddress(_dhcp->getLocalIp().raw_address());
        W5100.setGatewayIp(_dhcp->getGatewayIp().raw_address());
        W5100.setSubnetMask(_dhcp->getSubnetMask().raw_address());
        SPI.endTransaction();
        _dnsServerAddress = _dhcp->getDnsServerIp();
        break;
      default:
        //this is actually a error, it will retry though
        break;
    }
  }
  return rc;
}

IPAddress EthernetClass::localIP()
{
  IPAddress ret;
  SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
  W5100.getIPAddress(ret.raw_address());
  SPI.endTransaction();
  return ret;
}

IPAddress EthernetClass::subnetMask()
{
  IPAddress ret;
  SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
  W5100.getSubnetMask(ret.raw_address());
  SPI.endTransaction();
  return ret;
}

IPAddress EthernetClass::gatewayIP()
{
  IPAddress ret;
  SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
  W5100.getGatewayIp(ret.raw_address());
  SPI.endTransaction();
  return ret;
}

IPAddress EthernetClass::dnsServerIP()
{
  return _dnsServerAddress;
}

EthernetClass Ethernet;
