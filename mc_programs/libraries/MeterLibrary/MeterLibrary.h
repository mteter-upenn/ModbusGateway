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

#include <ModbusStructs.h>

/* _____CLASS DEFINITIONS____________________________________________________ */


/**
Arduino class library for parsing meter library stored in EEPROM for modbus 
gateway.  The meter library contains lists of registers and their data types,
along with selected registers.
*/

// class ModbusMaster;

class MeterLibBlocks {
	private:
		uint16_t m_u16_reqReg;
		uint16_t m_u16_numRegs;
		uint8_t m_u8_mtrType;
		
		uint16_t m_u16_mtrListingStart;
		uint16_t m_u16_mtrLibStart;
		uint16_t m_u16_blkStrtInd;
		uint8_t m_u8_numBlks;

		FloatConv m_reqRegDataType;
	public:
		// CONSTRUCTORS
		MeterLibBlocks(uint16_t u16_reqReg, uint16_t u16_numRegs, uint8_t u8_mtrType);
		
		//FUNCTIONS
		int changeInputs(uint16_t u16_reqReg, uint16_t u16_numRegs, uint8_t u8_mtrType);
		
		uint16_t getNumRegs();
		uint16_t getReqReg();
		
		// void convertToFloat(ModbusMaster &node, uint8_t *const u8p_data);
		// float convertToFloat(ModbusMaster &node, uint16_t u16_reg);  // returns float
		
};


class MeterLibGroups {
	private:
		uint16_t m_u16_reqReg;
		uint16_t m_u16_numRegs;
		uint8_t m_u8_mtrType;
		
		uint16_t m_u16_mtrListingStart;
		uint16_t m_u16_mtrLibStart;
		uint16_t m_u16_grpStrtInd;
		uint8_t m_u8_numGrps;
		uint8_t m_u8_curGrp;
		uint8_t m_u8_numGrpVals;
		uint16_t m_u16_grpDataTypeInd;

		FloatConv m_reqRegDataType;
	public:
		// CONSTRUCTORS
		MeterLibGroups(uint8_t u8_mtrType);
		
		//FUNCTIONS
		bool setMeterType(uint8_t u8_mtrType);
		bool setGroup(uint8_t u8_grpInd);
		
		uint16_t getNumRegs();
		uint16_t getReqReg();
		uint8_t getCurGrp();
		uint8_t getNumGrps();
		
		bool groupToFloat(const uint8_t *const u8p_data, float *const fp_retData, int8_t *const s8kp_dataFlags);
		bool groupMbErr(int8_t *const s8kp_dataFlags);
		bool groupLastFlags(int8_t *const s8kp_dataFlags);		
};

/* _____FUNCTION DEFINITIONS_________________________________________________ */
uint16_t swapBytes(uint16_t u16_word);
float g_convertToFloat(const uint16_t *const u16_reg, FloatConv regDataType);  // returns float
// float g_convertToFloat(const uint8_t *const u8_reg, FloatConv regDataType);  // returns float
#endif