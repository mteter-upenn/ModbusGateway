
#include "ModbusServer.h"
#include "MeterLibrary.h"
#include <IPAddress.h>

bool ModbusServer::ms_b_printComms = true;

ModbusServer::ModbusServer(uint8_t u8_serialPort) {
	ModbusServer(u8_serialPort, 255);  // call next constructor
}

ModbusServer::ModbusServer(uint8_t u8_serialPort, uint8_t u8_enablePin) {
	m_u8_serialPort = (u8_serialPort > 3) ? 0 : u8_serialPort;
	m_u8_enablePin = ((u8_enablePin > 13) || (u8_enablePin < 2)) ? 255 : u8_enablePin;
	m_u32_mbTimeout = 1000;
//  m_b_printComms = true;
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


void digitalClockDisplay(time_t t) {
 // digital clock display of the time
 Serial.print(hour(t));
 printDigits(minute(t));
 printDigits(second(t));
 Serial.print(" ");
 Serial.print(day(t));
 Serial.print(" ");
 Serial.print(monthStr(month(t)));
// Serial.print(month(t));
 Serial.print(" ");
 Serial.print(year(t));
 Serial.println();
}

void printDigits(int digits) {
 // utility function for digital clock display: prints preceding colon and leading 0
 Serial.print(":");
 if (digits < 10)
 Serial.print('0');
 Serial.print(digits);
}


void print3SpaceDigits(uint8_t num) {
  if (num < 10) {
    Serial.print("  ");
  }
  else if (num < 100) {
    Serial.print(" ");
  }
  Serial.print(num, DEC);
}


void write3SpaceDigits(File sdFile, uint8_t num) {
  if (num < 10) {
    sdFile.print("  ");
  }
  else if (num < 100) {
    sdFile.print(" ");
  }
  sdFile.print(num, DEC);
}


void ModbusServer::storeStringAndArr(const char *k_cp_string, uint8_t *u8p_arr, uint16_t u16_arrLen, uint16_t u16_unqId, uint8_t u8_sock, bool b_showTime) {
  if (ms_b_printComms) {
    time_t t_time = now();
    int t_yr, t_mn, t_dy;
    char ca_yr[5];
    char ca_mn[3];
    char ca_dy[3];
    uint8_t u8_digit;
    File tempFile;
    char cp_fileName[30] = {0};

    ca_yr[4] = 0;
    ca_mn[2] = 0;
    ca_dy[2] = 0;
    strcpy_P(cp_fileName, PSTR("/PASTDATA/ERRORS/"));

    t_yr = year(t_time);
    t_mn = month(t_time);
    t_dy = day(t_time);

    for (int ii = 3, jj = 0; jj < 4; --ii, ++jj) {
      u8_digit = t_yr / pow(10, ii);
      ca_yr[jj] = u8_digit + '0';
      t_yr -= u8_digit * pow(10, ii);
    }

    for (int ii = 1, jj = 0; jj < 2; --ii, ++jj) {
      u8_digit = t_mn / pow(10, ii);
      ca_mn[jj] = u8_digit + '0';
      t_mn -= u8_digit * pow(10, ii);
    }

    for (int ii = 1, jj = 0; jj < 2; --ii, ++jj) {
      u8_digit = t_dy / pow(10, ii);
      ca_dy[jj] = u8_digit + '0';
      t_dy -= u8_digit * pow(10, ii);
    }

    strcat(cp_fileName, ca_yr);
    strcat(cp_fileName, ca_mn);
    strcat(cp_fileName, ca_dy);
    strcat(cp_fileName, ".txt");

    tempFile = SD.open(cp_fileName, FILE_WRITE);

  //  tempFile.println();
    tempFile.print(k_cp_string);
    if (b_showTime) {
      tempFile.print(hour(t_time));

      tempFile.print(":");
      if (minute(t_time) < 10) tempFile.print('0');
      tempFile.print(minute(t_time));

      tempFile.print(":");
      if (second(t_time) < 10) tempFile.print('0');
      tempFile.print(second(t_time));

      tempFile.print(" ");
      tempFile.print(day(t_time));

      tempFile.print(" ");
      tempFile.print(monthStr(month(t_time)));

      tempFile.print(" ");
      tempFile.print(year(t_time));

      tempFile.print(" UTC");
    }
    tempFile.print(", unique id: ");
    tempFile.print(u16_unqId);

    tempFile.print(", on socket: ");
    if (u8_sock == 255) {
      tempFile.println("serial");
    }
    else {
      tempFile.println(u8_sock, DEC);
    }

    if (u16_arrLen > 0) {
      for (int ii = 0; ii < u16_arrLen; ++ii) {
        write3SpaceDigits(tempFile, u8p_arr[ii]); tempFile.print(" ");
      }
      tempFile.println();
    }
    tempFile.close();
  }
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
	
	// Serial.println("outgoing: ");
	// for (int ii = 0; ii < 8; ++ii) {
		// Serial.print(u8a_txBuffer[ii], DEC); Serial.print(", ");
	// }
	// Serial.println();
	
	m_MBSerial->write(u8a_txBuffer, 8);
	m_MBSerial->flush();
	
	m_u32_serialTime = millis();
	
  time_t t_time = now();

//    Serial.println();
  Serial.print("sent modbus serial mesg at ");
  digitalClockDisplay(t_time);
  for (int ii = 0; ii < 8; ++ii) {
//      Serial.print(u8a_txBuffer[ii], DEC); Serial.print(" ");
    print3SpaceDigits(u8a_txBuffer[ii]); Serial.print(" ");
  }
  Serial.println();

	if (m_u8_enablePin != 255) {
		digitalWrite(m_u8_enablePin, HIGH); // MJT, set pin for transmission  was low
	}
	
  storeStringAndArr("sent modbus serial mesg at ", u8a_txBuffer, 8, mr_mbReq.u16_unqId, 255, true);
	
	return true;
}



bool ModbusServer::sendTcpRequest(EthernetClient52 &ec_client, ModbusRequest mr_mbReq) {
	uint8_t u8a_txBuffer[12] = {0};
	uint8_t u8_sock = ec_client.getSocketNumber();  // or mr_mbReq.u8_flags & MRFLAG_sckMask
	uint8_t u8_slvInd;
	// uint8_t u8a_ip[4];
	IPAddress ipAddr;
	
	if (SlaveData.getIndByVid(mr_mbReq.u8_vid, u8_slvInd)) {
		// memcpy(u8a_ip, SlaveData[u8_slvInd].u8a_ip, 4);
		for (int ii = 0; ii < 4; ++ii) {
			ipAddr[ii] = SlaveData[u8_slvInd].u8a_ip[ii];
		}
		
	}
	else {
		return false;
	}
	// GET IP FROM mr_mbReq.u8_vid!!
	// ec_client.connect(IP);
	
	// if (ec_client.connect(u8a_ip, 502)) {
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
		
    time_t t_time = now();

//    Serial.println();
    Serial.print("sent modbus/tcp mesg at ");
    digitalClockDisplay(t_time);
    for (int ii = 0; ii < 12; ++ii) {
//      Serial.print(u8a_txBuffer[ii], DEC); Serial.print(" ");
      print3SpaceDigits(u8a_txBuffer[ii]); Serial.print(" ");
    }
    Serial.println();
    storeStringAndArr("sent modbus/tcp mesg at ", u8a_txBuffer, 12, mr_mbReq.u16_unqId, u8_sock, true);
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
  uint16_t u16_expLength(0);

  switch (mr_mbReq.u8_func) {
    case 1:  // reads
    case 2:
    case 3:
    case 4:
      u16_expLength = mr_mbReq.u16_length * 2 + 11;  // +9 accounts for tcp hdr, mb hdr
      break;
    case 5:  // writes
    case 6:
      u16_expLength = 12;  // +12 accounts for tcp hdr, mb hdr
      break;
  }

	memset(u8p_devResp, 0, 4);  // set first four bytes to 0, first 2 might get reassigned elsewhere
	
	while (!u8_mbStatus) {
		while (m_MBSerial->available()) {  // make sure to get all available data before checking timeout
			if (u16_mbRxSize > 255) {  // message too big, will run into problems if crc pushes past 255
        u8_mbStatus = k_u8_MBSlaveDeviceFailure;
				flushSerialRx();
//				return u8_mbStatus;
        goto ExitLoopAndFunc;
			}
			
			u8p_devResp[u16_mbRxSize++] = m_MBSerial->read();  // important bit here, storing data
		}
		
    if (u16_mbRxSize > 8) {
      if (bitRead(u8p_devResp[7], 7)) {
        u8_mbStatus = u8p_devResp[8];

        // Serial.println("modbus response: ");
        // for (int ii = 0; ii < u16_mbRxSize; ++ii) {
          // Serial.print(u8p_devResp[ii], DEC); Serial.print(", ");
        // }
        // Serial.println();

        storeStringAndArr("raw serial resp from slave at ", u8p_devResp, u16_mbRxSize, mr_mbReq.u16_unqId, 255, true);

        break;
      }
      else {
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

          // Serial.println("modbus response: ");
          // for (int ii = 0; ii < u16_mbRxSize; ++ii) {
            // Serial.print(u8p_devResp[ii], DEC); Serial.print(", ");
          // }
          // Serial.println();

          storeStringAndArr("raw serial resp from slave at ", u8p_devResp, u16_mbRxSize, mr_mbReq.u16_unqId, 255, true);

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
          else if (u16_expLength != u16_mbRxSize) {
            // recieved message does not conform to expected length
            u8_mbStatus = k_u8_MBSlaveDeviceFailure;
          }

          u8p_devResp[4] = highByte(u16_mbRxSize - 8);  // -8 accounts for tcp hdr and crc
          u8p_devResp[5] = lowByte(u16_mbRxSize - 8);
          // can exit from loop now
          break;
        }
      }
		}
		
    if (didSerialTimeOut()) {
      u8_mbStatus = k_u8_MBGatewayTargetFailed;
			break;
		}
	}
	
  ExitLoopAndFunc:

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
  uint16_t u16_expLength(0);  // = mr_mbReq.u16_length * 2 + 9; // +9 accounts for tcp header and mb header
	
  switch (mr_mbReq.u8_func) {
    case 1:  // reads
    case 2:
    case 3:
    case 4:
      u16_expLength = mr_mbReq.u16_length * 2 + 9;  // +9 accounts for tcp hdr, mb hdr
      break;
    case 5:  // writes
    case 6:
      u16_expLength = 12;  // +12 accounts for tcp hdr, mb hdr
      break;
  }

  u16_bytesLeft = u16_expLength;
	
	while (!u8_mbStatus) {
		while (ec_client.available()) {  // make sure to get all available data before checking timeout
//      Serial.print("recvTcpResponse, available: "); Serial.println(ec_client.available(), DEC);
      if (u16_bytesLeft == 0) {  // when recieving longer than expected message
//        Serial.println("returned message way too long");
//        for (int ii = 0; ii < u16_mbRxSize; ++ii) {
//          print3SpaceDigits(u8p_devResp[ii]); Serial.print(" ");
//        }
//        Serial.println();
//        uint16_t u16_dumleft = ec_client.available();

//        for (int ii = 0; ii < u16_dumleft; ++ii) {
//          print3SpaceDigits(ec_client.read()); Serial.print(" ");
//        }

//        while (true) {  // loop forever
//        }
        u8_mbStatus = k_u8_MBSlaveDeviceFailure;
        break;
      }

//      if (u16_mbRxSize + ec_client.available() > u16_expLength) {  // message too big
      if (u16_mbRxSize + ec_client.available() > 255) {  // message too big
        // u16_mbRxSize is amount read and .available is remaining.  if there was only a check for .available being too large
        //    at the beginning, then any latecoming values could push the total over the edge.  This should account for that scenario
        u8_mbStatus = k_u8_MBSlaveDeviceFailure;
				// flushTcpRx(ec_client);  // don't bother flushing tcp, simply close socket
//				return u8_mbStatus;
        goto ExitLoopAndFunc;
			}
			
			s16_lenRead = ec_client.read(u8p_devResp + u16_mbRxSize, u16_bytesLeft); //264 - u8ModbusADUSize);
			u16_mbRxSize += s16_lenRead;
			u16_bytesLeft -= s16_lenRead;
			
			// u8p_devResp[u16_mbRxSize++] = m_MBSerial->read();  // important bit here, storing data
		}
		
    if (u16_mbRxSize > 8) {
      if (bitRead(u8p_devResp[7], 7)) {
        u8_mbStatus = u8p_devResp[8];  // bit 8 is either byte length or an error code, make sure it isn't an error code
        Serial.println("raw tcp resp from slave: ");
        for (int ii = 0; ii < u16_mbRxSize; ++ii) {
//					Serial.print(u8p_devResp[ii], DEC); Serial.print(" ");
          print3SpaceDigits(u8p_devResp[ii]); Serial.print(" ");
        }
        Serial.println();
        storeStringAndArr("raw tcp resp from slave at ", u8p_devResp, u16_mbRxSize, mr_mbReq.u16_unqId, ec_client.getSocketNumber(), true);
        break;  // exit while .available() loop
      }
      else {
        // there is no error code, check that ids and funcs match
        uint16_t u16_givenSize(0);
        switch (mr_mbReq.u8_func) {
          case 1:  // reads
          case 2:
          case 3:
          case 4:
            u16_givenSize = u8p_devResp[8] + 9;  // +9 accounts for tcp hdr, mb hdr
            break;
          case 5:  // writes
          case 6:
            u16_givenSize = 12;  // +12 accounts for tcp hdr, mb hdr
            break;
        }

        if (u16_givenSize == u16_mbRxSize) {  // if sizes match, if they don't timeout will catch
          Serial.println("raw tcp resp from slave: ");
          for (int ii = 0; ii < u16_mbRxSize; ++ii) {
  //					Serial.print(u8p_devResp[ii], DEC); Serial.print(" ");
            print3SpaceDigits(u8p_devResp[ii]); Serial.print(" ");
          }
          Serial.println();
          storeStringAndArr("raw tcp resp from slave at ", u8p_devResp, u16_mbRxSize, mr_mbReq.u16_unqId, ec_client.getSocketNumber(), true);

          // verify device id
          if (u8p_devResp[6] != mr_mbReq.u8_id) {
            u8_mbStatus = k_u8_MBInvalidSlaveID;
          }
          // verify function is same regardless of returned errors
          else if ((u8p_devResp[7] & 0x7f) != mr_mbReq.u8_func) {
            u8_mbStatus = k_u8_MBInvalidFunction;
          }
          else if (u16_expLength != u16_mbRxSize) {
            // recieved message does not conform to expected length
            u8_mbStatus = k_u8_MBSlaveDeviceFailure;
          }
          // can exit from loop now
          break;  // exit while .available() loop
        }
      }
		}
		
    if (didTcpTimeOut(ec_client)) {
//      Serial.print("tcp request timed out on socket "); Serial.println(ec_client.getSocketNumber(), DEC);
      u8_mbStatus = k_u8_MBGatewayTargetFailed;
			break;
		}
	}
	
  ExitLoopAndFunc:
	if (!u8_mbStatus) {  // 
		u8_numBytes = u8array2regs(u8p_devResp, u16p_regs, mr_mbReq.u8_func);
		// Serial.print("regs from slave: ");
		// for (int ii = 0; ii < u8_numBytes / 2; ++ii) {
			// Serial.print(u16p_regs[ii], DEC); Serial.print(" ");
		// }
		// Serial.println();
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
		// for debugging
		// u8a_respBuf[0] = 0;
		// u8a_respBuf[1] = 0;
		// u8a_respBuf[2] = 0;
		// u8a_respBuf[3] = 0;
		// u8a_respBuf[4] = 0;
		// u8a_respBuf[5] = 7;
		
		// u8a_respBuf[6] = 101;
		// u8a_respBuf[7] = 3;
		// u8a_respBuf[8] = 4;
		// u8a_respBuf[9] = 3;
		// u8a_respBuf[10] = 190;
		// u8a_respBuf[11] = 14;
		// u8a_respBuf[12] = 208;
		
		// u16_respLen = 13;
		// end debugging
		for (int ii = 0; ii < u16_respLen; ++ii) {
//      Serial.print(u8a_respBuf[ii], DEC); Serial.print(" ");
      print3SpaceDigits(u8a_respBuf[ii]); Serial.print(" ");
		}
    Serial.println("\n");
    storeStringAndArr("message to laptop at ", u8a_respBuf, u16_respLen, mbReq.u16_unqId, ec_client.getSocketNumber(), true);

		ec_client.write(u8a_respBuf, u16_respLen);
		ec_client.flush();  // do anything?
		
	}
}

