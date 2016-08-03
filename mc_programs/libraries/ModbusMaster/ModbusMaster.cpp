/**
@file
Arduino library for communicating with Modbus slaves over RS232/485 (via RTU protocol).
*/
/*

  ModbusMaster.cpp - Arduino library for communicating with Modbus slaves
  over RS232/485 (via RTU protocol).
  
  This file is part of ModbusMaster.
  
  ModbusMaster is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  
  ModbusMaster is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with ModbusMaster.  If not, see <http://www.gnu.org/licenses/>.
  
  Written by Doc Walker (Rx)
  Copyright © 2009-2013 Doc Walker <4-20ma at wvfans dot net>
  
*/


/* _____PROJECT INCLUDES_____________________________________________________ */
#include "ModbusMaster.h"


/* _____GLOBAL VARIABLES_____________________________________________________ */
#if defined(ARDUINO_ARCH_AVR)
#if defined(__arm__) && defined(CORE_TEENSY)  // if teensy3.0 or greater
	HardwareSerial* MBSerial = &Serial1; ///< Pointer to Serial class object
#else
  HardwareSerial* MBSerial = &Serial; ///< Pointer to Serial class object
#endif
#elif defined(ARDUINO_ARCH_SAM)
  UARTClass* MBSerial = &Serial; ///< Pointer to Serial class object
#else
  #error "This library only supports boards with an AVR or SAM processor. Please open an issue at https://github.com/4-20ma/ModbusMaster/issues and indicate which processor/platform you're using."
#endif


/*______STATIC VARIABLES_____________________________________________________ */



/* _____PUBLIC FUNCTIONS_____________________________________________________ */
/**
Constructor.

Creates class object using default serial port 0, Modbus slave ID 1.

@ingroup setup
*/
ModbusMaster::ModbusMaster(void)
{
  // _u8SerialPort = 0;
  // _u8MBSlave = 1;
  // _u8EnablePin = 255;  // MJT
  // _u8ClientIP[0] = 192;
  // _u8ClientIP[1] = 168;
  // _u8ClientIP[2] = 1;
  // _u8ClientIP[3] = 1;
  // _bSerialTrans = true;
	ModbusMaster(9);
}


// /**
// Constructor.

// Creates class object using default serial port 0, specified Modbus slave ID.

// @overload void ModbusMaster::ModbusMaster(uint8_t u8MBSlave)
// @param u8MBSlave Modbus slave ID (1..255)
// @ingroup setup
// */
// ModbusMaster::ModbusMaster(uint8_t u8_mbSlave)
// {
  // // _u8SerialPort = 0;
  // // _u8MBSlave = u8MBSlave;
  // // _u8EnablePin = 255;  // MJT
  // // _u8ClientIP[0] = 192;
  // // _u8ClientIP[1] = 168;
  // // _u8ClientIP[2] = 1;
  // // _u8ClientIP[3] = 1;
  // // _bSerialTrans = true;
	// ModbusMaster(u8_mbSlave, )
// }

/**
Constructor.

Creates class object using default serial port 0, specified enable pin

@overload void ModbusMaster::ModbusMaster(uint8_t u8MBSlave)
@param u8MBSlave Modbus slave ID (1..255)
@ingroup setup
*/
ModbusMaster::ModbusMaster(const uint8_t u8_enablePin)
{
  // _u8SerialPort = 0;
  // _u8MBSlave = u8MBSlave;
  // _u8EnablePin = 255;  // MJT
  // _u8ClientIP[0] = 192;
  // _u8ClientIP[1] = 168;
  // _u8ClientIP[2] = 1;
  // _u8ClientIP[3] = 1;
  // _bSerialTrans = true;
	ModbusMaster(u8_enablePin, 0);
}


// /**
// Constructor.

// Creates class object using specified serial port, Modbus slave ID.

// @overload void ModbusMaster::ModbusMaster(uint8_t u8MBSlave, uint8_t u8EnablePin)
// @param u8MBSlave Modbus slave ID (1..255)
// @param cl_ip_mem client ip address
// @ingroup setup
// */
// ModbusMaster::ModbusMaster(uint8_t u8MBSlave, uint8_t (&cl_ip_mem)[4])  // MJT
// {
  // _u8EnablePin = 255;  // MJT
  // _u8SerialPort = 0;
  // _u8MBSlave = u8MBSlave;
  // _u8ClientIP[0] = cl_ip_mem[0];
  // _u8ClientIP[1] = cl_ip_mem[1];
  // _u8ClientIP[2] = cl_ip_mem[2];
  // _u8ClientIP[3] = cl_ip_mem[3];
  // _bSerialTrans = true;
// }


