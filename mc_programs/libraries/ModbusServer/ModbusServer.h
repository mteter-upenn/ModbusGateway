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
	
	uint32_t m_u32_mbTimeout;  // in ms
	uint32_t m_u32_serialTime;  // start time for serial
	uint32_t m_u32a_tcpTime[8];  // start time for each socket
	
	
#if defined(ARDUINO_ARCH_AVR)
	HardwareSerial* m_MBSerial = &Serial; ///< Pointer to Serial class object
#elif defined(CORE_TEENSY)  // if teensy3.0 or greater
	HardwareSerial* m_MBSerial = &Serial1; ///< Pointer to Serial class object
#else
  #error "This library only supports boards with an AVR or SAM processor. Please open an issue at https://github.com/4-20ma/ModbusMaster/issues and indicate which processor/platform you're using."
#endif

	uint16_t crc16_update(uint16_t crc, uint8_t a);
	void flushSerialRx();
	void flushTcpRx(EthernetClient52 &ec_client);
public:
	ModbusServer(): m_u8_serialPort(3), m_u8_enablePin(255), m_u32_mbTimeout(1000) {}
	ModbusServer(uint8_t u8_serialPort); // : m_u8_serialPort(u8_serialPort), m_u8_enablePin(255) {}
	ModbusServer(uint8_t u8_serialPort, uint8_t u8_enablePin); // : m_u8_serialPort(u8_serialPort), m_u8_enablePin(u8_enablePin) {}
	
	void begin();
	void begin(uint16_t u16_baudRate);
	
	bool sendSerial(ModbusRequest mr_mbReq);
	bool sendTcp(EthernetClient52 &ec_client, ModbusRequest mr_mbReq);
	
	int serialAvailable();
	int tcpAvailable(EthernetClient52 &ec_client);
	
	uint8_t recvSerial(ModbusRequest mr_mbReq, uint8_t *u8p_devResp);
	uint8_t recvTcp(EthernetClient52 &ec_client, ModbusRequest mr_mbReq, uint8_t *u8p_devResp);
	
	void setTimeout(uint32_t u32_timeout);
	
	bool serialTimedOut();
	bool tcpTimedOut(EthernetClient52 &ec_client);
	
	static const uint8_t k_u8_MBSuccess                    = 0x00;
	static const uint8_t k_u8_MBIllegalFunction            = 0x01;  // slave not capable of function
	static const uint8_t k_u8_MBIllegalDataAddress         = 0x02;
	static const uint8_t k_u8_MBIllegalDataValue           = 0x03;
	static const uint8_t k_u8_MBSlaveDeviceFailure         = 0x04;
	static const uint8_t k_u8_MBAcknowledge                = 0x05;
	static const uint8_t k_u8_MBSlaveDeviceBusy            = 0x06;
	static const uint8_t k_u8_MBNegAcknowledge             = 0x07;
	static const uint8_t k_u8_MBMemoryParityError          = 0x08;
	static const uint8_t k_u8_MBGatewayPathUnavail         = 0x0a;
	static const uint8_t k_u8_MBGatewayTargetFailed        = 0x0b;
	
	static const uint8_t k_u8_MBInvalidSlaveID             = 0xE0;
	static const uint8_t k_u8_MBInvalidFunction            = 0xE1;  // returned function doesn't match
	static const uint8_t k_u8_MBResponseTimedOut           = 0xE2;
	static const uint8_t k_u8_MBInvalidCRC                 = 0xE3;
	static const uint8_t k_u8_MBInvalidClient              = 0xE4;
};

#endif
