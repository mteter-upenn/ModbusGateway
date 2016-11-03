uint16_t meter7(uint16_t mtr_start){
  uint16_t blk_strt;
  uint8_t blk_num;
  uint16_t grp_strt;
  uint8_t grp_num;
  
  // squareD cm2350, meter 7 *************************************************************
  // 126
  
  Serial.print(F("meter 7: "));
  Serial.print(mtr_start, DEC);

  blk_num = 7;
  grp_num = 3;
  blk_strt = mtr_start + 4 + grp_num * 2;
  // used to be 126, now it's 96
  grp_strt = blk_strt + (blk_num * 5);
  
  EEPROM.write(mtr_start, highByte(blk_strt));
  EEPROM.write(mtr_start + 1, lowByte(blk_strt));
  EEPROM.write(mtr_start + 2, blk_num);  // number of blocks;

  EEPROM.write(mtr_start + 3, grp_num);  // number of blocks;
  EEPROM.write(mtr_start + 4, highByte(grp_strt));
  EEPROM.write(mtr_start + 5, lowByte(grp_strt));
  EEPROM.write(mtr_start + 6, highByte(grp_strt + 39));
  EEPROM.write(mtr_start + 7, lowByte(grp_strt + 39));
  EEPROM.write(mtr_start + 8, highByte(grp_strt + 49));
  EEPROM.write(mtr_start + 9, lowByte(grp_strt + 49));


  // Block #1 - [999, 1116, 2]
  EEPROM.write(blk_strt, highByte(999));
  EEPROM.write(blk_strt + 1, lowByte(999));
  EEPROM.write(blk_strt + 2, highByte(1116));
  EEPROM.write(blk_strt + 3, lowByte(1116));
  EEPROM.write(blk_strt + 4, FloatConv2Int8(FloatConv::INT16));

  // Block #2 - [1199, 1272, 2]
  EEPROM.write(blk_strt + 5, highByte(1199));
  EEPROM.write(blk_strt + 6, lowByte(1199));
  EEPROM.write(blk_strt + 7, highByte(1272));
  EEPROM.write(blk_strt + 8, lowByte(1272));
  EEPROM.write(blk_strt + 9, FloatConv2Int8(FloatConv::INT16));
  
  // Block #3 - [1399, 1472, 2]
  EEPROM.write(blk_strt + 10, highByte(1399));
  EEPROM.write(blk_strt + 11, lowByte(1399));
  EEPROM.write(blk_strt + 12, highByte(1472));
  EEPROM.write(blk_strt + 13, lowByte(1472));
  EEPROM.write(blk_strt + 14, FloatConv2Int8(FloatConv::INT16));

  // Block #4 - [1600, 1644, 8]
  EEPROM.write(blk_strt + 15, highByte(1600));
  EEPROM.write(blk_strt + 16, lowByte(1600));
  EEPROM.write(blk_strt + 17, highByte(1644));
  EEPROM.write(blk_strt + 18, lowByte(1644));
  EEPROM.write(blk_strt + 19, FloatConv2Int8(FloatConv::MOD30K));

  // Block #5 - [1648, 1660, 7]
  EEPROM.write(blk_strt + 20, highByte(1648));
  EEPROM.write(blk_strt + 21, lowByte(1648));
  EEPROM.write(blk_strt + 22, highByte(1660));
  EEPROM.write(blk_strt + 23, lowByte(1660));
  EEPROM.write(blk_strt + 24, FloatConv2Int8(FloatConv::MOD20K));

  // Block #6 - [1699, 1717, 2]
  EEPROM.write(blk_strt + 25, highByte(1699));
  EEPROM.write(blk_strt + 26, lowByte(1699));
  EEPROM.write(blk_strt + 27, highByte(1717));
  EEPROM.write(blk_strt + 28, lowByte(1717));
  EEPROM.write(blk_strt + 29, FloatConv2Int8(FloatConv::INT16));

  // Block #7 - [1729, 1751, 2]
  EEPROM.write(blk_strt + 30, highByte(1729));
  EEPROM.write(blk_strt + 31, lowByte(1729));
  EEPROM.write(blk_strt + 32, highByte(1751));
  EEPROM.write(blk_strt + 33, lowByte(1751));
  EEPROM.write(blk_strt + 34, FloatConv2Int8(FloatConv::INT16));
  

  
  // New Group #1, +0
  EEPROM.write(grp_strt, 28);                 // number of values
  EEPROM.write(++grp_strt, 48);              // number of registers
  EEPROM.write(++grp_strt, highByte(1002));  // starting register
  EEPROM.write(++grp_strt, lowByte(1002));   // starting register
  EEPROM.write(++grp_strt, 37);              // skip bytes to data types
                                             // value types
  EEPROM.write(++grp_strt, 1);
  EEPROM.write(++grp_strt, 2);
  EEPROM.write(++grp_strt, 3);
  EEPROM.write(++grp_strt, int8_t(-2));
  EEPROM.write(++grp_strt, 4);
  EEPROM.write(++grp_strt, int8_t(-5));
  EEPROM.write(++grp_strt, 10);
  EEPROM.write(++grp_strt, 11);
  EEPROM.write(++grp_strt, 12);
  EEPROM.write(++grp_strt, 13);
  EEPROM.write(++grp_strt, 6);
  EEPROM.write(++grp_strt, 7);
  EEPROM.write(++grp_strt, 8);
  EEPROM.write(++grp_strt, 9);
  EEPROM.write(++grp_strt, int8_t(-9));
  EEPROM.write(++grp_strt, 26);
  EEPROM.write(++grp_strt, 27);
  EEPROM.write(++grp_strt, 28);
  EEPROM.write(++grp_strt, 29);
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
  // datatypes
  EEPROM.write(++grp_strt, FloatConv2Int8(FloatConv::INT16));
  EEPROM.write(++grp_strt, 255);


  // New Group #2, +39
  EEPROM.write(++grp_strt, 3);               // number of values
  EEPROM.write(++grp_strt, 12);              // number of registers
  EEPROM.write(++grp_strt, highByte(1616));  // starting register
  EEPROM.write(++grp_strt, lowByte(1616));   // starting register
  EEPROM.write(++grp_strt, 8);              // skip bytes to data types
                                            // value types
  EEPROM.write(++grp_strt, 32);
  EEPROM.write(++grp_strt, 30);
  EEPROM.write(++grp_strt, 31);
  // datatypes
  EEPROM.write(++grp_strt, FloatConv2Int8(FloatConv::MOD30K_WS));
  EEPROM.write(++grp_strt, 255);


  // New Group #3 (LAST), +49
  EEPROM.write(++grp_strt, 1); // number of values
                               // value types
  EEPROM.write(++grp_strt, 5);

  
  Serial.print(F(" to "));
  Serial.println(grp_strt, DEC);

  return ++grp_strt;
}

