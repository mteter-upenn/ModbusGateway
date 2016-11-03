void meter1(uint16_t mtr_start){
  uint16_t blk_strt;
  uint8_t blk_num;
  uint16_t grp_strt;
  uint8_t grp_num;
  
  // eaton power xpert 4000
    
 // 90+6+20=116
  blk_num = 4;
  grp_num = 3;
  blk_strt = mtr_start + 4 + grp_num * 2;
  // used to be 117, now it's 83
  grp_strt = blk_strt + (blk_num * 5);
  
  EEPROM.write(mtr_start, highByte(blk_strt));
  EEPROM.write(mtr_start + 1, lowByte(blk_strt));
  EEPROM.write(mtr_start + 2, blk_num);  // number of blocks;

  
  EEPROM.write(mtr_start + 3, grp_num);  // number of blocks;
  EEPROM.write(mtr_start + 4, highByte(grp_strt));  // group 1 start
  EEPROM.write(mtr_start + 5, lowByte(grp_strt));
  EEPROM.write(mtr_start + 6, highByte(grp_strt + 40));  // group 2 start
  EEPROM.write(mtr_start + 7, lowByte(grp_strt + 40));
  EEPROM.write(mtr_start + 8, highByte(grp_strt + 51));  // group 3 start
  EEPROM.write(mtr_start + 9, lowByte(grp_strt + 51));


  // Block #1 - [4610, 4638, 128]
  EEPROM.write(blk_strt, highByte(4610));
  EEPROM.write(blk_strt + 1, lowByte(4610));
  EEPROM.write(blk_strt + 2, highByte(4638));
  EEPROM.write(blk_strt + 3, lowByte(4638));
  EEPROM.write(blk_strt + 4, FloatConv2Int8(FloatConv::FLOAT_WS));

  // Block #2 - [4650, 4704, 128]
  EEPROM.write(blk_strt + 5, highByte(4650));
  EEPROM.write(blk_strt + 6, lowByte(4650));
  EEPROM.write(blk_strt + 7, highByte(4704));
  EEPROM.write(blk_strt + 8, lowByte(4704));
  EEPROM.write(blk_strt + 9, FloatConv2Uint8(FloatConv::FLOAT_WS));
  
  // Block #3 - [4818, 4832, 128]
  EEPROM.write(blk_strt + 10, highByte(4818));
  EEPROM.write(blk_strt + 11, lowByte(4818));
  EEPROM.write(blk_strt + 12, highByte(4832));
  EEPROM.write(blk_strt + 13, lowByte(4832));
  EEPROM.write(blk_strt + 14, FloatConv2Int8(FloatConv::FLOAT_WS));

  // Block #4 - [6304, 6328, 10 | 128]
  EEPROM.write(blk_strt + 15, highByte(6304));
  EEPROM.write(blk_strt + 16, lowByte(6304));
  EEPROM.write(blk_strt + 17, highByte(6328));
  EEPROM.write(blk_strt + 18, lowByte(6328));
  EEPROM.write(blk_strt + 19, FloatConv2Int8(FloatConv::ENERGY_WS));



  // New Group #1, +0
  EEPROM.write(grp_strt, 28);                 // number of values
  EEPROM.write(++grp_strt, 86);              // number of registers
  EEPROM.write(++grp_strt, highByte(4610));  // starting register
  EEPROM.write(++grp_strt, lowByte(4610));   // starting register
  EEPROM.write(++grp_strt, 38);              // skip bytes to data types
  // value types
  EEPROM.write(++grp_strt, 1);
  EEPROM.write(++grp_strt, 2);
  EEPROM.write(++grp_strt, 3);
  EEPROM.write(++grp_strt, int8_t(-4));
  EEPROM.write(++grp_strt, 4);
  EEPROM.write(++grp_strt, 10);
  EEPROM.write(++grp_strt, 11);
  EEPROM.write(++grp_strt, 12);
  EEPROM.write(++grp_strt, 13);
  EEPROM.write(++grp_strt, 6);
  EEPROM.write(++grp_strt, 7);
  EEPROM.write(++grp_strt, 8);
  EEPROM.write(++grp_strt, 9);
  EEPROM.write(++grp_strt, int8_t(-12));
  EEPROM.write(++grp_strt, 17);
  EEPROM.write(++grp_strt, 21);
  EEPROM.write(++grp_strt, 25);
  EEPROM.write(++grp_strt, int8_t(-2));
  EEPROM.write(++grp_strt, 29);
  EEPROM.write(++grp_strt, int8_t(-6));
  EEPROM.write(++grp_strt, 14);
  EEPROM.write(++grp_strt, 15);
  EEPROM.write(++grp_strt, 16);
  EEPROM.write(++grp_strt, 18);
  EEPROM.write(++grp_strt, 19);
  EEPROM.write(++grp_strt, 20);
  EEPROM.write(++grp_strt, 22);
  EEPROM.write(++grp_strt, 23);
  EEPROM.write(++grp_strt, 24);
  EEPROM.write(++grp_strt, int8_t(-6));
  EEPROM.write(++grp_strt, 26);
  EEPROM.write(++grp_strt, 27);
  EEPROM.write(++grp_strt, 28);
  // datatypes
  EEPROM.write(++grp_strt, FloatConv2Int8(FloatConv::FLOAT_WS));
  EEPROM.write(++grp_strt, 255);


  // New Group #2, +40
  EEPROM.write(++grp_strt, 3);               // number of values
  EEPROM.write(++grp_strt, 20);              // number of registers
  EEPROM.write(++grp_strt, highByte(6312));  // starting register
  EEPROM.write(++grp_strt, lowByte(6312));   // starting register
  EEPROM.write(++grp_strt, 9);              // skip bytes to data types
  // value types
  EEPROM.write(++grp_strt, 30);
  EEPROM.write(++grp_strt, int8_t(-8));
  EEPROM.write(++grp_strt, 31);
  EEPROM.write(++grp_strt, 32);
  // datatypes
  EEPROM.write(++grp_strt, FloatConv2Int8(FloatConv::ENERGY_WS));
  EEPROM.write(++grp_strt, 255);


  // New Group #3 (LAST), +51
  EEPROM.write(++grp_strt, 1); // number of values
  // value types
  EEPROM.write(++grp_strt, 5);

  Serial.print(F("meter 1: "));
  Serial.print(mtr_start, DEC);
  Serial.print(F(" to "));
  Serial.println(grp_strt, DEC);

  //// Group #1
  //EEPROM.write(grp_strt, 3);
  //EEPROM.write(grp_strt + 1, highByte(4610));
  //EEPROM.write(grp_strt + 2, lowByte(4610));
  //for (i = 1; i < 4; i++){
  //  EEPROM.write(i + grp_strt + 2, i);  // data type
  //}
  //EEPROM.write(grp_strt + 6, FloatConv2Int8(FloatConv::FLOAT_WS));

  //// Group #2
  //EEPROM.write(grp_strt + 9, 9);
  //EEPROM.write(grp_strt + 10, highByte(4620));
  //EEPROM.write(grp_strt + 11, lowByte(4620));
  //EEPROM.write(grp_strt + 12, 4);
  //for (i = 6; i < 10; i++){
  //  EEPROM.write(i + grp_strt + 7, i + 4);  // data type
  //  EEPROM.write(i + grp_strt + 11, i);  // data type
  //  EEPROM.write(i + grp_strt + 15, 12);
  //  EEPROM.write(i + grp_strt + 19, 12);
  //}
  //EEPROM.write(grp_strt + 29, FloatConv2Int8(FloatConv::FLOAT_WS));

  //// Group #3
  //EEPROM.write(grp_strt + 30, 3);
  //EEPROM.write(grp_strt + 31, highByte(4650));
  //EEPROM.write(grp_strt + 32, lowByte(4650));
  //EEPROM.write(grp_strt + 33, 17);
  //EEPROM.write(grp_strt + 34, 21);
  //EEPROM.write(grp_strt + 35, 25);
  //for (i = (grp_strt + 36); i < (grp_strt + 36 + 3); i++){
  //  EEPROM.write(i, FloatConv2Int8(FloatConv::FLOAT_WS));  // data type
  //}

  //// Group #4
  //EEPROM.write(grp_strt + 39, 1);
  //EEPROM.write(grp_strt + 40, highByte(4658));
  //EEPROM.write(grp_strt + 41, lowByte(4658));
  //EEPROM.write(grp_strt + 42, 29);
  //EEPROM.write(grp_strt + 43, FloatConv2Int8(FloatConv::FLOAT_WS));

  //// Group #5
  //EEPROM.write(grp_strt + 44, 9);
  //EEPROM.write(grp_strt + 45, highByte(4666));
  //EEPROM.write(grp_strt + 46, lowByte(4666));
  //for (i = 14; i < 17; i++){
  //  EEPROM.write(i + grp_strt + 33, i);  // data type
  //  EEPROM.write(i + grp_strt + 36, i + 4);  // data type
  //  EEPROM.write(i + grp_strt + 39, i + 8);  // data type
  //  EEPROM.write(i + grp_strt + 42, FloatConv2Int8(FloatConv::FLOAT_WS));
  //  EEPROM.write(i + grp_strt + 45, FloatConv2Int8(FloatConv::FLOAT_WS));
  //  EEPROM.write(i + grp_strt + 48, FloatConv2Int8(FloatConv::FLOAT_WS));
  //}
  //
  //// Group #6
  //EEPROM.write(grp_strt + 65, 3);
  //EEPROM.write(grp_strt + 66, highByte(4690));
  //EEPROM.write(grp_strt + 67, lowByte(4690));
  //for (i = 26; i < 29; i++){
  //  EEPROM.write(i + grp_strt + 42, i);  // data type
  //  EEPROM.write(i + grp_strt + 45, FloatConv2Int8(FloatConv::FLOAT_WS));
  //}

  //// Group #7
  //EEPROM.write(grp_strt + 74, 1);
  //EEPROM.write(grp_strt + 75, highByte(6312));
  //EEPROM.write(grp_strt + 76, lowByte(6312));
  //EEPROM.write(grp_strt + 77, 30);
  //EEPROM.write(grp_strt + 78, FloatConv2Int8(FloatConv::ENERGY_WS));

  //// Group #8
  //EEPROM.write(grp_strt + 79, 2);
  //EEPROM.write(grp_strt + 80, highByte(6324));
  //EEPROM.write(grp_strt + 81, lowByte(6324));
  //for (i = 31; i < 33; i++){
  //  EEPROM.write(i + grp_strt + 51, i);  // data type
  //  EEPROM.write(i + grp_strt + 53, FloatConv2Int8(FloatConv::ENERGY_WS));
  //}

  //// Group #9
  //EEPROM.write(grp_strt + 86, 1);
  //EEPROM.write(grp_strt + 87, 255);
  //EEPROM.write(grp_strt + 88, 255);
  //EEPROM.write(grp_strt + 89, 5);

  //Serial.print(F("group 1: "));
  //Serial.print(mtr_start, DEC);
  //Serial.print(F(" to "));
  //Serial.println(grp_strt + 90, DEC);
}
