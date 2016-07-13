
#include <EEPROM.h>
#include "mac.h"

#define SERIAL_INPUT 1  // 0 for no serial input, 1 for serial input (mac, ip, name, etc)

 
byte const FLOAT = 0x00;
byte const U16_to_FLOAT = 0x01;
byte const S16_to_FLOAT = 0x02;
byte const U32_to_FLOAT = 0x03;
byte const S32_to_FLOAT = 0x04;
byte const M1K_to_FLOAT = 0x05;
byte const M10K_to_FLOAT = 0x06;
byte const M20K_to_FLOAT = 0x07;
byte const M30K_to_FLOAT = 0x08;
byte const U64_to_FLOAT = 0x09;
byte const EGY_to_FLOAT = 0x0A;
byte const DBL_to_FLOAT = 0x0B;
byte const WORDSWAP = 0x80;

bool bFirstLoop = true;
bool bQuit = false;

// PROTOTYPES
bool term_func(const __FlashStringHelper *, bool(*argFunc)(char*), const __FlashStringHelper *,
  const __FlashStringHelper *, char *, const char *, bool, uint8_t, bool);


void setup() {
  uint16_t ip_strt, nm_strt, reg_strt, mtr_strt;

  pinMode(19, OUTPUT);
  pinMode(20, OUTPUT);
  digitalWrite(19, HIGH);

  Serial.begin(9600);
  Serial.println(F("delay"));
  delay(2000);
  Serial.println(F("delay over"));


  nm_strt = 10;
  ip_strt = nm_strt + 33; // 43
  mtr_strt = ip_strt + 28; // 71
  reg_strt = mtr_strt + 181;  // 252

  EEPROM.write(0, highByte(nm_strt));
  EEPROM.write(1, lowByte(nm_strt));
  EEPROM.write(2, highByte(ip_strt));
  EEPROM.write(3, lowByte(ip_strt));
  EEPROM.write(4, highByte(mtr_strt));
  EEPROM.write(5, lowByte(mtr_strt));
  EEPROM.write(6, highByte(reg_strt));
  EEPROM.write(7, lowByte(reg_strt));
}

