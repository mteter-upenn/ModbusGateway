#include "term_func.h"
#include <Arduino.h>
#include "globals.h"
#include <EEPROM.h>
#include "ModbusStructs.h"

bool term_func(const __FlashStringHelper *fshp_msgStr, bool (*argFunc)(char*), const __FlashStringHelper *fshp_trueStr,
  const __FlashStringHelper *fshp_falseStr, char *cp_input, const char *kcp_defaultInput, bool b_verify, uint8_t u8_repeatMsgTimeout, 
  bool b_exitOnNeg) {
  //const __FlashStringHelper

//  uint32_t u32_time = millis();

  if (b_quit) {
    return false;
  }
  else {
    uint32_t u32_oldTime(0), u32_curTime(0);
    bool b_ready = false;
    bool b_funcReady = false;
    uint8_t ii = 0;


    if (u8_repeatMsgTimeout) {
      u32_oldTime = millis();
    }

    Serial.println(fshp_msgStr);

    while (!b_ready) {
      // repeat message if desired
//      if ((millis() - u32_time) > 2000) {
//        u32_time = millis();

//        Serial.print("ipStrt [43]: "); Serial.println(word(EEPROM.read(2), EEPROM.read(3)));
//      }

      if (u8_repeatMsgTimeout) {
        u32_curTime = millis();
        if ((u32_curTime - u32_oldTime) > u8_repeatMsgTimeout * 1000) {
          u32_oldTime = u32_curTime;
          Serial.println(fshp_msgStr);
        }
      }

      while (Serial.available()) {
        cp_input[ii] = Serial.read();

        if ((cp_input[ii] == '\n') || (ii == 49)) {
          clearSerialRx();
          cp_input[ii] = 0;
          b_funcReady = true;
        }
        //else {
        //  cp_input[i] = ch;
        //}
        ii++;

        if (b_funcReady) {
          checkDefault(cp_input, kcp_defaultInput);
          b_quit = checkQuit(cp_input);
          if (b_quit) {
            Serial.println(F("\nQuitting setup.  Returning to menu."));
            return false;
          }

          if ((*argFunc)(cp_input)) {  // add print to argFuncs?
            // cp_input is an acceptable value

            if (b_verify) {
              // run term_func with expectation of y/n cp_input,
              char verInput[50];


              if (term_func(F("Verify (y/n)"), verFunc, F("Input Verified"), F("Input Declined"), verInput, "n", false, 0, true)) {
                // user accepted cp_input
              }
              else {
                //  user decided to redo cp_input
                ii = 0;
                b_funcReady = false;
                Serial.println(F("Retry"));
                break;
              }
            }


            Serial.println(fshp_trueStr);
            return true;
          }
          else {
            // cp_input was invalid
            ii = 0;
            b_funcReady = false;


            if (b_exitOnNeg) {
              if (b_verify) {
                // run term_func with expectation of y/n cp_input,
                char verInput[50];

                if (term_func(F("Verify (y/n)"), verFunc, F("Input Verified"), F("Input Declined"), verInput, "n", false, 0, true)) {
                  // user accepted cp_input
                  
                }
                else {
                  //  user decided to redo cp_input
                  ii = 0;
                  b_funcReady = false;
                  Serial.println(F("Retry"));
                  break;
                }
              }
              //Serial.println("exit here");
              Serial.println(fshp_falseStr);
              return false;
            }

            Serial.println(fshp_falseStr);
          }
        }
      }

    }

    return false;
  }
}


void checkDefault(char *cp_input, const char *kcp_defaultInput) {
  if (!strncmp(cp_input, "default", 7)) {
    // cp_input == "default", replace with kcp_defaultInput
    uint8_t ii, u8_defaultLen;
    
    u8_defaultLen = min(49UL, strlen(kcp_defaultInput));

    for (ii = 0; ii < u8_defaultLen; ++ii) {
      cp_input[ii] = kcp_defaultInput[ii];
    }
    cp_input[ii] = 0;
  }
}

bool checkQuit(char *cp_input) {
  if (cp_input[0] == 'q' && cp_input[1] == 0) {
    return true;
  }
  else {
    return false;
  }
}

// b_verify funcs *****************************************************************************************************
bool verFunc(char *cp_input) {  // checks yes/no
  char c_firstChar = cp_input[0];
  Serial.print(F("Input: "));
  Serial.println(c_firstChar);
  if ((c_firstChar == 'y') || (c_firstChar == 'Y')) {
    return true;
  }
  else {
    return false;
  }
}

