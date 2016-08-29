void meter7(uint16_t mtr_start){
  uint16_t blk_strt;
  uint8_t blk_num;
  uint16_t grp_strt;
  uint8_t grp_num;
  uint16_t i;
  
  // squareD cm2350, meter 7 *************************************************************
  // 126
  
  blk_strt = mtr_start + 6;
  blk_num = 7;
  grp_num = 7;
  // used to be 126, now it's 
  grp_strt = blk_strt + (blk_num * 5);
  
  EEPROM.write(mtr_start, highByte(blk_strt));
  EEPROM.write(mtr_start + 1, lowByte(blk_strt));
  EEPROM.write(mtr_start + 2, blk_num);  // number of blocks;

  EEPROM.write(mtr_start + 3, grp_num);  // number of blocks;
  EEPROM.write(mtr_start + 4, highByte(grp_strt));
  EEPROM.write(mtr_start + 5, lowByte(grp_strt));


  // Block #1 - [999, 1116, 2]
  EEPROM.write(blk_strt, highByte(999));
  EEPROM.write(blk_strt + 1, lowByte(999));
  EEPROM.write(blk_strt + 2, highByte(1116));
  EEPROM.write(blk_strt + 3, lowByte(1116));
  EEPROM.write(blk_strt + 4, int8_t(FloatConv::INT16));

  // Block #2 - [1199, 1272, 2]
  EEPROM.write(blk_strt + 5, highByte(1199));
  EEPROM.write(blk_strt + 6, lowByte(1199));
  EEPROM.write(blk_strt + 7, highByte(1272));
  EEPROM.write(blk_strt + 8, lowByte(1272));
  EEPROM.write(blk_strt + 9, int8_t(FloatConv::INT16));
  
  // Block #3 - [1399, 1472, 2]
  EEPROM.write(blk_strt + 10, highByte(1399));
  EEPROM.write(blk_strt + 11, lowByte(1399));
  EEPROM.write(blk_strt + 12, highByte(1472));
  EEPROM.write(blk_strt + 13, lowByte(1472));
  EEPROM.write(blk_strt + 14, int8_t(FloatConv::INT16));

  // Block #4 - [1600, 1644, 8]
  EEPROM.write(blk_strt + 15, highByte(1600));
  EEPROM.write(blk_strt + 16, lowByte(1600));
  EEPROM.write(blk_strt + 17, highByte(1644));
  EEPROM.write(blk_strt + 18, lowByte(1644));
  EEPROM.write(blk_strt + 19, int8_t(FloatConv::MOD30K));

  // Block #5 - [1648, 1660, 7]
  EEPROM.write(blk_strt + 20, highByte(1648));
  EEPROM.write(blk_strt + 21, lowByte(1648));
  EEPROM.write(blk_strt + 22, highByte(1660));
  EEPROM.write(blk_strt + 23, lowByte(1660));
  EEPROM.write(blk_strt + 24, int8_t(FloatConv::MOD20K));

  // Block #6 - [1699, 1717, 2]
  EEPROM.write(blk_strt + 25, highByte(1699));
  EEPROM.write(blk_strt + 26, lowByte(1699));
  EEPROM.write(blk_strt + 27, highByte(1717));
  EEPROM.write(blk_strt + 28, lowByte(1717));
  EEPROM.write(blk_strt + 29, int8_t(FloatConv::INT16));

  // Block #7 - [1729, 1751, 2]
  EEPROM.write(blk_strt + 30, highByte(1729));
  EEPROM.write(blk_strt + 31, lowByte(1729));
  EEPROM.write(blk_strt + 32, highByte(1751));
  EEPROM.write(blk_strt + 33, lowByte(1751));
  EEPROM.write(blk_strt + 34, int8_t(FloatConv::INT16));
  
  // Group #1
  EEPROM.write(grp_strt, 3);
  EEPROM.write(grp_strt + 1, highByte(1002));
  EEPROM.write(grp_strt + 2, lowByte(1002));
  for (i = 1; i < 4; i++){
    EEPROM.write(i + grp_strt + 2, i);  // data type
    EEPROM.write(i + grp_strt + 5, int8_t(FloatConv::INT16));
  }

  // Group #2
  EEPROM.write(grp_strt + 9, 1);
  EEPROM.write(grp_strt + 10, highByte(1007));
  EEPROM.write(grp_strt + 11, lowByte(1007));
  EEPROM.write(grp_strt + 12, 4);
  EEPROM.write(grp_strt + 13, int8_t(FloatConv::INT16));

  // Group #3
  EEPROM.write(grp_strt + 14, 8);
  EEPROM.write(grp_strt + 15, highByte(1013));
  EEPROM.write(grp_strt + 16, lowByte(1013));
  EEPROM.write(grp_strt + 17, 10);
  EEPROM.write(grp_strt + 18, 11);
  EEPROM.write(grp_strt + 19, 12);
  EEPROM.write(grp_strt + 20, 13);
  EEPROM.write(grp_strt + 21, 6);
  EEPROM.write(grp_strt + 22, 7);
  EEPROM.write(grp_strt + 23, 8);
  EEPROM.write(grp_strt + 24, 9);
  for (i = (grp_strt + 25); i < (grp_strt + 25 + 8); i++){
    EEPROM.write(i, int8_t(FloatConv::INT16));  // data type
  }
  
  // Group #4
  EEPROM.write(grp_strt + 33, 4);
  EEPROM.write(grp_strt + 34, highByte(1030));
  EEPROM.write(grp_strt + 35, lowByte(1030));
  for (i = 26; i < 30; i++){
    EEPROM.write(i + grp_strt + 10, i);  // data type
    EEPROM.write(i + grp_strt + 14, int8_t(FloatConv::INT16));
  }

  // Group #5
  EEPROM.write(grp_strt + 44, 12);
  EEPROM.write(grp_strt + 45, highByte(1038));
  EEPROM.write(grp_strt + 46, lowByte(1038));
  for (i = 14; i < 26; i++){
    EEPROM.write(i + grp_strt + 33, i);  // data type
    EEPROM.write(i + grp_strt + 45, int8_t(FloatConv::INT16));
  }

  // Group #6
  EEPROM.write(grp_strt + 71, 3);
  EEPROM.write(grp_strt + 72, highByte(1616));
  EEPROM.write(grp_strt + 73, lowByte(1616));
  EEPROM.write(grp_strt + 74, 32);
  EEPROM.write(grp_strt + 75, 30);
  EEPROM.write(grp_strt + 76, 31);
  for (i = (grp_strt + 77); i < (grp_strt + 77 + 3); i++){
    EEPROM.write(i, int8_t(FloatConv::MOD30K));  // data type
  }

  // Group #7
  EEPROM.write(grp_strt + 80, 1);
  EEPROM.write(grp_strt + 81, 255);
  EEPROM.write(grp_strt + 82, 255);
  EEPROM.write(grp_strt + 83, 5);

  Serial.print(F("group 7: "));
  Serial.print(mtr_start, DEC);
  Serial.print(F(" to "));
  Serial.println(grp_strt + 84, DEC);
}