/**
Constructor.

Creates class object using specified serial port, Modbus slave ID.

@overload void ModbusMaster::ModbusMaster(uint8_t u8MBSlave, uint8_t u8EnablePin)
@param u8MBSlave Modbus slave ID (1..255)
@param cl_ip_mem client ip address
@param u8EnablePin enable pin (3..13, be careful of other shield requirements)
@ingroup setup
*/
ModbusMaster::ModbusMaster(const uint8_t u8_enablePin, const uint8_t u8_serialPort)  // MJT
{
  // _u8SerialPort = 0;
  // _u8MBSlave = u8MBSlave;
  // _u8EnablePin = ((u8EnablePin > 13) || (u8EnablePin < 2)) ? 255 : u8EnablePin;  // MJT
  // _u8ClientIP[0] = cl_ip_mem[0];
  // _u8ClientIP[1] = cl_ip_mem[1];
  // _u8ClientIP[2] = cl_ip_mem[2];
  // _u8ClientIP[3] = cl_ip_mem[3];
  // _bSerialTrans = true;
	
	_u8EnablePin = ((u8_enablePin > 13) || (u8_enablePin < 2)) ? 255 : u8_enablePin;
	_u8SerialPort = (u8_serialPort > 3) ? 0 : u8_serialPort;
	_u8MBSlave = 1;
	memset(_u8ClientIP, 0, 4);
	_bSerialTrans = true; // use serial
}


// /**
// Constructor.

// Creates class object using specified serial port, Modbus slave ID.

// @overload void ModbusMaster::ModbusMaster(uint8_t u8MBSlave, uint8_t u8EnablePin, uint8_t u8SerialPort)
// @param u8MBSlave Modbus slave ID (1..255)
// @param cl_ip_mem client ip address
// @param u8EnablePin enable pin (3..13, be careful of other shield requirements)
// @param u8SerialPort serial port (Serial, Serial1..Serial3)
// @ingroup setup
// */
// ModbusMaster::ModbusMaster(uint8_t u8MBSlave, uint8_t (&cl_ip_mem)[4], const uint8_t u8EnablePin, const uint8_t u8SerialPort)  // MJT
// {
  // _u8SerialPort = (u8SerialPort > 3) ? 0 : u8SerialPort;
  // _u8MBSlave = u8MBSlave;
  // _u8EnablePin = ((u8EnablePin > 13) || (u8EnablePin < 2)) ? 255 : u8EnablePin;  // MJT
  // _u8ClientIP[0] = cl_ip_mem[0];
  // _u8ClientIP[1] = cl_ip_mem[1];
  // _u8ClientIP[2] = cl_ip_mem[2];
  // _u8ClientIP[3] = cl_ip_mem[3];
  // _bSerialTrans = true;
// }


/**
Initialize class object.

Sets up the serial port using default 19200 baud rate.
Call once class has been instantiated, typically within setup().

@ingroup setup
*/
void ModbusMaster::begin(void)
{
  begin(19200);
}


/**
Initialize class object.

Sets up the serial port using specified baud rate.
Call once class has been instantiated, typically within setup()	.

@overload ModbusMaster::begin(uint16_t u16BaudRate)
@param u16BaudRate baud rate, in standard increments (300..115200)
@ingroup setup
*/
void ModbusMaster::begin(uint16_t u16BaudRate)
{
//  txBuffer = (uint16_t*) calloc(ku8MaxBufferSize, sizeof(uint16_t));
  _u8TransmitBufferIndex = 0;
  u16TransmitBufferLength = 0;
  
  if (_u8EnablePin != 255)
  {
	pinMode(_u8EnablePin, OUTPUT);  // MJT, LOW is Tx enabled
	digitalWrite(_u8EnablePin, LOW); // MJT  was low
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

#if defined(CORE_TEENSY)
	case 3:
	  MBSerial = &Serial3;
      break;
    case 0:
    default:
	  MBSerial = &Serial1; ///< Pointer to Serial class object
#else
	case 0:
    default:
      MBSerial = &Serial;
#endif
      break;
  }
  
  MBSerial->begin(u16BaudRate);
#if __MODBUSMASTER_DEBUG__
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
#endif
}

  // MJT
void ModbusMaster::setSlave(uint8_t u8MBSlave)
{
  _u8MBSlave = u8MBSlave;
}


 // MJT
void ModbusMaster::setClientIP(uint8_t u8a_clientIp[4])
{
	memcpy(_u8ClientIP, u8a_clientIp, 4);
  // _u8ClientIP[0] = cl_ip_mem[0];
  // _u8ClientIP[1] = cl_ip_mem[1];
  // _u8ClientIP[2] = cl_ip_mem[2];
  // _u8ClientIP[3] = cl_ip_mem[3];
}


// MJT
void ModbusMaster::setSerialEthernet(bool bSerialTrans)
{
  _bSerialTrans = bSerialTrans;
}


// MJT
void ModbusMaster::setTimeout(uint16_t u16MBTimeout)
{
  _u16MBResponseTimeout = u16MBTimeout;
}


