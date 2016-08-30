void meter15(uint16_t mtr_start) {
  uint16_t blk_strt;
  uint8_t blk_num;
  uint16_t grp_strt;
  uint8_t grp_num;

  // eaton series pxm 2000 **************************************************************
  // 108

  Serial.print(F("meter 15: "));
  Serial.print(mtr_start, DEC);

  blk_num = 1;
  grp_num = 6;
  blk_strt = mtr_start + 4 + grp_num * 2;
  // used to be 108, now it's 94
  grp_strt = blk_strt + (blk_num * 5);

  EEPROM.write(mtr_start, highByte(blk_strt));
  EEPROM.write(mtr_start + 1, lowByte(blk_strt));
  EEPROM.write(mtr_start + 2, blk_num);  // number of blocks;

  EEPROM.write(mtr_start + 3, grp_num);  // number of blocks;
  EEPROM.write(mtr_start + 4, highByte(grp_strt));
  EEPROM.write(mtr_start + 5, lowByte(grp_strt));
  EEPROM.write(++mtr_start, highByte(grp_strt + 15));
  EEPROM.write(++mtr_start, lowByte(grp_strt + 15));
  EEPROM.write(++mtr_start, highByte(grp_strt + 27));
  EEPROM.write(++mtr_start, lowByte(grp_strt + 27));
  EEPROM.write(++mtr_start, highByte(grp_strt + 43));
  EEPROM.write(++mtr_start, lowByte(grp_strt + 43));
  EEPROM.write(++mtr_start, highByte(grp_strt + 60));
  EEPROM.write(++mtr_start, lowByte(grp_strt + 60));
  EEPROM.write(++mtr_start, highByte(grp_strt + 71));
  EEPROM.write(++mtr_start, lowByte(grp_strt + 71));



                                         // Block #1 - [1, 201, 128]
  EEPROM.write(blk_strt, highByte(4002));
  EEPROM.write(blk_strt + 1, lowByte(4002));
  EEPROM.write(blk_strt + 2, highByte(7034));
  EEPROM.write(blk_strt + 3, lowByte(7034));
  EEPROM.write(blk_strt + 4, int8_t(FloatConv::FLOAT_WS));


  // New Group #1, +0
  EEPROM.write(grp_strt, 8);                 // number of values
  EEPROM.write(++grp_strt, 16);              // number of registers
  EEPROM.write(++grp_strt, highByte(4002));  // starting register
  EEPROM.write(++grp_strt, lowByte(4002));   // starting register
  EEPROM.write(++grp_strt, 13);              // skip bytes to data types
                                             // value types
  EEPROM.write(++grp_strt, 10);
  EEPROM.write(++grp_strt, 11);
  EEPROM.write(++grp_strt, 12);
  EEPROM.write(++grp_strt, 13);
  EEPROM.write(++grp_strt, 6);
  EEPROM.write(++grp_strt, 7);
  EEPROM.write(++grp_strt, 8);
  EEPROM.write(++grp_strt, 9);
  // datatypes
  EEPROM.write(++grp_strt, FloatConv2Int8(FloatConv::FLOAT_WS));
  EEPROM.write(++grp_strt, 255);


  // New Group #2, +15
  EEPROM.write(++grp_strt, 4);               // number of values
  EEPROM.write(++grp_strt, 12);              // number of registers
  EEPROM.write(++grp_strt, highByte(5002));  // starting register
  EEPROM.write(++grp_strt, lowByte(5002));   // starting register
  EEPROM.write(++grp_strt, 10);              // skip bytes to data types
                                             // value types
  EEPROM.write(++grp_strt, 1);
  EEPROM.write(++grp_strt, 2);
  EEPROM.write(++grp_strt, 3);
  EEPROM.write(++grp_strt, int8_t(-4));
  EEPROM.write(++grp_strt, 4);
  // datatypes
  EEPROM.write(++grp_strt, FloatConv2Int8(FloatConv::FLOAT_WS));
  EEPROM.write(++grp_strt, 255);


  // New Group #3, +27
  EEPROM.write(++grp_strt, 8);               // number of values
  EEPROM.write(++grp_strt, 72);              // number of registers
  EEPROM.write(++grp_strt, highByte(6000));  // starting register
  EEPROM.write(++grp_strt, lowByte(6000));   // starting register
  EEPROM.write(++grp_strt, 14);              // skip bytes to data types
                                             // value types
  EEPROM.write(++grp_strt, 14);
  EEPROM.write(++grp_strt, 15);
  EEPROM.write(++grp_strt, 16);
  EEPROM.write(++grp_strt, 17);
  EEPROM.write(++grp_strt, int8_t(-56));
  EEPROM.write(++grp_strt, 22);
  EEPROM.write(++grp_strt, 23);
  EEPROM.write(++grp_strt, 24);
  EEPROM.write(++grp_strt, 25);
  // datatypes
  EEPROM.write(++grp_strt, FloatConv2Int8(FloatConv::FLOAT_WS));
  EEPROM.write(++grp_strt, 255);


  // New Group #4, +43
  EEPROM.write(++grp_strt, 8);               // number of values
  EEPROM.write(++grp_strt, 94);              // number of registers
  EEPROM.write(++grp_strt, highByte(6128));  // starting register
  EEPROM.write(++grp_strt, lowByte(6128));   // starting register
  EEPROM.write(++grp_strt, 15);              // skip bytes to data types
                                             // value types
  EEPROM.write(++grp_strt, 18);
  EEPROM.write(++grp_strt, 19);
  EEPROM.write(++grp_strt, 20);
  EEPROM.write(++grp_strt, 21);
  EEPROM.write(++grp_strt, int8_t(-76));
  EEPROM.write(++grp_strt, 26);
  EEPROM.write(++grp_strt, 27);
  EEPROM.write(++grp_strt, 28);
  EEPROM.write(++grp_strt, int8_t(-2));
  EEPROM.write(++grp_strt, 29);
  // datatypes
  EEPROM.write(++grp_strt, FloatConv2Int8(FloatConv::FLOAT_WS));
  EEPROM.write(++grp_strt, 255);


  // New Group #5, +60
  EEPROM.write(++grp_strt, 3);               // number of values
  EEPROM.write(++grp_strt, 10);              // number of registers
  EEPROM.write(++grp_strt, highByte(7008));  // starting register
  EEPROM.write(++grp_strt, lowByte(7008));   // starting register
  EEPROM.write(++grp_strt, 9);              // skip bytes to data types
                                            // value types
  EEPROM.write(++grp_strt, 30);
  EEPROM.write(++grp_strt, int8_t(-4));
  EEPROM.write(++grp_strt, 31);
  EEPROM.write(++grp_strt, 32);
  // datatypes
  EEPROM.write(++grp_strt, FloatConv2Int8(FloatConv::FLOAT_WS));
  EEPROM.write(++grp_strt, 255);


  // New Group #6 (LAST), +71
  EEPROM.write(++grp_strt, 1); // number of values
                               // value types
  EEPROM.write(++grp_strt, 5);


  
  Serial.print(F(" to "));
  Serial.println(grp_strt, DEC);  // used to be +96
}


