/*
 * void flushEthRx
 * void send404
 * void sendBadSD
 * void sendWebFile
 * void LiveXML
 */

#include "secondaryHTTP.h"
#include "globals.h"
#include <Ethernet52.h>
#include <SD.h>
#include "mb_names.h"
#include "miscFuncs.h"
#include <MeterLibrary.h>
#include <Time.h>
#include <ModbusStructs.h>


// converts first line of http request to the file requested- overwrites char array, BE CAREFUL
void convertToFileName(char ca_fileReq[REQUEST_LINE_SIZE]) {
  uint16_t u16_strStart(0);
  char ca_dummy[REQUEST_LINE_SIZE];

  for (int ii = 0; ii < REQUEST_LINE_SIZE; ++ii) {
    if (ca_fileReq[ii] == 32) {
      u16_strStart = ii + 1;
      break;
    }
  }

  for (int ii = u16_strStart; ii < REQUEST_LINE_SIZE; ++ii) {
    switch (ca_fileReq[ii]) {
    case 32:  // space
    case 38:  // &
    case 63:  // ?
      ca_fileReq[ii] = 0;
      goto exitConvertForLoop;
      break;
    case 0:  // null char, string is done
      goto exitConvertForLoop;
      break;
    }
  }
exitConvertForLoop:

  strcpy(ca_dummy, ca_fileReq + u16_strStart);
  strcpy(ca_fileReq, ca_dummy);
  return;
}


void send404(EthernetClient52 &ec_client){
  char ca_resp404[44];

  strcpy_P(ca_resp404, PSTR("HTTP/1.1 404 Not Found\nConnnection: close\n\n"));
  ec_client.write(ca_resp404);

  ec_client.flush();
}


void sendBadSD(EthernetClient52 &ec_client){
  char ca_respBadSD[220];

  strcpy_P(ca_respBadSD, PSTR("HTTP/1.1 200 OK\nContent-Type: text/html\n")); // 41 with \0
  strcat_P(ca_respBadSD, PSTR("Connection: close\n\n"));  // 20 with \0

  ec_client.write(ca_respBadSD);

  strcpy_P(ca_respBadSD, PSTR("<!DOCTYPE html><html><head><title>"));// 35 with \0
  strcat(ca_respBadSD, g_gwName.ca_name);
  strcat_P(ca_respBadSD, PSTR("</title></head><body><p>The SD card for this gateway did not initialize properly.  "));// 84 with \0
  strcat_P(ca_respBadSD, PSTR("Website functionality is not available at this time.</p></body></html>"));// 71 with \0

  ec_client.write(ca_respBadSD);

  ec_client.flush();
}


void sendWebFile(EthernetClient52 &ec_client, const char* k_cp_fileName, FileType en_fileType, bool b_addFileLength) {
  File streamFile = SD.open(k_cp_fileName);

  if (streamFile) {
    uint32_t u32_fileSize;                                   // size of file being sent over tcp
    char ca_streamBuf[RESPONSE_BUFFER_SIZE];                       // buffer for moving data between sd card and ethernet

    u32_fileSize = streamFile.size();

    if (en_fileType != FileType::NONE) {
      uint32_t hdrLength;

      strcpy_P(ca_streamBuf, PSTR("HTTP/1.1 200 OK\nContent-Type: "));  // text / html\n"));
      switch (en_fileType) {
        case FileType::HTML:  // html
          strcat_P(ca_streamBuf, PSTR("text/html\n"));
          break;
        case FileType::CSS:  // css
          strcat_P(ca_streamBuf, PSTR("text/css\n"));
          break;
        case FileType::GIF:  // gif
          strcat_P(ca_streamBuf, PSTR("image/gif\n"));
          break;
        case FileType::XML:  // xml
          strcat_P(ca_streamBuf, PSTR("text/xml\n"));
          break;
        case FileType::CSV:  // csv - note, csvs for download are treated as NONE
          strcat_P(ca_streamBuf, PSTR("text/csv\n"));
          break;
        case FileType::TXT:
          strcat_P(ca_streamBuf, PSTR("text/plain\nContent-Disposition: attachment;\n"));
        default:
          break;
      }

      if (b_addFileLength) {  // xml files have extra bits tacked on 
        strcat_P(ca_streamBuf, PSTR("Connection: close\nContent - Length: "));
        hdrLength = strlen(ca_streamBuf);
        sprintf(ca_streamBuf + hdrLength, "%lu\n\n", u32_fileSize);
      }
      else {
        strcat_P(ca_streamBuf, PSTR("Connection: close\n\n"));
      }
      
      ec_client.write(ca_streamBuf);
    }

    uint16_t u16_maxIter;                                    // number of chunks file must be split into to send via tcp

    u16_maxIter = (u32_fileSize / RESPONSE_BUFFER_SIZE) + 1;

    for (int ii = 0; ii < u16_maxIter; ++ii) {
      uint32_t u32_remBytes;                                 // number of bytes remaining in last chunk (wfSize Mod STRM_BUF_SZ)
      uint16_t u16_bytesToSend;                                 // size of file buffer - min(remBytes, lclBufSz)

      u32_remBytes = u32_fileSize - (ii * RESPONSE_BUFFER_SIZE);  // might be able to get rid of this as well, just use STRM_BUF_SZ, read should spit out early

      u16_bytesToSend = (u32_remBytes < RESPONSE_BUFFER_SIZE) ? u32_remBytes : RESPONSE_BUFFER_SIZE;
      
      streamFile.read(ca_streamBuf, u16_bytesToSend);  // expect speed increase
      
      ec_client.write(ca_streamBuf, u16_bytesToSend);
    }

    ec_client.flush();
    streamFile.close();
  }
  else {
    send404(ec_client);
  }

#if SHOW_FREE_MEM
  Serial.print(F("sendWebFile mem: "));
  Serial.println(getFreeMemory());
#endif
}


