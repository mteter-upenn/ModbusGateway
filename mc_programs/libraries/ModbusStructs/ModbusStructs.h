#ifndef ModbusStructs_h
#define ModbusStructs_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


struct ModbusRequest {
	uint16_t  u16_unqId;
	uint8_t   u8_flags;
	uint8_t   u8_id;
	uint8_t   u8_vid;
	uint8_t   u8_func;
	uint16_t  u16_start;
	uint16_t  u16_length;
	uint8_t   u8_mtrType;  // 
	// bool      b_adjReq;
	// bool      b_sentReq;
	// uint8_t  u8_priority;  // try to use priority flags to show where new priorities begin
};

/*
ModbusRequest.u8_flags how bits are organized as flags
7 6 5 4 3 2 1 0
x x x x o x x x  - 0 is serial, 1 is tcp
x x x x x o o o  - socket index (only used for tcp)
x x x o x x x x  - data received from device and waiting in buffer
x x o x x x x x  - timeout, data not received, buffer is empty
x o x x x x x x  - sent modbus request
o x x x x x x x  - 0 is unadjusted data, 1 is adjusted

*/
const uint8_t MRFLAG_sckMask = 0x07;
const uint8_t MRFLAG_isTcp = 0x08;
const uint8_t MRFLAG_goodData = 0x10;
const uint8_t MRFLAG_timeout = 0x20;
const uint8_t MRFLAG_sentMsg = 0x40;
const uint8_t MRFLAG_adjReq = 0x80;


/**
enum class for definitions of different data types meters can return
*/
enum class FloatConv {
  SKIP = -1,        //  -1
  FLOAT,            //   0
  UINT16,           //   1
  INT16,            //   2
  UINT32,           //   3
  INT32,            //   4
  MOD1K,            //   5
  MOD10K,           //   6
  MOD20K,           //   7
  MOD30K,           //   8
  UINT64,           //   9
  ENERGY,           //  10
  DOUBLE,           //  11
  FLOAT_WS = 64,    //  64
	//UINT16_WS,
	//INT16_WS,
  UINT32_WS = 67,   //  67
  INT32_WS,         //  68
  MOD1K_WS,         //  69
  MOD10K_WS,        //  70
  MOD20K_WS,        //  71
  MOD30K_WS,        //  72
  UINT64_WS,        //  73
  ENERGY_WS,        //  74
  DOUBLE_WS,        //  75
};

uint16_t FloatConvEnumNumRegs(FloatConv dataType);
int8_t FloatConv2Int8(FloatConv dataType);
uint8_t FloatConv2Uint8(FloatConv dataType);
FloatConv Int8_2_FloatConv(int8_t s8_dataType);

#endif