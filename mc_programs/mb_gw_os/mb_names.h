// mb_names.h

#ifndef _MB_NAMES_h
#define _MB_NAMES_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif




enum SockFlag: uint16_t {  // 16 bit flag,  is this actually 32 bit?
  SockFlag_LISTEN =      0x0000,  // 0,    listen or closed - no communications
  SockFlag_ESTABLISHED = 0x0001,  // 1,    comms with client - active
  SockFlag_MODBUS =      0x0002,  // 2,    port 502
  SockFlag_HTTP =        0x0004,  // 4,    port 80
  SockFlag_MB485 =       0x0008,  // 8,    modbus device connected via 485 (local)  (UNUSED?)
  SockFlag_MBTCP =       0x0010,  // 16,   modbus device connected via ethernet (not local)  (UNUSED?)
  SockFlag_READ_REQ =    0x0020,  // 32,   initial message from client
  SockFlag_CLIENT =      0x0040,  // 64,   high if gateway acting as client to tcp slave
  SockFlag_GET =         0x0080,  // 128,  GET http request 
  SockFlag_POST =        0x0100,  // 256,  POST http request

};

inline SockFlag operator| (SockFlag a, SockFlag b) {
  return static_cast<SockFlag>(static_cast<int>(a) | static_cast<int>(b));
}

inline SockFlag& operator|= (SockFlag& a, SockFlag b) {
  return a = static_cast<SockFlag>(static_cast<int>(a) | static_cast<int>(b));
}

inline SockFlag operator& (SockFlag a, SockFlag b) {
  return static_cast<SockFlag>(static_cast<int>(a) & static_cast<int>(b));
}

inline SockFlag& operator&= (SockFlag& a, SockFlag b) {
  return a = static_cast<SockFlag>(static_cast<int>(a) & static_cast<int>(b));
}

inline SockFlag operator~ (SockFlag a) {
  return static_cast<SockFlag>(~static_cast<int>(a));
}



enum class FileType : int16_t {
  NONE = -1,
  HTML,
  CSS,
  GIF,
  XML,
  CSV,
  TXT
};

enum class XmlFile {
  INFO,
  GENERAL,
  METER
};

enum FileReq : uint16_t {
  FileReq_NOPAGE = 0,
  FileReq_EPSTYLE,    // css
  FileReq_INDEX,      // html
  FileReq_GENSETUP,   // html, xml, post
  FileReq_MTRSETUP,   // html, xml, post
  FileReq_LIVE,      // html
  //FileReq_SLIVE,      // html
  //FileReq_CLIVE,      // html
  FileReq_PASTDOWN,   // html
  FileReq_PASTVIEW,   // html
  FileReq_RESET,      // html
  FileReq_DATA,       // xml
  FileReq_INFO,       // xml
  FileReq_RESTART,    // xml
  FileReq_404,        // generic 404 response
  //FileReq_,
  //FileReq_,
  FileReq_LOGO_LET
};

#endif

