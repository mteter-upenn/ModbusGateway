#include "meters.h"
#include <EEPROM.h>
#include <ModbusStructs.h>

uint16_t meter10(uint16_t u16_mapStart){
  uint16_t u16_blkStart;
  uint8_t u8_numBlks;
  uint16_t u16_grpStart;
  uint8_t u8_numGrps;

  // squareD pm 800 series, meter 10 *****************************************************
  // 114
  
  Serial.print(F("meter 10: "));
  Serial.print(u16_mapStart, DEC);

  u8_numBlks = 4;
  u8_numGrps = 3;
  u16_blkStart = u16_mapStart + 4 + u8_numGrps * 2;
  // used to be 114, now it's 82
  u16_grpStart = u16_blkStart + (u8_numBlks * 5);
  
  EEPROM.write(u16_mapStart, highByte(u16_blkStart));
  EEPROM.write(u16_mapStart + 1, lowByte(u16_blkStart));
  EEPROM.write(u16_mapStart + 2, u8_numBlks);  // number of blocks;

  EEPROM.write(u16_mapStart + 3, u8_numGrps);  // number of blocks;
  EEPROM.write(u16_mapStart + 4, highByte(u16_grpStart));
  EEPROM.write(u16_mapStart + 5, lowByte(u16_grpStart));
  EEPROM.write(u16_mapStart + 6, highByte(u16_grpStart + 40));
  EEPROM.write(u16_mapStart + 7, lowByte(u16_grpStart + 40));
  EEPROM.write(u16_mapStart + 8, highByte(u16_grpStart + 50));
  EEPROM.write(u16_mapStart + 9, lowByte(u16_grpStart + 50));

  // Block #1 - [1099, 1298, 2]
  EEPROM.write(u16_blkStart, highByte(1099));
  EEPROM.write(u16_blkStart + 1, lowByte(1099));
  EEPROM.write(u16_blkStart + 2, highByte(1298));
  EEPROM.write(u16_blkStart + 3, lowByte(1298));
  EEPROM.write(u16_blkStart + 4, FloatConv2Int8(FloatConv::INT16));

  // Block #2 - [1699, 1743, 8]
  EEPROM.write(u16_blkStart + 5, highByte(1699));
  EEPROM.write(u16_blkStart + 6, lowByte(1699));
  EEPROM.write(u16_blkStart + 7, highByte(1746));
  EEPROM.write(u16_blkStart + 8, lowByte(1746));
  EEPROM.write(u16_blkStart + 9, FloatConv2Int8(FloatConv::MOD30K));

  // Block #3 - [1747, 1759, 7]
  EEPROM.write(u16_blkStart + 10, highByte(1747));
  EEPROM.write(u16_blkStart + 11, lowByte(1747));
  EEPROM.write(u16_blkStart + 12, highByte(1759));
  EEPROM.write(u16_blkStart + 13, lowByte(1759));
  EEPROM.write(u16_blkStart + 14, FloatConv2Int8(FloatConv::MOD20K));

  // Block #4 - [1766, 1790, 7]
  EEPROM.write(u16_blkStart + 15, highByte(1766));
  EEPROM.write(u16_blkStart + 16, lowByte(1766));
  EEPROM.write(u16_blkStart + 17, highByte(1790));
  EEPROM.write(u16_blkStart + 18, lowByte(1790));
  EEPROM.write(u16_blkStart + 19, FloatConv2Int8(FloatConv::MOD20K));

  

  // New Group #1, +0
  EEPROM.write(u16_grpStart, 28);                 // number of values
  EEPROM.write(++u16_grpStart, 68);              // number of registers
  EEPROM.write(++u16_grpStart, highByte(1099));  // starting register
  EEPROM.write(++u16_grpStart, lowByte(1099));   // starting register
  EEPROM.write(++u16_grpStart, 38);              // skip bytes to data types
                                             // value types
  EEPROM.write(++u16_grpStart, 1);
  EEPROM.write(++u16_grpStart, 2);
  EEPROM.write(++u16_grpStart, 3);
  EEPROM.write(++u16_grpStart, int8_t(-1));
  EEPROM.write(++u16_grpStart, 4);
  EEPROM.write(++u16_grpStart, int8_t(-15));
  EEPROM.write(++u16_grpStart, 10);
  EEPROM.write(++u16_grpStart, 11);
  EEPROM.write(++u16_grpStart, 12);
  EEPROM.write(++u16_grpStart, 13);
  EEPROM.write(++u16_grpStart, 6);
  EEPROM.write(++u16_grpStart, 7);
  EEPROM.write(++u16_grpStart, 8);
  EEPROM.write(++u16_grpStart, int8_t(-1));
  EEPROM.write(++u16_grpStart, 9);
  EEPROM.write(++u16_grpStart, int8_t(-11));
  EEPROM.write(++u16_grpStart, 14);
  EEPROM.write(++u16_grpStart, 15);
  EEPROM.write(++u16_grpStart, 16);
  EEPROM.write(++u16_grpStart, 17);
  EEPROM.write(++u16_grpStart, 18);
  EEPROM.write(++u16_grpStart, 19);
  EEPROM.write(++u16_grpStart, 20);
  EEPROM.write(++u16_grpStart, 21);
  EEPROM.write(++u16_grpStart, 22);
  EEPROM.write(++u16_grpStart, 23);
  EEPROM.write(++u16_grpStart, 24);
  EEPROM.write(++u16_grpStart, 25);
  EEPROM.write(++u16_grpStart, int8_t(-12));
  EEPROM.write(++u16_grpStart, 26);
  EEPROM.write(++u16_grpStart, 27);
  EEPROM.write(++u16_grpStart, 28);
  EEPROM.write(++u16_grpStart, 29);
  // datatypes
  EEPROM.write(++u16_grpStart, FloatConv2Int8(FloatConv::INT16));
  EEPROM.write(++u16_grpStart, 255);


  // New Group #2, +40
  EEPROM.write(++u16_grpStart, 3);               // number of values
  EEPROM.write(++u16_grpStart, 12);              // number of registers
  EEPROM.write(++u16_grpStart, highByte(1715));  // starting register
  EEPROM.write(++u16_grpStart, lowByte(1715));   // starting register
  EEPROM.write(++u16_grpStart, 8);              // skip bytes to data types
                                            // value types
  EEPROM.write(++u16_grpStart, 30);
  EEPROM.write(++u16_grpStart, 31);
  EEPROM.write(++u16_grpStart, 32);
  // datatypes
  EEPROM.write(++u16_grpStart, FloatConv2Int8(FloatConv::MOD30K));
  EEPROM.write(++u16_grpStart, 255);


  // New Group #3 (LAST), +50
  EEPROM.write(++u16_grpStart, 1); // number of values
                               // value types
  EEPROM.write(++u16_grpStart, 5);

  
  Serial.print(F(" to "));
  Serial.println(u16_grpStart, DEC);

  return ++u16_grpStart;
}
