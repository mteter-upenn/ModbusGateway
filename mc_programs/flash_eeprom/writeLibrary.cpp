#include "writeLibrary.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <SD.h>
#include <EEPROM.h>
#include <MeterLibrary.h>

uint16_t writeBlocks(uint16_t u16_mapIndStrt) {

  File jsonFile = SD.open("/maplist.jsn");

  // try using while loop, show_free_mem and delete[] to find minimum space needed
  //   (use switch case to select best size)
  const uint16_t u16_test = 40000;
  StaticJsonBuffer<u16_test> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(jsonFile);
  WriteMapsClass cls_writeMaps;
  Serial.print("JSON SIZE: "); Serial.println(jsonBuffer.size());

  return cls_writeMaps.writeMaps(root);

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
  */
}

