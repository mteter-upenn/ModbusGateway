#include "read_eeprom.h"
#include <Arduino.h>
#include <EEPROM.h>
#include <ModbusStructs.h>

void read_eeprom(char c_menuChar) {
  uint16_t ii, jj; //  , j;
  uint16_t u16_nmStrt, u16_ipStrt, u16_slvStrt, u16_mapStrt;
  uint32_t u32_baudrate;
  uint16_t u16_timeout;
  uint8_t u8_numSlvs, u8_numMaps;
  //char inpt[50];

  u16_nmStrt = word(EEPROM.read(0), EEPROM.read(1));
  u16_ipStrt = word(EEPROM.read(2), EEPROM.read(3));
  u16_slvStrt = word(EEPROM.read(4), EEPROM.read(5));
  u16_mapStrt = word(EEPROM.read(6), EEPROM.read(7));

  Serial.println();
  if (c_menuChar == 'a') {
    Serial.print(F("u16_nmStrt: "));
    Serial.println(u16_nmStrt);

    Serial.print(F("u16_ipStrt: "));
    Serial.println(u16_ipStrt);

    Serial.print(F("u16_slvStrt: "));
    Serial.println(u16_slvStrt);

    Serial.print(F("u16_mapStrt: "));
    Serial.println(u16_mapStrt);
    Serial.println();
  }

  if (c_menuChar == 'n' || c_menuChar == 'a') {
    // Name:
    Serial.print(F("Name: "));
    for (ii = u16_nmStrt; ii < u16_nmStrt + 31; ++ii) {
      char c_dum = EEPROM.read(ii);

      if (c_dum == 0) {
        Serial.println();
        break;
      }
      else {
        Serial.print(c_dum);
      }
    }
  }

  if (c_menuChar == 'i' || c_menuChar == 'a') {
    // IP info
    Serial.print(F("MAC: "));
    if (EEPROM.read(u16_ipStrt) < 16) {
      Serial.print('0');
    }
    Serial.print(EEPROM.read(u16_ipStrt), HEX);
    for (ii = 1; ii < 6; ++ii) {
      Serial.print(":");
      if (EEPROM.read(u16_ipStrt + ii) < 16) {
        Serial.print('0');
      }
      Serial.print(EEPROM.read(u16_ipStrt + ii), HEX);
    }
    Serial.println();

    Serial.print(F("IP: "));
    Serial.print(EEPROM.read(u16_ipStrt + 6), DEC);
    for (ii = 1; ii < 4; ++ii) {
      Serial.print(".");
      Serial.print(EEPROM.read(u16_ipStrt + 6 + ii), DEC);
    }
    Serial.println();

    Serial.print(F("Subnet Mask: "));
    Serial.print(EEPROM.read(u16_ipStrt + 10), DEC);
    for (ii = 1; ii < 4; ++ii) {
      Serial.print(".");
      Serial.print(EEPROM.read(u16_ipStrt + 10 + ii), DEC);
    }
    Serial.println();

    Serial.print(F("Default Gateway: "));
    Serial.print(EEPROM.read(u16_ipStrt + 14), DEC);
    for (ii = 1; ii < 4; ++ii) {
      Serial.print(".");
      Serial.print(EEPROM.read(u16_ipStrt + 14 + ii), DEC);
    }
    Serial.println();
  }

  if (c_menuChar == 't' || c_menuChar == 'a') {
    // NTP server
    Serial.print(F("Use NTP Server?: "));
    if (EEPROM.read(u16_ipStrt + 18)) {
      Serial.println(F("yes"));
    }
    else {
      Serial.println(F("no"));
    }

    Serial.print(F("NTP Server IP: "));
    Serial.print(EEPROM.read(u16_ipStrt + 19), DEC);
    for (ii = 1; ii < 4; ++ii) {
      Serial.print(".");
      Serial.print(EEPROM.read(u16_ipStrt + 19 + ii), DEC);
    }
    Serial.println('\n');
  }

  if (c_menuChar == 's' || c_menuChar == 'a') {
    // 485/modbus stuff

    Serial.print(F("Baud rate: "));
    u32_baudrate = EEPROM.read(u16_ipStrt + 23);
    u32_baudrate = (uint32_t)((u32_baudrate << 16) | (EEPROM.read(u16_ipStrt + 24) << 8) | (EEPROM.read(u16_ipStrt + 25)));
    Serial.println(u32_baudrate, DEC);

    Serial.print(F("Modbus u16_timeout: "));
    u16_timeout = word(EEPROM.read(u16_ipStrt + 26), EEPROM.read(u16_ipStrt + 27));
    Serial.println(u16_timeout, DEC);
    Serial.println();
  }

  if (c_menuChar == 'r' || c_menuChar == 'a') {
    // record stuff
    Serial.print(F("Record Data?: "));
    if (EEPROM.read(u16_nmStrt + 31)) {
      Serial.println(F("yes"));
    }
    else {
      Serial.println(F("no"));
    }

    Serial.print("Max number of meters to record: ");
    Serial.println(EEPROM.read(u16_nmStrt + 32), DEC);
    Serial.println();
  }
  

  //// Scroll?
  //term_func(F("Continue? (Y)"), verFunc, F(""), F("Continue? (Y)"), inpt, "n", false, 0, false);

  if (c_menuChar == 'm' || c_menuChar == 'a') {
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
    Serial.println(F("|  Schneider PM5300                   |  16.1.0   |"));
    Serial.println(F("|  Schneider PM8000                   |  17.1.0   |"));
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

    // meters listed to this gateway
    Serial.print(F("Meters listed in this gateway: "));
    u8_numSlvs = EEPROM.read(u16_slvStrt);
    if (u8_numSlvs > 20) {
      u8_numSlvs = 0;
    }
    Serial.println(u8_numSlvs, DEC);
    Serial.println();

    for (jj = 0; jj < u8_numSlvs; ++jj) {
      // Meter type
      Serial.print(F("Meter type: "));
      Serial.print(EEPROM.read(u16_slvStrt + jj * 9 + 1), DEC);
      for (ii = 1; ii < 3; ++ii) {
        Serial.print(".");
        Serial.print(EEPROM.read(u16_slvStrt + jj * 9 + 1 + ii), DEC);
      }
      Serial.println();

      // Meter IP
      if (EEPROM.read(u16_slvStrt + jj * 9 + 4) == 0) {
        Serial.println(F("Meter is connected via 485"));
      }
      else {
        Serial.print(F("Meter IP: "));
        Serial.print(EEPROM.read(u16_slvStrt + jj * 9 + 4), DEC);
        for (ii = 1; ii < 4; ++ii) {
          Serial.print(".");
          Serial.print(EEPROM.read(u16_slvStrt + jj * 9 + 4 + ii), DEC);
        }
        Serial.println();
      }

      // Meter actual modbus id
      Serial.print(F("Actual Modbus ID: "));
      Serial.println(EEPROM.read(u16_slvStrt + jj * 9 + 8), DEC);

      // Meter virt ID
      Serial.print(F("Virtual Modbus ID: "));
      Serial.println(EEPROM.read(u16_slvStrt + jj * 9 + 9), DEC);

      Serial.println();
    }
    

    //// Scroll?
    //term_func(F("Continue? (Y)"), verFunc, F(""), F("Continue? (Y)"), inpt, "n", false, 0, false);


    
  }

  if (c_menuChar == 'l' || c_menuChar == 'a') {
    char *cpa_idType[] = { "na", 
      "c_a", "c_b", "c_c", "c_av", "c_t", 
      "ln_a", "ln_b", "ln_c", "ln_av", 
      "ll_ab", "ll_bc", "ll_ca", "ll_av",
      "rl_a", "rl_b", "rl_c", "rl_t",
      "rc_a", "rc_b", "rc_c", "rc_t", 
      "ap_a", "ap_b", "ap_c", "ap_t", 
      "pf_a", "pf_b", "pf_c", "pf_t", 
      "e_rl", "e_rc", "e_ap" };


    // register library
    Serial.print(F("Number of libraries: "));
    u8_numMaps = EEPROM.read(u16_mapStrt + 2);
    Serial.println(u8_numMaps, DEC);
    Serial.println();

    for (ii = 0; ii < u8_numMaps; ++ii) {
      uint16_t u16_slvLibStrt = word(EEPROM.read(u16_mapStrt + 3 + ii * 4), EEPROM.read(u16_mapStrt + 4 + ii * 4));

      Serial.print(F("Library number: "));
      Serial.println(EEPROM.read(u16_mapStrt + 5 + ii * 4), DEC);

      Serial.print(F("Modbus function: "));
      Serial.println(EEPROM.read(u16_mapStrt + 6 + ii * 4), DEC);

      Serial.print(F("Library index: "));
      Serial.println(u16_slvLibStrt, DEC);

//      Serial.print("\tNumber of blocks: ");
//      Serial.println(EEPROM.read(u16_slvLibStrt + 2), DEC);
      uint8_t u8_numBlks = EEPROM.read(u16_slvLibStrt + 2);
      uint8_t u8_numGrps = EEPROM.read(u16_slvLibStrt + 3);
      uint16_t u16_blkStrt = word(EEPROM.read(u16_slvLibStrt), EEPROM.read(u16_slvLibStrt + 1));
      //uint16_t u16_grpStrt = word(EEPROM.read(u16_slvLibStrt + 4), EEPROM.read(u16_slvLibStrt + 5));

      Serial.println("\tBlock\tStart\tEnd\tType");
      for (int jj = 0; jj < u8_numBlks; ++jj) {
        Serial.print("\t"); Serial.print(jj + 1);
        Serial.print("\t"); Serial.print(word(EEPROM.read(u16_blkStrt + jj * 5), EEPROM.read(u16_blkStrt + jj * 5 + 1)));
        Serial.print("\t"); Serial.print(word(EEPROM.read(u16_blkStrt + jj * 5 + 2), EEPROM.read(u16_blkStrt + jj * 5 + 3)));
        Serial.print("\t"); Serial.print(EEPROM.read(u16_blkStrt + jj * 5 + 4));
        Serial.println();
      }
      Serial.println();
      
      //Serial.println("\tGroup\tRegister\tID\tType");
      for (int jj = 0; jj < u8_numGrps - 1; ++jj) {
        uint16_t u16_grpStrt = word(EEPROM.read(u16_slvLibStrt + 4 + jj * 2), EEPROM.read(u16_slvLibStrt + 5 + jj * 2));
        uint8_t u8_grpVals = EEPROM.read(u16_grpStrt);
        uint8_t u8_grpLens = EEPROM.read(u16_grpStrt + 4) - 5;
        uint16_t u16_grpReg = word(EEPROM.read(u16_grpStrt + 2), EEPROM.read(u16_grpStrt + 3));

        Serial.print("\t"); Serial.print("Group ");  Serial.print(jj + 1); Serial.print(" has "); Serial.print(u8_grpVals, DEC);
        Serial.print(" values over "); Serial.print(EEPROM.read(u16_grpStrt + 1), DEC); Serial.println(" registers");
        Serial.println("\tReg\tID\tType\t|\tReg\tID\tType\t|\tReg\tID\tType");
        Serial.println("\t------------------------------------------------------------------------------------");

        int8_t s8_type = static_cast<int8_t>(EEPROM.read(u16_grpStrt + u8_grpLens + 5));
        uint16_t u16_mult = FloatConvEnumNumRegs(Int8_2_FloatConv(s8_type));
        uint8_t u8_dataTypeCmp = EEPROM.read(u16_grpStrt + u8_grpLens + 6);
        uint8_t u8_cmpCntr = 2;

        uint8_t u8_valCntr = 0;

        for (int kk = 0; kk < u8_grpLens; ++kk) {
          int8_t s8_id = static_cast<int8_t>(EEPROM.read(u16_grpStrt + 5 + kk));

          if (s8_id < 0) {
            u16_grpReg -= s8_id;
          }
          else {
            while (u8_valCntr + 1 > u8_dataTypeCmp) {
              u8_dataTypeCmp = EEPROM.read(u16_grpStrt + u8_grpLens + 6 + u8_cmpCntr);
              s8_type = static_cast<int8_t>(EEPROM.read(u16_grpStrt + u8_grpLens + 5 + u8_cmpCntr));
              u16_mult = FloatConvEnumNumRegs(Int8_2_FloatConv(s8_type));
              u8_cmpCntr += 2;
            }

            Serial.print("\t"); Serial.print(u16_grpReg);
            Serial.print("\t"); Serial.print(cpa_idType[s8_id]);  // Serial.print(s8_id, DEC);
            Serial.print("\t"); Serial.print(s8_type, DEC);

            u16_grpReg += u16_mult;

            if ((u8_valCntr % 3 == 2) || (kk == u8_grpLens - 1)) {
              Serial.println();
            }
            else {
              Serial.print("\t|");
            }
            u8_valCntr++;
          }
        }
        Serial.println();

      }

      Serial.print("\t"); Serial.println("Don't forget last group!");
      //Serial.print("\tNumber of groups: ");
      //Serial.println(EEPROM.read(u16_slvLibStrt + 3), DEC);


      Serial.println();
    }
  }
}
