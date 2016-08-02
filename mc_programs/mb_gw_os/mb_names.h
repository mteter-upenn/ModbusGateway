// mb_names.h

#ifndef _MB_NAMES_h
#define _MB_NAMES_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

enum class FileType {
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

#endif

