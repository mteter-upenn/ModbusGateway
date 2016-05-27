void read_eeprom() {
  uint16_t i, j; //  , j;
  uint16_t nm_strt, ip_strt, mtr_strt, reg_strt;
  uint32_t baudrate;
  uint16_t timeout;
  uint8_t mtrs, libs;
  char inpt[50];

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
  if (EEPROM.read(ip_strt) < 16) {
    Serial.print('0');
  }
  Serial.print(EEPROM.read(ip_strt), HEX);
  for (i = 1; i < 6; i++) {
    Serial.print(":");
    if (EEPROM.read(ip_strt + i) < 16) {
      Serial.print('0');
    }
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

  // Scroll?
  term_func(F("Continue? (Y)"), verFunc, F(""), F("Continue? (Y)"), inpt, "n", false, 0, false);

  // meters listed to this gateway

  Serial.print(F("Meters listed in this gateway: "));
  mtrs = EEPROM.read(mtr_strt);
  if (mtrs > 20) {
    mtrs = 0;
  }
  Serial.println(mtrs, DEC);
  Serial.println();

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
    Serial.println(EEPROM.read(mtr_strt + j * 9 + 8), DEC);

    // Meter virt ID
    Serial.print(F("Virtual Modbus ID: "));
    Serial.println(EEPROM.read(mtr_strt + j * 9 + 9), DEC);
  }
  Serial.println();

  // Scroll?
  term_func(F("Continue? (Y)"), verFunc, F(""), F("Continue? (Y)"), inpt, "n", false, 0, false);

  // Meter Versions
 
  Serial.println(F("Meter library table:"));
  Serial.println(F("|-------------------------------------|-----------|"));
  Serial.println(F("|  Eaton IQ DP 4000                   |  0.1.1    |"));
  Serial.println(F("|  Eaton PXM 2260                     |  15.1.0   |"));
  Serial.println(F("|  Eaton IQ 200                       |  0.1.3    |"));
  Serial.println(F("|  Eaton IQ 300                       |  0.1.4    |"));
  Serial.println(F("|  Eaton Power Xpert 4000             |  1.1.0    |"));
  Serial.println(F("|--Eaton-PXM-2000---------------------|--x.x.x----|"));
  Serial.println(F("|-------------------------------------|-----------|"));
  Serial.println(F("|  Emon Dmon 3400                     |  2.1.0    |"));
  Serial.println(F("|-------------------------------------|-----------|"));
  Serial.println(F("|  GE EPM 3720                        |  3.1.0    |"));
  Serial.println(F("|  GE EPM 5100                        |  0.2.1    |"));
  Serial.println(F("|  GE PQM                             |  4.1.0    |"));
  Serial.println(F("|-------------------------------------|-----------|"));
  Serial.println(F("|  Siemens 9200                       |  0.3.1    |"));
  Serial.println(F("|  Siemens 9330                       |  5.1.0    |"));
  Serial.println(F("|  Siemens 9340                       |  10.8.0   |"));
  Serial.println(F("|  Siemens 9350                       |  5.2.0    |"));
  Serial.println(F("|  Siemens 9360                       |  5.3.0    |"));
  Serial.println(F("|  Siemens 9510                       |  6.1.0    |"));
  Serial.println(F("|  Siemens 9610                       |  6.2.0    |"));
  Serial.println(F("|  Siemens 9700                       |  5.4.0    |"));
  Serial.println(F("|  Siemens Sentron PAC 4200           |  14.1.0   |"));
  Serial.println(F("|  Siemens Sentron PAC 3200           |  14.2.0   |"));
  Serial.println(F("|-------------------------------------|-----------|"));
  Serial.println(F("|  SquareD CM 2350                    |  7.1.0    |"));
  Serial.println(F("|  SquareD PM 210                     |  13.1.0   |"));
  Serial.println(F("|  SquareD PM 710                     |  8.1.0    |"));
  Serial.println(F("|  SquareD Micrologic A Trip Unit     |  9.1.0    |"));
  Serial.println(F("|  SquareD Micrologic P Trip Unit     |  9.2.0    |"));
  Serial.println(F("|  SquareD Micrologic H Trip Unit     |  9.3.0    |"));
  Serial.println(F("|  SquareD CM 3350                    |  10.1.0   |"));
  Serial.println(F("|  SquareD CM 4000                    |  10.2.0   |"));
  Serial.println(F("|  SquareD CM 4250                    |  10.3.0   |"));
  Serial.println(F("|  SquareD PM 800                     |  10.4.0   |"));
  Serial.println(F("|  SquareD PM 820                     |  10.5.0   |"));
  Serial.println(F("|  SquareD PM 850                     |  10.6.0   |"));
  Serial.println(F("|  SquareD PM 870                     |  10.7.0   |"));
  Serial.println(F("|-------------------------------------|-----------|"));
  Serial.println(F("|  Chilled Water KEP                  |  11.1.0   |"));
  Serial.println(F("|  Steam KEP                          |  12.1.0   |"));
  Serial.println(F("|-------------------------------------|-----------|"));


  // register library
  Serial.print(F("Number of libraries: "));
  libs = EEPROM.read(reg_strt + 2);
  Serial.println(libs, DEC);
  Serial.println();

  for (i = 0; i < libs; i++) {
    Serial.print(F("Library number: "));
    Serial.println(EEPROM.read(reg_strt + 5 + i * 4), DEC);

    Serial.print(F("Modbus function: "));
    Serial.println(EEPROM.read(reg_strt + 6 + i * 4), DEC);

    Serial.print(F("Library index: "));
    Serial.println(word(EEPROM.read(reg_strt + 3 + i * 4), EEPROM.read(reg_strt + 4 + i * 4)));
    Serial.println();
  }
}
