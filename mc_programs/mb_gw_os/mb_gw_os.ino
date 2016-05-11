
/*
 Modbus gateway sketch
 */

#include <Time.h>
#include <ByteBuffer.h>
#include <SPI.h>
#include <Ethernet.h>
#include <ModbusMaster.h>
#include <EEPROM.h>
#include <SD.h>
#include <EthernetUdp.h>

#define REQ_BUF_SZ   40                                // buffer size to capture beginning of http request
#define ARR_SIZE     264                               // array size for modbus/tcp rx/tx buffers

//#define UPENN_TEENSY_MBGW

#if defined(CORE_TEENSY)  // if teensy3.0 or greater
#define STRM_BUF_SZ  3584                              // array size for buffer between sd card and ethernet
#else
#define STRM_BUF_SZ  1024                              // array size for buffer between sd card and ethernet
#endif

#if defined(CORE_TEENSY)  // if teensy3.0 or greater
#define MODBUS_SERIAL 3
#else
#define MODBUS_SERIAL 1                                // which hardware serial to use
#endif

#define DISP_TIMING_DEBUG 0                            // debug flag that will print out delta times for web page interface
#define RT_FROM_NTP 1                                  // 1 for ntp, 0 for rtc

// pin ids



#if defined(CORE_TEENSY)  // if teensy3.0 or greater
//int resetPin = 255;                                      // when set high, will trigger hard reset
int sdFailLed = 21;                                     // 
int sdWriteLed = 20;
int epWriteLed = 19;
int rtcFailLed = 22;
int battDeadLed = 23;
int mb485Ctrl = 6;                                     // when set low, transmit mode, high is receive mode?

#define CPU_RESTART_ADDR (uint32_t *)0xE000ED0C
#define CPU_RESTART_VAL 0x5FA0004
#define CPU_RESTART (*CPU_RESTART_ADDR = CPU_RESTART_VAL);
#else
int resetPin = 6;                                      // when set high, will trigger hard reset
int sdFailLed = 7;                                     // 
int sdWriteLed = 11;
int epWriteLed = 8;
int mb485Ctrl = 9;
int rtcFailLed = 8;    // USING EPWRITELED IS SILLY
#endif

// ethernet info
uint8_t mac[] = {0x46, 0x52, 0x45, 0x53, 0x00, 0x00};  // enter mac, will need some sort of generator for this
IPAddress ip(130, 91, 138, 140);                       // this value will be overwritten by ip stored in eeprom
IPAddress subnet(255, 255, 252, 0);                    // this value will be overwritten by ip stored in eeprom
IPAddress gateway(130, 91, 136, 1);                    // this value will be overwritten by ip stored in eeprom
uint8_t clientIP[] = {192, 168, 1, 1};                 // ip of slave on pseudo modbus network
bool bReset = false;

// gateway info
char meter_nm[31] = {0};                               // name of gateway
uint16_t nm_strt, ip_strt, mtr_strt, reg_strt;         // addresses for components stored in eeprom (see flash_eeprom.ino for more details)

// modbus info
uint32_t baudrate = 9600;                              // baud rate of 485 network
uint16_t timeout = 2000;                               // modbus timeout

// slave info
uint8_t slaves;                                        // number of modbus slaves attached to gateway
uint8_t slv_devs[20];                                  // array of modbus device ids (meters can share these!)
uint8_t slv_vids[20];                                  // array of modbus virtual ids (these should be unique!) they can be the same as devs
uint8_t slv_ips[20][4];                                // array of slave ips
uint8_t slv_typs[20][3];                               // array of slave meter types
uint8_t selSlv = 1;                                    // selected slave - used for webpage live data

// rtc info
bool bGoodRTC = false;
//uint32_t curExcelDay;                                  // current day - gotten from ntp
//uint32_t initExcelSecs;                                // time recieved from ntp
//uint32_t oldArdTime;                                   // local time recorded when message recieved from ntp server

// data collection timing
uint32_t oldDataTime;
bool bRecordData = false;
uint8_t maxSlvsRcd = 5;

// classes
EthernetServer serv_web(80);                           // start server on http
#ifdef UPENN_TEENSY_MBGW
EthernetServer serv_web2(80);                           // start server on http
#endif

