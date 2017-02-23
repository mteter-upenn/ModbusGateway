#ifndef GLOBALS_H
#define GLOBALS_H

#include "Arduino.h"

extern bool b_firstLoop;
extern bool b_quit;

struct NameArray {
  char ca_name[32];
};

struct MacArray {
  uint8_t u8a_mac[6];
};

struct IpArray {
  uint8_t u8a_ip[4];
};

struct TypeArray {
  uint8_t u8a_type[3];
};

struct SlaveArray {
  uint8_t u8a_mtrType[3];
  uint8_t u8a_ip[4];
  uint8_t u8_id;
  uint8_t u8_vid;
};


void clearSerialRx();

#endif // GLOBALS_H
