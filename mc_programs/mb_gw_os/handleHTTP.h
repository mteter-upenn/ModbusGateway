#ifndef HANDLEHTTP_H
#define HANDLEHTTP_H

#include "mb_names.h"
#include "globals.h"
#include <ModbusStack.h>


SockFlag readHttp(const uint8_t u8_socket, FileReq &u16_fileReq, FileType &s16_fileType, uint8_t &u8_selSlv, char ca_fileReq[REQUEST_LINE_SIZE]);
bool respondHttp(const uint8_t u8_socket, const SockFlag u16_sockFlag, const FileReq u16_fileReq, const FileType s16_fileType,  const uint8_t u8_selSlv, const char ca_fileReq[REQUEST_LINE_SIZE], ModbusStack &mbStack, uint8_t &u8_curGrp, float fa_liveXmlData[MAX_NUM_ELEC_VALS], int8_t s8a_dataFlags[MAX_NUM_ELEC_VALS]);

#endif // HANDLEHTTP_H
