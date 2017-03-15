#ifndef MeterLibrary_h
#define MeterLibrary_h


/* _____STANDARD INCLUDES____________________________________________________ */
// include types & constants of Wiring core API
#include "Arduino.h"

/* _____UTILITY MACROS_______________________________________________________ */


/* _____PROJECT INCLUDES_____________________________________________________ */
#include <EEPROM.h>
#include <ModbusStructs.h>
#include <ArduinoJson.h>

/* _____CLASS DEFINITIONS____________________________________________________ */


/**
Arduino class library for parsing meter library stored in EEPROM for modbus 
gateway.  The meter library contains lists of registers and their data types,
along with selected registers.
*/

// class ModbusMaster;
struct MapBlock {
  uint16_t u16_start;
  uint16_t u16_end;
  FloatConv dataType;
};

struct MapGroup {
  uint16_t u16_start;
  uint8_t u8_vals;
  uint8_t u8_regs;
  int8_t s8a_grpOrder[63];
  uint8_t u8_orderLen;
  int8_t s8a_grpType[64];
  uint8_t u8_typeLen;
};


class MeterLibBlocks {
	private:
		uint16_t m_u16_reqReg;
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
		
		uint16_t getReqReg();
		
		void convertToFloat(uint16_t u16p_regs[], uint8_t *const u8p_data, uint16_t u16_numRegs);
		float convertToFloat(uint16_t u16p_regs[], uint16_t u16_reg, uint16_t u16_numRegs);  // returns float
		
		bool adjFloatRegsToActualRegs(uint16_t u16_unadjLgth, uint16_t &u16_adjLgth);
		bool adjActualRegsToFloatRegs(uint16_t u16_unadjLgth, uint16_t &u16_adjLgth);
};


class MeterLibGroups {
	private:
		uint16_t m_u16_reqReg;
		uint16_t m_u16_numRegs;
		uint8_t m_u8_mtrType;  // THIS IS 1 BASED!
		uint8_t m_u8_func;
		
		uint16_t m_u16_mtrTypeListingStart;
		uint16_t m_u16_mtrLibStart;
		uint16_t m_u16_grpStrtInd;
		uint8_t m_u8_numGrps;
		uint8_t m_u8_curGrp;  // THIS IS 1 BASED!
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
		
		ModbusRequest getGroupRequest(bool b_serialComm, uint8_t u8_mbId, uint8_t mbVid);
		
		bool groupToFloat(const uint16_t * k_u16p_data, float *const fkp_retData, int8_t *const s8kp_dataFlags);
		bool groupMbErr(int8_t *const s8kp_dataFlags);
		bool groupLastFlags(int8_t *const s8kp_dataFlags);		
};


class WriteMapsClass {
private:
  uint8_t m_u8_numMaps;
  uint16_t m_u16_mapIndexStart;

  uint16_t calcStartingPos(uint8_t u8_map);
public:
  WriteMapsClass();

  uint16_t writeMaps(JsonObject& root);
  uint16_t addMap(uint8_t u8_map, MapBlock mapBlkArr[], MapGroup mapGrpArr[], uint8_t u8_numBlks, uint8_t u8_numGrps, uint8_t u8_mbFunc);
};


// class used to grab slave meta data (id, vid, ip, type) from eeprom
class SlaveDataClass {
	private:
		uint16_t m_u16_slaveDataStart;
		uint8_t m_u8_numSlaves;
		
    SlaveArray m_slaveList[20];
		
    static const SlaveArray mk_sdInvalid;
	public:
		void init();
		
		// INDEX IS ZERO BASED!!!!
		bool getIndByVid(uint8_t u8_vid, uint8_t &u8_ind);  // search for index with vid		
		bool isSlaveTcpByInd(uint8_t u8_slvInd);
		uint8_t getNumSlvs();
    SlaveArray operator[](int index) const;
};

extern SlaveDataClass SlaveData;

/* _____FUNCTION DEFINITIONS_________________________________________________ */
uint16_t swapBytes(uint16_t u16_word);
float g_convertToFloat(const uint16_t *const u16_reg, FloatConv regDataType);  // returns float
#endif
