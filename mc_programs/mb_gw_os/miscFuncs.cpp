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
#include <ModbusStructs.h>

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
  CPU_RESTART
  delay(20);
}

void setConstants() {
  EEPROM.get(g_u16_nameBlkStart, g_gwName);

  g_gwName.ca_name[31] = 0;
  Serial.print(F("name: "));
  Serial.println(g_gwName.ca_name);

  EEPROM.get(g_u16_nameBlkStart + 32, g_b_recordData);
  EEPROM.get(g_u16_nameBlkStart + 33, g_u8_maxRecordSlaves);
  g_u8_maxRecordSlaves = g_u8_maxRecordSlaves > 20 ? 5 : g_u8_maxRecordSlaves;  // max slaves to record

  EEPROM.get(g_u16_ipBlkStart, g_u8a_mac);

  EEPROM.get(g_u16_ipBlkStart + 6, g_ip_ip);
  EEPROM.get(g_u16_ipBlkStart + 10, g_ip_subnet);
  EEPROM.get(g_u16_ipBlkStart + 14, g_ip_gateway);
  EEPROM.get(g_u16_ipBlkStart + 19, g_ip_ntpIp);

  EEPROM.get(g_u16_ipBlkStart + 18, g_b_useNtp);

  EEPROM.get(g_u16_ipBlkStart + 23, g_u32_baudrate);
  EEPROM.get(g_u16_ipBlkStart + 27, g_u8_dataBits);
  EEPROM.get(g_u16_ipBlkStart + 28, g_u8_parity);
  EEPROM.get(g_u16_ipBlkStart + 29, g_u8_stopBits);

  EEPROM.get(g_u16_ipBlkStart + 30, g_u16_timeout);
}


void writeRestartFile() {
  File webFile;

  digitalWrite(SD_WRITE_LED_PIN, HIGH);

  SD.remove("restart.xml");
  webFile = SD.open("restart.xml", FILE_WRITE);
  webFile.print(F("<?xml version = \"1.0\" ?><setup><ip>"));
  webFile.print(g_ip_ip.u8a_ip[0], DEC);
  for (int ii = 1; ii < 4; ++ii){
    webFile.print(F("."));
    webFile.print(g_ip_ip.u8a_ip[ii], DEC);
  }

  webFile.print(F("</ip></setup>"));
  webFile.flush();
  webFile.close();
  digitalWrite(SD_WRITE_LED_PIN, LOW);
}

void writeGenSetupFile(){
  File webFile;

  digitalWrite(SD_WRITE_LED_PIN, HIGH);

  SD.remove("gensetup.xml");
  webFile = SD.open("gensetup.xml", FILE_WRITE);

  webFile.print(F("<?xml version = \"1.0\" ?><setup><name>"));
  webFile.print(g_gwName.ca_name);

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
  if (g_u8a_mac.u8a_mac[0] < 16) {
    webFile.print('0');
  }
  webFile.print(g_u8a_mac.u8a_mac[0], HEX);
  for (int ii = 1; ii < 6; ++ii){
    webFile.print(F(":"));
    if (g_u8a_mac.u8a_mac[ii] < 16){
      webFile.print('0');
    }
    webFile.print(g_u8a_mac.u8a_mac[ii], HEX);
  }

  webFile.print(F("</mac><ip>"));
  webFile.print(g_ip_ip.u8a_ip[0], DEC);
  for (int ii = 1; ii < 4; ++ii){
    webFile.print(F("."));
    webFile.print(g_ip_ip.u8a_ip[ii], DEC);
  }

  webFile.print(F("</ip><sm>"));
  webFile.print(g_ip_subnet.u8a_ip[0], DEC);
  for (int ii = 1; ii < 4; ++ii) {
    webFile.print(F("."));
    webFile.print(g_ip_subnet.u8a_ip[ii], DEC);
  }

  webFile.print(F("</sm><gw>"));
  webFile.print(g_ip_gateway.u8a_ip[0], DEC);
  for (int ii = 1; ii < 4; ++ii) {
    webFile.print(F("."));
    webFile.print(g_ip_gateway.u8a_ip[ii], DEC);
  }

  webFile.print(F("</gw><ntp>"));
  if (g_b_useNtp) {
    webFile.print(F("true"));
  }
  else {
    webFile.print(F("false"));
  }

  webFile.print(F("</ntp><nip>"));
  webFile.print(g_ip_ntpIp.u8a_ip[0], DEC);
  for (int ii = 1; ii < 4; ++ii) {
    webFile.print(F("."));
    webFile.print(g_ip_ntpIp.u8a_ip[ii], DEC);
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
  digitalWrite(SD_WRITE_LED_PIN, LOW);
}


void writeMtrSetupFile(){
  File webFile;

  digitalWrite(SD_WRITE_LED_PIN, HIGH);

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

    webFile.print(SlaveData[ii].u8a_mtrType[0], DEC);
    for (int jj = 1; jj < 3; ++jj){
      webFile.print(F("."));
      webFile.print(SlaveData[ii].u8a_mtrType[jj], DEC);
    }

    webFile.print(F("</type></meter>"));
  }

  webFile.print(F("</meterList>"));

  webFile.flush();
  webFile.close();

  digitalWrite(SD_WRITE_LED_PIN, LOW);
}

