time_t getNtpTime(){
  //IPAddress timeServer(128, 91, 2, 13);
//  char timeServer[] = "time.nist.gov"; // time.nist.gov NTP server
  const int NTP_PACKET_SIZE = 48;
  byte packetBuffer[NTP_PACKET_SIZE];
  EthernetUDP ntpClient;
  unsigned int localPort = 8888;
  uint32_t curTime, oldTime;
  time_t t;

  

  ntpClient.begin(localPort);

  memset(packetBuffer, 0, NTP_PACKET_SIZE);

  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  ntpClient.beginPacket(ntpIp, 123); //NTP requests are to port 123
  ntpClient.write(packetBuffer, NTP_PACKET_SIZE);
  ntpClient.endPacket();
  
  oldTime = millis();
  curTime = millis();

  while ((curTime - oldTime) < 1500) {
    if (ntpClient.parsePacket() >= NTP_PACKET_SIZE) {
      ntpClient.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
      //oldArdTime = millis();
      
      uint32_t highWd = word(packetBuffer[40], packetBuffer[41]);
      uint32_t lowWd = word(packetBuffer[42], packetBuffer[43]);
  
      uint32_t secsSince1900 = (highWd << 16) | lowWd;
      t = secsSince1900 - 2208988800UL;

      Serial.print(F("Retrieved time via NTP: "));
      printTime(t);
      ntpClient.stop();
      return t;  // return unix time

      //curExcelDay = (secsSince1900 / 86400UL) + 2UL;  // day in excel date format
      //initExcelSecs = (secsSince1900 % 86400UL);  // seconds in day

      //Serial.print(F("day: "));
      //Serial.print(curExcelDay);
      //Serial.print(F(", seconds: "));
      //Serial.println(initExcelSecs);

      //break;
    }

    curTime = millis();
  }

  ntpClient.stop();
  Serial.println(F("NTP time retrieval failed!"));
  return 0;  // return whether or not ntp server could be contacted
}


time_t getRtcTime() {
#if defined(CORE_TEENSY)
  return Teensy3Clock.get();
#else
  return 0;
#endif
}


//bool getFakeTime() {
//  curExcelDay = 42304;
//  initExcelSecs = 46000;
//  return true;
//}

void printTime(time_t t) {
  Serial.print(month(t));
  Serial.print(F("-"));
  Serial.print(day(t));
  Serial.print(F("-"));
  Serial.print(year(t));
  Serial.print(F(" "));
  Serial.print(hour(t));
  Serial.print(F(":"));
  Serial.print(minute(t));
  Serial.print(F(":"));
  Serial.println(second(t));
}


//void handle_RT(){
//  /*uint32_t curArdTime;
//
//  curArdTime = millis()*/;
//
//  //if (((curArdTime - oldArdTime) / 1000) > 60) {  // this line for debugging
// // if ((initExcelSecs + ((curArdTime - oldArdTime) / 1000)) > 86400){  // if it's been longer than a day -> 86400 seconds
// //   oldArdTime = curArdTime;
// //   //setSyncProvider(getNtpTime);  // get time from ntp server
// //   //getFakeTime();
//	////getRtcTime();  // get time from real time clock
// // }
//  
//  
//  
//}