void loop() {
  uint16_t ip_strt, nm_strt, reg_strt, reg_end, mtr_strt;
  uint16_t i, j, numMtrs;
  bool bResponse;
  char inpt[50];
  char cMenu;


  // duplicate should make this global
  nm_strt = 10;
  ip_strt = nm_strt + 33; // 43
  mtr_strt = ip_strt + 28; // 71
  reg_strt = mtr_strt + 181;  // 252

  bQuit = false;

  //term_func(F(""), argFunc, F(""), F(""), inpt, "", true, 0, false);

  if (bFirstLoop) {
    // start, keep flashing until ready
    term_func(F("Please set line ending to newline.  Ready to start? (Y)"), verFunc, F("OK, let's start! To choose defaults, type \"default.\""),
      F("Please set line ending to newline.  Ready to start? (Y)"), inpt, "n", false, 5, false);
  }

  // ask where to go
  term_func(F("\nUPenn Modbus Gateway Setup Menu:\n"
    "lowercase/UPPERCASE: read/WRITE options\n"
    "q/-: Quit and return to menu\n"
    "a/A: Everything\n"
    "n/N: Name\n"
    "i/I: Gateway IP and MAC\n"
    "t/T: NTP server\n"
    "r/R: Record data\n"
    "s/S: Serial options\n"
    "m/M: Gateway meter list\n"
    "l/L: Register library"), menuFunc, F("OK."), F("Sorry, please pick a valid option."), inpt, "n", false, 0, false);

  cMenu = inpt[0];

  // pull from inpt and follow through
  if (cMenu == 'N' || cMenu == 'A') {  // name

      // write name
    term_func(F("Please input a name.  Default was \"UPenn_Modbus_Gateway.\"  There is a 30 character limit."), nmFunc, F("Great name!"),
      F("Please input a name.  Default was \"UPenn_Modbus_Gateway.\"  There is a 30 character limit."), inpt, "UPenn_Modbus_Gateway", true, 0, false);

    storeName(inpt, nm_strt);
  }

  if (cMenu == 'I' || cMenu == 'A') {
    // mac
#if defined(CORE_TEENSY)  // if teensy3.0 or greater
    if (!bQuit) {
      read_mac();
      for (j = 0; j < 6; j++) {
        EEPROM.write(ip_strt + j, mac[j]);
      }

      Serial.print(F("This microcontroller (Teensy) already has a MAC!  It is "));

      if (mac[0] < 16) {
        Serial.print('0');
      }
      Serial.print(mac[0], HEX);
      for (j = 1; j < 6; j++) {
        Serial.print(':');
        if (mac[j] < 16) {
          Serial.print('0');
        }
        Serial.print(mac[j], HEX);
      }
      Serial.println();
    }
#else
    term_func(F("Please insert number from 1 to 65535 in decimal to be used as last two bytes in MAC."), macFunc, F("Ok, let's move on to the IP."),
      F("Please insert number from 1 to 65535 in decimal to be used as last two bytes in MAC."), inpt, "0", true, 0, false);

    //storeMac(inpt, ip_strt);
#endif

    // Gateway IP
    term_func(F("Please insert the device's IP address."), ipFunc, F("Ok, now the subnet mask"),
      F("Please insert IP using X.X.X.X format where X is in [0, 255]."), inpt, "130.91.138.141", true, 0, false);
    storeIP(inpt, ip_strt + 6, 4);

    // Subnet mask
    term_func(F("Please insert the device's subnet mask."), ipFunc, F("Ok, now the default gateway"),
      F("Please insert subnet mask using X.X.X.X format where X is in [0, 255]."), inpt, "255.255.252.0", true, 0, false);
    storeIP(inpt, ip_strt + 10, 4);

    // default gateway
    term_func(F("Please insert the device's default gateway address."), ipFunc, F("Ok."),
      F("Please insert default gateway using X.X.X.X format where X is in [0, 255]."), inpt, "130.91.136.1", true, 0, false);
    storeIP(inpt, ip_strt + 14, 4);
  }

  if (cMenu == 'T' || cMenu == 'A') {
    // yes/no on ntp
    bResponse = term_func(F("Do you want to use an NTP server? (y/n)"), verFunc, F("Ok, let's fill out its IP."),
      F("Ok, now for 485 parameters."), inpt, "n", true, 0, true);
    storeBool(inpt, ip_strt + 18);

    // ntp server ip
    if (bResponse) {
      term_func(F("Please insert the address of the NTP server."), ipFunc, F("Ok."),
        F("Please insert IP using X.X.X.X format where X is in [0, 255]."), inpt, "128.91.3.136", true, 0, false);
    }
    else {
      // write default to this?
      strcpy_P(inpt, PSTR("128.91.3.136"));
    }
    storeIP(inpt, ip_strt + 19, 4);
  }

  if (cMenu == 'S' || cMenu == 'A') {
    // baudrate
    term_func(F("Please insert a baudrate for 485 communications."), brFunc, F("Ok."),
      F("The number you entered is outside of the bounds!  Please select one of the following:\n300\n1200\n2400\n4800\n9600\n19200\n31250\n38400\n57600\n115200"),
      inpt, "9600", true, 0, false);
    storeMedInt(inpt, ip_strt + 23);

    // timeout
    term_func(F("Please insert a Modbus timeout. (ms)"), toFunc, F("Ok."),
      F("Please insert number from 1 to 30000 in decimal for Modbus timeout."), inpt, "1500", true, 0, false);
    storeInt(inpt, ip_strt + 26);
  }

  if (cMenu == 'R' || cMenu == 'A') {
    // record data locally?
    bResponse = term_func(F("Should this meter record data locally?"), verFunc, F("Ok, it will record data."),
      F("Ok, it won't record data."), inpt, "n", true, 0, true);
    storeBool(inpt, nm_strt + 31);

    // number of meters to record
    if (bResponse) {
      term_func(F("Please insert number of meters to record (max 20)."), mtrnumFunc, F("Ok."),
        F("Please insert number of meters to record (max 20)."), inpt, "5", true, 0, false);
      storeByte(inpt, nm_strt + 32);
    }
    else {
      // default number of meters? (change if outside of bounds)
      if (EEPROM.read(nm_strt + 32) > 20) {
        strcpy_P(inpt, PSTR("5"));

        storeByte(inpt, nm_strt + 32);
      }
      // else do nothing
    }
    
  }

  if (cMenu == 'M' || cMenu == 'A') {
    // number of meters listed
    numMtrs = 0;
    term_func(F("Please insert the number of meters actively controlled by the gateway (max 20)."), mtrnumFunc, F("Ok."),
      F("Please insert the number of meters actively controlled by the gateway (max 20)."), inpt, "0", true, 0, false);

    numMtrs = storeByte(inpt, mtr_strt);

    if (!bQuit) {
      // all meter information
      for (i = 0; i < numMtrs; i++) {
        Serial.print(F("Meta data for meter "));
        Serial.print(i + 1, DEC);
        Serial.print(F(" of "));
        Serial.println(numMtrs, DEC);

        // meter type
        term_func(F("Please insert meter type (X.X.X)."), mtrtypFunc, F("Ok."),
          F("Please insert meter type (X.X.X)."), inpt, "12.1.0", true, 0, false);
        storeIP(inpt, mtr_strt + 9 * (i + 1) - 8, 3);

        // 485 or mb/tcp
        bResponse = term_func(F("Is this meter connected via IP? (y/n)"), verFunc, F("This meter is connected via IP."),
          F("This meter is connected via serial comms."), inpt, "n", true, 0, true);  // nothing to store here

        if (bResponse) {
          // modbus ip
          term_func(F("Please insert the meter's IP."), ipFunc, F("Ok."),
            F("Please insert the meter's IP."), inpt, "0.0.0.0", true, 0, false);
        }
        else {
          // default ip of 0.0.0.0
          strcpy_P(inpt, PSTR("0.0.0.0"));
        }
        storeIP(inpt, mtr_strt + 9 * (i + 1) - 5, 4);

        // actual modbus id
        term_func(F("Please insert actual Modbus id. (0-247)"), mbidFunc, F("Ok."),
          F("Please insert actual Modbus id. (0-247)"), inpt, "1", true, 0, false);
        storeByte(inpt, mtr_strt + 9 * (i + 1) - 1);

        // virtual modbus id
        term_func(F("Please insert virtual Modbus id. (0-247)"), mbidFunc, F("Ok."),
          F("Please insert virtual Modbus id. (0-247)"), inpt, "1", true, 0, false);
        storeByte(inpt, mtr_strt + 9 * (i + 1));
      }
    }
  }

  if (cMenu == 'L' || cMenu == 'A') {
    // write library
    if (!bQuit) {
      reg_end = writeBlocks(reg_strt);

      Serial.println("Finished writing to EEPROM.");
      Serial.print("indexing stops at byte ");
      Serial.println(reg_end, DEC);
      digitalWrite(20, HIGH);
    }
  }
  
  if (islower(cMenu)) {
    read_eeprom(cMenu);
  }
    

  // term_func(F(""), Func,    F(""), F(""), inpt, "", true,    0,           false);
  // term_func(msg,   argFunc, pos,   neg,   inpt, "", verify?, repeat time, exit on neg)
  Serial.println(F("Setup is complete.\n"));
  bFirstLoop = false;
  //return;  // AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
}

