# Modbus Gateway

This code is for a modbus gateway built with a [Teensy 3.2], W5200 ethernet chip (via [Wiz820io]) and a serial to 485 converter.  The gateway can be attached as a master to a 2-wire 485 circuit to as many slaves as the circuit can handle, as well as communicate to TCP slaves.  The gateway can store up to 20 different devices for standardization of holding register values and TCP slaves.  There are rudimentary modbus mappings for roughly 15 different electric meters and some [KEP] flow computer standards stored in the EEPROM.


### Software
* [Visual Studio] (version 14.0.25425.01) - Main editor used (Arduino IDE is perfectly acceptable)
* [Arduino IDE] (version 1.6.11) - Visual studio requires arduino compiler, version is very important
* [Teensyduino] (version 1.30) - Teensy libraries, version if very important
* [Visual Micro] (version 1609.10.0) - Visual Studio add on

### Installation
It is best to install Visual Studio first (be sure to include the C++ package).  Then the Arduino IDE and Teensyduino.  Finally, install Visual Micro.


Before starting up Visual Studio, there are some files that need to be changed.  Navigate to `C:\Program Files (x86)\Arduino\teensy\avr\` (or wherever the Arduino IDE is stored) and append `_old` to `boards.txt` and `platform.txt`.  Next, copy the `boards.txt` and `platform.txt` files from `REPO_LOCATION\mc_programs\arduino_ide_setup`. Do the same for `serialX.c` files in  `C:\Program Files (x86)\Arduino\teensy\avr\cores\teensy3` and find the new files in `REPO_LOCATION\mc_programs\arduino_ide_setup\Serial_files`.  These files give drop down options and tell the compiler all the necessary flags that go with those options.  If these files are not added before Visual Studio is started, then it will need to be resarted for those changes to take place.  These changes give the ability to quickly change the size of serial buffers.


We also need to make links to the libraries stored in the local repository.  In Windows use `mklink /J ...\Documents\Arduino\libraries\LIBRARY_NAME ...\REPO_LOCATION\mc_programs\libraries\LIBRARY_NAME`.  As of v0.8.0d, the only libraries in use are Ethernet52, mac, MeterLibrary, ModbusServer, ModbusStack, and ModbusStructs.



[Teensy 3.2]: https://www.pjrc.com/store/teensy32.html
[Wiz820io]: http://www.wiznet.co.kr/product-item/wiz820io/
[KEP]: http://www.kep.com/productPages/flow-instruments/flow-computer.html
[Visual Studio]: https://www.visualstudio.com/downloads/
[Arduino IDE]: https://www.arduino.cc/en/Main/Software
[Teensyduino]: http://www.pjrc.com/teensy/td_download.html
[Visual Micro]: http://www.visualmicro.com/page/Arduino-Visual-Studio-Downloads.aspx