void sendDownLinks(EthernetClient52 &ec_client, const char *const k_ckp_firstLine) {
  uint16_t u16_dirNameLen;
  const uint16_t k_u16_truncReqLineSz = REQUEST_LINE_SIZE - 4;
  char ca_dirName[k_u16_truncReqLineSz];
  
  // request for folder contents looks like this:
  // GET /pastdown.htm/PASTDATA/2016/07   or
  // GET /pastdown.htm

  strcpy(ca_dirName, k_ckp_firstLine);  // +4 removed ("GET ")
  
  if (strstr(ca_dirName, "PASTDATA") == nullptr) {  // can't find PASTDATA
    // add /PASTDATA, don't want to look in higher folders than this
    strcat_P(ca_dirName, PSTR("/PASTDATA"));
  }

  u16_dirNameLen = strlen(ca_dirName);

  if (ca_dirName[u16_dirNameLen - 1] != '/') {  // if the last character is not a /, add one
    strcat(ca_dirName, "/");  // not sure if trailing '/' will screw up .open()
    u16_dirNameLen++;
  }

  File dir = SD.open(ca_dirName + 13);  // do not include "/pastdown.htm" which is 13 chars long

  if (dir) {
    char ca_streamBuf[RESPONSE_BUFFER_SIZE] = { 0 };                       // buffer for moving data to ethernet

    dir.rewindDirectory();  // resets library to top of directory

    if (u16_dirNameLen > 24) {  // if in directory lower than /PASTDATA, allow ../ option
      char ca_dirNameRed[k_u16_truncReqLineSz];

      strcpy(ca_dirNameRed, ca_dirName);
      for (int ii = (u16_dirNameLen - 2); ii > 1; --ii) {
        if (ca_dirNameRed[ii] == '/') {
          ca_dirNameRed[ii + 1] = 0;
          break;
        }
      }
      strcat_P(ca_streamBuf, PSTR("<a href=\""));
      strcat(ca_streamBuf, ca_dirNameRed);
      strcat_P(ca_streamBuf, PSTR("\">../</a></br>"));
    }
    
    while (true) {
      const uint16_t k_u16_minRemBytes(300);  // longest line should be ~119 bytes, give bit more than twice padding
      File entry = dir.openNextFile();
      
      if (!entry) {
        break; 
      }

      if (entry.isDirectory()) {
        // write <a href="/pastdown.htm/CUR_DIR_NAME/DIR_NAME">DIR_NAME</a></br>
        strcat_P(ca_streamBuf, PSTR("<a href=\""));  // 9
        strcat(ca_streamBuf, ca_dirName);
        strcat(ca_streamBuf, entry.name());
        strcat_P(ca_streamBuf, PSTR("\">"));  // 2
        strcat(ca_streamBuf, entry.name());
        strcat_P(ca_streamBuf, PSTR("/</a></br>"));  // 10
      }
      else {
        // write <a href="CUR_DIR_NAME/FILE_NAME" download="GW_NAME_FILE_NAME">FILE_NAME</a></br>
        strcat_P(ca_streamBuf, PSTR("<a href=\""));  // 9
        strcat(ca_streamBuf, ca_dirName + 13);  // current directory
        strcat(ca_streamBuf, entry.name());  // file
        strcat_P(ca_streamBuf, PSTR("\" download=\""));  // 12
        strcat(ca_streamBuf, g_gwName.ca_name);  // gw name, up to 32
        strcat_P(ca_streamBuf, PSTR("_"));  // 1
        strcat(ca_streamBuf, entry.name());  // file
        strcat_P(ca_streamBuf, PSTR("\">"));  // 2
        strcat(ca_streamBuf, entry.name());  // file
        strcat_P(ca_streamBuf, PSTR("</a></br>"));  // 9
      }

      entry.close();

      // if remaining room is less than desired, send message 
      if ((RESPONSE_BUFFER_SIZE - strlen(ca_streamBuf)) < k_u16_minRemBytes) {  
        ec_client.write(ca_streamBuf);
        ec_client.flush();

        ca_streamBuf[0] = 0;
      }
    }
    ec_client.write(ca_streamBuf);
    dir.close();
  }
}