bool menuFunc(char *cp_input) {
  switch (cp_input[0]) {
  case 't':
  case 'T':
  case 'a':
  case 'A':
  case 'i':
  case 'I':
  case 'n':
  case 'N':
  case 'r':
  case 'R':
  case 's':
  case 'S':
  case 'm':
  case 'M':
  case 'l':
  case 'L':
    return true;
    break;
  default:
    return false;
    break;
  }
  return false;
}

bool nmFunc(char *cp_input) {  // checks name
  uint8_t ii;

  cp_input[31] = 0;  // this would put a 0 in the 32nd slot, giving a name length of 31

  for (ii = 0; ii < 32; ++ii) {
    if (cp_input[ii] == 0) {  // end of string, don't need to check anything else
      break;
    }
    if (!isalnum(cp_input[ii])) {
      switch (cp_input[ii]) {
      case ' ':
      case '_':
      case '-':
      case '.':
        break;
      default:
        return false;  // the character is not alphanumeric, whitespace, underscore, hypen or period
      }
    }
  }

  Serial.print(F("Input: "));
//  for (ii = 0; ii < 32; ++ii) {
//    if (cp_input[ii] == 0) {
//      break;
//    }
//    Serial.print(cp_input[ii]);
//  }
  Serial.println(cp_input);
//  Serial.println();

  return true;
}

bool macFunc(char *cp_input) {  // checks mac
  uint16_t kk = 0;
  uint32_t u32_dum = 0;

  while (cp_input[kk] != 0) {
    u32_dum = u32_dum * 10 + (cp_input[kk] - '0');
    ++kk;
  }

  if (u32_dum > 65535) {
    return false;
  }

  Serial.print(F("Input: 50:45:4E:4E:"));
  if (highByte(u32_dum) < 16) {
    Serial.print('0');
  }
  Serial.print(highByte(u32_dum), HEX);
  Serial.print(":");
  if (lowByte(u32_dum) < 16) {
    Serial.print('0');
  }
  Serial.println(lowByte(u32_dum), HEX);

  return true;
}

bool ipFunc(char *cp_input) {  // check an ip
  uint16_t u16_dum = 0;
  uint16_t jj, kk;

  kk = 0;
  for (jj = 0; jj < 4; ++jj) {
    u16_dum = 0;

    while ((cp_input[kk] != '.') && (cp_input[kk] != 0)) {
      u16_dum = u16_dum * 10 + (cp_input[kk] - '0');
      ++kk;
    }

    if (u16_dum > 255) {
      return false;
    }
    ++kk;
  }

  Serial.print(F("Input: "));
  for (jj = 0; jj < 30; ++jj) {
    if (cp_input[jj] == 0) {
      break;
    }
    Serial.print(cp_input[jj]);
  }
  Serial.println();

  return true;
}

bool brFunc(char *cp_input) {  // checks baud rate
  uint16_t kk = 0;
  uint32_t u32_dum = 0;

  while (cp_input[kk] != 0) {
    u32_dum = u32_dum * 10 + (cp_input[kk] - '0');
    ++kk;
  }
  
  switch (u32_dum) {
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
      Serial.print(F("Input: "));
      Serial.println(u32_dum, DEC);
      return true;
      break;
    default:
      return false;
      break;
  }
}


bool dbFunc(char *cp_input) {
  uint16_t kk = 0;
  uint32_t u32_dum = 0;

  while (cp_input[kk] != 0) {
    u32_dum = u32_dum * 10 + (cp_input[kk] - '0');
    ++kk;
  }

  if (u32_dum == 7 || u32_dum == 8) {
    Serial.print(F("Input: "));
    Serial.println(u32_dum, DEC);
    return true;
  }
  return false;
}


bool parFunc(char *cp_input) {
  uint16_t kk = 0;
  uint32_t u32_dum = 0;

  while (cp_input[kk] != 0) {
    u32_dum = u32_dum * 10 + (cp_input[kk] - '0');
    ++kk;
  }

  if (u32_dum < 3) {
    Serial.print(F("Input: "));

    switch (u32_dum) {
    case 0:
      Serial.println("None (0)");
      break;
    case 1:
      Serial.println("Odd (1)");
      break;
    case 2:
      Serial.println("Even (2)");
      break;
    }
    return true;
  }
  return false;
}


bool sbFunc(char *cp_input) {
  uint16_t kk = 0;
  uint32_t u32_dum = 0;

  while (cp_input[kk] != 0) {
    u32_dum = u32_dum * 10 + (cp_input[kk] - '0');
    ++kk;
  }

  if (u32_dum == 1 || u32_dum == 2) {
    Serial.print(F("Input: "));
    Serial.println(u32_dum, DEC);
    return true;
  }
  return false;
}

