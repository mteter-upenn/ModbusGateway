#ifndef HANDLEHTTP_H
#define HANDLEHTTP_H

#include "mb_names.h"
#include "globals.h"
#include <ModbusStack.h>


SockFlag readHttp(const uint8_t u8_socket, FileReq &u16_fileReq, FileType &s16_fileType, uint8_t &u8_selSlv, char ca_fileReq[gk_u16_requestLineSize]);
bool respondHttp(const uint8_t u8_socket, const SockFlag u16_sockFlag, const FileReq u16_fileReq, const FileType s16_fileType,  const uint8_t u8_selSlv, const char ca_fileReq[gk_u16_requestLineSize], ModbusStack &mbStack, uint8_t &u8_curGrp, float fa_liveXmlData[gk_i_maxNumElecVals], int8_t s8a_dataFlags[gk_i_maxNumElecVals]);

#endif // HANDLEHTTP_H
