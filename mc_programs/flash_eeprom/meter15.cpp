#include "meters.h"

uint16_t meter15(uint16_t u16_mapStart) {
  uint16_t u16_blkStart;
  uint8_t u8_numBlks;
  uint16_t u16_grpStart;
  uint8_t u8_numGrps;

  // eaton series pxm 2000 **************************************************************
  // 108

  Serial.print(F("meter 15: "));
  Serial.print(u16_mapStart, DEC);

  u8_numBlks = 1;
  u8_numGrps = 6;
  u16_blkStart = u16_mapStart + 4 + u8_numGrps * 2;
  // used to be 108, now it's 94
  u16_grpStart = u16_blkStart + (u8_numBlks * 5);

  EEPROM.write(u16_mapStart, highByte(u16_blkStart));
  EEPROM.write(u16_mapStart + 1, lowByte(u16_blkStart));
  EEPROM.write(u16_mapStart + 2, u8_numBlks);  // number of blocks;

  EEPROM.write(u16_mapStart + 3, u8_numGrps);  // number of groups;
  EEPROM.write(u16_mapStart + 4, highByte(u16_grpStart));
  EEPROM.write(u16_mapStart + 5, lowByte(u16_grpStart));
  EEPROM.write(u16_mapStart + 6, highByte(u16_grpStart + 15));
  EEPROM.write(u16_mapStart + 7, lowByte(u16_grpStart + 15));
  EEPROM.write(u16_mapStart + 8, highByte(u16_grpStart + 27));
  EEPROM.write(u16_mapStart + 9, lowByte(u16_grpStart + 27));
  EEPROM.write(u16_mapStart + 10, highByte(u16_grpStart + 43));
  EEPROM.write(u16_mapStart + 11, lowByte(u16_grpStart + 43));
  EEPROM.write(u16_mapStart + 12, highByte(u16_grpStart + 60));
  EEPROM.write(u16_mapStart + 13, lowByte(u16_grpStart + 60));
  EEPROM.write(u16_mapStart + 14, highByte(u16_grpStart + 71));
  EEPROM.write(u16_mapStart + 15, lowByte(u16_grpStart + 71));



                                         // Block #1 - [1, 201, 128]
  EEPROM.write(u16_blkStart, highByte(4002));
  EEPROM.write(u16_blkStart + 1, lowByte(4002));
  EEPROM.write(u16_blkStart + 2, highByte(7034));
  EEPROM.write(u16_blkStart + 3, lowByte(7034));
  EEPROM.write(u16_blkStart + 4, FloatConv2Int8(FloatConv::FLOAT_WS));


  // New Group #1, +0
  EEPROM.write(u16_grpStart, 8);                 // number of values
  EEPROM.write(++u16_grpStart, 16);              // number of registers
  EEPROM.write(++u16_grpStart, highByte(4002));  // starting register
  EEPROM.write(++u16_grpStart, lowByte(4002));   // starting register
  EEPROM.write(++u16_grpStart, 13);              // skip bytes to data types
                                             // value types
  EEPROM.write(++u16_grpStart, 10);
  EEPROM.write(++u16_grpStart, 11);
  EEPROM.write(++u16_grpStart, 12);
  EEPROM.write(++u16_grpStart, 13);
  EEPROM.write(++u16_grpStart, 6);
  EEPROM.write(++u16_grpStart, 7);
  EEPROM.write(++u16_grpStart, 8);
  EEPROM.write(++u16_grpStart, 9);
  // datatypes
  EEPROM.write(++u16_grpStart, FloatConv2Int8(FloatConv::FLOAT_WS));
  EEPROM.write(++u16_grpStart, 255);


  // New Group #2, +15
  EEPROM.write(++u16_grpStart, 4);               // number of values
  EEPROM.write(++u16_grpStart, 12);              // number of registers
  EEPROM.write(++u16_grpStart, highByte(5002));  // starting register
  EEPROM.write(++u16_grpStart, lowByte(5002));   // starting register
  EEPROM.write(++u16_grpStart, 10);              // skip bytes to data types
                                             // value types
  EEPROM.write(++u16_grpStart, 1);
  EEPROM.write(++u16_grpStart, 2);
  EEPROM.write(++u16_grpStart, 3);
  EEPROM.write(++u16_grpStart, int8_t(-4));
  EEPROM.write(++u16_grpStart, 4);
  // datatypes
  EEPROM.write(++u16_grpStart, FloatConv2Int8(FloatConv::FLOAT_WS));
  EEPROM.write(++u16_grpStart, 255);


  // New Group #3, +27
  EEPROM.write(++u16_grpStart, 8);               // number of values
  EEPROM.write(++u16_grpStart, 72);              // number of registers
  EEPROM.write(++u16_grpStart, highByte(6000));  // starting register
  EEPROM.write(++u16_grpStart, lowByte(6000));   // starting register
  EEPROM.write(++u16_grpStart, 14);              // skip bytes to data types
                                             // value types
  EEPROM.write(++u16_grpStart, 14);
  EEPROM.write(++u16_grpStart, 15);
  EEPROM.write(++u16_grpStart, 16);
  EEPROM.write(++u16_grpStart, 17);
  EEPROM.write(++u16_grpStart, int8_t(-56));
  EEPROM.write(++u16_grpStart, 22);
  EEPROM.write(++u16_grpStart, 23);
  EEPROM.write(++u16_grpStart, 24);
  EEPROM.write(++u16_grpStart, 25);
  // datatypes
  EEPROM.write(++u16_grpStart, FloatConv2Int8(FloatConv::FLOAT_WS));
  EEPROM.write(++u16_grpStart, 255);


  // New Group #4, +43
  EEPROM.write(++u16_grpStart, 8);               // number of values
  EEPROM.write(++u16_grpStart, 94);              // number of registers
  EEPROM.write(++u16_grpStart, highByte(6128));  // starting register
  EEPROM.write(++u16_grpStart, lowByte(6128));   // starting register
  EEPROM.write(++u16_grpStart, 15);              // skip bytes to data types
                                             // value types
  EEPROM.write(++u16_grpStart, 18);
  EEPROM.write(++u16_grpStart, 19);
  EEPROM.write(++u16_grpStart, 20);
  EEPROM.write(++u16_grpStart, 21);
  EEPROM.write(++u16_grpStart, int8_t(-76));
  EEPROM.write(++u16_grpStart, 26);
  EEPROM.write(++u16_grpStart, 27);
  EEPROM.write(++u16_grpStart, 28);
  EEPROM.write(++u16_grpStart, int8_t(-2));
  EEPROM.write(++u16_grpStart, 29);
  // datatypes
  EEPROM.write(++u16_grpStart, FloatConv2Int8(FloatConv::FLOAT_WS));
  EEPROM.write(++u16_grpStart, 255);


  // New Group #5, +60
  EEPROM.write(++u16_grpStart, 3);               // number of values
  EEPROM.write(++u16_grpStart, 10);              // number of registers
  EEPROM.write(++u16_grpStart, highByte(7008));  // starting register
  EEPROM.write(++u16_grpStart, lowByte(7008));   // starting register
  EEPROM.write(++u16_grpStart, 9);              // skip bytes to data types
                                            // value types
  EEPROM.write(++u16_grpStart, 30);
  EEPROM.write(++u16_grpStart, int8_t(-4));
  EEPROM.write(++u16_grpStart, 31);
  EEPROM.write(++u16_grpStart, 32);
  // datatypes
  EEPROM.write(++u16_grpStart, FloatConv2Int8(FloatConv::FLOAT_WS));
  EEPROM.write(++u16_grpStart, 255);


  // New Group #6 (LAST), +71
  EEPROM.write(++u16_grpStart, 1); // number of values
                               // value types
  EEPROM.write(++u16_grpStart, 5);


  
  Serial.print(F(" to "));
  Serial.println(u16_grpStart, DEC);  // used to be +96

  return ++u16_grpStart;
}


