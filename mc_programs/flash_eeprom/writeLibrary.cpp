#include "writeLibrary.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <SD.h>
#include <EEPROM.h>
#include "meters.h"
#include <MeterLibrary.h>

#define VIAJSON 0
#define LIMITNUMMAPS 0

uint16_t writeBlocks(uint16_t u16_mapIndStrt) {

#if VIAJSON
  File jsonFile = SD.open("/maplist.jsn");

  // try using while loop, show_free_mem and delete[] to find minimum space needed
  //   (use switch case to select best size)
  const uint16_t u16_test = 20000;
  StaticJsonBuffer<u16_test> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(jsonFile);
  WriteMaps cls_writeMaps;

//  if (!root.success()) {
//    Serial.println("Failed to access maplist.jsn!");
//    return u16_mapIndStrt;
//  }

//  uint16_t u16_numMaps = root["meterlist"].size();

//  uint16_t u16_mapStart = u16_mapIndStrt + 3 + u16_numMaps * 4;

//  for (int ii = 0; ii < u16_numMaps; ++ii) {
//    EEPROM.write(u16_mapIndStrt + 3 + ii * 4, highByte(u16_mapStart));  // 117 -> 146
//    EEPROM.write(u16_mapIndStrt + 4 + ii * 4, lowByte(u16_mapStart));
//    EEPROM.write(u16_mapIndStrt + 5 + ii * 4, ii + 1);  // meter number
//    EEPROM.write(u16_mapIndStrt + 6 + ii * 4, 3);  // function
//  }


  return cls_writeMaps.writeMaps(root);
#else

  // OLD CODE STARTS HERE #############################################################################################
  //uint16_t indMtrStrt;
  const uint8_t k_u8_numMaps(3);
  uint16_t u16_mapStart;
  //  EEPROM.write(bt_strt, 2);  // current meter type  default is 2.1.0
  //  EEPROM.write(bt_strt + 1, 1);
  //  EEPROM.write(bt_strt + 2, 0);

  EEPROM.write(u16_mapIndStrt + 0, highByte(u16_mapIndStrt));  // THIS IS USELESS
  EEPROM.write(u16_mapIndStrt + 1, lowByte(u16_mapIndStrt));  // address of meter addresses
  EEPROM.write(u16_mapIndStrt + 2, k_u8_numMaps);  // number of meters adjust when adding new meters AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA



                                   // 6 + blknum * 5 + grpnum * 3 + 32 * 2  
//#if defined(CORE_TEENSY)  // if teensy3.0 or greater
//  indMtrStrt = 2048;
//#else
//  indMtrStrt = 4096;
//#endif

  //// New Group #1, +0
  //EEPROM.write(grp_strt, );                 // number of values
  //EEPROM.write(++grp_strt, );              // number of registers
  //EEPROM.write(++grp_strt, highByte());  // starting register
  //EEPROM.write(++grp_strt, lowByte());   // starting register
  //EEPROM.write(++grp_strt, );              // skip bytes to data types

  // MAKE IT SO INDEXING INCREMENTS INSTEAD OF DECREMENT
  // AS LONG AS THE STARTING POINT IS MORE THAN THE FOLLOWING INDEXING, IT SHOULD BE OK 
  // TAKE NUMBER of MAPS (15) *4 to get length
  // can then return next open eeprom address in each function.
  // eaton power xpert 4000    #1
  //indMtrStrt -= 83;
  //meter1(indMtrStrt);
  u16_mapStart = u16_mapIndStrt + 3 + k_u8_numMaps * 4;
  EEPROM.write(u16_mapIndStrt + 3, highByte(u16_mapStart));  // 117 -> 146
  EEPROM.write(u16_mapIndStrt + 4, lowByte(u16_mapStart));
  EEPROM.write(u16_mapIndStrt + 5, 1);  // meter number
  EEPROM.write(u16_mapIndStrt + 6, 3);  // function
  u16_mapStart = meter1(u16_mapStart);

// emon dmon    #2
  //indMtrStrt -= 61;
  //meter2(indMtrStrt);
  EEPROM.write(u16_mapIndStrt + 7, highByte(u16_mapStart));  // 95 -> 150
  EEPROM.write(u16_mapIndStrt + 8, lowByte(u16_mapStart));
  EEPROM.write(u16_mapIndStrt + 9, 2);  // meter number
  EEPROM.write(u16_mapIndStrt + 10, 3);  // function
  u16_mapStart = meter2(u16_mapStart);

// ge epm 3720    #3
  //indMtrStrt -= 65;
  //meter3(indMtrStrt);
  EEPROM.write(u16_mapIndStrt + 11, highByte(u16_mapStart));  // 101 -> 150
  EEPROM.write(u16_mapIndStrt + 12, lowByte(u16_mapStart));
  EEPROM.write(u16_mapIndStrt + 13, 3);  // meter number
  EEPROM.write(u16_mapIndStrt + 14, 3);  // function
  u16_mapStart = meter3(u16_mapStart);

#if LIMITNUMMAPS
  // ge pqm    #4
  //indMtrStrt -= 0;
  //meter4(0);
  EEPROM.write(u16_mapIndStrt + 15, highByte(0));  // assume 400
  EEPROM.write(u16_mapIndStrt + 16, lowByte(0));
  EEPROM.write(u16_mapIndStrt + 17, 4);  // meter number
  EEPROM.write(u16_mapIndStrt + 18, 3);  // function
  //u16_mapStart = meter4(u16_mapStart);


  // siemens 9330, 9350, 9360    #5
  //indMtrStrt -= 71;
  //meter5(indMtrStrt);
  EEPROM.write(u16_mapIndStrt + 19, highByte(u16_mapStart));  // 103 -> 150
  EEPROM.write(u16_mapIndStrt + 20, lowByte(u16_mapStart));
  EEPROM.write(u16_mapIndStrt + 21, 5);  // meter number
  EEPROM.write(u16_mapIndStrt + 22, 3);  // function
  u16_mapStart = meter5(u16_mapStart);

// siemens 9510, 9610    #6
  //indMtrStrt -= 84;
  //meter6(indMtrStrt);
  EEPROM.write(u16_mapIndStrt + 23, highByte(u16_mapStart));  // 126 -> 175
  EEPROM.write(u16_mapIndStrt + 24, lowByte(u16_mapStart));
  EEPROM.write(u16_mapIndStrt + 25, 6);  // meter number
  EEPROM.write(u16_mapIndStrt + 26, 3);  // function
  u16_mapStart = meter6(u16_mapStart);

// squareD cm2350   #7
  //indMtrStrt -= 96;
  //meter7(indMtrStrt);
  EEPROM.write(u16_mapIndStrt + 27, highByte(u16_mapStart));  // 126 -> 175
  EEPROM.write(u16_mapIndStrt + 28, lowByte(u16_mapStart));
  EEPROM.write(u16_mapIndStrt + 29, 7);  // meter number
  EEPROM.write(u16_mapIndStrt + 30, 3);  // function
  u16_mapStart = meter7(u16_mapStart);

// squareD pm710   #8
  //indMtrStrt -= 65;
  //meter8(indMtrStrt);
  EEPROM.write(u16_mapIndStrt + 31, highByte(u16_mapStart));  // 97 -> 150
  EEPROM.write(u16_mapIndStrt + 32, lowByte(u16_mapStart));
  EEPROM.write(u16_mapIndStrt + 33, 8);  // meter number
  EEPROM.write(u16_mapIndStrt + 34, 3);  // function
  u16_mapStart = meter8(u16_mapStart);

// squareD micrologic a, p, h trip units    #9
  //indMtrStrt -= 86;
  //meter9(indMtrStrt);
  EEPROM.write(u16_mapIndStrt + 35, highByte(u16_mapStart));  // 116 -> 150
  EEPROM.write(u16_mapIndStrt + 36, lowByte(u16_mapStart));
  EEPROM.write(u16_mapIndStrt + 37, 9);  // meter number
  EEPROM.write(u16_mapIndStrt + 38, 3);  // function
  u16_mapStart = meter9(u16_mapStart);

// squareD cm3350, cm4000 series, pm800 series    #10
  //indMtrStrt -= 82;
  //meter10(indMtrStrt);
  EEPROM.write(u16_mapIndStrt + 39, highByte(u16_mapStart));  // 111 -> 150
  EEPROM.write(u16_mapIndStrt + 40, lowByte(u16_mapStart));
  EEPROM.write(u16_mapIndStrt + 41, 10);  // meter number
  EEPROM.write(u16_mapIndStrt + 42, 3);  // function
  u16_mapStart = meter10(u16_mapStart);

// Chilled water KEP    #11
  //indMtrStrt -= 54;
  //meter11(indMtrStrt);
  EEPROM.write(u16_mapIndStrt + 43, highByte(u16_mapStart));
  EEPROM.write(u16_mapIndStrt + 44, lowByte(u16_mapStart));
  EEPROM.write(u16_mapIndStrt + 45, 11);  // meter number
  EEPROM.write(u16_mapIndStrt + 46, 3);  // function
  u16_mapStart = meter11(u16_mapStart);

// Steam KEP    #12
  //indMtrStrt -= 55;
  //meter12(indMtrStrt);
  EEPROM.write(u16_mapIndStrt + 47, highByte(u16_mapStart));  // 69 -> 100
  EEPROM.write(u16_mapIndStrt + 48, lowByte(u16_mapStart));
  EEPROM.write(u16_mapIndStrt + 49, 12);  // meter number
  EEPROM.write(u16_mapIndStrt + 50, 3);  // function
  u16_mapStart = meter12(u16_mapStart);

// SquareD PM210  13
  //indMtrStrt -= 65;
  //meter13(indMtrStrt);
  EEPROM.write(u16_mapIndStrt + 51, highByte(u16_mapStart));  // 97 -> 150
  EEPROM.write(u16_mapIndStrt + 52, lowByte(u16_mapStart));
  EEPROM.write(u16_mapIndStrt + 53, 13);  // meter number
  EEPROM.write(u16_mapIndStrt + 54, 3);  // function
  u16_mapStart = meter13(u16_mapStart);

// Siemens Pac4200/3200 14
  //indMtrStrt -= 95;
  //meter14(indMtrStrt);
  EEPROM.write(u16_mapIndStrt + 55, highByte(u16_mapStart));  //  -> 150
  EEPROM.write(u16_mapIndStrt + 56, lowByte(u16_mapStart));
  EEPROM.write(u16_mapIndStrt + 57, 14);  // meter number
  EEPROM.write(u16_mapIndStrt + 58, 3);  // function
  u16_mapStart = meter14(u16_mapStart);

// Eaton Series PXM 2000 15
  //indMtrStrt -= 94;
  //meter15(indMtrStrt);
  EEPROM.write(u16_mapIndStrt + 59, highByte(u16_mapStart));  //  -> 150
  EEPROM.write(u16_mapIndStrt + 60, lowByte(u16_mapStart));
  EEPROM.write(u16_mapIndStrt + 61, 15);  // meter number
  EEPROM.write(u16_mapIndStrt + 62, 3);  // function
  u16_mapStart = meter15(u16_mapStart);

  // Schneider PM5300 16
  EEPROM.write(u16_mapIndStrt + 63, highByte(u16_mapStart));  //  -> 150
  EEPROM.write(u16_mapIndStrt + 64, lowByte(u16_mapStart));
  EEPROM.write(u16_mapIndStrt + 65, 16);  // meter number
  EEPROM.write(u16_mapIndStrt + 66, 3);  // function
  u16_mapStart = meter16(u16_mapStart);

  // Schneider PM8000 17
  //EEPROM.write(u16_mapIndStrt + 67, highByte(u16_mapStart));  //  -> 150
  //EEPROM.write(u16_mapIndStrt + 68, lowByte(u16_mapStart));
  //EEPROM.write(u16_mapIndStrt + 69, 17);  // meter number
  //EEPROM.write(u16_mapIndStrt + 70, 3);  // function
  //u16_mapStart = meter17(u16_mapStart);

#endif
  Serial.print("Meter register library starts at: ");
  Serial.println(u16_mapIndStrt + 3 + k_u8_numMaps * 4);

  return (u16_mapStart);  // returns end of map libraries
#endif
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

