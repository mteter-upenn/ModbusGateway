int meter6(uint16_t mtr_start){
  uint16_t blk_strt;
  uint8_t blk_num;
  int grp_strt;
  uint8_t grp_num;
  uint16_t i;
  
  // siemens 9510, 9610 meter 6 **********************************************************
  // 126
 
  
  blk_num = 4;
  grp_num = 2;
  blk_strt = mtr_start + 4 + grp_num * 2;

  // used to be 126, now it's 83
  grp_strt = blk_strt + (blk_num * 5);  // 28+55
  // mtr_start = 3264
  EEPROM.write(mtr_start, highByte(blk_strt));  // EEPROM address for block start
  EEPROM.write(mtr_start + 1, lowByte(blk_strt));
  EEPROM.write(mtr_start + 2, blk_num);  // number of blocks;

  EEPROM.write(mtr_start + 3, grp_num);  // number of groups;
  EEPROM.write(mtr_start + 4, highByte(grp_strt));  // group 1 address
  EEPROM.write(mtr_start + 5, lowByte(grp_strt));
  EEPROM.write(mtr_start + 6, highByte(grp_strt + 54));  // group 2 address
  EEPROM.write(mtr_start + 7, lowByte(grp_strt + 54));


  // Block #1 - [149, 164, 1]
  EEPROM.write(blk_strt, highByte(149));
  EEPROM.write(blk_strt + 1, lowByte(149));
  EEPROM.write(blk_strt + 2, highByte(164));
  EEPROM.write(blk_strt + 3, lowByte(164));
  EEPROM.write(blk_strt + 4, int8_t(FloatConv::UINT16));

  // Block #2 - [165, 187, 3 | 128]
  EEPROM.write(blk_strt + 5, highByte(165));
  EEPROM.write(blk_strt + 6, lowByte(165));
  EEPROM.write(blk_strt + 7, highByte(187));
  EEPROM.write(blk_strt + 8, lowByte(187));
  EEPROM.write(blk_strt + 9, int8_t(FloatConv::UINT32_WS));
  
  // Block #3 - [197, 237, 4 | 128]
  EEPROM.write(blk_strt + 10, highByte(197));
  EEPROM.write(blk_strt + 11, lowByte(197));
  EEPROM.write(blk_strt + 12, highByte(237));
  EEPROM.write(blk_strt + 13, lowByte(237));
  EEPROM.write(blk_strt + 14, int8_t(FloatConv::INT32_WS));

  // Block #4 - [261, 276, 2]
  EEPROM.write(blk_strt + 15, highByte(261));
  EEPROM.write(blk_strt + 16, lowByte(261));
  EEPROM.write(blk_strt + 17, highByte(276));
  EEPROM.write(blk_strt + 18, lowByte(276));
  EEPROM.write(blk_strt + 19, int8_t(FloatConv::INT16));


  // New Group #1
  EEPROM.write(grp_strt, 31); // number of values
  EEPROM.write(++grp_strt, 116);  // number of registers
  EEPROM.write(++grp_strt, highByte(149));  // starting register
  EEPROM.write(++grp_strt, lowByte(149));  // +3
  EEPROM.write(++grp_strt, 46);  // registers to add to grp_strt to get to data types, +4
  for (i = 1; i < 4; i++) { // 1, 2, 3
    EEPROM.write(++grp_strt, i);  // value type
  }
  EEPROM.write(++grp_strt, uint8_t(int8_t(-2)));  // skip 2 registers
  EEPROM.write(++grp_strt, 4);
  EEPROM.write(++grp_strt, int8_t(-10));  // 10
  for (i = 6; i < 10; i++) { // 6, 7, 8, 9
    EEPROM.write(++grp_strt, i);  // value type
  }
  EEPROM.write(++grp_strt, int8_t(-4));
  for (i = 10; i < 14; i++) { //
    EEPROM.write(++grp_strt, i);  // value type
  }
  EEPROM.write(++grp_strt, int8_t(-12));
  for (i = 14; i < 18; i++) { //
    EEPROM.write(++grp_strt, i);  // value type
  }
  EEPROM.write(++grp_strt, int8_t(-2));  // +25
  for (i = 18; i < 22; i++) { //
    EEPROM.write(++grp_strt, i);  // value type
  }
  EEPROM.write(++grp_strt, int8_t(-2));
  for (i = 22; i < 26; i++) { //
    EEPROM.write(++grp_strt, i);  // value type
  }
  EEPROM.write(++grp_strt, int8_t(-4));  // +35
  EEPROM.write(++grp_strt, 30);
  EEPROM.write(++grp_strt, int8_t(-2));
  EEPROM.write(++grp_strt, 31);
  EEPROM.write(++grp_strt, int8_t(-2));
  EEPROM.write(++grp_strt, 32);
  EEPROM.write(++grp_strt, int8_t(-22));
  for (i = 26; i < 30; i++) { //
    EEPROM.write(++grp_strt, i);  // value type
  }  // +45


  EEPROM.write(++grp_strt, FloatConv2Int8(FloatConv::UINT16));  // +46
  EEPROM.write(++grp_strt, 4);  // first four
  EEPROM.write(++grp_strt, int8_t(FloatConv::UINT32_WS));
  EEPROM.write(++grp_strt, 12);  // between four and twelve
  EEPROM.write(++grp_strt, int8_t(FloatConv::INT32_WS));
  EEPROM.write(++grp_strt, 27);
  EEPROM.write(++grp_strt, int8_t(FloatConv::INT16));
  EEPROM.write(++grp_strt, 255);  // +53

  // New Group #2
  EEPROM.write(++grp_strt, 1); // number of values, +54
  EEPROM.write(++grp_strt, 5);  // which values

  Serial.print(F("group 6: "));
  Serial.print(mtr_start, DEC);
  Serial.print(F(" to "));
  Serial.println(grp_strt, DEC);  // +55

  // EEPROM.write(++grp_strt, );
}

