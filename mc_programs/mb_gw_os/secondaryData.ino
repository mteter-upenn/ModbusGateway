
PwrEgyRegs getElecRegs(uint16_t u16_mtrLibStart) {
  //uint16_t lclmtr_strt;
  uint16_t u16_mtrGrpStart;
  PwrEgyRegs elecReg;
  uint16_t u16_curGrpInd;
  uint8_t u8_numGrps;
  uint8_t u8_numGrpVals;
  uint8_t u8_valType;
  uint8_t u8_typeMltplr;
  uint16_t u16_reqReg;
  bool b_fndPwr = false;
  bool b_fndEgy = false;

  //lclmtr_strt = word(EEPROM.read(g_u16_regBlkStart + 4 * meter - 1), EEPROM.read(g_u16_regBlkStart + 4 * meter));

  u16_mtrGrpStart = word(EEPROM.read(u16_mtrLibStart + 3), EEPROM.read(u16_mtrLibStart + 4));  // where the first group starts
  u8_numGrps = EEPROM.read(u16_mtrLibStart + 5);  // number of groups in meter
  u16_curGrpInd = u16_mtrGrpStart;

  //Serial.println("getElecRegs:");
  //Serial.println(u16_mtrGrpStart);
  //Serial.println(u8_numGrps);
  //Serial.println(u16_curGrpInd);

  for (int ii = 0; ii < u8_numGrps; ++ii) {
    u8_numGrpVals = EEPROM.read(u16_curGrpInd);
    u16_reqReg = word(EEPROM.read(u16_curGrpInd + 1), EEPROM.read(u16_curGrpInd + 2));

    for (int jj = 0; jj < u8_numGrpVals; ++jj) {
      if ((!b_fndPwr) && (EEPROM.read(jj + u16_curGrpInd + 3) == 17)) {
        u8_valType = (EEPROM.read(jj + u16_curGrpInd + 3 + u8_numGrpVals) & 0x7F);  // only look at ls 7 bits

        if ((u8_valType < 3) && (u8_valType > 0)) {
          u8_typeMltplr = 1;
        }
        else if (u8_valType == 7) {
          u8_typeMltplr = 3;
        }
        else if (u8_valType > 7) {
          u8_typeMltplr = 4;
        }
        else {
          u8_typeMltplr = 2;
        }

        elecReg.u16_pwr = 10000 + u16_reqReg + jj * u8_typeMltplr;
        b_fndPwr = true;
      }
      else if ((!b_fndEgy) && (EEPROM.read(jj + u16_curGrpInd + 3) == 30)) {
        u8_valType = (EEPROM.read(jj + u16_curGrpInd + 3 + u8_numGrpVals) & 0x7F);  // only look at ls 7 bits

        if ((u8_valType < 3) && (u8_valType > 0)) {
          u8_typeMltplr = 1;
        }
        else if (u8_valType == 7) {
          u8_typeMltplr = 3;
        }
        else if (u8_valType > 7) {
          u8_typeMltplr = 4;
        }
        else {
          u8_typeMltplr = 2;
        }

        elecReg.u16_egy = 10000 + u16_reqReg + jj * u8_typeMltplr;
        b_fndEgy = true;
      }
    }  // end for rotate through values

    if (b_fndPwr && b_fndEgy) {
      return elecReg;
    }
    u16_curGrpInd = 3 + 2 * (u8_numGrpVals)+u16_curGrpInd;  // starting address of next group
  }  // end for rotate through groups

  elecReg.u16_pwr = 0;
  elecReg.u16_egy = 0;
  return elecReg;
}