/**
Set idle time callback function (cooperative multitasking).

This function gets called in the idle time between transmission of data
and response from slave. Do not call functions that read from the serial
buffer that is used by ModbusMaster. Use of i2c/TWI, 1-Wire, other
serial ports, etc. is permitted within callback function.

@see ModbusMaster::ModbusMasterTransaction()
*/
void ModbusMaster::idle(void (*idle)())
{
  _idle = idle;
}


/**
Retrieve data from response buffer.

@see ModbusMaster::clearResponseBuffer()
@param u8Index index of response buffer array (0x00..0x3F)
@return value in position u8Index of response buffer (0x0000..0xFFFF)
@ingroup buffer
*/
uint16_t ModbusMaster::getResponseBuffer(uint8_t u8Index)
{
  if (u8Index < ku8MaxBufferSize)
  {
    return _u16ResponseBuffer[u8Index];
  }
  else
  {
    return 0xFFFF;
  }
}


/**
Clear Modbus response buffer.

@see ModbusMaster::getResponseBuffer(uint8_t u8Index)
@ingroup buffer
*/
void ModbusMaster::clearResponseBuffer()
{
  uint8_t i;
  
  for (i = 0; i < ku8MaxBufferSize; i++)
  {
    _u16ResponseBuffer[i] = 0;
  }
}


/**
Place data in transmit buffer.

@see ModbusMaster::clearTransmitBuffer()
@param u8Index index of transmit buffer array (0x00..0x3F)
@param u16Value value to place in position u8Index of transmit buffer (0x0000..0xFFFF)
@return 0 on success; exception number on failure
@ingroup buffer
*/
uint8_t ModbusMaster::setTransmitBuffer(uint8_t u8Index, uint16_t u16Value)
{
  if (u8Index < ku8MaxBufferSize)
  {
    _u16TransmitBuffer[u8Index] = u16Value;
    return ku8MBSuccess;
  }
  else
  {
    return ku8MBIllegalDataAddress;
  }
}


/**
Clear Modbus transmit buffer.

@see ModbusMaster::setTransmitBuffer(uint8_t u8Index, uint16_t u16Value)
@ingroup buffer
*/
void ModbusMaster::clearTransmitBuffer()
{
  uint8_t i;
  
  for (i = 0; i < ku8MaxBufferSize; i++)
  {
    _u16TransmitBuffer[i] = 0;
  }
}


/**
Modbus function 0x01 Read Coils.

This function code is used to read from 1 to 2000 contiguous status of 
coils in a remote device. The request specifies the starting address, 
i.e. the address of the first coil specified, and the number of coils. 
Coils are addressed starting at zero.

The coils in the response buffer are packed as one coil per bit of the 
data field. Status is indicated as 1=ON and 0=OFF. The LSB of the first 
data word contains the output addressed in the query. The other coils 
follow toward the high order end of this word and from low order to high 
order in subsequent words.

If the returned quantity is not a multiple of sixteen, the remaining 
bits in the final data word will be padded with zeros (toward the high 
order end of the word).

@param u16ReadAddress address of first coil (0x0000..0xFFFF)
@param u16BitQty quantity of coils to read (1..2000, enforced by remote device)
@return 0 on success; exception number on failure
@ingroup discrete
*/
uint8_t ModbusMaster::readCoils(uint16_t u16ReadAddress, uint16_t u16BitQty)
{
  _u16ReadAddress = u16ReadAddress;
  _u16ReadQty = u16BitQty;
  return ModbusMasterTransaction(ku8MBReadCoils);
}


/**
Modbus function 0x02 Read Discrete Inputs.

This function code is used to read from 1 to 2000 contiguous status of 
discrete inputs in a remote device. The request specifies the starting 
address, i.e. the address of the first input specified, and the number 
of inputs. Discrete inputs are addressed starting at zero.

The discrete inputs in the response buffer are packed as one input per 
bit of the data field. Status is indicated as 1=ON; 0=OFF. The LSB of 
the first data word contains the input addressed in the query. The other 
inputs follow toward the high order end of this word, and from low order 
to high order in subsequent words.

If the returned quantity is not a multiple of sixteen, the remaining 
bits in the final data word will be padded with zeros (toward the high 
order end of the word).

@param u16ReadAddress address of first discrete input (0x0000..0xFFFF)
@param u16BitQty quantity of discrete inputs to read (1..2000, enforced by remote device)
@return 0 on success; exception number on failure
@ingroup discrete
*/
uint8_t ModbusMaster::readDiscreteInputs(uint16_t u16ReadAddress,
  uint16_t u16BitQty)
{
  _u16ReadAddress = u16ReadAddress;
  _u16ReadQty = u16BitQty;
  return ModbusMasterTransaction(ku8MBReadDiscreteInputs);
}


