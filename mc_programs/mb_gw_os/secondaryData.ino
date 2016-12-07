



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
    
    uint8_t u8_maxSlvs;

    tempFile = SD.open(cp_fileName, FILE_WRITE);
    u8_maxSlvs = SlaveData.getNumSlvs() > g_u8_maxRecordSlaves ? g_u8_maxRecordSlaves : SlaveData.getNumSlvs();

    if (tempFile) {
      tempFile.print(F("UTC time"));

      for (int ii = 0; ii < u8_maxSlvs; ++ii) {
        tempFile.print(F(","));

        if (SlaveData[ii].u8a_ip[0] == 0) { // if serial device, print gateway ip
          tempFile.print(g_ip_ip[0], DEC);
          for (int jj = 1; jj < 4; ++jj) {
            tempFile.print(F("."));
            tempFile.print(g_ip_ip[jj], DEC);
          }
        }
        else {  // otherwise, print device ip
          tempFile.print(SlaveData[ii].u8a_ip[0], DEC);
          for (int jj = 1; jj < 4; ++jj) {
            tempFile.print(F("."));
            tempFile.print(SlaveData[ii].u8a_ip[jj], DEC);
          }
        }

        tempFile.print(F(": "));
        tempFile.print(SlaveData[ii].u8_id, DEC);
        tempFile.print(F("/"));
        tempFile.print(SlaveData[ii].u8_vid, DEC);
        tempFile.print(F(": "));

        switch (SlaveData[ii].u8a_type[0]) {
          case 11:  // chilled water
            tempFile.print(F("Heat Flow,Mass Flow,Vol. Flow,Supply Temp,Return Temp,Delta Temp,Reserved,Heat Total,"
              "Mass Total,Vol. Total"));
            break;
          case 12:  // steam
            tempFile.print(F("Heat Flow,Mass Flow,Vol. Flow,Supply Temp,Reserved,Reserved,Pressure,Heat Total,"
              "Mass Total,Vol. Total"));
            break;
          default:  // electric
            tempFile.print(F("A Curr,B Curr,C Curr,Avg. Curr,Total Curr,A L-N Volt,B L-N Volt,C L-N Volt,L-N Avg.,"
              "A-B L-L Volt,B-C L-L Volt,C-A L-L Volt,L-L Avg.,A Real Pwr,B Real Pwr,C Real Pwr,Total Real Pwr,"
              "A Rctv Pwr,B Rctv Pwr,C Rctv Pwr,Total Rctv Pwr,A App Pwr,B App Pwr,C App Pwr,Total App Pwr,"
              "A PF,B PF,C PF,Total PF,Total Real Egy,Total Rctv Egy,Total App Egy"));
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