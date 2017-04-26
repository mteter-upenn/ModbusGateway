#-------------------------------------------------
#
# Project created by QtCreator
#
#-------------------------------------------------

SOURCES += 	flash_eeprom.ino \
    read_eeprom.cpp \
    term_func.cpp \
    writeLibrary.cpp \
    globals.cpp

HEADERS += \ 
    globals.h \
    read_eeprom.h \
    term_func.h \
    writeLibrary.h

DISTFILES +=     Makefile

INCLUDEPATH = /home/fresep/Programs/arduino-1.8.0/hardware/teensy/avr/cores/teensy3 \
/home/fresep/Programs/arduino-1.8.0/hardware/teensy/avr/libraries/SD \
/home/fresep/Arduino/libraries/ModbusStructs \
/home/fresep/Arduino/libraries/ModbusStack \
/home/fresep/Arduino/libraries/MeterLibrary \
/home/fresep/Arduino/libraries/mac \
/home/fresep/Arduino/libraries/ArduinoJson \
/home/fresep/Arduino/libraries/FRAM_MB85RS_SPI
#/home/fresep/Programs/arduino-1.8.0/hardware/teensy/avr/libraries/EEPROM \
