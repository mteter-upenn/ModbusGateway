#ifndef EXTERNALFRAM_h
#define EXTERNALFRAM_h

#include "Arduino.h"

#include <SPI.h>

#define SPI_EXTFRAM_SETTINGS SPISettings(24000000, MSBFIRST, SPI_MODE0)  // was 14000000
//#define SPI_EXTFRAM_SETTINGS SPISettings(8000000, MSBFIRST, SPI_MODE0)
//#define SPI_EXTFRAM_SETTINGS SPISettings(4000000, MSBFIRST, SPI_MODE0)

class ExtFRAMClass {
public:
  static uint8_t init();

};

extern ExtFRAMClass ExtFRAM;
#endif
