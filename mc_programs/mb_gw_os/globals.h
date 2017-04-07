#ifndef GLOBALS_H
#define GLOBALS_H

#include <Ethernet52.h>
#include <ModbusServer.h>
#include "mb_names.h"
#include <ModbusStructs.h>

#define DISP_TIMING_DEBUG 0                            // debug flag that will print out delta times for web page interface

#define REQUEST_LINE_SIZE 40 // extern const uint16_t                      // g_REQ_BUF_SZ buffer size to capture beginning of http request
#define REQUEST_BUFFER_SIZE 1500 // extern const uint16_t                    // g_REQ_ARR_SZ size of array for http request, first REQ_BUF_SZ bytes will always be first part of
                                                       //   message, the rest of the array may loop around if the http request is large enough
#define POST_BUFFER_SIZE ((REQUEST_BUFFER_SIZE - 1 - 1030 - 50))  // POST_BUF_SZ currently 419

// FIND ANOTHER WAY TO TEST FOR SMALL POST BUFFER SIZE
#if POST_BUFFER_SIZE < 20                                   // make sure post buffer has enough room to play with, 20 bytes sounds good for min
#error "not enough room in array for POST messages"
#endif

#define MB_ARRAY_SIZE 264                    // MB_ARR_SIZE array size for modbus/tcp rx/tx buffers - limited by modbus standards

#define RESPONSE_BUFFER_SIZE 1400              // RESP_BUF_SZ array size for buffer between sd card and ethernet
                                                       //     keep short of 1500 to make room for headers
#define MB_SERIAL_HARDWARE_PORT 3           // MODBUS_SERIAL use hardware serial 3

// pin ids
#define SD_FAIL_LED_PIN        21                     // sd card unavailable
#define SD_WRITE_LED_PIN       20                     // currently writing to sd card
#define EEPROM_WRITE_LEN_PIN   19                     // currently writing to eeprom
#define RTC_FAIL_LED_PIN       22                     // no rtc set
#define BATT_DEAD_LED_PIN      23                     // dead battery - currently no way to determine
#define MB_485_CTRL_PIN         6                     // when set low, transmit mode, high is receive mode
#define SPI_SD_PIN              4
#define SPI_ETHERNET_PIN       10
#define W5200_RESET_PIN         9

#if MB_SERIAL_HARDWARE_PORT == 2
#define MB_SERIAL_RX_PIN        9
#define MB_SERIAL_TX_PIN       10
#elif MB_SERIAL_HARDWARE_PORT == 3
#define MB_SERIAL_RX_PIN        7
#define MB_SERIAL_TX_PIN        8
#else
#define MB_SERIAL_RX_PIN        0
#define MB_SERIAL_TX_PIN        1
#endif

// ethernet info
extern MacArray g_u8a_mac;                      // enter mac, will need some sort of generator for this
extern IpArray g_ip_ip;
extern IpArray g_ip_subnet;                    // this value will be overwritten by ip stored in eeprom
extern IpArray g_ip_gateway;                    // this value will be overwritten by ip stored in eeprom

extern bool g_b_useNtp;                                 // bNTPserv turns ntp on/off (overwritten by eeprom)
extern IpArray g_ip_ntpIp;                      // this value will be overwritten by ip stored in eeprom

// gateway info
extern NameArray g_gwName;  //[32];                               // meter_nm name of gateway
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

// rtc info
extern bool g_b_rtcGood;  // bGoodRTC
extern uint32_t g_u32_rtcNtpLastReset;  // for resetting rtc with ntp
#define NTP_RESET_DELAY 604800000UL // delay in ms
#define TIME_ZONE_DIFF (0UL)

// data collection timing
extern uint32_t g_u32_lastDataRequest;  // oldDataTime
extern bool g_b_recordData;  // bRecordData
extern uint8_t g_u8_maxRecordSlaves;  // maxSlvsRcd

// classes
extern EthernetServer52 g_es_webServ;  //serv_web                           // start server on http

extern EthernetServer52 g_es_mbServ;  // serv_mb                           // start server on modbus port

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
#define MAX_NUM_ELEC_VALS 32
#define MAX_NUM_STMCHW_VALS 10

// test vars
#if DISP_TIMING_DEBUG
extern uint32_t doneHttp, gotClient, doneFind, doneSend, time1, time2;
#endif


#endif // GLOBALS_H