bool toFunc(char *cp_input) {  // checks modbus timeout
  uint16_t kk = 0;
  uint32_t u32_dum = 0;

  while (cp_input[kk] != 0) {
    u32_dum = u32_dum * 10 + (cp_input[kk] - '0');
    ++kk;
  }

  if (u32_dum > 30000) {
    return false;
  }
  
  Serial.print(F("Input: "));
  Serial.println(u32_dum, DEC);
  return true;
}

bool mtrnumFunc(char *cp_input) {  // checks number of meters to record/store in gateway
  uint16_t kk = 0;
  uint32_t u32_dum = 0;

  while (cp_input[kk] != 0) {
    u32_dum = u32_dum * 10 + (cp_input[kk] - '0');
    ++kk;
  }

  if (u32_dum > 20) {
    return false;
  }

  Serial.print(F("Input: "));
  Serial.println(u32_dum, DEC);
  return true;
}

bool mtrtypFunc(char *cp_input) {  // checks meter type vs table
  uint16_t u16_dum = 0;
  uint16_t ii, jj, kk;
  uint8_t u8a_mtrTyp[3];
  const uint8_t k_u8_numMtrs(36);
  uint8_t u8a_mtrLib[k_u8_numMtrs][3] = { {0, 1, 1},  // 1
                            {15, 1, 0},
                            {0, 1, 3},
                            {0, 1, 4},
                            {1, 1, 0},
                            {2, 1, 0},
                            {3, 1, 0},
                            {0, 2, 1},
                            {4, 1, 0},
                            {16, 1, 0},  // 10
                            {17, 1, 0},
                            {0, 3, 1},
                            {5, 1, 0},
                            {10, 8, 0},
                            {5, 2, 0},
                            {5, 3, 0},
                            {6, 1, 0},
                            {6, 2, 0},
                            {5, 4, 0},
                            {14, 1, 0},  // 20
                            {14, 2, 0},
                            {7, 1, 0},
                            {13, 1, 0},
                            {8, 1, 0},
                            {9, 1, 0},
                            {9, 2, 0},
                            {9, 3, 0},
                            {10, 1, 0},
                            {10, 2, 0},
                            {10, 3, 0},  // 30
                            {10, 4, 0},
                            {10, 5, 0},
                            {10, 6, 0},
                            {10, 7, 0},
                            {11, 1, 0},
                            {12, 1, 0} };
  char *cpa_mtrNames[] = {"Eaton IQ DP 4000",
    "Eaton PXM 2260",
    "Eaton IQ 200",
    "Eaton IQ 300",
    "Eaton Power Xpert 4000",
    "Emon Dmon 3400",
    "GE EPM 3720",
    "GE EPM 5100",
    "GE PQM",
    "Schneider PM5300",  // 10
    "Schneider PM8000",
    "Siemens 9200",
    "Siemens 9330",
    "Siemens 9340",
    "Siemens 9350",
    "Siemens 9360",
    "Siemens 9510",
    "Siemens 9610",
    "Siemens 9700",
    "Siemens Sentron PAC 4200",  // 20
    "Siemens Sentron PAC 3200",
    "SquareD CM2350",
    "SquareD PM210",
    "SquareD PM710",
    "SquareD Micrologic A Trip Unit",
    "SquareD Micrologic P Trip Unit",
    "SquareD Micrologic H Trip Unit",
    "SquareD CM3350",
    "SquareD CM4000",
    "SquareD CM4250",  // 30
    "SquareD PM800",
    "SquareD PM820",
    "SquareD PM850",
    "SquareD PM870",
    "Chilled Water KEP",
    "Steam KEP"};

  kk = 0;
  for (jj = 0; jj < 3; ++jj) {
    u16_dum = 0;

    while ((cp_input[kk] != '.') && (cp_input[kk] != 0)) {
      u16_dum = u16_dum * 10 + (cp_input[kk] - '0');
      ++kk;
    }
    ++kk;
    u8a_mtrTyp[jj] = u16_dum;
  }

  for (jj = 0; jj < k_u8_numMtrs; ++jj) {
    for (kk = 0; kk < 3; ++kk) {
      if (u8a_mtrTyp[kk] == u8a_mtrLib[jj][kk]) {
        if (kk == 2) {
          Serial.print(F("Input: "));

          for (ii = 0; ii < 30; ++ii) {
            if (cp_input[ii] == 0) {
              break;
            }
            Serial.print(cp_input[ii]);
          }

          Serial.print(", ");
          Serial.println(cpa_mtrNames[jj]);
          return true;
        }
      }
      else {
        break;
      }
    }
  }

  Serial.println(F("Meter options:"));
  for (ii = 0; ii < k_u8_numMtrs; ++ii) {
    Serial.print(cpa_mtrNames[ii]);
    Serial.print(", ");
    Serial.print(u8a_mtrLib[ii][0], DEC);
    Serial.print(".");
    Serial.print(u8a_mtrLib[ii][1], DEC);
    Serial.print(".");
    Serial.println(u8a_mtrLib[ii][2], DEC);
  }
  return false;
}