void clearSerialRx() {
  while(Serial.available()){
    Serial.read();
  }
}

  /* Meter Versions
   *
   * |-------------------------------------|-----------|
   * |  Eaton IQ DP 4000                   |  0.1.1    |
   * |  Eaton PXM 2260                     |  15.1.0   |
   * |  Eaton IQ 200                       |  0.1.3    |
   * |  Eaton IQ 300                       |  0.1.4    |
   * |  Eaton Power Xpert 4000             |  1.1.0    |
   * |--Eaton-PXM-2000---------------------|--x.x.x----|
   * |-------------------------------------|-----------|
   * |  Emon Dmon 3400                     |  2.1.0    |
   * |-------------------------------------|-----------|
   * |  GE EPM 3720                        |  3.1.0    |
   * |  GE EPM 5100                        |  0.2.1    |
   * |  GE PQM                             |  4.1.0    |
   * |-------------------------------------|-----------|
   * |  Siemens 9200                       |  0.3.1    |  10
   * |  Siemens 9330                       |  5.1.0    |
   * |  Siemens 9340                       |  10.8.0   |
   * |  Siemens 9350                       |  5.2.0    |
   * |  Siemens 9360                       |  5.3.0    |
   * |  Siemens 9510                       |  6.1.0    |
   * |  Siemens 9610                       |  6.2.0    |
   * |  Siemens 9700                       |  5.4.0    |
   * |  Siemens Sentron PAC 4200           |  14.1.0   |
   * |  Siemens Sentron PAC 3200           |  14.2.0   |
   * |-------------------------------------|-----------|
   * |  SquareD CM 2350                    |  7.1.0    |  20
   * |  SquareD PM 210                     |  13.1.0   |
   * |  SquareD PM 710                     |  8.1.0    |
   * |  SquareD Micrologic A Trip Unit     |  9.1.0    |
   * |  SquareD Micrologic P Trip Unit     |  9.2.0    |
   * |  SquareD Micrologic H Trip Unit     |  9.3.0    |
   * |  SquareD CM 3350                    |  10.1.0   |
   * |  SquareD CM 4000                    |  10.2.0   |
   * |  SquareD CM 4250                    |  10.3.0   |
   * |  SquareD PM 800                     |  10.4.0   |
   * |  SquareD PM 820                     |  10.5.0   |  30
   * |  SquareD PM 850                     |  10.6.0   |
   * |  SquareD PM 870                     |  10.7.0   |
   * |-------------------------------------|-----------|
   * |  Chilled Water KEP                  |  11.1.0   |
   * |  Steam KEP                          |  12.1.0   |
   * |-------------------------------------|-----------|
   */
