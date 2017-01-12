#include "meters.h"

uint16_t meter7(uint16_t u16_mapStart){
  uint16_t u16_blkStart;
  uint8_t u8_numBlks;
  uint16_t u16_grpStart;
  uint8_t u8_numGrps;
  
  // squareD cm2350, meter 7 *************************************************************
  // 126
  
  Serial.print(F("meter 7: "));
  Serial.print(u16_mapStart, DEC);

  u8_numBlks = 7;
  u8_numGrps = 3;
  u16_blkStart = u16_mapStart + 4 + u8_numGrps * 2;
  // used to be 126, now it's 96
  u16_grpStart = u16_blkStart + (u8_numBlks * 5);
  
  EEPROM.write(u16_mapStart, highByte(u16_blkStart));
  EEPROM.write(u16_mapStart + 1, lowByte(u16_blkStart));
  EEPROM.write(u16_mapStart + 2, u8_numBlks);  // number of blocks;

  EEPROM.write(u16_mapStart + 3, u8_numGrps);  // number of blocks;
  EEPROM.write(u16_mapStart + 4, highByte(u16_grpStart));
  EEPROM.write(u16_mapStart + 5, lowByte(u16_grpStart));
  EEPROM.write(u16_mapStart + 6, highByte(u16_grpStart + 39));
  EEPROM.write(u16_mapStart + 7, lowByte(u16_grpStart + 39));
  EEPROM.write(u16_mapStart + 8, highByte(u16_grpStart + 49));
  EEPROM.write(u16_mapStart + 9, lowByte(u16_grpStart + 49));


  // Block #1 - [999, 1116, 2]
  EEPROM.write(u16_blkStart, highByte(999));
  EEPROM.write(u16_blkStart + 1, lowByte(999));
  EEPROM.write(u16_blkStart + 2, highByte(1116));
  EEPROM.write(u16_blkStart + 3, lowByte(1116));
  EEPROM.write(u16_blkStart + 4, FloatConv2Int8(FloatConv::INT16));

  // Block #2 - [1199, 1272, 2]
  EEPROM.write(u16_blkStart + 5, highByte(1199));
  EEPROM.write(u16_blkStart + 6, lowByte(1199));
  EEPROM.write(u16_blkStart + 7, highByte(1272));
  EEPROM.write(u16_blkStart + 8, lowByte(1272));
  EEPROM.write(u16_blkStart + 9, FloatConv2Int8(FloatConv::INT16));
  
  // Block #3 - [1399, 1472, 2]
  EEPROM.write(u16_blkStart + 10, highByte(1399));
  EEPROM.write(u16_blkStart + 11, lowByte(1399));
  EEPROM.write(u16_blkStart + 12, highByte(1472));
  EEPROM.write(u16_blkStart + 13, lowByte(1472));
  EEPROM.write(u16_blkStart + 14, FloatConv2Int8(FloatConv::INT16));

  // Block #4 - [1600, 1644, 8]
  EEPROM.write(u16_blkStart + 15, highByte(1600));
  EEPROM.write(u16_blkStart + 16, lowByte(1600));
  EEPROM.write(u16_blkStart + 17, highByte(1644));
  EEPROM.write(u16_blkStart + 18, lowByte(1644));
  EEPROM.write(u16_blkStart + 19, FloatConv2Int8(FloatConv::MOD30K));

  // Block #5 - [1648, 1660, 7]
  EEPROM.write(u16_blkStart + 20, highByte(1648));
  EEPROM.write(u16_blkStart + 21, lowByte(1648));
  EEPROM.write(u16_blkStart + 22, highByte(1660));
  EEPROM.write(u16_blkStart + 23, lowByte(1660));
  EEPROM.write(u16_blkStart + 24, FloatConv2Int8(FloatConv::MOD20K));

  // Block #6 - [1699, 1717, 2]
  EEPROM.write(u16_blkStart + 25, highByte(1699));
  EEPROM.write(u16_blkStart + 26, lowByte(1699));
  EEPROM.write(u16_blkStart + 27, highByte(1717));
  EEPROM.write(u16_blkStart + 28, lowByte(1717));
  EEPROM.write(u16_blkStart + 29, FloatConv2Int8(FloatConv::INT16));

  // Block #7 - [1729, 1751, 2]
  EEPROM.write(u16_blkStart + 30, highByte(1729));
  EEPROM.write(u16_blkStart + 31, lowByte(1729));
  EEPROM.write(u16_blkStart + 32, highByte(1751));
  EEPROM.write(u16_blkStart + 33, lowByte(1751));
  EEPROM.write(u16_blkStart + 34, FloatConv2Int8(FloatConv::INT16));
  

  
  // New Group #1, +0
  EEPROM.write(u16_grpStart, 28);                 // number of values
  EEPROM.write(++u16_grpStart, 48);              // number of registers
  EEPROM.write(++u16_grpStart, highByte(1002));  // starting register
  EEPROM.write(++u16_grpStart, lowByte(1002));   // starting register
  EEPROM.write(++u16_grpStart, 37);              // skip bytes to data types
                                             // value types
  EEPROM.write(++u16_grpStart, 1);
  EEPROM.write(++u16_grpStart, 2);
  EEPROM.write(++u16_grpStart, 3);
  EEPROM.write(++u16_grpStart, int8_t(-2));
  EEPROM.write(++u16_grpStart, 4);
  EEPROM.write(++u16_grpStart, int8_t(-5));
  EEPROM.write(++u16_grpStart, 10);
  EEPROM.write(++u16_grpStart, 11);
  EEPROM.write(++u16_grpStart, 12);
  EEPROM.write(++u16_grpStart, 13);
  EEPROM.write(++u16_grpStart, 6);
  EEPROM.write(++u16_grpStart, 7);
  EEPROM.write(++u16_grpStart, 8);
  EEPROM.write(++u16_grpStart, 9);
  EEPROM.write(++u16_grpStart, int8_t(-9));
  EEPROM.write(++u16_grpStart, 26);
  EEPROM.write(++u16_grpStart, 27);
  EEPROM.write(++u16_grpStart, 28);
  EEPROM.write(++u16_grpStart, 29);
  EEPROM.write(++u16_grpStart, int8_t(-4));
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
  // datatypes
  EEPROM.write(++u16_grpStart, FloatConv2Int8(FloatConv::INT16));
  EEPROM.write(++u16_grpStart, 255);


  // New Group #2, +39
  EEPROM.write(++u16_grpStart, 3);               // number of values
  EEPROM.write(++u16_grpStart, 12);              // number of registers
  EEPROM.write(++u16_grpStart, highByte(1616));  // starting register
  EEPROM.write(++u16_grpStart, lowByte(1616));   // starting register
  EEPROM.write(++u16_grpStart, 8);              // skip bytes to data types
                                            // value types
  EEPROM.write(++u16_grpStart, 32);
  EEPROM.write(++u16_grpStart, 30);
  EEPROM.write(++u16_grpStart, 31);
  // datatypes
  EEPROM.write(++u16_grpStart, FloatConv2Int8(FloatConv::MOD30K_WS));
  EEPROM.write(++u16_grpStart, 255);


  // New Group #3 (LAST), +49
  EEPROM.write(++u16_grpStart, 1); // number of values
                               // value types
  EEPROM.write(++u16_grpStart, 5);

  
  Serial.print(F(" to "));
  Serial.println(u16_grpStart, DEC);

  return ++u16_grpStart;
}

