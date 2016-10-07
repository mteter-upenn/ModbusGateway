#ifndef ModbusServer_h
#define ModbusServer_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Ethernet52.h>
#include <ModbusStructs.h>

class ModbusServer {
private:
	
public:
	bool sendSerial(ModbusRequest mr_mbReq);
	bool sendTcp(EthernetClient52 ec_client, ModbusRequest mr_mbReq);
	
	int serialAvailable();
	int tcpAvailable(EthernetClient52 ec_client);
	
	uint8_t recvSerial(ModbusRequest mr_mbReq, uint8_t *u8p_devResp);
	uint8_t recvTcp(EthernetClient52 ec_client, ModbusRequest mr_mbReq, uint8_t *u8p_devResp);
}

#endif
