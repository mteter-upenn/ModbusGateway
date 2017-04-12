
/*
 Modbus gateway sketch

 SET COMPILER VARIABLES IN DROP DOWN MENUS!!!!
 CPU Speed: 144MHz Overclocked
 Serial Buffer Size: Serial3 256 Bytes (or All 256 Bytes)
 */


// testing
#include "mb_names.h"
#include <ModbusStructs.h>  // FloatConvEnum, ModbusRequest
#include <EEPROM.h>
#include <SPI.h>

#include <MeterLibrary.h>
#include "ModbusStack.h"
#include <ModbusServer.h>

#include <TimeLib.h>  // sometimes should be called as TimeLib.h

#include <Ethernet52.h>
#include <EthernetUdp52.h>

#include <SD.h>

#ifndef CORE_TEENSY
#error "This program designed for the Teensy 3.2"
#endif


#include "globals.h"
#include "miscFuncs.h"
#include "handleServers.h"
#include "handleData.h"
#include "handleRTC.h"


void setup() {
  Serial.begin(9600);
  Serial.println(F("delay here"));
  delay(2000);
  Serial.println(F("delay over"));

  // set output pins
  pinMode(RTC_FAIL_LED_PIN, OUTPUT);
  pinMode(BATT_DEAD_LED_PIN, OUTPUT);
  pinMode(SD_FAIL_LED_PIN, OUTPUT);
  pinMode(SD_WRITE_LED_PIN, OUTPUT);
  pinMode(EEPROM_WRITE_LEN_PIN, OUTPUT);
  
  // get indices from eeprom
  EEPROM.get(0, g_u16_nameBlkStart);
  EEPROM.get(2, g_u16_ipBlkStart);
  EEPROM.get(4, g_u16_mtrBlkStart);
  EEPROM.get(6, g_u16_regBlkStart);

  // take constants from eeprom into memory
  setConstants();

  // initialize SlaveData
  SlaveData.init();
  
//  set serial1 pins high - needed for 485 shield to work
  pinMode(MB_SERIAL_RX_PIN, INPUT_PULLUP);
  digitalWrite(MB_SERIAL_RX_PIN, HIGH);
  //pin MB_SERIAL_TX_PIN should have external pullup

  // may be necessary to set pin 10 high regardless of arduino type
  pinMode(SPI_ETHERNET_PIN, OUTPUT);
  digitalWrite(SPI_ETHERNET_PIN, HIGH);
  pinMode(SPI_SD_PIN, OUTPUT);
  digitalWrite(SPI_SD_PIN, HIGH);
  
//  reset w5200 ethernet chip
  pinMode(W5200_RESET_PIN, OUTPUT);
  digitalWrite(W5200_RESET_PIN, LOW);

  Serial.println(F("Initializing SD card..."));
  if (!SD.begin(SPI_SD_PIN)) {
    Serial.println(F("ERROR - SD card initialization failed!"));
  }
  else{
    Serial.println(F("SUCCESS - SD card initialized."));
    g_b_sdInit = true;

    // generate xml files for web server  SHOULD THIS FIRE AT THE TOP EVERY TIME?  probably, won't know changes if flash_eeprom used
    writeRestartFile();
    writeGenSetupFile();
    writeMtrSetupFile();
  }
  
  // start ethernet 
  uint16_t u16a_socketSizes[8] = { 4, 4, 1, 1, 1, 1, 2, 2 };  // sizes are >>10 eg 4->4096
  uint16_t u16a_socketPorts[8] = { 80, 80, 502, 502, 502 ,502, 0, 0 };

  Ethernet52.begin(g_u8a_mac, g_ip_ip, g_ip_gateway, g_ip_gateway, g_ip_subnet, 8, u16a_socketSizes, u16a_socketPorts);

  // initialize sockets
  g_es_webServ.begin();
  g_es_webServ.begin();

  g_es_mbServ.begin();
  g_es_mbServ.begin();
  g_es_mbServ.begin();
  g_es_mbServ.begin();

  g_modbusServer.begin(g_u32_baudrate, g_u8_dataBits, g_u8_parity, g_u8_stopBits);
  g_modbusServer.setTimeout(g_u16_timeout);
  g_modbusServer.setPrintComms(g_b_printMbComms);

  // start ntp or rtc
  time_t t_localTime(0);
  delay(1500);  // 1100
  setSyncProvider(getRtcTime);

  if (g_b_useNtp) {
    t_localTime = getNtpTime();
    g_u32_rtcNtpLastReset = millis();
  }

  if (t_localTime == 0) {  //  could not get ntp time, or ntp was disabled
    if (getRtcTime() > 1451606400L) {  // check if rtc is already working well, if not show as led error
      g_b_rtcGood = true;  // time in RTC is greater than Jan 1 2016
    }
  }
  else {  // set clock to time gotten from ntp
    Teensy3Clock.set(t_localTime);
    setTime(t_localTime);
    g_b_rtcGood = true;
  }

  setSyncInterval(86400);

  // 2 s total delay
  // 450 from flashing leds
  // 1500 from pre ntp wait, 

  digitalWrite(BATT_DEAD_LED_PIN, HIGH);
  delay(50);
  digitalWrite(RTC_FAIL_LED_PIN, HIGH);
  delay(50);
  digitalWrite(BATT_DEAD_LED_PIN, LOW);
  digitalWrite(SD_FAIL_LED_PIN, HIGH);
  delay(50);
  digitalWrite(RTC_FAIL_LED_PIN, LOW);
  digitalWrite(SD_WRITE_LED_PIN, HIGH);
  delay(50);
  digitalWrite(SD_FAIL_LED_PIN, LOW);
  digitalWrite(EEPROM_WRITE_LEN_PIN, HIGH);
  delay(50);
  digitalWrite(SD_WRITE_LED_PIN, LOW);
  delay(50);
  digitalWrite(EEPROM_WRITE_LEN_PIN, LOW);

  delay(50);
  digitalWrite(BATT_DEAD_LED_PIN, HIGH);
  digitalWrite(RTC_FAIL_LED_PIN, HIGH);
  digitalWrite(SD_FAIL_LED_PIN, HIGH);
  digitalWrite(SD_WRITE_LED_PIN, HIGH);
  digitalWrite(EEPROM_WRITE_LEN_PIN, HIGH);
  delay(50);
  digitalWrite(BATT_DEAD_LED_PIN, LOW);
  digitalWrite(RTC_FAIL_LED_PIN, LOW);
  digitalWrite(SD_FAIL_LED_PIN, LOW);
  digitalWrite(SD_WRITE_LED_PIN, LOW);
  digitalWrite(EEPROM_WRITE_LEN_PIN, LOW);
  delay(50);

  if (!g_b_sdInit) {
    digitalWrite(SD_FAIL_LED_PIN, HIGH);
  }

  if (!g_b_rtcGood) {
    digitalWrite(RTC_FAIL_LED_PIN, HIGH);
  }

#if SHOW_FREE_MEM
  Serial.print(F("start: "));
  Serial.println(getFreeMemory());
#endif
}  // end setup


void loop() {
  handleServers();

//  if (g_b_recordData && g_b_rtcGood) {
//    //handle_data();
//  }

//  if (g_b_useNtp && ((millis() - g_u32_rtcNtpLastReset) > NTP_RESET_DELAY)) {
//    // if enough time has elapsed and we want to use ntp
//    time_t t_localTime(0);

//    t_localTime = getNtpTime();

//    if (t_localTime != 0) {  // set clock to time gotten from ntp
//      Teensy3Clock.set(t_localTime);  // just need to set Teensy3 time, class defined time updates from here
//      //setTime(t_localTime);  // this should not be strictly necessary, though update will be delayed
//      g_b_rtcGood = true;

//      digitalWrite(RTC_FAIL_LED_PIN, LOW);
//    }

//    g_u32_rtcNtpLastReset = millis();  // reset timer
//  }
}  // end loop

