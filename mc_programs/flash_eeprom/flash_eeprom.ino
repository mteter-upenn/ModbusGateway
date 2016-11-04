


#include <ModbusStructs.h>
#include <ModbusStack.h>
#include <MeterLibrary.h>
#include <EEPROM.h>
#include "mac.h"


#define SERIAL_INPUT 1  // 0 for no serial input, 1 for serial input (mac, ip, name, etc)

#define NEW_GROUP_STYLE 1
 
//byte const FLOAT = 0x00;
//byte const U16_to_FLOAT = 0x01;
//byte const S16_to_FLOAT = 0x02;
//byte const U32_to_FLOAT = 0x03;
//byte const S32_to_FLOAT = 0x04;
//byte const M1K_to_FLOAT = 0x05;
//byte const M10K_to_FLOAT = 0x06;
//byte const M20K_to_FLOAT = 0x07;
//byte const M30K_to_FLOAT = 0x08;
//byte const U64_to_FLOAT = 0x09;
//byte const EGY_to_FLOAT = 0x0A;
//byte const DBL_to_FLOAT = 0x0B;
//byte const WORDSWAP = 0x80;

bool b_firstLoop = true;
bool b_quit = false;

// PROTOTYPES
bool term_func(const __FlashStringHelper *, bool(*argFunc)(char*), const __FlashStringHelper *,
  const __FlashStringHelper *, char *, const char *, bool, uint8_t, bool);
//const __FlashStringHelper *

void setup() {
  uint16_t u16_ipStrt, u16_nmStrt, u16_mapStrt, u16_slvStrt;

  pinMode(19, OUTPUT);
  pinMode(20, OUTPUT);
  digitalWrite(19, HIGH);

  Serial.begin(9600);
  Serial.println(F("delay"));
  delay(2000);
  Serial.println(F("delay over"));


  u16_nmStrt = 10;
  u16_ipStrt = u16_nmStrt + 33; // 43
  u16_slvStrt = u16_ipStrt + 28; // 71
  u16_mapStrt = u16_slvStrt + 181;  // 252

  EEPROM.write(0, highByte(u16_nmStrt));
  EEPROM.write(1, lowByte(u16_nmStrt));
  EEPROM.write(2, highByte(u16_ipStrt));
  EEPROM.write(3, lowByte(u16_ipStrt));
  EEPROM.write(4, highByte(u16_slvStrt));
  EEPROM.write(5, lowByte(u16_slvStrt));
  EEPROM.write(6, highByte(u16_mapStrt));
  EEPROM.write(7, lowByte(u16_mapStrt));
}

