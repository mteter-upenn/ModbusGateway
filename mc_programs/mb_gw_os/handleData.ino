void handle_data() {
  uint32_t curDataTime;

  curDataTime = millis();
  
  if ((curDataTime - oldDataTime) > 300000UL) {  // 300000UL wait every 5 minutes 300000UL
    uint8_t numSlvs;
    uint8_t i, j;
    uint8_t dev;
    uint8_t func;
    uint8_t meter;
    uint16_t lclmtr_strt;
    uint16_t grp_strt;
    uint8_t grp_len;
    uint16_t pwrReg;
    uint16_t egyReg;
    uint16_t stmRegs[4] = {2, 8, 14, 32};  // lbs/hr, T, P, lbs
    uint16_t chwRegs[5] = {0, 6, 8, 10, 30};  // tons, gpm, Ts, Tr, tonhrs
    //uint16_t grp_adr;
    uint8_t mbStat;
    uint8_t in_mb[12];
    uint8_t out_mb[MB_ARR_SIZE];
    uint16_t out_len;
    char streamBuf[150] = {0};
    time_t t = now();
    File dataFile;
    char fileName[31] = {0};

    union convertUnion
    {
      float f;
      uint8_t u8[4];
    } int2flt;

    oldDataTime = curDataTime;
    numSlvs = slaves > maxSlvsRcd ? maxSlvsRcd : slaves;
    memset(in_mb, 0, 5);
    in_mb[5] = 6;     // length of modbus half

    fileName[30] = 0;
    digitalWrite(sdWriteLed, HIGH);
    getFileName(t, fileName);  // gets fileName and prints header (if needed) and first column (date)

    dataFile = SD.open(fileName, FILE_WRITE);
    printTime(t);
    if (dataFile) {
      Serial.println(F("opened file"));

      for (i = 0; i < numSlvs; i++) {
        meter = slv_typs[i][0];
        dev = slv_vids[i];
        func = EEPROM.read(reg_strt + 4 * meter + 2);
        lclmtr_strt = word(EEPROM.read(reg_strt + 4 * meter - 1), EEPROM.read(reg_strt + 4 * meter));

        grp_strt = word(EEPROM.read(lclmtr_strt + 3), EEPROM.read(lclmtr_strt + 4));
        grp_len = EEPROM.read(lclmtr_strt + 5);
        //grp_adr = grp_strt;

        in_mb[6] = dev;   // device id
        in_mb[7] = func;  // modbus function
        in_mb[10] = 0;  // assume no length higher than 255
        in_mb[11] = 2;  // ask for float conversion = 2*num for registers

        delay(5); // ensure long enough delay between polls

        switch (meter) {
          case 11:  // chw
            in_mb[8] = 0; // highByte(chwRegs[j]);  // should always be 0
            in_mb[9] = 0; // lowByte(chwRegs[j]);
            in_mb[11] = 34;  // ask for float conversion = 2*num for registers

            //delay(1); // ensure long enough delay between polls
            mbStat = getModbus(in_mb, 12, out_mb, out_len);
            // {0, 6, 8, 10, 30};
            for (j = 0; j < 5; j++) {
              if (mbStat) {  // good message
                int2flt.u8[3] = out_mb[2 * chwRegs[j] + 11];
                int2flt.u8[2] = out_mb[2 * chwRegs[j] + 12];
                int2flt.u8[1] = out_mb[2 * chwRegs[j] + 9]; // high word
                int2flt.u8[0] = out_mb[2 * chwRegs[j] + 10];  // low word

                strcat_P(streamBuf, PSTR(","));
                if (j < 4) {
                  dtostrf(int2flt.f, 1, 3, (streamBuf + strlen(streamBuf)));
                }
                else {  // have totalizer use less digits after decimal point
                  dtostrf(int2flt.f, 1, 1, (streamBuf + strlen(streamBuf)));
                }
              }
              else if (j == 0) {  // bad message on first try, just skip everything to avoid wasting time
                strcat_P(streamBuf, PSTR(",error,error,error,error,error"));
                break;
              }
              else {  // bad message
                strcat_P(streamBuf, PSTR(",error"));
              }
            }
            break;
          case 12:  // stm
            in_mb[8] = 0; // highByte(stmRegs[j]);
            in_mb[9] = 0; // lowByte(stmRegs[j]);
            in_mb[11] = 34;  // ask for float conversion = 2*num for registers

            //delay(1); // ensure long enough delay between polls
            mbStat = getModbus(in_mb, 12, out_mb, out_len);

            for (j = 0; j < 4; j++) {
              if (mbStat) {  // good message
                int2flt.u8[3] = out_mb[2 * stmRegs[j] + 11];
                int2flt.u8[2] = out_mb[2 * stmRegs[j] + 12];
                int2flt.u8[1] = out_mb[2 * stmRegs[j] + 9]; // high word
                int2flt.u8[0] = out_mb[2 * stmRegs[j] + 10];  // low word

                strcat_P(streamBuf, PSTR(","));
                if (j < 3) {
                  dtostrf(int2flt.f, 1, 3, (streamBuf + strlen(streamBuf)));
                }
                else {  // have totalizer use less digits after decimal point
                  dtostrf(int2flt.f, 1, 1, (streamBuf + strlen(streamBuf)));
                }
              }
              else if (j == 0) {  // bad message on first try, just skip everything to avoid wasting time
                strcat_P(streamBuf, PSTR(",error,error,error,error"));
                break;
              }
              else {  // bad message
                strcat_P(streamBuf, PSTR(",error"));
              }
            }
            
            break;
          default:
            getElecRegs(grp_strt, grp_len, pwrReg, egyReg);
            /*Serial.print(pwrReg);
            Serial.print(F(", "));
            Serial.println(egyReg);*/

            in_mb[8] = highByte(pwrReg);
            in_mb[9] = lowByte(pwrReg);
            

            //delay(5); // ensure long enough delay between polls
            mbStat = getModbus(in_mb, 12, out_mb, out_len);

            
            if (mbStat) {
              int2flt.u8[3] = out_mb[11];
              int2flt.u8[2] = out_mb[12];
              int2flt.u8[1] = out_mb[9]; // high word
              int2flt.u8[0] = out_mb[10];  // low word

              /*Serial.print(i + 1, DEC);
              Serial.print(F(" power: "));
              Serial.println(int2flt.f);*/

              strcat_P(streamBuf, PSTR(","));
              dtostrf(int2flt.f, 1, 3, (streamBuf + 1));

    // get energy now
              in_mb[8] = highByte(egyReg);
              in_mb[9] = lowByte(egyReg);

              //delay(1); // ensure long enough delay between polls
              mbStat = getModbus(in_mb, 12, out_mb, out_len);

              if (mbStat) {
                int2flt.u8[3] = out_mb[11];
                int2flt.u8[2] = out_mb[12];
                int2flt.u8[1] = out_mb[9]; // high word
                int2flt.u8[0] = out_mb[10];  // low word

                /*Serial.print(i + 1, DEC);
                Serial.print(F(" energy: "));
                Serial.println(int2flt.f);*/

                strcat_P(streamBuf, PSTR(","));
                dtostrf(int2flt.f, 1, 1, (streamBuf + strlen(streamBuf)));
              }
              else {
                strcat_P(streamBuf, PSTR(",error"));
              }
            }
            else {
              // print failures for both 
              strcat_P(streamBuf, PSTR(",error,error"));
            }
            break;
        }  // end switch

        dataFile.write(streamBuf);
        streamBuf[0] = 0;
      }  // end for rotate through slaves

      dataFile.print(F("\n"));
      dataFile.close();
    }  // end if dataFile

    digitalWrite(sdWriteLed, LOW);
  }  // end if time
}
