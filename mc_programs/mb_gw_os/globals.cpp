#include "globals.h"
#include <Ethernet52.h>
#include <ModbusServer.h>
#include "mb_names.h"

// ethernet info
MacArray g_u8a_mac;                      // enter mac, will need some sort of generator for this
IpArray g_ip_ip;
IpArray g_ip_subnet;  //(0, 0, 0, 0);                    // this value will be overwritten by ip stored in eeprom
IpArray g_ip_gateway;  //(0, 0, 0, 0);                    // this value will be overwritten by ip stored in eeprom

bool g_b_useNtp = false;                                 // bNTPserv turns ntp on/off (overwritten by eeprom)
IpArray g_ip_ntpIp;  //(0, 0, 0, 0);                      // this value will be overwritten by ip stored in eeprom

// gateway info
NameArray g_gwName;
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

// rtc info
bool g_b_rtcGood = false;  // bGoodRTC
uint32_t g_u32_rtcNtpLastReset(0UL);  // for resetting rtc with ntp

// data collection timing
uint32_t g_u32_lastDataRequest;  // oldDataTime
bool g_b_recordData = false;  // bRecordData
uint8_t g_u8_maxRecordSlaves = 5;  // maxSlvsRcd

// classes
EthernetServer52 g_es_webServ(80);  //serv_web                           // start server on http

EthernetServer52 g_es_mbServ(502);  // serv_mb                           // start server on modbus port

ModbusServer g_modbusServer(MB_SERIAL_HARDWARE_PORT, MB_485_CTRL_PIN);

// server socket info
uint32_t g_u32a_socketTimeoutStart[8] = { 0 };  // time to compare to timeout
SockFlag g_u16a_socketFlags[8] = { SockFlag_LISTEN, SockFlag_LISTEN, SockFlag_LISTEN, SockFlag_LISTEN,
                                  SockFlag_LISTEN, SockFlag_LISTEN, SockFlag_LISTEN, SockFlag_LISTEN };  // flag names found in mb_names.h
uint16_t  g_u16a_mbReqUnqId[8] = { 0 };  // unique id of modbus request in ModbusStack
EthernetClient52 g_eca_socks[8] = {0, 1, 2, 3, 4, 5, 6, 7};

// MOVE THESE TO MBSTACK CLASS
bool g_b_485avail = true;  // can assume 485 is open at init
bool g_ba_clientSocksAvail[2] = { true, true };  // assume both socks used at init


// miscellaneous
bool g_b_sdInit = false;  // sdInit                                   // set flag corresponding to sd card initializtion

// test vars
#if DISP_TIMING_DEBUG
uint32_t doneHttp, gotClient, doneFind, doneSend, time1, time2;
#endif
