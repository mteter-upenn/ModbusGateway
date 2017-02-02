#include "meters.h"
#include <EEPROM.h>
#include <ModbusStructs.h>

uint16_t meter11(uint16_t u16_mapStart){
  uint16_t u16_blkStart;
  uint8_t u8_numBlks;
  uint16_t u16_grpStart;
  uint8_t u8_numGrps;

  // chw kep *****************************************************
  // 
  
  Serial.print(F("meter 11: "));
  Serial.print(u16_mapStart, DEC);

  u8_numBlks = 5;  
  u8_numGrps = 2;
  u16_blkStart = u16_mapStart + 4 + u8_numGrps * 2;
  // used to be 66, now it's 54
  u16_grpStart = u16_blkStart + (u8_numBlks * 5);
  
  EEPROM.write(u16_mapStart, highByte(u16_blkStart));
  EEPROM.write(u16_mapStart + 1, lowByte(u16_blkStart));
  EEPROM.write(u16_mapStart + 2, u8_numBlks);  // number of blocks;

  EEPROM.write(u16_mapStart + 3, u8_numGrps);  // number of blocks;
  EEPROM.write(u16_mapStart + 4, highByte(u16_grpStart));
  EEPROM.write(u16_mapStart + 5, lowByte(u16_grpStart));
  EEPROM.write(u16_mapStart + 6, highByte(u16_grpStart + 19));
  EEPROM.write(u16_mapStart + 7, lowByte(u16_grpStart + 19));


  // Block #1 - [0, 42, 0]
  EEPROM.write(u16_blkStart, highByte(0));
  EEPROM.write(u16_blkStart + 1, lowByte(0));
  EEPROM.write(u16_blkStart + 2, highByte(42));
  EEPROM.write(u16_blkStart + 3, lowByte(42));
  EEPROM.write(u16_blkStart + 4, FloatConv2Int8(FloatConv::FLOAT));

  // Block #2 - [44, 49, 2]
  EEPROM.write(u16_blkStart + 5, highByte(44));
  EEPROM.write(u16_blkStart + 6, lowByte(44));
  EEPROM.write(u16_blkStart + 7, highByte(49));
  EEPROM.write(u16_blkStart + 8, lowByte(49));
  EEPROM.write(u16_blkStart + 9, FloatConv2Int8(FloatConv::INT16));

  // Block #3 - [50, 58, 0]
  EEPROM.write(u16_blkStart + 10, highByte(50));
  EEPROM.write(u16_blkStart + 11, lowByte(50));
  EEPROM.write(u16_blkStart + 12, highByte(58));
  EEPROM.write(u16_blkStart + 13, lowByte(58));
  EEPROM.write(u16_blkStart + 14, FloatConv2Int8(FloatConv::FLOAT));

  // Block #4 - [60, 61, 2]
  EEPROM.write(u16_blkStart + 15, highByte(60));
  EEPROM.write(u16_blkStart + 16, lowByte(60));
  EEPROM.write(u16_blkStart + 17, highByte(61));
  EEPROM.write(u16_blkStart + 18, lowByte(61));
  EEPROM.write(u16_blkStart + 19, FloatConv2Int8(FloatConv::INT16));

  // Block #5 - [62, 62, 0]
  EEPROM.write(u16_blkStart + 20, highByte(62));
  EEPROM.write(u16_blkStart + 21, lowByte(62));
  EEPROM.write(u16_blkStart + 22, highByte(62));
  EEPROM.write(u16_blkStart + 23, lowByte(62));
  EEPROM.write(u16_blkStart + 24, FloatConv2Int8(FloatConv::FLOAT));



  // New Group #1
  EEPROM.write(u16_grpStart, 9);              // number of values
  EEPROM.write(++u16_grpStart, 38);           // number of registers
  EEPROM.write(++u16_grpStart, highByte(0));  // starting register
  EEPROM.write(++u16_grpStart, lowByte(0));   // 
  EEPROM.write(++u16_grpStart, 17);             // skip bytes to data types
  // value types
  EEPROM.write(++u16_grpStart, 1);
  EEPROM.write(++u16_grpStart, 2);
  EEPROM.write(++u16_grpStart, int8_t(-2));
  EEPROM.write(++u16_grpStart, 3);
  EEPROM.write(++u16_grpStart, 4);
  EEPROM.write(++u16_grpStart, 5);
  EEPROM.write(++u16_grpStart, 6);
  EEPROM.write(++u16_grpStart, int8_t(-16));
  EEPROM.write(++u16_grpStart, 8);
  EEPROM.write(++u16_grpStart, 9);
  EEPROM.write(++u16_grpStart, int8_t(-2));
  EEPROM.write(++u16_grpStart, 10);
  // datatypes
  EEPROM.write(++u16_grpStart, FloatConv2Int8(FloatConv::FLOAT));
  EEPROM.write(++u16_grpStart, 255);


  // New Group #2 (LAST), +19
  EEPROM.write(++u16_grpStart, 1); // number of values
  // value types
  EEPROM.write(++u16_grpStart, 7);


  //EEPROM.write(++u16_grpStart, );

 
  Serial.print(F(" to "));
  Serial.println(u16_grpStart, DEC);

  return ++u16_grpStart;
}
