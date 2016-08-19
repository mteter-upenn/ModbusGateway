void meter5(uint16_t mtr_start){
  uint16_t blk_strt;
  uint8_t blk_num;
  uint16_t grp_strt;
  uint8_t grp_num;
  uint16_t i;

  // Siemens 9330, 9350, 9360
  // 
  
  blk_strt = mtr_start + 6;
  blk_num = 3;
  grp_num = 6;

  grp_strt = blk_strt + (blk_num * 5);
  
  EEPROM.write(mtr_start, highByte(blk_strt));
  EEPROM.write(mtr_start + 1, lowByte(blk_strt));
  EEPROM.write(mtr_start + 2, blk_num);  // number of blocks;

  EEPROM.write(mtr_start + 3, grp_num);  // number of blocks;
  EEPROM.write(mtr_start + 4, highByte(grp_strt));
  EEPROM.write(mtr_start + 5, lowByte(grp_strt));


  // Block #1 - [10, 25, 1]
  EEPROM.write(blk_strt, highByte(10));
  EEPROM.write(blk_strt + 1, lowByte(10));
  EEPROM.write(blk_strt + 2, highByte(25));
  EEPROM.write(blk_strt + 3, lowByte(25));
  EEPROM.write(blk_strt + 4, int8_t(FloatConv::UINT16));

  // Block #2 - [26, 86, 4 | 128]
  EEPROM.write(blk_strt + 5, highByte(26));
  EEPROM.write(blk_strt + 6, lowByte(26));
  EEPROM.write(blk_strt + 7, highByte(86));
  EEPROM.write(blk_strt + 8, lowByte(86));
  EEPROM.write(blk_strt + 9, int8_t(FloatConv::INT32_WS));
  
  // Block #3 - [88, 116, 6 | 128]
  EEPROM.write(blk_strt + 10, highByte(88));
  EEPROM.write(blk_strt + 11, lowByte(88));
  EEPROM.write(blk_strt + 12, highByte(116));
  EEPROM.write(blk_strt + 13, lowByte(116));
  EEPROM.write(blk_strt + 14, int8_t(FloatConv::MOD10K_WS));



  // Group #1
  EEPROM.write(grp_strt, 12);
  EEPROM.write(grp_strt + 1, highByte(10));
  EEPROM.write(grp_strt + 2, lowByte(10));
  for (i = 1; i < 5; i++){
    EEPROM.write(i + grp_strt + 2, i + 5);  // data type
    EEPROM.write(i + grp_strt + 6, i + 9);  // data type
    EEPROM.write(i + grp_strt + 10, i);  // data type
    EEPROM.write(i + grp_strt + 14, int8_t(FloatConv::UINT16));
    EEPROM.write(i + grp_strt + 18, int8_t(FloatConv::UINT16));
    EEPROM.write(i + grp_strt + 22, int8_t(FloatConv::UINT16));
  }

  // Group #3
  EEPROM.write(grp_strt + 27, 16);
  EEPROM.write(grp_strt + 28, highByte(26));
  EEPROM.write(grp_strt + 29, lowByte(26));
  for (i = 14; i < 30; i++){
    EEPROM.write(i + grp_strt + 16, i);  // data type
    EEPROM.write(i + grp_strt + 32, int8_t(FloatConv::INT32_WS));
  }

  // Group #3
  EEPROM.write(grp_strt + 62, 1);
  EEPROM.write(grp_strt + 63, highByte(92));
  EEPROM.write(grp_strt + 64, lowByte(92));
  EEPROM.write(grp_strt + 65, 30);
  EEPROM.write(grp_strt + 66, int8_t(FloatConv::MOD10K_WS));

  // Group #4
  EEPROM.write(grp_strt + 67, 1);
  EEPROM.write(grp_strt + 68, highByte(100));
  EEPROM.write(grp_strt + 69, lowByte(100));
  EEPROM.write(grp_strt + 70, 31);
  EEPROM.write(grp_strt + 71, int8_t(FloatConv::MOD10K_WS));

  // Group #5
  EEPROM.write(grp_strt + 72, 1);
  EEPROM.write(grp_strt + 73, highByte(104));
  EEPROM.write(grp_strt + 74, lowByte(104));
  EEPROM.write(grp_strt + 75, 32);
  EEPROM.write(grp_strt + 76, int8_t(FloatConv::MOD10K_WS));

  // Group #6
  EEPROM.write(grp_strt + 77, 1);
  EEPROM.write(grp_strt + 78, 255);
  EEPROM.write(grp_strt + 79, 255);
  EEPROM.write(grp_strt + 80, 5);
  
  Serial.print(F("group 5: "));
  Serial.print(mtr_start, DEC);
  Serial.print(F(" to "));
  Serial.println(grp_strt + 81, DEC);
  
}
