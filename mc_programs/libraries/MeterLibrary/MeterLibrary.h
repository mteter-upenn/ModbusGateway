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
		// uint16_t m_u16_numRegs;
		uint8_t m_u8_mtrType;
		
		uint16_t m_u16_mtrTypeListingStart;
		uint16_t m_u8_numTypes;
		
		uint16_t m_u16_mtrLibStart;
		uint16_t m_u16_blkStrtInd;
		uint8_t m_u8_numBlks;

		FloatConv m_reqRegDataType;
	public:
		// CONSTRUCTORS
		MeterLibBlocks(uint16_t u16_reqReg, uint8_t u8_mtrType);
		
		//FUNCTIONS
		int changeInputs(uint16_t u16_reqReg, uint8_t u8_mtrType);
		
		// uint16_t getNumRegs();
		uint16_t getReqReg();
		
		// void convertToFloat(ModbusMaster &node, uint8_t *const u8p_data);
		// float convertToFloat(ModbusMaster &node, uint16_t u16_reg);  // returns float
		void convertToFloat(uint16_t u16p_regs[], uint8_t *const u8p_data, uint16_t u16_numRegs);
		float convertToFloat(uint16_t u16p_regs[], uint16_t u16_reg, uint16_t u16_numRegs);  // returns float
		
		bool adjustLength(uint16_t u16_unadjLgth, uint16_t &u16_adjLgth);
};


class MeterLibGroups {
	private:
		uint16_t m_u16_reqReg;
		uint16_t m_u16_numRegs;
		uint8_t m_u8_mtrType;
		
		uint16_t m_u16_mtrTypeListingStart;
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


struct SlaveDataStruct {
	uint8_t u8_id;
	uint8_t u8_vid;
	uint8_t u8a_ip[4];
	uint8_t u8a_type[3];
};

// class used to grab slave meta data (id, vid, ip, type) from eeprom
class SlaveDataClass {
	private:
		uint16_t m_u16_slaveDataStart;
		uint8_t m_u8_numSlaves;
		
		// uint8_t m_u8a_slaveIds[20];      // array of modbus device ids (meters can share these!)
		// uint8_t m_u8a_slaveVids[20];     // array of modbus virtual ids (these should be unique!) they can be the same as devs
		// uint8_t m_u8a_slaveIps[20][4];   // array of slave ips
		// uint8_t m_u8a_slaveTypes[20][3]; // array of slave meter types
		
		SlaveDataStruct m_slaveList[20];
		
		static const SlaveDataStruct mk_sdInvalid;
	public:
		void init();
		
		// INDEX IS ZERO BASED!!!!
		bool getIndByVid(uint8_t u8_vid, uint8_t &u8_ind);  // search for index with vid
	
		// bool getFullTypeByInd(uint8_t u8_slvInd, uint8_t u8a_type[3]);
		// bool getRedTypeByInd(uint8_t u8_slvInd, uint8_t &u8_type);
		// bool getIdByInd(uint8_t u8_slvInd, uint8_t &u8_id);
		// bool getVidByInd(uint8_t u8_slvInd, uint8_t &u8_vid);
		// bool getIPByInd(uint8_t u8_slvInd, uint8_t u8a_ip[4]);
		
		bool isSlaveTcpByInd(uint8_t u8_slvInd);
		
		SlaveDataStruct operator[](int index) const;
};

extern SlaveDataClass SlaveData;

/* _____FUNCTION DEFINITIONS_________________________________________________ */
uint16_t swapBytes(uint16_t u16_word);
float g_convertToFloat(const uint16_t *const u16_reg, FloatConv regDataType);  // returns float
// float g_convertToFloat(const uint8_t *const u8_reg, FloatConv regDataType);  // returns float
#endif