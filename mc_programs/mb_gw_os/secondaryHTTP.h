#ifndef SECONDARYHTTP_H
#define SECONDARYHTTP_H

#include "globals.h"
#include <Ethernet52.h>
#include "mb_names.h"

void convertToFileName(char ca_fileReq[gk_u16_requestLineSize]);
void send404(EthernetClient52 &ec_client);
void sendBadSD(EthernetClient52 &ec_client);
void sendWebFile(EthernetClient52 &ec_client, const char* ccp_fileName, FileType en_fileType = FileType::NONE, bool b_addFileLength = true);
void sendDownLinks(EthernetClient52 &ec_client, const char *const cp_firstLine);
void sendXmlEnd(EthernetClient52 &ec_client, XmlFile en_xmlType);
void sendIP(EthernetClient52 &ec_client);
void liveXML(uint8_t u8_socket, uint8_t u8_selSlv, float fa_data[gk_i_maxNumElecVals], int8_t s8a_dataFlags[gk_i_maxNumElecVals]);

#endif // SECONDARYHTTP_H
