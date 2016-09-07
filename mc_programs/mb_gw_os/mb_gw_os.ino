
/*
 Modbus gateway sketch
 */


#include "mb_names.h"
#include <FloatConvEnum.h>
#include <Time.h>
#include <SPI.h>
#include <Ethernet52.h>
#include <EthernetUdp52.h>
#include <ModbusMaster.h>
#include <EEPROM.h>
#include <MeterLibrary.h>
#include <SD.h>

#define DEBUG_HTTP_TCP_TIMEOUT 0                       // debug flag for keep-alive attempt  
#define DISP_TIMING_DEBUG 0                            // debug flag that will print out delta times for web page interface
#define SHOW_FREE_MEM 0                                // 1 for print free memory
#define NEW_GROUP_STYLE 1

const uint16_t gk_u16_requestLineSize(40);                       // g_REQ_BUF_SZ buffer size to capture beginning of http request
const uint16_t gk_u16_requestBuffSize(1500);                     // g_REQ_ARR_SZ size of array for http request, first REQ_BUF_SZ bytes will always be first part of 
                                                       //   message, the rest of the array may loop around if the http request is large enough
const uint16_t gk_u16_postBuffSize((gk_u16_requestBuffSize - 1 - 1030 - 50));  // POST_BUF_SZ currently 419

// FIND ANOTHER WAY TO TEST FOR SMALL POST BUFFER SIZE
//#if gk_u16_postBuffSize < 20                                   // make sure post buffer has enough room to play with, 20 bytes sounds good for min
//#error "not enough room in array for POST messages"
//#endif

const uint16_t gk_u16_mbArraySize(264);                    // MB_ARR_SIZE array size for modbus/tcp rx/tx buffers - limited by modbus standards
    
#if defined(CORE_TEENSY)                               // if teensy3.0 or greater  SHOULD PROBABLY JUST ASSUME TEENSY FOR NOW, 
                                                       //     ARDUINO BOARDS DEFINITELY WON'T WORK WITHOUT SIGNIFICANT READJUSTMENT ANYWAYS

const uint16_t gk_u16_respBuffSize(1400);              // RESP_BUF_SZ array size for buffer between sd card and ethernet
                                                       //     keep short of 1500 to make room for headers
const uint8_t gk_u8_modbusSerialHardware(3);           // MODBUS_SERIAL use hardware serial 3

// reset necessaries
#define CPU_RESTART_ADDR (uint32_t *)0xE000ED0C
#define CPU_RESTART_VAL 0x5FA0004
#define CPU_RESTART (*CPU_RESTART_ADDR = CPU_RESTART_VAL);


// pin ids
const int gk_s16_sdFailLed   = 21;                     // sd card unavailable
const int gk_s16_sdWriteLed  = 20;                     // currently writing to sd card
const int gk_s16_epWriteLed  = 19;                     // currently writing to eeprom
const int gk_s16_rtcFailLed  = 22;                     // no rtc set
const int gk_s16_battDeadLed = 23;                     // dead battery - currently no way to determine
const uint8_t gk_u8_mb485Ctrl   =  6;                     // when set low, transmit mode, high is receive mode
#else
#define RESP_BUF_SZ  1024                              // array size for buffer between sd card and ethernet
#define MODBUS_SERIAL 1                                // use hardware serial 1

int resetPin = 6;                                      // when set high, will trigger hard reset
int sdFailLed = 7;                                     // sd card unavailable
int sdWriteLed = 11;                                   // currently writing to sd card
int epWriteLed = 8;                                    // currently writing to eeprom
int mb485Ctrl = 9;                                     // when set low, transmit mode, high is receive mode
int rtcFailLed = 8;                                    // no rtc set
#endif

/*
// define struct
struct PwrEgyRegs {
  uint16_t u16_pwr;
  uint16_t u16_egy;
};
*/

// ethernet info
uint8_t g_u8a_mac[8] = {0};                      // enter mac, will need some sort of generator for this
IPAddress g_ip_ip(0, 0, 0, 0);                       // this value will be overwritten by ip stored in eeprom
IPAddress g_ip_subnet(0, 0, 0, 0);                    // this value will be overwritten by ip stored in eeprom
IPAddress g_ip_gateway(0, 0, 0, 0);                    // this value will be overwritten by ip stored in eeprom
bool g_b_reset = false;                                    // bReset

