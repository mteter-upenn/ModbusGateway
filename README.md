# Modbus Gateway

This code is for a modbus gateway built with a [Teensy 3.2], W5200 ethernet chip (via [Wiz820io]) and a serial to 485 converter.  The gateway can be attached as a master to a 2-wire 485 circuit to as many slaves as the circuit can handle, as well as communicate to TCP slaves.  The gateway can store up to 20 different devices for standardization of holding register values and TCP slaves.  There are rudimentary modbus mappings for roughly 15 different electric meters and some [KEP] flow computer standards stored in the EEPROM.


### Software
* [Visual Studio] (version 14.0.25425.01) - Main editor used (Arduino IDE is perfectly acceptable)
* [Arduino IDE] (version 1.6.11) - Visual studio requires arduino compiler, version is very important
* [Teensyduino] (version 1.30) - Teensy libraries, version if very important
* [Visual Micro] (version 1609.10.0) - Visual Studio add on



[Teensy 3.2]: https://www.pjrc.com/store/teensy32.html
[Wiz820io]: http://www.wiznet.co.kr/product-item/wiz820io/
[KEP]: http://www.kep.com/productPages/flow-instruments/flow-computer.html
[Visual Studio]: https://www.visualstudio.com/downloads/
[Arduino IDE]: https://www.arduino.cc/en/Main/Software
[Teensyduino]: http://www.pjrc.com/teensy/td_download.html
[Visual Micro]: http://www.visualmicro.com/page/Arduino-Visual-Studio-Downloads.aspx