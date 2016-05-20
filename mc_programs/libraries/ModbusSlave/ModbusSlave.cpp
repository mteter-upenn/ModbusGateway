/**
@file
Arduino library for communicating with Modbus slaves over RS232/485 (via RTU protocol).
*/
/*

  ModbusSlave.cpp - Arduino library for communicating with Modbus slaves
  over RS232/485 (via RTU protocol).
  
  This file is part of ModbusSlave.
  
  ModbusSlave is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  
  ModbusSlave is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with ModbusSlave.  If not, see <http://www.gnu.org/licenses/>.
  
  Written by Doc Walker (Rx)
  Copyright © 2009-2013 Doc Walker <4-20ma at wvfans dot net>
  
*/


/* _____PROJECT INCLUDES_____________________________________________________ */
#include "ModbusSlave.h"


/* _____GLOBAL VARIABLES_____________________________________________________ */
#if defined(ARDUINO_ARCH_AVR)
  HardwareSerial* MBSerial = &Serial; ///< Pointer to Serial class object
  // SoftwareSerial* TestSerial;
#elif defined(ARDUINO_ARCH_SAM)
  UARTClass* MBSerial = &Serial; ///< Pointer to Serial class object
#else
  #error "This library only supports boards with an AVR or SAM processor."
#endif


/* _____PUBLIC FUNCTIONS_____________________________________________________ */


/**
Constructor.

Creates class object using default serial port 0, Modbus slave ID 1.

@ingroup setup
*/
ModbusSlave::ModbusSlave(void)
{
  _u8SerialPort = 0;
  _u8MBSlave = 1;
  _u8EnablePin = 255;  // MJT
}


/**
Constructor.

Creates class object using default serial port 0, specified Modbus slave ID.

@overload void ModbusSlave::ModbusSlave(uint8_t u8MBSlave)
@param u8MBSlave Modbus slave ID (1..255)
@ingroup setup
*/
ModbusSlave::ModbusSlave(uint8_t u8MBSlave)
{
  _u8SerialPort = 0;
  _u8MBSlave = u8MBSlave;
  _u8EnablePin = 255;  // MJT
}


/**
Constructor.

Creates class object using specified serial port, Modbus slave ID.

@overload void ModbusSlave::ModbusSlave(uint8_t u8MBSlave, uint8_t u8EnablePin)
@param u8MBSlave Modbus slave ID (1..255)
@param u8EnablePin enable pin (3..13, be careful of other shield requirements)
@ingroup setup
*/
ModbusSlave::ModbusSlave(uint8_t u8MBSlave, uint8_t u8EnablePin) // MJT
{
  _u8SerialPort = 0;
  _u8MBSlave = u8MBSlave;
  _u8EnablePin = ((u8EnablePin > 13) || (u8EnablePin < 2)) ? 255 : u8EnablePin;  // MJT
}


/**
Constructor.

Creates class object using specified serial port, Modbus slave ID.

@overload void ModbusSlave::ModbusSlave(uint8_t u8MBSlave, uint8_t u8EnablePin, uint8_t u8SerialPort)
@param u8MBSlave Modbus slave ID (1..255)
@param u8EnablePin enable pin (3..13, be careful of other shield requirements)
@param u8SerialPort serial port (Serial, Serial1..Serial3)
@ingroup setup
*/
ModbusSlave::ModbusSlave(uint8_t u8MBSlave, uint8_t u8EnablePin, uint8_t u8SerialPort)  // MJT
{
  _u8SerialPort = (u8SerialPort > 3) ? 0 : u8SerialPort;
  _u8MBSlave = u8MBSlave;
  _u8EnablePin = ((u8EnablePin > 13) || (u8EnablePin < 2)) ? 255 : u8EnablePin;  // MJT
}


/**
Initialize class object.

Adds register block to slave
Call once class has been instantiated, typically within setup().

@ingroup setup
*/
void ModbusSlave::addBlock(reg_struct arg_blk)
{
  _list_blocks[_u8NumBlocks++] = arg_blk;
}


/**
Initialize class object.

Sets up the serial port using default 19200 baud rate.
Call once class has been instantiated, typically within setup().

@ingroup setup
*/
void ModbusSlave::begin(void)
{
  begin(19200);
}