bool g_b_useNtp = false;                                 // bNTPserv turns ntp on/off (overwritten by eeprom)
IPAddress g_ip_ntpIp(0, 0, 0, 0);                      // this value will be overwritten by ip stored in eeprom

// gateway info
char g_c_gwName[31] = {0};                               // meter_nm name of gateway
// addresses for components stored in eeprom (see flash_eeprom.ino for more details)
uint16_t g_u16_nameBlkStart;  // nm_strt
uint16_t g_u16_ipBlkStart;  // ip_strt
uint16_t g_u16_mtrBlkStart;  // mtr_strt
uint16_t g_u16_regBlkStart;  // reg_strt

// modbus info
uint32_t g_u32_baudrate = 9600;                              // baud rate of 485 network
uint16_t g_u16_timeout = 2000;                               // modbus timeout

// slave info
uint8_t g_u8_numSlaves;  // slaves                              // number of modbus slaves attached to gateway
uint8_t g_u8a_slaveIds[20];  // slv_devs                                // array of modbus device ids (meters can share these!)
uint8_t g_u8a_slaveVids[20];    // slv_vids                              // array of modbus virtual ids (these should be unique!) they can be the same as devs
uint8_t g_u8a_slaveIps[20][4];  // slv_ips                              // array of slave ips
uint8_t g_u8a_slaveTypes[20][3]; // slv_typs                              // array of slave meter types
uint8_t g_u8a_selectedSlave = 1;      // selSlv                              // selected slave - used for webpage live data

// rtc info
bool g_b_rtcGood = false;  // bGoodRTC
uint32_t g_u32_rtcNtpLastReset(0UL);  // for resetting rtc with ntp
const uint32_t gk_u32_rtcNtpResetDelay(604800000UL);  // delay in ms

// data collection timing
uint32_t g_u32_lastDataRequest;  // oldDataTime
bool g_b_recordData = false;  // bRecordData
uint8_t g_u8_maxRecordSlaves = 5;  // maxSlvsRcd

// classes
EthernetServer52 g_es_webServ(80);  //serv_web                           // start server on http
EthernetServer52 g_es_webServ2(80);                           // start server on http

EthernetServer52 g_es_mbServ(502);  // serv_mb                           // start server on modbus port
EthernetServer52 g_es_mbServ2(502);                           // start server on modbus port
EthernetServer52 g_es_mbServ3(502);                           // start server on modbus port
EthernetServer52 g_es_mbServ4(502);                           // start server on modbus port

ModbusMaster g_mm_node(gk_u8_mb485Ctrl, gk_u8_modbusSerialHardware); // node  // initialize node on device 1, client ip, enable pin, serial port


// miscellaneous
bool g_b_sdInit = false;  // sdInit                                   // set flag corresponding to sd card initializtion

// test vars
//#if DISP_TIMING_DEBUG
//uint32_t doneHttp, gotClient, doneFind, doneSend, time1, time2;
//#endif

// PROTOTYPES:
// main
void resetArd(void);
// handleHTTP
void handle_http(bool b_idleModbus);
// secondaryHTTP - GET and general functions
void flushEthRx(EthernetClient52 &ec_client, uint8_t *u8p_buffer, uint16_t u16_length);
void send404(EthernetClient52 &ec_client);
void sendBadSD(EthernetClient52 &ec_client);
//void sendGifHdr(EthernetClient52 &ec_client);
void sendWebFile(EthernetClient52 &ec_client, const char* ccp_fileName, FileType en_fileType = FileType::NONE);
void sendDownLinks(EthernetClient52 &ec_client, char *const cp_firstLine);
void sendXmlEnd(EthernetClient52 &ec_client, XmlFile en_xmlType);
void sendIP(EthernetClient52 &ec_client);
void liveXML(EthernetClient52 &ec_client);
// tertiaryHTTP - POST related functions
void sendPostResp(EthernetClient52 &ec_client);
char* preprocPost(EthernetClient52 &ec_client, char *cp_httpHdr, uint16_t &u16_postLen);
void getPostSetupData(EthernetClient52 &ec_client, char *cp_httpHdr);
// handleModbus
uint8_t getModbus(uint8_t u8a_mbReq[gk_u16_mbArraySize], uint16_t u16_mbReqLen, uint8_t u8a_mbResp[gk_u16_mbArraySize], uint16_t &u16_mbRespLen, bool b_byteSwap);
void handle_modbus(bool b_idleHttp);
// secondaryModbus
/*bool findRegister(uint16_t u16_reqRegister, uint8_t &u8_regFlags, uint8_t u8_meterType);*/
bool findRegister(uint16_t u16_reqRegister, FloatConv &fltConv, uint8_t u8_meterType);
bool isMeterEth(uint8_t u8_virtId, uint8_t &u8_meterType, uint8_t &u8_trueId);
// setConstants
void setConstants(void);
void writeGenSetupFile(void);
void writeMtrSetupFile(void);
// handleRTC
time_t getNtpTime(void);
time_t getRtcTime(void);
void printTime(time_t t_time);
// handleData
void handle_data();
// secondaryData
/*PwrEgyRegs getElecRegs(uint16_t u16_mtrLibStart);*/
void getFileName(time_t t_time, char *cp_fileName);


