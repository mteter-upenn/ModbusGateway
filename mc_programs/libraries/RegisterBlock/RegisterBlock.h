



#ifndef RegisterBlock_h
#define RegisterBlock_h


/**
@def __REGISTERBLOCK_DEBUG__ (1).
Set to 1 to enable debugging features within class:

*/
#define __MREGISTERBLOCK_DEBUG__ (0)

/* _____STANDARD INCLUDES____________________________________________________ */
// include types & constants of Wiring core API
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

/* _____UTILITY MACROS_______________________________________________________ */


/* _____PROJECT INCLUDES_____________________________________________________ */

/* _____CLASS DEFINITIONS____________________________________________________ */
/**
Arduino class library for communicating with Modbus slaves over 
RS232/485 (via RTU protocol).
*/

struct reg_struct
{
	uint16_t *u16regs;
	uint8_t u8func;
	uint16_t u16strt;
	uint16_t u16lgth;
};
	
class RegisterBlock
{
  public:
	RegisterBlock(int16_t*, uint16_t*, uint8_t, uint16_t, uint16_t);
    RegisterBlock(int16_t*, uint16_t*, uint8_t, uint16_t, uint16_t, uint8_t);
	RegisterBlock(uint16_t*, uint8_t, uint16_t, uint16_t);
    RegisterBlock(uint16_t*, uint8_t, uint16_t, uint16_t, uint8_t);
	RegisterBlock(int32_t*, uint16_t*, uint8_t, uint16_t, uint16_t);
    RegisterBlock(int32_t*, uint16_t*, uint8_t, uint16_t, uint16_t, uint8_t);
	RegisterBlock(uint32_t*, uint16_t*, uint8_t, uint16_t, uint16_t);
    RegisterBlock(uint32_t*, uint16_t*, uint8_t, uint16_t, uint16_t, uint8_t);
	RegisterBlock(float*, uint16_t*, uint8_t, uint16_t, uint16_t);
    RegisterBlock(float*, uint16_t*, uint8_t, uint16_t, uint16_t, uint8_t);
	
	// struct reg_struct
	// {
		// uint16_t *u16regs;
		// uint8_t u8func;
		// uint16_t u16strt;
		// uint16_t u16lgth;
	// };
	
	reg_struct block; // define necessary bit

  private:
	void _byteSwap(void);
	void _wordSwap(void);
	
	
};
#endif
	