void meter14(uint16_t mtr_start) {
  uint16_t blk_strt;
  uint8_t blk_num;
  uint16_t grp_strt;
  uint8_t grp_num;
  int16_t i;

  // siemens pac 4200/3200 **************************************************************
  // 123

  blk_strt = mtr_start + 6;
  blk_num = 7;
  grp_num = 6;
  // used to be 123, now it's 
  grp_strt = blk_strt + (blk_num * 5);

  EEPROM.write(mtr_start, highByte(blk_strt));
  EEPROM.write(mtr_start + 1, lowByte(blk_strt));
  EEPROM.write(mtr_start + 2, blk_num);  // number of blocks;

  EEPROM.write(mtr_start + 3, grp_num);  // number of blocks;
  EEPROM.write(mtr_start + 4, highByte(grp_strt));
  EEPROM.write(mtr_start + 5, lowByte(grp_strt));

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

  // Group #1
  EEPROM.write(grp_strt, 21);
  EEPROM.write(grp_strt + 1, highByte(1));
  EEPROM.write(grp_strt + 2, lowByte(1));
  for (i = 0; i < 3; i++) {
    EEPROM.write(i + grp_strt + 3, i + 6);  // data type
    EEPROM.write(i + grp_strt + 6, i + 10);  // data type
    EEPROM.write(i + grp_strt + 9, i + 1);  // data type
    EEPROM.write(i + grp_strt + 12, i + 22);  // data type
    EEPROM.write(i + grp_strt + 15, i + 14);  // data type
    EEPROM.write(i + grp_strt + 18, i + 18);  // data type
    EEPROM.write(i + grp_strt + 21, i + 26);  // data type
  }
  for (i = (grp_strt + 24); i < (grp_strt + 24 + 21); i++) {
    EEPROM.write(i, int8_t(FloatConv::FLOAT_WS));  // data type
  }

  // Group #2
  EEPROM.write(grp_strt + 45, 7);
  EEPROM.write(grp_strt + 46, highByte(57));
  EEPROM.write(grp_strt + 47, lowByte(57));
  EEPROM.write(grp_strt + 48, 9);
  EEPROM.write(grp_strt + 49, 13);
  EEPROM.write(grp_strt + 50, 4);
  EEPROM.write(grp_strt + 51, 25);
  EEPROM.write(grp_strt + 52, 17);
  EEPROM.write(grp_strt + 53, 21);
  EEPROM.write(grp_strt + 54, 29);
  for (i = 0; i < 7; i++) {
    EEPROM.write(i + grp_strt + 55, int8_t(FloatConv::FLOAT_WS));
  }

  // Group #3
  EEPROM.write(grp_strt + 62, 1);
  EEPROM.write(grp_strt + 63, highByte(801));
  EEPROM.write(grp_strt + 64, lowByte(801));
  EEPROM.write(grp_strt + 65, 30);
  EEPROM.write(grp_strt + 66, int8_t(FloatConv::DOUBLE_WS));

  // Group #4
  EEPROM.write(grp_strt + 67, 1);
  EEPROM.write(grp_strt + 68, highByte(817));
  EEPROM.write(grp_strt + 69, lowByte(817));
  EEPROM.write(grp_strt + 70, 31);
  EEPROM.write(grp_strt + 71, int8_t(FloatConv::DOUBLE_WS));

  // Group #5
  EEPROM.write(grp_strt + 72, 1);
  EEPROM.write(grp_strt + 73, highByte(833));
  EEPROM.write(grp_strt + 74, lowByte(833));
  EEPROM.write(grp_strt + 75, 32);
  EEPROM.write(grp_strt + 76, int8_t(FloatConv::DOUBLE_WS));

  // Group #6
  EEPROM.write(grp_strt + 77, 1);
  EEPROM.write(grp_strt + 78, 255);
  EEPROM.write(grp_strt + 79, 255);
  EEPROM.write(grp_strt + 80, 5);

  Serial.print(F("group 14: "));
  Serial.print(mtr_start, DEC);
  Serial.print(F(" to "));
  Serial.println(grp_strt + 81, DEC);
}

