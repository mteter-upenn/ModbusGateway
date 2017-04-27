#include "globals.h"
#include <Arduino.h>

bool b_firstLoop = true;
bool b_quit = false;

void clearSerialRx() {
  while(Serial.available()){
    Serial.read();
  }
}

/*
 * name...................... @ g_u32_nmStrt
 *
 * mac....................... @ g_u32_ipStrt
 * ip........................ @ g_u32_ipStrt +  6
 * subnet mask............... @ g_u32_ipStrt + 10
 * defautl gateway........... @ g_u32_ipStrt + 14
 * tcp socket timeout........ @ g_u32_ipStrt + 18
 *
 * use ntp?.................. @ g_u32_ntpStrt
 * ntp ip.................... @ g_u32_ntpStrt + 1
 *
 * baud rate................. @ g_u32_serStrt
 * data bits................. @ g_u32_serStrt + 4
 * parity.................... @ g_u32_serStrt + 5
 * stop bits................. @ g_u32_serStrt + 6
 * modbus timeout............ @ g_u32_serStrt + 7
 * print comms to sd?........ @ g_u32_serStrt + 9
 *
 * number of slaves connected @ g_u32_slvStrt
 * polling interval.......... @ g_u32_slvStrt + 1
 * historical data interval.. @ g_u32_slvStrt + 5
 */
