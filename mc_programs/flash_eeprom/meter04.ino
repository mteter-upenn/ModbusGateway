uint16_t meter4(uint16_t u16_mapStart){
  uint16_t u16_blkStart;
  uint8_t u8_numBlks;
  uint16_t u16_grpStart;
  uint8_t u8_numGrps;
  //uint16_t i;

  // ge pqm
  // 384
  
  u8_numBlks = 52;
  u8_numGrps = 0;
  u16_blkStart = u16_mapStart + 4 + u8_numGrps * 2;
  // used to be , now it's 
  u16_grpStart = u16_blkStart + (u8_numBlks * 5);
  

  return ++u16_grpStart;
  //EEPROM.write(u16_mapStart, highByte(u16_blkStart));
  //EEPROM.write(u16_mapStart + 1, lowByte(u16_blkStart));
  //EEPROM.write(u16_mapStart + 2, u8_numBlks);  // number of blocks;

  //EEPROM.write(u16_mapStart + 3, u8_numGrps);  // number of blocks;
  //EEPROM.write(u16_mapStart + 4, highByte(u16_grpStart));
  //EEPROM.write(u16_mapStart + 5, lowByte(u16_grpStart));

//  // Block #1 - [, , ]
//  EEPROM.write(u16_blkStart, highByte());
//  EEPROM.write(u16_blkStart + 1, lowByte());
//  EEPROM.write(u16_blkStart + 2, highByte());
//  EEPROM.write(u16_blkStart + 3, lowByte());
//  EEPROM.write(u16_blkStart + 4, );
//
//  // Block #2 - [, , ]
//  EEPROM.write(u16_blkStart + 5, highByte());
//  EEPROM.write(u16_blkStart + 6, lowByte());
//  EEPROM.write(u16_blkStart + 7, highByte());
//  EEPROM.write(u16_blkStart + 8, lowByte());
//  EEPROM.write(u16_blkStart + 9, );
//  
//  // Block #3 - [, , ]
//  EEPROM.write(u16_blkStart + 10, highByte());
//  EEPROM.write(u16_blkStart + 11, lowByte());
//  EEPROM.write(u16_blkStart + 12, highByte());
//  EEPROM.write(u16_blkStart + 13, lowByte());
//  EEPROM.write(u16_blkStart + 14, );
//
//  // Block #4 - [, , ]
//  EEPROM.write(u16_blkStart + 15, highByte());
//  EEPROM.write(u16_blkStart + 16, lowByte());
//  EEPROM.write(u16_blkStart + 17, highByte());
//  EEPROM.write(u16_blkStart + 18, lowByte());
//  EEPROM.write(u16_blkStart + 19, );
//
//  // Block #5 - [, , ]
//  EEPROM.write(u16_blkStart + 20, highByte());
//  EEPROM.write(u16_blkStart + 21, lowByte());
//  EEPROM.write(u16_blkStart + 22, highByte());
//  EEPROM.write(u16_blkStart + 23, lowByte());
//  EEPROM.write(u16_blkStart + 24, );
//  
//  // Block #6 - [, , ]
//  EEPROM.write(u16_blkStart + 25, highByte());
//  EEPROM.write(u16_blkStart + 26, lowByte());
//  EEPROM.write(u16_blkStart + 27, highByte());
//  EEPROM.write(u16_blkStart + 28, lowByte());
//  EEPROM.write(u16_blkStart + 29, );
//
//  // Block #7 - [, , ]
//  EEPROM.write(u16_blkStart + 30, highByte());
//  EEPROM.write(u16_blkStart + 31, lowByte());
//  EEPROM.write(u16_blkStart + 32, highByte());
//  EEPROM.write(u16_blkStart + 33, lowByte());
//  EEPROM.write(u16_blkStart + 34, );
//
//  // Group #
//  EEPROM.write(u16_grpStart + , );
//  EEPROM.write(u16_grpStart + , highByte());
//  EEPROM.write(u16_grpStart + , lowByte());
//  EEPROM.write(u16_grpStart + , );
//  EEPROM.write(u16_grpStart + , );

}
