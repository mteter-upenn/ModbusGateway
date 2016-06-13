/* 
	Editor: http://www.visualmicro.com
	        visual micro and the arduino ide ignore this code during compilation. this code is automatically maintained by visualmicro, manual changes to this file will be overwritten
	        the contents of the Visual Micro sketch sub folder can be deleted prior to publishing a project
	        all non-arduino files created by visual micro and all visual studio project or solution files can be freely deleted and are not required to compile a sketch (do not delete your own code!).
	        note: debugger breakpoints are stored in '.sln' or '.asln' files, knowledge of last uploaded breakpoints is stored in the upload.vmps.xml file. Both files are required to continue a previous debug session without needing to compile and upload again
	
	Hardware: Teensy 3.2 / 3.1, Platform=avr, Package=teensy
*/

#ifndef _VSARDUINO_H_
#define _VSARDUINO_H_
#define __MK20DX256__
#define TEENSYDUINO 125
#define F_CPU 96000000
#define USB_SERIAL
#define LAYOUT_US_ENGLISH
#define ARDUINO 165
#define ARDUINO_ARCH_AVR
#define __cplusplus
#define __AVR__
#define __inline__
#define __asm__(x)
#define __extension__
//#define __ATTR_PURE__
//#define __ATTR_CONST__
#define __inline__
//#define __asm__ 
#define __volatile__
#define GCC_VERSION 40801
#define volatile(va_arg) 
#define _CONST
typedef void *__builtin_va_list;
#define __builtin_va_start
#define __builtin_va_end
//#define __DOXYGEN__
#define __attribute__(x)
#define NOINLINE __attribute__((noinline))
#define prog_void
#define PGM_VOID_P int
#ifndef __builtin_constant_p
#define __builtin_constant_p __attribute__((__const__))
#endif
#ifndef __builtin_strlen
#define __builtin_strlen  __attribute__((__const__))
#endif
#define NEW_H
/*
#ifndef __ATTR_CONST__
#define __ATTR_CONST__ __attribute__((__const__))
#endif

#ifndef __ATTR_MALLOC__
#define __ATTR_MALLOC__ __attribute__((__malloc__))
#endif

#ifndef __ATTR_NORETURN__
#define __ATTR_NORETURN__ __attribute__((__noreturn__))
#endif

#ifndef __ATTR_PURE__
#define __ATTR_PURE__ __attribute__((__pure__))
#endif            
*/
typedef unsigned char byte;
extern "C" void __cxa_pure_virtual() {;}



#include <arduino.h>
#undef F
#define F(string_literal) ((const PROGMEM char *)(string_literal))
#undef PSTR
#define PSTR(string_literal) ((const PROGMEM char *)(string_literal))
#undef cli
#define cli()
#define pgm_read_byte(address_short)
#define pgm_read_word(address_short)
#define pgm_read_word2(address_short)
#define digitalPinToPort(P)
#define digitalPinToBitMask(P) 
#define digitalPinToTimer(P)
#define analogInPinToBit(P)
#define portOutputRegister(P)
#define portInputRegister(P)
#define portModeRegister(P)
#include <..\flash_eeprom\flash_eeprom.ino>
#include <..\flash_eeprom\meter01.ino>
#include <..\flash_eeprom\meter02.ino>
#include <..\flash_eeprom\meter03.ino>
#include <..\flash_eeprom\meter04.ino>
#include <..\flash_eeprom\meter05.ino>
#include <..\flash_eeprom\meter06.ino>
#include <..\flash_eeprom\meter07.ino>
#include <..\flash_eeprom\meter08.ino>
#include <..\flash_eeprom\meter09.ino>
#include <..\flash_eeprom\meter10.ino>
#include <..\flash_eeprom\meter11.ino>
#include <..\flash_eeprom\meter12.ino>
#include <..\flash_eeprom\meter13.ino>
#include <..\flash_eeprom\meter14.ino>
#include <..\flash_eeprom\meter15.ino>
#include <..\flash_eeprom\read_eeprom.ino>
#include <..\flash_eeprom\term_func.ino>
#include <..\flash_eeprom\writeLibrary.ino>
#endif