/**
Initialize class object.

Sets up the serial port using specified baud rate.
Call once class has been instantiated, typically within setup()	.

@overload ModbusSlave::begin(uint16_t u16BaudRate)
@param u16BaudRate baud rate, in standard increments (300..115200)
@ingroup setup
*/
void ModbusSlave::begin(uint16_t u16BaudRate)
{
//  txBuffer = (uint16_t*) calloc(ku8MaxBufferSize, sizeof(uint16_t));
  // _u8TransmitBufferIndex = 0;
  // u16TransmitBufferLength = 0;
  
  if (_u8EnablePin != 255)
  {
	pinMode(_u8EnablePin, OUTPUT);  // MJT, LOW is Tx enabled
	digitalWrite(_u8EnablePin, HIGH); // MJT
  }
  
  switch(_u8SerialPort)
  {
#if defined(UBRR1H)
    case 1:
      MBSerial = &Serial1;
      break;
#endif
      
#if defined(UBRR2H)
    case 2:
      MBSerial = &Serial2;
      break;
#endif
      
#if defined(UBRR3H)
    case 3:
      MBSerial = &Serial3;
      break;
#endif
      
    case 0:
    default:
      MBSerial = &Serial;
      break;
  }
  
  MBSerial->begin(u16BaudRate);
#if __MODBUSSLAVE_DEBUG__
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
#endif
}

  // MJT
void ModbusSlave::setSlave(uint8_t u8MBSlave)
{
  _u8MBSlave = u8MBSlave;
}


void ModbusSlave::listen()
{
  uint8_t i;
  uint8_t u8ModbusADU[256];
  uint8_t u8ModbusADUSize = 0;
  uint8_t u8BytesLeft     = 8;
  uint8_t u8MBStatus      = ku8MBSuccess;
  uint8_t u8BlockIndex    = 255;
  // uint8_t u8MBFunction     = 0x00;
  // uint16_t u16ReadAddress = 0;
  // uint16_t u16ReadQty     = 0;
  uint16_t u16CRC;
  
  setPinRx();  // just to be safe
  
  while (true)  // (u8BytesLeft && !u8MBStatus)
  {
	
	
	if (MBSerial->available())
    {
#if __MODBUSMASTER_DEBUG__
      digitalWrite(4, LOW);
#endif
      u8ModbusADU[u8ModbusADUSize++] = MBSerial->read();
      // u8BytesLeft--;
#if __MODBUSMASTER_DEBUG__
      digitalWrite(4, false);
#endif
    }
    else
    {
#if __MODBUSMASTER_DEBUG__
      digitalWrite(5, HIGH);
#endif
      if (_idle)
      {
        _idle();
      }
#if __MODBUSMASTER_DEBUG__
      digitalWrite(5, LOW);
#endif
    }
	
	// evaluate slave ID, function code once enough bytes have been read
    if (u8ModbusADUSize == 8)  // expect 8 byte request, dev, func, hi, lo, hi, lo, crcx2
    {
	  
      // verify response is for correct Modbus slave, if not ignore by flushing receive buffer
      if (u8ModbusADU[0] != _u8MBSlave)
      {
		flushReadBuffer();  
		u8ModbusADUSize = 0;
		
		continue;
      }
      
	  // past this point a response will be sent since message is directed at this slave
	  setPinTx();
	  
	  if (MBSerial->available())  // if still more available, then send message
	  {
		flushReadBuffer();
        u8MBStatus = 0x07;  // negative acknowledge
        break;
	  }
	  
      // verify request is either 3 or 4
	  _u8MBFunction = u8ModbusADU[1];
	  
	  if (!((_u8MBFunction == 3) || (_u8MBFunction == 4)))
      {
        u8MBStatus = ku8MBInvalidFunction;  // if not, set status, then break to 
        break;
      }
	  
	  
	  
	  // verify crc
	  if (!u8MBStatus && u8ModbusADUSize >= 5)
	  {
		// calculate CRC
		u16CRC = 0xFFFF;
		for (i = 0; i < (u8ModbusADUSize - 2); i++)
		{
		  u16CRC = crc16_update(u16CRC, u8ModbusADU[i]);
		}
		
		// verify CRC
		if (!u8MBStatus && ((lowByte(u16CRC) != u8ModbusADU[u8ModbusADUSize - 2]) || (highByte(u16CRC) != u8ModbusADU[u8ModbusADUSize - 1])))
		{
		  u8MBStatus = 0x07;  //negative acknowledge, old - ku8MBInvalidCRC;
		  
		  break;
		}
	  }
	  
	  _u16ReadAddress = word(u8ModbusADU[2], u8ModbusADU[3]);
	  _u16ReadQty = word(u8ModbusADU[4], u8ModbusADU[5]);
	  
	  // check if requested registers exist in scope of class
	  u8BlockIndex = checkRegisters();
	  
	  if (u8BlockIndex == 255)
	  {
		  u8MBStatus = ku8MBIllegalDataAddress;
	  }
	  
      break;  // everything looks good, pass off to response function
	}
  }
  sendResponse(u8MBStatus, u8BlockIndex);
  
}


void ModbusSlave::flushReadBuffer()
{
	while (MBSerial->read() != -1);
}