uint8_t ModbusServer::returnResponse(const ModbusRequest &mbReq, const uint8_t u8a_strtBytes[2], uint8_t u8a_respBuf[256], uint16_t &u16_respLen) {
	// uint8_t u8a_respBuf[256] = {0};  // actual buffer used to respond
	uint16_t u16a_interBuf[128] = {0};  // used for grabbing data in register format
	uint8_t u8_mbStatus(0);
	// uint16_t u16_respLen(0);
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
	
//	Serial.print("numBytes: "); Serial.println(u8_numBytes, DEC);
	
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
			
			// Serial.println("modbus message");
			// for (int ii = 0; ii < u16_lenRead; ++ii) {
				// Serial.print(u8a_mbReq[ii], DEC); Serial.print(", ");
			// }
			// Serial.println();
			// Serial.print("given: "); Serial.println(u16_givenLen, DEC);
			// Serial.print("lenRead: "); Serial.println(u16_lenRead, DEC);


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
			
      storeStringAndArr("\nincoming request at ", u8a_mbReq, u16_lenRead, 0, ec_client.getSocketNumber(), true);

			// SET MODBUSREQUEST VARIABLE/
			u8a_strtBytes[0] = u8a_mbReq[0];
			u8a_strtBytes[1] = u8a_mbReq[1];
			
			mbReq.u8_vid = u8a_mbReq[6];
			mbReq.u8_func = u8a_mbReq[7];
			
			if (mbReq.u8_func > 6) {  // check if function is valid
				u8_mbStatus = k_u8_MBIllegalFunction;
				break;
			}

      uint16_t u16_dumLgth = word(u8a_mbReq[10], u8a_mbReq[11]);

      if (u16_dumLgth * 2 + 9 > 255) {
        u8_mbStatus = k_u8_MBIllegalDataAddress;  // requesting too many registers/bytes to fit in mb packet
        break;
      }
			
			if (SlaveData.getIndByVid(mbReq.u8_vid, u8_slvInd)) {  // if vid exists for slave
				uint16_t u16_dumStrt = word(u8a_mbReq[8], u8a_mbReq[9]);
//				uint16_t u16_dumLgth = word(u8a_mbReq[10], u8a_mbReq[11]);
				
				// SlaveData.getIdByInd(u8_slvInd, mbReq.u8_id);
				// SlaveData.getRedTypeByInd(u8_slvInd, mbReq.u8_mtrType);
				
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

          if (mbReq.u16_length * 2 + 9 > 255) {  // check to make sure adjusting length is still ok
            u8_mbStatus = k_u8_MBIllegalDataAddress;  // requesting too many registers/bytes to fit in mb packet
            break;
          }
				}
				else {
					mbReq.u16_start = word(u8a_mbReq[8], u8a_mbReq[9]);
          mbReq.u16_length = u16_dumLgth;
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
	

void ModbusServer::setPrintComms(bool b_printComms) {
  ms_b_printComms = b_printComms;
}

	
bool ModbusServer::didSerialTimeOut() {  // if timed out, return true
	return ((millis() - m_u32_serialTime) > m_u32_mbTimeout) ? true : false;
}
	
	
bool ModbusServer::didTcpTimeOut(EthernetClient52 &ec_client) {  // if timed out, return true
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

