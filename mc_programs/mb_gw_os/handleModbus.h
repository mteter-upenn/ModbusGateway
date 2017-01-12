#ifndef HANDLEMODBUS_H
#define HANDLEMODBUS_H

#include "globals.h"

uint8_t getModbus(uint8_t u8a_mbReq[gk_u16_mbArraySize], uint16_t u16_mbReqLen, uint8_t u8a_mbResp[gk_u16_mbArraySize], uint16_t &u16_mbRespLen, bool b_byteSwap);
void handle_modbus(bool b_idleHttp);

#endif // HANDLEMODBUS_H
