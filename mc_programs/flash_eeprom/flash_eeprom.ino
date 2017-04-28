


#include <ModbusStructs.h>
#include <ModbusStack.h>
#include <MeterLibrary.h>
//#include <EEPROM.h>
#include "mac.h"
#include <SD.h>
#include <ArduinoJson.h>
#include <FRAM_MB85RS_SPI.h>

#define SERIAL_INPUT 1  // 0 for no serial input, 1 for serial input (mac, ip, name, etc)

#define NEW_GROUP_STYLE 1

#include "globals.h"
#include "read_eeprom.h"
#include "term_func.h"
#include "writeLibrary.h"

void setup() {
//  uint16_t u16_ipStrt, u16_nmStrt, u16_mapStrt, u16_slvStrt;
  Serial.begin(9600);
  Serial.println(F("delay"));
  delay(2000);
  Serial.println(F("delay over"));


  pinMode(19, OUTPUT);
  pinMode(20, OUTPUT);
  digitalWrite(19, HIGH);

  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);

  //  reset w5200 ethernet chip, won't run without this
  pinMode(9, OUTPUT);
  digitalWrite(9, LOW);

  Serial.println(F("Initializing FRAM"));
  if (ExtFRAM.init(FRAM_SS_PIN)) {
    Serial.println("FRAM found");
  }
  else {
    Serial.println("FRAM not found, do nothing");
    while (true) {}
  }

  Serial.println(F("Initializing SD card..."));
  if (!SD.begin(4)) {
    Serial.println(F("ERROR - SD card initialization failed!"));
  }
  else{
    Serial.println(F("SUCCESS - SD card initialized."));
  }



//  u16_nmStrt = 10;
//  u16_ipStrt = u16_nmStrt + 34; // 44
//  u16_slvStrt = u16_ipStrt + 37; // 76   was 32
//  u16_mapStrt = u16_slvStrt + 181;  // 257

//  EEPROM.put(0, u16_nmStrt);
//  EEPROM.put(2, u16_ipStrt);
//  EEPROM.put(4, u16_slvStrt);
//  EEPROM.put(6, u16_mapStrt);

  g_u32_nmStrt = 100;  // data storage starts at byte 100, length 32, ends 131
  g_u32_ipStrt = g_u32_nmStrt + 32; // length 22, ends 153
  g_u32_ntpStrt = g_u32_ipStrt + 22;  // length 5, ends 158
  g_u32_serStrt = g_u32_ntpStrt + 5;  // length 10, ends 168
  g_u32_slvStrt = 256;  // length 1355, ends
  g_u32_mapStrt = 2096;  // unknown length
  g_u32_curStrt = 28672;  // 32k - 4k

  ExtFRAM.write(0, g_u32_nmStrt);
  ExtFRAM.write(4, g_u32_ipStrt);
  ExtFRAM.write(8, g_u32_ntpStrt);
  ExtFRAM.write(12, g_u32_serStrt);
  ExtFRAM.write(16, g_u32_slvStrt);
  ExtFRAM.write(20, g_u32_mapStrt);
  ExtFRAM.write(24, g_u32_curStrt);
}

