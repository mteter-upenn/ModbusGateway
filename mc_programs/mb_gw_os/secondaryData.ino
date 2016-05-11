
void getElecRegs(uint16_t grp_strt, uint8_t grp_len, uint16_t &pwrReg, uint16_t &egyReg) {
  //uint16_t lclmtr_strt;
  uint16_t grp_s/*trt;
                uint8_t grp_len*/;
  uint16_t grp_adr;
  uint8_t numVals;
  uint8_t valType;
  uint8_t mplr;
  uint16_t strtReg;
  uint8_t i, j;
  bool fndPwr = false;
  bool fndEgy = false;

  //lclmtr_strt = word(EEPROM.read(reg_strt + 4 * meter - 1), EEPROM.read(reg_strt + 4 * meter));

  //grp_strt = word(EEPROM.read(lclmtr_strt + 3), EEPROM.read(lclmtr_strt + 4));  // where the first group starts
  //grp_len = EEPROM.read(lclmtr_strt + 5);  // number of groups in meter
  grp_adr = grp_strt;

  for (i = 0; i < grp_len; i++) {
    numVals = EEPROM.read(grp_adr);
    strtReg = word(EEPROM.read(grp_adr + 1), EEPROM.read(grp_adr + 2));

    for (j = 0; j < numVals; j++) {
      if ((!fndPwr) && (EEPROM.read(j + grp_adr + 3) == 17)) {
        valType = (EEPROM.read(j + grp_adr + 3 + numVals) & 0x7F);  // only look at ls 7 bits

        if ((valType < 3) && (valType > 0)) {
          mplr = 1;
        }
        else if (valType == 7) {
          mplr = 3;
        }
        else if (valType > 7) {
          mplr = 4;
        }
        else {
          mplr = 2;
        }

        pwrReg = 10000 + strtReg + j * mplr;
        fndPwr = true;
      }
      else if ((!fndEgy) && (EEPROM.read(j + grp_adr + 3) == 30)) {
        valType = (EEPROM.read(j + grp_adr + 3 + numVals) & 0x7F);  // only look at ls 7 bits

        if ((valType < 3) && (valType > 0)) {
          mplr = 1;
        }
        else if (valType == 7) {
          mplr = 3;
        }
        else if (valType > 7) {
          mplr = 4;
        }
        else {
          mplr = 2;
        }

        egyReg = 10000 + strtReg + j * mplr;
        fndEgy = true;
      }
    }  // end for rotate through values

    if (fndPwr && fndEgy) {
      break;
    }
    grp_adr = 3 + 2 * (numVals)+grp_adr;  // starting address of next group
  }  // end for rotate through groups
}


void getFileName(time_t t, char * fileName) {
  int t_yr, t_mn, t_dy, t_hr, t_mm, t_ss;
  char ch_yr[5];
  char ch_mn[3];
  char ch_dy[3];
  char ch_hr[3];
  char ch_mm[3];
  char ch_ss[3];
  int8_t i, j;
  uint8_t digit;
  File tempFile;

  ch_yr[4] = 0;
  ch_mn[2] = 0;
  ch_dy[2] = 0;
  ch_hr[2] = 0;
  ch_mm[2] = 0;
  ch_ss[2] = 0;
  strcpy_P(fileName, PSTR("/PASTDATA/"));

  t_yr = year(t);
  t_mn = month(t);
  t_dy = day(t);
  t_hr = hour(t);
  t_mm = minute(t);
  t_ss = second(t);

  for (i = 3, j = 0; j < 4; i--, j++) {
    digit = t_yr / pow(10, i);
    ch_yr[j] = digit + '0';
    t_yr -= digit * pow(10, i);
  }

  for (i = 1, j = 0; j < 2; i--, j++) {
    digit = t_mn / pow(10, i);
    ch_mn[j] = digit + '0';
    t_mn -= digit * pow(10, i);
  }

  for (i = 1, j = 0; j < 2; i--, j++) {
    digit = t_dy / pow(10, i);
    ch_dy[j] = digit + '0';
    t_dy -= digit * pow(10, i);
  }

  for (i = 1, j = 0; j < 2; i--, j++) {
    digit = t_hr / pow(10, i);
    ch_hr[j] = digit + '0';
    t_hr -= digit * pow(10, i);
  }

  for (i = 1, j = 0; j < 2; i--, j++) {
    digit = t_mm / pow(10, i);
    ch_mm[j] = digit + '0';
    t_mm -= digit * pow(10, i);
  }

  for (i = 1, j = 0; j < 2; i--, j++) {
    digit = t_ss / pow(10, i);
    ch_ss[j] = digit + '0';
    t_ss -= digit * pow(10, i);
  }

  strcat(fileName, ch_yr);
  strcat(fileName, "/");
  strcat(fileName, ch_mn);

  if (!SD.exists(fileName)) {  // can't find this folder!
    SD.mkdir(fileName);
  }

  strcat(fileName, "/");
  strcat(fileName, ch_yr);
  strcat(fileName, ch_mn);
  strcat(fileName, ch_dy);
  strcat(fileName, ".csv");

  if (!SD.exists(fileName)) {  // can't find this file!
    // write header for the file
    
    uint8_t maxSlvs;

    tempFile = SD.open(fileName, FILE_WRITE);
    maxSlvs = slaves > maxSlvsRcd ? maxSlvsRcd : slaves;

    if (tempFile) {
      for (i = 0; i < maxSlvs; i++) {
        tempFile.print(F("UTC time,"));

        tempFile.print(slv_ips[i][0], DEC);
        for (j = 1; j < 4; j++) {
          tempFile.print(F("."));
          tempFile.print(slv_ips[i][j], DEC);
        }

        tempFile.print(F(": "));
        tempFile.print(slv_devs[i], DEC);
        tempFile.print(F("/"));
        tempFile.print(slv_vids[i], DEC);

        switch (slv_typs[i][0]) {
          case 11:
            tempFile.print(F(",,,,"));
          case 12:
            tempFile.print(F(",,,"));
            break;
          default:
            tempFile.print(F(","));
            break;
        }
      }  // end for rotate slaves
      tempFile.println();
      tempFile.close();
    }  // if tempFile
  }  // if tempFile exists
  
  tempFile = SD.open(fileName, FILE_WRITE);

  // write out date
  tempFile.write(ch_mn);
  tempFile.print(F("/"));
  tempFile.write(ch_dy);
  tempFile.print(F("/"));
  tempFile.write(ch_yr);
  tempFile.print(F(" "));
  tempFile.write(ch_hr);
  tempFile.print(F(":"));
  tempFile.write(ch_mm);
  tempFile.print(F(":"));
  tempFile.write(ch_ss);

  tempFile.close();
}