#if SHOW_FREE_MEM
extern int __bss_end;
extern void *__brkval;

int getFreeMemory()
{
  int free_memory;

  if((int)__brkval == 0)
    free_memory = ((int)&free_memory) - ((int)&__bss_end);
  else
    free_memory = ((int)&free_memory) - ((int)__brkval);

  return free_memory;
}
#endif

void resetArd() {
//   Serial.println("resetting...");
#if defined(CORE_TEENSY)  // if teensy3.0 or greater
  CPU_RESTART
  delay(20);
#else
   digitalWrite(resetPin, HIGH);
#endif
}


void setup() {
  Serial.begin(9600);
  //Serial.println(F("delay here"));
  //delay(2000);
  //Serial.println(F("delay over"));
  
  // set output pins
#if defined(CORE_TEENSY)  // if teensy3.0 or greater
  pinMode(gk_s16_rtcFailLed, OUTPUT);
  pinMode(gk_s16_battDeadLed, OUTPUT);
#else
  pinMode(resetPin, OUTPUT);
#endif
  pinMode(gk_s16_sdFailLed, OUTPUT);
  pinMode(gk_s16_sdWriteLed, OUTPUT);
  pinMode(gk_s16_epWriteLed, OUTPUT);
  
  // get indices from eeprom
  g_u16_nameBlkStart = word(EEPROM.read(0), EEPROM.read(1));
  g_u16_ipBlkStart = word(EEPROM.read(2), EEPROM.read(3));
  g_u16_mtrBlkStart = word(EEPROM.read(4), EEPROM.read(5));
  g_u16_regBlkStart = word(EEPROM.read(6), EEPROM.read(7));
  //g_c_gwName[30] = 0;

  // take constants from eeprom into memory
  setConstants();

  
//  set serial1 pins high - needed for 485 shield to work
#if defined(CORE_TEENSY)  // if teensy3.0 or greater
  switch (gk_u8_modbusSerialHardware) {
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
#else
  switch (gk_u8_modbusSerialHardware) {
    case 1:
      digitalWrite(19, HIGH);
      digitalWrite(18, HIGH);
      break;
    case 2:
      digitalWrite(17, HIGH);
      digitalWrite(16, HIGH);
      break;
    case 3:
      digitalWrite(15, HIGH);
      digitalWrite(14, HIGH);
      break;
    default:
      digitalWrite(0, HIGH);
      digitalWrite(1, HIGH);
      break;
  }
#endif

  // may be necessary to set pin 10 high regardless of arduino type
  pinMode(10, OUTPUT);  
  digitalWrite(10, HIGH);
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  
//  168 and 328 arduino
#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega328P__)
  Serial.println(F("uno"));
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  pinMode(53, OUTPUT);  // set ethernet high
  digitalWrite(53, HIGH);
  Serial.println(F("mega"));
#elif defined(CORE_TEENSY)  // if teensy3.0 or greater
  pinMode(9, OUTPUT);
  digitalWrite(9, LOW);
  Serial.println(F("teensy"));  
#endif

  
  
  Serial.println(F("Initializing SD card..."));
  if (!SD.begin(4)) {
    Serial.println(F("ERROR - SD card initialization failed!"));
  }
  else{
    Serial.println(F("SUCCESS - SD card initialized."));
    g_b_sdInit = true;

    // generate xml files for web server  SHOULD THIS FIRE AT THE TOP EVERY TIME?  probably, won't know changes if flash_eeprom used
    writeGenSetupFile();
    writeMtrSetupFile();
  }
  
  // start ethernet 
  uint16_t u16a_socketSizes[8] = { 4, 4, 1, 1, 1, 1, 2, 2 };  // sizes are >>10 eg 4->4096
  uint16_t u16a_socketPorts[8] = { 80, 80, 502, 502, 502 ,502, 0, 0 };

  Ethernet52.begin(g_u8a_mac, g_ip_ip, g_ip_gateway, g_ip_gateway, g_ip_subnet, 8, u16a_socketSizes, u16a_socketPorts);

  g_es_webServ.begin();
  g_es_webServ2.begin();

  g_es_mbServ.begin();
  g_es_mbServ2.begin();
  g_es_mbServ3.begin();
  g_es_mbServ4.begin();

  g_mm_node.begin(g_u32_baudrate);
  g_mm_node.setTimeout(g_u16_timeout);
  //  g_mm_node.idle(*function_here);  // add function for idling during wait for modbus return message

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
#if defined(CORE_TEENSY)
    Teensy3Clock.set(t_localTime);
#endif
    setTime(t_localTime);
    g_b_rtcGood = true;
  }

  setSyncInterval(86400);

  // 2 s total delay
  // 450 from flashing leds
  // 1500 from pre ntp wait, 

  digitalWrite(gk_s16_battDeadLed, HIGH);
  delay(50);
  digitalWrite(gk_s16_rtcFailLed, HIGH);
  delay(50);
  digitalWrite(gk_s16_battDeadLed, LOW);
  digitalWrite(gk_s16_sdFailLed, HIGH);
  delay(50);
  digitalWrite(gk_s16_rtcFailLed, LOW);
  digitalWrite(gk_s16_sdWriteLed, HIGH);
  delay(50);
  digitalWrite(gk_s16_sdFailLed, LOW);
  digitalWrite(gk_s16_epWriteLed, HIGH);
  delay(50);
  digitalWrite(gk_s16_sdWriteLed, LOW);
  delay(50);
  digitalWrite(gk_s16_epWriteLed, LOW);

  delay(50);
  digitalWrite(gk_s16_battDeadLed, HIGH);
  digitalWrite(gk_s16_rtcFailLed, HIGH);
  digitalWrite(gk_s16_sdFailLed, HIGH);
  digitalWrite(gk_s16_sdWriteLed, HIGH);
  digitalWrite(gk_s16_epWriteLed, HIGH);
  delay(50);
  digitalWrite(gk_s16_battDeadLed, LOW);
  digitalWrite(gk_s16_rtcFailLed, LOW);
  digitalWrite(gk_s16_sdFailLed, LOW);
  digitalWrite(gk_s16_sdWriteLed, LOW);
  digitalWrite(gk_s16_epWriteLed, LOW);
  delay(50);

  if (!g_b_sdInit) {
    digitalWrite(gk_s16_sdFailLed, HIGH);
  }

  if (!g_b_rtcGood) {
    digitalWrite(gk_s16_rtcFailLed, HIGH);
  }

#if SHOW_FREE_MEM
  Serial.print(F("start: "));
  Serial.println(getFreeMemory());
#endif
}  // end setup


void loop() { 
  handle_modbus(true);
  handle_http(true);
  if (g_b_recordData && g_b_rtcGood) {
#if defined(CORE_TEENSY)  // if teensy3.0 or greater
    handle_data();
    
#else
    handle_data();
#endif
  }
  if (g_b_useNtp && ((millis() - g_u32_rtcNtpLastReset) > gk_u32_rtcNtpResetDelay)) {
    // if enough time has elapsed and we want to use ntp
    time_t t_localTime(0);

    t_localTime = getNtpTime();

    if (t_localTime != 0) {  // set clock to time gotten from ntp
      Teensy3Clock.set(t_localTime);  // just need to set Teensy3 time, class defined time updates from here
      //setTime(t_localTime);  // this should not be strictly necessary, though update will be delayed
      g_b_rtcGood = true;

      digitalWrite(gk_s16_rtcFailLed, LOW);
    }

    g_u32_rtcNtpLastReset = millis();  // reset timer
  }
}  // end loop

