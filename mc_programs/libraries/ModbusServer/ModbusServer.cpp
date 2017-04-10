
#include "ModbusServer.h"
#include "MeterLibrary.h"
#include <IPAddress.h>


ModbusServer::ModbusServer(uint8_t u8_serialPort) {
	ModbusServer(u8_serialPort, 255);  // call next constructor
}

ModbusServer::ModbusServer(uint8_t u8_serialPort, uint8_t u8_enablePin) {
	m_u8_serialPort = (u8_serialPort > 3) ? 0 : u8_serialPort;
	m_u8_enablePin = ((u8_enablePin > 13) || (u8_enablePin < 2)) ? 255 : u8_enablePin;
	m_u32_mbTimeout = 1000;
}


void ModbusServer::begin(uint16_t u16_baudRate, uint8_t u8_dataBits, uint8_t u8_parity, uint8_t u8_stopBits) {
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
	
  uint32_t u32_serialFormat = SERIAL_8N1;
  if (u8_dataBits == 8) {
    if (u8_parity == 0) {
      if (u8_stopBits == 2) {
        u32_serialFormat = SERIAL_8N2;
      }
    }
    else if (u8_parity == 1) {
      if (u8_stopBits == 1) {
        u32_serialFormat = SERIAL_8O1;
      }
    }
    else if (u8_parity == 2) {
      if (u8_stopBits == 1) {
        u32_serialFormat = SERIAL_8E1;
      }
    }
  }
  else if (u8_dataBits == 7) {
    if (u8_parity == 1) {
      if (u8_stopBits == 1) {
        u32_serialFormat = SERIAL_7O1;
      }
    }
    else if (u8_parity == 2) {
      if (u8_stopBits == 1) {
        u32_serialFormat = SERIAL_7E1;
      }
    }
  }

  m_MBSerial->begin(u16_baudRate, u32_serialFormat);
}


bool ModbusServer::sendSerialRequest(ModbusRequest mr_mbReq) {
	uint8_t  u8a_txBuffer[8];  // assume funcs 1 - 6
	uint16_t u16_crc = 0xFFFF;
	
	flushSerialRx();
	if (m_u8_enablePin != 255) {
		digitalWrite(m_u8_enablePin, LOW); // MJT, set pin for transmission  was low
	}
			
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
	m_MBSerial->flush();
	
	m_u32_serialTime = millis();
	
	if (m_u8_enablePin != 255) {
		digitalWrite(m_u8_enablePin, HIGH); // MJT, set pin for transmission  was low
	}
	return true;
}


bool ModbusServer::sendTcpRequest(EthernetClient52 &ec_client, ModbusRequest mr_mbReq) {
	uint8_t u8a_txBuffer[12] = {0};
	uint8_t u8_sock = ec_client.getSocketNumber();  // or mr_mbReq.u8_flags & MRFLAG_sckMask
	uint8_t u8_slvInd;
	IPAddress ipAddr;
	
	if (SlaveData.getIndByVid(mr_mbReq.u8_vid, u8_slvInd)) {
		for (int ii = 0; ii < 4; ++ii) {
			ipAddr[ii] = SlaveData[u8_slvInd].u8a_ip[ii];
		}
		
	}
	else {
		return false;
	}

	if (ec_client.connect(ipAddr, 502)) {
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
		
		Serial.println("sent modbus/tcp mesg");
		return true;
	}
	return false;
}


int ModbusServer::serialAvailable() {
	return m_MBSerial->available();
}


int ModbusServer::tcpAvailable(EthernetClient52 &ec_client) {
	return ec_client.available();
}


