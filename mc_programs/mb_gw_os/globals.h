#ifndef GLOBALS_H
#define GLOBALS_H

#include <Ethernet52.h>
#include <ModbusServer.h>
#include "mb_names.h"

#define DISP_TIMING_DEBUG 0                            // debug flag that will print out delta times for web page interface

#define gk_u16_requestLineSize 40 // extern const uint16_t                      // g_REQ_BUF_SZ buffer size to capture beginning of http request
#define gk_u16_requestBuffSize 1500 // extern const uint16_t                    // g_REQ_ARR_SZ size of array for http request, first REQ_BUF_SZ bytes will always be first part of
                                                       //   message, the rest of the array may loop around if the http request is large enough
#define gk_u16_postBuffSize ((gk_u16_requestBuffSize - 1 - 1030 - 50))  // POST_BUF_SZ currently 419

// FIND ANOTHER WAY TO TEST FOR SMALL POST BUFFER SIZE
//#if gk_u16_postBuffSize < 20                                   // make sure post buffer has enough room to play with, 20 bytes sounds good for min
//#error "not enough room in array for POST messages"
//#endif

#define gk_u16_mbArraySize 264                    // MB_ARR_SIZE array size for modbus/tcp rx/tx buffers - limited by modbus standards

#define gk_u16_respBuffSize 1400              // RESP_BUF_SZ array size for buffer between sd card and ethernet
                                                       //     keep short of 1500 to make room for headers
#define gk_u8_modbusSerialHardware 3           // MODBUS_SERIAL use hardware serial 3

//// reset necessaries
//#define CPU_RESTART_ADDR (uint32_t *)0xE000ED0C
//#define CPU_RESTART_VAL 0x5FA0004
//#define CPU_RESTART (*CPU_RESTART_ADDR = CPU_RESTART_VAL);

// pin ids
#define gk_s16_sdFailLed    21                     // sd card unavailable
#define gk_s16_sdWriteLed   20                     // currently writing to sd card
#define gk_s16_epWriteLed   19                     // currently writing to eeprom
#define gk_s16_rtcFailLed   22                     // no rtc set
#define gk_s16_battDeadLed  23                     // dead battery - currently no way to determine
#define gk_u8_mb485Ctrl     6                     // when set low, transmit mode, high is receive mode

// ethernet info
extern uint8_t g_u8a_mac[8];                      // enter mac, will need some sort of generator for this
extern IPAddress g_ip_ip;                       // this value will be overwritten by ip stored in eeprom
extern IPAddress g_ip_subnet;                    // this value will be overwritten by ip stored in eeprom
extern IPAddress g_ip_gateway;                    // this value will be overwritten by ip stored in eeprom
//bool g_b_reset = false;                                    // bReset

extern bool g_b_useNtp;                                 // bNTPserv turns ntp on/off (overwritten by eeprom)
extern IPAddress g_ip_ntpIp;                      // this value will be overwritten by ip stored in eeprom

// gateway info
extern char g_c_gwName[32];                               // meter_nm name of gateway
// addresses for components stored in eeprom (see flash_eeprom.ino for more details)
extern uint16_t g_u16_nameBlkStart;  // nm_strt
extern uint16_t g_u16_ipBlkStart;  // ip_strt
extern uint16_t g_u16_mtrBlkStart;  // mtr_strt
extern uint16_t g_u16_regBlkStart;  // reg_strt

// modbus info
extern uint32_t g_u32_baudrate;                              // baud rate of 485 network
extern uint8_t g_u8_dataBits;                                // data bits 7 or 8
extern uint8_t g_u8_parity;                                  // 0: None, 1: Odd, 2: Even
extern uint8_t g_u8_stopBits;                                // 1 or 2 stop bits
extern uint16_t g_u16_timeout;                               // modbus timeout

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
extern bool g_b_rtcGood;  // bGoodRTC
extern uint32_t g_u32_rtcNtpLastReset;  // for resetting rtc with ntp
#define gk_u32_rtcNtpResetDelay 604800000UL // delay in ms

// data collection timing
extern uint32_t g_u32_lastDataRequest;  // oldDataTime
extern bool g_b_recordData;  // bRecordData
extern uint8_t g_u8_maxRecordSlaves;  // maxSlvsRcd

// classes
extern EthernetServer52 g_es_webServ;  //serv_web                           // start server on http

extern EthernetServer52 g_es_mbServ;  // serv_mb                           // start server on modbus port

//ModbusMaster g_mm_node(gk_u8_mb485Ctrl, gk_u8_modbusSerialHardware); // node  // initialize node on device 1, client ip, enable pin, serial port
extern ModbusServer g_modbusServer;

// server socket info
extern uint32_t g_u32a_socketTimeoutStart[8];  // time to compare to timeout
extern SockFlag g_u16a_socketFlags[8];  // flag names found in mb_names.h
extern uint16_t  g_u16a_mbReqUnqId[8];  // unique id of modbus request in ModbusStack
extern EthernetClient52 g_eca_socks[8];

// MOVE THESE TO MBSTACK CLASS
extern bool g_b_485avail;  // can assume 485 is open at init
extern bool g_ba_clientSocksAvail[2];  // assume both socks used at init


// miscellaneous
extern bool g_b_sdInit;  // sdInit                                   // set flag corresponding to sd card initializtion
#define gk_i_maxNumElecVals 32
#define gk_i_maxNumStmChwVals 10

// test vars
#if DISP_TIMING_DEBUG
extern uint32_t doneHttp, gotClient, doneFind, doneSend, time1, time2;
#endif


#endif // GLOBALS_H
