void meter13(uint16_t mtr_start){
  uint16_t blk_strt;
  uint8_t blk_num;
  uint16_t grp_strt;
  uint8_t grp_num;

  // squareD pm210, meter 13 **************************************************************
  // 97
  
  Serial.print(F("meter 13: "));
  Serial.print(mtr_start, DEC);

  blk_num = 3;
  grp_num = 2;
  blk_strt = mtr_start + 4 + grp_num * 2;
  // used to be 97, now it's 65
  grp_strt = blk_strt + (blk_num * 5);
  
  EEPROM.write(mtr_start, highByte(blk_strt));
  EEPROM.write(mtr_start + 1, lowByte(blk_strt));
  EEPROM.write(mtr_start + 2, blk_num);  // number of blocks;

  EEPROM.write(mtr_start + 3, grp_num);  // number of blocks;
  EEPROM.write(mtr_start + 4, highByte(grp_strt));
  EEPROM.write(mtr_start + 5, lowByte(grp_strt));
  EEPROM.write(++mtr_start, highByte(grp_strt + 25));
  EEPROM.write(++mtr_start, lowByte(grp_strt + 25));

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

  
  // New Group #1, +0
  EEPROM.write(grp_strt, 16);                 // number of values
  EEPROM.write(++grp_strt, 66);              // number of registers
  EEPROM.write(++grp_strt, highByte(999));  // starting register
  EEPROM.write(++grp_strt, lowByte(999));   // starting register
  EEPROM.write(++grp_strt, 23);              // skip bytes to data types
                                             // value types
  EEPROM.write(++grp_strt, 30);
  EEPROM.write(++grp_strt, 32);
  EEPROM.write(++grp_strt, 31);
  EEPROM.write(++grp_strt, 17);
  EEPROM.write(++grp_strt, 25);
  EEPROM.write(++grp_strt, 21);
  EEPROM.write(++grp_strt, 29);
  EEPROM.write(++grp_strt, int8_t(-20));
  EEPROM.write(++grp_strt, 1);
  EEPROM.write(++grp_strt, 2);
  EEPROM.write(++grp_strt, 3);
  EEPROM.write(++grp_strt, int8_t(-14));
  EEPROM.write(++grp_strt, 10);
  EEPROM.write(++grp_strt, 11);
  EEPROM.write(++grp_strt, 12);
  EEPROM.write(++grp_strt, 6);
  EEPROM.write(++grp_strt, 7);
  EEPROM.write(++grp_strt, 8);
  // datatypes
  EEPROM.write(++grp_strt, FloatConv2Int8(FloatConv::FLOAT_WS));
  EEPROM.write(++grp_strt, 255);


  // New Group #2 (LAST), +25
  EEPROM.write(++grp_strt, 16); // number of values
                                // value types
  EEPROM.write(++grp_strt, 23);
  EEPROM.write(++grp_strt, 24);
  EEPROM.write(++grp_strt, 28);
  EEPROM.write(++grp_strt, 27);
  EEPROM.write(++grp_strt, 26);
  EEPROM.write(++grp_strt, 22);
  EEPROM.write(++grp_strt, 13);
  EEPROM.write(++grp_strt, 14);
  EEPROM.write(++grp_strt, 9);
  EEPROM.write(++grp_strt, 4);
  EEPROM.write(++grp_strt, 5);
  EEPROM.write(++grp_strt, 19);
  EEPROM.write(++grp_strt, 20);
  EEPROM.write(++grp_strt, 18);
  EEPROM.write(++grp_strt, 15);
  EEPROM.write(++grp_strt, 16);  //41

  
  Serial.print(F(" to "));
  Serial.println(grp_strt, DEC);
}
