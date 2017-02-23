#ifndef TERM_FUNC_H
#define TERM_FUNC_H

#include <Arduino.h>

bool term_func(const __FlashStringHelper *, bool(*argFunc)(char*), const __FlashStringHelper *,
  const __FlashStringHelper *, char *, const char *, bool, uint8_t, bool);
//const __FlashStringHelper *

void checkDefault(char *cp_input, const char *kcp_defaultInput);
bool checkQuit(char *cp_input);
bool verFunc(char *cp_input);
bool menuFunc(char *cp_input);
bool nmFunc(char *cp_input);
bool macFunc(char *cp_input);
bool ipFunc(char *cp_input);
bool brFunc(char *cp_input);
bool dbFunc(char *cp_input);
bool parFunc(char *cp_input);
bool sbFunc(char *cp_input);
bool toFunc(char *cp_input);
bool mtrnumFunc(char *cp_input);
bool mtrtypFunc(char *cp_input);
bool mbidFunc(char *cp_input);
void storeName(char *cp_input, uint16_t u16_regStrt);
void storeIP(char *cp_input, uint16_t u16_regStrt, uint8_t u8_elmts);
bool storeBool(char *cp_input, uint16_t u16_regStrt);
uint8_t storeByte(char *cp_input, uint16_t u16_regStrt);
uint16_t storeShortInt(char *cp_input, uint16_t u16_regStrt);
uint32_t storeInt(char *cp_input, uint16_t u16_regStrt);


#endif // TERM_FUNC_H