/**
Modbus function 0x03 Read Holding Registers.

This function code is used to read the contents of a contiguous block of 
holding registers in a remote device. The request specifies the starting 
register address and the number of registers. Registers are addressed 
starting at zero.

The register data in the response buffer is packed as one word per 
register.

@param u16ReadAddress address of the first holding register (0x0000..0xFFFF)
@param u16ReadQty quantity of holding registers to read (1..125, enforced by remote device)
@return 0 on success; exception number on failure
@ingroup register
*/
uint8_t ModbusMaster::readHoldingRegisters(uint16_t u16ReadAddress,
  uint16_t u16ReadQty)
{
  _u16ReadAddress = u16ReadAddress;
  _u16ReadQty = u16ReadQty;
  return ModbusMasterTransaction(ku8MBReadHoldingRegisters);
}


/**
Modbus function 0x04 Read Input Registers.

This function code is used to read from 1 to 125 contiguous input 
registers in a remote device. The request specifies the starting 
register address and the number of registers. Registers are addressed 
starting at zero.

The register data in the response buffer is packed as one word per 
register.

@param u16ReadAddress address of the first input register (0x0000..0xFFFF)
@param u16ReadQty quantity of input registers to read (1..125, enforced by remote device)
@return 0 on success; exception number on failure
@ingroup register
*/
uint8_t ModbusMaster::readInputRegisters(uint16_t u16ReadAddress,
  uint8_t u16ReadQty)
{
  _u16ReadAddress = u16ReadAddress;
  _u16ReadQty = u16ReadQty;
  return ModbusMasterTransaction(ku8MBReadInputRegisters);
}


/**
Modbus function 0x05 Write Single Coil.

This function code is used to write a single output to either ON or OFF 
in a remote device. The requested ON/OFF state is specified by a 
constant in the state field. A non-zero value requests the output to be 
ON and a value of 0 requests it to be OFF. The request specifies the 
address of the coil to be forced. Coils are addressed starting at zero.

@param u16WriteAddress address of the coil (0x0000..0xFFFF)
@param u8State 0=OFF, non-zero=ON (0x00..0xFF)
@return 0 on success; exception number on failure
@ingroup discrete
*/
uint8_t ModbusMaster::writeSingleCoil(uint16_t u16WriteAddress, uint8_t u8State)
{
  _u16WriteAddress = u16WriteAddress;
  _u16WriteQty = (u8State ? 0xFF00 : 0x0000);
  return ModbusMasterTransaction(ku8MBWriteSingleCoil);
}


/**
Modbus function 0x06 Write Single Register.

This function code is used to write a single holding register in a 
remote device. The request specifies the address of the register to be 
written. Registers are addressed starting at zero.

@param u16WriteAddress address of the holding register (0x0000..0xFFFF)
@param u16WriteValue value to be written to holding register (0x0000..0xFFFF)
@return 0 on success; exception number on failure
@ingroup register
*/
uint8_t ModbusMaster::writeSingleRegister(uint16_t u16WriteAddress,
  uint16_t u16WriteValue)
{
  _u16WriteAddress = u16WriteAddress;
  _u16WriteQty = 0;
  _u16TransmitBuffer[0] = u16WriteValue;
  return ModbusMasterTransaction(ku8MBWriteSingleRegister);
}


/**
Modbus function 0x0F Write Multiple Coils.

This function code is used to force each coil in a sequence of coils to 
either ON or OFF in a remote device. The request specifies the coil 
references to be forced. Coils are addressed starting at zero.

The requested ON/OFF states are specified by contents of the transmit 
buffer. A logical '1' in a bit position of the buffer requests the 
corresponding output to be ON. A logical '0' requests it to be OFF.

@param u16WriteAddress address of the first coil (0x0000..0xFFFF)
@param u16BitQty quantity of coils to write (1..2000, enforced by remote device)
@return 0 on success; exception number on failure
@ingroup discrete
*/
uint8_t ModbusMaster::writeMultipleCoils(uint16_t u16WriteAddress,
  uint16_t u16BitQty)
{
  _u16WriteAddress = u16WriteAddress;
  _u16WriteQty = u16BitQty;
  return ModbusMasterTransaction(ku8MBWriteMultipleCoils);
}
uint8_t ModbusMaster::writeMultipleCoils()
{
  _u16WriteQty = u16TransmitBufferLength;
  return ModbusMasterTransaction(ku8MBWriteMultipleCoils);
}


/**
Modbus function 0x10 Write Multiple Registers.

This function code is used to write a block of contiguous registers (1 
to 123 registers) in a remote device.

The requested written values are specified in the transmit buffer. Data 
is packed as one word per register.

@param u16WriteAddress address of the holding register (0x0000..0xFFFF)
@param u16WriteQty quantity of holding registers to write (1..123, enforced by remote device)
@return 0 on success; exception number on failure
@ingroup register
*/
uint8_t ModbusMaster::writeMultipleRegisters(uint16_t u16WriteAddress,
  uint16_t u16WriteQty)
{
  _u16WriteAddress = u16WriteAddress;
  _u16WriteQty = u16WriteQty;
  return ModbusMasterTransaction(ku8MBWriteMultipleRegisters);
}

