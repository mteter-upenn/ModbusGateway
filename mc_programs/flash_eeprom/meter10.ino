void meter10(uint16_t mtr_start){
  uint16_t blk_strt;
  uint8_t blk_num;
  uint16_t grp_strt;
  uint8_t grp_num;
  uint16_t i;

  // squareD pm 800 series, meter 10 *****************************************************
  // 114
  
  blk_strt = mtr_start + 6; 
  blk_num = 4;
  grp_num = 8;

  grp_strt = blk_strt + (blk_num * 5);
  
  EEPROM.write(mtr_start, highByte(blk_strt));
  EEPROM.write(mtr_start + 1, lowByte(blk_strt));
  EEPROM.write(mtr_start + 2, blk_num);  // number of blocks;

  EEPROM.write(mtr_start + 3, highByte(grp_strt));
  EEPROM.write(mtr_start + 4, lowByte(grp_strt));
  EEPROM.write(mtr_start + 5, grp_num);  // number of blocks;

  // Block #1 - [1099, 1298, 2]
  EEPROM.write(blk_strt, highByte(1099));
  EEPROM.write(blk_strt + 1, lowByte(1099));
  EEPROM.write(blk_strt + 2, highByte(1298));
  EEPROM.write(blk_strt + 3, lowByte(1298));
  EEPROM.write(blk_strt + 4, int8_t(FloatConv::INT16));

  // Block #2 - [1699, 1743, 8]
  EEPROM.write(blk_strt + 5, highByte(1699));
  EEPROM.write(blk_strt + 6, lowByte(1699));
  EEPROM.write(blk_strt + 7, highByte(1746));
  EEPROM.write(blk_strt + 8, lowByte(1746));
  EEPROM.write(blk_strt + 9, int8_t(FloatConv::MOD30K));

  // Block #3 - [1747, 1759, 7]
  EEPROM.write(blk_strt + 10, highByte(1747));
  EEPROM.write(blk_strt + 11, lowByte(1747));
  EEPROM.write(blk_strt + 12, highByte(1759));
  EEPROM.write(blk_strt + 13, lowByte(1759));
  EEPROM.write(blk_strt + 14, int8_t(FloatConv::MOD20K));

  // Block #4 - [1766, 1790, 7]
  EEPROM.write(blk_strt + 15, highByte(1766));
  EEPROM.write(blk_strt + 16, lowByte(1766));
  EEPROM.write(blk_strt + 17, highByte(1790));
  EEPROM.write(blk_strt + 18, lowByte(1790));
  EEPROM.write(blk_strt + 19, int8_t(FloatConv::MOD20K));

  // Group #1
  EEPROM.write(grp_strt, 3);
  EEPROM.write(grp_strt + 1, highByte(1099));
  EEPROM.write(grp_strt + 2, lowByte(1099));
  for (i = 1; i < 4; i++){
    EEPROM.write(i + grp_strt + 2, i);  // data type
    EEPROM.write(i + grp_strt + 5, int8_t(FloatConv::INT16));
  }

  // Group #2
  EEPROM.write(grp_strt + 9, 1);
  EEPROM.write(grp_strt + 10, highByte(1104));
  EEPROM.write(grp_strt + 11, lowByte(1104));
  EEPROM.write(grp_strt + 12, 4);
  EEPROM.write(grp_strt + 13, int8_t(FloatConv::INT16));

  // Group #3
  EEPROM.write(grp_strt + 14, 7);
  EEPROM.write(grp_strt + 15, highByte(1119));
  EEPROM.write(grp_strt + 16, lowByte(1119));
  EEPROM.write(grp_strt + 17, 10);
  EEPROM.write(grp_strt + 18, 11);
  EEPROM.write(grp_strt + 19, 12);
  EEPROM.write(grp_strt + 20, 13);
  EEPROM.write(grp_strt + 21, 6);
  EEPROM.write(grp_strt + 22, 7);
  EEPROM.write(grp_strt + 23, 8);
  for (i = grp_strt + 24; i < (grp_strt + 24 + 7); i++){
    EEPROM.write(i, int8_t(FloatConv::INT16));  // data type
  }

  // Group #4
  EEPROM.write(grp_strt + 31, 1);
  EEPROM.write(grp_strt + 32, highByte(1127));
  EEPROM.write(grp_strt + 33, lowByte(1127));
  EEPROM.write(grp_strt + 34, 9);
  EEPROM.write(grp_strt + 35, int8_t(FloatConv::INT16));

  // Group #5
  EEPROM.write(grp_strt + 36, 12);
  EEPROM.write(grp_strt + 37, highByte(1139));
  EEPROM.write(grp_strt + 38, lowByte(1139));
  for (i = 14; i < 26; i++){
    EEPROM.write(i + grp_strt + 25, i);  // data type
    EEPROM.write(i + grp_strt + 37, int8_t(FloatConv::INT16));
  }

  // Group #6
  EEPROM.write(grp_strt + 63, 4);
  EEPROM.write(grp_strt + 64, highByte(1163));
  EEPROM.write(grp_strt + 65, lowByte(1163));
  for (i = 26; i < 30; i++){
    EEPROM.write(i + grp_strt + 40, i);
    EEPROM.write(i + grp_strt + 44, int8_t(FloatConv::INT16));
  }

  // Group #7
  EEPROM.write(grp_strt + 74, 3);
  EEPROM.write(grp_strt + 75, highByte(1715));
  EEPROM.write(grp_strt + 76, lowByte(1715));
  for (i = 30; i < 33; i++){
    EEPROM.write(i + grp_strt + 47, i);  // data type
    EEPROM.write(i + grp_strt + 50, int8_t(FloatConv::MOD30K));
  }
  
  // Group #8
  EEPROM.write(grp_strt + 83, 1);
  EEPROM.write(grp_strt + 84, 255);
  EEPROM.write(grp_strt + 85, 255);
  EEPROM.write(grp_strt + 86, 5);

  Serial.print(F("group 10: "));
  Serial.print(mtr_start, DEC);
  Serial.print(F(" to "));
  Serial.println(grp_strt + 87, DEC);
}