void sendXmlEnd(EthernetClient52 &ec_client, XmlFile en_xmlType) {
  char ca_buffer[32];

  ca_buffer[0] = 0;

  switch (en_xmlType) {
    case XmlFile::GENERAL: // gensetup.xml
      if (g_b_rtcGood) {
        time_t t = now();

        strcat_P(ca_buffer, PSTR("<rtc>"));
        sprintf(ca_buffer + 5, "%lu", t);
        strcat_P(ca_buffer, PSTR("</rtc>"));
      }

      strcat_P(ca_buffer, PSTR("</setup>"));
      ec_client.write(ca_buffer);
    default:
      break;
  }
}

void sendIP(EthernetClient52 &ec_client) {
  int jj;
  uint8_t u8_ipByte;
  char ca_postResp[15];  // largest that a ip address could be in string form

  jj = 0;
  u8_ipByte = g_ip_ip.u8a_ip[0];
  for (int ii = log10(u8_ipByte); ii > -1; --ii) {
    ca_postResp[jj] = u8_ipByte / pow(10, ii) + '0';
    u8_ipByte -= (ca_postResp[jj] - '0') * pow(10, ii);
    jj++;
  }

  for (int kk = 1; kk < 4; ++kk) {
    ca_postResp[jj] = '.';
    jj++;

    u8_ipByte = g_ip_ip.u8a_ip[kk];
    for (int ii = log10(u8_ipByte); ii > -1; --ii) {
      ca_postResp[jj] = u8_ipByte / pow(10, ii) + '0';
      u8_ipByte -= (ca_postResp[jj] - '0') * pow(10, ii);
      jj++;
    }
  }

  ec_client.write(ca_postResp, jj);
}


