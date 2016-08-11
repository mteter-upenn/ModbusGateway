void meter2(uint16_t mtr_start){
  uint16_t blk_strt;
  uint8_t blk_num;
  uint16_t grp_strt;
  uint8_t grp_num;
  uint16_t i;

  // emon dmon
  // 95
    
  blk_strt = mtr_start + 6;
  blk_num = 2;
  grp_num = 5;

  grp_strt = blk_strt + (blk_num * 5);
  
  EEPROM.write(mtr_start, highByte(blk_strt));
  EEPROM.write(mtr_start + 1, lowByte(blk_strt));
  EEPROM.write(mtr_start + 2, blk_num);  // number of blocks;

  EEPROM.write(mtr_start + 3, highByte(grp_strt));
  EEPROM.write(mtr_start + 4, lowByte(grp_strt));
  EEPROM.write(mtr_start + 5, grp_num);  // number of blocks;

  // Block #1 - [0, 6, 4 | 128]
  EEPROM.write(blk_strt, highByte(0));
  EEPROM.write(blk_strt + 1, lowByte(0));
  EEPROM.write(blk_strt + 2, highByte(6));
  EEPROM.write(blk_strt + 3, lowByte(6));  // discount 7 to avoid asking for 8 as well
  EEPROM.write(blk_strt + 4, int8_t(FloatConv::INT32_WS));
  
  // Block #2 - [1000, 1074, 128]
  EEPROM.write(blk_strt + 5, highByte(1000));
  EEPROM.write(blk_strt + 6, lowByte(1000));
  EEPROM.write(blk_strt + 7, highByte(1074));
  EEPROM.write(blk_strt + 8, lowByte(1074));
  EEPROM.write(blk_strt + 9, int8_t(FloatConv::FLOAT_WS));

  // Group #1
  EEPROM.write(grp_strt, 1);
  EEPROM.write(grp_strt + 1, highByte(1000));
  EEPROM.write(grp_strt + 2, lowByte(1000));
  EEPROM.write(grp_strt + 3, 30);
  EEPROM.write(grp_strt + 4, int8_t(FloatConv::FLOAT_WS));

  // Group #2
  EEPROM.write(grp_strt + 5, 1);
  EEPROM.write(grp_strt + 6, highByte(1004));
  EEPROM.write(grp_strt + 7, lowByte(1004));
  EEPROM.write(grp_strt + 8, 31);
  EEPROM.write(grp_strt + 9, int8_t(FloatConv::FLOAT_WS));

  // Group #3
  EEPROM.write(grp_strt + 10, 8);
  EEPROM.write(grp_strt + 11, highByte(1008));
  EEPROM.write(grp_strt + 12, lowByte(1008));
  EEPROM.write(grp_strt + 13, 17);  // collection type
  EEPROM.write(grp_strt + 14, 21);
  EEPROM.write(grp_strt + 15, 25);
  EEPROM.write(grp_strt + 16, 29);
  EEPROM.write(grp_strt + 17, 5);
  EEPROM.write(grp_strt + 18, 4);
  EEPROM.write(grp_strt + 19, 9);
  EEPROM.write(grp_strt + 20, 13);
  for (i = (grp_strt + 21); i < (grp_strt + 21 + 8); i++){
    EEPROM.write(i, int8_t(FloatConv::FLOAT_WS));  // data type
  }

  // Group #4
  EEPROM.write(grp_strt + 29, 21);
  EEPROM.write(grp_strt + 30, highByte(1028));
  EEPROM.write(grp_strt + 31, lowByte(1028));
  for (i = 1; i < 4; i++){
    EEPROM.write(i + grp_strt + 31, i + 13);  // data type
    EEPROM.write(i + grp_strt + 34, i + 17);  // data type
    EEPROM.write(i + grp_strt + 37, i + 21);  // data type
    EEPROM.write(i + grp_strt + 40, i + 25);  // data type
    EEPROM.write(i + grp_strt + 43, i);  // data type
    EEPROM.write(i + grp_strt + 46, i + 5);  // data type
    EEPROM.write(i + grp_strt + 49, i + 9);  // data type
  }
  for (i = (grp_strt + 53); i < (grp_strt + 53 + 21); i++){
    EEPROM.write(i, int8_t(FloatConv::FLOAT_WS));  // data type
  }

  // Group #5
  EEPROM.write(grp_strt + 74, 1);
  EEPROM.write(grp_strt + 75, 255);
  EEPROM.write(grp_strt + 76, 255);
  EEPROM.write(grp_strt + 77, 32);  // collection type
  
  Serial.print(F("group 2: "));
  Serial.print(mtr_start, DEC);
  Serial.print(F(" to "));
  Serial.println(grp_strt + 78, DEC);
}
