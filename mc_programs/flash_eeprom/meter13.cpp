#include "meters.h"
#include <EEPROM.h>
#include <ModbusStructs.h>

uint16_t meter13(uint16_t u16_mapStart){
  uint16_t u16_blkStart;
  uint8_t u8_numBlks;
  uint16_t u16_grpStart;
  uint8_t u8_numGrps;

  // squareD pm210, meter 13 **************************************************************
  // 97
  
  Serial.print(F("meter 13: "));
  Serial.print(u16_mapStart, DEC);

  u8_numBlks = 3;
  u8_numGrps = 2;
  u16_blkStart = u16_mapStart + 4 + u8_numGrps * 2;
  // used to be 97, now it's 65
  u16_grpStart = u16_blkStart + (u8_numBlks * 5);
  
  EEPROM.write(u16_mapStart, highByte(u16_blkStart));
  EEPROM.write(u16_mapStart + 1, lowByte(u16_blkStart));
  EEPROM.write(u16_mapStart + 2, u8_numBlks);  // number of blocks;

  EEPROM.write(u16_mapStart + 3, u8_numGrps);  // number of blocks;
  EEPROM.write(u16_mapStart + 4, highByte(u16_grpStart));
  EEPROM.write(u16_mapStart + 5, lowByte(u16_grpStart));
  EEPROM.write(u16_mapStart + 6, highByte(u16_grpStart + 25));
  EEPROM.write(u16_mapStart + 7, lowByte(u16_grpStart + 25));

  // Block #1 - [999, 1063, 128]
  EEPROM.write(u16_blkStart, highByte(999));
  EEPROM.write(u16_blkStart + 1, lowByte(999));
  EEPROM.write(u16_blkStart + 2, highByte(1063));
  EEPROM.write(u16_blkStart + 3, lowByte(1063));
  EEPROM.write(u16_blkStart + 4, FloatConv2Int8(FloatConv::FLOAT_WS));

  // Block #2 - [3999, 4003, (3 | 128]
  EEPROM.write(u16_blkStart + 5, highByte(3999));
  EEPROM.write(u16_blkStart + 6, lowByte(3999));
  EEPROM.write(u16_blkStart + 7, highByte(4003));
  EEPROM.write(u16_blkStart + 8, lowByte(4003));
  EEPROM.write(u16_blkStart + 9, FloatConv2Int8(FloatConv::UINT32_WS));
  
  // Block #3 - [4005, 4034, 2]
  EEPROM.write(u16_blkStart + 10, highByte(4005));
  EEPROM.write(u16_blkStart + 11, lowByte(4005));
  EEPROM.write(u16_blkStart + 12, highByte(4034));
  EEPROM.write(u16_blkStart + 13, lowByte(4034));
  EEPROM.write(u16_blkStart + 14, FloatConv2Int8(FloatConv::INT16));

  
  // New Group #1, +0
  EEPROM.write(u16_grpStart, 16);                 // number of values
  EEPROM.write(++u16_grpStart, 66);              // number of registers
  EEPROM.write(++u16_grpStart, highByte(999));  // starting register
  EEPROM.write(++u16_grpStart, lowByte(999));   // starting register
  EEPROM.write(++u16_grpStart, 23);              // skip bytes to data types
                                             // value types
  EEPROM.write(++u16_grpStart, 30);
  EEPROM.write(++u16_grpStart, 32);
  EEPROM.write(++u16_grpStart, 31);
  EEPROM.write(++u16_grpStart, 17);
  EEPROM.write(++u16_grpStart, 25);
  EEPROM.write(++u16_grpStart, 21);
  EEPROM.write(++u16_grpStart, 29);
  EEPROM.write(++u16_grpStart, int8_t(-20));
  EEPROM.write(++u16_grpStart, 1);
  EEPROM.write(++u16_grpStart, 2);
  EEPROM.write(++u16_grpStart, 3);
  EEPROM.write(++u16_grpStart, int8_t(-14));
  EEPROM.write(++u16_grpStart, 10);
  EEPROM.write(++u16_grpStart, 11);
  EEPROM.write(++u16_grpStart, 12);
  EEPROM.write(++u16_grpStart, 6);
  EEPROM.write(++u16_grpStart, 7);
  EEPROM.write(++u16_grpStart, 8);
  // datatypes
  EEPROM.write(++u16_grpStart, FloatConv2Int8(FloatConv::FLOAT_WS));
  EEPROM.write(++u16_grpStart, 255);


  // New Group #2 (LAST), +25
  EEPROM.write(++u16_grpStart, 16); // number of values
                                // value types
  EEPROM.write(++u16_grpStart, 23);
  EEPROM.write(++u16_grpStart, 24);
  EEPROM.write(++u16_grpStart, 28);
  EEPROM.write(++u16_grpStart, 27);
  EEPROM.write(++u16_grpStart, 26);
  EEPROM.write(++u16_grpStart, 22);
  EEPROM.write(++u16_grpStart, 13);
  EEPROM.write(++u16_grpStart, 14);
  EEPROM.write(++u16_grpStart, 9);
  EEPROM.write(++u16_grpStart, 4);
  EEPROM.write(++u16_grpStart, 5);
  EEPROM.write(++u16_grpStart, 19);
  EEPROM.write(++u16_grpStart, 20);
  EEPROM.write(++u16_grpStart, 18);
  EEPROM.write(++u16_grpStart, 15);
  EEPROM.write(++u16_grpStart, 16);  //41

  
  Serial.print(F(" to "));
  Serial.println(u16_grpStart, DEC);

  return ++u16_grpStart;
}