void liveXML(uint8_t u8_socket, uint8_t u8_selSlv,float fa_data[MAX_NUM_ELEC_VALS], int8_t s8a_dataFlags[MAX_NUM_ELEC_VALS]) {  // sends xml file of live meter data
  const uint16_t k_u16_minRemBytes(50);
  char ca_respXml[RESPONSE_BUFFER_SIZE] = {0};  // 68

  strcat_P(ca_respXml, PSTR("<?xml version = \"1.0\" ?><inputs><has_data>true</has_data>"));

  if ((SlaveData[u8_selSlv].u8a_mtrType[0] == 11) || (SlaveData[u8_selSlv].u8a_mtrType[0] == 12)){
    for (int ii = 0; ii < MAX_NUM_STMCHW_VALS; ++ii) { 
      if (ii < 3) strcat_P(ca_respXml, PSTR("<flow>"));
      else if (ii < 4) strcat_P(ca_respXml, PSTR("<tmp1>"));
      else if (ii < 5) strcat_P(ca_respXml, PSTR("<tmp2>"));
      else if (ii < 6) strcat_P(ca_respXml, PSTR("<dlta>"));
      else if (ii < 7) strcat_P(ca_respXml, PSTR("<pres>"));
      else strcat_P(ca_respXml, PSTR("<engy>"));

      switch (s8a_dataFlags[ii]){
        case 1:  // good data
            dtostrf(fa_data[ii], 1, 2, (ca_respXml + strlen(ca_respXml)));
          break;
        case 0:  // data does not exist on hardware
          strcat_P(ca_respXml, PSTR("&#8212;"));
          break;
        case (-1):  // no modbus return
          strcat_P(ca_respXml, PSTR("error"));
          break;
        case (-2):
          strcat_P(ca_respXml, PSTR("nan"));
          break;
        case (-3):
          strcat_P(ca_respXml, PSTR("inf"));
          break;
        default:
          break;
      }    

      if (ii < 3) strcat_P(ca_respXml, PSTR("</flow>")); 
      else if (ii < 4) strcat_P(ca_respXml, PSTR("</tmp1>"));
      else if (ii < 5) strcat_P(ca_respXml, PSTR("</tmp2>"));
      else if (ii < 6) strcat_P(ca_respXml, PSTR("</dlta>"));
      else if (ii < 7) strcat_P(ca_respXml, PSTR("</pres>"));
      else strcat_P(ca_respXml, PSTR("</engy>"));

      if ((RESPONSE_BUFFER_SIZE - strlen(ca_respXml)) < k_u16_minRemBytes) {
        g_eca_socks[u8_socket].write(ca_respXml);  // write line of xml response every other time (stacks 2 together)
        ca_respXml[0] = 0;  // reset respXml to 0 length so strcat starts at beginning
      }
    }
  }
  else {  // electric meter
    for (int ii = 0; ii < MAX_NUM_ELEC_VALS; ++ii) {
      if (ii < 5){strcat_P(ca_respXml, PSTR("<curr>"));}
      else if (ii < 9){strcat_P(ca_respXml, PSTR("<v_LN>"));}
      else if (ii < 13){strcat_P(ca_respXml, PSTR("<v_LL>"));}
      else if (ii < 17){strcat_P(ca_respXml, PSTR("<rl_p>"));}
      else if (ii < 21){strcat_P(ca_respXml, PSTR("<rc_p>"));}
      else if (ii < 25){strcat_P(ca_respXml, PSTR("<ap_p>"));}
      else if (ii < 29){strcat_P(ca_respXml, PSTR("<p_fc>"));}
      else{strcat_P(ca_respXml, PSTR("<engy>"));}

      switch (s8a_dataFlags[ii]){
        case 1:  // good data
            dtostrf(fa_data[ii], 1, 2, (ca_respXml + strlen(ca_respXml)));
          break;
        case 0:  // data does not exist on hardware
          strcat_P(ca_respXml, PSTR("&#8212;"));  // long dash
          break;
        case (-1):  // no modbus return
          strcat_P(ca_respXml, PSTR("error"));
          break;
        case (-2):
          strcat_P(ca_respXml, PSTR("nan"));
          break;
        case (-3):
          strcat_P(ca_respXml, PSTR("inf"));
          break;
        default:
          break;
      }    


      if (ii < 5){strcat_P(ca_respXml, PSTR("</curr>"));}
      else if (ii < 9){strcat_P(ca_respXml, PSTR("</v_LN>"));}
      else if (ii < 13){strcat_P(ca_respXml, PSTR("</v_LL>"));}
      else if (ii < 17){strcat_P(ca_respXml, PSTR("</rl_p>"));}
      else if (ii < 21){strcat_P(ca_respXml, PSTR("</rc_p>"));}
      else if (ii < 25){strcat_P(ca_respXml, PSTR("</ap_p>"));}
      else if (ii < 29){strcat_P(ca_respXml, PSTR("</p_fc>"));}
      else{strcat_P(ca_respXml, PSTR("</engy>"));}


      if ((RESPONSE_BUFFER_SIZE - strlen(ca_respXml)) < k_u16_minRemBytes) {  // if buffer close to overflowing, send existing
        g_eca_socks[u8_socket].write(ca_respXml);  // write line of xml response
        ca_respXml[0] = 0;  // reset respXml to 0 length
      }
    }
  }

  strcat_P(ca_respXml, PSTR("</inputs>"));

  g_eca_socks[u8_socket].write(ca_respXml);
  g_eca_socks[u8_socket].flush();
}


