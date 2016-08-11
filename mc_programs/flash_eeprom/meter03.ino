void meter3(uint16_t mtr_start){
  uint16_t blk_strt;
  uint8_t blk_num;
  uint16_t grp_strt;
  uint8_t grp_num;
  uint16_t i;

  // GE 3720
  // 
  
  blk_strt = mtr_start + 6;
  blk_num = 2;
  grp_num = 7;

  grp_strt = blk_strt + (blk_num * 5);
  
  EEPROM.write(mtr_start, highByte(blk_strt));
  EEPROM.write(mtr_start + 1, lowByte(blk_strt));
  EEPROM.write(mtr_start + 2, blk_num);  // number of blocks;

  EEPROM.write(mtr_start + 3, highByte(grp_strt));
  EEPROM.write(mtr_start + 4, lowByte(grp_strt));
  EEPROM.write(mtr_start + 5, grp_num);  // number of blocks;

  // Block #1 - [10, 49, 2]
  EEPROM.write(blk_strt, highByte(10));
  EEPROM.write(blk_strt + 1, lowByte(10));
  EEPROM.write(blk_strt + 2, highByte(49));
  EEPROM.write(blk_strt + 3, lowByte(49));
  EEPROM.write(blk_strt + 4, int8_t(FloatConv::INT16));

  // Block #2 - [50, 76, 5]
  EEPROM.write(blk_strt + 5, highByte(50));
  EEPROM.write(blk_strt + 6, lowByte(50));
  EEPROM.write(blk_strt + 7, highByte(76));
  EEPROM.write(blk_strt + 8, lowByte(76));
  EEPROM.write(blk_strt + 9, int8_t(FloatConv::MOD1K));
  

  // Group #1
  EEPROM.write(grp_strt, 8);
  EEPROM.write(grp_strt + 1, highByte(10));
  EEPROM.write(grp_strt + 2, lowByte(10));
  for (i = 6; i < 14; i++){
    EEPROM.write(i + grp_strt - 3, i);  // data type
    EEPROM.write(i + grp_strt + 5, int8_t(FloatConv::INT16));
  }

  // Group #2
  EEPROM.write(grp_strt + 19, 4);
  EEPROM.write(grp_strt + 20, highByte(20));
  EEPROM.write(grp_strt + 21, lowByte(20));
  for (i = 1; i < 5; i++){
    EEPROM.write(i + grp_strt + 21, i);  // data type
    EEPROM.write(i + grp_strt + 25, int8_t(FloatConv::INT16));
  }

  // Group #3
  EEPROM.write(grp_strt + 30, 16);
  EEPROM.write(grp_strt + 31, highByte(30));
  EEPROM.write(grp_strt + 32, lowByte(30));
  for (i = 14; i < 18; i++){
    EEPROM.write(i + grp_strt + 19, i);  // data type
    EEPROM.write(i + grp_strt + 23, i + 4);  // data type
    EEPROM.write(i + grp_strt + 27, i + 12);  // data type
    EEPROM.write(i + grp_strt + 31, i + 8);  // data type
    EEPROM.write(i + grp_strt + 35, int8_t(FloatConv::INT16));
    EEPROM.write(i + grp_strt + 39, int8_t(FloatConv::INT16));
    EEPROM.write(i + grp_strt + 43, int8_t(FloatConv::INT16));
    EEPROM.write(i + grp_strt + 47, int8_t(FloatConv::INT16));
  }

  // Group #4
  EEPROM.write(grp_strt + 65, 1);
  EEPROM.write(grp_strt + 66, highByte(54));
  EEPROM.write(grp_strt + 67, lowByte(54));
  EEPROM.write(grp_strt + 68, 30);
  EEPROM.write(grp_strt + 79, int8_t(FloatConv::MOD1K));

  // Group #5
  EEPROM.write(grp_strt + 70, 1);
  EEPROM.write(grp_strt + 71, highByte(64));
  EEPROM.write(grp_strt + 72, lowByte(64));
  EEPROM.write(grp_strt + 73, 31);
  EEPROM.write(grp_strt + 74, int8_t(FloatConv::MOD1K));

  // Group #6
  EEPROM.write(grp_strt + 75, 1);
  EEPROM.write(grp_strt + 76, highByte(74));
  EEPROM.write(grp_strt + 77, lowByte(74));
  EEPROM.write(grp_strt + 78, 32);
  EEPROM.write(grp_strt + 79, int8_t(FloatConv::MOD1K));
  
  // Group #7
  EEPROM.write(grp_strt + 80, 1);
  EEPROM.write(grp_strt + 81, 255);
  EEPROM.write(grp_strt + 82, 255);
  EEPROM.write(grp_strt + 83, 5);

  Serial.print(F("group 3: "));
  Serial.print(mtr_start, DEC);
  Serial.print(F(" to "));
  Serial.println(grp_strt + 84, DEC);
}
