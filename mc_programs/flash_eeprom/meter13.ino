void meter13(uint16_t mtr_start){
  uint16_t blk_strt;
  uint8_t blk_num;
  uint16_t grp_strt;
  uint8_t grp_num;
  uint16_t i;

  // squareD pm210, meter 13 **************************************************************
  // 97
  
  blk_strt = mtr_start + 6;
  blk_num = 3;
  grp_num = 4;
  // used to be 97, now it's 
  grp_strt = blk_strt + (blk_num * 5);
  
  EEPROM.write(mtr_start, highByte(blk_strt));
  EEPROM.write(mtr_start + 1, lowByte(blk_strt));
  EEPROM.write(mtr_start + 2, blk_num);  // number of blocks;

  EEPROM.write(mtr_start + 3, grp_num);  // number of blocks;
  EEPROM.write(mtr_start + 4, highByte(grp_strt));
  EEPROM.write(mtr_start + 5, lowByte(grp_strt));

  // Block #1 - [999, 1063, 128]
  EEPROM.write(blk_strt, highByte(999));
  EEPROM.write(blk_strt + 1, lowByte(999));
  EEPROM.write(blk_strt + 2, highByte(1063));
  EEPROM.write(blk_strt + 3, lowByte(1063));
  EEPROM.write(blk_strt + 4, int8_t(FloatConv::FLOAT_WS));

  // Block #2 - [3999, 4003, (3 | 128]
  EEPROM.write(blk_strt + 5, highByte(3999));
  EEPROM.write(blk_strt + 6, lowByte(3999));
  EEPROM.write(blk_strt + 7, highByte(4003));
  EEPROM.write(blk_strt + 8, lowByte(4003));
  EEPROM.write(blk_strt + 9, int8_t(FloatConv::UINT32_WS));
  
  // Block #3 - [4005, 4034, 2]
  EEPROM.write(blk_strt + 10, highByte(4005));
  EEPROM.write(blk_strt + 11, lowByte(4005));
  EEPROM.write(blk_strt + 12, highByte(4034));
  EEPROM.write(blk_strt + 13, lowByte(4034));
  EEPROM.write(blk_strt + 14, int8_t(FloatConv::INT16));

  // Group #1
  EEPROM.write(grp_strt, 7);
  EEPROM.write(grp_strt + 1, highByte(999));
  EEPROM.write(grp_strt + 2, lowByte(999));
  EEPROM.write(grp_strt + 3, 30);
  EEPROM.write(grp_strt + 4, 32);
  EEPROM.write(grp_strt + 5, 31);
  EEPROM.write(grp_strt + 6, 17);
  EEPROM.write(grp_strt + 7, 25);
  EEPROM.write(grp_strt + 8, 21);
  EEPROM.write(grp_strt + 9, 29);
  for (i = (grp_strt + 10); i < (grp_strt + 10 + 7); i++){
    EEPROM.write(i, int8_t(FloatConv::FLOAT_WS));  // data type
  }

  // Group #2
  EEPROM.write(grp_strt + 17, 3);
  EEPROM.write(grp_strt + 18, highByte(1033));
  EEPROM.write(grp_strt + 19, lowByte(1033));
  for (i = 1; i < 4; i++){
    EEPROM.write(i + grp_strt + 19, i);  // data type
    EEPROM.write(i + grp_strt + 22, int8_t(FloatConv::FLOAT_WS));
  }

  // Group #3
  EEPROM.write(grp_strt + 26, 6);
  EEPROM.write(grp_strt + 27, highByte(1053));
  EEPROM.write(grp_strt + 28, lowByte(1053));
  EEPROM.write(grp_strt + 29, 10);
  EEPROM.write(grp_strt + 30, 11);
  EEPROM.write(grp_strt + 31, 12);
  EEPROM.write(grp_strt + 32, 6);
  EEPROM.write(grp_strt + 33, 7);
  EEPROM.write(grp_strt + 34, 8);
  for (i = (grp_strt + 35); i < (grp_strt + 35 + 6); i++){
    EEPROM.write(i, int8_t(FloatConv::FLOAT_WS));  // data type
  }

  // Group #4
  EEPROM.write(grp_strt + 41, 16);
  EEPROM.write(grp_strt + 42, 255);
  EEPROM.write(grp_strt + 43, 255);
  EEPROM.write(grp_strt + 44, 4);
  EEPROM.write(grp_strt + 45, 5);
  EEPROM.write(grp_strt + 46, 9);
  EEPROM.write(grp_strt + 47, 13);
  EEPROM.write(grp_strt + 48, 14);
  EEPROM.write(grp_strt + 49, 15);
  EEPROM.write(grp_strt + 50, 16);
  EEPROM.write(grp_strt + 51, 18);
  EEPROM.write(grp_strt + 52, 19);
  EEPROM.write(grp_strt + 53, 20);
  EEPROM.write(grp_strt + 54, 22);
  EEPROM.write(grp_strt + 55, 23);
  EEPROM.write(grp_strt + 56, 24);
  EEPROM.write(grp_strt + 57, 26);
  EEPROM.write(grp_strt + 58, 27);
  EEPROM.write(grp_strt + 59, 28);

  Serial.print(F("group 13: "));
  Serial.print(mtr_start, DEC);
  Serial.print(F(" to "));
  Serial.println(grp_strt + 75, DEC);
}
