#-------------------------------------------------
#
# Project created by QtCreator
#
#-------------------------------------------------

SOURCES += 	mb_gw_os.ino \
    globals.cpp \
    handleData.cpp \
    handleHTTP.cpp \
    handleModbus.cpp \
    handleRTC.cpp \
    handleServers.cpp \
    secondaryData.cpp \
    secondaryHTTP.cpp \
    secondaryModbus.cpp \
    tertiaryHTTP.cpp \
    miscFuncs.cpp

HEADERS += \ 
    globals.h \
    mb_names.h \
    main.cpp.ignore \
    handleHTTP.h \
    handleData.h \
    handleModbus.h \
    handleRTC.h \
    handleServers.h \
    secondaryData.h \
    secondaryHTTP.h \
    secondaryModbus.h \
    tertiaryHTTP.h \
    miscFuncs.h

DISTFILES +=     Makefile

INCLUDEPATH = /home/fresep/Programs/arduino-1.8.0/hardware/teensy/avr/cores/teensy3 \
    /home/fresep/Programs/arduino-1.8.0/hardware/teensy/avr/libraries/EEPROM \
    /home/fresep/Programs/arduino-1.8.0/hardware/teensy/avr/libraries/SPI \
    /home/fresep/Programs/arduino-1.8.0/hardware/teensy/avr/libraries/Time \
    /home/fresep/Programs/arduino-1.8.0/hardware/teensy/avr/libraries/SD \
    /home/fresep/Arduino/libraries/ModbusStructs \
    /home/fresep/Arduino/libraries/MeterLibrary \
    /home/fresep/Arduino/libraries/ModbusStack \
    /home/fresep/Arduino/libraries/ModbusServer \
    /home/fresep/Arduino/libraries/Ethernet52


