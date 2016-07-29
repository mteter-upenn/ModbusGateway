/*
 * void setConstants
 * void writeGenSetup
 * void writeMeterSetup
 */



void setConstants(){
  uint8_t i, j;

  for (i = 0; i < 30; i++){
    g_c_gwName[i] = (char)EEPROM.read(i + g_u16_nameBlkStart);

    if (g_c_gwName[i] == 0){
      break;
    }
  }
  g_c_gwName[30] = 0;  // doublecheck to make sure string ends in null
  Serial.print(F("name: "));
  Serial.println(g_c_gwName);
  
  g_b_recordData = EEPROM.read(g_u16_nameBlkStart + 31);  // whether or not to record data
  g_u8_maxRecordSlaves = EEPROM.read(g_u16_nameBlkStart + 32) > 20 ? 5 : EEPROM.read(g_u16_nameBlkStart + 32);  // max slaves to record

  for (i = 0; i < 6; i++){
    g_u8a_mac[i] = EEPROM.read(g_u16_ipBlkStart + i);
  }

  for (i = 0; i < 4; i++){
    g_ip_ip[i] = EEPROM.read(i + g_u16_ipBlkStart + 6);
    g_ip_subnet[i] = EEPROM.read(i + g_u16_ipBlkStart + 10);
    g_ip_gateway[i] = EEPROM.read(i + g_u16_ipBlkStart + 14);

    g_ip_ntpIp[i] = EEPROM.read(i + g_u16_ipBlkStart + 19);
  }

  g_b_useNtp = EEPROM.read(g_u16_ipBlkStart + 18);

  g_u32_baudrate = EEPROM.read(g_u16_ipBlkStart + 23);
  g_u32_baudrate = (uint32_t)((g_u32_baudrate << 16) | (EEPROM.read(g_u16_ipBlkStart + 24) << 8) | (EEPROM.read(g_u16_ipBlkStart + 25)));

  g_u16_timeout = word(EEPROM.read(g_u16_ipBlkStart + 26), EEPROM.read(g_u16_ipBlkStart + 27));


  g_u8_numSlaves = EEPROM.read(g_u16_mtrBlkStart);

  for (i = 0; i < g_u8_numSlaves; i++){
    g_u8a_slaveIds[i] = EEPROM.read(9 * i + 8 + g_u16_mtrBlkStart);
    g_u8a_slaveVids[i] = EEPROM.read(9 * i + 9 + g_u16_mtrBlkStart);

    g_u8a_slaveIps[i][0] = EEPROM.read(9 * i + 4 + g_u16_mtrBlkStart);
    for (j = 1; j < 4; j++){
      g_u8a_slaveIps[i][j] = EEPROM.read(9 * i + g_u16_mtrBlkStart + j + 4);
      g_u8a_slaveTypes[i][(j - 1)] = EEPROM.read(9 * i + g_u16_mtrBlkStart + j);
    }
  }

//  for (i = 0; i < g_u8_numSlaves; i++){
//    Serial.print(F("dev: "));
//    Serial.print(g_u8a_slaveIds[i], DEC);
//    Serial.print(F(", ip: "));
//
//    Serial.print(g_u8a_slaveIps[i][0], DEC);
//    for (j = 1; j < 4; j++){
//      Serial.print(F("."));
//      Serial.print(g_u8a_slaveIps[i][j], DEC);
//    }
//
//    Serial.print(F(", type: "));
//
//    Serial.print(g_u8a_slaveTypes[i][0], DEC);
//    for (j = 1; j < 3; j++){
//      Serial.print(F("."));
//      Serial.print(g_u8a_slaveTypes[i][j], DEC);
//    }
//    Serial.println();
//  }
}


void writeGenSetupFile(){
  uint8_t i;
  File webFile;

  SD.remove("gensetup.xml");
  webFile = SD.open("gensetup.xml", FILE_WRITE);
  
  //webFile.println(F("HTTP/1.1 200 OK"));
  //webFile.println(F("Content-Type: text/xml"));  
  //webFile.println(F("Connection: close\n"));   
  
  webFile.print(F("<?xml version = \"1.0\" ?><setup><name>"));

  for (i = 0; i < 30; i++){
    if (g_c_gwName[i] == 0){
      break;
    }
    webFile.print(g_c_gwName[i]);
  }
  
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
  for (i = 1; i < 6; i++){
    webFile.print(F(":"));
    if (g_u8a_mac[i] < 16){
      webFile.print('0');
    }
    webFile.print(g_u8a_mac[i], HEX);
  }
  
  webFile.print(F("</mac><ip>"));

  webFile.print(g_ip_ip[0], DEC);
  for (i = 1; i < 4; i++){
    webFile.print(F("."));
    webFile.print(g_ip_ip[i], DEC);
  }
  
  webFile.print(F("</ip><sm>"));

  webFile.print(g_ip_subnet[0], DEC);
  for (i = 1; i < 4; i++){
    webFile.print(F("."));
    webFile.print(g_ip_subnet[i], DEC);
  }
  
  webFile.print(F("</sm><gw>"));

  webFile.print(g_ip_gateway[0], DEC);
  for (i = 1; i < 4; i++){
    webFile.print(F("."));
    webFile.print(g_ip_gateway[i], DEC);
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
  for (i = 1; i < 4; i++) {
    webFile.print(F("."));
    webFile.print(g_ip_ntpIp[i], DEC);
  }

  webFile.print(F("</nip><br>"));

  webFile.print(g_u32_baudrate, DEC);
  
  webFile.print(F("</br><to>"));

  webFile.print(g_u16_timeout, DEC);
  
  //webFile.print(F("</to></setup>"));
  webFile.print(F("</to>"));
  webFile.flush();
  webFile.close();
}


void writeMtrSetupFile(){
  uint8_t i, j;
  File webFile;

  SD.remove("mtrsetup.xml");
  webFile = SD.open("mtrsetup.xml", FILE_WRITE);

  //webFile.println(F("HTTP/1.1 200 OK"));
  //webFile.println(F("Content-Type: text/xml"));  
  //webFile.println(F("Connection: close\n"));   
                      
  webFile.print(F("<?xml version = \"1.0\" ?><meterList>"));
  
  for (i = 0; i < g_u8_numSlaves; i++){
    webFile.print(F("<meter><mip>"));
    
    if (g_u8a_slaveIps[i][0] != 0){
      webFile.print(g_u8a_slaveIps[i][0], DEC);
      for (j = 1; j < 4; j++){
        webFile.print(F("."));
        webFile.print(g_u8a_slaveIps[i][j], DEC);
      }
    }
    webFile.print(F("</mip><dev>"));
    
    webFile.print(g_u8a_slaveIds[i], DEC);

    webFile.print(F("</dev><vid>"));
    
    webFile.print(g_u8a_slaveVids[i], DEC);

    webFile.print(F("</vid><type>"));

    webFile.print(g_u8a_slaveTypes[i][0], DEC);
    for (j = 1; j < 3; j++){
      webFile.print(F("."));
      webFile.print(g_u8a_slaveTypes[i][j], DEC);
    }

    webFile.print(F("</type></meter>"));
  }

//  webFile.print(F("</meterList>"));
  
  webFile.flush();
  webFile.close();
}

