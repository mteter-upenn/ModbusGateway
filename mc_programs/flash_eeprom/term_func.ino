
bool term_func(const __FlashStringHelper *msgStr, bool (*argFunc)(char*), const __FlashStringHelper *posStr,
  const __FlashStringHelper *negStr, char *input, const char *defaultInput, bool verify, uint8_t repeatMsg, bool negExit) {
  //const __FlashStringHelper
  if (bQuit) {
    return false;
  }
  else {
    uint32_t oldTime(0), curTime(0);
    bool bReady = false;
    bool funcReady = false;
    uint8_t i = 0;


    if (repeatMsg) {
      oldTime = millis();
    }

    Serial.println(msgStr);

    while (!bReady) {
      // repeat message if desired
      if (repeatMsg) {
        curTime = millis();
        if ((curTime - oldTime) > repeatMsg * 1000) {
          oldTime = curTime;
          Serial.println(msgStr);
        }
      }

      while (Serial.available()) {
        input[i] = Serial.read();

        if ((input[i] == '\n') || (i == 49)) {
          clearSerialRx();
          input[i] = 0;
          funcReady = true;
        }
        //else {
        //  input[i] = ch;
        //}
        i++;

        if (funcReady) {
          checkDefault(input, defaultInput);
          bQuit = checkQuit(input);
          if (bQuit) {
            Serial.println(F("\nQuitting setup.  Returning to menu."));
            return false;
          }

          if ((*argFunc)(input)) {  // add print to argFuncs?
            // input is an acceptable value

            if (verify) {
              // run term_func with expectation of y/n input,
              char verInput[50];


              if (term_func(F("Verify (y/n)"), verFunc, F("Input Verified"), F("Input Declined"), verInput, "n", false, 0, true)) {
                // user accepted input
              }
              else {
                //  user decided to redo input
                i = 0;
                funcReady = false;
                Serial.println(F("Retry"));
                break;
              }
            }


            Serial.println(posStr);
            return true;
          }
          else {
            // input was invalid
            i = 0;
            funcReady = false;


            if (negExit) {
              if (verify) {
                // run term_func with expectation of y/n input,
                char verInput[50];

                if (term_func(F("Verify (y/n)"), verFunc, F("Input Verified"), F("Input Declined"), verInput, "n", false, 0, true)) {
                  // user accepted input
                  
                }
                else {
                  //  user decided to redo input
                  i = 0;
                  funcReady = false;
                  Serial.println(F("Retry"));
                  break;
                }
              }
              //Serial.println("exit here");
              Serial.println(negStr);
              return false;
            }

            Serial.println(negStr);
          }
        }
      }

    }

    return false;
  }
}


void checkDefault(char *input, const char *defaultInput) {
  if (!strncmp(input, "default", 7)) {
    // input == "default", replace with defaultInput
    uint8_t i, defLen;
    
    defLen = min(49UL, strlen(defaultInput));

    for (i = 0; i < defLen; i++) {
      input[i] = defaultInput[i];
    }
    input[i] = 0;
  }
}

bool checkQuit(char *input) {
  if (input[0] == 'q' && input[1] == 0) {
    return true;
  }
  else {
    return false;
  }
}

// verify funcs *****************************************************************************************************
bool verFunc(char *input) {  // checks yes/no
  char c = input[0];
  Serial.print(F("Input: "));
  if ((c == 'y') || (c == 'Y')) {
    Serial.println(input[0]);
    return true;
  }
  else {
    Serial.println(input[0]);
    return false;
  }
}

