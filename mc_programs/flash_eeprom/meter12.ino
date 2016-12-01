uint16_t meter12(uint16_t u16_mapStart){
  uint16_t u16_blkStart;
  uint8_t u8_numBlks;
  uint16_t u16_grpStart;
  uint8_t u8_numGrps;
  //uint16_t i;

  // steam kep *****************************************************
  // 
  
  Serial.print(F("meter 12: "));
  Serial.print(u16_mapStart, DEC);

  u8_numBlks = 5;
  u8_numGrps = 2;
  u16_blkStart = u16_mapStart + 4 + u8_numGrps * 2;
  // used to be 69, now it's 55
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



  // New Group #1, +0
  EEPROM.write(u16_grpStart, 8);                 // number of values
  EEPROM.write(++u16_grpStart, 30);              // number of registers
  EEPROM.write(++u16_grpStart, highByte(0));  // starting register
  EEPROM.write(++u16_grpStart, lowByte(0));   // starting register
  EEPROM.write(++u16_grpStart, 17);              // skip bytes to data types
  // value types
  EEPROM.write(++u16_grpStart, 1);
  EEPROM.write(++u16_grpStart, 2);
  EEPROM.write(++u16_grpStart, int8_t(-2));
  EEPROM.write(++u16_grpStart, 3);
  EEPROM.write(++u16_grpStart, 4);
  EEPROM.write(++u16_grpStart, int8_t(-4));
  EEPROM.write(++u16_grpStart, 7);
  EEPROM.write(++u16_grpStart, int8_t(-14));
  EEPROM.write(++u16_grpStart, 8);
  EEPROM.write(++u16_grpStart, 9);
  EEPROM.write(++u16_grpStart, int8_t(-2));
  EEPROM.write(++u16_grpStart, 10);
  // data types
  EEPROM.write(++u16_grpStart, FloatConv2Int8(FloatConv::FLOAT));
  EEPROM.write(++u16_grpStart, 255);


  // New Group #2 (LAST), +19
  EEPROM.write(++u16_grpStart, 2); // number of values
  // value types
  EEPROM.write(++u16_grpStart, 5);
  EEPROM.write(++u16_grpStart, 6);

  //// Group #1
  //EEPROM.write(u16_grpStart, 2);
  //EEPROM.write(u16_grpStart + 1, highByte(0));
  //EEPROM.write(u16_grpStart + 2, lowByte(0));
  //EEPROM.write(u16_grpStart + 3, 1);
  //EEPROM.write(u16_grpStart + 4, 2);
  //EEPROM.write(u16_grpStart + 5, FloatConv2Int8(FloatConv::FLOAT));
  //EEPROM.write(u16_grpStart + 6, FloatConv2Int8(FloatConv::FLOAT));

  //// Group #2
  //EEPROM.write(u16_grpStart + 7, 2);
  //EEPROM.write(u16_grpStart + 8, highByte(6));
  //EEPROM.write(u16_grpStart + 9, lowByte(6));
  //EEPROM.write(u16_grpStart + 10, 3);
  //EEPROM.write(u16_grpStart + 11, 4);
  //EEPROM.write(u16_grpStart + 12, FloatConv2Int8(FloatConv::FLOAT));
  //EEPROM.write(u16_grpStart + 13, FloatConv2Int8(FloatConv::FLOAT));

  //// Group #3
  //EEPROM.write(u16_grpStart + 14, 1);
  //EEPROM.write(u16_grpStart + 15, highByte(14));
  //EEPROM.write(u16_grpStart + 16, lowByte(14));
  //EEPROM.write(u16_grpStart + 17, 7);
  //EEPROM.write(u16_grpStart + 18, FloatConv2Int8(FloatConv::FLOAT));

  //// Group #4
  //EEPROM.write(u16_grpStart + 19, 2);
  //EEPROM.write(u16_grpStart + 20, highByte(22));
  //EEPROM.write(u16_grpStart + 21, lowByte(22));
  //EEPROM.write(u16_grpStart + 22, 8);
  //EEPROM.write(u16_grpStart + 23, 9);
  //EEPROM.write(u16_grpStart + 24, FloatConv2Int8(FloatConv::FLOAT));
  //EEPROM.write(u16_grpStart + 25, FloatConv2Int8(FloatConv::FLOAT));

  //// Group #5
  //EEPROM.write(u16_grpStart + 26, 1);
  //EEPROM.write(u16_grpStart + 27, highByte(28));
  //EEPROM.write(u16_grpStart + 28, lowByte(28));
  //EEPROM.write(u16_grpStart + 29, 10);
  //EEPROM.write(u16_grpStart + 30, FloatConv2Int8(FloatConv::FLOAT));

  //// Group #6
  //EEPROM.write(u16_grpStart + 31, 2);
  //EEPROM.write(u16_grpStart + 32, 255);
  //EEPROM.write(u16_grpStart + 33, 255);
  //EEPROM.write(u16_grpStart + 34, 5);
  //EEPROM.write(u16_grpStart + 35, 6);

  
  Serial.print(F(" to "));
  Serial.println(u16_grpStart, DEC);

  return ++u16_grpStart;
}
