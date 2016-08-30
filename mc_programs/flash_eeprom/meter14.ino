void meter14(uint16_t mtr_start) {
  uint16_t blk_strt;
  uint8_t blk_num;
  uint16_t grp_strt;
  uint8_t grp_num;

  // siemens pac 4200/3200 **************************************************************
  // 123

  Serial.print(F("meter 14: "));
  Serial.print(mtr_start, DEC);

  blk_num = 7;
  grp_num = 3;
  blk_strt = mtr_start + 4 + grp_num * 2;
  // used to be 123, now it's 95
  grp_strt = blk_strt + (blk_num * 5);

  EEPROM.write(mtr_start, highByte(blk_strt));
  EEPROM.write(mtr_start + 1, lowByte(blk_strt));
  EEPROM.write(mtr_start + 2, blk_num);  // number of blocks;

  EEPROM.write(mtr_start + 3, grp_num);  // number of blocks;
  EEPROM.write(mtr_start + 4, highByte(grp_strt));
  EEPROM.write(mtr_start + 5, lowByte(grp_strt));
  EEPROM.write(++mtr_start, highByte(grp_strt + 36));
  EEPROM.write(++mtr_start, lowByte(grp_strt + 36));
  EEPROM.write(++mtr_start, highByte(grp_strt + 48));
  EEPROM.write(++mtr_start, lowByte(grp_strt + 48));

  // Block #1 - [1, 201, 128]
  EEPROM.write(blk_strt, highByte(1));
  EEPROM.write(blk_strt + 1, lowByte(1));
  EEPROM.write(blk_strt + 2, highByte(201));
  EEPROM.write(blk_strt + 3, lowByte(201));
  EEPROM.write(blk_strt + 4, int8_t(FloatConv::FLOAT_WS));

  // Block #2 - [203, 237, (3 | 128]
  EEPROM.write(blk_strt + 5, highByte(203));
  EEPROM.write(blk_strt + 6, lowByte(203));
  EEPROM.write(blk_strt + 7, highByte(238));
  EEPROM.write(blk_strt + 8, lowByte(238));
  EEPROM.write(blk_strt + 9, int8_t(FloatConv::UINT32_WS));

  // Block #3 - [243, 367, 128]
  EEPROM.write(blk_strt + 10, highByte(243));
  EEPROM.write(blk_strt + 11, lowByte(243));
  EEPROM.write(blk_strt + 12, highByte(367));
  EEPROM.write(blk_strt + 13, lowByte(367));
  EEPROM.write(blk_strt + 14, int8_t(FloatConv::FLOAT_WS));

  // Block #4 - [489, 515, 128]
  EEPROM.write(blk_strt + 15, highByte(489));
  EEPROM.write(blk_strt + 16, lowByte(489));
  EEPROM.write(blk_strt + 17, highByte(515));
  EEPROM.write(blk_strt + 18, lowByte(515));
  EEPROM.write(blk_strt + 19, int8_t(FloatConv::FLOAT_WS));

  // Block #5 - [517, 521, 3 | 128]
  EEPROM.write(blk_strt + 20, highByte(517));
  EEPROM.write(blk_strt + 21, lowByte(517));
  EEPROM.write(blk_strt + 22, highByte(521));
  EEPROM.write(blk_strt + 23, lowByte(521));
  EEPROM.write(blk_strt + 24, int8_t(FloatConv::UINT32_WS));

  // Block #6 - [523, 543, 128]
  EEPROM.write(blk_strt + 25, highByte(523));
  EEPROM.write(blk_strt + 26, lowByte(523));
  EEPROM.write(blk_strt + 27, highByte(543));
  EEPROM.write(blk_strt + 28, lowByte(543));
  EEPROM.write(blk_strt + 29, int8_t(FloatConv::FLOAT_WS));

  // Block #7 - [801, 837, 128]
  EEPROM.write(blk_strt + 30, highByte(801));
  EEPROM.write(blk_strt + 31, lowByte(801));
  EEPROM.write(blk_strt + 32, highByte(837));
  EEPROM.write(blk_strt + 33, lowByte(837));
  EEPROM.write(blk_strt + 34, int8_t(FloatConv::DOUBLE_WS));

  

  // New Group #1, +0
  EEPROM.write(grp_strt, 28);                 // number of values
  EEPROM.write(++grp_strt, 70);              // number of registers
  EEPROM.write(++grp_strt, highByte(1));  // starting register
  EEPROM.write(++grp_strt, lowByte(1));   // starting register
  EEPROM.write(++grp_strt, 34);              // skip bytes to data types
                                             // value types
  EEPROM.write(++grp_strt, 6);
  EEPROM.write(++grp_strt, 7);
  EEPROM.write(++grp_strt, 8);
  EEPROM.write(++grp_strt, 10);
  EEPROM.write(++grp_strt, 11);
  EEPROM.write(++grp_strt, 12);
  EEPROM.write(++grp_strt, 1);
  EEPROM.write(++grp_strt, 2);
  EEPROM.write(++grp_strt, 3);
  EEPROM.write(++grp_strt, 22);
  EEPROM.write(++grp_strt, 23);
  EEPROM.write(++grp_strt, 24);
  EEPROM.write(++grp_strt, 14);
  EEPROM.write(++grp_strt, 15);
  EEPROM.write(++grp_strt, 16);
  EEPROM.write(++grp_strt, 18);
  EEPROM.write(++grp_strt, 19);
  EEPROM.write(++grp_strt, 20);
  EEPROM.write(++grp_strt, 26);
  EEPROM.write(++grp_strt, 27);
  EEPROM.write(++grp_strt, 28);
  EEPROM.write(++grp_strt, int8_t(-14));
  EEPROM.write(++grp_strt, 9);
  EEPROM.write(++grp_strt, 13);
  EEPROM.write(++grp_strt, 4);
  EEPROM.write(++grp_strt, 25);
  EEPROM.write(++grp_strt, 17);
  EEPROM.write(++grp_strt, 21);
  EEPROM.write(++grp_strt, 29);
  // datatypes
  EEPROM.write(++grp_strt, FloatConv2Int8(FloatConv::FLOAT_WS));
  EEPROM.write(++grp_strt, 255);


  // New Group #2, +36
  EEPROM.write(++grp_strt, 3);               // number of values
  EEPROM.write(++grp_strt, 36);              // number of registers
  EEPROM.write(++grp_strt, highByte(800));  // starting register
  EEPROM.write(++grp_strt, lowByte(800));   // starting register
  EEPROM.write(++grp_strt, 10);              // skip bytes to data types
                                             // value types
  EEPROM.write(++grp_strt, 30);
  EEPROM.write(++grp_strt, int8_t(-12));
  EEPROM.write(++grp_strt, 31);
  EEPROM.write(++grp_strt, int8_t(-12));
  EEPROM.write(++grp_strt, 32);
  // datatypes
  EEPROM.write(++grp_strt, FloatConv2Int8(FloatConv::DOUBLE_WS));
  EEPROM.write(++grp_strt, 255);


  // New Group #3 (LAST), +48
  EEPROM.write(++grp_strt, 1); // number of values
                               // value types
  EEPROM.write(++grp_strt, 5);

  
  Serial.print(F(" to "));
  Serial.println(grp_strt, DEC);
}

