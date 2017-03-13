
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
//#include <ModbusMaster.h>  // REMOVE

#include <SD.h>

#ifndef CORE_TEENSY
#error "This program designed for the Teensy 3.2"
#endif


#include "globals.h"
#include "miscFuncs.h"
#include "handleServers.h"
#include "handleData.h"
#include "handleRTC.h"
//#include "handleHTTP.h"
//#include "handleModbus.h"




//const uint16_t REQUEST_LINE_SIZE(40);                       // g_REQ_BUF_SZ buffer size to capture beginning of http request
//const uint16_t REQUEST_BUFFER_SIZE(1500);                     // g_REQ_ARR_SZ size of array for http request, first REQ_BUF_SZ bytes will always be first part of
//                                                       //   message, the rest of the array may loop around if the http request is large enough
//const uint16_t POST_BUFFER_SIZE((REQUEST_BUFFER_SIZE - 1 - 1030 - 50));  // POST_BUF_SZ currently 419

//// FIND ANOTHER WAY TO TEST FOR SMALL POST BUFFER SIZE
////#if POST_BUFFER_SIZE < 20                                   // make sure post buffer has enough room to play with, 20 bytes sounds good for min
////#error "not enough room in array for POST messages"
////#endif

//const uint16_t MB_ARRAY_SIZE(264);                    // MB_ARR_SIZE array size for modbus/tcp rx/tx buffers - limited by modbus standards
    
//const uint16_t RESPONSE_BUFFER_SIZE(1400);              // RESP_BUF_SZ array size for buffer between sd card and ethernet
//                                                       //     keep short of 1500 to make room for headers
//const uint8_t MB_SERIAL_HARDWARE_PORT(3);           // MODBUS_SERIAL use hardware serial 3

//// reset necessaries
//#define CPU_RESTART_ADDR (uint32_t *)0xE000ED0C
//#define CPU_RESTART_VAL 0x5FA0004
//#define CPU_RESTART (*CPU_RESTART_ADDR = CPU_RESTART_VAL);

//// pin ids
//const int SD_FAIL_LED_PIN   = 21;                     // sd card unavailable
//const int SD_WRITE_LED_PIN  = 20;                     // currently writing to sd card
//const int EEPROM_WRITE_LEN_PIN  = 19;                     // currently writing to eeprom
//const int RTC_FAIL_LED_PIN  = 22;                     // no rtc set
//const int BATT_DEAD_LED_PIN = 23;                     // dead battery - currently no way to determine
//const uint8_t MB_485_CTRL_PIN   =  6;                     // when set low, transmit mode, high is receive mode

//// ethernet info
//uint8_t g_u8a_mac[8] = {0};                      // enter mac, will need some sort of generator for this
//IPAddress g_ip_ip(0, 0, 0, 0);                       // this value will be overwritten by ip stored in eeprom
//IPAddress g_ip_subnet(0, 0, 0, 0);                    // this value will be overwritten by ip stored in eeprom
//IPAddress g_ip_gateway(0, 0, 0, 0);                    // this value will be overwritten by ip stored in eeprom
////bool g_b_reset = false;                                    // bReset

//bool g_b_useNtp = false;                                 // bNTPserv turns ntp on/off (overwritten by eeprom)
//IPAddress g_ip_ntpIp(0, 0, 0, 0);                      // this value will be overwritten by ip stored in eeprom

//// gateway info
//char g_c_gwName[31] = {0};                               // meter_nm name of gateway
//// addresses for components stored in eeprom (see flash_eeprom.ino for more details)
//uint16_t g_u16_nameBlkStart;  // nm_strt
//uint16_t g_u16_ipBlkStart;  // ip_strt
//uint16_t g_u16_mtrBlkStart;  // mtr_strt
//uint16_t g_u16_regBlkStart;  // reg_strt

//// modbus info
//uint32_t g_u32_baudrate = 9600;                              // baud rate of 485 network
//uint16_t g_u16_timeout = 2000;                               // modbus timeout

//// slave info
//// REMOVE BELOW!
////uint8_t g_u8_numSlaves;                                // number of modbus slaves attached to gateway
////uint8_t g_u8a_slaveIds[20];  // slv_devs                                // array of modbus device ids (meters can share these!)
////uint8_t g_u8a_slaveVids[20];    // slv_vids                              // array of modbus virtual ids (these should be unique!) they can be the same as devs
////uint8_t g_u8a_slaveIps[20][4];  // slv_ips                              // array of slave ips
////uint8_t g_u8a_slaveTypes[20][3]; // slv_typs                              // array of slave meter types
//// REMOVE ABOVE!
////uint8_t g_u8a_selectedSlave = 1;      // selSlv                              // selected slave - used for webpage live data
////ModbusStack mbStack;


