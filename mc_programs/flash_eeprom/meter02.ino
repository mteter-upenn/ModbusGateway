uint16_t meter2(uint16_t u16_mapStart){
  uint16_t u16_blkStart;
  uint16_t u8_numBlks;
  uint16_t u16_grpStart;
  uint16_t u8_numGrps;

  // emon dmon
  // 95
    
  Serial.print(F("meter 2: "));
  Serial.print(u16_mapStart, DEC);

  u8_numBlks = 2;
  u8_numGrps = 2;
  u16_blkStart = u16_mapStart + 4 + u8_numGrps * 2;
  // used to be 95, now it's 61
  u16_grpStart = u16_blkStart + (u8_numBlks * 5);


  EEPROM.write(u16_mapStart, highByte(u16_blkStart));
  EEPROM.write(u16_mapStart + 1, lowByte(u16_blkStart));
  EEPROM.write(u16_mapStart + 2, u8_numBlks);  // number of blocks;


  EEPROM.write(u16_mapStart + 3, u8_numGrps);  // number of blocks;
  EEPROM.write(u16_mapStart + 4, highByte(u16_grpStart));
  EEPROM.write(u16_mapStart + 5, lowByte(u16_grpStart));
  EEPROM.write(u16_mapStart + 6, highByte(u16_grpStart + 41));
  EEPROM.write(u16_mapStart + 7, lowByte(u16_grpStart + 41));

  // Block #1 - [0, 6, 4 | 128]
  EEPROM.write(u16_blkStart, highByte(0));
  EEPROM.write(u16_blkStart + 1, lowByte(0));
  EEPROM.write(u16_blkStart + 2, highByte(6));
  EEPROM.write(u16_blkStart + 3, lowByte(6));  // discount 7 to avoid asking for 8 as well
  EEPROM.write(u16_blkStart + 4, FloatConv2Int8(FloatConv::INT32_WS));
  
  // Block #2 - [1000, 1074, 128]
  EEPROM.write(u16_blkStart + 5, highByte(1000));
  EEPROM.write(u16_blkStart + 6, lowByte(1000));
  EEPROM.write(u16_blkStart + 7, highByte(1074));
  EEPROM.write(u16_blkStart + 8, lowByte(1074));
  EEPROM.write(u16_blkStart + 9, FloatConv2Int8(FloatConv::FLOAT_WS));

  

  // New Group #1, +0
  EEPROM.write(u16_grpStart, 31);                 // number of values
  EEPROM.write(++u16_grpStart, 70);              // number of registers
  EEPROM.write(++u16_grpStart, highByte(1000));  // starting register
  EEPROM.write(++u16_grpStart, lowByte(1000));   // starting register
  EEPROM.write(++u16_grpStart, 39);              // skip bytes to data types
                                             // value types
  EEPROM.write(++u16_grpStart, 30);
  EEPROM.write(++u16_grpStart, int8_t(-2));
  EEPROM.write(++u16_grpStart, 31);
  EEPROM.write(++u16_grpStart, int8_t(-2));
  EEPROM.write(++u16_grpStart, 17);
  EEPROM.write(++u16_grpStart, 21);
  EEPROM.write(++u16_grpStart, 25);
  EEPROM.write(++u16_grpStart, 29);
  EEPROM.write(++u16_grpStart, 5);
  EEPROM.write(++u16_grpStart, 4);
  EEPROM.write(++u16_grpStart, 9);
  EEPROM.write(++u16_grpStart, 13);
  EEPROM.write(++u16_grpStart, int8_t(-4));
  EEPROM.write(++u16_grpStart, 14);
  EEPROM.write(++u16_grpStart, 15);
  EEPROM.write(++u16_grpStart, 16);
  EEPROM.write(++u16_grpStart, 18);
  EEPROM.write(++u16_grpStart, 19);
  EEPROM.write(++u16_grpStart, 20);
  EEPROM.write(++u16_grpStart, 22);
  EEPROM.write(++u16_grpStart, 23);
  EEPROM.write(++u16_grpStart, 24);
  EEPROM.write(++u16_grpStart, 26);
  EEPROM.write(++u16_grpStart, 27);
  EEPROM.write(++u16_grpStart, 28);
  EEPROM.write(++u16_grpStart, 1);
  EEPROM.write(++u16_grpStart, 2);
  EEPROM.write(++u16_grpStart, 3);
  EEPROM.write(++u16_grpStart, 6);
  EEPROM.write(++u16_grpStart, 7);
  EEPROM.write(++u16_grpStart, 8);
  EEPROM.write(++u16_grpStart, 10);
  EEPROM.write(++u16_grpStart, 11);
  EEPROM.write(++u16_grpStart, 12);
  // datatypes
  EEPROM.write(++u16_grpStart, FloatConv2Int8(FloatConv::FLOAT_WS));
  EEPROM.write(++u16_grpStart, 255);


  // New Group #2 (LAST), +41
  EEPROM.write(++u16_grpStart, 1); // number of values
                               // value types
  EEPROM.write(++u16_grpStart, 32);
  
  
  Serial.print(F(" to "));
  Serial.println(u16_grpStart, DEC);

  return ++u16_grpStart;

}
