void meter8(uint16_t mtr_start){
  uint16_t blk_strt;
  uint8_t blk_num;
  uint16_t grp_strt;
  uint8_t grp_num;
  uint16_t i;

  // squareD pm710, meter 8 **************************************************************
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


  // Block #1 - [999, 1201, 128]
  EEPROM.write(blk_strt, highByte(999));
  EEPROM.write(blk_strt + 1, lowByte(999));
  EEPROM.write(blk_strt + 2, highByte(1201));
  EEPROM.write(blk_strt + 3, lowByte(1201));
  EEPROM.write(blk_strt + 4, int8_t(FloatConv::FLOAT_WS));

  // Block #2 - [3999, 4003, (3 | 128]
  EEPROM.write(blk_strt + 5, highByte(3999));
  EEPROM.write(blk_strt + 6, lowByte(3999));
  EEPROM.write(blk_strt + 7, highByte(4003));
  EEPROM.write(blk_strt + 8, lowByte(4003));
  EEPROM.write(blk_strt + 9, int8_t(FloatConv::UINT32_WS));
  
  // Block #3 - [4005, 4103, 2]
  EEPROM.write(blk_strt + 10, highByte(4005));
  EEPROM.write(blk_strt + 11, lowByte(4005));
  EEPROM.write(blk_strt + 12, highByte(4103));
  EEPROM.write(blk_strt + 13, lowByte(4103));
  EEPROM.write(blk_strt + 14, int8_t(FloatConv::INT16));

  // Group #1
  EEPROM.write(grp_strt, 10);
  EEPROM.write(grp_strt + 1, highByte(999));
  EEPROM.write(grp_strt + 2, lowByte(999));
  EEPROM.write(grp_strt + 3, 30);
  EEPROM.write(grp_strt + 4, 32);
  EEPROM.write(grp_strt + 5, 31);
  EEPROM.write(grp_strt + 6, 17);
  EEPROM.write(grp_strt + 7, 25);
  EEPROM.write(grp_strt + 8, 21);
  EEPROM.write(grp_strt + 9, 29);
  EEPROM.write(grp_strt + 10, 13);
  EEPROM.write(grp_strt + 11, 9);
  EEPROM.write(grp_strt + 12, 4);
  for (i = (grp_strt + 13); i < (grp_strt + 13 + 10); i++){
    EEPROM.write(i, int8_t(FloatConv::FLOAT_WS));  // data type
  }

  // Group #2
  EEPROM.write(grp_strt + 23, 3);
  EEPROM.write(grp_strt + 24, highByte(1033));
  EEPROM.write(grp_strt + 25, lowByte(1033));
  for (i = 1; i < 4; i++){
    EEPROM.write(i + grp_strt + 25, i);  // data type
    EEPROM.write(i + grp_strt + 28, int8_t(FloatConv::FLOAT_WS));
  }

  // Group #3
  EEPROM.write(grp_strt + 32, 15);
  EEPROM.write(grp_strt + 33, highByte(1053));
  EEPROM.write(grp_strt + 34, lowByte(1053));
  EEPROM.write(grp_strt + 35, 10);
  EEPROM.write(grp_strt + 36, 11);
  EEPROM.write(grp_strt + 37, 12);
  EEPROM.write(grp_strt + 38, 6);
  EEPROM.write(grp_strt + 39, 7);
  EEPROM.write(grp_strt + 40, 8);
  EEPROM.write(grp_strt + 41, 14);
  EEPROM.write(grp_strt + 42, 15);
  EEPROM.write(grp_strt + 43, 16);
  EEPROM.write(grp_strt + 44, 22);
  EEPROM.write(grp_strt + 45, 23);
  EEPROM.write(grp_strt + 46, 24);
  EEPROM.write(grp_strt + 47, 18);
  EEPROM.write(grp_strt + 48, 19);
  EEPROM.write(grp_strt + 49, 20);
  for (i = (grp_strt + 50); i < (grp_strt + 50 + 15); i++){
    EEPROM.write(i, int8_t(FloatConv::FLOAT_WS));  // data type
  }

  // Group #4
  EEPROM.write(grp_strt + 65, 4);
  EEPROM.write(grp_strt + 66, 255);
  EEPROM.write(grp_strt + 67, 255);
  EEPROM.write(grp_strt + 68, 5);
  EEPROM.write(grp_strt + 69, 26);
  EEPROM.write(grp_strt + 70, 27);
  EEPROM.write(grp_strt + 71, 28);

  Serial.print(F("group 8: "));
  Serial.print(mtr_start, DEC);
  Serial.print(F(" to "));
  Serial.println(grp_strt + 75, DEC);
}