bool mbidFunc(char *cp_input) {  // checks valid modbus device id
  uint16_t kk = 0;
  uint32_t u32_dum = 0;

  while (cp_input[kk] != 0) {
    u32_dum = u32_dum * 10 + (cp_input[kk] - '0');
    ++kk;
  }

  if (u32_dum > 255) {
    return false;
  }

  Serial.print(F("Input: "));
  Serial.println(u32_dum, DEC);
  return true;
}


// storage functions *******************************************************************************************************
void storeName(char cp_input[32], uint16_t u16_regStrt) {
  if (b_quit) {
    return;
  }
  else {
    NameArray qc_var;

    cp_input[31] = 0;
    memcpy(qc_var.ca_name, cp_input, 32);
//    for (int ii = 0; ii < 32; ++ii) {
//      EEPROM.write(u16_regStrt + ii, cp_input[ii]);

//      if (cp_input[ii] == 0) {
//        break;
//      }
//    }
//    EEPROM.write(u16_regStrt + 31, 0);

    EEPROM.put(u16_regStrt, qc_var.ca_name);
  }
}

void storeIP(char *cp_input, uint16_t u16_regStrt, uint8_t u8_elmts) {
  if (b_quit) {
    return;
  }
  else {
    uint16_t jj, kk;
    uint8_t u8_dum;
    IpArray ipStruct;
    TypeArray typeStruct;

    kk = 0;
    for (jj = 0; jj < u8_elmts; ++jj) {
      u8_dum = 0;

      while ((cp_input[kk] != '.') && (cp_input[kk] != 0)) {
        u8_dum = u8_dum * 10 + (cp_input[kk] - '0');
        ++kk;
      }

      ++kk;
//      EEPROM.write(u16_regStrt + jj, u8_dum);
      if (u8_elmts == 3) {
        typeStruct.u8a_type[jj] = u8_dum;
      }
      else if (u8_elmts == 4) {
        ipStruct.u8a_ip[jj] = u8_dum;
      }

    }

    if (u8_elmts == 3) {
      EEPROM.put(u16_regStrt, typeStruct);
    }
    else if (u8_elmts == 4) {
      EEPROM.put(u16_regStrt, ipStruct);
    }
  }
}

bool storeBool(char *cp_input, uint16_t u16_regStrt) {
  if (b_quit) {
    return false;
  }
  else {
    if (cp_input[0] == 'y' || cp_input[0] == 'Y') {
//      EEPROM.write(u16_regStrt, true);
      EEPROM.put(u16_regStrt, true);
      return true;
    }
    else {
//      EEPROM.write(u16_regStrt, false);
      EEPROM.put(u16_regStrt, false);
      return false;
    }
  }
}

uint8_t storeByte(char *cp_input, uint16_t u16_regStrt) {
  
  if (b_quit) {
    return 0;
  }
  else {
    uint16_t kk = 0;
    uint8_t u8_dum = 0;

    while (cp_input[kk] != 0) {
      u8_dum = u8_dum * 10 + (cp_input[kk] - '0');
      ++kk;
    }

    EEPROM.put(u16_regStrt, u8_dum);

    return u8_dum;
  }
}

uint16_t storeShortInt(char *cp_input, uint16_t u16_regStrt) {
  if (b_quit) {
    return 0;
  }
  else {
    uint16_t kk = 0;
    uint16_t u16_dum = 0;

    while (cp_input[kk] != 0) {
      u16_dum = u16_dum * 10 + (cp_input[kk] - '0');
      ++kk;
    }

//    EEPROM.write(u16_regStrt, highByte(u16_dum));
//    EEPROM.write(u16_regStrt + 1, lowByte(u16_dum));
    EEPROM.put(u16_regStrt, u16_dum);
    return u16_dum;
  }
}

uint32_t storeInt(char *cp_input, uint16_t u16_regStrt) {
  if (b_quit) {
    return 0;
  }
  else {
    uint16_t kk = 0;
    uint32_t u32_dum = 0;

    while (cp_input[kk] != 0) {
      u32_dum = u32_dum * 10 + (cp_input[kk] - '0');
     ++kk;
    }

//    EEPROM.write(u16_regStrt, ((u32_dum >> 16) & 0xFF));
//    EEPROM.write(u16_regStrt + 1, ((u32_dum >> 8) & 0xFF));
//    EEPROM.write(u16_regStrt + 2, (u32_dum & 0xFF));
    EEPROM.put(u16_regStrt, u32_dum);

    return u32_dum;
  }
}