// new version based on Wire.h
uint8_t ModbusMaster::writeMultipleRegisters()
{
  _u16WriteQty = _u8TransmitBufferIndex;
  return ModbusMasterTransaction(ku8MBWriteMultipleRegisters);
}


/**
Modbus function 0x16 Mask Write Register.

This function code is used to modify the contents of a specified holding 
register using a combination of an AND mask, an OR mask, and the 
register's current contents. The function can be used to set or clear 
individual bits in the register.

The request specifies the holding register to be written, the data to be 
used as the AND mask, and the data to be used as the OR mask. Registers 
are addressed starting at zero.

The function's algorithm is:

Result = (Current Contents && And_Mask) || (Or_Mask && (~And_Mask))

@param u16WriteAddress address of the holding register (0x0000..0xFFFF)
@param u16AndMask AND mask (0x0000..0xFFFF)
@param u16OrMask OR mask (0x0000..0xFFFF)
@return 0 on success; exception number on failure
@ingroup register
*/
uint8_t ModbusMaster::maskWriteRegister(uint16_t u16WriteAddress,
  uint16_t u16AndMask, uint16_t u16OrMask)
{
  _u16WriteAddress = u16WriteAddress;
  _u16TransmitBuffer[0] = u16AndMask;
  _u16TransmitBuffer[1] = u16OrMask;
  return ModbusMasterTransaction(ku8MBMaskWriteRegister);
}


/**
Modbus function 0x17 Read Write Multiple Registers.

This function code performs a combination of one read operation and one 
write operation in a single MODBUS transaction. The write operation is 
performed before the read. Holding registers are addressed starting at 
zero.

The request specifies the starting address and number of holding 
registers to be read as well as the starting address, and the number of 
holding registers. The data to be written is specified in the transmit 
buffer.

@param u16ReadAddress address of the first holding register (0x0000..0xFFFF)
@param u16ReadQty quantity of holding registers to read (1..125, enforced by remote device)
@param u16WriteAddress address of the first holding register (0x0000..0xFFFF)
@param u16WriteQty quantity of holding registers to write (1..121, enforced by remote device)
@return 0 on success; exception number on failure
@ingroup register
*/
uint8_t ModbusMaster::readWriteMultipleRegisters(uint16_t u16ReadAddress,
  uint16_t u16ReadQty, uint16_t u16WriteAddress, uint16_t u16WriteQty)
{
  _u16ReadAddress = u16ReadAddress;
  _u16ReadQty = u16ReadQty;
  _u16WriteAddress = u16WriteAddress;
  _u16WriteQty = u16WriteQty;
  return ModbusMasterTransaction(ku8MBReadWriteMultipleRegisters);
}
uint8_t ModbusMaster::readWriteMultipleRegisters(uint16_t u16ReadAddress,
  uint16_t u16ReadQty)
{
  _u16ReadAddress = u16ReadAddress;
  _u16ReadQty = u16ReadQty;
  _u16WriteQty = _u8TransmitBufferIndex;
  return ModbusMasterTransaction(ku8MBReadWriteMultipleRegisters);
}