//// rtc info
//bool g_b_rtcGood = false;  // bGoodRTC
//uint32_t g_u32_rtcNtpLastReset(0UL);  // for resetting rtc with ntp
//const uint32_t gk_u32_rtcNtpResetDelay(604800000UL);  // delay in ms

//// data collection timing
//uint32_t g_u32_lastDataRequest;  // oldDataTime
//bool g_b_recordData = false;  // bRecordData
//uint8_t g_u8_maxRecordSlaves = 5;  // maxSlvsRcd

//// classes
//EthernetServer52 g_es_webServ(80);  //serv_web                           // start server on http

//EthernetServer52 g_es_mbServ(502);  // serv_mb                           // start server on modbus port

////ModbusMaster g_mm_node(MB_485_CTRL_PIN, MB_SERIAL_HARDWARE_PORT); // node  // initialize node on device 1, client ip, enable pin, serial port
//ModbusServer g_modbusServer(MB_SERIAL_HARDWARE_PORT, MB_485_CTRL_PIN);

//// server socket info
//uint32_t g_u32a_socketTimeoutStart[8] = { 0 };  // time to compare to timeout
//SockFlag g_u16a_socketFlags[8] = { SockFlag_LISTEN, SockFlag_LISTEN, SockFlag_LISTEN, SockFlag_LISTEN,
//                                  SockFlag_LISTEN, SockFlag_LISTEN, SockFlag_LISTEN, SockFlag_LISTEN };  // flag names found in mb_names.h
//uint16_t  g_u16a_mbReqUnqId[8] = { 0 };  // unique id of modbus request in ModbusStack
//EthernetClient52 g_eca_socks[8] = {0, 1, 2, 3, 4, 5, 6, 7};

//// MOVE THESE TO MBSTACK CLASS
//bool g_b_485avail = true;  // can assume 485 is open at init
//bool g_ba_clientSocksAvail[2] = { false, false };  // assume both socks used at init


//// miscellaneous
//bool g_b_sdInit = false;  // sdInit                                   // set flag corresponding to sd card initializtion
//const int gk_i_maxNumElecVals(32);
//const int gk_i_maxNumStmChwVals(10);

//// test varsvoid writeRestartFile()
//#if DISP_TIMING_DEBUG
//uint32_t doneHttp, gotClient, doneFind, doneSend, time1, time2;
//#endif

