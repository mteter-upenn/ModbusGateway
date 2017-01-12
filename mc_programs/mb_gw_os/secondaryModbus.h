#ifndef SECONDARYMODBUS_H
#define SECONDARYMODBUS_H

#include <ModbusStructs.h>

bool findRegister(uint16_t u16_reqRegister, FloatConv &fltConv, uint8_t u8_meterType);

#endif // SECONDARYMODBUS_H
