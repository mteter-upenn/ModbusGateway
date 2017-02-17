#include "writeLibrary.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <SD.h>
#include <EEPROM.h>
#include <MeterLibrary.h>

uint16_t writeBlocks(uint16_t u16_mapIndStrt) {

  File jsonFile = SD.open("/maplist5.jsn");

  // try using while loop, show_free_mem and delete[] to find minimum space needed
  //   (use switch case to select best size)
  const uint16_t u16_test = 40000;
  StaticJsonBuffer<u16_test> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(jsonFile);
  WriteMaps cls_writeMaps;
  Serial.print("SIZE: "); Serial.println(jsonBuffer.size());

  return cls_writeMaps.writeMaps(root);

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

