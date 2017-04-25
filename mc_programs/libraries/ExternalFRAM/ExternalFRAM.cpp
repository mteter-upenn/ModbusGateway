
#include "ExternalFRAM.h"

#if defined(__arm__)
#include "SPIFIFO.h"
#ifdef  HAS_SPIFIFO
#define USE_SPIFIFO
#endif
#endif


#include "Arduino.h"


uint8_t ExtFRAMClass::init() {
  delay(200);  // unsure if necessary, copied from w5200

  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH);

#ifdef USE_SPIFIFO
  SPI.begin();
  SPIFIFO.begin(5, SPI_CLOCK_24MHz);  // 5 is pin for SS
#else
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  initSS();
#endif

  SPI.beginTransaction(SPI_EXTFRAM_SETTINGS);

  SPIFIFO.clear();
  SPIFIFO.write(0x9F);

  uint32_t u32_test;

  for (int ii = 0; ii < 4; ++ii) {
    u32_test = SPIFIFO.read();
    SPIFIFO.write(0);
    Serial.print(ii, DEC); Serial.print(": ");
    Serial.println(u32_test, DEC);
  }

  SPI.endTransaction();



  return 1;
}

