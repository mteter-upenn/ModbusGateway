void read_eeprom() {
  uint16_t i, j; //  , j;
  uint16_t nm_strt, ip_strt, mtr_strt, reg_strt;
  uint32_t baudrate;
  uint16_t timeout;
  uint8_t mtrs;


  nm_strt = word(EEPROM.read(0), EEPROM.read(1));
  ip_strt = word(EEPROM.read(2), EEPROM.read(3));
  mtr_strt = word(EEPROM.read(4), EEPROM.read(5));
  reg_strt = word(EEPROM.read(6), EEPROM.read(7));

  Serial.println();
  Serial.print(F("nm_strt: "));
  Serial.println(nm_strt);

  Serial.print(F("ip_strt: "));
  Serial.println(ip_strt);

  Serial.print(F("mtr_strt: "));
  Serial.println(mtr_strt);

  Serial.print(F("reg_strt: "));
  Serial.println(reg_strt);
  Serial.println();

  // Name:
  Serial.print(F("Name: "));
  for (i = nm_strt; i < nm_strt + 31; i++) {
    char c = EEPROM.read(i);

    if (c == 0) {
      Serial.println();
      break;
    }
    else {
      Serial.print(c);
    }
  }

  // record stuff
  Serial.print(F("Record Data?: "));
  if (EEPROM.read(nm_strt + 31)) {
    Serial.println(F("yes"));
  }
  else {
    Serial.println(F("no"));
  }

  Serial.print("Max number of meters to record: ");
  Serial.println(EEPROM.read(nm_strt + 32), DEC);
  Serial.println();

  // IP info
  Serial.print(F("MAC: "));
  Serial.print(EEPROM.read(ip_strt), HEX);
  for (i = 1; i < 6; i++) {
    Serial.print(":");
    Serial.print(EEPROM.read(ip_strt + i), HEX);
  }
  Serial.println();

  Serial.print(F("IP: "));
  Serial.print(EEPROM.read(ip_strt + 6), DEC);
  for (i = 1; i < 4; i++) {
    Serial.print(".");
    Serial.print(EEPROM.read(ip_strt + 6 + i), DEC);
  }
  Serial.println();

  Serial.print(F("Subnet Mask: "));
  Serial.print(EEPROM.read(ip_strt + 10), DEC);
  for (i = 1; i < 4; i++) {
    Serial.print(".");
    Serial.print(EEPROM.read(ip_strt + 10 + i), DEC);
  }
  Serial.println();

  Serial.print(F("Default Gateway: "));
  Serial.print(EEPROM.read(ip_strt + 14), DEC);
  for (i = 1; i < 4; i++) {
    Serial.print(".");
    Serial.print(EEPROM.read(ip_strt + 14 + i), DEC);
  }
  Serial.println();

  Serial.print(F("Use NTP Server?: "));
  if (EEPROM.read(ip_strt + 18)) {
    Serial.println(F("yes"));
  }
  else {
    Serial.println(F("no"));
  }

  Serial.print(F("NTP Server IP: "));
  Serial.print(EEPROM.read(ip_strt + 19), DEC);
  for (i = 1; i < 4; i++) {
    Serial.print(".");
    Serial.print(EEPROM.read(ip_strt + 19 + i), DEC);
  }
  Serial.println('\n');

  // 485/modbus stuff
  
  Serial.print(F("Baud rate: "));
  baudrate = EEPROM.read(ip_strt + 23);
  baudrate = (uint32_t)((baudrate << 16) | (EEPROM.read(ip_strt + 24) << 8) | (EEPROM.read(ip_strt + 25)));
  Serial.println(baudrate, DEC);

  Serial.print(F("Modbus timeout: "));
  timeout = word(EEPROM.read(ip_strt + 26), EEPROM.read(ip_strt + 27));
  Serial.println(timeout, DEC);
  Serial.println();

  // meters listed to this gateway

  Serial.print(F("Meters listed in this gateway: "));
  mtrs = EEPROM.read(mtr_strt);
  if (mtrs > 20) {
    mtrs = 0;
  }
  Serial.println(mtrs, DEC);

  for (j = 0; j < mtrs; j++) {
    // Meter type
    Serial.print(F("Meter type: "));
    Serial.print(EEPROM.read(mtr_strt + j * 9 + 1), DEC);
    for (i = 1; i < 3; i++) {
      Serial.print(".");
      Serial.print(EEPROM.read(mtr_strt + j * 9 + 1 + i), DEC);
    }
    Serial.println();

    // Meter IP
    if (EEPROM.read(mtr_strt + j * 9 + 4) == 0) {
      Serial.println(F("Meter is connected via 485"));
    }
    else {
      Serial.print(F("Meter IP: "));
      Serial.print(EEPROM.read(mtr_strt + j * 9 + 4), DEC);
      for (i = 1; i < 4; i++) {
        Serial.print(".");
        Serial.print(EEPROM.read(mtr_strt + j * 9 + 4 + i), DEC);
      }
      Serial.println();
    }

    // Meter actual modbus id
    Serial.print(F("Actual Modbus ID: "));
    Serial.println(EEPROM.read(mtr_strt + j * 9 + 8));

    // Meter virt ID
    Serial.print(F("Virtual Modbus ID: "));
    Serial.println(EEPROM.read(mtr_strt + j * 9 + 9));
  }
  Serial.println();

  //EEPROM.write(mtr_strt + 1, 12);  // meter type
  //EEPROM.write(mtr_strt + 2, 1);
  //EEPROM.write(mtr_strt + 3, 0);
  //EEPROM.write(mtr_strt + 4, 10);  // ip  (all zero if connected via 485)
  //EEPROM.write(mtr_strt + 5, 166);
  //EEPROM.write(mtr_strt + 6, 4);
  //EEPROM.write(mtr_strt + 7, 34);
  //EEPROM.write(mtr_strt + 8, 10);  // actual id
  //EEPROM.write(mtr_strt + 9, 1);  // virtual id
}
