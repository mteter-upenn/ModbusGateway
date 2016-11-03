uint16_t meter10(uint16_t mtr_start){
  uint16_t blk_strt;
  uint8_t blk_num;
  uint16_t grp_strt;
  uint8_t grp_num;

  // squareD pm 800 series, meter 10 *****************************************************
  // 114
  
  Serial.print(F("meter 10: "));
  Serial.print(mtr_start, DEC);

  blk_num = 4;
  grp_num = 3;
  blk_strt = mtr_start + 4 + grp_num * 2;
  // used to be 114, now it's 82
  grp_strt = blk_strt + (blk_num * 5);
  
  EEPROM.write(mtr_start, highByte(blk_strt));
  EEPROM.write(mtr_start + 1, lowByte(blk_strt));
  EEPROM.write(mtr_start + 2, blk_num);  // number of blocks;

  EEPROM.write(mtr_start + 3, grp_num);  // number of blocks;
  EEPROM.write(mtr_start + 4, highByte(grp_strt));
  EEPROM.write(mtr_start + 5, lowByte(grp_strt));
  EEPROM.write(mtr_start + 6, highByte(grp_strt + 40));
  EEPROM.write(mtr_start + 7, lowByte(grp_strt + 40));
  EEPROM.write(mtr_start + 8, highByte(grp_strt + 50));
  EEPROM.write(mtr_start + 9, lowByte(grp_strt + 50));

  // Block #1 - [1099, 1298, 2]
  EEPROM.write(blk_strt, highByte(1099));
  EEPROM.write(blk_strt + 1, lowByte(1099));
  EEPROM.write(blk_strt + 2, highByte(1298));
  EEPROM.write(blk_strt + 3, lowByte(1298));
  EEPROM.write(blk_strt + 4, FloatConv2Int8(FloatConv::INT16));

  // Block #2 - [1699, 1743, 8]
  EEPROM.write(blk_strt + 5, highByte(1699));
  EEPROM.write(blk_strt + 6, lowByte(1699));
  EEPROM.write(blk_strt + 7, highByte(1746));
  EEPROM.write(blk_strt + 8, lowByte(1746));
  EEPROM.write(blk_strt + 9, FloatConv2Int8(FloatConv::MOD30K));

  // Block #3 - [1747, 1759, 7]
  EEPROM.write(blk_strt + 10, highByte(1747));
  EEPROM.write(blk_strt + 11, lowByte(1747));
  EEPROM.write(blk_strt + 12, highByte(1759));
  EEPROM.write(blk_strt + 13, lowByte(1759));
  EEPROM.write(blk_strt + 14, FloatConv2Int8(FloatConv::MOD20K));

  // Block #4 - [1766, 1790, 7]
  EEPROM.write(blk_strt + 15, highByte(1766));
  EEPROM.write(blk_strt + 16, lowByte(1766));
  EEPROM.write(blk_strt + 17, highByte(1790));
  EEPROM.write(blk_strt + 18, lowByte(1790));
  EEPROM.write(blk_strt + 19, FloatConv2Int8(FloatConv::MOD20K));

  

  // New Group #1, +0
  EEPROM.write(grp_strt, 28);                 // number of values
  EEPROM.write(++grp_strt, 68);              // number of registers
  EEPROM.write(++grp_strt, highByte(1099));  // starting register
  EEPROM.write(++grp_strt, lowByte(1099));   // starting register
  EEPROM.write(++grp_strt, 38);              // skip bytes to data types
                                             // value types
  EEPROM.write(++grp_strt, 1);
  EEPROM.write(++grp_strt, 2);
  EEPROM.write(++grp_strt, 3);
  EEPROM.write(++grp_strt, int8_t(-1));
  EEPROM.write(++grp_strt, 4);
  EEPROM.write(++grp_strt, int8_t(-15));
  EEPROM.write(++grp_strt, 10);
  EEPROM.write(++grp_strt, 11);
  EEPROM.write(++grp_strt, 12);
  EEPROM.write(++grp_strt, 13);
  EEPROM.write(++grp_strt, 6);
  EEPROM.write(++grp_strt, 7);
  EEPROM.write(++grp_strt, 8);
  EEPROM.write(++grp_strt, int8_t(-1));
  EEPROM.write(++grp_strt, 9);
  EEPROM.write(++grp_strt, int8_t(-11));
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
  EEPROM.write(++grp_strt, int8_t(-12));
  EEPROM.write(++grp_strt, 26);
  EEPROM.write(++grp_strt, 27);
  EEPROM.write(++grp_strt, 28);
  EEPROM.write(++grp_strt, 29);
  // datatypes
  EEPROM.write(++grp_strt, FloatConv2Int8(FloatConv::INT16));
  EEPROM.write(++grp_strt, 255);


  // New Group #2, +40
  EEPROM.write(++grp_strt, 3);               // number of values
  EEPROM.write(++grp_strt, 12);              // number of registers
  EEPROM.write(++grp_strt, highByte(1715));  // starting register
  EEPROM.write(++grp_strt, lowByte(1715));   // starting register
  EEPROM.write(++grp_strt, 8);              // skip bytes to data types
                                            // value types
  EEPROM.write(++grp_strt, 30);
  EEPROM.write(++grp_strt, 31);
  EEPROM.write(++grp_strt, 32);
  // datatypes
  EEPROM.write(++grp_strt, FloatConv2Int8(FloatConv::MOD30K));
  EEPROM.write(++grp_strt, 255);


  // New Group #3 (LAST), +50
  EEPROM.write(++grp_strt, 1); // number of values
                               // value types
  EEPROM.write(++grp_strt, 5);

  
  Serial.print(F(" to "));
  Serial.println(grp_strt, DEC);

  return ++grp_strt;
}
