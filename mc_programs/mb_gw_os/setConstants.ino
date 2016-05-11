/*
 * void setConstants
 * void writeSetupFile
 * void writeGenSetup
 * void writeMeterSetup
 */



void setConstants(){
  uint8_t i, j;

  for (i = 0; i < 30; i++){
    meter_nm[i] = (char)EEPROM.read(i + nm_strt);

    if (meter_nm[i] == 0){
      break;
    }
  }
  meter_nm[30] = 0;  // doublecheck to make sure string ends in null
  Serial.print(F("name: "));
  Serial.println(meter_nm);
  
  bRecordData = EEPROM.read(nm_strt + 31);  // whether or not to record data
  maxSlvsRcd = EEPROM.read(nm_strt + 32) > 20 ? 5 : EEPROM.read(nm_strt + 32);  // max slaves to record

  for (i = 0; i < 6; i++){
    mac[i] = EEPROM.read(ip_strt + i);
  }

  for (i = 0; i < 4; i++){
    ip[i] = EEPROM.read(i + ip_strt + 6);
    subnet[i] = EEPROM.read(i + ip_strt + 10);
    gateway[i] = EEPROM.read(i + ip_strt + 14);
  }

  baudrate = EEPROM.read(ip_strt + 18);
  baudrate = (uint32_t)((baudrate << 16) | (EEPROM.read(ip_strt + 19) << 8) | (EEPROM.read(ip_strt + 20)));

  timeout = word(EEPROM.read(ip_strt + 21), EEPROM.read(ip_strt + 22));


  slaves = EEPROM.read(mtr_strt);
  
  for (i = 0; i < slaves; i++){
    slv_devs[i] = EEPROM.read(9 * i + 8 + mtr_strt);
    slv_vids[i] = EEPROM.read(9 * i + 9 + mtr_strt);

    slv_ips[i][0] = EEPROM.read(9 * i + 4 + mtr_strt);
    for (j = 1; j < 4; j++){
      slv_ips[i][j] = EEPROM.read(9 * i + mtr_strt + j + 4);
      slv_typs[i][(j - 1)] = EEPROM.read(9 * i + mtr_strt + j);
    }
  }

//  for (i = 0; i < slaves; i++){
//    Serial.print(F("dev: "));
//    Serial.print(slv_devs[i], DEC);
//    Serial.print(F(", ip: "));
//
//    Serial.print(slv_ips[i][0], DEC);
//    for (j = 1; j < 4; j++){
//      Serial.print(F("."));
//      Serial.print(slv_ips[i][j], DEC);
//    }
//
//    Serial.print(F(", type: "));
//
//    Serial.print(slv_typs[i][0], DEC);
//    for (j = 1; j < 3; j++){
//      Serial.print(F("."));
//      Serial.print(slv_typs[i][j], DEC);
//    }
//    Serial.println();
//  }
}


void writeGenSetupFile(){
  uint8_t i;
  File webFile;

  SD.remove("gensetup.xml");
  webFile = SD.open("gensetup.xml", FILE_WRITE);
  
  webFile.println(F("HTTP/1.1 200 OK"));
  webFile.println(F("Content-Type: text/xml"));  
  webFile.println(F("Connection: close\n"));   
  
  webFile.print(F("<?xml version = \"1.0\" ?><setup><name>"));

  for (i = 0; i < 30; i++){
    if (meter_nm[i] == 0){
      break;
    }
    webFile.print(meter_nm[i]);
  }
  
  webFile.print(F("</name><rd>"));
  
  if (bRecordData) {
    webFile.print(F("true"));
  }
  else {
    webFile.print(F("false"));
  }

  webFile.print(F("</rd><mxslvs>"));

  webFile.print(maxSlvsRcd, DEC);

  webFile.print(F("</mxslvs><mac>"));

  if (mac[0] < 16) {
    webFile.print('0');
  }
  webFile.print(mac[0], HEX);
  for (i = 1; i < 6; i++){
    webFile.print(F(":"));
    if (mac[i] < 16){
      webFile.print('0');
    }
    webFile.print(mac[i], HEX);
  }
  
  webFile.print(F("</mac><ip>"));

  webFile.print(ip[0], DEC);
  for (i = 1; i < 4; i++){
    webFile.print(F("."));
    webFile.print(ip[i], DEC);
  }
  
  webFile.print(F("</ip><sm>"));

  webFile.print(subnet[0], DEC);
  for (i = 1; i < 4; i++){
    webFile.print(F("."));
    webFile.print(subnet[i], DEC);
  }
  
  webFile.print(F("</sm><gw>"));

  webFile.print(gateway[0], DEC);
  for (i = 1; i < 4; i++){
    webFile.print(F("."));
    webFile.print(gateway[i], DEC);
  }
  
  webFile.print(F("</gw><br>"));

  webFile.print(baudrate, DEC);
  
  webFile.print(F("</br><to>"));

  webFile.print(timeout, DEC);
  
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

  webFile.println(F("HTTP/1.1 200 OK"));
  webFile.println(F("Content-Type: text/xml"));  
  webFile.println(F("Connection: close\n"));   
                      
  webFile.print(F("<?xml version = \"1.0\" ?><meterList>"));
  
  for (i = 0; i < slaves; i++){
    webFile.print(F("<meter><mip>"));
    
    if (slv_ips[i][0] != 0){
      webFile.print(slv_ips[i][0], DEC);
      for (j = 1; j < 4; j++){
        webFile.print(F("."));
        webFile.print(slv_ips[i][j], DEC);
      }
    }
    webFile.print(F("</mip><dev>"));
    
    webFile.print(slv_devs[i], DEC);

    webFile.print(F("</dev><vid>"));
    
    webFile.print(slv_vids[i], DEC);

    webFile.print(F("</vid><type>"));

    webFile.print(slv_typs[i][0], DEC);
    for (j = 1; j < 3; j++){
      webFile.print(F("."));
      webFile.print(slv_typs[i][j], DEC);
    }

    webFile.print(F("</type></meter>"));
  }

//  webFile.print(F("</meterList>"));
  
  webFile.flush();
  webFile.close();
}

