void meter9(uint16_t mtr_start){
  uint16_t blk_strt;
  uint8_t blk_num;
  uint16_t grp_strt;
  uint8_t grp_num;
  uint16_t i;
  
  // squareD micrologic a, p, h trip units, meter 9 **************************************
  // 116
  
  blk_strt = mtr_start + 6;
  blk_num = 5;
  grp_num = 7;

  grp_strt = blk_strt + (blk_num * 5);
  
  EEPROM.write(mtr_start, highByte(blk_strt));  // 3866
  EEPROM.write(mtr_start + 1, lowByte(blk_strt));
  EEPROM.write(mtr_start + 2, blk_num);  // number of blocks;

  EEPROM.write(mtr_start + 3, highByte(grp_strt));
  EEPROM.write(mtr_start + 4, lowByte(grp_strt));
  EEPROM.write(mtr_start + 5, grp_num);  // number of blocks;

  // Block #1 - [999, 1134, 2]
  EEPROM.write(blk_strt, highByte(999));  //3872
  EEPROM.write(blk_strt + 1, lowByte(999));
  EEPROM.write(blk_strt + 2, highByte(1134));
  EEPROM.write(blk_strt + 3, lowByte(1134));
  EEPROM.write(blk_strt + 4, S16_to_FLOAT);

  // Block #2 - [1999, 2023, 8]
  EEPROM.write(blk_strt + 5, highByte(1999));
  EEPROM.write(blk_strt + 6, lowByte(1999));
  EEPROM.write(blk_strt + 7, highByte(2023));
  EEPROM.write(blk_strt + 8, lowByte(2023));
  EEPROM.write(blk_strt + 9, M30K_to_FLOAT);

  // Block #3 - [2199, 2206, 2]
  EEPROM.write(blk_strt + 10, highByte(2199));
  EEPROM.write(blk_strt + 11, lowByte(2199));
  EEPROM.write(blk_strt + 12, highByte(2206));
  EEPROM.write(blk_strt + 13, lowByte(2206));
  EEPROM.write(blk_strt + 14, S16_to_FLOAT);

  // Block #4 - [2211, 2218, 2]
  EEPROM.write(blk_strt + 15, highByte(2211));
  EEPROM.write(blk_strt + 16, lowByte(2211));
  EEPROM.write(blk_strt + 17, highByte(2218));
  EEPROM.write(blk_strt + 18, lowByte(2218));
  EEPROM.write(blk_strt + 19, S16_to_FLOAT);

// Block #5 - [2223, 2240, 2]
  EEPROM.write(blk_strt + 20, highByte(2223));
  EEPROM.write(blk_strt + 21, lowByte(2223));
  EEPROM.write(blk_strt + 22, highByte(2240));
  EEPROM.write(blk_strt + 23, lowByte(2240));
  EEPROM.write(blk_strt + 24, S16_to_FLOAT);

// Group #1
  EEPROM.write(grp_strt, 8);
  EEPROM.write(grp_strt + 1, highByte(999));
  EEPROM.write(grp_strt + 2, lowByte(999));
  EEPROM.write(grp_strt + 3, 10);
  EEPROM.write(grp_strt + 4, 11);
  EEPROM.write(grp_strt + 5, 12);
  EEPROM.write(grp_strt + 6, 6);
  EEPROM.write(grp_strt + 7, 7);
  EEPROM.write(grp_strt + 8, 8);
  EEPROM.write(grp_strt + 9, 13);
  EEPROM.write(grp_strt + 10, 9);
  for (i = (grp_strt + 11); i < (grp_strt + 11 + 8); i++){
    EEPROM.write(i, S16_to_FLOAT);  // data type
  }

  // Group #2
  EEPROM.write(grp_strt + 19, 3);
  EEPROM.write(grp_strt + 20, highByte(1015));
  EEPROM.write(grp_strt + 21, lowByte(1015));
  for (i = 1; i < 4; i++){
    EEPROM.write(i + grp_strt + 21, i);  // data type
    EEPROM.write(i + grp_strt + 24, S16_to_FLOAT);
  }

  // Group #3
  EEPROM.write(grp_strt + 28, 1);
  EEPROM.write(grp_strt + 29, highByte(1026));
  EEPROM.write(grp_strt + 30, lowByte(1026));
  EEPROM.write(grp_strt + 31, 4);
  EEPROM.write(grp_strt + 32, S16_to_FLOAT);

  // Group #4
  EEPROM.write(grp_strt + 33, 16);
  EEPROM.write(grp_strt + 34, highByte(1033));
  EEPROM.write(grp_strt + 35, lowByte(1033));
  for (i = 14; i < 30; i++){
    EEPROM.write(i + grp_strt + 22, i);  // data type
    EEPROM.write(i + grp_strt + 38, S16_to_FLOAT);
  }

  // Group #5
  EEPROM.write(grp_strt + 68, 2);
  EEPROM.write(grp_strt + 69, highByte(1999));
  EEPROM.write(grp_strt + 70, lowByte(1999));
  EEPROM.write(grp_strt + 71, 30);
  EEPROM.write(grp_strt + 72, 31);
  EEPROM.write(grp_strt + 73, M30K_to_FLOAT);
  EEPROM.write(grp_strt + 74, M30K_to_FLOAT);

  // Group #6
  EEPROM.write(grp_strt + 75, 1);
  EEPROM.write(grp_strt + 76, highByte(2023));
  EEPROM.write(grp_strt + 77, lowByte(2023));
  EEPROM.write(grp_strt + 78, 32);
  EEPROM.write(grp_strt + 79, M30K_to_FLOAT);

  // Group #7
  EEPROM.write(grp_strt + 80, 1);
  EEPROM.write(grp_strt + 81, 255);
  EEPROM.write(grp_strt + 82, 255);
  EEPROM.write(grp_strt + 83, 5);


  Serial.print(F("group 8: "));
  Serial.print(mtr_start, DEC);
  Serial.print(F(" to "));
  Serial.println(grp_strt + 84, DEC);
}
