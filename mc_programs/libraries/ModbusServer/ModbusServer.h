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
	uint8_t m_u8_serialPort;
	uint8_t m_u8_enablePin;
	
#if defined(ARDUINO_ARCH_AVR)
	HardwareSerial* m_MBSerial = &Serial; ///< Pointer to Serial class object
#elif defined(CORE_TEENSY)  // if teensy3.0 or greater
	HardwareSerial* m_MBSerial = &Serial1; ///< Pointer to Serial class object
#else
  #error "This library only supports boards with an AVR or SAM processor. Please open an issue at https://github.com/4-20ma/ModbusMaster/issues and indicate which processor/platform you're using."
#endif

public:
	ModbusServer(): m_u8_serialPort(3), m_u8_enablePin(255) {}
	ModbusServer(uint8_t u8_serialPort); // : m_u8_serialPort(u8_serialPort), m_u8_enablePin(255) {}
	ModbusServer(uint8_t u8_serialPort, uint8_t u8_enablePin); // : m_u8_serialPort(u8_serialPort), m_u8_enablePin(u8_enablePin) {}
	
	void begin();
	void begin(uint16_t u16_baudRate);
	
	bool sendSerial(ModbusRequest mr_mbReq);
	bool sendTcp(EthernetClient52 ec_client, ModbusRequest mr_mbReq);
	
	int serialAvailable();
	int tcpAvailable(EthernetClient52 ec_client);
	
	uint8_t recvSerial(ModbusRequest mr_mbReq, uint8_t *u8p_devResp);
	uint8_t recvTcp(EthernetClient52 ec_client, ModbusRequest mr_mbReq, uint8_t *u8p_devResp);
};

#endif
