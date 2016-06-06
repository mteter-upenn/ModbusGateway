
bool term_func(const __FlashStringHelper *msgStr, bool (*argFunc)(char*), const __FlashStringHelper *posStr, 
  const __FlashStringHelper *negStr, char *input, const char *defaultInput, bool verify, uint8_t repeatMsg, bool negExit) {

  uint32_t oldTime, curTime;
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

        if ((*argFunc)(input)) {
          // input is an acceptable value
          uint8_t j;

          Serial.print(F("Input: "));
          for (j = 0; j < 50; j++) {
            if (input[j] == 0) {
              break;
            }
            Serial.print(input[j]);
          }
          Serial.println();

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
              Serial.println(negStr);
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
          Serial.println(negStr);

          if (negExit) {
            return false;
          }
        }
      }
    }

  }

  return false;
}


void checkDefault(char *input, const char *defaultInput) {
  if (!strncmp(input, "default", 7)) {
    // input == "default", replace with defaultInput
    uint8_t i, defLen;
    
    defLen = min(49, strlen(defaultInput));

    for (i = 0; i < defLen; i++) {
      input[i] = defaultInput[i];
    }
    input[i] = 0;
  }
}

bool verFunc(char *input) {  // checks yes/no
  char c = input[0];

  if ((c == 'y') || (c == 'Y')) {
    return true;
  }
  else {
    return false;
  }
}

bool nmFunc(char *input) {  // checks name
  uint8_t i;

  /*for (i = 0; i < 50; i++) {
    if ()
  }*/
  // should probably check for non alphanumeric chars
  input[30] = 0;  // this would put a 0 in the 31st slot, giving a name length of 30
  return true;
}

bool macFunc(char *input) {  // checks mac
  return false;
}

bool ipFunc(char *input) {  // check an ip
  return false;
}

bool brFunc(char *input) {  // checks baud rate
  return false;
}

bool toFunc(char *input) {  // checks modbus timeout
  return false;
}

bool mtrnumFunc(char *input) {  // checks number of meters to record/store in gateway
  return false;
}

bool mtrtypFunc(char *input) {  // checks meter type vs table

}

bool mbidFunc(char *input) {  // checks valid modbus device id

}

void storeIP(char *input, uint16_t regStrt) {
  uint16_t j, k;
  uint8_t u8dum;

  k = 0;
  for (j = 0; j < 4; j++) {
    u8dum = 0;

    while ((input[k] != '.') && (input[k] != 0)) {
      u8dum = u8dum * 10 + (input[k] - '0');
      k++;
    }

    k++;
    EEPROM.write(regStrt + j, u8dum);
  }
}

void storeBool(char *input, uint16_t regStrt) {
  if (input[0] == 'y' || input[0] == 'Y') {
    EEPROM.write(regStrt, true);
  }
  else {
    EEPROM.write(regStrt, false);
  }
}

void storeByte(char *input, uint16_t regStrt) {
  uint16_t k = 0;
  uint8_t u8dum;

  while (input[k] != 0) {
    u8dum = u8dum * 10 + (input[k] - '0');
    k++;
  }

  EEPROM.write(regStrt, u8dum);
}

void storeInt(char *input, uint16_t regStrt) {
  uint16_t k = 0;
  uint16_t u16dum;

  while (input[k] != 0) {
    u16dum = u16dum * 10 + (input[k] - '0');
    k++;
  }

  EEPROM.write(regStrt, highByte(u16dum));
  EEPROM.write(regStrt + 1, lowByte(u16dum));
}

void storeMedInt(char *input, uint16_t regStrt) {
  uint16_t k = 0;
  uint32_t u32dum;

  while (input[k] != 0) {
    u32dum = u32dum * 10 + (input[k] - '0');
    k++;
  }

  EEPROM.write(regStrt, ((u32dum >> 16) & 0xFF));
  EEPROM.write(regStrt + 1, ((u32dum >> 8) & 0xFF));
  EEPROM.write(regStrt + 2, (u32dum & 0xFF));
}