#ifndef MeterLibrary_h
#define MeterLibrary_h


/* _____STANDARD INCLUDES____________________________________________________ */
// include types & constants of Wiring core API
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

/* _____UTILITY MACROS_______________________________________________________ */


/* _____PROJECT INCLUDES_____________________________________________________ */
#include <EEPROM.h>

#include <FloatConvEnum.h>

/* _____CLASS DEFINITIONS____________________________________________________ */


/**
Arduino class library for parsing meter library stored in EEPROM for modbus 
gateway.  The meter library contains lists of registers and their data types,
along with selected registers.
*/

class ModbusMaster;

class MeterLibrary {
	private:
		uint16_t m_u16_reqReg;
		uint16_t m_u16_numRegs;
		uint8_t m_u8_mtrType;
		
		uint16_t m_u16_regBlkStart;
		uint16_t m_u16_mtrLibStart;
		uint16_t m_u16_blkStrtInd;
		uint16_t m_u16_grpStrtInd;
		uint8_t m_u8_numBlks;
		uint8_t m_u8_numGrps;
		uint8_t m_u8_curGrp;
		uint8_t m_u8_numGrpVals;
		// uint8_t m_u8_numGrpRegs;
		// uint16_t m_u16_grpReqReg;
		
		bool m_b_allSameType;
		FloatConv m_reqRegDataType;
		
		// uint16_t getNumReqVals();
	public:
		MeterLibrary(uint16_t u16_reqReg, uint16_t u16_numRegs, uint8_t u8_mtrType);
		
		int changeInputs(uint16_t u16_reqReg, uint16_t u16_numRegs, uint8_t u8_mtrType, 
		      bool b_useGrps = false);
		// bool changeMeterType(uint8_t u8_mtrType);
		// void changeRegisters(uint16_t u16_reqReg, uint16_t u16_numRegs);
		
		bool setGroup(uint8_t u8_grpInd);
		
		void convertToFloat(ModbusMaster &node, uint8_t *const u8p_data, bool checkType);
		float convertToFloat(ModbusMaster &node, uint16_t u16_reg, bool checkType);  // returns float
		
};

/* _____FUNCTION DEFINITIONS_________________________________________________ */
uint16_t swapBytes(uint16_t u16_word);
float g_convertToFloat(uint16_t *const u16_reg, FloatConv regDataType);  // returns float

#endif