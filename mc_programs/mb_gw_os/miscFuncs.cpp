/*
 * void setConstants
 * void writeGenSetup
 * void writeMeterSetup
 */

#include <SD.h>
#include <EEPROM.h>
#include "globals.h"
#include "miscFuncs.h"
#include <MeterLibrary.h>

#if SHOW_FREE_MEM
int __bss_end;
void *__brkval;

int getFreeMemory()
{
  int free_memory;

  if((int)__brkval == 0)
    free_memory = ((int)&free_memory) - ((int)&__bss_end);
  else
    free_memory = ((int)&free_memory) - ((int)__brkval);

  return free_memory;
}
#endif

void resetArd() {
//   Serial.println("resetting...");
  CPU_RESTART
  delay(20);
}

void setConstants() {
  for (int ii = 0; ii < 30; ++ii){
    g_c_gwName[ii] = (char)EEPROM.read(ii + g_u16_nameBlkStart);

    if (g_c_gwName[ii] == 0){
      break;
    }
  }
  g_c_gwName[31] = 0;  // doublecheck to make sure string ends in null
  Serial.print(F("name: "));
  Serial.println(g_c_gwName);
  
  g_b_recordData = EEPROM.read(g_u16_nameBlkStart + 32);  // whether or not to record data
  g_u8_maxRecordSlaves = EEPROM.read(g_u16_nameBlkStart + 33) > 20 ? 5 : EEPROM.read(g_u16_nameBlkStart + 33);  // max slaves to record

  for (int ii = 0; ii < 6; ++ii){
    g_u8a_mac[ii] = EEPROM.read(g_u16_ipBlkStart + ii);
  }

  for (int ii = 0; ii < 4; ++ii){
    g_ip_ip[ii] = EEPROM.read(ii + g_u16_ipBlkStart + 6);
    g_ip_subnet[ii] = EEPROM.read(ii + g_u16_ipBlkStart + 10);
    g_ip_gateway[ii] = EEPROM.read(ii + g_u16_ipBlkStart + 14);

    g_ip_ntpIp[ii] = EEPROM.read(ii + g_u16_ipBlkStart + 19);
  }

  g_b_useNtp = EEPROM.read(g_u16_ipBlkStart + 18);

  g_u32_baudrate = EEPROM.read(g_u16_ipBlkStart + 24);
  g_u32_baudrate = (uint32_t)((g_u32_baudrate << 16) | (EEPROM.read(g_u16_ipBlkStart + 25) << 8) | (EEPROM.read(g_u16_ipBlkStart + 26)));

  g_u8_dataBits = EEPROM.read(g_u16_ipBlkStart + 27);
  g_u8_parity = EEPROM.read(g_u16_ipBlkStart + 28);
  g_u8_stopBits = EEPROM.read(g_u16_ipBlkStart + 29);

  g_u16_timeout = word(EEPROM.read(g_u16_ipBlkStart + 30), EEPROM.read(g_u16_ipBlkStart + 31));

//  Serial.print("baud: "); Serial.print(g_u32_baudrate);
//  Serial.print(" at address "); Serial.println(g_u16_ipBlkStart + 23);

  //g_u8_numSlaves = EEPROM.read(g_u16_mtrBlkStart);

  //for (int ii = 0; ii < g_u8_numSlaves; ++ii){
  //  //g_u8a_slaveIds[ii] = EEPROM.read(9 * ii + 8 + g_u16_mtrBlkStart);
  //  g_u8a_slaveVids[ii] = EEPROM.read(9 * ii + 9 + g_u16_mtrBlkStart);

  //  g_u8a_slaveIps[ii][0] = EEPROM.read(9 * ii + 4 + g_u16_mtrBlkStart);
  //  for (int jj = 1; jj < 4; ++jj){
  //    g_u8a_slaveIps[ii][jj] = EEPROM.read(9 * ii + g_u16_mtrBlkStart + jj + 4);
  //    g_u8a_slaveTypes[ii][(jj - 1)] = EEPROM.read(9 * ii + g_u16_mtrBlkStart + jj);
  //  }
  //}
}