EthernetServer serv_mb(502);                           // start server on modbus port
#ifdef UPENN_TEENSY_MBGW
EthernetServer serv_mb2(502);                           // start server on modbus port
EthernetServer serv_mb3(502);                           // start server on modbus port
EthernetServer serv_mb4(502);                           // start server on modbus port
#endif

ModbusMaster node(1, clientIP, mb485Ctrl, MODBUS_SERIAL);                  // initialize node on device 1, client ip, enable pin, serial port


// miscellaneous
ByteBuffer post_cont;                                  // circular buffer for string searches within entire http request
bool sdInit = false;                                   // set flag corresponding to sd card initializtion

// test vars
#if DISP_TIMING_DEBUG
uint32_t doneHttp, gotClient, doneFind, doneSend, time1, time2;
#endif

// PROTOTYPES:
// main
void resetArd(void);
// handleHTTP
void handle_http(void);
// secondaryHTTP
void send404(EthernetClient);
void sendBadSD(EthernetClient);
void sendGifHdr(EthernetClient);
void sendWebFile(EthernetClient, const char*);
void sendDownLinks(EthernetClient, char*);
void sendXmlEnd(EthernetClient, uint8_t);
void sendIP(EthernetClient);
void sendPostResp(EthernetClient);
void liveXML(EthernetClient);
void getPostSetupData(EthernetClient, uint16_t);
// handleModbus
bool getModbus(uint8_t*, uint16_t, uint8_t*, uint16_t&);
void handle_modbus(void);
// secondaryModbus
bool findRegister(uint16_t&, uint8_t&, uint8_t);
bool isMeterEth(uint8_t, uint8_t&, uint8_t&);
// setConstants
void setConstants(void);
void writeGenSetupFile(void);
void writeMtrSetupFile(void);
// stringFuncs
void StrClear(char*, char);
//byte StrContains(char*, const char*);
// handleRTC
time_t getNtpTime(void);
time_t getRtcTime(void);
void handle_RT(void);
void printTime(time_t);
// handleData
void handle_data(void);
// secondaryData
void getElecRegs(uint16_t, uint8_t, uint16_t&, uint16_t&);
void getFileName(time_t, char*);

// debugging
bool getFakeTime(void);


//extern int __bss_end;
//extern void *__brkval;
//
//int get_free_memory()
//{
//  int free_memory;
//
//  if((int)__brkval == 0)
//    free_memory = ((int)&free_memory) - ((int)&__bss_end);
//  else
//    free_memory = ((int)&free_memory) - ((int)__brkval);
//
//  return free_memory;
//}


void resetArd(){
//   Serial.println("resetting...");
#if defined(CORE_TEENSY)  // if teensy3.0 or greater
  CPU_RESTART
  delay(20);
#else
   digitalWrite(resetPin, HIGH);
#endif
}


void setup() {
  time_t t;

  Serial.begin(9600);
  //Serial.println(F("delay here"));
  //delay(2000);
  //Serial.println(F("delay over"));
  
#if defined(CORE_TEENSY)  // if teensy3.0 or greater
  pinMode(rtcFailLed, OUTPUT);
  pinMode(battDeadLed, OUTPUT);
#else
  pinMode(resetPin, OUTPUT);
#endif
  pinMode(sdFailLed, OUTPUT);
  pinMode(sdWriteLed, OUTPUT);
  pinMode(epWriteLed, OUTPUT);
  
  
  nm_strt = word(EEPROM.read(0), EEPROM.read(1));
  ip_strt = word(EEPROM.read(2), EEPROM.read(3));
  mtr_strt = word(EEPROM.read(4), EEPROM.read(5));
  reg_strt = word(EEPROM.read(6), EEPROM.read(7));
  meter_nm[30] = 0;
  setConstants();

  
//  set serial1 pins high - needed for 485 shield to work
#if defined(CORE_TEENSY)  // if teensy3.0 or greater
#if MODBUS_SERIAL == 2
  digitalWrite(9, HIGH);
  digitalWrite(10, HIGH);
#elif MODBUS_SERIAL == 3
  /*pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);*/
  pinMode(7, INPUT_PULLUP);
  //pinMode(8, OUTPUT);
  digitalWrite(7, HIGH);
  //digitalWrite(8, HIGH);
  //Serial3.transmitterEnable(6);
  /*digitalWrite(7, LOW);
  digitalWrite(8, LOW);*/
#else
  digitalWrite(0, HIGH);
  digitalWrite(1, HIGH);
#endif
#else
#if MODBUS_SERIAL == 1
  digitalWrite(19, HIGH);
  digitalWrite(18, HIGH);
#elif MODBUS_SERIAL == 2
  digitalWrite(17, HIGH);
  digitalWrite(16, HIGH);
#elif MODBUS_SERIAL == 3
  digitalWrite(15, HIGH);
  digitalWrite(14, HIGH);
#else
  digitalWrite(0, HIGH);
  digitalWrite(1, HIGH);
#endif
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
    
    //digitalWrite(sdFailLed, HIGH);
  }
  else{
    Serial.println(F("SUCCESS - SD card initialized."));
    sdInit = true;

    writeGenSetupFile();
    writeMtrSetupFile();
  }

  /*delay(1);
  digitalWrite(9, HIGH);
  digitalWrite(4, HIGH);
  digitalWrite(10, HIGH);
  delay(150);*/

  
  Ethernet.begin(mac, ip, gateway, gateway, subnet);
  
  serv_web.begin();
