void read_eeprom() {
  uint16_t i, j;
  uint16_t nm_strt, ip_strt, mtr_strt, reg_strt;

  nm_strt = word(EEPROM.read(0), EEPROM.read(1));
  ip_strt = word(EEPROM.read(2), EEPROM.read(3));
  mtr_strt = word(EEPROM.read(4), EEPROM.read(5));
  reg_strt = word(EEPROM.read(6), EEPROM.read(7));


  Serial.print(F("nm_strt: "));
  Serial.println(nm_strt);

  Serial.print(F("ip_strt: "));
  Serial.println(ip_strt);

  Serial.print(F("mtr_strt: "));
  Serial.println(mtr_strt);

  Serial.print(F("reg_strt: "));
  Serial.println(reg_strt);

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
    Serial.println(F("true"));
  }
  else {
    Serial.println(F("false"));
  }

  Serial.print("Max number of meters to record: ");
  Serial.println(EEPROM.read(nm_strt + 32), DEC);

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

  
}
