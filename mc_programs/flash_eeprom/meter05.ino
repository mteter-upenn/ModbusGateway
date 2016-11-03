uint16_t meter5(uint16_t mtr_start){
  uint16_t blk_strt;
  uint8_t blk_num;
  uint16_t grp_strt;
  uint8_t grp_num;

  // Siemens 9330, 9350, 9360
  // 

  Serial.print(F("meter 5: "));
  Serial.print(mtr_start, DEC);
  
  blk_num = 3;
  grp_num = 2;
  blk_strt = mtr_start + 4 + grp_num * 2;
  // used to be 103, now it's 71
  grp_strt = blk_strt + (blk_num * 5);
  
  EEPROM.write(mtr_start, highByte(blk_strt));
  EEPROM.write(mtr_start + 1, lowByte(blk_strt));
  EEPROM.write(mtr_start + 2, blk_num);  // number of blocks;

  EEPROM.write(mtr_start + 3, grp_num);  // number of blocks;
  EEPROM.write(mtr_start + 4, highByte(grp_strt));
  EEPROM.write(mtr_start + 5, lowByte(grp_strt));
  EEPROM.write(mtr_start + 6, highByte(grp_strt + 46));
  EEPROM.write(mtr_start + 7, lowByte(grp_strt + 46));


  // Block #1 - [10, 25, 1]
  EEPROM.write(blk_strt, highByte(10));
  EEPROM.write(blk_strt + 1, lowByte(10));
  EEPROM.write(blk_strt + 2, highByte(25));
  EEPROM.write(blk_strt + 3, lowByte(25));
  EEPROM.write(blk_strt + 4, FloatConv2Int8(FloatConv::UINT16));

  // Block #2 - [26, 86, 4 | 128]
  EEPROM.write(blk_strt + 5, highByte(26));
  EEPROM.write(blk_strt + 6, lowByte(26));
  EEPROM.write(blk_strt + 7, highByte(86));
  EEPROM.write(blk_strt + 8, lowByte(86));
  EEPROM.write(blk_strt + 9, FloatConv2Int8(FloatConv::INT32_WS));
  
  // Block #3 - [88, 116, 6 | 128]
  EEPROM.write(blk_strt + 10, highByte(88));
  EEPROM.write(blk_strt + 11, lowByte(88));
  EEPROM.write(blk_strt + 12, highByte(116));
  EEPROM.write(blk_strt + 13, lowByte(116));
  EEPROM.write(blk_strt + 14, FloatConv2Int8(FloatConv::MOD10K_WS));



  // New Group #1, +0
  EEPROM.write(grp_strt, 31);                 // number of values
  EEPROM.write(++grp_strt, 96);              // number of registers
  EEPROM.write(++grp_strt, highByte(10));  // starting register
  EEPROM.write(++grp_strt, lowByte(10));   // starting register
  EEPROM.write(++grp_strt, 40);              // skip bytes to data types
                                             // value types
  EEPROM.write(++grp_strt, 6);
  EEPROM.write(++grp_strt, 7);
  EEPROM.write(++grp_strt, 8);
  EEPROM.write(++grp_strt, 9);
  EEPROM.write(++grp_strt, 10);
  EEPROM.write(++grp_strt, 11);
  EEPROM.write(++grp_strt, 12);
  EEPROM.write(++grp_strt, 13);
  EEPROM.write(++grp_strt, 1);
  EEPROM.write(++grp_strt, 2);
  EEPROM.write(++grp_strt, 3);
  EEPROM.write(++grp_strt, 4);
  EEPROM.write(++grp_strt, int8_t(-4));
  EEPROM.write(++grp_strt, 14);
  EEPROM.write(++grp_strt, 15);
  EEPROM.write(++grp_strt, 16);
  EEPROM.write(++grp_strt, 17);
  EEPROM.write(++grp_strt, 18);
  EEPROM.write(++grp_strt, 19);
  EEPROM.write(++grp_strt, 20);
  EEPROM.write(++grp_strt, 21);
  EEPROM.write(++grp_strt, 22);
  EEPROM.write(++grp_strt, 23);
  EEPROM.write(++grp_strt, 24);
  EEPROM.write(++grp_strt, 25);
  EEPROM.write(++grp_strt, 26);
  EEPROM.write(++grp_strt, 27);
  EEPROM.write(++grp_strt, 28);
  EEPROM.write(++grp_strt, 29);
  EEPROM.write(++grp_strt, int8_t(-34));
  EEPROM.write(++grp_strt, 30);
  EEPROM.write(++grp_strt, int8_t(-6));
  EEPROM.write(++grp_strt, 31);
  EEPROM.write(++grp_strt, int8_t(-2));
  EEPROM.write(++grp_strt, 32);
  // datatypes
  EEPROM.write(++grp_strt, FloatConv2Int8(FloatConv::UINT16));
  EEPROM.write(++grp_strt, 12);
  EEPROM.write(++grp_strt, FloatConv2Int8(FloatConv::INT32_WS));
  EEPROM.write(++grp_strt, 28);
  EEPROM.write(++grp_strt, FloatConv2Int8(FloatConv::MOD10K_WS));
  EEPROM.write(++grp_strt, 255);


  // New Group #2 (LAST), +46
  EEPROM.write(++grp_strt, 1); // number of values
                               // value types
  EEPROM.write(++grp_strt, 5);
  
  
  Serial.print(F(" to "));
  Serial.println(grp_strt, DEC);
  
  return ++grp_strt;
}
