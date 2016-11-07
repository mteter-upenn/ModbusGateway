uint16_t meter14(uint16_t u16_mapStart) {
  uint16_t u16_blkStart;
  uint8_t u8_numBlks;
  uint16_t u16_grpStart;
  uint8_t u8_numGrps;

  // siemens pac 4200/3200 **************************************************************
  // 123

  Serial.print(F("meter 14: "));
  Serial.print(u16_mapStart, DEC);

  u8_numBlks = 7;
  u8_numGrps = 3;
  u16_blkStart = u16_mapStart + 4 + u8_numGrps * 2;
  // used to be 123, now it's 95
  u16_grpStart = u16_blkStart + (u8_numBlks * 5);

  EEPROM.write(u16_mapStart, highByte(u16_blkStart));
  EEPROM.write(u16_mapStart + 1, lowByte(u16_blkStart));
  EEPROM.write(u16_mapStart + 2, u8_numBlks);  // number of blocks;

  EEPROM.write(u16_mapStart + 3, u8_numGrps);  // number of blocks;
  EEPROM.write(u16_mapStart + 4, highByte(u16_grpStart));
  EEPROM.write(u16_mapStart + 5, lowByte(u16_grpStart));
  EEPROM.write(u16_mapStart + 6, highByte(u16_grpStart + 36));
  EEPROM.write(u16_mapStart + 7, lowByte(u16_grpStart + 36));
  EEPROM.write(u16_mapStart + 8, highByte(u16_grpStart + 48));
  EEPROM.write(u16_mapStart + 9, lowByte(u16_grpStart + 48));

  // Block #1 - [1, 201, 128]
  EEPROM.write(u16_blkStart, highByte(1));
  EEPROM.write(u16_blkStart + 1, lowByte(1));
  EEPROM.write(u16_blkStart + 2, highByte(201));
  EEPROM.write(u16_blkStart + 3, lowByte(201));
  EEPROM.write(u16_blkStart + 4, FloatConv2Int8(FloatConv::FLOAT_WS));

  // Block #2 - [203, 237, (3 | 128]
  EEPROM.write(u16_blkStart + 5, highByte(203));
  EEPROM.write(u16_blkStart + 6, lowByte(203));
  EEPROM.write(u16_blkStart + 7, highByte(238));
  EEPROM.write(u16_blkStart + 8, lowByte(238));
  EEPROM.write(u16_blkStart + 9, FloatConv2Int8(FloatConv::UINT32_WS));

  // Block #3 - [243, 367, 128]
  EEPROM.write(u16_blkStart + 10, highByte(243));
  EEPROM.write(u16_blkStart + 11, lowByte(243));
  EEPROM.write(u16_blkStart + 12, highByte(367));
  EEPROM.write(u16_blkStart + 13, lowByte(367));
  EEPROM.write(u16_blkStart + 14, FloatConv2Int8(FloatConv::FLOAT_WS));

  // Block #4 - [489, 515, 128]
  EEPROM.write(u16_blkStart + 15, highByte(489));
  EEPROM.write(u16_blkStart + 16, lowByte(489));
  EEPROM.write(u16_blkStart + 17, highByte(515));
  EEPROM.write(u16_blkStart + 18, lowByte(515));
  EEPROM.write(u16_blkStart + 19, FloatConv2Int8(FloatConv::FLOAT_WS));

  // Block #5 - [517, 521, 3 | 128]
  EEPROM.write(u16_blkStart + 20, highByte(517));
  EEPROM.write(u16_blkStart + 21, lowByte(517));
  EEPROM.write(u16_blkStart + 22, highByte(521));
  EEPROM.write(u16_blkStart + 23, lowByte(521));
  EEPROM.write(u16_blkStart + 24, FloatConv2Int8(FloatConv::UINT32_WS));

  // Block #6 - [523, 543, 128]
  EEPROM.write(u16_blkStart + 25, highByte(523));
  EEPROM.write(u16_blkStart + 26, lowByte(523));
  EEPROM.write(u16_blkStart + 27, highByte(543));
  EEPROM.write(u16_blkStart + 28, lowByte(543));
  EEPROM.write(u16_blkStart + 29, FloatConv2Int8(FloatConv::FLOAT_WS));

  // Block #7 - [801, 837, 128]
  EEPROM.write(u16_blkStart + 30, highByte(801));
  EEPROM.write(u16_blkStart + 31, lowByte(801));
  EEPROM.write(u16_blkStart + 32, highByte(837));
  EEPROM.write(u16_blkStart + 33, lowByte(837));
  EEPROM.write(u16_blkStart + 34, FloatConv2Int8(FloatConv::DOUBLE_WS));

  

  // New Group #1, +0
  EEPROM.write(u16_grpStart, 28);                 // number of values
  EEPROM.write(++u16_grpStart, 70);              // number of registers
  EEPROM.write(++u16_grpStart, highByte(1));  // starting register
  EEPROM.write(++u16_grpStart, lowByte(1));   // starting register
  EEPROM.write(++u16_grpStart, 34);              // skip bytes to data types
                                             // value types
  EEPROM.write(++u16_grpStart, 6);
  EEPROM.write(++u16_grpStart, 7);
  EEPROM.write(++u16_grpStart, 8);
  EEPROM.write(++u16_grpStart, 10);
  EEPROM.write(++u16_grpStart, 11);
  EEPROM.write(++u16_grpStart, 12);
  EEPROM.write(++u16_grpStart, 1);
  EEPROM.write(++u16_grpStart, 2);
  EEPROM.write(++u16_grpStart, 3);
  EEPROM.write(++u16_grpStart, 22);
  EEPROM.write(++u16_grpStart, 23);
  EEPROM.write(++u16_grpStart, 24);
  EEPROM.write(++u16_grpStart, 14);
  EEPROM.write(++u16_grpStart, 15);
  EEPROM.write(++u16_grpStart, 16);
  EEPROM.write(++u16_grpStart, 18);
  EEPROM.write(++u16_grpStart, 19);
  EEPROM.write(++u16_grpStart, 20);
  EEPROM.write(++u16_grpStart, 26);
  EEPROM.write(++u16_grpStart, 27);
  EEPROM.write(++u16_grpStart, 28);
  EEPROM.write(++u16_grpStart, int8_t(-14));
  EEPROM.write(++u16_grpStart, 9);
  EEPROM.write(++u16_grpStart, 13);
  EEPROM.write(++u16_grpStart, 4);
  EEPROM.write(++u16_grpStart, 25);
  EEPROM.write(++u16_grpStart, 17);
  EEPROM.write(++u16_grpStart, 21);
  EEPROM.write(++u16_grpStart, 29);
  // datatypes
  EEPROM.write(++u16_grpStart, FloatConv2Int8(FloatConv::FLOAT_WS));
  EEPROM.write(++u16_grpStart, 255);


  // New Group #2, +36
  EEPROM.write(++u16_grpStart, 3);               // number of values
  EEPROM.write(++u16_grpStart, 36);              // number of registers
  EEPROM.write(++u16_grpStart, highByte(801));  // starting register
  EEPROM.write(++u16_grpStart, lowByte(801));   // starting register
  EEPROM.write(++u16_grpStart, 10);              // skip bytes to data types
                                             // value types
  EEPROM.write(++u16_grpStart, 30);
  EEPROM.write(++u16_grpStart, int8_t(-12));
  EEPROM.write(++u16_grpStart, 31);
  EEPROM.write(++u16_grpStart, int8_t(-12));
  EEPROM.write(++u16_grpStart, 32);
  // datatypes
  EEPROM.write(++u16_grpStart, FloatConv2Int8(FloatConv::DOUBLE_WS));
  EEPROM.write(++u16_grpStart, 255);


  // New Group #3 (LAST), +48
  EEPROM.write(++u16_grpStart, 1); // number of values
                               // value types
  EEPROM.write(++u16_grpStart, 5);

  
  Serial.print(F(" to "));
  Serial.println(u16_grpStart, DEC);

  return ++u16_grpStart;
}

