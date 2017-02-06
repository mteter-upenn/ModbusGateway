#ifndef ModbusServer_h
#define ModbusServer_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Ethernet52.h>
#include <ModbusStructs.h>
#include <Time.h>
#include <SD.h>

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
	uint8_t u8array2regs(uint8_t u8p_devResp[], uint16_t *u16p_regs, uint8_t u8_func);
public:
	ModbusServer(): m_u8_serialPort(3), m_u8_enablePin(255), m_u32_mbTimeout(1000) {}
	ModbusServer(uint8_t u8_serialPort); // : m_u8_serialPort(u8_serialPort), m_u8_enablePin(255) {}
	ModbusServer(uint8_t u8_serialPort, uint8_t u8_enablePin); // : m_u8_serialPort(u8_serialPort), m_u8_enablePin(u8_enablePin) {}
	
	void begin();
	void begin(uint16_t u16_baudRate);
	
	bool sendSerialRequest(ModbusRequest mr_mbReq);
	bool sendTcpRequest(EthernetClient52 &ec_client, ModbusRequest mr_mbReq);
	
	int serialAvailable();
	int tcpAvailable(EthernetClient52 &ec_client);
	
	uint8_t recvSerialResponse(ModbusRequest mr_mbReq, uint16_t *u16p_regs, uint8_t &u8_numBytes);
	uint8_t recvTcpResponse(ModbusRequest mr_mbReq, uint16_t *u16p_regs, uint8_t &u8_numBytes);
	
	// ec_client is requestor

	void sendResponse(EthernetClient52 &ec_client, const ModbusRequest &mbReq,const uint8_t u8a_strtBytes[2]);
	uint8_t returnResponse(const ModbusRequest &mbReq,const uint8_t u8a_strtBytes[2], uint8_t u8a_respBuf[256], uint16_t &u16_respLen);
	
	uint8_t parseRequest(EthernetClient52 &ec_client, ModbusRequest &mbReq, 
	                     uint8_t u8a_strtBytes[2]);
	
	
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
	static const uint8_t k_u8_MBGatewayPathUnavail         = 0x0a;  // 10
	static const uint8_t k_u8_MBGatewayTargetFailed        = 0x0b;  // 11
	
	static const uint8_t k_u8_MBInvalidSlaveID             = 0xE0;  // 224
	static const uint8_t k_u8_MBInvalidFunction            = 0xE1;  // 225 - returned function doesn't match
	static const uint8_t k_u8_MBResponseTimedOut           = 0xE2;  // 226
	static const uint8_t k_u8_MBInvalidCRC                 = 0xE3;  // 227
	static const uint8_t k_u8_MBInvalidClient              = 0xE4;  // 228
};

void digitalClockDisplay(time_t t);
void printDigits(int digits);
void print3SpaceDigits(uint8_t num);
void write3SpaceDigits(File sdFile, uint8_t num);
void storeStringAndArr(const char *k_cp_string, uint8_t *u8p_arr, uint16_t u16_arrLen, bool b_showTime = false);
#endif