void loop() {
  uint16_t u16_ipStrt, u16_nmStrt, u16_mapStrt, u16_mapEnd, u16_slvStrt;
  uint16_t u16_numSlvs;
  bool b_resp;
  char ca_input[50];
  char c_menuSelect;


  // duplicate should make this global
  u16_nmStrt = 10;
  u16_ipStrt = u16_nmStrt + 33; // 43
  u16_slvStrt = u16_ipStrt + 28; // 71
  u16_mapStrt = u16_slvStrt + 181;  // 252

  b_quit = false;

  //term_func(F(""), argFunc, F(""), F(""), ca_input, "", true, 0, false);

  if (b_firstLoop) {
    // start, keep flashing until ready
    term_func(F("Please set line ending to newline.  Ready to start? (Y)"), verFunc, F("OK, let's start! To choose defaults, type \"default.\""),
      F("Please set line ending to newline.  Ready to start? (Y)"), ca_input, "n", false, 5, false);
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
    "l/L: Register library"), menuFunc, F("OK."), F("Sorry, please pick a valid option."), ca_input, "n", false, 0, false);

  c_menuSelect = ca_input[0];

  // pull from ca_input and follow through
  if (c_menuSelect == 'N' || c_menuSelect == 'A') {  // name

      // write name
    term_func(F("Please input a name.  Default was \"UPenn_Modbus_Gateway.\"  There is a 30 character limit."), nmFunc, F("Great name!"),
      F("Please input a name.  Default was \"UPenn_Modbus_Gateway.\"  There is a 30 character limit."), ca_input, "UPenn_Modbus_Gateway", true, 0, false);

    storeName(ca_input, u16_nmStrt);
  }

  if (c_menuSelect == 'I' || c_menuSelect == 'A') {
    // mac
#if defined(CORE_TEENSY)  // if teensy3.0 or greater
    if (!b_quit) {
      read_mac();
      for (int jj = 0; jj < 6; ++jj) {
        EEPROM.write(u16_ipStrt + jj, mac[jj]);
      }

      Serial.print(F("This microcontroller (Teensy) already has a MAC!  It is "));

      if (mac[0] < 16) {
        Serial.print('0');
      }
      Serial.print(mac[0], HEX);
      for (int jj = 1; jj < 6; ++jj) {
        Serial.print(':');
        if (mac[jj] < 16) {
          Serial.print('0');
        }
        Serial.print(mac[jj], HEX);
      }
      Serial.println();
    }
#else
    term_func(F("Please insert number from 1 to 65535 in decimal to be used as last two bytes in MAC."), macFunc, F("Ok, let's move on to the IP."),
      F("Please insert number from 1 to 65535 in decimal to be used as last two bytes in MAC."), ca_input, "0", true, 0, false);

    //storeMac(ca_input, u16_ipStrt);
#endif

    // Gateway IP
    term_func(F("Please insert the device's IP address."), ipFunc, F("Ok, now the subnet mask"),
      F("Please insert IP using X.X.X.X format where X is in [0, 255]."), ca_input, "130.91.138.141", true, 0, false);
    storeIP(ca_input, u16_ipStrt + 6, 4);

    // Subnet mask
    term_func(F("Please insert the device's subnet mask."), ipFunc, F("Ok, now the default gateway"),
      F("Please insert subnet mask using X.X.X.X format where X is in [0, 255]."), ca_input, "255.255.252.0", true, 0, false);
    storeIP(ca_input, u16_ipStrt + 10, 4);

    // default gateway
    term_func(F("Please insert the device's default gateway address."), ipFunc, F("Ok."),
      F("Please insert default gateway using X.X.X.X format where X is in [0, 255]."), ca_input, "130.91.136.1", true, 0, false);
    storeIP(ca_input, u16_ipStrt + 14, 4);
  }

  if (c_menuSelect == 'T' || c_menuSelect == 'A') {
    // yes/no on ntp
    b_resp = term_func(F("Do you want to use an NTP server? (y/n)"), verFunc, F("Ok, let's fill out its IP."),
      F("Ok, now for 485 parameters."), ca_input, "n", true, 0, true);
    storeBool(ca_input, u16_ipStrt + 18);

    // ntp server ip
    if (b_resp) {
      term_func(F("Please insert the address of the NTP server."), ipFunc, F("Ok."),
        F("Please insert IP using X.X.X.X format where X is in [0, 255]."), ca_input, "128.91.3.136", true, 0, false);
    }
    else {
      // write default to this?
      strcpy_P(ca_input, PSTR("128.91.3.136"));
    }
    storeIP(ca_input, u16_ipStrt + 19, 4);
  }

  if (c_menuSelect == 'S' || c_menuSelect == 'A') {
    // baudrate
    term_func(F("Please insert a baudrate for 485 communications."), brFunc, F("Ok."),
      F("The number you entered is outside of the bounds!  Please select one of the following:\n300\n1200\n2400\n4800\n9600\n19200\n31250\n38400\n57600\n115200"),
      ca_input, "9600", true, 0, false);
    storeMedInt(ca_input, u16_ipStrt + 23);

    // timeout
    term_func(F("Please insert a Modbus timeout. (ms)"), toFunc, F("Ok."),
      F("Please insert number from 1 to 30000 in decimal for Modbus timeout."), ca_input, "1500", true, 0, false);
    storeInt(ca_input, u16_ipStrt + 26);
  }

  if (c_menuSelect == 'R' || c_menuSelect == 'A') {
    // record data locally?
    b_resp = term_func(F("Should this meter record data locally?"), verFunc, F("Ok, it will record data."),
      F("Ok, it won't record data."), ca_input, "n", true, 0, true);
    storeBool(ca_input, u16_nmStrt + 31);

    // number of meters to record
    if (b_resp) {
      term_func(F("Please insert number of meters to record (max 20)."), mtrnumFunc, F("Ok."),
        F("Please insert number of meters to record (max 20)."), ca_input, "5", true, 0, false);
      storeByte(ca_input, u16_nmStrt + 32);
    }
    else {
      // default number of meters? (change if outside of bounds)
      if (EEPROM.read(u16_nmStrt + 32) > 20) {
        strcpy_P(ca_input, PSTR("5"));

        storeByte(ca_input, u16_nmStrt + 32);
      }
      // else do nothing
    }
    
  }

  if (c_menuSelect == 'M' || c_menuSelect == 'A') {
    // number of meters listed
    u16_numSlvs = 0;
    term_func(F("Please insert the number of meters actively controlled by the gateway (max 20)."), mtrnumFunc, F("Ok."),
      F("Please insert the number of meters actively controlled by the gateway (max 20)."), ca_input, "0", true, 0, false);

    u16_numSlvs = storeByte(ca_input, u16_slvStrt);

    if (!b_quit) {
      // all meter information
      for (int ii = 0; ii < u16_numSlvs; ++ii) {
        bool b_slaveDataGood = false;
        char ca_slvType[50];
        char ca_slvIp[50];
        char ca_slvId[50];
        char ca_slvVid[50];
        char ca_checkQues[300] = "Is this the correct information for the meter? (y/n)\nType: ";

        Serial.print(F("Meta data for meter "));
        Serial.print(ii + 1, DEC);
        Serial.print(F(" of "));
        Serial.println(u16_numSlvs, DEC);
         
        while (!b_slaveDataGood) {
          // meter type
          term_func(F("Please insert meter type (X.X.X)."), mtrtypFunc, F(""),
            F("Please insert meter type (X.X.X)."), ca_slvType, "12.1.0", false, 0, false);
          //storeIP(ca_input, u16_slvStrt + 9 * (i + 1) - 8, 3);
          strcat(ca_checkQues, ca_slvType);
          

          // 485 or mb/tcp
          b_resp = term_func(F("Is this meter connected via IP? (y/n)"), verFunc, F("This meter is connected via IP."),
            F("This meter is connected via serial comms."), ca_input, "n", false, 0, true);  // nothing to store here

          if (b_resp) {
            // modbus ip
            term_func(F("Please insert the meter's IP."), ipFunc, F(""),
              F("Please insert the meter's IP."), ca_slvIp, "0.0.0.0", false, 0, false);
            strcat(ca_checkQues, "\nIP: ");
            strcat(ca_checkQues, ca_slvIp);
          }
          else {
            // default ip of 0.0.0.0
            strcpy_P(ca_slvIp, PSTR("0.0.0.0"));
            strcat(ca_checkQues, "\nConnected via 485");
          }
          //storeIP(ca_input, u16_slvStrt + 9 * (i + 1) - 5, 4);

          // actual modbus id
          term_func(F("Please insert actual Modbus id. (0-247)"), mbidFunc, F(""),
            F("Please insert actual Modbus id. (0-247)"), ca_slvId, "1", false, 0, false);
          //storeByte(ca_input, u16_slvStrt + 9 * (i + 1) - 1);
          strcat(ca_checkQues, "\nActual Id: ");
          strcat(ca_checkQues, ca_slvId);

          // virtual modbus id
          term_func(F("Please insert virtual Modbus id. (0-247)"), mbidFunc, F(""),
            F("Please insert virtual Modbus id. (0-247)"), ca_slvVid, "1", false, 0, false);
          //storeByte(ca_input, u16_slvStrt + 9 * (i + 1));
          strcat(ca_checkQues, "\nVirtual Id: ");
          strcat(ca_checkQues, ca_slvVid);


          b_slaveDataGood = term_func(F(ca_checkQues), verFunc, F("Great!"),
            F("Please reenter slave data."), ca_input, "n", false, 0, true);  // nothing to store here
        }

        // once broken free from loop, store all the given data
        storeIP(ca_slvType, u16_slvStrt + 9 * (ii + 1) - 8, 3);
        storeIP(ca_slvIp, u16_slvStrt + 9 * (ii + 1) - 5, 4);
        storeByte(ca_slvId, u16_slvStrt + 9 * (ii + 1) - 1);
        storeByte(ca_slvVid, u16_slvStrt + 9 * (ii + 1));
      }
    }
  }

  if (c_menuSelect == 'L' || c_menuSelect == 'A') {
    // write library
    if (!b_quit) {
      u16_mapEnd = writeBlocks(u16_mapStrt);

      Serial.println("Finished writing to EEPROM.");
      Serial.print("indexing stops at byte ");
      Serial.println(u16_mapEnd, DEC);
      digitalWrite(20, HIGH);
    }
  }
  
  if (islower(c_menuSelect)) {
    read_eeprom(c_menuSelect);
  }
    

  // term_func(F(""), Func,    F(""), F(""), ca_input, "", true,    0,           false);
  // term_func(msg,   argFunc, pos,   neg,   ca_input, "", verify?, repeat time, exit on neg)
  Serial.println(F("Setup is complete.\n"));
  b_firstLoop = false;
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