uint8_t ModbusSlave::checkRegisters()
{
	for (uint8_t i = 0;i < _u8NumBlocks; i++)  // rotate through register blocks
	{
		// Serial.println("block start");
		
		if (_list_blocks[i].u8func == _u8MBFunction)  // check if requested function matches with selected block
		{
			uint16_t u16Start = _list_blocks[i].u16strt;
			uint16_t u16End = (_list_blocks[i].u16lgth + u16Start - 1);

			// Serial.println("function match");
			
			if ((_u16ReadAddress >= u16Start) && (_u16ReadAddress <= u16End))  // check if starting address within block
			{
				// Serial.println("address inside");
				if ((_u16ReadQty * 2) < 254)  // check if requested length is greater than buffer length - header
				{
					// Serial.println("under buffer length");
					if (_u16ReadQty <= (u16End - _u16ReadAddress + 1))  // check if length desired is available
					{
						// Serial.println("within block");
						return i;  // index of block in _list_blocks
					}
					else
					{
						// Serial.println("too long");
						break;  // block contains starting address, but does not have enough data for request
					}
				}
				else
				{
					// Serial.println("too long, buffer");
					break;  // block contains starting address, but buffer is not large enough
				}
			}
			else
			{
				// Serial.println("address not in block");
				continue;  // starting address not in this block, move on
			}
		}
		else
		{
			// Serial.println("wrong func");
			continue;  // wrong function, move on
		}
	}
	// Serial.println("return error");
	return 255;  // index of block in _list_blocks - 255 indicates error
}


void ModbusSlave::sendResponse(uint8_t u8MBStatus, uint8_t u8BlockIndex)
{
	uint8_t u8ModbusADU[256];
    uint8_t u8ModbusADUSize = 0;
	uint16_t u16CRC;
	uint8_t i;
	
	u8ModbusADU[u8ModbusADUSize++] = _u8MBSlave;
	// u8ModbusADU[u8ModbusADUSize++] = _u8MBFunction;
	
	if (u8MBStatus)  // if error
	{
		_u8MBFunction = (_u8MBFunction | 0x80);
		
		u8ModbusADU[u8ModbusADUSize++] = _u8MBFunction;
		u8ModbusADU[u8ModbusADUSize++] = u8MBStatus;
		
		
	}
	else
	{
		uint16_t &u16Strt = _list_blocks[u8BlockIndex].u16strt;
		uint16_t &u16Lgth = _list_blocks[u8BlockIndex].u16lgth;
		
		u8ModbusADU[u8ModbusADUSize++] = _u8MBFunction;
		u8ModbusADU[u8ModbusADUSize++] = lowByte((_u16ReadQty * 2));  // length of data in message in bytes
		
		for (i = (_u16ReadAddress - u16Strt); i < (_u16ReadQty + _u16ReadAddress - u16Strt); i++)
		{
			u8ModbusADU[u8ModbusADUSize++] = highByte(_list_blocks[u8BlockIndex].u16regs[i]);
			u8ModbusADU[u8ModbusADUSize++] = lowByte(_list_blocks[u8BlockIndex].u16regs[i]);
		}
	}
	
	
	// append CRC
	u16CRC = 0xFFFF;
	for (i = 0; i < u8ModbusADUSize; i++)
	{
		u16CRC = crc16_update(u16CRC, u8ModbusADU[i]);
	}
	u8ModbusADU[u8ModbusADUSize++] = lowByte(u16CRC);
	u8ModbusADU[u8ModbusADUSize++] = highByte(u16CRC);
	u8ModbusADU[u8ModbusADUSize] = 0;
	
	// digitalWrite(6, HIGH);
	// flush receive buffer before transmitting request
	// flushReadBuffer();
	setPinTx();  // just to be sure?
	
	// transmit request

	
	for (i = 0; i < u8ModbusADUSize; i++)
	{
#if defined(ARDUINO) && ARDUINO >= 100
		MBSerial->write(u8ModbusADU[i]);
#else
		MBSerial->print(u8ModbusADU[i], BYTE);
#endif
	}
  
	MBSerial->flush();    // flush transmit buffer
	
	setPinRx();
}


void ModbusSlave::setPinTx()
{
	if (_u8EnablePin != 255)
	{
		digitalWrite(_u8EnablePin, LOW);  // LOW is for TX
	}
}


void ModbusSlave::setPinRx()
{
	if (_u8EnablePin != 255)
	{
		digitalWrite(_u8EnablePin, HIGH);  // HIGH is for Rx
	}
}


/**
Set idle time callback function (cooperative multitasking).

This function gets called in the idle time between transmission of data
and response from slave. Do not call functions that read from the serial
buffer that is used by ModbusSlave. Use of i2c/TWI, 1-Wire, other
serial ports, etc. is permitted within callback function.

@see ModbusSlave::ModbusSlaveTransaction()
*/
void ModbusSlave::idle(void (*idle)())
{
  _idle = idle;
}


