uint16_t meter9(uint16_t mtr_start){
  uint16_t blk_strt;
  uint8_t blk_num;
  uint16_t grp_strt;
  uint8_t grp_num;
  
  // squareD micrologic a, p, h trip units, meter 9 **************************************
  // 116
  
  Serial.print(F("meter 8: "));
  Serial.print(mtr_start, DEC);

  blk_num = 5;
  grp_num = 3;
  blk_strt = mtr_start + 4 + grp_num * 2;
  // used to be 116, now it's 86
  grp_strt = blk_strt + (blk_num * 5);
  
  EEPROM.write(mtr_start, highByte(blk_strt));  // 3866
  EEPROM.write(mtr_start + 1, lowByte(blk_strt));
  EEPROM.write(mtr_start + 2, blk_num);  // number of blocks;

  EEPROM.write(mtr_start + 3, grp_num);  // number of blocks;
  EEPROM.write(mtr_start + 4, highByte(grp_strt));
  EEPROM.write(mtr_start + 5, lowByte(grp_strt));
  EEPROM.write(mtr_start + 6, highByte(grp_strt + 38));
  EEPROM.write(mtr_start + 7, lowByte(grp_strt + 38));
  EEPROM.write(mtr_start + 8, highByte(grp_strt + 49));
  EEPROM.write(mtr_start + 9, lowByte(grp_strt + 49));


  // Block #1 - [999, 1134, 2]
  EEPROM.write(blk_strt, highByte(999));  //3872
  EEPROM.write(blk_strt + 1, lowByte(999));
  EEPROM.write(blk_strt + 2, highByte(1134));
  EEPROM.write(blk_strt + 3, lowByte(1134));
  EEPROM.write(blk_strt + 4, FloatConv2Int8(FloatConv::INT16));

  // Block #2 - [1999, 2023, 8]
  EEPROM.write(blk_strt + 5, highByte(1999));
  EEPROM.write(blk_strt + 6, lowByte(1999));
  EEPROM.write(blk_strt + 7, highByte(2023));
  EEPROM.write(blk_strt + 8, lowByte(2023));
  EEPROM.write(blk_strt + 9, FloatConv2Int8(FloatConv::MOD30K));

  // Block #3 - [2199, 2206, 2]
  EEPROM.write(blk_strt + 10, highByte(2199));
  EEPROM.write(blk_strt + 11, lowByte(2199));
  EEPROM.write(blk_strt + 12, highByte(2206));
  EEPROM.write(blk_strt + 13, lowByte(2206));
  EEPROM.write(blk_strt + 14, FloatConv2Int8(FloatConv::INT16));

  // Block #4 - [2211, 2218, 2]
  EEPROM.write(blk_strt + 15, highByte(2211));
  EEPROM.write(blk_strt + 16, lowByte(2211));
  EEPROM.write(blk_strt + 17, highByte(2218));
  EEPROM.write(blk_strt + 18, lowByte(2218));
  EEPROM.write(blk_strt + 19, FloatConv2Int8(FloatConv::INT16));

// Block #5 - [2223, 2240, 2]
  EEPROM.write(blk_strt + 20, highByte(2223));
  EEPROM.write(blk_strt + 21, lowByte(2223));
  EEPROM.write(blk_strt + 22, highByte(2240));
  EEPROM.write(blk_strt + 23, lowByte(2240));
  EEPROM.write(blk_strt + 24, FloatConv2Int8(FloatConv::INT16));



  // New Group #1, +0
  EEPROM.write(grp_strt, 28);                 // number of values
  EEPROM.write(++grp_strt, 50);              // number of registers
  EEPROM.write(++grp_strt, highByte(999));  // starting register
  EEPROM.write(++grp_strt, lowByte(999));   // starting register
  EEPROM.write(++grp_strt, 36);              // skip bytes to data types
                                             // value types
  EEPROM.write(++grp_strt, 10);
  EEPROM.write(++grp_strt, 11);
  EEPROM.write(++grp_strt, 12);
  EEPROM.write(++grp_strt, 6);
  EEPROM.write(++grp_strt, 7);
  EEPROM.write(++grp_strt, 8);
  EEPROM.write(++grp_strt, 13);
  EEPROM.write(++grp_strt, 9);
  EEPROM.write(++grp_strt, int8_t(-8));
  EEPROM.write(++grp_strt, 1);
  EEPROM.write(++grp_strt, 2);
  EEPROM.write(++grp_strt, 3);
  EEPROM.write(++grp_strt, int8_t(-8));
  EEPROM.write(++grp_strt, 4);
  EEPROM.write(++grp_strt, int8_t(-6));
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
  // datatypes
  EEPROM.write(++grp_strt, FloatConv2Int8(FloatConv::INT16));
  EEPROM.write(++grp_strt, 255);


  // New Group #2, +38
  EEPROM.write(++grp_strt, 3);               // number of values
  EEPROM.write(++grp_strt, 28);              // number of registers
  EEPROM.write(++grp_strt, highByte(1999));  // starting register
  EEPROM.write(++grp_strt, lowByte(1999));   // starting register
  EEPROM.write(++grp_strt, 9);              // skip bytes to data types
                                            // value types
  EEPROM.write(++grp_strt, 30);
  EEPROM.write(++grp_strt, 31);
  EEPROM.write(++grp_strt, int8_t(-16));
  EEPROM.write(++grp_strt, 32);
  // datatypes
  EEPROM.write(++grp_strt, FloatConv2Int8(FloatConv::MOD30K));
  EEPROM.write(++grp_strt, 255);


  // New Group #3 (LAST), +49
  EEPROM.write(++grp_strt, 1); // number of values
                               // value types
  EEPROM.write(++grp_strt, 5);


  
  Serial.print(F(" to "));
  Serial.println(grp_strt, DEC);

  return ++grp_strt;
}
