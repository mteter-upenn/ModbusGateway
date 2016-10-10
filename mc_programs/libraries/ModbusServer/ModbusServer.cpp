
#include "ModbusServer.h"




ModbusServer::ModbusServer(uint8_t u8_serialPort) {
	ModbusServer(u8_serialPort, 255);
}

ModbusServer::ModbusServer(uint8_t u8_serialPort, uint8_t u8_enablePin) {
	m_u8_serialPort = (u8_serialPort > 3) ? 0 : u8_serialPort;
	m_u8_enablePin = ((u8_enablePin > 13) || (u8_enablePin < 2)) ? 255 : u8_enablePin;
}


void ModbusServer::begin() {
	begin(19200);
}


void ModbusServer::begin(uint16_t u16_baudRate) {
	if (m_u8_enablePin != 255) {
		pinMode(m_u8_enablePin, OUTPUT);
		digitalWrite(m_u8_enablePin, LOW); // LOW is Tx enabled
  }
	
	switch (m_u8_serialPort) {
#if defined(CORE_TEENSY)
		case 3:
			m_MBSerial = &Serial3;
			break;
    case 0:
    default:
			m_MBSerial = &Serial1; ///< Pointer to Serial class object
#else
		case 0:
    default:
      m_MBSerial = &Serial;
#endif
      break;
	}
	
	m_MBSerial->begin(u16_baudRate);
}


bool ModbusServer::sendSerial(ModbusRequest mr_mbReq) {
	
}


bool ModbusServer::sendTcp(EthernetClient52 ec_client, ModbusRequest mr_mbReq) {
	
}


int ModbusServer::serialAvailable() {
	
}


int ModbusServer::tcpAvailable(EthernetClient52 ec_client) {
	
}


uint8_t ModbusServer::recvSerial(ModbusRequest mr_mbReq, uint8_t *u8p_devResp) {
	
}


uint8_t ModbusServer::recvTcp(EthernetClient52 ec_client, ModbusRequest mr_mbReq, uint8_t *u8p_devResp) {
	
}














