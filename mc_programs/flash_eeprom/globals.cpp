#include "globals.h"
#include <Arduino.h>

bool b_firstLoop = true;
bool b_quit = false;

void clearSerialRx() {
  while(Serial.available()){
    Serial.read();
  }
}
