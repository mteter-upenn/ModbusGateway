#include "meters.h"

uint16_t meter6(uint16_t u16_mapStart){
  uint16_t u16_blkStart;
  uint8_t u8_numBlks;
  uint16_t u16_grpStart;
  uint8_t u8_numGrps;
  
  // siemens 9510, 9610 meter 6 **********************************************************
  // 126
 
  Serial.print(F("meter 6: "));
  Serial.print(u16_mapStart, DEC);
  
  u8_numBlks = 4;
  u8_numGrps = 2;
  u16_blkStart = u16_mapStart + 4 + u8_numGrps * 2;

  // used to be 126, now it's 84
  u16_grpStart = u16_blkStart + (u8_numBlks * 5);  // 28+56
  // u16_mapStart = 3264
  EEPROM.write(u16_mapStart, highByte(u16_blkStart));  // EEPROM address for block start
  EEPROM.write(u16_mapStart + 1, lowByte(u16_blkStart));
  EEPROM.write(u16_mapStart + 2, u8_numBlks);  // number of blocks;

  EEPROM.write(u16_mapStart + 3, u8_numGrps);  // number of groups;
  EEPROM.write(u16_mapStart + 4, highByte(u16_grpStart));  // group 1 address
  EEPROM.write(u16_mapStart + 5, lowByte(u16_grpStart));
  EEPROM.write(u16_mapStart + 6, highByte(u16_grpStart + 54));  // group 2 address
  EEPROM.write(u16_mapStart + 7, lowByte(u16_grpStart + 54));


  // Block #1 - [149, 164, 1]
  EEPROM.write(u16_blkStart, highByte(149));
  EEPROM.write(u16_blkStart + 1, lowByte(149));
  EEPROM.write(u16_blkStart + 2, highByte(164));
  EEPROM.write(u16_blkStart + 3, lowByte(164));
  EEPROM.write(u16_blkStart + 4, FloatConv2Int8(FloatConv::UINT16));

  // Block #2 - [165, 187, 3 | 128]
  EEPROM.write(u16_blkStart + 5, highByte(165));
  EEPROM.write(u16_blkStart + 6, lowByte(165));
  EEPROM.write(u16_blkStart + 7, highByte(187));
  EEPROM.write(u16_blkStart + 8, lowByte(187));
  EEPROM.write(u16_blkStart + 9, FloatConv2Int8(FloatConv::UINT32_WS));
  
  // Block #3 - [197, 237, 4 | 128]
  EEPROM.write(u16_blkStart + 10, highByte(197));
  EEPROM.write(u16_blkStart + 11, lowByte(197));
  EEPROM.write(u16_blkStart + 12, highByte(237));
  EEPROM.write(u16_blkStart + 13, lowByte(237));
  EEPROM.write(u16_blkStart + 14, FloatConv2Int8(FloatConv::INT32_WS));

  // Block #4 - [261, 276, 2]
  EEPROM.write(u16_blkStart + 15, highByte(261));
  EEPROM.write(u16_blkStart + 16, lowByte(261));
  EEPROM.write(u16_blkStart + 17, highByte(276));
  EEPROM.write(u16_blkStart + 18, lowByte(276));
  EEPROM.write(u16_blkStart + 19, FloatConv2Int8(FloatConv::INT16));


  // New Group #1, +0
  EEPROM.write(u16_grpStart, 31);                 // number of values
  EEPROM.write(++u16_grpStart, 116);              // number of registers
  EEPROM.write(++u16_grpStart, highByte(149));   // starting register
  EEPROM.write(++u16_grpStart, lowByte(149));    // starting register
  EEPROM.write(++u16_grpStart, 46);              // skip bytes to data types
  // value types
  EEPROM.write(++u16_grpStart, 1);
  EEPROM.write(++u16_grpStart, 2);
  EEPROM.write(++u16_grpStart, 3);
  EEPROM.write(++u16_grpStart, int8_t(-2));
  EEPROM.write(++u16_grpStart, 4);
  EEPROM.write(++u16_grpStart, int8_t(-10));
  EEPROM.write(++u16_grpStart, 6);
  EEPROM.write(++u16_grpStart, 7);
  EEPROM.write(++u16_grpStart, 8);
  EEPROM.write(++u16_grpStart, 9);
  EEPROM.write(++u16_grpStart, int8_t(-4));
  EEPROM.write(++u16_grpStart, 10);
  EEPROM.write(++u16_grpStart, 11);
  EEPROM.write(++u16_grpStart, 12);
  EEPROM.write(++u16_grpStart, 13);
  EEPROM.write(++u16_grpStart, int8_t(-12));
  EEPROM.write(++u16_grpStart, 14);
  EEPROM.write(++u16_grpStart, 15);
  EEPROM.write(++u16_grpStart, 16);
  EEPROM.write(++u16_grpStart, 17);
  EEPROM.write(++u16_grpStart, int8_t(-2));
  EEPROM.write(++u16_grpStart, 18);
  EEPROM.write(++u16_grpStart, 19);
  EEPROM.write(++u16_grpStart, 20);
  EEPROM.write(++u16_grpStart, 21);
  EEPROM.write(++u16_grpStart, int8_t(-2));
  EEPROM.write(++u16_grpStart, 22);
  EEPROM.write(++u16_grpStart, 23);
  EEPROM.write(++u16_grpStart, 24);
  EEPROM.write(++u16_grpStart, 25);
  EEPROM.write(++u16_grpStart, int8_t(-4));
  EEPROM.write(++u16_grpStart, 30);
  EEPROM.write(++u16_grpStart, int8_t(-2));
  EEPROM.write(++u16_grpStart, 31);
  EEPROM.write(++u16_grpStart, int8_t(-2));
  EEPROM.write(++u16_grpStart, 32);
  EEPROM.write(++u16_grpStart, int8_t(-22));
  EEPROM.write(++u16_grpStart, 26);
  EEPROM.write(++u16_grpStart, 27);
  EEPROM.write(++u16_grpStart, 28);
  EEPROM.write(++u16_grpStart, 29);
  // datatypes
  EEPROM.write(++u16_grpStart, FloatConv2Int8(FloatConv::UINT16));
  EEPROM.write(++u16_grpStart, 4);
  EEPROM.write(++u16_grpStart, FloatConv2Int8(FloatConv::UINT32_WS));
  EEPROM.write(++u16_grpStart, 12);
  EEPROM.write(++u16_grpStart, FloatConv2Int8(FloatConv::INT32_WS));
  EEPROM.write(++u16_grpStart, 27);
  EEPROM.write(++u16_grpStart, FloatConv2Int8(FloatConv::INT16));
  EEPROM.write(++u16_grpStart, 255);


  // New Group #2 (LAST), +54
  EEPROM.write(++u16_grpStart, 1); // number of values
  // value types
  EEPROM.write(++u16_grpStart, 5);

  
  Serial.print(F(" to "));
  Serial.println(u16_grpStart, DEC);  // +55

  return ++u16_grpStart;
}

