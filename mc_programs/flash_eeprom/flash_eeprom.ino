
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

// PROTOTYPES
bool term_func(const __FlashStringHelper *, bool(*argFunc)(char*), const __FlashStringHelper *,
  const __FlashStringHelper *, char *, const char *, bool, uint8_t, bool);


void setup() {
  uint16_t ip_strt, nm_strt, reg_strt, reg_end, mtr_strt;
  uint16_t i, nm_lgth;
  char nm[31] = "UPenn Modbus Gateway";
#if SERIAL_INPUT == 1
  uint32_t curTime, oldTime;
  char ipStr[17];
  uint8_t ipArr[4];
  uint16_t j, k, u8dum, val_end, numMtrs;
  bool bReady = false;
  bool nmReady = false;
  bool intReady = false;
  bool bResponse;
  char inpt[50];
#endif


  nm_strt = 10;
  ip_strt = nm_strt + 33; // 43
  mtr_strt = ip_strt + 28; // 71
  reg_strt = mtr_strt + 181;  // 252

  pinMode(19, OUTPUT);
  pinMode(20, OUTPUT);
  digitalWrite(19, HIGH);

  Serial.begin(9600);
  Serial.println(F("delay"));
  delay(2000);
  Serial.println(F("delay over"));

#if SERIAL_INPUT == 1
  //term_func(F(""), argFunc, F(""), F(""), inpt, "", true, 0, false);

  // start, keep flashing until ready
  term_func(F("Please set line ending to newline.  Ready to start? (Y)"), verFunc, F("OK, let's start! To choose defaults, type \"default.\""),
    F("Please set line ending to newline.  Ready to start? (Y)"), inpt, "n", false, 5, false);

  // ask where to go
  if (term_func(F("Do you want to write to EEPROM? (y/n)"), verFunc, F("OK, let's write..."), 
    F("OK, let's read..."), inpt, "n", true, 0, true)) {

    // write eeprom

    if (term_func(F("Do you want to write all properties? (y/n)"), verFunc, F("OK, let's write everything..."), 
      F("OK, let's just write the library..."), inpt, "n", true, 0, true)) {

      // write name
      term_func(F("Please input a name.  Default was \"UPenn_Modbus_Gateway.\"  There is a 30 character limit."), nmFunc, F("Great name!"),
        F("Please input a name.  Default was \"UPenn_Modbus_Gateway.\"  There is a 30 character limit."), inpt, "UPenn_Modbus_Gateway", true, 0, false);

      for (i = 0; i < 30; i++) {
        if (inpt[i] == 0) {
          break;
        }
        EEPROM.write(nm_strt + i, inpt[i]);
      }

      // mac
#if defined(CORE_TEENSY)  // if teensy3.0 or greater
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
#else
      term_func(F("Please insert number from 1 to 65535 in decimal to be used as last two bytes in MAC."), macFunc, F("Ok, let's move on to the IP."),
        F("Please insert number from 1 to 65535 in decimal to be used as last two bytes in MAC."), inpt, "0", true, 0, false);


#endif

      // Gateway IP
      term_func(F("Please insert the device's IP address."), ipFunc, F("Ok, now the subnet mask"), 
        F("Please insert IP using X.X.X.X format where X is in [0, 255]."), inpt, "130.91.138.141", true, 0, false);

      storeIP(inpt, ip_strt + 6);

      // Subnet mask
      term_func(F("Please insert the device's subnet mask."), ipFunc, F("Ok, now the default gateway"), 
        F("Please insert subnet mask using X.X.X.X format where X is in [0, 255]."), inpt, "255.255.252.0", true, 0, false);

      storeIP(inpt, ip_strt + 10);

      // default gateway
      term_func(F("Please insert the device's default gateway address."), ipFunc, F("Ok, now the NTP server."), 
        F("Please insert default gateway using X.X.X.X format where X is in [0, 255]."), inpt, "130.91.136.1", true, 0, false);

      storeIP(inpt, ip_strt + 14);

      // yes/no on ntp
      bResponse = term_func(F("Do you want to use an NTP server? (y/n)"), verFunc, F("Ok, let's fill out its IP."), 
        F("Ok, now for 485 parameters."), inpt, "n", true, 0, true);
      if (bResponse) {
        // ntp server ip
        term_func(F("Please insert the device's IP address."), ipFunc, F("Ok, now for 485 parameters."), 
          F("Please insert IP using X.X.X.X format where X is in [0, 255]."), inpt, "128.91.3.136", true, 0, false);

        storeIP(inpt, ip_strt + 19);
      }
      else {
        // write default to this?
      }

      // baudrate
      term_func(F("Please insert a baudrate for 485 communications."), brFunc, F("Ok."),
        F("The number you entered is outside of the bounds!  Please select one of the following:\n300\n1200\n2400\n4800\n9600\n19200\n31250\n38400\n57600\n115200"),
        inpt, "9600", true, 0, false);

      // timeout
      term_func(F("Please insert a Modbus timeout. (ms)"), toFunc, F("Ok."), 
        F("Please insert number from 1 to 30000 in decimal for Modbus timeout."), inpt, "1500", true, 0, false);

      // record data locally?
      bResponse = term_func(F("Should this meter record data locally?"), verFunc, F("Ok, it will record data."),
        F("Ok, it won't record data."), inpt, "n", true, 0, true);
      if (bResponse) {
        // number of meters to record
        term_func(F("Please insert number of meters to record (max 20)."), mtrnumFunc, F("Ok."),
          F("Please insert number of meters to record (max 20)."), inpt, "5", true, 0, false);
      }
      else {
        // default number of meters? (change if outside of bounds)
      }

      numMtrs = 0;
      // number of meters listed
      term_func(F("Please insert the number of meters actively controlled by the gateway (max 20)."), mtrnumFunc, F("Ok."),
        F("Please insert the number of meters actively controlled by the gateway (max 20)."), inpt, "0", true, 0, false);

      

      // all meter information
      for (i = 0; i < numMtrs; i++) {
        Serial.print(F("Meta data for meter "));
        Serial.print(i + 1, DEC);
        Serial.print(F(" of "));
        Serial.println(numMtrs, DEC);

        // meter type
        term_func(F("Please insert meter type (X.X.X)."), mtrtypFunc, F("Ok."),
          F("Please insert meter type (X.X.X)."), inpt, "12.1.0", true, 0, false);

        // 485 or mb/tcp
        bResponse = term_func(F("Is this meter connected via IP? (y/n)"), verFunc, F("This meter is connected via IP."), 
          F("This meter is connected via serial comms."), inpt, "n", true, 0, true);

        if (bResponse) {
          // modbus ip
          term_func(F("Please insert the meter's IP."), ipFunc, F("Ok."), 
            F("Please insert the meter's IP."), inpt, "0.0.0.0", true, 0, false);
        }
        else {
          // default ip of 0.0.0.0
          strcpy_P(inpt, PSTR("0.0.0.0\n"));
        }

        storeIP(inpt, mtr_strt + 9 * (i + 1) - 8);

        // actual modbus id
        term_func(F("Please insert actual Modbus id. (0-247)"), mbidFunc, F("Ok."), 
          F("Please insert actual Modbus id. (0-247)"), inpt, "1", true, 0, false);
        
        // virtual modbus id
        term_func(F("Please insert virtual Modbus id. (0-247)"), mbidFunc, F("Ok."),
          F("Please insert virtual Modbus id. (0-247)"), inpt, "1", true, 0, false);
      }
      
    }
    
    // write library
    //reg_end = writeBlocks(reg_strt);

    //Serial.println("Finished writing to EEPROM.");
    //Serial.print("indexing stops at byte ");
    //Serial.println(reg_end, DEC);
    //digitalWrite(20, HIGH);
  }
  else {
    // read eeprom
    read_eeprom();
  }
  // term_func(F(""), Func, F(""), F(""), inpt, "", true,    0,           false);
  // term_func(msg,   argFunc, pos,     neg, inpt, "", verify?, repeat time, exit on neg)
  Serial.println(F("setup has exited"));
  return;  // AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
   

  oldTime = millis();
  Serial.println(F("Please set line ending to newline.  Ready to start? (Y)"));
  while (!bReady) {
    curTime = millis();
    if ((curTime - oldTime) > 5000) {
      oldTime = curTime;
      Serial.println(F("Please set line ending to newline.  Ready to start? (Y)"));
    }

    if (Serial.available()) {
      char ch = Serial.read();

      if (ch == 'Y' || ch == 'y') {
        bReady = true;

        clearSerialRx();
      }
    }
  }
  bReady = false;
  Serial.println(F("OK, let's start!"));
#else
  Serial.println("Start writing to EEPROM.");
#endif
  

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
   * |  Siemens 9200                       |  0.3.1    |
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
   * |  SquareD CM 2350                    |  7.1.0    |
   * |  SquareD PM 210                     |  13.1.0   |
   * |  SquareD PM 710                     |  8.1.0    |
   * |  SquareD Micrologic A Trip Unit     |  9.1.0    |
   * |  SquareD Micrologic P Trip Unit     |  9.2.0    |
   * |  SquareD Micrologic H Trip Unit     |  9.3.0    |
   * |  SquareD CM 3350                    |  10.1.0   |
   * |  SquareD CM 4000                    |  10.2.0   |
   * |  SquareD CM 4250                    |  10.3.0   |
   * |  SquareD PM 800                     |  10.4.0   |
   * |  SquareD PM 820                     |  10.5.0   |
   * |  SquareD PM 850                     |  10.6.0   |
   * |  SquareD PM 870                     |  10.7.0   |
   * |-------------------------------------|-----------|
   * |  Chilled Water KEP                  |  11.1.0   |
   * |  Steam KEP                          |  12.1.0   |
   * |-------------------------------------|-----------|
   */


  


#if SERIAL_INPUT == 1

#else

#endif

/*
#if SERIAL_INPUT == 1
  Serial.println(F("Do you wish to re/write all parameters? (Y/N)"));
  bool yReady = false;
  bool nReady = false;
  
  while (!bReady){
    while (true) {
      if (Serial.available()) {
        char ch = Serial.read();
  
        if (ch == 'Y' || ch == 'y'){
          yReady = true;
          clearSerialRx();
          Serial.println(F("This will write over anything currently in EEPROM.  Is that OK? (Y/N)"));
          break;
        }
        else {
          read_eeprom();
          while (1) {};
          nReady = true;
          Serial.println(F("This will only write the meter library to EEPROM.  Is that OK?  (Y/N)"));
          clearSerialRx();
          break;
        }
      }
    }

    if (yReady) {
      while (true){
        if (Serial.available()) {
          char ch = Serial.read();
  
          if (ch == 'Y' || ch == 'y'){
            bReady = true;
            
            Serial.println(F("OK!, let's continue..."));
            
            EEPROM.write(0, highByte(nm_strt));
            EEPROM.write(1, lowByte(nm_strt));
            EEPROM.write(2, highByte(ip_strt));
            EEPROM.write(3, lowByte(ip_strt));
            EEPROM.write(4, highByte(mtr_strt));
            EEPROM.write(5, lowByte(mtr_strt));
            EEPROM.write(6, highByte(reg_strt));
            EEPROM.write(7, lowByte(reg_strt));
            clearSerialRx();
            break;
          }
          else {
            yReady = false;
            Serial.println(F("Do you wish to re/write all parameters? (Y/N)"));
            clearSerialRx();
            break;
          }
        }
      }
    }

    if (nReady) {
      while (true){
        if (Serial.available()) {
          char ch = Serial.read();
  
          if (ch == 'Y' || ch == 'y'){
            nReady = false;
            
            Serial.println(F("OK!, let's continue..."));
            
            reg_end = writeBlocks(reg_strt);
          
            Serial.println("Finished writing to EEPROM.");
            Serial.print("indexing stops at byte ");
            Serial.println(reg_end, DEC);
            digitalWrite(20, HIGH);
            clearSerialRx();
            return;
          }
          else {
            nReady = false;
            Serial.println(F("Do you wish to re/write all parameters? (Y/N)"));
            clearSerialRx();
            break;
          }
        }
      }
    }
  }

  Serial.println(F("Please input a name.  Default was \"UPenn Modbus Gateway.\"  There is a 30 character limit."));
  i = 0;
  bReady = false;
  
  while (!bReady){
    while (Serial.available()) {
      nm[i] = Serial.read();
      if (nm[i] == '\n') {
        clearSerialRx();
        nmReady = true;
        nm[i] = 0;
      }
      i++;
      
      if (i > 29) {
        clearSerialRx();
        nmReady = true;
      }
    }

    if (nmReady) {
      Serial.print(F("Please confirm name: \""));
      Serial.print(nm);
      Serial.println(F("\" (Y/N)"));

      while (true) {
        if (Serial.available()) {
          char ch = Serial.read();

          if (ch == 'Y' || ch == 'y'){
            bReady = true;
            clearSerialRx();
            break;
          }
          else {
            nmReady = false;
            i = 0;
            Serial.println(F("Please input a name.  Default was \"UPenn Modbus Gateway.\"  There is a 30 character limit."));
            clearSerialRx();
            break;
          }
        }
      }
    }
  }

  bReady = false;
#else
  EEPROM.write(0, highByte(nm_strt));
  EEPROM.write(1, lowByte(nm_strt));
  EEPROM.write(2, highByte(ip_strt));
  EEPROM.write(3, lowByte(ip_strt));
  EEPROM.write(4, highByte(mtr_strt));
  EEPROM.write(5, lowByte(mtr_strt));
  EEPROM.write(6, highByte(reg_strt));
  EEPROM.write(7, lowByte(reg_strt));
#endif



  
//  Serial.println(strlen(nm), DEC);
  nm_lgth = strlen(nm);
  if (nm_lgth > 30){
    nm_lgth = 30;
  }
  for (i = 0; i < (nm_lgth); i++){
    EEPROM.write((i + nm_strt), nm[i]);
//    Serial.print((i + ver_strt + 3), DEC);
//    Serial.print(": ");
//    Serial.println((char)EEPROM.read((i + nm_str)));
  }
  
  EEPROM.write((nm_strt + nm_lgth), 0);

  EEPROM.write(nm_strt + 31, false);  // this is bRecordData, should be set to false initially
  EEPROM.write(nm_strt + 32, 5);  // max number of meters to record
  
  // Bytes 0-5 are MAC
  //EEPROM.write(ip_strt, 0x50);  // P
  //EEPROM.write(ip_strt + 1, 0x45);  // E
  //EEPROM.write(ip_strt + 2, 0x4E);  // N
  //EEPROM.write(ip_strt + 3, 0x4E);  // N
  */
/*
#if SERIAL_INPUT == 1
#if defined(CORE_TEENSY)  // if teensy3.0 or greater
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
#else
  Serial.println(F("Please insert number from 1 to 65535 in decimal to be used as last two bytes in MAC."));
  uint32_t macEnd = 0;
  
  while (!bReady) {
    while (Serial.available()) {
      char ch = Serial.read();

      if (ch == '\n'){
        intReady = true;
        clearSerialRx();
      }
      else {
        macEnd = macEnd * 10 + (ch - '0');
      }
    }

    if (intReady) {
      if (macEnd < 65536) {
        Serial.print(F("Please confirm MAC: 50:45:4E:4E:"));
        if (highByte(macEnd) < 16) {
          Serial.print('0');
        }
        Serial.print(highByte(macEnd), HEX);
        Serial.print(':');
        if (lowByte(macEnd) < 16) {
          Serial.print('0');
        }
        Serial.print(lowByte(macEnd), HEX);
        Serial.println(F(", (Y/N)"));

        while (true) {
          if (Serial.available()) {
            char ch = Serial.read();
  
            if (ch == 'Y' || ch == 'y'){
              bReady = true;
              EEPROM.write(ip_strt + 4, highByte(macEnd));  // id
              EEPROM.write(ip_strt + 5, lowByte(macEnd));  // id
              clearSerialRx();
              break;
            }
            else {
              intReady = false;
              macEnd = 0;
              Serial.println(F("Please insert number from 1 to 65535 in decimal to be used as last two bytes in MAC."));
              clearSerialRx();
              break;
            }
          }
        }
      }
      else {
        macEnd = 0;
        intReady = false;
        Serial.println(F("The number you entered is outside of the bounds!  Please enter a new one."));
      }
    }
  }
  bReady = false;
#endif
#else
  EEPROM.write(ip_strt + 4, 0x00);  // id
  EEPROM.write(ip_strt + 5, 0x01);  // id
#endif
  
*/
/*
#if SERIAL_INPUT == 1
  Serial.println(F("Please insert IP."));
  i = 0;
  intReady = false;
  val_end = 0;
  while (!bReady) {
    while (Serial.available()) {
      ipStr[i] = Serial.read();
      
      if (ipStr[i] == '\n') {
        ipStr[i] = 0;
        intReady = true;
        clearSerialRx();
        val_end = i;
        break;
      }
      i++;
      if (i > 14) {
        intReady = true;
        clearSerialRx();
        val_end = i + 1;
      }
      
    }

    if (intReady) {
      bool ipGood = true;
      
      k = 0;
      for (j = 0; j < 4; j++) {
        u8dum = 0;

        while ((ipStr[k] != '.') && (k < val_end)) {
          u8dum = u8dum * 10 + (ipStr[k] - '0');
          k++;
        } 
        
        if (u8dum > 255) {
          ipGood = false;
          intReady = false;
          break;
        }
        k++;
        ipArr[j] = u8dum;
      }

      if (ipGood) {
        Serial.print(F("Please confirm IP: "));
        Serial.print(ipArr[0], DEC);
        for (j = 1; j < 4; j++) {
          Serial.print('.');
          Serial.print(ipArr[j], DEC);
        }
        Serial.println(F(", (Y/N)"));
  
        while (true) {
          if (Serial.available()) {
            char ch = Serial.read();
  
            if (ch == 'Y' || ch == 'y'){
              bReady = true;

              for (j = 0; j < 4; j++) {
                EEPROM.write(ip_strt + 6 + j, ipArr[j]);
              }
              clearSerialRx();
              break;
            }
            else {
              intReady = false;
              i = 0;
              Serial.println(F("Please insert IP using X.X.X.X format where X is in [0, 255]."));
              clearSerialRx();
              break;
            }
          }  // end serial.available
        }  // end while

      }
      else {
        i = 0;
        Serial.println(F("Invalid IP! Please insert IP using X.X.X.X format where X is in [0, 255]."));
      }
    }
  }
  bReady = false;
#else
  // Bytes 6-9 are IP
  EEPROM.write(ip_strt + 6, 130);
  EEPROM.write(ip_strt + 7, 91);
  EEPROM.write(ip_strt + 8, 138);
  EEPROM.write(ip_strt + 9, 141);
#endif

  
#if SERIAL_INPUT == 1
  Serial.println(F("Please insert subnet mask."));
  i = 0;
  intReady = false;
  val_end = 0;
  while (!bReady) {
    while (Serial.available()) {
      ipStr[i] = Serial.read();
      //Serial.println(ipStr[i]);
      if (ipStr[i] == '\n') {
        ipStr[i] = 0;
        intReady = true;
        clearSerialRx();
        val_end = i;
        break;
      }
      i++;
      if (i > 14) {
        //Serial.print(i);
        //Serial.print(": ");
        
        intReady = true;
        ipStr[i] = 0;
        ipStr[16] = 0;
        //Serial.println(ipStr[i]);
        clearSerialRx();
        val_end = i;
      }
      
    }

    if (intReady) {
      bool ipGood = true;
      
      k = 0;
      for (j = 0; j < 4; j++) {
        u8dum = 0;

        while ((ipStr[k] != '.') && (k < val_end)) {
          u8dum = u8dum * 10 + (ipStr[k] - '0');
          k++;
        } 
        
        if (u8dum > 255) {
          ipGood = false;
          intReady = false;
          break;
        }
        k++;
        ipArr[j] = u8dum;
      }

      if (ipGood) {
        Serial.print(F("Please confirm subnet mask: "));
        Serial.print(ipArr[0], DEC);
        for (j = 1; j < 4; j++) {
          Serial.print('.');
          Serial.print(ipArr[j], DEC);
        }
        Serial.println(F(", (Y/N)"));
  
        while (true) {
          if (Serial.available()) {
            char ch = Serial.read();
  
            if (ch == 'Y' || ch == 'y'){
              bReady = true;

              for (j = 0; j < 4; j++) {
                EEPROM.write(ip_strt + 10 + j, ipArr[j]);
              }
              clearSerialRx();
              break;
            }
            else {
              intReady = false;
              i = 0;
              Serial.println(F("Please insert subnet mask using X.X.X.X format where X is in [0, 255]."));
              clearSerialRx();
              break;
            }
          }  // end serial.available
        }  // end while

      }
      else {
        i = 0;
        Serial.println(F("Invalid subnet mask! Please insert subnet mask using X.X.X.X format where X is in [0, 255]."));
      }
    }
  }
  bReady = false;
#else
  // Bytes 10-13 are SM
  EEPROM.write(ip_strt + 10, 255);
  EEPROM.write(ip_strt + 11, 255);
  EEPROM.write(ip_strt + 12, 252);
  EEPROM.write(ip_strt + 13, 0);
#endif


#if SERIAL_INPUT == 1
  Serial.println(F("Please insert default gateway."));
  i = 0;
  intReady = false;
  val_end = 0;
  while (!bReady) {
    while (Serial.available()) {
      ipStr[i] = Serial.read();
      
      if (ipStr[i] == '\n') {
        ipStr[i] = 0;
        intReady = true;
        clearSerialRx();
        val_end = i;
        break;
      }
      i++;
      if (i > 14) {
        intReady = true;
        clearSerialRx();
        val_end = i + 1;
      }
      
    }

    if (intReady) {
      bool ipGood = true;
      
      k = 0;
      for (j = 0; j < 4; j++) {
        u8dum = 0;

        while ((ipStr[k] != '.') && (k < val_end)) {
          u8dum = u8dum * 10 + (ipStr[k] - '0');
          k++;
        } 
        
        if (u8dum > 255) {
          ipGood = false;
          intReady = false;
          break;
        }
        k++;
        ipArr[j] = u8dum;
      }

      if (ipGood) {
        Serial.print(F("Please confirm default gateway: "));
        Serial.print(ipArr[0], DEC);
        for (j = 1; j < 4; j++) {
          Serial.print('.');
          Serial.print(ipArr[j], DEC);
        }
        Serial.println(F(", (Y/N)"));
  
        while (true) {
          if (Serial.available()) {
            char ch = Serial.read();
  
            if (ch == 'Y' || ch == 'y'){
              bReady = true;

              for (j = 0; j < 4; j++) {
                EEPROM.write(ip_strt + 14 + j, ipArr[j]);
              }
              clearSerialRx();
              break;
            }
            else {
              intReady = false;
              i = 0;
              Serial.println(F("Please insert default gateway using X.X.X.X format where X is in [0, 255]."));
              clearSerialRx();
              break;
            }
          }  // end serial.available
        }  // end while

      }
      else {
        i = 0;
        Serial.println(F("Invalid default gateway! Please insert default gateway using X.X.X.X format where X is in [0, 255]."));
      }
    }
  }
  bReady = false;
  
#else
  // Bytes 14-17 are DG
  EEPROM.write(ip_strt + 14, 130);
  EEPROM.write(ip_strt + 15, 91);
  EEPROM.write(ip_strt + 16, 136);
  EEPROM.write(ip_strt + 17, 1);
#endif

//ntp server ip

  EEPROM.write(ip_strt + 18, false);  // use ntp or not

  EEPROM.write(ip_strt + 19, 128);  // ntp server ip
  EEPROM.write(ip_strt + 20, 91);
  EEPROM.write(ip_strt + 21, 3);
  EEPROM.write(ip_strt + 22, 136);
  */

#if SERIAL_INPUT == 1
  Serial.println(F("Please enter a baudrate for 485 communications."));

  uint32_t bdrt = 0;
  intReady = false;
  
  while (!bReady) {
    while (Serial.available()) {
      char ch = Serial.read();

      if (ch == '\n'){
        intReady = true;
        clearSerialRx();
      }
      else {
        bdrt = bdrt * 10 + (ch - '0');
      }
    }

    if (intReady) {
      bool bdReady = false;

      switch (bdrt) {
        case 300:
        case 1200:
        case 2400:
        case 4800:
        case 9600:
        case 19200:
        case 31250:
        case 38400:
        case 57600:
        case 115200:
          bdReady = true;
          break;
        default:
          break;
      }
      
      if (bdReady) {
        Serial.print(F("Please confirm baudrate: "));
        Serial.print(bdrt, DEC);
        Serial.println(F(", (Y/N)"));

        while (true) {
          if (Serial.available()) {
            char ch = Serial.read();
  
            if (ch == 'Y' || ch == 'y'){
              bReady = true;

              EEPROM.write(ip_strt + 23, (bdrt >> 16) & 0xFF);
              EEPROM.write(ip_strt + 24, (bdrt >> 8) & 0xFF);
              EEPROM.write(ip_strt + 25, bdrt & 0xFF);
              clearSerialRx();
              break;
            }
            else {
              intReady = false;
              bdReady = false;
              bdrt = 0;
              Serial.println(F("Please enter a baudrate for 485 communications."));
              clearSerialRx();
              break;
            }
          }
        }
      }
      else {
        bdrt = 0;
        intReady = false;
        Serial.println(F("The number you entered is outside of the bounds!  Please select one of the following:\n300\n1200\n2400\n4800\n9600\n19200\n31250\n38400\n57600\n115200"));
      }
    }
  }
  bReady = false;  
#else
  // Bytes 18-20 are BR
  EEPROM.write(ip_strt + 23, 0);
  EEPROM.write(ip_strt + 24, highByte(9600));
  EEPROM.write(ip_strt + 25, lowByte(9600));
#endif


#if SERIAL_INPUT == 1
  Serial.println(F("Please enter a Modbus timeout (2000ms is typical)."));

  uint32_t tmout = 0;
  intReady = false;
  
  while (!bReady) {
    while (Serial.available()) {
      char ch = Serial.read();

      if (ch == '\n'){
        intReady = true;
        clearSerialRx();
      }
      else {
        tmout = tmout * 10 + (ch - '0');
      }
    }

    if (intReady) {
      if (tmout < 30000) {
        Serial.print(F("Please confirm Modbus timeout: "));
        Serial.print(tmout, DEC);
        Serial.println(F(", (Y/N)"));

        while (true) {
          if (Serial.available()) {
            char ch = Serial.read();
  
            if (ch == 'Y' || ch == 'y'){
              bReady = true;
              EEPROM.write(ip_strt + 26, (tmout >> 8) & 0xFF);
              EEPROM.write(ip_strt + 27, tmout & 0xFF);
              clearSerialRx();
              break;
            }
            else {
              intReady = false;
              tmout = 0;
              Serial.println(F("Please insert number from 1 to 30000 in decimal for Modbus timeout."));
              clearSerialRx();
              break;
            }
          }
        }
      }
      else {
        tmout = 0;
        intReady = false;
        Serial.println(F("The number you entered is outside of the bounds!  Please enter a new one."));
      }
    }
  }
  bReady = false;  
#else
  // Bytes 21-22 are T/O
  EEPROM.write(ip_strt + 26, highByte(2000));
  EEPROM.write(ip_strt + 27, lowByte(2000));
#endif


#if SERIAL_INPUT == 0
//  Serial.println(F("Do you want to 
  if (EEPROM.read(mtr_strt) > 20) { 
    EEPROM.write(mtr_strt, 0); 
  }
#else
  // meter version/type
  // ALL DEVICE IDS MUST BE UNIQUE REGARDLESS OF COMM PROTOCOL USED
  // this creates a psuedo modbus network over 485 and ethernet
  EEPROM.write(mtr_strt, 1);  // number of meters
  
  EEPROM.write(mtr_strt + 1, 12);  // meter type
  EEPROM.write(mtr_strt + 2, 1);
  EEPROM.write(mtr_strt + 3, 0);
  EEPROM.write(mtr_strt + 4, 10);  // ip  (all zero if connected via 485)
  EEPROM.write(mtr_strt + 5, 166);
  EEPROM.write(mtr_strt + 6, 4);
  EEPROM.write(mtr_strt + 7, 34);
  EEPROM.write(mtr_strt + 8, 10);  // actual id
  EEPROM.write(mtr_strt + 9, 1);  // virtual id

  //EEPROM.write(mtr_strt + 9, 12);  // meter type
  //EEPROM.write(mtr_strt + 10, 1);
  //EEPROM.write(mtr_strt + 11, 0);
  //EEPROM.write(mtr_strt + 12, 0);  // either ip or last byte is mb id  (all others 0)
  //EEPROM.write(mtr_strt + 13, 0);
  //EEPROM.write(mtr_strt + 14, 0);
  //EEPROM.write(mtr_strt + 15, 0);
  //EEPROM.write(mtr_strt + 16, 15);
  
  //EEPROM.write(mtr_strt + 17, 10);  // meter type
  //EEPROM.write(mtr_strt + 18, 4);
  //EEPROM.write(mtr_strt + 19, 0);
  //EEPROM.write(mtr_strt + 20, 165);  // either ip or last byte is mb id  (all others 0)
  //EEPROM.write(mtr_strt + 21, 123);
  //EEPROM.write(mtr_strt + 22, 7);
  //EEPROM.write(mtr_strt + 23, 217);
  //EEPROM.write(mtr_strt + 24, 5);

  //EEPROM.write(mtr_strt + 25, 12);  // meter type
  //EEPROM.write(mtr_strt + 26, 1);
  //EEPROM.write(mtr_strt + 27, 0);
  //EEPROM.write(mtr_strt + 28, 10);  // either ip or last byte is mb id  (all others 0)
  //EEPROM.write(mtr_strt + 29, 166);
  //EEPROM.write(mtr_strt + 30, 4);
  //EEPROM.write(mtr_strt + 31, 34);
  //EEPROM.write(mtr_strt + 32, 10);

  //EEPROM.write(mtr_strt + 33, 14);  // meter type
  //EEPROM.write(mtr_strt + 34, 1);
  //EEPROM.write(mtr_strt + 35, 0);
  //EEPROM.write(mtr_strt + 36, 130);  // either ip or last byte is mb id  (all others 0)
  //EEPROM.write(mtr_strt + 37, 91);
  //EEPROM.write(mtr_strt + 38, 105);
  //EEPROM.write(mtr_strt + 39, 12);
  //EEPROM.write(mtr_strt + 40, 1);
  
/*
  EEPROM.write(mtr_strt + , );  // meter type
  EEPROM.write(mtr_strt + , );
  EEPROM.write(mtr_strt + , );
  EEPROM.write(mtr_strt + , );  // either ip or last byte is mb id  (all others 0)
  EEPROM.write(mtr_strt + , );
  EEPROM.write(mtr_strt + , );
  EEPROM.write(mtr_strt + , );
  EEPROM.write(mtr_strt + , );
*/
#endif
  
  reg_end = writeBlocks(reg_strt);

  Serial.println("Finished writing to EEPROM.");
  Serial.print("indexing stops at byte ");
  Serial.println(reg_end, DEC);
  digitalWrite(20, HIGH);
}

void loop() {
  //Serial.println(F("setup has exited"));
  //delay(2000);
}

void clearSerialRx() {
  while(Serial.available()){
    Serial.read();
  }
}

