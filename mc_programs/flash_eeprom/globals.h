#ifndef GLOBALS_H
#define GLOBALS_H

#include "Arduino.h"
#define FRAM_SS_PIN

extern bool b_firstLoop;
extern bool b_quit;

void clearSerialRx();

uint32_t g_u32_nmStrt;
uint32_t g_u32_ipStrt;
uint32_t g_u32_ntpStrt;
uint32_t g_u32_serStrt;
uint32_t g_u32_slvStrt;
uint32_t g_u32_mapStrt;
uint32_t g_u32_curStrt;

#endif // GLOBALS_H
