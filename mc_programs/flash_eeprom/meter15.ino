void meter15(uint16_t mtr_start) {
  uint16_t blk_strt;
  uint8_t blk_num;
  uint16_t grp_strt;
  uint8_t grp_num;
  int16_t i;

  // eaton series pxm 2000 **************************************************************
  // 108

  blk_strt = mtr_start + 6;
  blk_num = 1;
  grp_num = 11;
  // used to be 98, now it's 
  grp_strt = blk_strt + (blk_num * 5);

  EEPROM.write(mtr_start, highByte(blk_strt));
  EEPROM.write(mtr_start + 1, lowByte(blk_strt));
  EEPROM.write(mtr_start + 2, blk_num);  // number of blocks;

  EEPROM.write(mtr_start + 3, grp_num);  // number of blocks;
  EEPROM.write(mtr_start + 4, highByte(grp_strt));
  EEPROM.write(mtr_start + 5, lowByte(grp_strt));

                                         // Block #1 - [1, 201, 128]
  EEPROM.write(blk_strt, highByte(4002));
  EEPROM.write(blk_strt + 1, lowByte(4002));
  EEPROM.write(blk_strt + 2, highByte(7034));
  EEPROM.write(blk_strt + 3, lowByte(7034));
  EEPROM.write(blk_strt + 4, int8_t(FloatConv::FLOAT_WS));


  // Group #1
  EEPROM.write(grp_strt, 8);
  EEPROM.write(grp_strt + 1, highByte(4002));
  EEPROM.write(grp_strt + 2, lowByte(4002));
  for (i = 0; i < 4; i++) {
    EEPROM.write(i + grp_strt + 3, i + 10);  // data type
    EEPROM.write(i + grp_strt + 7, i + 6);  // data type
    EEPROM.write(i + grp_strt + 11, int8_t(FloatConv::FLOAT_WS));  // data type
    EEPROM.write(i + grp_strt + 15, int8_t(FloatConv::FLOAT_WS));  // data type
  }

  // Group #2
  EEPROM.write(grp_strt + 19, 3);
  EEPROM.write(grp_strt + 20, highByte(5002));
  EEPROM.write(grp_strt + 21, lowByte(5002));
  for (i = 0; i < 3; i++) {
    EEPROM.write(i + grp_strt + 22, i + 1);  // data type
    EEPROM.write(i + grp_strt + 25, int8_t(FloatConv::FLOAT_WS));  // data type
  }

  // Group #3
  EEPROM.write(grp_strt + 28, 1);
  EEPROM.write(grp_strt + 29, highByte(5012));
  EEPROM.write(grp_strt + 30, lowByte(5012));
  EEPROM.write(grp_strt + 31, 4);
  EEPROM.write(grp_strt + 32, int8_t(FloatConv::FLOAT_WS));

  // Group #4
  EEPROM.write(grp_strt + 33, 4);
  EEPROM.write(grp_strt + 34, highByte(6000));
  EEPROM.write(grp_strt + 35, lowByte(6000));
  for (i = 0; i < 4; i++) {
    EEPROM.write(i + grp_strt + 36, i + 14);  // data type
    EEPROM.write(i + grp_strt + 40, int8_t(FloatConv::FLOAT_WS));  // data type
  }

  // Group 5
  EEPROM.write(grp_strt + 44, 4);
  EEPROM.write(grp_strt + 45, highByte(6064));
  EEPROM.write(grp_strt + 46, lowByte(6064));
  for (i = 0; i < 4; i++) {
    EEPROM.write(i + grp_strt + 47, i + 22);  // data type
    EEPROM.write(i + grp_strt + 51, int8_t(FloatConv::FLOAT_WS));  // data type
  }

  // Group #6
  EEPROM.write(grp_strt + 55, 4);
  EEPROM.write(grp_strt + 56, highByte(6128));
  EEPROM.write(grp_strt + 57, lowByte(6128));
  for (i = 0; i < 4; i++) {
    EEPROM.write(i + grp_strt + 58, i + 18);  // data type
    EEPROM.write(i + grp_strt + 62, int8_t(FloatConv::FLOAT_WS));  // data type
  };

  // Group #7
  EEPROM.write(grp_strt + 66, 3);
  EEPROM.write(grp_strt + 67, highByte(6212));
  EEPROM.write(grp_strt + 68, lowByte(6212));
  for (i = 0; i < 3; i++) {
    EEPROM.write(i + grp_strt + 69, i + 26);  // data type
    EEPROM.write(i + grp_strt + 72, int8_t(FloatConv::FLOAT_WS));  // data type
  }

  // Group #8
  EEPROM.write(grp_strt + 75, 1);
  EEPROM.write(grp_strt + 76, highByte(6220));
  EEPROM.write(grp_strt + 77, lowByte(6220));
  EEPROM.write(grp_strt + 78, 29);
  EEPROM.write(grp_strt + 79, int8_t(FloatConv::FLOAT_WS));

  // Group #9
  EEPROM.write(grp_strt + 80, 1);
  EEPROM.write(grp_strt + 81, highByte(7008));
  EEPROM.write(grp_strt + 82, lowByte(7008));
  EEPROM.write(grp_strt + 83, 30);
  EEPROM.write(grp_strt + 84, int8_t(FloatConv::FLOAT_WS));

  // Group #10
  EEPROM.write(grp_strt + 85, 2);
  EEPROM.write(grp_strt + 86, highByte(7014));
  EEPROM.write(grp_strt + 87, lowByte(7014));
  EEPROM.write(grp_strt + 88, 31);
  EEPROM.write(grp_strt + 89, 32);
  EEPROM.write(grp_strt + 90, int8_t(FloatConv::FLOAT_WS));
  EEPROM.write(grp_strt + 91, int8_t(FloatConv::FLOAT_WS));

  // Group #11
  EEPROM.write(grp_strt + 92, 1);
  EEPROM.write(grp_strt + 93, 255);
  EEPROM.write(grp_strt + 94, 255);
  EEPROM.write(grp_strt + 95, 5);

  Serial.print(F("group 15: "));
  Serial.print(mtr_start, DEC);
  Serial.print(F(" to "));
  Serial.println(grp_strt + 96, DEC);
}


