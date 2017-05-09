/* _____PROJECT INCLUDES_____________________________________________________ */
#include "DebugLib.h"
#include "Ethernet52.h"

/* _____GLOBAL VARIABLES_____________________________________________________ */



/*______STATIC VARIABLES_____________________________________________________ */
bool DebugLibClass::ms_b_printComms = true;


/* _____PUBLIC FUNCTIONS_____________________________________________________ */

void DebugLibClass::digitalClockDisplaySerial(time_t t) {
 // digital clock display of the time
 Serial.print(hour(t));
 printDigitsSerial(minute(t));
 printDigitsSerial(second(t));
 Serial.print(" ");
 Serial.print(day(t));
 Serial.print(" ");
 Serial.print(monthStr(month(t)));
// Serial.print(month(t));
 Serial.print(" ");
 Serial.print(year(t));
 Serial.println();
}

void DebugLibClass::printDigitsSerial(int digits) {
 // utility function for digital clock display: prints preceding colon and leading 0
 Serial.print(":");
 if (digits < 10)
 Serial.print('0');
 Serial.print(digits);
}


void DebugLibClass::print3SpaceDigitsSerial(uint8_t num) {
  if (num < 10) {
    Serial.print("  ");
  }
  else if (num < 100) {
    Serial.print(" ");
  }
  Serial.print(num, DEC);
}


void DebugLibClass::print3SpaceDigitsSD(File sdFile, uint8_t num) {
  if (num < 10) {
    sdFile.print("  ");
  }
  else if (num < 100) {
    sdFile.print(" ");
  }
  sdFile.print(num, DEC);
}


void DebugLibClass::storeStringAndArr(const char *k_cp_string, uint8_t *u8p_arr, uint16_t u16_arrLen, uint16_t u16_unqId, uint8_t u8_sock, bool b_showTime) {
  if (ms_b_printComms) {
    time_t t_time = now();
    uint32_t u32_millis = millis();
    int t_yr, t_mn, t_dy;
    char ca_yr[5];
    char ca_mn[3];
    char ca_dy[3];
    uint8_t u8_digit;
    File tempFile;
    char cp_fileName[30] = {0};

    ca_yr[4] = 0;
    ca_mn[2] = 0;
    ca_dy[2] = 0;
    strcpy_P(cp_fileName, PSTR("/PASTDATA/ERRORS/"));

    t_yr = year(t_time);
    t_mn = month(t_time);
    t_dy = day(t_time);

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

    strcat(cp_fileName, ca_yr);
    strcat(cp_fileName, ca_mn);
    strcat(cp_fileName, ca_dy);
    strcat(cp_fileName, ".txt");

    tempFile = SD.open(cp_fileName, FILE_WRITE);

  //  tempFile.println();
    tempFile.print(k_cp_string);
    if (b_showTime) {
      tempFile.print(hour(t_time));

      tempFile.print(":");
      if (minute(t_time) < 10) tempFile.print('0');
      tempFile.print(minute(t_time));

      tempFile.print(":");
      if (second(t_time) < 10) tempFile.print('0');
      tempFile.print(second(t_time));

      tempFile.print(" ");
      tempFile.print(day(t_time));

      tempFile.print(" ");
      tempFile.print(monthStr(month(t_time)));

      tempFile.print(" ");
      tempFile.print(year(t_time));

      tempFile.print(" UTC, ");
      tempFile.print(u32_millis);

      tempFile.print(" ms");
    }
    tempFile.print(", unique id: ");
    tempFile.print(u16_unqId);

    tempFile.print(", on socket: ");
    if (u8_sock == 255) {
      tempFile.println("serial");
    }
    else {
      tempFile.println(u8_sock, DEC);
    }

    if (u16_arrLen > 0) {
      for (int ii = 0; ii < u16_arrLen; ++ii) {
        print3SpaceDigitsSD(tempFile, u8p_arr[ii]); tempFile.print(" ");
      }
      tempFile.println();
    }
    tempFile.close();
  }
}

void DebugLibClass::storeSockets(const char *k_cp_msg) {
  if (ms_b_printComms) {
    time_t t_time = now();
//    uint32_t u32_millis = millis();
    int t_yr, t_mn, t_dy;
    char ca_yr[5];
    char ca_mn[3];
    char ca_dy[3];
    uint8_t u8_digit;
    File tempFile;
    char cp_fileName[30] = {0};

    ca_yr[4] = 0;
    ca_mn[2] = 0;
    ca_dy[2] = 0;
    strcpy_P(cp_fileName, PSTR("/PASTDATA/ERRORS/"));

    t_yr = year(t_time);
    t_mn = month(t_time);
    t_dy = day(t_time);

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

    strcat(cp_fileName, ca_yr);
    strcat(cp_fileName, ca_mn);
    strcat(cp_fileName, ca_dy);
    strcat(cp_fileName, ".txt");

    tempFile = SD.open(cp_fileName, FILE_WRITE);

    if (k_cp_msg != nullptr) {
      tempFile.println(k_cp_msg);
    }

    tempFile.println("sock\tport mask\tport arr\tport 5200\tcl port\t\tcl stat\t\tcl ip\t\tavail");
    for (int sock = 0; sock < EthernetClass52::getMaxUsedSocks(); sock++) {
      EthernetClient52 client(sock);

      tempFile.print(sock, DEC); tempFile.print("\t");
      tempFile.print(EthernetClass52::getServerPortMask(sock), DEC); tempFile.print("\t\t");
      tempFile.print(EthernetClass52::getServerPort(sock), DEC); tempFile.print("\t\t");
      tempFile.print(W5200.readSnPORT(sock), DEC); tempFile.print("\t\t");
      tempFile.print(EthernetClass52::getClientPort(sock), DEC); tempFile.print("\t\t");
      tempFile.print("0x"); tempFile.print(client.status(), HEX); tempFile.print("\t\t");

      if (client.status() == SnSR::ESTABLISHED || client.status() == SnSR::CLOSE_WAIT) {
        uint8_t u8a_ip[4] = {0};

        client.getRemoteIP(u8a_ip);
        tempFile.print(u8a_ip[0], DEC);
        for (int ii = 1; ii < 4; ++ii) {
          tempFile.print("."); tempFile.print(u8a_ip[ii]);
        }
        tempFile.print("\t"); tempFile.print(client.available(), DEC);
      }

      tempFile.println();
    }

    tempFile.close();
  }
}


void DebugLibClass::setPrintComms(bool b_printComms) {
  ms_b_printComms = b_printComms;
}
