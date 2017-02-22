#include "globals.h"
#include <Ethernet52.h>
#include <ModbusServer.h>
#include "mb_names.h"

//const uint16_t gk_u16_requestLineSize(40);                       // g_REQ_BUF_SZ buffer size to capture beginning of http request
//const uint16_t gk_u16_requestBuffSize(1500);                     // g_REQ_ARR_SZ size of array for http request, first REQ_BUF_SZ bytes will always be first part of
                                                       //   message, the rest of the array may loop around if the http request is large enough
//const uint16_t gk_u16_postBuffSize((gk_u16_requestBuffSize - 1 - 1030 - 50));  // POST_BUF_SZ currently 419

// FIND ANOTHER WAY TO TEST FOR SMALL POST BUFFER SIZE
//#if gk_u16_postBuffSize < 20                                   // make sure post buffer has enough room to play with, 20 bytes sounds good for min
//#error "not enough room in array for POST messages"
//#endif

//const uint16_t gk_u16_mbArraySize(264);                    // MB_ARR_SIZE array size for modbus/tcp rx/tx buffers - limited by modbus standards

//const uint16_t gk_u16_respBuffSize(1400);              // RESP_BUF_SZ array size for buffer between sd card and ethernet
//                                                       //     keep short of 1500 to make room for headers
//const uint8_t gk_u8_modbusSerialHardware(3);           // MODBUS_SERIAL use hardware serial 3

//// reset necessaries
//#define CPU_RESTART_ADDR (uint32_t *)0xE000ED0C
//#define CPU_RESTART_VAL 0x5FA0004
//#define CPU_RESTART (*CPU_RESTART_ADDR = CPU_RESTART_VAL);

// pin ids
//const int gk_s16_sdFailLed   = 21;                     // sd card unavailable
//const int gk_s16_sdWriteLed  = 20;                     // currently writing to sd card
//const int gk_s16_epWriteLed  = 19;                     // currently writing to eeprom
//const int gk_s16_rtcFailLed  = 22;                     // no rtc set
//const int gk_s16_battDeadLed = 23;                     // dead battery - currently no way to determine
//const uint8_t gk_u8_mb485Ctrl   =  6;                     // when set low, transmit mode, high is receive mode

// ethernet info
uint8_t g_u8a_mac[8] = {0};                      // enter mac, will need some sort of generator for this
IPAddress g_ip_ip(0, 0, 0, 0);                       // this value will be overwritten by ip stored in eeprom
IPAddress g_ip_subnet(0, 0, 0, 0);                    // this value will be overwritten by ip stored in eeprom
IPAddress g_ip_gateway(0, 0, 0, 0);                    // this value will be overwritten by ip stored in eeprom
//bool g_b_reset = false;                                    // bReset

bool g_b_useNtp = false;                                 // bNTPserv turns ntp on/off (overwritten by eeprom)
IPAddress g_ip_ntpIp(0, 0, 0, 0);                      // this value will be overwritten by ip stored in eeprom

// gateway info
char g_c_gwName[32] = {0};                               // meter_nm name of gateway
// addresses for components stored in eeprom (see flash_eeprom.ino for more details)
uint16_t g_u16_nameBlkStart;  // nm_strt
uint16_t g_u16_ipBlkStart;  // ip_strt
uint16_t g_u16_mtrBlkStart;  // mtr_strt
uint16_t g_u16_regBlkStart;  // reg_strt

// modbus info
uint32_t g_u32_baudrate = 9600;                              // baud rate of 485 network
uint8_t g_u8_dataBits = 8;                            // data bits 7 or 8
uint8_t g_u8_parity = 0;                              // 0: None, 1: Odd, 2: Even
uint8_t g_u8_stopBits = 1;                            // 1 or 2 stop bits
uint16_t g_u16_timeout = 2000;                               // modbus timeout

// slave info
// REMOVE BELOW!
//uint8_t g_u8_numSlaves;                                // number of modbus slaves attached to gateway
//uint8_t g_u8a_slaveIds[20];  // slv_devs                                // array of modbus device ids (meters can share these!)
//uint8_t g_u8a_slaveVids[20];    // slv_vids                              // array of modbus virtual ids (these should be unique!) they can be the same as devs
//uint8_t g_u8a_slaveIps[20][4];  // slv_ips                              // array of slave ips
//uint8_t g_u8a_slaveTypes[20][3]; // slv_typs                              // array of slave meter types
// REMOVE ABOVE!
//uint8_t g_u8a_selectedSlave = 1;      // selSlv                              // selected slave - used for webpage live data
//ModbusStack mbStack;


// rtc info
bool g_b_rtcGood = false;  // bGoodRTC
uint32_t g_u32_rtcNtpLastReset(0UL);  // for resetting rtc with ntp
//const uint32_t gk_u32_rtcNtpResetDelay(604800000UL);  // delay in ms

// data collection timing
uint32_t g_u32_lastDataRequest;  // oldDataTime
bool g_b_recordData = false;  // bRecordData
uint8_t g_u8_maxRecordSlaves = 5;  // maxSlvsRcd

// classes
EthernetServer52 g_es_webServ(80);  //serv_web                           // start server on http

EthernetServer52 g_es_mbServ(502);  // serv_mb                           // start server on modbus port

//ModbusMaster g_mm_node(gk_u8_mb485Ctrl, gk_u8_modbusSerialHardware); // node  // initialize node on device 1, client ip, enable pin, serial port
ModbusServer g_modbusServer(gk_u8_modbusSerialHardware, gk_u8_mb485Ctrl);

// server socket info
uint32_t g_u32a_socketTimeoutStart[8] = { 0 };  // time to compare to timeout
SockFlag g_u16a_socketFlags[8] = { SockFlag_LISTEN, SockFlag_LISTEN, SockFlag_LISTEN, SockFlag_LISTEN,
                                  SockFlag_LISTEN, SockFlag_LISTEN, SockFlag_LISTEN, SockFlag_LISTEN };  // flag names found in mb_names.h
uint16_t  g_u16a_mbReqUnqId[8] = { 0 };  // unique id of modbus request in ModbusStack
EthernetClient52 g_eca_socks[8] = {0, 1, 2, 3, 4, 5, 6, 7};

// MOVE THESE TO MBSTACK CLASS
bool g_b_485avail = true;  // can assume 485 is open at init
bool g_ba_clientSocksAvail[2] = { false, false };  // assume both socks used at init


// miscellaneous
bool g_b_sdInit = false;  // sdInit                                   // set flag corresponding to sd card initializtion
//const int gk_i_maxNumElecVals(32);
//const int gk_i_maxNumStmChwVals(10);

// test vars
#if DISP_TIMING_DEBUG
uint32_t doneHttp, gotClient, doneFind, doneSend, time1, time2;
#endif
