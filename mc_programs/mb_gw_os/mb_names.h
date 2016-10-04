// mb_names.h

#ifndef _MB_NAMES_h
#define _MB_NAMES_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

enum class FileType {
  NONE = -1,
  HTML,
  CSS,
  GIF,
  XML,
  CSV,
  TXT
};

enum class XmlFile {
  INFO,
  GENERAL,
  METER
};


enum SockFlag {
  SockFlag_LISTEN = 0x00,
  SockFlag_ESTABLISHED = 0x01,
  SockFlag_MODBUS = 0x02,
  SockFlag_HTTP = 0x04,
  SockFlag_MB485 = 0x08,
  SockFlag_MBTCP = 0x16,
  SockFlag_SENT_MSG = 0x32

};

//enum class FloatConv {
//  SKIP = -1,        //  -1
//  FLOAT,            //   0
//  UINT16,           //   1
//  INT16,            //   2
//  UINT32,           //   3
//  INT32,            //   4
//  MOD1K,            //   5
//  MOD10K,           //   6
//  MOD20K,           //   7
//  MOD30K,           //   8
//  UINT64,           //   9
//  ENERGY,           //  10
//  DOUBLE,           //  11
//  FLOAT_WS = 64,    //  64
//                    //UINT16_WS,
//                    //INT16_WS,
//  UINT32_WS = 67,   //  67
//  INT32_WS,         //  68
//  MOD1K_WS,         //  69
//  MOD10K_WS,        //  70
//  MOD20K_WS,        //  71
//  MOD30K_WS,        //  72
//  UINT64_WS,        //  73
//  ENERGY_WS,        //  74
//  DOUBLE_WS,        //  75
//};

#endif

