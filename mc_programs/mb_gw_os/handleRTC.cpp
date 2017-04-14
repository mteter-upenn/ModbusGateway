#include "handleRTC.h"
#include <Time.h>
#include <Ethernet52.h>
#include <EthernetUdp52.h>
#include "globals.h"

time_t getNtpTime() {
  const int k_s_ntpPacketSize = 48;  // NTP_PACKET_SIZE
  char ca_packetBuffer[k_s_ntpPacketSize] = {0};
  EthernetUDP52 eu_ntpClient;
  uint16_t u16_localPort = 8888;

  eu_ntpClient.begin(u16_localPort);

  ca_packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  ca_packetBuffer[1] = 0;     // Stratum, or type of clock
  ca_packetBuffer[2] = 6;     // Polling Interval
  ca_packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  ca_packetBuffer[12]  = 49;
  ca_packetBuffer[13]  = 0x4E;
  ca_packetBuffer[14]  = 49;
  ca_packetBuffer[15]  = 52;

  eu_ntpClient.beginPacket(g_ip_ntpIp, 123); //NTP requests are to port 123
  eu_ntpClient.write(ca_packetBuffer, k_s_ntpPacketSize);
  eu_ntpClient.endPacket();
  
  uint32_t u32_startTime;
  u32_startTime = millis();

  while ((millis() - u32_startTime) < 1500) {
    if (eu_ntpClient.parsePacket() >= k_s_ntpPacketSize) {
      time_t t_time;
//      const int32_t k_s32_timeZone = 0;

      eu_ntpClient.read(ca_packetBuffer, k_s_ntpPacketSize); // read the packet into the buffer
      
      uint32_t highWd = word(ca_packetBuffer[40], ca_packetBuffer[41]);
      uint32_t lowWd = word(ca_packetBuffer[42], ca_packetBuffer[43]);
  
      uint32_t secsSince1900 = (highWd << 16) | lowWd;
      t_time = secsSince1900 - 2208988800UL + TIME_ZONE_DIFF * SECS_PER_HOUR;

      Serial.print(F("Retrieved time via NTP: "));
      printTime(t_time);
      eu_ntpClient.stop();
      return t_time;  // return unix time
    }
  }

  eu_ntpClient.stop();
  Serial.println(F("NTP time retrieval failed!"));
  return 0;  // return whether or not ntp server could be contacted
}


time_t getRtcTime() {
  time_t t_time = Teensy3Clock.get();
  Serial.print("rtc time: "); printTime(t_time);
  return t_time;
}


void printTime(time_t t_time) {
  Serial.print(month(t_time));
  Serial.print(F("-"));
  Serial.print(day(t_time));
  Serial.print(F("-"));
  Serial.print(year(t_time));
  Serial.print(F(" "));
  Serial.print(hour(t_time));
  Serial.print(F(":"));
  Serial.print(minute(t_time));
  Serial.print(F(":"));
  Serial.println(second(t_time));
}


