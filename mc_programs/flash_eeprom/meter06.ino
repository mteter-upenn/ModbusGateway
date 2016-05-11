void meter6(uint16_t mtr_start){
  uint16_t blk_strt;
  uint8_t blk_num;
  uint16_t grp_strt;
  uint8_t grp_num;
  uint16_t i;
  
  // siemens 9510, 9610 meter 6 **********************************************************
  // 126
  
  blk_strt = mtr_start + 6;
  blk_num = 4;
  grp_num = 12;

  grp_strt = blk_strt + (blk_num * 5);
  // mtr_start = 3264
  EEPROM.write(mtr_start, highByte(blk_strt));
  EEPROM.write(mtr_start + 1, lowByte(blk_strt));
  EEPROM.write(mtr_start + 2, blk_num);  // number of blocks;

  EEPROM.write(mtr_start + 3, highByte(grp_strt));
  EEPROM.write(mtr_start + 4, lowByte(grp_strt));
  EEPROM.write(mtr_start + 5, grp_num);  // number of blocks;

  // Block #1 - [149, 164, 1]
  EEPROM.write(blk_strt, highByte(149));
  EEPROM.write(blk_strt + 1, lowByte(149));
  EEPROM.write(blk_strt + 2, highByte(164));
  EEPROM.write(blk_strt + 3, lowByte(164));
  EEPROM.write(blk_strt + 4, U16_to_FLOAT);

  // Block #2 - [165, 187, 3 | 128]
  EEPROM.write(blk_strt + 5, highByte(165));
  EEPROM.write(blk_strt + 6, lowByte(165));
  EEPROM.write(blk_strt + 7, highByte(187));
  EEPROM.write(blk_strt + 8, lowByte(187));
  EEPROM.write(blk_strt + 9, (U32_to_FLOAT | WORDSWAP));
  
  // Block #3 - [197, 237, 4 | 128]
  EEPROM.write(blk_strt + 10, highByte(197));
  EEPROM.write(blk_strt + 11, lowByte(197));
  EEPROM.write(blk_strt + 12, highByte(237));
  EEPROM.write(blk_strt + 13, lowByte(237));
  EEPROM.write(blk_strt + 14, (S32_to_FLOAT | WORDSWAP));

  // Block #4 - [261, 276, 2]
  EEPROM.write(blk_strt + 15, highByte(261));
  EEPROM.write(blk_strt + 16, lowByte(261));
  EEPROM.write(blk_strt + 17, highByte(276));
  EEPROM.write(blk_strt + 18, lowByte(276));
  EEPROM.write(blk_strt + 19, S16_to_FLOAT);

  // Group #1
  EEPROM.write(grp_strt, 3);
  EEPROM.write(grp_strt + 1, highByte(149));
  EEPROM.write(grp_strt + 2, lowByte(149));
  for (i = 1; i < 4; i++){
    EEPROM.write(i + grp_strt + 2, i);  // data type
    EEPROM.write(i + grp_strt + 5, U16_to_FLOAT);
  }

  // Group #2
  EEPROM.write(grp_strt + 9, 1);
  EEPROM.write(grp_strt + 10, highByte(154));
  EEPROM.write(grp_strt + 11, lowByte(154));
  EEPROM.write(grp_strt + 12, 4);
  EEPROM.write(grp_strt + 13, U16_to_FLOAT);

  // Group #3
  EEPROM.write(grp_strt + 14, 4);
  EEPROM.write(grp_strt + 15, highByte(165));
  EEPROM.write(grp_strt + 16, lowByte(165));
  for (i = 6; i < 10; i++){
    EEPROM.write(i + grp_strt + 11, i);  // data type
    EEPROM.write(i + grp_strt + 15, (U32_to_FLOAT | WORDSWAP));
  }

  // Group #4
  EEPROM.write(grp_strt + 25, 4);
  EEPROM.write(grp_strt + 26, highByte(177));
  EEPROM.write(grp_strt + 27, lowByte(177));
  for (i = 10; i < 14; i++){
    EEPROM.write(i + grp_strt + 18, i);  // data type
    EEPROM.write(i + grp_strt + 22, (U32_to_FLOAT | WORDSWAP));
  }

  // Group #5
  EEPROM.write(grp_strt + 36, 4);
  EEPROM.write(grp_strt + 37, highByte(197));
  EEPROM.write(grp_strt + 38, lowByte(197));
  for (i = 14; i < 18; i++){
    EEPROM.write(i + grp_strt + 25, i);  // data type
    EEPROM.write(i + grp_strt + 29, (S32_to_FLOAT | WORDSWAP));
  }

  // Group #6
  EEPROM.write(grp_strt + 47, 4);
  EEPROM.write(grp_strt + 48, highByte(207));
  EEPROM.write(grp_strt + 49, lowByte(207));
  for (i = 18; i < 22; i++){
    EEPROM.write(i + grp_strt + 32, i);  // data type
    EEPROM.write(i + grp_strt + 36, (S32_to_FLOAT | WORDSWAP));
  }

  // Group #7
  EEPROM.write(grp_strt + 58, 4);
  EEPROM.write(grp_strt + 59, highByte(217));
  EEPROM.write(grp_strt + 60, lowByte(217));
  for (i = 22; i < 26; i++){
    EEPROM.write(i + grp_strt + 39, i);  // data type
    EEPROM.write(i + grp_strt + 43, (S32_to_FLOAT | WORDSWAP));
  }

  // Group #8
  EEPROM.write(grp_strt + 69, 1);
  EEPROM.write(grp_strt + 70, highByte(229));
  EEPROM.write(grp_strt + 71, lowByte(229));
  EEPROM.write(grp_strt + 72, 30);
  EEPROM.write(grp_strt + 73, (S32_to_FLOAT | WORDSWAP));

  // Group #9
  EEPROM.write(grp_strt + 74, 1);
  EEPROM.write(grp_strt + 75, highByte(233));
  EEPROM.write(grp_strt + 76, lowByte(233));
  EEPROM.write(grp_strt + 77, 31);
  EEPROM.write(grp_strt + 78, (S32_to_FLOAT | WORDSWAP));

  // Group #10
  EEPROM.write(grp_strt + 79, 1);
  EEPROM.write(grp_strt + 80, highByte(237));
  EEPROM.write(grp_strt + 81, lowByte(237));
  EEPROM.write(grp_strt + 82, 32);
  EEPROM.write(grp_strt + 83, (S32_to_FLOAT | WORDSWAP));

  // Group #11
  EEPROM.write(grp_strt + 84, 4);
  EEPROM.write(grp_strt + 85, highByte(261));
  EEPROM.write(grp_strt + 86, lowByte(261));
  for (i = 26; i < 30; i++){
    EEPROM.write(i + grp_strt + 61, i);  // data type
    EEPROM.write(i + grp_strt + 65, S16_to_FLOAT);
  }

  // Group #12
  EEPROM.write(grp_strt + 95, 1);
  EEPROM.write(grp_strt + 96, 255);
  EEPROM.write(grp_strt + 97, 255);
  EEPROM.write(grp_strt + 98, 5);

  Serial.print(F("group 6: "));
  Serial.print(mtr_start, DEC);
  Serial.print(F(" to "));
  Serial.println(grp_strt + 99, DEC);
}

