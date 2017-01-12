#ifndef TERTIARYHTTP_H
#define TERTIARYHTTP_H

#include <Ethernet52.h>

void sendPostResp(EthernetClient52 &ec_client);
char* preprocPost(EthernetClient52 &ec_client, char *cp_httpHdr, uint16_t &u16_postLen);
void getPostSetupData(EthernetClient52 &ec_client);

#endif // TERTIARYHTTP_H
