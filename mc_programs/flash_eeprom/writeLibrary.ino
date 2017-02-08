uint16_t writeBlocks(uint16_t reg_strt) {
  uint16_t indMtrStrt;

  //  EEPROM.write(bt_strt, 2);  // current meter type  default is 2.1.0
  //  EEPROM.write(bt_strt + 1, 1);
  //  EEPROM.write(bt_strt + 2, 0);

  EEPROM.write(reg_strt + 0, highByte(reg_strt));
  EEPROM.write(reg_strt + 1, lowByte(reg_strt));  // address of meter addresses
  EEPROM.write(reg_strt + 2, 15);  // number of meters adjust when adding new meters AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA



                                   // 6 + blknum * 5 + grpnum * 3 + 32 * 2  
#if defined(CORE_TEENSY)  // if teensy3.0 or greater
  indMtrStrt = 2048;
#else
  indMtrStrt = 4096;
#endif

  //// New Group #1, +0
  //EEPROM.write(grp_strt, );                 // number of values
  //EEPROM.write(++grp_strt, );              // number of registers
  //EEPROM.write(++grp_strt, highByte());  // starting register
  //EEPROM.write(++grp_strt, lowByte());   // starting register
  //EEPROM.write(++grp_strt, );              // skip bytes to data types


  // eaton power xpert 4000    #1
  indMtrStrt -= 83;
  meter1(indMtrStrt);
  EEPROM.write(reg_strt + 3, highByte(indMtrStrt));  // 117 -> 146
  EEPROM.write(reg_strt + 4, lowByte(indMtrStrt));
  EEPROM.write(reg_strt + 5, 1);  // meter number
  EEPROM.write(reg_strt + 6, 3);  // function

// emon dmon    #2
  indMtrStrt -= 61;
  meter2(indMtrStrt);
  EEPROM.write(reg_strt + 7, highByte(indMtrStrt));  // 95 -> 150
  EEPROM.write(reg_strt + 8, lowByte(indMtrStrt));
  EEPROM.write(reg_strt + 9, 2);  // meter number
  EEPROM.write(reg_strt + 10, 3);  // function

// ge epm 3720    #3
  indMtrStrt -= 65;
  meter3(indMtrStrt);
  EEPROM.write(reg_strt + 11, highByte(indMtrStrt));  // 101 -> 150
  EEPROM.write(reg_strt + 12, lowByte(indMtrStrt));
  EEPROM.write(reg_strt + 13, 3);  // meter number
  EEPROM.write(reg_strt + 14, 3);  // function

  // ge pqm    #4
  indMtrStrt -= 0;

  meter4(0);
  EEPROM.write(reg_strt + 15, highByte(0));  // assume 400
  EEPROM.write(reg_strt + 16, lowByte(0));

  EEPROM.write(reg_strt + 17, 4);  // meter number
  EEPROM.write(reg_strt + 18, 3);  // function


  // siemens 9330, 9350, 9360    #5
  indMtrStrt -= 71;
  meter5(indMtrStrt);
  EEPROM.write(reg_strt + 19, highByte(indMtrStrt));  // 103 -> 150
  EEPROM.write(reg_strt + 20, lowByte(indMtrStrt));
  EEPROM.write(reg_strt + 21, 5);  // meter number
  EEPROM.write(reg_strt + 22, 3);  // function

// siemens 9510, 9610    #6
  indMtrStrt -= 84;
  meter6(indMtrStrt);
  EEPROM.write(reg_strt + 23, highByte(indMtrStrt));  // 126 -> 175
  EEPROM.write(reg_strt + 24, lowByte(indMtrStrt));
  EEPROM.write(reg_strt + 25, 6);  // meter number
  EEPROM.write(reg_strt + 26, 3);  // function

// squareD cm2350   #7
  indMtrStrt -= 96;
  meter7(indMtrStrt);
  EEPROM.write(reg_strt + 27, highByte(indMtrStrt));  // 126 -> 175
  EEPROM.write(reg_strt + 28, lowByte(indMtrStrt));
  EEPROM.write(reg_strt + 29, 7);  // meter number
  EEPROM.write(reg_strt + 30, 3);  // function

// squareD pm710   #8
  indMtrStrt -= 65;
  meter8(indMtrStrt);
  EEPROM.write(reg_strt + 31, highByte(indMtrStrt));  // 97 -> 150
  EEPROM.write(reg_strt + 32, lowByte(indMtrStrt));
  EEPROM.write(reg_strt + 33, 8);  // meter number
  EEPROM.write(reg_strt + 34, 3);  // function

// squareD micrologic a, p, h trip units    #9
  indMtrStrt -= 86;
  meter9(indMtrStrt);
  EEPROM.write(reg_strt + 35, highByte(indMtrStrt));  // 116 -> 150
  EEPROM.write(reg_strt + 36, lowByte(indMtrStrt));
  EEPROM.write(reg_strt + 37, 9);  // meter number
  EEPROM.write(reg_strt + 38, 3);  // function

// squareD cm3350, cm4000 series, pm800 series    #10
  indMtrStrt -= 82;
  meter10(indMtrStrt);
  EEPROM.write(reg_strt + 39, highByte(indMtrStrt));  // 111 -> 150
  EEPROM.write(reg_strt + 40, lowByte(indMtrStrt));
  EEPROM.write(reg_strt + 41, 10);  // meter number
  EEPROM.write(reg_strt + 42, 3);  // function

// Chilled water KEP    #11
  indMtrStrt -= 54;
  meter11(indMtrStrt);
  EEPROM.write(reg_strt + 43, highByte(indMtrStrt));
  EEPROM.write(reg_strt + 44, lowByte(indMtrStrt));
  EEPROM.write(reg_strt + 45, 11);  // meter number
  EEPROM.write(reg_strt + 46, 3);  // function

// Steam KEP    #12
  indMtrStrt -= 55;
  meter12(indMtrStrt);
  EEPROM.write(reg_strt + 47, highByte(indMtrStrt));  // 69 -> 100
  EEPROM.write(reg_strt + 48, lowByte(indMtrStrt));
  EEPROM.write(reg_strt + 49, 12);  // meter number
  EEPROM.write(reg_strt + 50, 3);  // function

// SquareD PM210  13
  indMtrStrt -= 65;
  meter13(indMtrStrt);
  EEPROM.write(reg_strt + 51, highByte(indMtrStrt));  // 97 -> 150
  EEPROM.write(reg_strt + 52, lowByte(indMtrStrt));
  EEPROM.write(reg_strt + 53, 13);  // meter number
  EEPROM.write(reg_strt + 54, 3);  // function

// Siemens Pac4200/3200 14
  indMtrStrt -= 95;
  meter14(indMtrStrt);
  EEPROM.write(reg_strt + 55, highByte(indMtrStrt));  //  -> 150
  EEPROM.write(reg_strt + 56, lowByte(indMtrStrt));
  EEPROM.write(reg_strt + 57, 14);  // meter number
  EEPROM.write(reg_strt + 58, 3);  // function

// Eaton Series PXM 2000 15
  indMtrStrt -= 94;
  meter15(indMtrStrt);
  EEPROM.write(reg_strt + 59, highByte(indMtrStrt));  //  -> 150
  EEPROM.write(reg_strt + 60, lowByte(indMtrStrt));
  EEPROM.write(reg_strt + 61, 15);  // meter number
  EEPROM.write(reg_strt + 62, 3);  // function

  Serial.print("Meter register library starts at: ");
  Serial.println(indMtrStrt);

  return (reg_strt + 63);

  //  //
  //  indMtrStrt -= ;
  //  meterXX(indMtrStrt);
  //  EEPROM.write(mtr_strt + , highByte(indMtrStrt));  //  -> 150
  //  EEPROM.write(mtr_strt + , lowByte(indMtrStrt));
  //  EEPROM.write(mtr_strt + , );  // meter number
  //  EEPROM.write(mtr_strt + , );  // function

  /*
  * |------|-----------------------------|--------------------------------|
  * | ID:  | Collection types (elec):    | Collection types (stm/chw):    |
  * |------|-----------------------------|--------------------------------|
  * |  1   |   Current, A                |   Heat flow                    |
  * |  2   |   Current, B                |   Mass flow                    |
  * |  3   |   Current, C                |   Volumetric flow              |
  * |  4   |   Current, Average          |   Temperature 1                |
  * |  5   |   Current, Total            |   Temperature 2                |
  * |------|-----------------------------|--------------------------------|
  * |  6   |   Voltage, L-N, A           |   Temperature delta            |
  * |  7   |   Voltage, L-N, B           |   Pressure                     |
  * |  8   |   Voltage, L-N, C           |   Heat total                   |
  * |  9   |   Voltage, L-N, Average     |   Mass total                   |
  * |  10  |   Voltage, L-L, A-B         |   Volume total                 |
  * |------|-----------------------------|--------------------------------|
  * |  11  |   Voltage, L-L, B-C         |                                |
  * |  12  |   Voltage, L-L, C-A         |                                |
  * |  13  |   Voltage, L-L, Average     |                                |
  * |  14  |   Real Power, A             |                                |
  * |  15  |   Real Power, B             |                                |
  * |------|-----------------------------|--------------------------------|
  * |  16  |   Real Power, C             |                                |
  * |  17  |   Real Power, Total         |                                |
  * |  18  |   Reactive Power, A         |                                |
  * |  19  |   Reactive Power, B         |                                |
  * |  20  |   Reactive Power, C         |                                |
  * |------|-----------------------------|--------------------------------|
  * |  21  |   Reactive Power, Total     |                                |
  * |  22  |   Apparent Power, A         |                                |
  * |  23  |   Apparent Power, B         |                                |
  * |  24  |   Apparent Power, C         |                                |
  * |  25  |   Apparent Power, Total     |                                |
  * |------|-----------------------------|--------------------------------|
  * |  26  |   Power Factor, A           |                                |
  * |  27  |   Power Factor, B           |                                |
  * |  28  |   Power Factor, C           |                                |
  * |  29  |   Power Factor, Total       |                                |
  * |  30  |   Real Energy, Total        |                                |
  * |------|-----------------------------|--------------------------------|
  * |  31  |   Reactive Energy, Total    |                                |
  * |  32  |   Apparent Energy, Total    |                                |
  * |------|-----------------------------|--------------------------------|
  *
  *
  // Block #1 - [, , ]
  EEPROM.write(blk_strt, highByte());
  EEPROM.write(blk_strt + 1, lowByte());
  EEPROM.write(blk_strt + 2, highByte());
  EEPROM.write(blk_strt + 3, lowByte());
  EEPROM.write(blk_strt + 4, );

  // Block #2 - [, , ]
  EEPROM.write(blk_strt + 5, highByte());
  EEPROM.write(blk_strt + 6, lowByte());
  EEPROM.write(blk_strt + 7, highByte());
  EEPROM.write(blk_strt + 8, lowByte());
  EEPROM.write(blk_strt + 9, );

  // Block #3 - [, , ]
  EEPROM.write(blk_strt + 10, highByte());
  EEPROM.write(blk_strt + 11, lowByte());
  EEPROM.write(blk_strt + 12, highByte());
  EEPROM.write(blk_strt + 13, lowByte());
  EEPROM.write(blk_strt + 14, );

  // Block #4 - [, , ]
  EEPROM.write(blk_strt + 15, highByte());
  EEPROM.write(blk_strt + 16, lowByte());
  EEPROM.write(blk_strt + 17, highByte());
  EEPROM.write(blk_strt + 18, lowByte());
  EEPROM.write(blk_strt + 19, );

  // Block #5 - [, , ]
  EEPROM.write(blk_strt + 20, highByte());
  EEPROM.write(blk_strt + 21, lowByte());
  EEPROM.write(blk_strt + 22, highByte());
  EEPROM.write(blk_strt + 23, lowByte());
  EEPROM.write(blk_strt + 24, );

  // Block #6 - [, , ]
  EEPROM.write(blk_strt + 25, highByte());
  EEPROM.write(blk_strt + 26, lowByte());
  EEPROM.write(blk_strt + 27, highByte());
  EEPROM.write(blk_strt + 28, lowByte());
  EEPROM.write(blk_strt + 29, );

  // Block #7 - [, , ]
  EEPROM.write(blk_strt + 30, highByte());
  EEPROM.write(blk_strt + 31, lowByte());
  EEPROM.write(blk_strt + 32, highByte());
  EEPROM.write(blk_strt + 33, lowByte());
  EEPROM.write(blk_strt + 34, );

  // Group #
  EEPROM.write(grp_strt + , );
  EEPROM.write(grp_strt + , highByte());
  EEPROM.write(grp_strt + , lowByte());
  EEPROM.write(grp_strt + , );
  EEPROM.write(grp_strt + , );
  */
}

