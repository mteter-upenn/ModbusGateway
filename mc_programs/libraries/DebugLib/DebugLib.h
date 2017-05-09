#ifndef DebugLib_h
#define DebugLib_h


/* _____STANDARD INCLUDES____________________________________________________ */
// include types & constants of Wiring core API
#include "Arduino.h"

/* _____UTILITY MACROS_______________________________________________________ */


/* _____PROJECT INCLUDES_____________________________________________________ */
#include <Time.h>
#include <SD.h>

/* _____CLASS DEFINITIONS____________________________________________________ */


class DebugLibClass {
private:
  static bool ms_b_printComms;
public:
  static void setPrintComms(bool b_printComms);

  static void digitalClockDisplaySerial(time_t t);
  static void printDigitsSerial(int digits);
  static void print3SpaceDigitsSerial(uint8_t num);
  static void print3SpaceDigitsSD(File sdFile, uint8_t num);
  static void storeStringAndArr(const char *k_cp_string, uint8_t *u8p_arr, uint16_t u16_arrLen, uint16_t u16_unqId, uint8_t u8_sock, bool b_showTime);
  static void storeSockets(const char *k_cp_msg);
};


#endif
