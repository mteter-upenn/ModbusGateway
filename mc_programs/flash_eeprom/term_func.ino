
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

bool verFunc(char *input) {
  char c = input[0];

  if ((c == 'y') || (c == 'Y')) {
    return true;
  }
  else {
    return false;
  }
}

bool nmFunc(char *input) {
  uint8_t i;

  /*for (i = 0; i < 50; i++) {
    if ()
  }*/
  // should probably check for non alphanumeric chars
  input[30] = 0;  // this would put a 0 in the 31st slot, giving a name length of 30
  return true;
}

bool macFunc(char *input) {
  return false;
}

bool ipFunc(char *input) {
  return false;
}

bool brFunc(char *input) {
  return false;
}

bool toFunc(char *input) {
  return false;
}

bool mtrnumFunc(char *input) {
  return false;
}

bool mtrtypFunc(char *input) {

}

bool mbidFunc(char *input) {

}