bool menuFunc(char *input) {
  switch (input[0]) {
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

bool nmFunc(char *input) {  // checks name
  uint8_t i;

  input[30] = 0;  // this would put a 0 in the 31st slot, giving a name length of 30

  for (i = 0; i < 30; i++) {
    if (input[i] == 0) {  // end of string, don't need to check anything else
      break;
    }
    if (!isalnum(input[i])) {
      switch (input[i]) {
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
  for (i = 0; i < 30; i++) {
    if (input[i] == 0) {
      break;
    }
    Serial.print(input[i]);
  }
  Serial.println();

  return true;
}

bool macFunc(char *input) {  // checks mac
  uint16_t k = 0;
  uint32_t u32dum = 0;

  while (input[k] != 0) {
    u32dum = u32dum * 10 + (input[k] - '0');
    k++;
  }

  if (u32dum > 65535) {
    return false;
  }

  Serial.print(F("Input: 50:45:4E:4E:"));
  if (highByte(u32dum) < 16) {
    Serial.print('0');
  }
  Serial.print(highByte(u32dum), HEX);
  Serial.print(":");
  if (lowByte(u32dum) < 16) {
    Serial.print('0');
  }
  Serial.println(lowByte(u32dum), HEX);

  return true;
}

bool ipFunc(char *input) {  // check an ip
  uint16_t u16dum = 0;
  uint16_t j, k;

  k = 0;
  for (j = 0; j < 4; j++) {
    u16dum = 0;

    while ((input[k] != '.') && (input[k] != 0)) {
      u16dum = u16dum * 10 + (input[k] - '0');
      k++;
    }

    if (u16dum > 255) {
      return false;
    }
    k++;
  }

  Serial.print(F("Input: "));
  for (j = 0; j < 30; j++) {
    if (input[j] == 0) {
      break;
    }
    Serial.print(input[j]);
  }
  Serial.println();

  return true;
}

bool brFunc(char *input) {  // checks baud rate
  uint16_t k = 0;
  uint32_t u32dum = 0;

  while (input[k] != 0) {
    u32dum = u32dum * 10 + (input[k] - '0');
    k++;
  }
  
  switch (u32dum) {
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
      Serial.println(u32dum, DEC);
      return true;
      break;
    default:
      return false;
      break;
  }
}

bool toFunc(char *input) {  // checks modbus timeout
  uint16_t k = 0;
  uint32_t u32dum = 0;

  while (input[k] != 0) {
    u32dum = u32dum * 10 + (input[k] - '0');
    k++;
  }

  if (u32dum > 30000) {
    return false;
  }
  
  Serial.print(F("Input: "));
  Serial.println(u32dum, DEC);
  return true;
}

bool mtrnumFunc(char *input) {  // checks number of meters to record/store in gateway
  uint16_t k = 0;
  uint32_t u32dum = 0;

  while (input[k] != 0) {
    u32dum = u32dum * 10 + (input[k] - '0');
    k++;
  }

  if (u32dum > 20) {
    return false;
  }

  Serial.print(F("Input: "));
  Serial.println(u32dum, DEC);
  return true;
}

bool mtrtypFunc(char *input) {  // checks meter type vs table
  uint16_t u16dum = 0;
  uint16_t i, j, k;
  uint8_t mtrTyp[3];
  uint8_t mtrLib[34][3] = { {0, 1, 1},  // 1
                            {15, 1, 0},
                            {0, 1, 3},
                            {0, 1, 4},
                            {1, 1, 0},
                            {2, 1, 0},
                            {3, 1, 0},
                            {0, 2, 1},
                            {4, 1, 0},
                            {0, 3, 1},  // 10
                            {5, 1, 0},
                            {10, 8, 0},
                            {5, 2, 0},
                            {5, 3, 0},
                            {6, 1, 0},
                            {6, 2, 0},
                            {5, 4, 0},
                            {14, 1, 0},
                            {14, 2, 0},
                            {7, 1, 0},  // 20
                            {13, 1, 0},
                            {8, 1, 0},
                            {9, 1, 0},
                            {9, 2, 0},
                            {9, 3, 0},
                            {10, 1, 0},
                            {10, 2, 0},
                            {10, 3, 0},
                            {10, 4, 0},
                            {10, 5, 0},  // 30
                            {10, 6, 0},
                            {10, 7, 0},
                            {11, 1, 0},
                            {12, 1, 0} };
  char *mtrNames[] = {"Eaton IQ DP 4000",
    "Eaton PXM 2260",
    "Eaton IQ 200",
    "Eaton IQ 300",
    "Eaton Power Xpert 4000",
    "Emon Dmon 3400",
    "GE EPM 3720",
    "GE EPM 5100",
    "GE PQM",
    "Siemens 9200",  // 10
    "Siemens 9330",
    "Siemens 9340",
    "Siemens 9350",
    "Siemens 9360",
    "Siemens 9510",
    "Siemens 9610",
    "Siemens 9700",
    "Siemens Sentron PAC 4200",
    "Siemens Sentron PAC 3200",
    "SquareD CM 2350",  // 20
    "SquareD PM 210",
    "SquareD PM 710",
    "SquareD Micrologic A Trip Unit",
    "SquareD Micrologic P Trip Unit",
    "SquareD Micrologic H Trip Unit",
    "SquareD CM 3350",
    "SquareD CM 4000",
    "SquareD CM 4250",
    "SquareD PM 800",
    "SquareD PM 820",  // 30
    "SquareD PM 850",
    "SquareD PM 870",
    "Chilled Water KEP",
    "Steam KEP"};

  k = 0;
  for (j = 0; j < 3; j++) {
    u16dum = 0;

    while ((input[k] != '.') && (input[k] != 0)) {
      u16dum = u16dum * 10 + (input[k] - '0');
      k++;
    }
    k++;
    mtrTyp[j] = u16dum;
  }

  for (j = 0; j < 34; j++) {
    for (k = 0; k < 3; k++) {
      if (mtrTyp[k] == mtrLib[j][k]) {
        if (k == 2) {
          Serial.print(F("Input: "));

          for (i = 0; i < 30; i++) {
            if (input[i] == 0) {
              break;
            }
            Serial.print(input[i]);
          }

          Serial.print(", ");
          Serial.println(mtrNames[j]);
          return true;
        }
      }
      else {
        break;
      }
    }
  }

  Serial.println(F("Meter options:"));
  for (i = 0; i < 34; i++) {
    Serial.print(mtrNames[i]);
    Serial.print(", ");
    Serial.print(mtrLib[i][0], DEC);
    Serial.print(".");
    Serial.print(mtrLib[i][1], DEC);
    Serial.print(".");
    Serial.println(mtrLib[i][2], DEC);
  }
  return false;
}

bool mbidFunc(char *input) {  // checks valid modbus device id
  uint16_t k = 0;
  uint32_t u32dum = 0;

  while (input[k] != 0) {
    u32dum = u32dum * 10 + (input[k] - '0');
    k++;
  }

  if (u32dum > 247) {
    return false;
  }

  Serial.print(F("Input: "));
  Serial.println(u32dum, DEC);
  return true;
}


// storage functions *******************************************************************************************************
void storeName(char *input, uint16_t regStrt) {
  if (bQuit) {
    return;
  }
  else {
    uint8_t i;

    for (i = 0; i < 30; i++) {
      EEPROM.write(regStrt + i, input[i]);

      if (input[i] == 0) {
        break;
      }
    }
  }
}

void storeIP(char *input, uint16_t regStrt, uint8_t elmts) {
  if (bQuit) {
    return;
  }
  else {
    uint16_t j, k;
    uint8_t u8dum;

    k = 0;
    for (j = 0; j < elmts; j++) {
      u8dum = 0;

      while ((input[k] != '.') && (input[k] != 0)) {
        u8dum = u8dum * 10 + (input[k] - '0');
        k++;
      }

      k++;
      EEPROM.write(regStrt + j, u8dum);
    }
  }
}

bool storeBool(char *input, uint16_t regStrt) {
  if (bQuit) {
    return false;
  }
  else {
    if (input[0] == 'y' || input[0] == 'Y') {
      EEPROM.write(regStrt, true);

      return true;
    }
    else {
      EEPROM.write(regStrt, false);

      return false;
    }
  }
}

uint8_t storeByte(char *input, uint16_t regStrt) {
  
  if (bQuit) {
    return 0;
  }
  else {
    uint16_t k = 0;
    uint8_t u8dum = 0;

    while (input[k] != 0) {
      u8dum = u8dum * 10 + (input[k] - '0');
      k++;
    }

    EEPROM.write(regStrt, u8dum);

    return u8dum;
  }
}

uint16_t storeInt(char *input, uint16_t regStrt) {
  if (bQuit) {
    return 0;
  }
  else {
    uint16_t k = 0;
    uint16_t u16dum = 0;

    while (input[k] != 0) {
      u16dum = u16dum * 10 + (input[k] - '0');
      k++;
    }

    EEPROM.write(regStrt, highByte(u16dum));
    EEPROM.write(regStrt + 1, lowByte(u16dum));

    return u16dum;
  }
}

uint32_t storeMedInt(char *input, uint16_t regStrt) {
  if (bQuit) {
    return 0;
  }
  else {
    uint16_t k = 0;
    uint32_t u32dum = 0;

    while (input[k] != 0) {
      u32dum = u32dum * 10 + (input[k] - '0');
      k++;
    }

    EEPROM.write(regStrt, ((u32dum >> 16) & 0xFF));
    EEPROM.write(regStrt + 1, ((u32dum >> 8) & 0xFF));
    EEPROM.write(regStrt + 2, (u32dum & 0xFF));

    return u32dum;
  }
}