uint8_t ModbusServer::recvSerialResponse(ModbusRequest mr_mbReq, uint16_t *u16p_regs, uint8_t &u8_numBytes) {
	uint8_t u8_mbStatus = k_u8_MBSuccess;
	uint16_t u16_mbRxSize = 6;
	uint8_t u8p_devResp[256] = {0};
	
	memset(u8p_devResp, 0, 4);  // set first four bytes to 0, first 2 might get reassigned elsewhere
	
	while (!u8_mbStatus) {
		while (m_MBSerial->available()) {  // make sure to get all available data before checking timeout
			if (u16_mbRxSize > 255) {  // message too big, will run into problems if crc pushes past 255
				if (m_u8_enablePin != 255) {
					digitalWrite(m_u8_enablePin, LOW); // MJT, set pin for transmission  was low
				}
	
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
				for (int ii = 6; ii < (u16_mbRxSize - 2); ++ii) {
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
      u8_mbStatus = k_u8_MBGatewayTargetFailed;
			break;
		}
	}
	
	if (!u8_mbStatus) {  // if no errors, then move data to register format, otherwise return error
		u8_numBytes = u8array2regs(u8p_devResp, u16p_regs, mr_mbReq.u8_func);
	}
	
	if (m_u8_enablePin != 255) {
		digitalWrite(m_u8_enablePin, LOW); // MJT, set pin for transmission  was low
	}
	
	return u8_mbStatus;
}


uint8_t ModbusServer::recvTcpResponse(ModbusRequest mr_mbReq, 
                              uint16_t *u16p_regs, uint8_t &u8_numBytes) {
	uint8_t u8_mbStatus = k_u8_MBSuccess;
	uint16_t u16_mbRxSize = 0;
	int16_t s16_lenRead;
	uint16_t u16_bytesLeft;
	uint8_t u8p_devResp[256] = {0};
	EthernetClient52 ec_client(mr_mbReq.u8_flags & MRFLAG_sckMask);
	
	
	u16_bytesLeft = mr_mbReq.u16_length * 2 + 9; // +9 accounts for tcp header and mb header
	
	while (!u8_mbStatus) {
		while (ec_client.available()) {  // make sure to get all available data before checking timeout
			
			if (u16_mbRxSize > 255) {  // message too big
				u8_mbStatus = k_u8_MBIllegalDataValue;
				return u8_mbStatus;
			}
			
			s16_lenRead = ec_client.read(u8p_devResp + u16_mbRxSize, u16_bytesLeft); //264 - u8ModbusADUSize);
			u16_mbRxSize += s16_lenRead;
			u16_bytesLeft -= s16_lenRead;
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
				Serial.println("raw tcp resp from slave: ");
				for (int ii = 0; ii < u16_mbRxSize; ++ii) {
					Serial.print(u8p_devResp[ii], DEC); Serial.print(" ");
				}
				Serial.println();
				
				// verify device id
				if (u8p_devResp[6] != mr_mbReq.u8_id) {
					u8_mbStatus = k_u8_MBInvalidSlaveID;
				}
				// verify function is same regardless of returned errors
				else if ((u8p_devResp[7] & 0x7f) != mr_mbReq.u8_func) {
					u8_mbStatus = k_u8_MBInvalidFunction;
				}
				// check for returned errors
				else if (bitRead(u8p_devResp[7], 7)) {
					u8_mbStatus = u8p_devResp[8];
				}
				// can exit from loop now
				break;
			}
		}
		
		if (tcpTimedOut(ec_client)) {
      u8_mbStatus = k_u8_MBGatewayTargetFailed;
			break;
		}
	}
	
	if (!u8_mbStatus) {  // 
		u8_numBytes = u8array2regs(u8p_devResp, u16p_regs, mr_mbReq.u8_func);
	}
	return u8_mbStatus;
}


uint8_t ModbusServer::u8array2regs(uint8_t u8p_devResp[], uint16_t *u16p_regs, uint8_t u8_func){
	uint8_t u8_numBytes = u8p_devResp[8];
	uint8_t u8_numRegs = u8_numBytes >> 1;
	
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
	
	return u8_numBytes;
}

void ModbusServer::sendResponse(EthernetClient52 &ec_client, const ModbusRequest &mbReq,const uint8_t u8a_strtBytes[2]) {
	uint8_t u8a_respBuf[256] = {0};  // actual buffer used to respond
	uint16_t u16_respLen(0);
	returnResponse(mbReq, u8a_strtBytes, u8a_respBuf, u16_respLen);
	
	if (u16_respLen > 0) {
		Serial.println("message to laptop: ");

		for (int ii = 0; ii < u16_respLen; ++ii) {
			Serial.print(u8a_respBuf[ii], DEC); Serial.print(", ");
		}
		Serial.println();
		
		ec_client.write(u8a_respBuf, u16_respLen);
		ec_client.flush();  // do anything?
		
	}
}

uint8_t ModbusServer::returnResponse(const ModbusRequest &mbReq, const uint8_t u8a_strtBytes[2], uint8_t u8a_respBuf[256], uint16_t &u16_respLen) {
	uint16_t u16a_interBuf[128] = {0};  // used for grabbing data in register format
	uint8_t u8_mbStatus(0);
	uint8_t u8_numBytes(0);
	
	u8a_respBuf[0] = u8a_strtBytes[0];
	u8a_respBuf[1] = u8a_strtBytes[1];
	
	if (mbReq.u8_flags & MRFLAG_timeout) {  // protocol has timed out, no message from slave device
    u8_mbStatus = k_u8_MBGatewayTargetFailed;
	}
	else if (mbReq.u8_flags & MRFLAG_isTcp) {  // there is a good message over tcp
		u8_mbStatus = recvTcpResponse(mbReq, u16a_interBuf, u8_numBytes);
	}
	else {  // there is a good message over serial
		u8_mbStatus = recvSerialResponse(mbReq, u16a_interBuf, u8_numBytes);
	}
	
	Serial.print("numBytes: "); Serial.println(u8_numBytes, DEC);
	
	if (!u8_mbStatus) {  // good data
		if (mbReq.u8_flags & MRFLAG_adjReq) {  // need to translate data via MeterLibrary
			uint16_t u16_dumLgth;
			
			MeterLibBlocks mtrLibBlk(mbReq.u16_start, mbReq.u8_mtrType);
			mtrLibBlk.adjActualRegsToFloatRegs(u8_numBytes / 2, u16_dumLgth);
			
			u8a_respBuf[5] = 3 + u16_dumLgth * 2;  // need to adjust this to fit new length
			u8a_respBuf[6] = mbReq.u8_vid;
			u8a_respBuf[7] = mbReq.u8_func;
			u8a_respBuf[8] = u16_dumLgth * 2;
			
			mtrLibBlk.convertToFloat(u16a_interBuf, &u8a_respBuf[9], mbReq.u16_length);
			
			u16_respLen = 9 + u16_dumLgth * 2;
		}
		else {  // raw data is acceptable to pass back
			if (mbReq.u8_func < 5) {  // read function
				u8a_respBuf[5] = u8_numBytes + 3;
				u8a_respBuf[6] = mbReq.u8_vid;
				u8a_respBuf[7] = mbReq.u8_func;
				u8a_respBuf[8] = u8_numBytes;
				
				for (int ii = 0, jj = 9; ii < (u8_numBytes >> 1); ++ii, jj += 2) {
					u8a_respBuf[jj]     = highByte(u16a_interBuf[ii]);
					u8a_respBuf[jj + 1] = lowByte(u16a_interBuf[ii]);
				}
					
				if (u8_numBytes % 2) { // if there are an odd number of bytes
					u8a_respBuf[8 + u8_numBytes] = lowByte(u16a_interBuf[u8_numBytes >> 1]);
				}
				
				u16_respLen = 9 + u8_numBytes;
			}
			else {  // (mbReq.u8_func < 7)
				u8a_respBuf[5] = 6;
				u8a_respBuf[6] = mbReq.u8_vid;
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
		u8a_respBuf[6] = mbReq.u8_vid;
		u8a_respBuf[7] = mbReq.u8_func | 0x80;
		u8a_respBuf[8] = u8_mbStatus;
		
		u16_respLen = 9;
	}
	
	return u8_mbStatus;
}


uint8_t ModbusServer::parseRequest(EthernetClient52 &ec_client, ModbusRequest &mbReq, 
                                   uint8_t u8a_strtBytes[2]) {
	uint32_t u32_startTime = millis();
	uint16_t u16_lenRead;
	uint16_t u16_givenLen;
	const uint32_t k_u32_mbRecvMsgTimeout(50);  // timeout for reading incoming request
	const uint16_t k_u16_mbMsgMaxLen(300);
	uint8_t u8a_mbReq[k_u16_mbMsgMaxLen];
	uint8_t u8_mbStatus(k_u8_MBSuccess);
	uint8_t u8_slvInd;
	
	while (true) {
		if (ec_client.available()) {
			u16_lenRead = ec_client.read(u8a_mbReq, k_u16_mbMsgMaxLen);

			// if client hasn't read 6 bytes, then there is a huge problem here
			if (u16_lenRead < 6) {
				u16_givenLen = 65535;
			}
			else {
				u16_givenLen = word(u8a_mbReq[4], u8a_mbReq[5]) + 6;
			}

			if ((u16_lenRead > u16_givenLen) || (u16_givenLen > k_u16_mbMsgMaxLen)) {
				// need to dump error and set ModbusRequest 
				return k_u8_MBGatewayTargetFailed;   //   should not happen, modbus/tcp deals with pretty small stuff overall)
			}
			
			while (u16_lenRead < u16_givenLen) {  // make sure to grab the full packet
				u16_lenRead += ec_client.read(u8a_mbReq + u16_lenRead, k_u16_mbMsgMaxLen - u16_lenRead);

				if ((millis() - u32_startTime) > k_u32_mbRecvMsgTimeout) {  // 10 ms might be too quick, but not really sure
					// set mbReq
          return k_u8_MBNegAcknowledge;  // can't get full message
				}

				if (u16_lenRead < 6) {
					u16_givenLen = 65535;
				}
				else {
					u16_givenLen = word(u8a_mbReq[4], u8a_mbReq[5]) + 6;
				}
			}
			
			// SET MODBUSREQUEST VARIABLE/
			u8a_strtBytes[0] = u8a_mbReq[0];
			u8a_strtBytes[1] = u8a_mbReq[1];
			
			mbReq.u8_vid = u8a_mbReq[6];
			mbReq.u8_func = u8a_mbReq[7];
			
			if (mbReq.u8_func > 6) {  // check if function is valid
				u8_mbStatus = k_u8_MBIllegalFunction;
				break;
			}
			
			if (SlaveData.getIndByVid(mbReq.u8_vid, u8_slvInd)) {  // if vid exists for slave
				uint16_t u16_dumStrt = word(u8a_mbReq[8], u8a_mbReq[9]);
				uint16_t u16_dumLgth = word(u8a_mbReq[10], u8a_mbReq[11]);
				
				mbReq.u8_id = SlaveData[u8_slvInd].u8_id;
				mbReq.u8_mtrType = SlaveData[u8_slvInd].u8a_mtrType[0];
					
				if (SlaveData.isSlaveTcpByInd(u8_slvInd)) {  // if slave is tcp
					mbReq.u8_flags = MRFLAG_isTcp;
				}
				else {
					mbReq.u8_flags = 0;
				}

				// ALL requests in 10k band returned as floats
				if ((u16_dumStrt > 49999) && ((mbReq.u8_func == 3) || (mbReq.u8_func == 4))) { //  && (u16_dumStrt < 20000)
					bool b_foundReg;
					
					if (0x01 & u16_dumLgth) {  // if odd number, can't return float which has even number of regs
						u8_mbStatus = k_u8_MBIllegalDataAddress;
						break;
					}
					
					mbReq.u16_start = u16_dumStrt - 50000;
					mbReq.u8_flags |= MRFLAG_adjReq;
					
					// GET ADJUSTED LENGTH - MAKE FUNCTION IN METERLIBBLK
					MeterLibBlocks mtrBlks(mbReq.u16_start, mbReq.u8_mtrType);
					b_foundReg = mtrBlks.adjFloatRegsToActualRegs(u16_dumLgth, mbReq.u16_length);  // float regs to actual regs
					
					if (!b_foundReg) {  // start register not listed in library
						u8_mbStatus = k_u8_MBIllegalDataAddress;
					}
				}
				else {
					mbReq.u16_start = word(u8a_mbReq[8], u8a_mbReq[9]);
					mbReq.u16_length = word(u8a_mbReq[10], u8a_mbReq[11]);
				}
			}
			else { // assume serial
				mbReq.u8_id = mbReq.u8_vid;
				mbReq.u8_mtrType = 0;
				mbReq.u8_flags = 0;
				mbReq.u16_start = word(u8a_mbReq[8], u8a_mbReq[9]);  // hopefully not greater than 10k, if so, just pass on, device will return error if necessary
				mbReq.u16_length = word(u8a_mbReq[10], u8a_mbReq[11]);  // can't be adjusted
			}
			break;  // VERY IMPORTANT!
			// find actual id and type and set flags, set necessary errors
		}
		else if ((millis() - u32_startTime) > k_u32_mbRecvMsgTimeout) {  // 10 ms might be too quick, but not really sure
			// set mbReq
      return k_u8_MBResponseTimedOut;  // no message
		}
	}
	
	return u8_mbStatus;
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