void loop() {
  uint32_t u32_mapEnd;
  uint16_t u16_numSlvs;
  bool b_resp;
  char ca_input[50];
  char c_menuSelect;

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
    "m/M: Gateway slave list\n"
    "l/L: Register library"), menuFunc, F("OK."), F("Sorry, please pick a valid option."), ca_input, "n", false, 0, false);

  c_menuSelect = ca_input[0];

  // pull from ca_input and follow through
  if (c_menuSelect == 'N' || c_menuSelect == 'A') {  // name

      // write name
    term_func(F("Please input a name. There is a 31 character limit. [\"UPenn_Modbus_Gateway\"]"), nmFunc, F("Great name!"),
      F("Please input a name. There is a 31 character limit. [\"UPenn_Modbus_Gateway\"]"), ca_input, "UPenn_Modbus_Gateway", true, 0, false);

    storeName(ca_input, g_u32_nmStrt);
  }

  if (c_menuSelect == 'I' || c_menuSelect == 'A') {
    // mac
#if defined(CORE_TEENSY)  // if teensy3.0 or greater
    if (!b_quit) {
//      MacArray macStruct;
      uint8_t u8a_mac[6];

      read_mac();
      for (int jj = 0; jj < 6; ++jj) {
//        macStruct.u8a_mac[jj] = mac[jj];
        u8a_mac[jj] = mac[jj];
      }
//      EEPROM.put(u16_ipStrt, macStruct);
      ExtFRAM.writeArray(g_u32_ipStrt, &u8a_mac, 6);

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
    term_func(F("Please insert the device's IP address. [130.91.138.141]"), ipFunc, F("Ok, now the subnet mask"),
      F("Please insert IP using X.X.X.X format. [130.91.138.141]"), ca_input, "130.91.138.141", true, 0, false);
    storeIP(ca_input, g_u32_ipStrt + 6, 4);

    // Subnet mask
    term_func(F("Please insert the device's subnet mask. [255.255.252.0]"), ipFunc, F("Ok, now the default gateway"),
      F("Please insert subnet mask using X.X.X.X format. [255.255.252.0]"), ca_input, "255.255.252.0", true, 0, false);
    storeIP(ca_input, g_u32_ipStrt + 10, 4);

    // default gateway
    term_func(F("Please insert the device's default gateway address. [130.91.136.1]"), ipFunc, F("Ok."),
      F("Please insert default gateway using X.X.X.X format. [130.91.136.1]"), ca_input, "130.91.136.1", true, 0, false);
    storeIP(ca_input, g_u32_ipStrt + 14, 4);

    // tcp socket timeout
    term_func(F("Please insert a TCP socket timeout. [500] (ms)"), tcpToFunc, F("Ok."),
      F("Please insert number from 1 to 30000 in decimal for TCP socket timeout. [500] (ms)"), ca_input, "500", true, 0, false);
    storeInt(ca_input, g_u32_ipStrt + 18);
  }

  if (c_menuSelect == 'T' || c_menuSelect == 'A') {
    // yes/no on ntp
    b_resp = term_func(F("Do you want to use an NTP server? (y/[n])"), verFunc, F("Ok, let's fill out its IP."),
      F("Ok, now for 485 parameters."), ca_input, "n", true, 0, true);
    storeBool(ca_input, g_u32_ntpStrt);

    // ntp server ip
    if (b_resp) {
      term_func(F("Please insert the address of the NTP server. [128.91.3.136]"), ipFunc, F("Ok."),
        F("Please insert IP using X.X.X.X format. [128.91.3.136]"), ca_input, "128.91.3.136", true, 0, false);
    }
    else {
      // write default to this?
      strcpy_P(ca_input, PSTR("128.91.3.136"));
    }
    storeIP(ca_input, g_u32_ntpStrt + 1, 4);
  }

  if (c_menuSelect == 'S' || c_menuSelect == 'A') {
    // baudrate
    term_func(F("Please insert a baudrate for 485 communications. [9600]"), brFunc, F("Ok."),
      F("The number you entered is outside of the bounds!  Please select one of the following:\n300\n1200\n2400\n4800\n[9600]\n19200\n31250\n38400\n57600\n115200"),
      ca_input, "9600", true, 0, false);
    storeInt(ca_input, g_u32_serStrt);

    // data bits
    term_func(F("Please insert number of data bits (7 or 8). [8]"), dbFunc, F("Ok."),
              F("That is not a viable number of data bits, please pick either 7 or 8. [8]"), ca_input, "8", true, 0, false);
    storeByte(ca_input, g_u32_serStrt + 4);

    // parity
    term_func(F("Please insert parity of 485 communications ([0: None], 1: Odd, 2: Even)."), parFunc, F("Ok."),
              F("That is not a viable parity, please pick either [0], 1, or 2 for 'None', 'Odd', or 'Even'."), ca_input, "0", true, 0, false);
    storeByte(ca_input, g_u32_serStrt + 5);

    // stop bits
    term_func(F("Please insert number of stop bits (1 or 2). [1]"), sbFunc, F("Ok."),
              F("That is not a viable number of stop bits, please pick either 1 or 2. [1]"), ca_input, "1", true, 0, false);
    storeByte(ca_input, g_u32_serStrt + 6);

    // modbus timeout
    term_func(F("Please insert a Modbus timeout. [1500] (ms)"), toFunc, F("Ok."),
      F("Please insert number from 1 to 30000 in decimal for Modbus timeout. [1500]"), ca_input, "1500", true, 0, false);
    storeShortInt(ca_input, g_u32_serStrt + 7);

    // yes/no on print modbus comms to sd card
    b_resp = term_func(F("Do you want to save all Modbus communications to an SD card? (y/[n])"), verFunc, F("Ok, they will be saved."),
      F("Ok, they won't be saved."), ca_input, "n", true, 0, true);
    storeBool(ca_input, g_u32_serStrt + 9);
  }

  if (c_menuSelect == 'R' || c_menuSelect == 'A') {
    // poll interval for polling
    term_func(F("Please insert a poll interval (y/n for polling determined by individual slaves). [300,000] (ms)"), timingFunc, F("Ok."),
      F("Please insert a poll interval (y/n for polling determined by individual slaves). [300,000] (ms)"), ca_input, "300000", true, 0, false);
    storeInt(ca_input, g_u32_slvStrt + 1);

    // poll interval for storing history data
    term_func(F("Please insert a poll interval for storing historic data. [600,000] (ms)"), timingFunc, F("Ok."),
      F("Please insert a poll interval for storing historic data. [600,000] (ms)"), ca_input, "600000", true, 0, false);
    storeInt(ca_input, g_u32_slvStrt + 5);
  }

  if (c_menuSelect == 'M' || c_menuSelect == 'A') {
    // number of meters listed
    u16_numSlvs = 0;
    term_func(F("Please insert the number of meters actively controlled by the gateway (max 20). [0]"), mtrnumFunc, F("Ok."),
      F("Please insert the number of meters actively controlled by the gateway (max 20). [0]"), ca_input, "0", true, 0, false);

    u16_numSlvs = storeByte(ca_input, g_u32_slvStrt);

    if (!b_quit) {
      // all meter information
      for (int ii = 0; ii < u16_numSlvs; ++ii) {
        SlaveArray slvStruct;
        slvStruct.u8_flags = 0;  // just to be sure
        bool b_slaveDataGood = false;
        char ca_checkQues[500] = "Is this the correct information for the meter? (y/n)\nName: ";

        Serial.print(F("Meta data for meter "));
        Serial.print(ii + 1, DEC);
        Serial.print(F(" of "));
        Serial.println(u16_numSlvs, DEC);
         
        while (!b_slaveDataGood) {
          char ca_dumName[32];
          sprintf(ca_dumName, "slave %i", ii);

          // write name
          term_func(F("Please input a name. There is a 31 character limit. [\"slave x\"]"), nmFunc, F("Great name!"),
            F("Please input a name. There is a 30 character limit. [\"slave x\"]"), ca_input, ca_dumName, true, 0, false);
          strcat(ca_checkQues, ca_input);
          storeNameRam(ca_input, slvStruct.ca_name);

          // meter type
          term_func(F("Please insert meter type. [12.1.0]"), mtrtypFunc, F(""),
            F("Please insert meter type. [12.1.0]"), ca_input, "12.1.0", false, 0, false);
          strcat(ca_checkQues, "\nType: ");
          strcat(ca_checkQues, ca_input);
          storeIPRam(ca_input, slvStruct.u8a_mtrType, 3);

          // 485 or mb/tcp
          b_resp = term_func(F("Is this meter connected via IP? (y/[n])"), verFunc, F("This meter is connected via IP."),
            F("This meter is connected via serial comms."), ca_input, "n", false, 0, true);  // nothing to store here

          if (b_resp) {
            // modbus ip
            term_func(F("Please insert the meter's IP. [0.0.0.0]"), ipFunc, F(""),
              F("Please insert the meter's IP. [0.0.0.0]"), ca_input, "0.0.0.0", false, 0, false);
            strcat(ca_checkQues, "\nIP: ");
            strcat(ca_checkQues, ca_input);
            storeIPRam(ca_input, slvStruct.u8a_ip, 4);
          }
          else {
            // default ip of 0.0.0.0
            strcpy_P(ca_input, PSTR("0.0.0.0"));
            strcat(ca_checkQues, "\nConnected via 485");
            storeIPRam(ca_input, slvStruct.u8a_ip, 4);
          }

          // actual modbus id
          term_func(F("Please insert actual Modbus id from 0 to 255. [1]"), mbidFunc, F(""),
            F("Please insert actual Modbus id from 0 to 255. [1]"), ca_input, "1", false, 0, false);
          strcat(ca_checkQues, "\nActual Id: ");
          strcat(ca_checkQues, ca_input);
          storeByteRam(ca_input, slvStruct.u8_id);

          // virtual modbus id
          term_func(F("Please insert virtual Modbus id from 0 to 255. [1]"), mbidFunc, F(""),
            F("Please insert virtual Modbus id from 0 to 255. [1]"), ca_input, "1", false, 0, false);
          strcat(ca_checkQues, "\nVirtual Id: ");
          strcat(ca_checkQues, ca_input);
          storeByteRam(ca_input, slvStruct.u8_vid);

          term_func(F("Should this slave be polled? (y//  uint16_t kk = 0;
                      //  uint32_t u32_dum = 0;

                      //  while (cp_input[kk] != 0) {
                      //    u32_dum = u32_dum * 10 + (cp_input[kk] - '0');
                      //    ++kk;
                      //  }

                      //  if (u32_dum > 20) {
                      //    return false;
                      //  }

                      //  Serial.print(F("Input: "));
                      //  Serial.println(u32_dum, DEC);
                      //  return true;
                      //}/[n])"), verFunc, F("Ok, it will poll."),
            F("Ok, it won't be polled."), ca_input, "n", false, 0, true);
          strcat(ca_checkQues, "\nPoll slave?: ");
          strcat(ca_checkQues, ca_input);
          storeFlagRam(ca_input, slvStruct.u8_flags, 0);

          term_func(F("Should this slave store historic data? (y/[n])"), verFunc, F("Ok, it will store data."),
            F("Ok, it won't store data."), ca_input, "n", false, 0, true);
          strcat(ca_checkQues, "\nStore historic data?: ");
          strcat(ca_checkQues, ca_input);
          storeFlagRam(ca_input, slvStruct.u8_flags, 1);



          b_slaveDataGood = term_func(F(ca_checkQues), verFunc, F("Great!"),
            F("Please reenter slave data."), ca_input, "n", false, 0, true);  // nothing to store here
        }

        // once broken free from loop, store all the given data
        storeSlaveStruct(slvStruct, g_u32_slvStrt + 9 * ii + 9);
      }
    }
  }

  if (c_menuSelect == 'L' || c_menuSelect == 'A') {
    // write library
    if (!b_quit) {
      u32_mapEnd = writeBlocks(g_u32_mapStrt);

      if (u16_mapEnd) {
        Serial.println("Finished writing to memory.");
        Serial.print("indexing stops at byte ");
        Serial.println(u32_mapEnd, DEC);
      }
      else {
        Serial.println("There was an error writing the library!");
      }
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
