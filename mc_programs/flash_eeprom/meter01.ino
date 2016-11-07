uint16_t meter1(uint16_t u16_mapStart){
  uint16_t u16_blkStart;
  uint8_t u8_numBlks;
  uint16_t u16_grpStart;
  uint8_t u8_numGrps;
  
  // eaton power xpert 4000
    
 // 90+6+20=116
  u8_numBlks = 4;
  u8_numGrps = 3;
  u16_blkStart = u16_mapStart + 4 + u8_numGrps * 2;
  // used to be 117, now it's 83
  u16_grpStart = u16_blkStart + (u8_numBlks * 5);
  
  EEPROM.write(u16_mapStart, highByte(u16_blkStart));
  EEPROM.write(u16_mapStart + 1, lowByte(u16_blkStart));
  EEPROM.write(u16_mapStart + 2, u8_numBlks);  // number of blocks;

  
  EEPROM.write(u16_mapStart + 3, u8_numGrps);  // number of blocks;
  EEPROM.write(u16_mapStart + 4, highByte(u16_grpStart));  // group 1 start
  EEPROM.write(u16_mapStart + 5, lowByte(u16_grpStart));
  EEPROM.write(u16_mapStart + 6, highByte(u16_grpStart + 40));  // group 2 start
  EEPROM.write(u16_mapStart + 7, lowByte(u16_grpStart + 40));
  EEPROM.write(u16_mapStart + 8, highByte(u16_grpStart + 51));  // group 3 start
  EEPROM.write(u16_mapStart + 9, lowByte(u16_grpStart + 51));


  // Block #1 - [4610, 4638, 128]
  EEPROM.write(u16_blkStart, highByte(4610));
  EEPROM.write(u16_blkStart + 1, lowByte(4610));
  EEPROM.write(u16_blkStart + 2, highByte(4638));
  EEPROM.write(u16_blkStart + 3, lowByte(4638));
  EEPROM.write(u16_blkStart + 4, FloatConv2Int8(FloatConv::FLOAT_WS));

  // Block #2 - [4650, 4704, 128]
  EEPROM.write(u16_blkStart + 5, highByte(4650));
  EEPROM.write(u16_blkStart + 6, lowByte(4650));
  EEPROM.write(u16_blkStart + 7, highByte(4704));
  EEPROM.write(u16_blkStart + 8, lowByte(4704));
  EEPROM.write(u16_blkStart + 9, FloatConv2Uint8(FloatConv::FLOAT_WS));
  
  // Block #3 - [4818, 4832, 128]
  EEPROM.write(u16_blkStart + 10, highByte(4818));
  EEPROM.write(u16_blkStart + 11, lowByte(4818));
  EEPROM.write(u16_blkStart + 12, highByte(4832));
  EEPROM.write(u16_blkStart + 13, lowByte(4832));
  EEPROM.write(u16_blkStart + 14, FloatConv2Int8(FloatConv::FLOAT_WS));

  // Block #4 - [6304, 6328, 10 | 128]
  EEPROM.write(u16_blkStart + 15, highByte(6304));
  EEPROM.write(u16_blkStart + 16, lowByte(6304));
  EEPROM.write(u16_blkStart + 17, highByte(6328));
  EEPROM.write(u16_blkStart + 18, lowByte(6328));
  EEPROM.write(u16_blkStart + 19, FloatConv2Int8(FloatConv::ENERGY_WS));



  // New Group #1, +0
  EEPROM.write(u16_grpStart, 28);                 // number of values
  EEPROM.write(++u16_grpStart, 86);              // number of registers
  EEPROM.write(++u16_grpStart, highByte(4610));  // starting register
  EEPROM.write(++u16_grpStart, lowByte(4610));   // starting register
  EEPROM.write(++u16_grpStart, 38);              // skip bytes to data types
  // value types
  EEPROM.write(++u16_grpStart, 1);
  EEPROM.write(++u16_grpStart, 2);
  EEPROM.write(++u16_grpStart, 3);
  EEPROM.write(++u16_grpStart, int8_t(-4));
  EEPROM.write(++u16_grpStart, 4);
  EEPROM.write(++u16_grpStart, 10);
  EEPROM.write(++u16_grpStart, 11);
  EEPROM.write(++u16_grpStart, 12);
  EEPROM.write(++u16_grpStart, 13);
  EEPROM.write(++u16_grpStart, 6);
  EEPROM.write(++u16_grpStart, 7);
  EEPROM.write(++u16_grpStart, 8);
  EEPROM.write(++u16_grpStart, 9);
  EEPROM.write(++u16_grpStart, int8_t(-12));
  EEPROM.write(++u16_grpStart, 17);
  EEPROM.write(++u16_grpStart, 21);
  EEPROM.write(++u16_grpStart, 25);
  EEPROM.write(++u16_grpStart, int8_t(-2));
  EEPROM.write(++u16_grpStart, 29);
  EEPROM.write(++u16_grpStart, int8_t(-6));
  EEPROM.write(++u16_grpStart, 14);
  EEPROM.write(++u16_grpStart, 15);
  EEPROM.write(++u16_grpStart, 16);
  EEPROM.write(++u16_grpStart, 18);
  EEPROM.write(++u16_grpStart, 19);
  EEPROM.write(++u16_grpStart, 20);
  EEPROM.write(++u16_grpStart, 22);
  EEPROM.write(++u16_grpStart, 23);
  EEPROM.write(++u16_grpStart, 24);
  EEPROM.write(++u16_grpStart, int8_t(-6));
  EEPROM.write(++u16_grpStart, 26);
  EEPROM.write(++u16_grpStart, 27);
  EEPROM.write(++u16_grpStart, 28);
  // datatypes
  EEPROM.write(++u16_grpStart, FloatConv2Int8(FloatConv::FLOAT_WS));
  EEPROM.write(++u16_grpStart, 255);


  // New Group #2, +40
  EEPROM.write(++u16_grpStart, 3);               // number of values
  EEPROM.write(++u16_grpStart, 20);              // number of registers
  EEPROM.write(++u16_grpStart, highByte(6312));  // starting register
  EEPROM.write(++u16_grpStart, lowByte(6312));   // starting register
  EEPROM.write(++u16_grpStart, 9);              // skip bytes to data types
  // value types
  EEPROM.write(++u16_grpStart, 30);
  EEPROM.write(++u16_grpStart, int8_t(-8));
  EEPROM.write(++u16_grpStart, 31);
  EEPROM.write(++u16_grpStart, 32);
  // datatypes
  EEPROM.write(++u16_grpStart, FloatConv2Int8(FloatConv::ENERGY_WS));
  EEPROM.write(++u16_grpStart, 255);


  // New Group #3 (LAST), +51
  EEPROM.write(++u16_grpStart, 1); // number of values
  // value types
  EEPROM.write(++u16_grpStart, 5);

  Serial.print(F("meter 1: "));
  Serial.print(u16_mapStart, DEC);
  Serial.print(F(" to "));
  Serial.println(u16_grpStart, DEC);

  return ++u16_grpStart;
  //// Group #1
  //EEPROM.write(u16_grpStart, 3);
  //EEPROM.write(u16_grpStart + 1, highByte(4610));
  //EEPROM.write(u16_grpStart + 2, lowByte(4610));
  //for (i = 1; i < 4; i++){
  //  EEPROM.write(i + u16_grpStart + 2, i);  // data type
  //}
  //EEPROM.write(u16_grpStart + 6, FloatConv2Int8(FloatConv::FLOAT_WS));

  //// Group #2
  //EEPROM.write(u16_grpStart + 9, 9);
  //EEPROM.write(u16_grpStart + 10, highByte(4620));
  //EEPROM.write(u16_grpStart + 11, lowByte(4620));
  //EEPROM.write(u16_grpStart + 12, 4);
  //for (i = 6; i < 10; i++){
  //  EEPROM.write(i + u16_grpStart + 7, i + 4);  // data type
  //  EEPROM.write(i + u16_grpStart + 11, i);  // data type
  //  EEPROM.write(i + u16_grpStart + 15, 12);
  //  EEPROM.write(i + u16_grpStart + 19, 12);
  //}
  //EEPROM.write(u16_grpStart + 29, FloatConv2Int8(FloatConv::FLOAT_WS));

  //// Group #3
  //EEPROM.write(u16_grpStart + 30, 3);
  //EEPROM.write(u16_grpStart + 31, highByte(4650));
  //EEPROM.write(u16_grpStart + 32, lowByte(4650));
  //EEPROM.write(u16_grpStart + 33, 17);
  //EEPROM.write(u16_grpStart + 34, 21);
  //EEPROM.write(u16_grpStart + 35, 25);
  //for (i = (u16_grpStart + 36); i < (u16_grpStart + 36 + 3); i++){
  //  EEPROM.write(i, FloatConv2Int8(FloatConv::FLOAT_WS));  // data type
  //}

  //// Group #4
  //EEPROM.write(u16_grpStart + 39, 1);
  //EEPROM.write(u16_grpStart + 40, highByte(4658));
  //EEPROM.write(u16_grpStart + 41, lowByte(4658));
  //EEPROM.write(u16_grpStart + 42, 29);
  //EEPROM.write(u16_grpStart + 43, FloatConv2Int8(FloatConv::FLOAT_WS));

  //// Group #5
  //EEPROM.write(u16_grpStart + 44, 9);
  //EEPROM.write(u16_grpStart + 45, highByte(4666));
  //EEPROM.write(u16_grpStart + 46, lowByte(4666));
  //for (i = 14; i < 17; i++){
  //  EEPROM.write(i + u16_grpStart + 33, i);  // data type
  //  EEPROM.write(i + u16_grpStart + 36, i + 4);  // data type
  //  EEPROM.write(i + u16_grpStart + 39, i + 8);  // data type
  //  EEPROM.write(i + u16_grpStart + 42, FloatConv2Int8(FloatConv::FLOAT_WS));
  //  EEPROM.write(i + u16_grpStart + 45, FloatConv2Int8(FloatConv::FLOAT_WS));
  //  EEPROM.write(i + u16_grpStart + 48, FloatConv2Int8(FloatConv::FLOAT_WS));
  //}
  //
  //// Group #6
  //EEPROM.write(u16_grpStart + 65, 3);
  //EEPROM.write(u16_grpStart + 66, highByte(4690));
  //EEPROM.write(u16_grpStart + 67, lowByte(4690));
  //for (i = 26; i < 29; i++){
  //  EEPROM.write(i + u16_grpStart + 42, i);  // data type
  //  EEPROM.write(i + u16_grpStart + 45, FloatConv2Int8(FloatConv::FLOAT_WS));
  //}

  //// Group #7
  //EEPROM.write(u16_grpStart + 74, 1);
  //EEPROM.write(u16_grpStart + 75, highByte(6312));
  //EEPROM.write(u16_grpStart + 76, lowByte(6312));
  //EEPROM.write(u16_grpStart + 77, 30);
  //EEPROM.write(u16_grpStart + 78, FloatConv2Int8(FloatConv::ENERGY_WS));

  //// Group #8
  //EEPROM.write(u16_grpStart + 79, 2);
  //EEPROM.write(u16_grpStart + 80, highByte(6324));
  //EEPROM.write(u16_grpStart + 81, lowByte(6324));
  //for (i = 31; i < 33; i++){
  //  EEPROM.write(i + u16_grpStart + 51, i);  // data type
  //  EEPROM.write(i + u16_grpStart + 53, FloatConv2Int8(FloatConv::ENERGY_WS));
  //}

  //// Group #9
  //EEPROM.write(u16_grpStart + 86, 1);
  //EEPROM.write(u16_grpStart + 87, 255);
  //EEPROM.write(u16_grpStart + 88, 255);
  //EEPROM.write(u16_grpStart + 89, 5);

  //Serial.print(F("group 1: "));
  //Serial.print(u16_mapStart, DEC);
  //Serial.print(F(" to "));
  //Serial.println(u16_grpStart + 90, DEC);
}