void getFileName(time_t t_time, char *cp_fileName) {
  int t_yr, t_mn, t_dy, t_hr, t_mm, t_ss;
  char ca_yr[5];
  char ca_mn[3];
  char ca_dy[3];
  char ca_hr[3];
  char ca_mm[3];
  char ca_ss[3];
  uint8_t u8_digit;
  File tempFile;

  ca_yr[4] = 0;
  ca_mn[2] = 0;
  ca_dy[2] = 0;
  ca_hr[2] = 0;
  ca_mm[2] = 0;
  ca_ss[2] = 0;
  strcpy_P(cp_fileName, PSTR("/PASTDATA/"));

  t_yr = year(t_time);
  t_mn = month(t_time);
  t_dy = day(t_time);
  t_hr = hour(t_time);
  t_mm = minute(t_time);
  t_ss = second(t_time);

  for (int ii = 3, jj = 0; jj < 4; --ii, ++jj) {
    u8_digit = t_yr / pow(10, ii);
    ca_yr[jj] = u8_digit + '0';
    t_yr -= u8_digit * pow(10, ii);
  }

  for (int ii = 1, jj = 0; jj < 2; --ii, ++jj) {
    u8_digit = t_mn / pow(10, ii);
    ca_mn[jj] = u8_digit + '0';
    t_mn -= u8_digit * pow(10, ii);
  }

  for (int ii = 1, jj = 0; jj < 2; --ii, ++jj) {
    u8_digit = t_dy / pow(10, ii);
    ca_dy[jj] = u8_digit + '0';
    t_dy -= u8_digit * pow(10, ii);
  }

  for (int ii = 1, jj = 0; jj < 2; --ii, ++jj) {
    u8_digit = t_hr / pow(10, ii);
    ca_hr[jj] = u8_digit + '0';
    t_hr -= u8_digit * pow(10, ii);
  }

  for (int ii = 1, jj = 0; jj < 2; --ii, ++jj) {
    u8_digit = t_mm / pow(10, ii);
    ca_mm[jj] = u8_digit + '0';
    t_mm -= u8_digit * pow(10, ii);
  }

  for (int ii = 1, jj = 0; jj < 2; --ii, ++jj) {
    u8_digit = t_ss / pow(10, ii);
    ca_ss[jj] = u8_digit + '0';
    t_ss -= u8_digit * pow(10, ii);
  }

  strcat(cp_fileName, ca_yr);
  strcat(cp_fileName, "/");
  strcat(cp_fileName, ca_mn);

  if (!SD.exists(cp_fileName)) {  // can't find this folder!
    SD.mkdir(cp_fileName);
  }

  strcat(cp_fileName, "/");
  strcat(cp_fileName, ca_yr);
  strcat(cp_fileName, ca_mn);
  strcat(cp_fileName, ca_dy);
  strcat(cp_fileName, ".csv");

  if (!SD.exists(cp_fileName)) {  // can't find this file!
    // write header for the file
    
    uint8_t maxSlvs;

    tempFile = SD.open(cp_fileName, FILE_WRITE);
    maxSlvs = g_u8_numSlaves > g_u8_maxRecordSlaves ? g_u8_maxRecordSlaves : g_u8_numSlaves;

    if (tempFile) {
      tempFile.print(F("UTC time,"));

      for (int ii = 0; ii < maxSlvs; ++ii) {
        if (g_u8a_slaveIps[ii][0] == 0) { // if serial device, print gateway ip
          tempFile.print(g_ip_ip[0], DEC);
          for (int jj = 1; jj < 4; ++jj) {
            tempFile.print(F("."));
            tempFile.print(g_ip_ip[jj], DEC);
          }
        }
        else {  // otherwise, print device ip
          tempFile.print(g_u8a_slaveIps[ii][0], DEC);
          for (int jj = 1; jj < 4; ++jj) {
            tempFile.print(F("."));
            tempFile.print(g_u8a_slaveIps[ii][jj], DEC);
          }
        }

        tempFile.print(F(": "));
        tempFile.print(g_u8a_slaveIds[ii], DEC);
        tempFile.print(F("/"));
        tempFile.print(g_u8a_slaveVids[ii], DEC);

        switch (g_u8a_slaveTypes[ii][0]) {
          case 11:
            tempFile.print(F(",,,,,"));
          case 12:
            tempFile.print(F(",,,,"));
            break;
          default:
            tempFile.print(F(",,"));
            break;
        }
      }  // end for rotate slaves
      tempFile.println();
      tempFile.close();
    }  // if tempFile
  }  // if tempFile exists
  
  tempFile = SD.open(cp_fileName, FILE_WRITE);

  // write out date
  tempFile.write(ca_mn);  // month
  tempFile.print(F("/"));
  tempFile.write(ca_dy);  // day
  tempFile.print(F("/"));
  tempFile.write(ca_yr);  // year
  tempFile.print(F(" "));
  tempFile.write(ca_hr);  // hour
  tempFile.print(F(":"));
  tempFile.write(ca_mm);  // minute
  tempFile.print(F(":"));
  tempFile.write(ca_ss);  // second

  tempFile.close();
}