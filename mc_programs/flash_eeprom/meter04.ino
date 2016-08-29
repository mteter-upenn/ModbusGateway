void meter4(uint16_t mtr_start){
  uint16_t blk_strt;
  uint8_t blk_num;
  uint16_t grp_strt;
  uint8_t grp_num;
  //uint16_t i;

  // ge pqm
  // 384
  
  blk_strt = mtr_start + 6;
  blk_num = 52;
  grp_num = 18;
  // used to be , now it's 
  grp_strt = blk_strt + (blk_num * 5);
  
  //EEPROM.write(mtr_start, highByte(blk_strt));
  //EEPROM.write(mtr_start + 1, lowByte(blk_strt));
  //EEPROM.write(mtr_start + 2, blk_num);  // number of blocks;

  //EEPROM.write(mtr_start + 3, grp_num);  // number of blocks;
  //EEPROM.write(mtr_start + 4, highByte(grp_strt));
  //EEPROM.write(mtr_start + 5, lowByte(grp_strt));

//  // Block #1 - [, , ]
//  EEPROM.write(blk_strt, highByte());
//  EEPROM.write(blk_strt + 1, lowByte());
//  EEPROM.write(blk_strt + 2, highByte());
//  EEPROM.write(blk_strt + 3, lowByte());
//  EEPROM.write(blk_strt + 4, );
//
//  // Block #2 - [, , ]
//  EEPROM.write(blk_strt + 5, highByte());
//  EEPROM.write(blk_strt + 6, lowByte());
//  EEPROM.write(blk_strt + 7, highByte());
//  EEPROM.write(blk_strt + 8, lowByte());
//  EEPROM.write(blk_strt + 9, );
//  
//  // Block #3 - [, , ]
//  EEPROM.write(blk_strt + 10, highByte());
//  EEPROM.write(blk_strt + 11, lowByte());
//  EEPROM.write(blk_strt + 12, highByte());
//  EEPROM.write(blk_strt + 13, lowByte());
//  EEPROM.write(blk_strt + 14, );
//
//  // Block #4 - [, , ]
//  EEPROM.write(blk_strt + 15, highByte());
//  EEPROM.write(blk_strt + 16, lowByte());
//  EEPROM.write(blk_strt + 17, highByte());
//  EEPROM.write(blk_strt + 18, lowByte());
//  EEPROM.write(blk_strt + 19, );
//
//  // Block #5 - [, , ]
//  EEPROM.write(blk_strt + 20, highByte());
//  EEPROM.write(blk_strt + 21, lowByte());
//  EEPROM.write(blk_strt + 22, highByte());
//  EEPROM.write(blk_strt + 23, lowByte());
//  EEPROM.write(blk_strt + 24, );
//  
//  // Block #6 - [, , ]
//  EEPROM.write(blk_strt + 25, highByte());
//  EEPROM.write(blk_strt + 26, lowByte());
//  EEPROM.write(blk_strt + 27, highByte());
//  EEPROM.write(blk_strt + 28, lowByte());
//  EEPROM.write(blk_strt + 29, );
//
//  // Block #7 - [, , ]
//  EEPROM.write(blk_strt + 30, highByte());
//  EEPROM.write(blk_strt + 31, lowByte());
//  EEPROM.write(blk_strt + 32, highByte());
//  EEPROM.write(blk_strt + 33, lowByte());
//  EEPROM.write(blk_strt + 34, );
//
//  // Group #
//  EEPROM.write(grp_strt + , );
//  EEPROM.write(grp_strt + , highByte());
//  EEPROM.write(grp_strt + , lowByte());
//  EEPROM.write(grp_strt + , );
//  EEPROM.write(grp_strt + , );

}
