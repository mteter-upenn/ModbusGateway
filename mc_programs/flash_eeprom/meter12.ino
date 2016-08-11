void meter12(uint16_t mtr_start){
  uint16_t blk_strt;
  uint8_t blk_num;
  uint16_t grp_strt;
  uint8_t grp_num;
  //uint16_t i;

  // steam kep *****************************************************
  // 
  
  blk_strt = mtr_start + 6; 
  blk_num = 5;
  grp_num = 6;

  grp_strt = blk_strt + (blk_num * 5);
  
  EEPROM.write(mtr_start, highByte(blk_strt));
  EEPROM.write(mtr_start + 1, lowByte(blk_strt));
  EEPROM.write(mtr_start + 2, blk_num);  // number of blocks;

  EEPROM.write(mtr_start + 3, highByte(grp_strt));
  EEPROM.write(mtr_start + 4, lowByte(grp_strt));
  EEPROM.write(mtr_start + 5, grp_num);  // number of blocks;

  // Block #1 - [0, 42, 0]
  EEPROM.write(blk_strt, highByte(0));
  EEPROM.write(blk_strt + 1, lowByte(0));
  EEPROM.write(blk_strt + 2, highByte(42));
  EEPROM.write(blk_strt + 3, lowByte(42));
  EEPROM.write(blk_strt + 4, int8_t(FloatConv::FLOAT));

  // Block #2 - [44, 49, 2]
  EEPROM.write(blk_strt + 5, highByte(44));
  EEPROM.write(blk_strt + 6, lowByte(44));
  EEPROM.write(blk_strt + 7, highByte(49));
  EEPROM.write(blk_strt + 8, lowByte(49));
  EEPROM.write(blk_strt + 9, int8_t(FloatConv::INT16));

  // Block #3 - [50, 58, 0]
  EEPROM.write(blk_strt + 10, highByte(50));
  EEPROM.write(blk_strt + 11, lowByte(50));
  EEPROM.write(blk_strt + 12, highByte(58));
  EEPROM.write(blk_strt + 13, lowByte(58));
  EEPROM.write(blk_strt + 14, int8_t(FloatConv::FLOAT));

  // Block #4 - [60, 61, 2]
  EEPROM.write(blk_strt + 15, highByte(60));
  EEPROM.write(blk_strt + 16, lowByte(60));
  EEPROM.write(blk_strt + 17, highByte(61));
  EEPROM.write(blk_strt + 18, lowByte(61));
  EEPROM.write(blk_strt + 19, int8_t(FloatConv::INT16));

  // Block #5 - [62, 62, 0]
  EEPROM.write(blk_strt + 20, highByte(62));
  EEPROM.write(blk_strt + 21, lowByte(62));
  EEPROM.write(blk_strt + 22, highByte(62));
  EEPROM.write(blk_strt + 23, lowByte(62));
  EEPROM.write(blk_strt + 24, int8_t(FloatConv::FLOAT));


  // Group #1
  EEPROM.write(grp_strt, 2);
  EEPROM.write(grp_strt + 1, highByte(0));
  EEPROM.write(grp_strt + 2, lowByte(0));
  EEPROM.write(grp_strt + 3, 1);
  EEPROM.write(grp_strt + 4, 2);
  EEPROM.write(grp_strt + 5, int8_t(FloatConv::FLOAT));
  EEPROM.write(grp_strt + 6, int8_t(FloatConv::FLOAT));

  // Group #2
  EEPROM.write(grp_strt + 7, 2);
  EEPROM.write(grp_strt + 8, highByte(6));
  EEPROM.write(grp_strt + 9, lowByte(6));
  EEPROM.write(grp_strt + 10, 3);
  EEPROM.write(grp_strt + 11, 4);
  EEPROM.write(grp_strt + 12, int8_t(FloatConv::FLOAT));
  EEPROM.write(grp_strt + 13, int8_t(FloatConv::FLOAT));

  // Group #3
  EEPROM.write(grp_strt + 14, 1);
  EEPROM.write(grp_strt + 15, highByte(14));
  EEPROM.write(grp_strt + 16, lowByte(14));
  EEPROM.write(grp_strt + 17, 7);
  EEPROM.write(grp_strt + 18, int8_t(FloatConv::FLOAT));

  // Group #4
  EEPROM.write(grp_strt + 19, 2);
  EEPROM.write(grp_strt + 20, highByte(22));
  EEPROM.write(grp_strt + 21, lowByte(22));
  EEPROM.write(grp_strt + 22, 8);
  EEPROM.write(grp_strt + 23, 9);
  EEPROM.write(grp_strt + 24, int8_t(FloatConv::FLOAT));
  EEPROM.write(grp_strt + 25, int8_t(FloatConv::FLOAT));

  // Group #5
  EEPROM.write(grp_strt + 26, 1);
  EEPROM.write(grp_strt + 27, highByte(28));
  EEPROM.write(grp_strt + 28, lowByte(28));
  EEPROM.write(grp_strt + 29, 10);
  EEPROM.write(grp_strt + 30, int8_t(FloatConv::FLOAT));

  // Group #6
  EEPROM.write(grp_strt + 31, 2);
  EEPROM.write(grp_strt + 32, 255);
  EEPROM.write(grp_strt + 33, 255);
  EEPROM.write(grp_strt + 34, 5);
  EEPROM.write(grp_strt + 35, 6);

  Serial.print(F("group 12: "));
  Serial.print(mtr_start, DEC);
  Serial.print(F(" to "));
  Serial.println(grp_strt + 37, DEC);
}
