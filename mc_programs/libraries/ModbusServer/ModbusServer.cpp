
#include "ModbusServer.h"
#include "MeterLibrary.h"



ModbusServer::ModbusServer(uint8_t u8_serialPort) {
	ModbusServer(u8_serialPort, 255);
}

ModbusServer::ModbusServer(uint8_t u8_serialPort, uint8_t u8_enablePin) {
	m_u8_serialPort = (u8_serialPort > 3) ? 0 : u8_serialPort;
	m_u8_enablePin = ((u8_enablePin > 13) || (u8_enablePin < 2)) ? 255 : u8_enablePin;
	m_u32_mbTimeout = 1000;
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


bool ModbusServer::sendSerialRequest(ModbusRequest mr_mbReq) {
	uint8_t  u8a_txBuffer[8];  // assume funcs 1 - 6
	uint16_t u16_crc = 0xFFFF;
	
	flushSerialRx();
	
	u8a_txBuffer[0] = mr_mbReq.u8_id;
	u8a_txBuffer[1] = mr_mbReq.u8_func;
	u8a_txBuffer[2] = highByte(mr_mbReq.u16_start);
	u8a_txBuffer[3] = lowByte(mr_mbReq.u16_start);
	u8a_txBuffer[4] = highByte(mr_mbReq.u16_length);
	u8a_txBuffer[5] = lowByte(mr_mbReq.u16_length);
	
	for (int ii = 0; ii < 6; ++ii) {
		u16_crc = crc16_update(u16_crc, u8a_txBuffer[ii]);
	}
	u8a_txBuffer[6] = lowByte(u16_crc);
	u8a_txBuffer[7] = highByte(u16_crc);
	
	m_MBSerial->write(u8a_txBuffer, 8);
	m_u32_serialTime = millis();
	return true;
}


bool ModbusServer::sendTcpRequest(EthernetClient52 &ec_client, ModbusRequest mr_mbReq) {
	uint8_t  u8a_txBuffer[12] = {0};
	uint8_t u8_sock = ec_client.getSocketNumber();
	
	flushTcpRx(ec_client);
	
	u8a_txBuffer[5] = 6;
	u8a_txBuffer[6] = mr_mbReq.u8_id;
	u8a_txBuffer[7] = mr_mbReq.u8_func;
	u8a_txBuffer[8] = highByte(mr_mbReq.u16_start);
	u8a_txBuffer[9] = lowByte(mr_mbReq.u16_start);
	u8a_txBuffer[10] = highByte(mr_mbReq.u16_length);
	u8a_txBuffer[11] = lowByte(mr_mbReq.u16_length);
	
	ec_client.write(u8a_txBuffer, 12);
	m_u32a_tcpTime[u8_sock] = millis();
	return true;
}


int ModbusServer::serialAvailable() {
	return m_MBSerial->available();
}


int ModbusServer::tcpAvailable(EthernetClient52 &ec_client) {
	return ec_client.available();
}


uint8_t ModbusServer::recvSerialResponse(ModbusRequest mr_mbReq, uint16_t *u16p_regs, uint8_t u8_numRegs) {
	uint8_t u8_mbStatus = k_u8_MBSuccess;
	uint16_t u16_mbRxSize = 6;
	uint8_t u8p_devResp[256] = {0};
	
	memset(u8p_devResp, 0, 4);  // set first four bytes to 0, first 2 might get reassigned elsewhere
	
	while (!u8_mbStatus) {
		while (m_MBSerial->available()) {  // make sure to get all available data before checking timeout
			if (u16_mbRxSize > 255) {  // message too big, will run into problems if crc pushes past 255
				u8_mbStatus = k_u8_MBIllegalDataValue;
				flushSerialRx();
				return u8_mbStatus;
			}
			
			u8p_devResp[u16_mbRxSize++] = m_MBSerial->read();  // important bit here, storing data
		}
		
		if (u16_mbRxSize > 9) {
			uint16_t u16_givenSize(0);
			switch (mr_mbReq.u8_func) {
				case 1:  // reads
				case 2:
				case 3:
				case 4:
					u16_givenSize = u8p_devResp[8] + 11;  // +11 accounts for tcp hdr, mb hdr, and crc
					break;
				case 5:  // writes
				case 6:
					u16_givenSize = 14;  // +14 accounts for tcp hdr, mb hdr, and crc
					break;
			}
			
			if (u16_givenSize == u16_mbRxSize) {  // if sizes match, if they don't timeout will catch
				// calculate CRC
				uint16_t u16_crc = 0xFFFF;
				for (int ii = 0; ii < (u16_mbRxSize - 2); ++ii) {
					u16_crc = crc16_update(u16_crc, u8p_devResp[ii]);
				}
				
				// verify CRC
				if ((lowByte(u16_crc) != u8p_devResp[u16_mbRxSize - 2]) || 
				    (highByte(u16_crc) != u8p_devResp[u16_mbRxSize - 1])) {
					u8_mbStatus = k_u8_MBInvalidCRC;
				}
				// verify device id
				else if (u8p_devResp[6] != mr_mbReq.u8_id) {
					u8_mbStatus = k_u8_MBInvalidSlaveID;
				}
				// verify function is same regardless of returned errors
				else if ((u8p_devResp[7] & 0x7f) != mr_mbReq.u8_func) {
					u8_mbStatus = k_u8_MBInvalidFunction;
				}
				// check for returned errors
				else if (bitRead(u8p_devResp[8], 7)) {
					u8_mbStatus = u8p_devResp[8];
				}
				
				u8p_devResp[4] = highByte(u16_mbRxSize - 8);  // -8 accounts for tcp hdr and crc
				u8p_devResp[5] = lowByte(u16_mbRxSize - 8);
				// can exit from loop now
				break;
			}
		}
		
		if (serialTimedOut()) {
			u8_mbStatus = k_u8_MBResponseTimedOut;
			break;
		}
	}
	
	if (!u8_mbStatus) {  // if no errors, then move data to register format, otherwise return error
		u8_numRegs = u8array2regs(u8p_devResp, u16p_regs, mr_mbReq.u8_func);
	}
	
	return u8_mbStatus;
}


uint8_t ModbusServer::recvTcpResponse(EthernetClient52 &ec_client, ModbusRequest mr_mbReq, 
                              uint16_t *u16p_regs, uint8_t u8_numRegs) {
	uint8_t u8_mbStatus = k_u8_MBSuccess;
	uint16_t u16_mbRxSize = 0;
	int16_t s16_lenRead;
	uint16_t u16_bytesLeft;
	uint8_t u8p_devResp[256] = {0};
	
	u16_bytesLeft = mr_mbReq.u16_length * 2 + 9; // +9 accounts for tcp header and mb header
	
	while (!u8_mbStatus) {
		while (ec_client.available()) {  // make sure to get all available data before checking timeout
			if (u16_mbRxSize > 255) {  // message too big
				u8_mbStatus = k_u8_MBIllegalDataValue;
				// flushTcpRx(ec_client);  // don't bother flushing tcp, simply close socket
				return u8_mbStatus;
			}
			
			s16_lenRead = ec_client.read(u8p_devResp + u16_mbRxSize, u16_bytesLeft); //264 - u8ModbusADUSize);
			u16_mbRxSize += s16_lenRead;
			u16_bytesLeft -= s16_lenRead;
			
			// u8p_devResp[u16_mbRxSize++] = m_MBSerial->read();  // important bit here, storing data
		}
		
		if (u16_mbRxSize > 9) {
			uint16_t u16_givenSize(0);
			switch (mr_mbReq.u8_func) {
				case 1:  // reads
				case 2:
				case 3:
				case 4:
					u16_givenSize = u8p_devResp[8] + 9;  // +11 accounts for tcp hdr, mb hdr, and crc
					break;
				case 5:  // writes
				case 6:
					u16_givenSize = 12;  // +12 accounts for tcp hdr, mb hdr
					break;
			}
			
			if (u16_givenSize == u16_mbRxSize) {  // if sizes match, if they don't timeout will catch
				// verify device id
				if (u8p_devResp[6] != mr_mbReq.u8_id) {
					u8_mbStatus = k_u8_MBInvalidSlaveID;
				}
				// verify function is same regardless of returned errors
				else if ((u8p_devResp[7] & 0x7f) != mr_mbReq.u8_func) {
					u8_mbStatus = k_u8_MBInvalidFunction;
				}
				// check for returned errors
				else if (bitRead(u8p_devResp[8], 7)) {
					u8_mbStatus = u8p_devResp[8];
				}
				// can exit from loop now
				break;
			}
		}
		
		if (tcpTimedOut(ec_client)) {
			u8_mbStatus = k_u8_MBResponseTimedOut;
			break;
		}
	}
	
	if (!u8_mbStatus) {  // if no errors, then move data to register format, otherwise return error
		u8_numRegs = u8array2regs(u8p_devResp, u16p_regs, mr_mbReq.u8_func);
	}
	return u8_mbStatus;
}


uint8_t ModbusServer::u8array2regs(uint8_t u8p_devResp[], uint16_t *u16p_regs, uint8_t u8_func){
	uint8_t u8_numRegs = u8p_devResp[8] >> 1;
	
	switch (u8_func) {
		case 1:
		case 2:
			for (int ii = 0; ii < u8_numRegs; ++ii) {
				u16p_regs[ii] = word(u8p_devResp[2 * ii + 10], u8p_devResp[2 * ii + 9]);
			}
			
			if (u8p_devResp[8] % 2) {
				u16p_regs[u8_numRegs] = word(0, u8p_devResp[2 * u8_numRegs + 9]);
				u8_numRegs++;
			}
			break;
		case 3:
		case 4:
			for (int ii = 0; ii < u8_numRegs; ++ii) {
				u16p_regs[ii] = word(u8p_devResp[2 * ii + 9], u8p_devResp[2 * ii + 10]);
			}
			break;
		case 5:
		case 6:
			// NOT SURE ABOUT THIS AT ALL
			u16p_regs[0] = word(u8p_devResp[10], u8p_devResp[11]);
			u8_numRegs = 1;
			break;
	}
	
	return u8_numRegs;
}


void ModbusServer::sendResponse(EthernetClient52 ec_client, ModbusRequest mbReq) {
	uint8_t u8a_respBuf[256] = {0};  // actual buffer used to respond
	uint16_t u16a_interBuf[128] = {0};  // used for grabbing data in register format
	uint8_t u8_mbStatus(0);
	uint16_t u16_respLen(0);
	uint8_t u8_numRegs(0);
	
	if (mbReq.u8_flags & 0x20) {  // protocol has timed out, no message from slave device
		u8_mbStatus = k_u8_MBResponseTimedOut;
	}
	else if (mbReq.u8_flags & 0x01) {  // there is a good message over tcp
		u8_mbStatus = recvTcpResponse(ec_client, mbReq, u16a_interBuf, u8_numRegs);
	}
	else {  // there is a good message over serial
		u8_mbStatus = recvSerialResponse(mbReq, u16a_interBuf, u8_numRegs);
	}
	
	if (!u8_mbStatus) {  // good data
		if (mbReq.u8_flags & 0x80) {  // need to translate data via MeterLibrary
			MeterLibBlocks mtrLibBlk(mbReq.u16_start, mbReq.u16_length, mbReq.u8_mtrType);
				
			// void convertToFloat(uint16_t u16p_regs[], uint8_t *const u8p_data);
		}
		else {  // raw data is acceptable to pass back
			if (mbReq.u8_func < 5) {
				u8a_respBuf[6] = mbReq.u8_id;
				u8a_respBuf[7] = mbReq.u8_func;
				
			}
			else {  // (mbReq.u8_func < 7)
				u8a_respBuf[5] = 6;
				u8a_respBuf[6] = mbReq.u8_id;
				u8a_respBuf[7] = mbReq.u8_func;
				u8a_respBuf[8] = highByte(mbReq.u16_start);
				u8a_respBuf[9] = lowByte(mbReq.u16_start);
				u8a_respBuf[10] = highByte(mbReq.u16_length);
				u8a_respBuf[11] = lowByte(mbReq.u16_length);
				
				u16_respLen = 12;
			}
		}
	}
	else {  // error
		u8a_respBuf[5] = 3;
		u8a_respBuf[6] = mbReq.u8_id;
		u8a_respBuf[7] = mbReq.u8_func | 0x80;
		u8a_respBuf[8] = u8_mbStatus;
		
		u16_respLen = 9;
	}
	
	if (u16_respLen > 0) {
		ec_client.write(u8a_respBuf, u16_respLen);
		ec_client.flush();  // do anything?
	}
}


void ModbusServer::setTimeout(uint32_t u32_timeout) {
	m_u32_mbTimeout = u32_timeout;
}
	
	
bool ModbusServer::serialTimedOut() {  // if timed out, return true
	return ((millis() - m_u32_serialTime) > m_u32_mbTimeout) ? true : false;
}
	
	
bool ModbusServer::tcpTimedOut(EthernetClient52 &ec_client) {  // if timed out, return true
	uint8_t u8_sock = ec_client.getSocketNumber();
	
	return ((millis() - m_u32a_tcpTime[u8_sock]) > m_u32_mbTimeout) ? true : false;
}





void ModbusServer::flushSerialRx() {
	while (m_MBSerial->read() != -1);
}


void ModbusServer::flushTcpRx(EthernetClient52 &ec_client) {
	while (ec_client.read() != -1);
}


uint16_t ModbusServer::crc16_update(uint16_t crc, uint8_t a) {
  int i;

  crc ^= a;
  for (i = 0; i < 8; ++i)
  {
    if (crc & 1)
      crc = (crc >> 1) ^ 0xA001;
    else
      crc = (crc >> 1);
  }

  return crc;
}

