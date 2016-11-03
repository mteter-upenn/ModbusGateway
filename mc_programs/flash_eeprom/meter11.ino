uint16_t meter11(uint16_t mtr_start){
  uint16_t blk_strt;
  uint8_t blk_num;
  uint16_t grp_strt;
  uint8_t grp_num;

  // chw kep *****************************************************
  // 
  
  Serial.print(F("meter 11: "));
  Serial.print(mtr_start, DEC);

  blk_num = 5;  
  grp_num = 2;
  blk_strt = mtr_start + 4 + grp_num * 2;
  // used to be 66, now it's 54
  grp_strt = blk_strt + (blk_num * 5);
  
  EEPROM.write(mtr_start, highByte(blk_strt));
  EEPROM.write(mtr_start + 1, lowByte(blk_strt));
  EEPROM.write(mtr_start + 2, blk_num);  // number of blocks;

  EEPROM.write(mtr_start + 3, grp_num);  // number of blocks;
  EEPROM.write(mtr_start + 4, highByte(grp_strt));
  EEPROM.write(mtr_start + 5, lowByte(grp_strt));
  EEPROM.write(mtr_start + 6, highByte(grp_strt + 19));
  EEPROM.write(mtr_start + 7, lowByte(grp_strt + 19));


  // Block #1 - [0, 42, 0]
  EEPROM.write(blk_strt, highByte(0));
  EEPROM.write(blk_strt + 1, lowByte(0));
  EEPROM.write(blk_strt + 2, highByte(42));
  EEPROM.write(blk_strt + 3, lowByte(42));
  EEPROM.write(blk_strt + 4, FloatConv2Int8(FloatConv::FLOAT));

  // Block #2 - [44, 49, 2]
  EEPROM.write(blk_strt + 5, highByte(44));
  EEPROM.write(blk_strt + 6, lowByte(44));
  EEPROM.write(blk_strt + 7, highByte(49));
  EEPROM.write(blk_strt + 8, lowByte(49));
  EEPROM.write(blk_strt + 9, FloatConv2Int8(FloatConv::INT16));

  // Block #3 - [50, 58, 0]
  EEPROM.write(blk_strt + 10, highByte(50));
  EEPROM.write(blk_strt + 11, lowByte(50));
  EEPROM.write(blk_strt + 12, highByte(58));
  EEPROM.write(blk_strt + 13, lowByte(58));
  EEPROM.write(blk_strt + 14, FloatConv2Int8(FloatConv::FLOAT));

  // Block #4 - [60, 61, 2]
  EEPROM.write(blk_strt + 15, highByte(60));
  EEPROM.write(blk_strt + 16, lowByte(60));
  EEPROM.write(blk_strt + 17, highByte(61));
  EEPROM.write(blk_strt + 18, lowByte(61));
  EEPROM.write(blk_strt + 19, FloatConv2Int8(FloatConv::INT16));

  // Block #5 - [62, 62, 0]
  EEPROM.write(blk_strt + 20, highByte(62));
  EEPROM.write(blk_strt + 21, lowByte(62));
  EEPROM.write(blk_strt + 22, highByte(62));
  EEPROM.write(blk_strt + 23, lowByte(62));
  EEPROM.write(blk_strt + 24, FloatConv2Int8(FloatConv::FLOAT));



  // New Group #1
  EEPROM.write(grp_strt, 9);              // number of values
  EEPROM.write(++grp_strt, 30);           // number of registers
  EEPROM.write(++grp_strt, highByte(0));  // starting register
  EEPROM.write(++grp_strt, lowByte(0));   // 
  EEPROM.write(++grp_strt, 17);             // skip bytes to data types
  // value types
  EEPROM.write(++grp_strt, 1);
  EEPROM.write(++grp_strt, 2);
  EEPROM.write(++grp_strt, int8_t(-2));
  EEPROM.write(++grp_strt, 3);
  EEPROM.write(++grp_strt, 4);
  EEPROM.write(++grp_strt, 5);
  EEPROM.write(++grp_strt, 6);
  EEPROM.write(++grp_strt, int8_t(-8));
  EEPROM.write(++grp_strt, 8);
  EEPROM.write(++grp_strt, 9);
  EEPROM.write(++grp_strt, int8_t(-2));
  EEPROM.write(++grp_strt, 10);
  // datatypes
  EEPROM.write(++grp_strt, FloatConv2Int8(FloatConv::FLOAT));
  EEPROM.write(++grp_strt, 255);


  // New Group #2 (LAST), +19
  EEPROM.write(++grp_strt, 1); // number of values
  // value types
  EEPROM.write(++grp_strt, 7);


  //EEPROM.write(++grp_strt, );

 
  Serial.print(F(" to "));
  Serial.println(grp_strt, DEC);

  return ++grp_strt;
}