void writeGenSetupFile(){
  File webFile;

  digitalWrite(gk_s16_sdWriteLed, HIGH);

  SD.remove("gensetup.xml");
  webFile = SD.open("gensetup.xml", FILE_WRITE);
  
  webFile.print(F("<?xml version = \"1.0\" ?><setup><name>"));
//  for (int ii = 0; ii < 32; ++ii){
//    if (g_c_gwName[ii] == 0){
//      break;
//    }
//    webFile.print(g_c_gwName[ii]);
//  }
  webFile.print(g_c_gwName);
  
  webFile.print(F("</name><rd>"));
  if (g_b_recordData) {
    webFile.print(F("true"));
  }
  else {
    webFile.print(F("false"));
  }

  webFile.print(F("</rd><mxslvs>"));
  webFile.print(g_u8_maxRecordSlaves, DEC);

  webFile.print(F("</mxslvs><mac>"));
  if (g_u8a_mac[0] < 16) {
    webFile.print('0');
  }
  webFile.print(g_u8a_mac[0], HEX);
  for (int ii = 1; ii < 6; ++ii){
    webFile.print(F(":"));
    if (g_u8a_mac[ii] < 16){
      webFile.print('0');
    }
    webFile.print(g_u8a_mac[ii], HEX);
  }
  
  webFile.print(F("</mac><ip>"));
  webFile.print(g_ip_ip[0], DEC);
  for (int ii = 1; ii < 4; ++ii){
    webFile.print(F("."));
    webFile.print(g_ip_ip[ii], DEC);
  }
  
  webFile.print(F("</ip><sm>"));
  webFile.print(g_ip_subnet[0], DEC);
  for (int ii = 1; ii < 4; ++ii) {
    webFile.print(F("."));
    webFile.print(g_ip_subnet[ii], DEC);
  }
  
  webFile.print(F("</sm><gw>"));
  webFile.print(g_ip_gateway[0], DEC);
  for (int ii = 1; ii < 4; ++ii) {
    webFile.print(F("."));
    webFile.print(g_ip_gateway[ii], DEC);
  }
  
  webFile.print(F("</gw><ntp>"));
  if (g_b_useNtp) {
    webFile.print(F("true"));
  }
  else {
    webFile.print(F("false"));
  }

  webFile.print(F("</ntp><nip>"));
  webFile.print(g_ip_ntpIp[0], DEC);
  for (int ii = 1; ii < 4; ++ii) {
    webFile.print(F("."));
    webFile.print(g_ip_ntpIp[ii], DEC);
  }

  webFile.print(F("</nip><br>"));
  webFile.print(g_u32_baudrate, DEC);
  
  webFile.print(F("</br><db>"));
  webFile.print(g_u8_dataBits, DEC);

  webFile.print(F("</db><par>"));
  webFile.print(g_u8_parity, DEC);

  webFile.print(F("</par><sb>"));
  webFile.print(g_u8_stopBits, DEC);

  webFile.print(F("</sb><to>"));
  webFile.print(g_u16_timeout, DEC);
  
  //webFile.print(F("</to></setup>"));
  webFile.print(F("</to>"));
  webFile.flush();
  webFile.close();
  digitalWrite(gk_s16_sdWriteLed, LOW);
}


void writeMtrSetupFile(){
  File webFile;

  digitalWrite(gk_s16_sdWriteLed, HIGH);

  SD.remove("mtrsetup.xml");
  webFile = SD.open("mtrsetup.xml", FILE_WRITE);
                      
  webFile.print(F("<?xml version = \"1.0\" ?><meterList>"));
  
  for (int ii = 0; ii < SlaveData.getNumSlvs(); ++ii){
    webFile.print(F("<meter><mip>"));
    
    if (SlaveData[ii].u8a_ip[0] != 0){
      webFile.print(SlaveData[ii].u8a_ip[0], DEC);
      for (int jj = 1; jj < 4; ++jj){
        webFile.print(F("."));
        webFile.print(SlaveData[ii].u8a_ip[jj], DEC);
      }
    }
    webFile.print(F("</mip><dev>"));
    
    webFile.print(SlaveData[ii].u8_id, DEC);

    webFile.print(F("</dev><vid>"));
    
    webFile.print(SlaveData[ii].u8_vid, DEC);

    webFile.print(F("</vid><type>"));

    webFile.print(SlaveData[ii].u8a_type[0], DEC);
    for (int jj = 1; jj < 3; ++jj){
      webFile.print(F("."));
      webFile.print(SlaveData[ii].u8a_type[jj], DEC);
    }

    webFile.print(F("</type></meter>"));
  }

  webFile.print(F("</meterList>"));
  
  webFile.flush();
  webFile.close();

  digitalWrite(gk_s16_sdWriteLed, LOW);
}