/* _____PRIVATE FUNCTIONS____________________________________________________ */
/**
Modbus transaction engine.
Sequence:
  - assemble Modbus Request Application Data Unit (ADU),
    based on particular function called
  - transmit request over selected serial port
  - wait for/retrieve response
  - evaluate/disassemble response
  - return status (success/exception)

@param u8MBFunction Modbus function (0x01..0xFF)
@return 0 on success; exception number on failure
*/
uint8_t ModbusMaster::ModbusMasterTransaction(uint8_t u8MBFunction)
{
  uint8_t u8ModbusADU[264];
  uint8_t u8ModbusADUSize = 0;
  uint8_t i, u8Qty;
  uint16_t u16CRC;
  uint32_t u32StartTime;
  uint8_t u8BytesLeft = 3;
  uint8_t u8MBStatus = ku8MBSuccess;
  uint16_t u16ClientConnect = 0;
  uint8_t u8MBSlv, u8MBFnc, u8MBCntErr, u8MBHigh, u8MBLow, u8MBArrExt;
  int16_t lenRead;
  EthernetClient52 ecClient;
  // uint32_t sendTime, flushTime, availTime, readTime;
  
  
  // assemble Modbus Request Application Data Unit
  if (!_bSerialTrans){
	  u8ModbusADU[u8ModbusADUSize++] = 0;  // need to be random
	  u8ModbusADU[u8ModbusADUSize++] = 0;
	  u8ModbusADU[u8ModbusADUSize++] = 0;
	  u8ModbusADU[u8ModbusADUSize++] = 0;
	  u8ModbusADU[u8ModbusADUSize++] = 0;
	  u8ModbusADU[u8ModbusADUSize++] = 0;  // changed later
	  u8BytesLeft = 9;
	  u8MBSlv = 6;
	  u8MBFnc = 7;
	  u8MBCntErr = 8;
	  // u8MBCutoffSize = 8;
	  u8MBHigh = 9;
	  u8MBArrExt = 9;
	  u8MBLow = 10;
  }
  else{
	  u8MBSlv = 0;
	  u8MBFnc = 1;
	  u8MBCntErr = 2;
	  // u8MBCutoffSize = 2;
	  u8MBHigh = 3;
	  u8MBLow = 4;
	  u8MBArrExt = 5;
  }
  u8ModbusADU[u8ModbusADUSize++] = _u8MBSlave;
  u8ModbusADU[u8ModbusADUSize++] = u8MBFunction;
  
  switch(u8MBFunction)
  {
    case ku8MBReadCoils:
    case ku8MBReadDiscreteInputs:
    case ku8MBReadInputRegisters:
    case ku8MBReadHoldingRegisters:
    case ku8MBReadWriteMultipleRegisters:
      u8ModbusADU[u8ModbusADUSize++] = highByte(_u16ReadAddress);
      u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16ReadAddress);
      u8ModbusADU[u8ModbusADUSize++] = highByte(_u16ReadQty);
      u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16ReadQty);
	  if (!_bSerialTrans){
		  u8ModbusADU[5] = 6;
	  }
      break;
  }
  
  switch(u8MBFunction)
  {
    case ku8MBWriteSingleCoil:
    case ku8MBMaskWriteRegister:
    case ku8MBWriteMultipleCoils:
    case ku8MBWriteSingleRegister:
    case ku8MBWriteMultipleRegisters:
    case ku8MBReadWriteMultipleRegisters:
      u8ModbusADU[u8ModbusADUSize++] = highByte(_u16WriteAddress);
      u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16WriteAddress);
	  if (!_bSerialTrans){
		  return ku8MBInvalidFunction;
	  }
      break;
  }
  
  switch(u8MBFunction)
  {
    case ku8MBWriteSingleCoil:
      u8ModbusADU[u8ModbusADUSize++] = highByte(_u16WriteQty);
      u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16WriteQty);
	  if (!_bSerialTrans){
		  return ku8MBInvalidFunction;
	  }
      break;
      
    case ku8MBWriteSingleRegister:
      u8ModbusADU[u8ModbusADUSize++] = highByte(_u16TransmitBuffer[0]);
      u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16TransmitBuffer[0]);
	  if (!_bSerialTrans){
		  return ku8MBInvalidFunction;
	  }
      break;
      
    case ku8MBWriteMultipleCoils:
      u8ModbusADU[u8ModbusADUSize++] = highByte(_u16WriteQty);
      u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16WriteQty);
      u8Qty = (_u16WriteQty % 8) ? ((_u16WriteQty >> 3) + 1) : (_u16WriteQty >> 3);
      u8ModbusADU[u8ModbusADUSize++] = u8Qty;
      for (i = 0; i < u8Qty; i++)
      {
        switch(i % 2)
        {
          case 0: // i is even
            u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16TransmitBuffer[i >> 1]);
            break;
            
          case 1: // i is odd
            u8ModbusADU[u8ModbusADUSize++] = highByte(_u16TransmitBuffer[i >> 1]);
            break;
        }
      }
	  if (!_bSerialTrans){
		  return ku8MBInvalidFunction;
	  }
      break;
      
    case ku8MBWriteMultipleRegisters:
    case ku8MBReadWriteMultipleRegisters:
      u8ModbusADU[u8ModbusADUSize++] = highByte(_u16WriteQty);
      u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16WriteQty);
      u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16WriteQty << 1);
      
      for (i = 0; i < lowByte(_u16WriteQty); i++)
      {
        u8ModbusADU[u8ModbusADUSize++] = highByte(_u16TransmitBuffer[i]);
        u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16TransmitBuffer[i]);
      }
	  if (!_bSerialTrans){
		  return ku8MBInvalidFunction;
	  }
      break;
      
    case ku8MBMaskWriteRegister:
      u8ModbusADU[u8ModbusADUSize++] = highByte(_u16TransmitBuffer[0]);
      u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16TransmitBuffer[0]);
      u8ModbusADU[u8ModbusADUSize++] = highByte(_u16TransmitBuffer[1]);
      u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16TransmitBuffer[1]);
	  if (!_bSerialTrans){
		  return ku8MBInvalidFunction;
	  }
      break;
  }
  
  if (_bSerialTrans){
	  // append CRC
	  u16CRC = 0xFFFF;
	  for (i = 0; i < u8ModbusADUSize; i++)
	  {
		u16CRC = crc16_update(u16CRC, u8ModbusADU[i]);
	  }
	  u8ModbusADU[u8ModbusADUSize++] = lowByte(u16CRC);
	  u8ModbusADU[u8ModbusADUSize++] = highByte(u16CRC);
	  u8ModbusADU[u8ModbusADUSize] = 0;

	  // flush receive buffer before transmitting request
	  while (MBSerial->read() != -1);  // flush rx, I'm ok with the potential slowness of this since it should be empty

	  // transmit request
	  //digitalWrite(_u8EnablePin, LOW);
	  
	  // for (i = 0; i < u8ModbusADUSize; i++)
	  // {
	// #if defined(ARDUINO) && ARDUINO >= 100
		// MBSerial->write(u8ModbusADU[i]);
	// #else
		// MBSerial->print(u8ModbusADU[i], BYTE);
	// #endif
	  // }
	  
	  MBSerial->write(u8ModbusADU, u8ModbusADUSize);
	  
	  // sendTime = millis();
	  
	  MBSerial->flush();    // flush transmit buffer
	  
	  // flushTime = millis();
	  
	  // MJT
	  // wait until message is sent to raise enable pin
	  if (_u8EnablePin != 255)
	  {
		digitalWrite(_u8EnablePin, HIGH);  // was high
	  }
	  // MJT 
  }
  else{
	  u8ModbusADU[u8ModbusADUSize] = 0;  //                                          is this necessary?
		
	  u16ClientConnect = ecClient.connect(_u8ClientIP, 502);  // should be 502!
	  if (u16ClientConnect){
		  while (ecClient.read() != -1);  // flush rx, I'm ok with the potential slowness of this since it should be empty
		  
		  
		  ecClient.write(u8ModbusADU, u8ModbusADUSize);
		  // for (i = 0; i < u8ModbusADUSize; i++){
			  // ecClient.write(u8ModbusADU[i]);
			  // // Serial.println(u8ModbusADU[i], DEC);
		  // }
		  
		  ecClient.flush();  // flush tx
	  }
  }
  
  
  
  u8ModbusADUSize = 0;
  
  // loop until we run out of time or bytes, or an error occurs
  u32StartTime = millis();

  if (_bSerialTrans || u16ClientConnect){
	  while (u8BytesLeft && !u8MBStatus){
		if (_bSerialTrans) {
			if (MBSerial->available()) {
				// availTime = millis();
				
			  u8ModbusADU[u8ModbusADUSize++] = MBSerial->read();
			  u8BytesLeft--;
			  
			  // lenRead = MBSerial->readBytes(u8ModbusADU + u8ModbusADUSize, u8BytesLeft); //264 - u8ModbusADUSize);
			  // u8ModbusADUSize += lenRead;
			  // u8BytesLeft -= lenRead;
			  
			  // readTime = millis();
			  
			  // Serial.println(F("modbus serial: "));
			  // for (i = 0; i < u8ModbusADUSize; i++) {
				// Serial.print(u8ModbusADU[i], DEC);
				// Serial.print(" ");
			  // }
			  // Serial.println();
			  
			  // Serial.print(F("flush - send: "));
			  // Serial.println(flushTime - sendTime);
			  // Serial.print(F("avail - flush: "));
			  // Serial.println(availTime - flushTime);
			  // Serial.print(F("read - avail: "));
			  // Serial.println(readTime - availTime);
			}
			else{
				// Serial.print("na: ");
				// Serial.println(MBSerial->available(), DEC);
			  if (_idle){
				_idle();
			  }
			}
		}
		else{
			if (ecClient.available()){
			  // u8ModbusADU[u8ModbusADUSize++] = ecClient.read();
			  // // Serial.print((u8ModbusADUSize - 1), DEC);
			  // // Serial.print(F(": "));
			  // // Serial.println(u8ModbusADU[(u8ModbusADUSize - 1)], DEC);
			  // u8BytesLeft--;
			  // // Serial.print(F(", bytes left: "));
			  // // Serial.println(u8BytesLeft, DEC);
			  
			  lenRead = ecClient.read(u8ModbusADU + u8ModbusADUSize, u8BytesLeft); //264 - u8ModbusADUSize);
			  u8ModbusADUSize += lenRead;
			  u8BytesLeft -= lenRead;
			  // Serial.println(F("modbus ethernet: "));
			  // for (i = 0; i < u8ModbusADUSize; i++) {
				// Serial.print(u8ModbusADU[i], DEC);
				// Serial.print(" ");
			  // }
			  // Serial.println();
			}
			else{
			  if (_idle){
				_idle();
			  }
			}
		}
		
		// evaluate slave ID, function code once enough bytes have been read
		if (u8ModbusADUSize == u8MBHigh)  // 3, 9
		// if (u8ModbusADUSize == u8MBCutoffSize + 1)  // 5, 9
		{
		  // verify response is for correct Modbus slave
		  if (u8ModbusADU[u8MBSlv] != _u8MBSlave)  // 0, 6
		  {
			u8MBStatus = ku8MBInvalidSlaveID;
			break;
		  }
		  
		  // verify response is for correct Modbus function code (mask exception bit 7)
		  if ((u8ModbusADU[u8MBFnc] & 0x7F) != u8MBFunction)  // 1, 7
		  {
			u8MBStatus = ku8MBInvalidFunction;
			break;
		  }
		  
		  // check whether Modbus exception occurred; return Modbus Exception Code
		  if (bitRead(u8ModbusADU[u8MBFnc], 7))  // 1, 7
		  {
			u8MBStatus = u8ModbusADU[u8MBCntErr];  // 2, 8
			break;
		  }
		  
		  // evaluate returned Modbus function code
		  switch(u8ModbusADU[u8MBFnc])
		  {
			case ku8MBReadCoils:
			case ku8MBReadDiscreteInputs:
			case ku8MBReadInputRegisters:
			case ku8MBReadHoldingRegisters:
			case ku8MBReadWriteMultipleRegisters:
			  u8BytesLeft = u8ModbusADU[u8MBCntErr] + u8MBArrExt - u8ModbusADUSize;  // expected MB msg len + possible TCP
			  // u8BytesLeft = u8ModbusADU[u8MBCntErr];
			        // header - total read size
					// Serial.println(u8BytesLeft, DEC);
					// Serial.println(u8ModbusADU[u8MBCntErr], DEC);
					// Serial.println(u8ModbusADUSize, DEC);
			  break;
			  
			case ku8MBWriteSingleCoil:
			case ku8MBWriteMultipleCoils:
			case ku8MBWriteSingleRegister:
			case ku8MBWriteMultipleRegisters:
			  u8BytesLeft = 3 + u8MBArrExt - u8ModbusADUSize;
			  break;
			  
			case ku8MBMaskWriteRegister:
			  u8BytesLeft = 5 + u8MBArrExt - u8ModbusADUSize;
			  break;
		  }
		}
		if ((millis() - u32StartTime) > _u16MBResponseTimeout)
		{
		  u8MBStatus = ku8MBResponseTimedOut;
		}
	  }
	  
	  if (!_bSerialTrans){
		ecClient.stop();
	  }
	  
	  if (_bSerialTrans){
		  if (_u8EnablePin != 255)
		  {
			digitalWrite(_u8EnablePin, LOW); // MJT, set pin for transmission  was low
		  }
		  
		  // verify response is large enough to inspect further
		  if (!u8MBStatus && u8ModbusADUSize >= 5)
		  {
			// calculate CRC
			u16CRC = 0xFFFF;
			for (i = 0; i < (u8ModbusADUSize - 2); i++)
			{
			  u16CRC = crc16_update(u16CRC, u8ModbusADU[i]);
			}
			
			// verify CRC
			if (!u8MBStatus && (lowByte(u16CRC) != u8ModbusADU[u8ModbusADUSize - 2] ||
			  highByte(u16CRC) != u8ModbusADU[u8ModbusADUSize - 1]))
			{
			  u8MBStatus = ku8MBInvalidCRC;
			}
		  }
	  }

	  // disassemble ADU into words
	  if (!u8MBStatus){
		// evaluate returned Modbus function code
		switch(u8ModbusADU[u8MBFnc]){  // 1, 7
		  case ku8MBReadCoils:
		  case ku8MBReadDiscreteInputs:
			// load bytes into word; response bytes are ordered L, H, L, H, ...
			for (i = 0; i < (u8ModbusADU[u8MBCntErr] >> 1); i++){  // 2, 8
			  if (i < ku8MaxBufferSize){
				_u16ResponseBuffer[i] = word(u8ModbusADU[2 * i + u8MBLow], u8ModbusADU[2 * i + u8MBHigh]);
			  }
			  _u8ResponseBufferLength = i;
			}
			
			// in the event of an odd number of bytes, load last byte into zero-padded word
			if (u8ModbusADU[u8MBCntErr] % 2){  // 2, 8
			  if (i < ku8MaxBufferSize){
				_u16ResponseBuffer[i] = word(0, u8ModbusADU[2 * i + u8MBHigh]);
			  }
			  
			  _u8ResponseBufferLength = i + 1;
			}
			break;
			
		  case ku8MBReadInputRegisters:
		  case ku8MBReadHoldingRegisters:
		  case ku8MBReadWriteMultipleRegisters:
			// load bytes into word; response bytes are ordered H, L, H, L, ...
			// Serial.println(F("buffer:"));
			for (i = 0; i < (u8ModbusADU[u8MBCntErr] >> 1); i++){  // 2, 8
			  if (i < ku8MaxBufferSize){
				_u16ResponseBuffer[i] = word(u8ModbusADU[2 * i + u8MBHigh], u8ModbusADU[2 * i + u8MBLow]);
				
				// Serial.print(i, DEC);
				// Serial.print(F(": "));
				// Serial.println(_u16ResponseBuffer[i], DEC);
			  }
			  
			  _u8ResponseBufferLength = i;
			}
			break;
		}
	  }
	  
	  _u8TransmitBufferIndex = 0;
	  u16TransmitBufferLength = 0;
	  _u8ResponseBufferIndex = 0;
	  return u8MBStatus;
  }
  u8MBStatus = ku8MBInvalidClient;  // could not connect
  return u8MBStatus;
}

uint16_t ModbusMaster::crc16_update(uint16_t crc, uint8_t a)
{
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
