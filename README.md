# Modbus Gateway

This code is for a modbus gateway built with a [Teensy 3.2][teensy 3.2], W5200 ethernet chip (via a [Wiz820io][wiz820io]) and a serial to 485 converter.  The gateway can be attached as a master to a 2-wire 485 circuit to as many slaves as the circuit can handle, as well as communicate to TCP slaves.  The gateway can store up to 20 different devices for standardization of holding register values and TCP slaves.  There are rudimentary modbus mappings for roughly 15 different electric meters and some [KEP flow computer][kep] standards stored in the EEPROM.

## Table of Contents
[Software](#software)  
[Installation](#installation)  
[Gateway Setup](#gateway-setup)  
[New Modbus Mappings](#new-modbus-mappings)  
[Appendix](#appendix)

### Software
* [Visual Studio][visual studio] (version 14.0.25425.01) - Main editor used (Arduino IDE is perfectly acceptable)
* [Arduino IDE][arduino ide] (version 1.6.11) - Visual studio requires arduino compiler, version is very important
* [Teensyduino][teensyduino] (version 1.30) - Teensy libraries, version if very important
* [Visual Micro][visual micro] (version 1609.10.0) - Visual Studio add on

### Installation
It is best to install Visual Studio first (be sure to include the C++ package).  Then the Arduino IDE and Teensyduino.  Finally, install Visual Micro.


Before starting up Visual Studio or Arduino IDE, there are some files that need to be changed.  Navigate to `C:\Program Files (x86)\Arduino\teensy\avr\` (or wherever the Arduino IDE is stored) and append `_old` to `boards.txt` and `platform.txt`.  Next, copy the `boards.txt` and `platform.txt` files from `REPO_LOCATION\mc_programs\arduino_ide_setup`. Do the same for `serialX.c` files in  `C:\Program Files (x86)\Arduino\teensy\avr\cores\teensy3` and find the new files in `REPO_LOCATION\mc_programs\arduino_ide_setup\Serial_files`.  These files give drop down options and tell the compiler all the necessary flags that go with those options.  If these files are not added before Visual Studio is started, then it will need to be resarted for those changes to take place.  These changes give the ability to quickly change the size of serial buffers.


We also need to make links to the libraries stored in the local repository.  In Windows use this in the command line: `mklink /J ...\Documents\Arduino\libraries\LIBRARY_NAME ...\REPO_LOCATION\mc_programs\libraries\LIBRARY_NAME`.  As of v0.8.0d, the only libraries in use are Ethernet52, mac, MeterLibrary, ModbusServer, ModbusStack, and ModbusStructs.

### Gateway Setup
To set up the gateway, first compile the flash_eeprom project and download to the Teensy.  Visual Micro's serial monitor has typically been somewhat buggy and will crash Visual Studio, so use at your own risk.  The safer option is to use the serial monitor in Arduino IDE.  Make sure to set the line ending to newline.  Follow the instructions to fill out the information the gateway needs to operate.  Once this is completed, mb_gw_os can be compiled and downloaded to the Teensy.

### New Modbus Mappings
Modbus mappings are made up of blocks and groups.  Blocks show the consecutive registers for a given data type.  There can be more than one block associated with a data type.  Each block consists of 5 bytes.

##### Block Listing
| Byte | Value                          |
| ---- | ------------------------------ |
| 0    | High byte of starting register |
| 1    | Low byte of starting register  |
| 2    | High byte of final register    |
| 3    | Low byte of final register     |
| 4    | Data type stored as int8_t     |

The registers are stored in a zero based format.  The [data types](#floatConv-enum-class) are identified with the `enum class FloatConv`.  A converting function must be used to store in EEPROM.  Groups map desired values (demand, energy, etc.) to their zero based registers.  Each group is made up of as many bytes as it needs.  It is made up of three main parts- a header, a list of values as they appear in the meter's modbus map, and a list of their data types.  

##### Group Header
| Byte | Value                                                                             |
| ---- | --------------------------------------------------------------------------------- |
| 0    | Number of values in group                                                         |
| 1    | Span of registers (must be less than 126- maximum bytes read through Modbus\TCP)  |
| 2    | High byte of starting register                                                    |
| 3    | Low byte of starting register                                                     |
| 4    | Number of bytes in header and listing section ( 5 + n)                            |

##### Group Listing
| Byte | Value                        |
| ----- | --------------------------- |
| 0     | Value type                  |
| 1     | Value type                  |
| ...   | ...                         |
| i     | Value type                  |
| i + 1 | Number of registers to skip |
| i + 2 | Value type                  |
| ...   | ...                         |
| n     | Value type                  |

##### Group Data Types
| Byte | Value                                              |
| ----- | ------------------------------------------------- |
| 0     | Data type                                         |
| 1     | Index of value type where data type is last valid |
| ...   | ...                                               |
| n - 1 | Data type                                         |
| n     | 255                                               |

The span of registers is the total length needed in a Modbus request to obtain all the data necessary for the values in the group.  This is the limiting factor in how large a group can be.  In the group listing, values are listed in the order found in the meter's Modbus map.  Definitions are in the [appendix](#desired-meter-values). When the values are not consecutive, the number of registers between values is given as a negative number.  

The group data types show first the data type for a given set of values, and then the index of the last value that uses that data type.  The last index is given as 255, since there are no more values to come after.  

The blocks and groups are contained within a unique function for each Modbus map.  At the top of this function there is a header.

##### Modbus Map Function Header
| Byte | Value                          |
| ---- | ------------------------------ |
| 0    | High byte of EEPROM address of the first block |
| 1    | Low byte of EEPROM address of the first block  |
| 2    | Number of blocks    |
| 3    | Number of groups     |
| 4    | High byte of EEPROM address of the first group |
| 5    | Low byte of EEPROM address of the first group  |
| 6    | High byte of EEPROM address of the second group |
| 7    | Low byte of EEPROM address of the second group  |
| 2 * i + 2    | High byte of EEPROM address of the ith group |
| 2 * i + 3    | Low byte of EEPROM address of the ith group  |
| 2 * m + 2    | High byte of EEPROM address of the last group |
| 2 * m + 3    | Low byte of EEPROM address of the last group  |

The functions are all called in  `writeLibrary.ino`.  The total number of maps is stored at `reg_strt` in the EEPROM, found at approximately line 10.  The indexing of the maps is as follows:

##### Map Indexing
| Byte | Value                          |
| ---- | ------------------------------ |
| 0    | High byte of EEPROM address of the map |
| 1    | Low byte of EEPROM address of the map  |
| 2    | Map number    |
| 3    | Modbus function (3 or 4)     |

The address of the map is determined by counting the bytes used in the previous map and starting where it left off.

### Appendix
#### FloatConv Enum Class
| Name      | Value |
| --------- | ----- |
| SKIP      | -1    |
| FLOAT     |   0   |
| UINT16    |   1   |
| INT16     |   2   |
| UINT32    |   3   |
| INT32     |   4   |
| MOD1K     |   5   |
| MOD10K    |   6   |
| MOD20K    |   7   |
| MOD30K    |   8   |
| INT64     |   9   |
| UINT64    |  10   |
| ENERGY    |  11   |
| DOUBLE    |  12   |
| FLOAT_WS  |  64   |
| UINT16_WS | none  |
| INT16_WS  | none  |
| UINT32_WS |  67   |
| INT32_WS  |  68   |
| MOD1K_WS  |  69   |
| MOD10K_WS |  70   |
| MOD20K_WS |  71   |
| MOD30K_WS |  72   |
| INT64_WS  |  73   |
| UINT64_WS |  74   |
| ENERGY_WS |  75   |
| DOUBLE_WS |  76   |
  
#### Desired Meter Values
| ID:  | Collection types (elec):    | Collection types (stm/chw):    |
|------|-----------------------------|--------------------------------|
|  1   |   Current, A                |   Heat flow                    |
|  2   |   Current, B                |   Mass flow                    |
|  3   |   Current, C                |   Volumetric flow              |
|  4   |   Current, Average          |   Temperature 1                |
|  5   |   Current, Total            |   Temperature 2                |
|  6   |   Voltage, L-N, A           |   Temperature delta            |
|  7   |   Voltage, L-N, B           |   Pressure                     |
|  8   |   Voltage, L-N, C           |   Heat total                   |
|  9   |   Voltage, L-N, Average     |   Mass total                   |
|  10  |   Voltage, L-L, A-B         |   Volume total                 |
|  11  |   Voltage, L-L, B-C         |                                |
|  12  |   Voltage, L-L, C-A         |                                |
|  13  |   Voltage, L-L, Average     |                                |
|  14  |   Real Power, A             |                                |
|  15  |   Real Power, B             |                                |
|  16  |   Real Power, C             |                                |
|  17  |   Real Power, Total         |                                |
|  18  |   Reactive Power, A         |                                |
|  19  |   Reactive Power, B         |                                |
|  20  |   Reactive Power, C         |                                |
|  21  |   Reactive Power, Total     |                                |
|  22  |   Apparent Power, A         |                                |
|  23  |   Apparent Power, B         |                                |
|  24  |   Apparent Power, C         |                                |
|  25  |   Apparent Power, Total     |                                |
|  26  |   Power Factor, A           |                                |
|  27  |   Power Factor, B           |                                |
|  28  |   Power Factor, C           |                                |
|  29  |   Power Factor, Total       |                                |
|  30  |   Real Energy, Total        |                                |
|  31  |   Reactive Energy, Total    |                                |
|  32  |   Apparent Energy, Total    |                                |

#### Meter Types
Those that start with a 0 are not currently stored in the EEPROM.  

| Meter                               | Type      |
| ----------------------------------- | --------- |
|  Eaton IQ DP 4000                   |  0.1.1    |
|  Eaton PXM 2260                     |  15.1.0   |
|  Eaton IQ 200                       |  0.1.3    |
|  Eaton IQ 300                       |  0.1.4    |
|  Eaton Power Xpert 4000             |  1.1.0    |
|  Emon Dmon 3400                     |  2.1.0    |
|  GE EPM 3720                        |  3.1.0    |
|  GE EPM 5100                        |  0.2.1    |
|  GE PQM                             |  4.1.0    |
|  Schneider PM5300                   |  16.1.0   |
|  Schneider PM8000                   |  17.1.0   |
|  Siemens 9200                       |  0.3.1    |
|  Siemens 9330                       |  5.1.0    |
|  Siemens 9340                       |  10.8.0   |
|  Siemens 9350                       |  5.2.0    |
|  Siemens 9360                       |  5.3.0    |
|  Siemens 9510                       |  6.1.0    |
|  Siemens 9610                       |  6.2.0    |
|  Siemens 9700                       |  5.4.0    |
|  Siemens Sentron PAC 4200           |  14.1.0   |
|  Siemens Sentron PAC 3200           |  14.2.0   |
|  SquareD CM 2350                    |  7.1.0    |
|  SquareD PM 210                     |  13.1.0   |
|  SquareD PM 710                     |  8.1.0    |
|  SquareD Micrologic A Trip Unit     |  9.1.0    |
|  SquareD Micrologic P Trip Unit     |  9.2.0    |
|  SquareD Micrologic H Trip Unit     |  9.3.0    |
|  SquareD CM 3350                    |  10.1.0   |
|  SquareD CM 4000                    |  10.2.0   |
|  SquareD CM 4250                    |  10.3.0   |
|  SquareD PM 800                     |  10.4.0   |
|  SquareD PM 820                     |  10.5.0   |
|  SquareD PM 850                     |  10.6.0   |
|  SquareD PM 870                     |  10.7.0   |
|  Chilled Water KEP                  |  11.1.0   |
|  Steam KEP                          |  12.1.0   |

[teensy 3.2]: https://www.pjrc.com/store/teensy32.html
[wiz820io]: http://www.wiznet.co.kr/product-item/wiz820io/
[kep]: http://www.kep.com/productPages/flow-instruments/flow-computer.html
[visual studio]: https://www.visualstudio.com/downloads/
[arduino ide]: https://www.arduino.cc/en/Main/Software
[teensyduino]: http://www.pjrc.com/teensy/td_download.html
[visual micro]: http://www.visualmicro.com/page/Arduino-Visual-Studio-Downloads.aspx