// PROTOTYPES:
// main
//void resetArd(void);
// handleServers
//void handleServers();
// handleHTTP
//SockFlag readHttp(const uint8_t u8_socket, FileReq &u16_fileReq, FileType &s16_fileType, uint8_t &u8_selSlv, char ca_fileReq[REQUEST_LINE_SIZE]);
//bool respondHttp(const uint8_t u8_socket, const SockFlag u16_sockFlag, const FileReq u16_fileReq, const FileType s16_fileType,  const uint8_t u8_selSlv, const char ca_fileReq[REQUEST_LINE_SIZE], ModbusStack &mbStack, uint8_t &u8_curGrp, float fa_liveXmlData[gk_i_maxNumElecVals], int8_t s8a_dataFlags[gk_i_maxNumElecVals]);
// secondaryHTTP - GET and general functions
//void flushEthRx(EthernetClient52 &ec_client, uint8_t *u8p_buffer, uint16_t u16_length);
//bool isSerial(uint8_t u8_selSlv);
//void convertToFileName(char ca_fileReq[REQUEST_LINE_SIZE]);
//void send404(EthernetClient52 &ec_client);
//void sendBadSD(EthernetClient52 &ec_client);
////void sendGifHdr(EthernetClient52 &ec_client);
//void sendWebFile(EthernetClient52 &ec_client, const char* ccp_fileName, FileType en_fileType = FileType::NONE, bool b_addFileLength = true);
//void sendDownLinks(EthernetClient52 &ec_client, const char *const cp_firstLine);
//void sendXmlEnd(EthernetClient52 &ec_client, XmlFile en_xmlType);
//void sendIP(EthernetClient52 &ec_client);
//void liveXML(uint8_t u8_socket, uint8_t u8_selSlv, float fa_data[gk_i_maxNumElecVals], int8_t s8a_dataFlags[gk_i_maxNumElecVals]);
// tertiaryHTTP - POST related functions
//void sendPostResp(EthernetClient52 &ec_client);
//char* preprocPost(EthernetClient52 &ec_clientvoid writeRestartFile(), char *cp_httpHdr, uint16_t &u16_postLen);
//void getPostSetupData(EthernetClient52 &ec_client);
// handleModbus
//uint8_t getModbus(uint8_t u8a_mbReq[MB_ARRAY_SIZE], uint16_t u16_mbReqLen, uint8_t u8a_mbResp[MB_ARRAY_SIZE], uint16_t &u16_mbRespLen, bool b_byteSwap);
//void handle_modbus(bool b_idleHttp);
// secondaryModbus
bool findRegister(uint16_t u16_reqRegister, FloatConv &fltConv, uint8_t u8_meterType);
//bool isMeterEth(uint8_t u8_virtId, uint8_t &u8_meterType, uint8_t &u8_trueId);
// setConstants
//void setConstants(void);
//void writeGenSetupFile(void);
//void writeMtrSetupFile(void);
// handleRTC
//time_t getNtpTime(void);
//time_t getRtcTime(void);
//void printTime(time_t t_time);
// handleData
//void handle_data();
// secondaryData
//void getFileName(time_t t_time, char *cp_fileName);



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
//  g_u16_nameBlkStart = word(EEPROM.read(0), EEPROM.read(1));
//  g_u16_ipBlkStart = word(EEPROM.read(2), EEPROM.read(3));
//  g_u16_mtrBlkStart = word(EEPROM.read(4), EEPROM.read(5));
//  g_u16_regBlkStart = word(EEPROM.read(6), EEPROM.read(7));

  EEPROM.get(0, g_u16_nameBlkStart);
  EEPROM.get(2, g_u16_ipBlkStart);
  EEPROM.get(4, g_u16_mtrBlkStart);
  EEPROM.get(6, g_u16_regBlkStart);

  // take constants from eeprom into memory
  setConstants();

  // initialize SlaveData
  SlaveData.init();
  
//  set serial1 pins high - needed for 485 shield to work
  switch (MB_SERIAL_HARDWARE_PORT) {
    case 2:
      digitalWrite(9, HIGH);
      digitalWrite(10, HIGH);
      break;
    case 3:
      pinMode(7, INPUT_PULLUP);
      //pin 8 has external pullup
      digitalWrite(7, HIGH);
      break;
    default:
      digitalWrite(0, HIGH);
      digitalWrite(1, HIGH);
      break;
  }

  // may be necessary to set pin 10 high regardless of arduino type
  pinMode(10, OUTPUT);  
  digitalWrite(10, HIGH);
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  
//  reset w5200 ethernet chip
  pinMode(9, OUTPUT);
  digitalWrite(9, LOW);

  Serial.println(F("Initializing SD card..."));
  if (!SD.begin(4)) {
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

  

  //g_mm_node.begin(g_u32_baudrate);
  //g_mm_node.setTimeout(g_u16_timeout);
  //  g_mm_node.idle(*function_here);  // add function for idling during wait for modbus return message
  g_modbusServer.begin(g_u32_baudrate, g_u8_dataBits, g_u8_parity, g_u8_stopBits);
  g_modbusServer.setTimeout(g_u16_timeout);
  //g_modbusServer.setTimeout(500);


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
  //handle_modbus(true);
  //handle_http(true);
  handleServers();

  if (g_b_recordData && g_b_rtcGood) {
    //handle_data();
  }

  if (g_b_useNtp && ((millis() - g_u32_rtcNtpLastReset) > gk_u32_rtcNtpResetDelay)) {
    // if enough time has elapsed and we want to use ntp
    time_t t_localTime(0);

    t_localTime = getNtpTime();

    if (t_localTime != 0) {  // set clock to time gotten from ntp
      Teensy3Clock.set(t_localTime);  // just need to set Teensy3 time, class defined time updates from here
      //setTime(t_localTime);  // this should not be strictly necessary, though update will be delayed
      g_b_rtcGood = true;

      digitalWrite(RTC_FAIL_LED_PIN, LOW);
    }

    g_u32_rtcNtpLastReset = millis();  // reset timer
  }
}  // end loop

