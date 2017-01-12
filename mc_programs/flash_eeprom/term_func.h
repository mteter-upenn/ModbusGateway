#ifndef TERM_FUNC_H
#define TERM_FUNC_H

#include <Arduino.h>

bool term_func(const __FlashStringHelper *, bool(*argFunc)(char*), const __FlashStringHelper *,
  const __FlashStringHelper *, char *, const char *, bool, uint8_t, bool);
//const __FlashStringHelper *

#endif // TERM_FUNC_H
