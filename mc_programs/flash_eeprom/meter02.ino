uint16_t meter2(uint16_t mtr_start){
  uint16_t blk_strt;
  uint16_t blk_num;
  uint16_t grp_strt;
  uint16_t grp_num;

  // emon dmon
  // 95
    
  Serial.print(F("meter 2: "));
  Serial.print(mtr_start, DEC);

  blk_num = 2;
  grp_num = 2;
  blk_strt = mtr_start + 4 + grp_num * 2;
  // used to be 95, now it's 61
  grp_strt = blk_strt + (blk_num * 5);


  EEPROM.write(mtr_start, highByte(blk_strt));
  EEPROM.write(mtr_start + 1, lowByte(blk_strt));
  EEPROM.write(mtr_start + 2, blk_num);  // number of blocks;


  EEPROM.write(mtr_start + 3, grp_num);  // number of blocks;
  EEPROM.write(mtr_start + 4, highByte(grp_strt));
  EEPROM.write(mtr_start + 5, lowByte(grp_strt));
  EEPROM.write(mtr_start + 6, highByte(grp_strt + 41));
  EEPROM.write(mtr_start + 7, lowByte(grp_strt + 41));

  // Block #1 - [0, 6, 4 | 128]
  EEPROM.write(blk_strt, highByte(0));
  EEPROM.write(blk_strt + 1, lowByte(0));
  EEPROM.write(blk_strt + 2, highByte(6));
  EEPROM.write(blk_strt + 3, lowByte(6));  // discount 7 to avoid asking for 8 as well
  EEPROM.write(blk_strt + 4, FloatConv2Int8(FloatConv::INT32_WS));
  
  // Block #2 - [1000, 1074, 128]
  EEPROM.write(blk_strt + 5, highByte(1000));
  EEPROM.write(blk_strt + 6, lowByte(1000));
  EEPROM.write(blk_strt + 7, highByte(1074));
  EEPROM.write(blk_strt + 8, lowByte(1074));
  EEPROM.write(blk_strt + 9, FloatConv2Int8(FloatConv::FLOAT_WS));

  

  // New Group #1, +0
  EEPROM.write(grp_strt, 31);                 // number of values
  EEPROM.write(++grp_strt, 70);              // number of registers
  EEPROM.write(++grp_strt, highByte(1000));  // starting register
  EEPROM.write(++grp_strt, lowByte(1000));   // starting register
  EEPROM.write(++grp_strt, 39);              // skip bytes to data types
                                             // value types
  EEPROM.write(++grp_strt, 30);
  EEPROM.write(++grp_strt, int8_t(-2));
  EEPROM.write(++grp_strt, 31);
  EEPROM.write(++grp_strt, int8_t(-2));
  EEPROM.write(++grp_strt, 17);
  EEPROM.write(++grp_strt, 21);
  EEPROM.write(++grp_strt, 25);
  EEPROM.write(++grp_strt, 29);
  EEPROM.write(++grp_strt, 5);
  EEPROM.write(++grp_strt, 4);
  EEPROM.write(++grp_strt, 9);
  EEPROM.write(++grp_strt, 13);
  EEPROM.write(++grp_strt, int8_t(-4));
  EEPROM.write(++grp_strt, 14);
  EEPROM.write(++grp_strt, 15);
  EEPROM.write(++grp_strt, 16);
  EEPROM.write(++grp_strt, 18);
  EEPROM.write(++grp_strt, 19);
  EEPROM.write(++grp_strt, 20);
  EEPROM.write(++grp_strt, 22);
  EEPROM.write(++grp_strt, 23);
  EEPROM.write(++grp_strt, 24);
  EEPROM.write(++grp_strt, 26);
  EEPROM.write(++grp_strt, 27);
  EEPROM.write(++grp_strt, 28);
  EEPROM.write(++grp_strt, 1);
  EEPROM.write(++grp_strt, 2);
  EEPROM.write(++grp_strt, 3);
  EEPROM.write(++grp_strt, 6);
  EEPROM.write(++grp_strt, 7);
  EEPROM.write(++grp_strt, 8);
  EEPROM.write(++grp_strt, 10);
  EEPROM.write(++grp_strt, 11);
  EEPROM.write(++grp_strt, 12);
  // datatypes
  EEPROM.write(++grp_strt, FloatConv2Int8(FloatConv::FLOAT_WS));
  EEPROM.write(++grp_strt, 255);


  // New Group #2 (LAST), +41
  EEPROM.write(++grp_strt, 1); // number of values
                               // value types
  EEPROM.write(++grp_strt, 32);
  
  
  Serial.print(F(" to "));
  Serial.println(grp_strt, DEC);

  return ++grp_strt;

}