#ifdef UPENN_TEENSY_MBGW
  serv_web2.begin();
#endif

  serv_mb.begin();
#ifdef UPENN_TEENSY_MBGW
  serv_mb2.begin();
  serv_mb3.begin();
  serv_mb4.begin();
#endif

  node.begin(baudrate);
  node.setTimeout(timeout);
  //Serial3.begin(9600);
  
  post_cont.init(16);  // creates circular buffer 16 bytes around

  delay(1100);
  //setTime(4, 40, 0, 30, 10, 2015);
  setSyncProvider(getRtcTime);

  t = getNtpTime();
  if (t == 0) {
    if (getRtcTime() > 1451606400UL) {
      bGoodRTC = true;  // time in RTC is greater than Jan 1 2016
    }
    else {
      //digitalWrite(rtcFailLed, HIGH);  // no connection to ntp servers and rtc has not been set
    }
  }
  else {
#if defined(CORE_TEENSY)
    Teensy3Clock.set(t);
#endif
    setTime(t);
    bGoodRTC = true;
  }

  setSyncInterval(86400);
  //setSyncInterval(3600);


//#if RT_FROM_NTP
//  setSyncProvider(getNtpTime);
//  //getFakeTime();
//#else
//  setSyncProvider(getRtcTime);
//#endif
  
//  node.idle(*function_here);  // add function for idling during wait for modbus return message
  delay(550);  // initial delay


  digitalWrite(battDeadLed, HIGH);
  delay(50);
  digitalWrite(rtcFailLed, HIGH);
  delay(50);
  digitalWrite(battDeadLed, LOW);
  digitalWrite(sdFailLed, HIGH);
  delay(50);
  digitalWrite(rtcFailLed, LOW);
  digitalWrite(sdWriteLed, HIGH);
  delay(50);
  digitalWrite(sdFailLed, LOW);
  digitalWrite(epWriteLed, HIGH);
  delay(50);
  digitalWrite(sdWriteLed, LOW);
  delay(50);
  digitalWrite(epWriteLed, LOW);

  delay(50);
  digitalWrite(battDeadLed, HIGH);
  digitalWrite(rtcFailLed, HIGH);
  digitalWrite(sdFailLed, HIGH);
  digitalWrite(sdWriteLed, HIGH);
  digitalWrite(epWriteLed, HIGH);
  delay(50);
  digitalWrite(battDeadLed, LOW);
  digitalWrite(rtcFailLed, LOW);
  digitalWrite(sdFailLed, LOW);
  digitalWrite(sdWriteLed, LOW);
  digitalWrite(epWriteLed, LOW);
  delay(50);

  if (!sdInit) {
    digitalWrite(sdFailLed, HIGH);
  }

  if (!bGoodRTC) {
    digitalWrite(rtcFailLed, HIGH);
  }
}  // end setup


void loop()
{ 
  //handle_RT(); // not necessary with time.h
  handle_modbus();
  handle_http();
  if (bRecordData && bGoodRTC) {
#if defined(CORE_TEENSY)  // if teensy3.0 or greater
    handle_data();
    
#else
    handle_data();
#endif
  }
}  // end loop

