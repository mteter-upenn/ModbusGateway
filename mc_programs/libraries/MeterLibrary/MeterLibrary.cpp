/* _____PROJECT INCLUDES_____________________________________________________ */
#include "MeterLibrary.h"

/* _____GLOBAL VARIABLES_____________________________________________________ */



/*______STATIC VARIABLES_____________________________________________________ */



/* _____PUBLIC FUNCTIONS_____________________________________________________ */

// MeterLibBlocks ##################################################################################

/**
Constructor.

@ingroup setup
*/
MeterLibBlocks::MeterLibBlocks(uint16_t u16_reqReg, uint8_t u8_mtrType) {  // , uint16_t u16_numRegs
	// address for meter register library, this will never change
  EEPROM.get(6, m_u16_mtrTypeListingStart);
  EEPROM.get(m_u16_mtrTypeListingStart + 2, m_u8_numTypes);
	
	changeInputs(u16_reqReg, u8_mtrType);
}


int MeterLibBlocks::changeInputs(uint16_t u16_reqReg, uint8_t u8_mtrType) {  // , uint16_t u16_numRegs
	m_u16_reqReg = u16_reqReg;
	
	if ((u8_mtrType <= m_u8_numTypes) || (u8_mtrType != 0)) {
		m_u8_mtrType = u8_mtrType;
	}
	else {
		m_u8_mtrType = 0;
		m_u16_reqReg = 0xFFFF;
		return 1;  // meter type not viable
	}
	
  EEPROM.get(m_u16_mtrTypeListingStart + 4 * u8_mtrType - 1, m_u16_mtrLibStart);
  EEPROM.get(m_u16_mtrLibStart, m_u16_blkStrtInd);
  EEPROM.get(m_u16_mtrLibStart + 2, m_u8_numBlks);
	m_reqRegDataType = FloatConv::FLOAT;
	
	for (int ii = 0; ii < m_u8_numBlks; ++ii) {
		uint16_t u16_blkFirstReg;
		uint16_t u16_blkLastReg;
	
    EEPROM.get(5 * ii + m_u16_blkStrtInd, u16_blkFirstReg);
    EEPROM.get(5 * ii + m_u16_blkStrtInd + 2, u16_blkLastReg);


		if ((m_u16_reqReg >= u16_blkFirstReg) && (m_u16_reqReg <= u16_blkLastReg)) {
      EEPROM.get(5 * ii + m_u16_blkStrtInd + 4, m_reqRegDataType);
			return 0;
		} 
	} // end for
	
	m_u16_reqReg = 0xFFFF;
	return 2;  // could not find register in blocks listed
}


uint16_t MeterLibBlocks::getReqReg() {
	return m_u16_reqReg;
}


/**
Convert full data request to float.

u8a_data must be address where floats will start (no header spacing accounted for)
*/
void MeterLibBlocks::convertToFloat(uint16_t u16p_regs[], uint8_t *const u8kp_data, uint16_t u16_numRegs) {
	uint16_t u16_numReqVals;
	// multiplier such that u16_regMult * (number of requested values) = (number of requested 
	//     registers)
	uint16_t u16_regMult;  
	uint16_t * const u16kp_data = (uint16_t*)u8kp_data;
	uint16_t u16_reg;
	union cnvtUnion {
		uint16_t u16[2];
		float fl;
	} int2flt;

	u16_regMult = FloatConvEnumNumRegs(m_reqRegDataType);
	u16_numReqVals = u16_numRegs / u16_regMult;
	
	for (int ii = 0, jj = 0; ii < u16_numReqVals; ++ii, jj += 2) {
		u16_reg = m_u16_reqReg + ii * u16_regMult;
		
		int2flt.fl = convertToFloat(u16p_regs, u16_reg, u16_numRegs);
		
		// words are in correct order, but the bytes in the words need to be swapped
		u16kp_data[jj] = swapBytes(int2flt.u16[0]);
		u16kp_data[jj + 1] = swapBytes(int2flt.u16[1]);
	}
}


float MeterLibBlocks::convertToFloat(uint16_t u16p_regs[], uint16_t u16_reg, uint16_t u16_numRegs) {
	uint8_t u8_regInd;
	
	// check if register is within requested data range
	if ((u16_reg >= m_u16_reqReg) && (u16_reg < (m_u16_reqReg + u16_numRegs))) {
		u8_regInd = u16_reg - m_u16_reqReg;
	}
	else {
		return 0.0f;
	}
	return g_convertToFloat(&(u16p_regs[u8_regInd]), m_reqRegDataType);
}


bool MeterLibBlocks::adjFloatRegsToActualRegs(uint16_t u16_unadjLgth, uint16_t &u16_adjLgth) {
	if ((m_u8_mtrType > m_u8_numTypes) || (m_u8_mtrType == 0)) {
		return false;  // no type listed in eeprom- can't adjust length
	}
	
	switch (m_reqRegDataType) {
		case FloatConv::UINT16:  // u16
		case FloatConv::INT16:  // s16
			u16_adjLgth = u16_unadjLgth / 2;  // single register values
			break;
		case FloatConv::MOD20K:  // m20k
		case FloatConv::MOD20K_WS:  // m20k
			u16_adjLgth = u16_unadjLgth * 3 / 2;  // 3 register values
			break;
		case FloatConv::MOD30K:  // m30k
		case FloatConv::MOD30K_WS:  // m30k
		case FloatConv::INT64:  // s64
		case FloatConv::INT64_WS:  // s64
		case FloatConv::UINT64:  // u64
		case FloatConv::UINT64_WS:  // u64
		case FloatConv::ENERGY:  // engy
		case FloatConv::ENERGY_WS:  // engy
		case FloatConv::DOUBLE:  // dbl
		case FloatConv::DOUBLE_WS:  // dbl
			u16_adjLgth = u16_unadjLgth * 2;  // 4 register values
			break;
		default:  // float, u32, s32, m10k, m1k
			u16_adjLgth = u16_unadjLgth;  // 2 register values
			break;              
	}
	
	return true;
}


bool MeterLibBlocks::adjActualRegsToFloatRegs(uint16_t u16_unadjLgth, uint16_t &u16_adjLgth) {
	if ((m_u8_mtrType > m_u8_numTypes) || (m_u8_mtrType == 0)) {
		return false;  // no type listed in eeprom- can't adjust length
	}
	
	switch (m_reqRegDataType) {
		case FloatConv::UINT16:  // u16
		case FloatConv::INT16:  // s16
			u16_adjLgth = u16_unadjLgth * 2;  // single register values
			break;
		case FloatConv::MOD20K:  // m20k
		case FloatConv::MOD20K_WS:  // m20k
			u16_adjLgth = u16_unadjLgth * 2 / 3;  // 3 register values
			break;
		case FloatConv::MOD30K:  // m30k
		case FloatConv::MOD30K_WS:  // m30k
		case FloatConv::INT64:  // s64
		case FloatConv::INT64_WS:  // s64
		case FloatConv::UINT64:  // u64
		case FloatConv::UINT64_WS:  // u64
		case FloatConv::ENERGY:  // engy
		case FloatConv::ENERGY_WS:  // engy
		case FloatConv::DOUBLE:  // dbl
		case FloatConv::DOUBLE_WS:  // dbl
			u16_adjLgth = u16_unadjLgth / 2;  // 4 register values
			break;
		default:  // float, u32, s32, m10k, m1k
			u16_adjLgth = u16_unadjLgth;  // 2 register values
			break;              
	}
	
	return true;
}


// MeterLibGroups ##################################################################################

/**
Constructor.

@ingroup setup
*/
MeterLibGroups::MeterLibGroups(uint8_t u8_mtrType) {
	// address for meter register library, this will never change
  EEPROM.get(6, m_u16_mtrTypeListingStart);
		
	setMeterType(u8_mtrType);
}

bool MeterLibGroups::setMeterType(uint8_t u8_mtrType) {  // 1 based
  uint8_t u8_numMaps;
  EEPROM.get(m_u16_mtrTypeListingStart + 2, u8_numMaps);

  if ((u8_mtrType <= u8_numMaps) && (u8_mtrType > 0)) {
		m_u8_mtrType = u8_mtrType;
	}
	else {
		m_u8_mtrType = 1;
		m_u16_reqReg = 0xFFFF;
		return false;  // meter type not viable
	}
	
  EEPROM.get(m_u16_mtrTypeListingStart + 4 * u8_mtrType - 1, m_u16_mtrLibStart);

	// assume group is 1:
  EEPROM.get(m_u16_mtrLibStart + 3, m_u8_numGrps);
  EEPROM.get(m_u16_mtrTypeListingStart + 4 * u8_mtrType + 2, m_u8_func);
	
	return setGroup(1);
}



bool MeterLibGroups::setGroup(uint8_t u8_grpInd) {  // u8_grpInd is 1 based!!
	if (m_u8_mtrType != 0) {
		if (u8_grpInd <= m_u8_numGrps) {
			m_u8_curGrp = u8_grpInd;

      EEPROM.get(m_u16_mtrLibStart + 4 + m_u8_curGrp * 2 - 2, m_u16_grpStrtInd);
      EEPROM.get(m_u16_grpStrtInd, m_u8_numGrpVals);

			if (u8_grpInd < m_u8_numGrps) {
        uint8_t u8_grpLenVals;
        uint8_t u8_dum;

        EEPROM.get(m_u16_grpStrtInd + 1, u8_dum);
        m_u16_numRegs = u8_dum;

        EEPROM.get(m_u16_grpStrtInd + 2, m_u16_reqReg);
        EEPROM.get(m_u16_grpStrtInd + 4, u8_grpLenVals);
        m_u16_grpDataTypeInd = m_u16_grpStrtInd + u8_grpLenVals;
			}
			else {  // last group
        uint8_t u8_grpLenVals;
        EEPROM.get(m_u16_grpStrtInd, u8_grpLenVals);

				m_u16_numRegs = 0;
				m_u16_reqReg = 0;
        m_u16_grpDataTypeInd = m_u16_grpStrtInd + u8_grpLenVals + 1;
			}
			
			return true;
		}
	}
	
	return false;
}

uint16_t MeterLibGroups::getNumRegs() {
	return m_u16_numRegs;
}


uint16_t MeterLibGroups::getReqReg() {
	return m_u16_reqReg;
}

uint8_t MeterLibGroups::getCurGrp() {
	return m_u8_curGrp;
}

uint8_t MeterLibGroups::getNumGrps() {
	return m_u8_numGrps;
}


ModbusRequest MeterLibGroups::getGroupRequest(bool b_tcpComm, uint8_t u8_mbId, uint8_t u8_mbVid) {
	ModbusRequest mbReq;
	
	mbReq.u8_flags = 0;  // might be problems with initialization
	// mbReq.u8_flags = MRFLAG_adjReq;  // remove this to stop double reversal
	if (b_tcpComm) {
		mbReq.u8_flags |= MRFLAG_isTcp;
	}
	mbReq.u8_id = u8_mbId;
	mbReq.u8_vid = u8_mbVid;
	mbReq.u8_func = m_u8_func;
	mbReq.u16_start = m_u16_reqReg;
	mbReq.u16_length = m_u16_numRegs;
	mbReq.u8_mtrType = m_u8_mtrType;
	
	
	return mbReq;	
}


// bool MeterLibGroups::groupToFloat(const uint8_t *const k_u8kp_data, float *const fkp_retData, 
bool MeterLibGroups::groupToFloat(const uint16_t * k_u16p_data, float *const fkp_retData, 
	int8_t *const s8kp_dataFlags) {
	
	if (m_u8_mtrType == 0) {
		return false;
	}
	
	if (!(m_u8_curGrp < m_u8_numGrps)) {  // check to make sure curGrp is not last group
		return false;
	}

	/* GROUP STRUCTURE:
	*  ADDRESS   0: number of values in group
	*  ADDRESS   1: number of registers to request in modbus
	*  ADDRESS   2: high byte of starting register
	*  ADDRESS   3: low byte of starting register
	*  ADDRESS   4: add this to ADDRESS 0 to get index of data types
	*  ADDRESS 5->: value types + skips, value types are [1, 32], skips are negative
	*  ADDRESS  (ADDRESS 0 + eval(ADDRESS 4)): data types in pairs (FloatConv, which values fall under it)
	*/

  uint8_t u8_dataTypeCmp;
  FloatConv dataType;
	uint8_t u8_valInd(0);  // order of value in group (NOT the VALUE TYPE (ie real power))
	int ii(2); // skip 0 on assumption that u8_dataTypeCmp is initialized in such a case

  EEPROM.get(m_u16_grpDataTypeInd + 1, u8_dataTypeCmp);
  EEPROM.get(m_u16_grpDataTypeInd, dataType);

	for (uint16_t u16_valEepAdr = m_u16_grpStrtInd + 5; u16_valEepAdr < m_u16_grpDataTypeInd; ++u16_valEepAdr) {
		int8_t s8_valType;  // if > 0, then this is the value Type (Power, current A, etc)
		                    // if < 0, then this is the number of registers to skip to the next value
		
    EEPROM.get(u16_valEepAdr, s8_valType);
	
		if (s8_valType < 0) {
			// skip registers
			k_u16p_data += (-1) * s8_valType;
		}
		else {
			++u8_valInd;
			
      while (u8_valInd > u8_dataTypeCmp) {
        EEPROM.get(m_u16_grpDataTypeInd + 1 + ii, u8_dataTypeCmp);
        EEPROM.get(m_u16_grpDataTypeInd + ii, dataType);
				ii += 2;
			}
			
			// convert register data to float and store
			fkp_retData[s8_valType - 1] = g_convertToFloat(k_u16p_data, dataType);

			// mark all flags as a successful read
			if (isnan(fkp_retData[s8_valType - 1])) {
				s8kp_dataFlags[s8_valType - 1] = -2;
			}
			else if (isinf(fkp_retData[s8_valType - 1])) {
				s8kp_dataFlags[s8_valType - 1] = -3;
			}
			else {
				s8kp_dataFlags[s8_valType - 1] = 1;
			}
			// skip necessary number of registers to get to next value
			k_u16p_data += FloatConvEnumNumRegs(dataType);
		}
	}
	
	return false;
}

// set all flags for the values in the group to -1 for unsuccessful modbus read
bool MeterLibGroups::groupMbErr(int8_t *const s8kp_dataFlags) {
	if (m_u8_mtrType == 0) {
		return false;
	}
	
	if (!(m_u8_curGrp < m_u8_numGrps)) {  // check to make sure curGrp is not last group
		return false;
	}
	Serial.print("mb err cur grp: "); Serial.println(m_u8_curGrp, DEC);
	for (uint16_t u16_valEepAdr = m_u16_grpStrtInd + 5; u16_valEepAdr < m_u16_grpDataTypeInd; ++u16_valEepAdr) {
		int8_t s8_valType;
		
    EEPROM.get(u16_valEepAdr, s8_valType);
		
		if (s8_valType > 0) {
			// mark all flags as an unsuccessful read
			s8kp_dataFlags[s8_valType - 1] = -1;
			
		}
	}
	
	return true;
}


bool MeterLibGroups::groupLastFlags(int8_t *const s8kp_dataFlags) {
	if (m_u8_mtrType == 0) {
		return false;
	}
	
	if (m_u8_curGrp != m_u8_numGrps) {  // check to make sure curGrp is last group
		return false;
	}
	
	for (uint16_t u16_valEepAdr = m_u16_grpStrtInd + 1; u16_valEepAdr < m_u16_grpDataTypeInd; ++u16_valEepAdr) {
		int8_t s8_valType;
		
    EEPROM.get(u16_valEepAdr, s8_valType);
		
		if (s8_valType > 0) {
			// mark all flags as not applicable
			s8kp_dataFlags[s8_valType - 1] = 0;
		}
	}
	
	return true;
}


WriteMapsClass::WriteMapsClass(void) {
  EEPROM.get(6, m_u16_mapIndexStart);
  EEPROM.get(m_u16_mapIndexStart + 2, m_u8_numMaps);
}


uint16_t WriteMapsClass::writeMaps(JsonObject &root) {
  uint16_t u16_mapLibEnd = 0;
  // reset total num of maps
  if (!root.success()) {
    Serial.println("Failed to parse maplist.jsn!");
    return 0;
  }

  Serial.println();
  Serial.print("Index Start: "); Serial.println(m_u16_mapIndexStart);
  Serial.print("eeprom nummaps: "); Serial.println(m_u8_numMaps, DEC);

  m_u8_numMaps = root["meterlist"].size();

  Serial.print("json nummaps: "); Serial.println(m_u8_numMaps, DEC);

  EEPROM.put(m_u16_mapIndexStart + 2, m_u8_numMaps);

  Serial.print("eeprom new nummaps: "); Serial.println(EEPROM.read(m_u16_mapIndexStart + 2), DEC);
  Serial.println();

  uint8_t u8_numBlks;
  uint8_t u8_numGrps;
  uint8_t u8_mbFunc;

  for (int ii = 0; ii < m_u8_numMaps; ++ii) {
    u8_numBlks = root["meterlist"][ii]["blocks"].size();
    u8_numGrps = root["meterlist"][ii]["groups"].size();
    u8_mbFunc = root["meterlist"][ii]["mb_func"];

    MapBlock mapBlkArr[u8_numBlks];
    MapGroup mapGrpArr[u8_numGrps];

    for (int jj = 0; jj < u8_numBlks; ++jj) {
      mapBlkArr[jj].u16_start = root["meterlist"][ii]["blocks"][jj]["start"];
      mapBlkArr[jj].u16_end = root["meterlist"][ii]["blocks"][jj]["end"];
      const char *k_cp_dataType = root["meterlist"][ii]["blocks"][jj]["type"];
      mapBlkArr[jj].dataType = Char_2_FloatConv(k_cp_dataType);
    }

    for (int jj = 0; jj < u8_numGrps - 1; ++jj) {
      mapGrpArr[jj].u8_vals = root["meterlist"][ii]["groups"][jj]["values"];
      mapGrpArr[jj].u8_regs = root["meterlist"][ii]["groups"][jj]["registers"];
      mapGrpArr[jj].u16_start = root["meterlist"][ii]["groups"][jj]["start"];
      mapGrpArr[jj].u8_orderLen = root["meterlist"][ii]["groups"][jj]["order"].size();
      mapGrpArr[jj].u8_typeLen = root["meterlist"][ii]["groups"][jj]["types"].size();

      for (int kk = 0; kk < mapGrpArr[jj].u8_orderLen; ++kk) {
        mapGrpArr[jj].s8a_grpOrder[kk] = root["meterlist"][ii]["groups"][jj]["order"][kk];
      }
      for (int kk = 0; kk < mapGrpArr[jj].u8_typeLen; ++kk) {
        const char *k_cp_dataType = root["meterlist"][ii]["groups"][jj]["types"][kk]["type"];
        mapGrpArr[jj].s8a_grpType[2 * kk] = FloatConv2Int8(Char_2_FloatConv(k_cp_dataType));
        mapGrpArr[jj].s8a_grpType[2 * kk + 1] = root["meterlist"][ii]["groups"][jj]["types"][kk]["valid_to"];
      }
    }
    // LAST GROUP!
    mapGrpArr[u8_numGrps - 1].u8_vals = root["meterlist"][ii]["groups"][u8_numGrps - 1]["values"];
    mapGrpArr[u8_numGrps - 1].u8_orderLen = root["meterlist"][ii]["groups"][u8_numGrps - 1]["order"].size();
    for (int kk = 0; kk < mapGrpArr[u8_numGrps - 1].u8_orderLen; ++kk) {
      mapGrpArr[u8_numGrps - 1].s8a_grpOrder[kk] = root["meterlist"][ii]["groups"][u8_numGrps - 1]["order"][kk];
    }

    u16_mapLibEnd = addMap(ii, mapBlkArr, mapGrpArr, u8_numBlks, u8_numGrps, u8_mbFunc);
  }
  return u16_mapLibEnd;
}


uint16_t WriteMapsClass::addMap(uint8_t u8_map, MapBlock mapBlkArr[], MapGroup mapGrpArr[], uint8_t u8_numBlks, uint8_t u8_numGrps, uint8_t u8_mbFunc) {

  uint16_t u16_mapStart = calcStartingPos(u8_map);

  Serial.println();
  Serial.print("map: "); Serial.println(u8_map, DEC);
  Serial.print("u16_mapStart: "); Serial.println(u16_mapStart);
  Serial.println();

  uint16_t u16_mapIdx = u16_mapStart;

  EEPROM.put(m_u16_mapIndexStart + u8_map * 4 + 3, u16_mapStart);
  EEPROM.put(m_u16_mapIndexStart + u8_map * 4 + 5, static_cast<uint8_t>(u8_map + 1));
  EEPROM.put(m_u16_mapIndexStart + u8_map * 4 + 6, u8_mbFunc);

  uint16_t u16_blkStart = u16_mapStart + 4 + u8_numGrps * 2;
  uint16_t u16_grpStart = u16_blkStart + u8_numBlks * 5;

  Serial.println();
  Serial.print("map: "); Serial.println(u8_map, DEC);
  Serial.print("u16_mapIdx: "); Serial.println(u16_mapIdx);
  Serial.println();

  EEPROM.put(u16_mapIdx, u16_blkStart);
  u16_mapIdx += 2;

  EEPROM.put(u16_mapIdx, u8_numBlks);
  u16_mapIdx += 1;
  EEPROM.put(u16_mapIdx, u8_numGrps);
  u16_mapIdx += 1;

  // group starting addresses
  EEPROM.put(u16_mapIdx, u16_grpStart);
  u16_mapIdx += 2;

  for (int ii = 0; ii < u8_numGrps - 1; ++ii) {
    uint16_t u16_prevGrpStrt;
    EEPROM.get(u16_mapIdx - 2, u16_prevGrpStrt);

    EEPROM.put(u16_mapIdx, static_cast<uint16_t>(u16_prevGrpStrt + mapGrpArr[ii].u8_orderLen + mapGrpArr[ii].u8_typeLen * 2 + 5));
    u16_mapIdx += 2;
  }


  // blocks
  for (int ii = 0; ii < u8_numBlks; ++ii) {
    Serial.print("write block: "); Serial.println(ii + 1);
    Serial.print("blk start: "); Serial.println(u16_mapIdx);

    EEPROM.put(u16_mapIdx, mapBlkArr[ii].u16_start);
    u16_mapIdx += 2;
    EEPROM.put(u16_mapIdx, mapBlkArr[ii].u16_end);
    u16_mapIdx += 2;
    EEPROM.put(u16_mapIdx, FloatConv2Uint8(mapBlkArr[ii].dataType));
    u16_mapIdx += 1;
  }

  Serial.println();

  // groups
  for (int ii = 0; ii < u8_numGrps - 1; ++ii) {
    Serial.print("write group: "); Serial.println(ii + 1);
    Serial.print("group start: "); Serial.println(u16_mapIdx + 1);
    Serial.print("order len: "); Serial.println(mapGrpArr[ii].u8_orderLen, DEC);
    Serial.print("type len: "); Serial.println(mapGrpArr[ii].u8_typeLen, DEC);

    EEPROM.put(u16_mapIdx, mapGrpArr[ii].u8_vals);
    u16_mapIdx += 1;
    EEPROM.put(u16_mapIdx, mapGrpArr[ii].u8_regs);
    u16_mapIdx += 1;
    EEPROM.put(u16_mapIdx, mapGrpArr[ii].u16_start);
    u16_mapIdx += 2;
    EEPROM.put(u16_mapIdx, static_cast<uint8_t>(mapGrpArr[ii].u8_orderLen + 5));
    u16_mapIdx += 1;

    for (int jj = 0; jj < mapGrpArr[ii].u8_orderLen; ++jj) {
      EEPROM.put(u16_mapIdx, mapGrpArr[ii].s8a_grpOrder[jj]);
      u16_mapIdx += 1;
    }
    for (int jj = 0; jj < mapGrpArr[ii].u8_typeLen * 2; ++jj) {
      EEPROM.put(u16_mapIdx, mapGrpArr[ii].s8a_grpType[jj]);
      u16_mapIdx += 1;
    }
  }
  // LAST GROUP!
  EEPROM.put(u16_mapIdx, mapGrpArr[u8_numGrps - 1].u8_vals);
  u16_mapIdx += 1;
  Serial.println();
  Serial.print("map: "); Serial.println(u8_map, DEC);
  Serial.print("last group start: "); Serial.println(u16_mapIdx - 1);
  Serial.print("last group vals: "); Serial.println(EEPROM.read(u16_mapIdx - 1), DEC);
  Serial.println();

  for (int jj = 0; jj < mapGrpArr[u8_numGrps - 1].u8_orderLen; ++jj) {
    EEPROM.put(u16_mapIdx, static_cast<uint8_t>(mapGrpArr[u8_numGrps - 1].s8a_grpOrder[jj]));
    u16_mapIdx += 1;
  }

  return u16_mapIdx - 1;
}


uint16_t WriteMapsClass::calcStartingPos(uint8_t u8_map) {
// where should map u8_map start?
  if (u8_map == 0) {
    return m_u8_numMaps * 4 + m_u16_mapIndexStart + 3;
  }
  else {
    uint16_t u16_prevStart;
    uint8_t u8_prevNumGrps;
    uint16_t u16_lastGrpStart;
    uint8_t u8_lastGrpVals;

    EEPROM.get(m_u16_mapIndexStart + (u8_map - 1) * 4 + 3, u16_prevStart);
    EEPROM.get(u16_prevStart + 3, u8_prevNumGrps);
    EEPROM.get(u16_prevStart + u8_prevNumGrps * 2 + 2, u16_lastGrpStart);
    EEPROM.get(u16_lastGrpStart, u8_lastGrpVals);

    Serial.print("last group start: "); Serial.println(u16_lastGrpStart);
    Serial.print("num vals last grp: "); Serial.println(u8_lastGrpVals, DEC);
    Serial.println();

    return u16_lastGrpStart + u8_lastGrpVals + 1;
  }
}


// SlaveData Functions##############################################################################
SlaveDataClass SlaveData;
const SlaveArray SlaveDataClass::mk_sdInvalid = {{0, 0, 0}, {0, 0, 0, 0}, 0, 0};

void SlaveDataClass::init() {
  EEPROM.get(4, m_u16_slaveDataStart);
  EEPROM.get(m_u16_slaveDataStart, m_u8_numSlaves);
	
	for (int ii = 0; ii < m_u8_numSlaves; ++ii) {
    EEPROM.get(9 * ii + m_u16_slaveDataStart + 1, m_slaveList[ii]);
  }
}


bool SlaveDataClass::getIndByVid(uint8_t u8_vid, uint8_t &u8_ind) {
	for (int ii = 0; ii < m_u8_numSlaves; ++ii) {
		if (m_slaveList[ii].u8_vid == u8_vid) {
			u8_ind = ii;
			return true;
		}
	}
	
	return false;
}


bool SlaveDataClass::isSlaveTcpByInd(uint8_t u8_slvInd) {
	for (int ii = 0; ii < 4; ++ii) {
		if (m_slaveList[u8_slvInd].u8a_ip[ii] != 0) {
			return true;
		}
	}
	return false;
}


uint8_t SlaveDataClass::getNumSlvs() {
	return m_u8_numSlaves;
}


SlaveArray SlaveDataClass::operator[](int index) const {
	if (!(index < m_u8_numSlaves)) {
		return mk_sdInvalid;
	}
	
	return m_slaveList[index];
}

// General Functions ###############################################################################



uint16_t swapBytes(uint16_t u16_word) {
	return ((u16_word >> 8) | (u16_word << 8));
}


// k_u16kp_reg is a const pointer to a const u16 variable - don't want anything to change
float g_convertToFloat(const uint16_t *const k_u16kp_reg, FloatConv regDataType) {
	switch (regDataType) {
		case FloatConv::FLOAT:  // float
		case FloatConv::FLOAT_WS: { // float
			union cnvtUnion {
				uint16_t u16[2];
				float fl;
			} int2flt;
			if (regDataType == FloatConv::FLOAT_WS) {
				int2flt.u16[1] = k_u16kp_reg[0];
				int2flt.u16[0] = k_u16kp_reg[1];
			}
			else{  // no ws, no adjustments needed
				int2flt.u16[0] = k_u16kp_reg[0];
				int2flt.u16[1] = k_u16kp_reg[1];
			}
			return int2flt.fl;
			break;
		}
		case FloatConv::UINT16: // u16 to float
			return float(k_u16kp_reg[0]);
			break;
		case FloatConv::INT16: // s16 to float
			return float(static_cast<int16_t>(k_u16kp_reg[0]));
			break;
		case FloatConv::UINT32: // u32 to float
		case FloatConv::UINT32_WS: { // u32 to float
			union cnvtUnion {
				uint16_t u16[2];
				uint32_t u32;
			} int2u32;
			
			if (regDataType == FloatConv::UINT32_WS) {
				int2u32.u16[1] = k_u16kp_reg[0];
				int2u32.u16[0] = k_u16kp_reg[1];
			}
			else{  // no ws, no adjustments needed
				int2u32.u16[0] = k_u16kp_reg[0];
				int2u32.u16[1] = k_u16kp_reg[1];
			}
			return float(int2u32.u32);
			break;
		}
		case FloatConv::INT32: // s32 to float
		case FloatConv::INT32_WS: {// s32 to float			
			union cnvtUnion {
				uint16_t u16[2];
				int32_t s32;
			} int2s32;
			
			if (regDataType == FloatConv::INT32_WS) {
				int2s32.u16[1] = k_u16kp_reg[0];
				int2s32.u16[0] = k_u16kp_reg[1];
			}
			else{  // no ws, no adjustments needed
				int2s32.u16[0] = k_u16kp_reg[0];
				int2s32.u16[1] = k_u16kp_reg[1];
			}
			return float(int2s32.s32);
			break;
		}
		case FloatConv::MOD1K: // mod1k to float
		case FloatConv::MOD1K_WS: { // mod1k to float
			int32_t s32_regVal;
			uint32_t u32_highWord;
			
			if (regDataType == FloatConv::MOD1K_WS) {
				s32_regVal = k_u16kp_reg[1];
				u32_highWord = k_u16kp_reg[0];
			}
			else{  // no ws, no adjustments needed
				s32_regVal = k_u16kp_reg[0];
				u32_highWord = k_u16kp_reg[1];
			}
			
			s32_regVal += (u32_highWord & 0x7fff) * 1000;  // chop off negative bit, multiply and add
			if (u32_highWord >> 15) {  // if there is a negative bit
				s32_regVal *= (-1);
			}
			return float(s32_regVal);
			break;
		}
		case FloatConv::MOD10K: // mod10k to float
		case FloatConv::MOD10K_WS: { // mod10k to float
			int32_t s32_regVal;
			uint32_t u32_highWord;
			
			if (regDataType == FloatConv::MOD10K_WS) {
				s32_regVal = k_u16kp_reg[1];
				u32_highWord = k_u16kp_reg[0];
			}
			else{  // no ws, no adjustments needed
				s32_regVal = k_u16kp_reg[0];
				u32_highWord = k_u16kp_reg[1];
			}
			
			s32_regVal += (u32_highWord & 0x7fff) * 10000;  // chop off negative bit, multiply and add
			if (u32_highWord >> 15) {  // if there is a negative bit
				s32_regVal *= (-1);
			}
			return float(s32_regVal);
			break;
		}
		case FloatConv::MOD20K: // mod20k to float
		case FloatConv::MOD20K_WS: { // mod20k to float
			float f_regVal;
			uint32_t u32_highWord;
			
			if (regDataType == FloatConv::MOD20K_WS) {
				f_regVal = float(k_u16kp_reg[2]);
				f_regVal += float(k_u16kp_reg[1]) * pow(10.0, 4.0);
				u32_highWord = k_u16kp_reg[0];
			}
			else{  // no ws, no adjustments needed
				f_regVal = float(k_u16kp_reg[0]);
				f_regVal += float(k_u16kp_reg[1]) * pow(10.0, 4.0);
				u32_highWord = k_u16kp_reg[2];
			}
			
			f_regVal += float(u32_highWord & 0x7fff) * pow(10.0, 8.0);
			if (u32_highWord >> 15) {  // if there is a negative bit
				f_regVal *= (-1);
			}
			return f_regVal;
			break;
		}
		case FloatConv::MOD30K: // mod30k to float
		case FloatConv::MOD30K_WS: { // mod30k to float
			float f_regVal;
			uint32_t u32_highWord;
			
			if (regDataType == FloatConv::MOD30K_WS) {
				f_regVal = float(k_u16kp_reg[3]);
				f_regVal += float(k_u16kp_reg[2]) * pow(10.0, 4.0);
				f_regVal += float(k_u16kp_reg[1]) * pow(10.0, 8.0);
				u32_highWord = k_u16kp_reg[0];
			}
			else{  // no ws, no adjustments needed
				f_regVal = float(k_u16kp_reg[0]);
				f_regVal += float(k_u16kp_reg[1]) * pow(10.0, 4.0);
				f_regVal += float(k_u16kp_reg[2]) * pow(10.0, 8.0);
				u32_highWord = k_u16kp_reg[3];
			}
			
			f_regVal += float(u32_highWord & 0x7fff) * pow(10.0, 12.0);
			if (u32_highWord >> 15) {  // if there is a negative bit
				f_regVal *= (-1);
			}
			return f_regVal;
			break;
		}
		case FloatConv::INT64: // u64 to float
		case FloatConv::INT64_WS: { // u64 to float			
			union cnvtUnion {
				uint16_t u16[4];
				int64_t s64;
			} int2s64;
			
			if (regDataType == FloatConv::INT64_WS) {
				int2s64.u16[3] = k_u16kp_reg[0];
				int2s64.u16[2] = k_u16kp_reg[1];
				int2s64.u16[1] = k_u16kp_reg[2];
				int2s64.u16[0] = k_u16kp_reg[3];
			}
			else{  // no ws, no adjustments needed
				int2s64.u16[0] = k_u16kp_reg[0];
				int2s64.u16[1] = k_u16kp_reg[1];
				int2s64.u16[2] = k_u16kp_reg[2];
				int2s64.u16[3] = k_u16kp_reg[3];
			}
			
			return float(int2s64.s64);
			break;
		}
		case FloatConv::UINT64: // u64 to float
		case FloatConv::UINT64_WS: { // u64 to float
			float f_regVal;
			
			if (regDataType == FloatConv::UINT64_WS) {
				f_regVal = float(k_u16kp_reg[3]);
				f_regVal += float(k_u16kp_reg[2]) * pow(2.0, 16.0);
				f_regVal += float(k_u16kp_reg[1]) * pow(2.0, 32.0);
				f_regVal += float(k_u16kp_reg[0]) * pow(2.0, 48.0);
			}
			else{  // no ws, no adjustments needed
				f_regVal = float(k_u16kp_reg[0]);
				f_regVal += float(k_u16kp_reg[1]) * pow(2.0, 16.0);
				f_regVal += float(k_u16kp_reg[2]) * pow(2.0, 32.0);
				f_regVal += float(k_u16kp_reg[3]) * pow(2.0, 48.0);
			}

			return f_regVal;
			break;
		}
		case FloatConv::ENERGY:  // energy units  (eaton designed unit)
		case FloatConv::ENERGY_WS: { // energy units  (eaton designed unit)
			float f_regVal;
			int8_t s8_engrExp;
			int8_t s8_sclExp;
			
			if (regDataType == FloatConv::ENERGY_WS) {
				f_regVal = float(k_u16kp_reg[3]);
				f_regVal += float(k_u16kp_reg[2]) * pow(2.0, 16.0);
				f_regVal += float(k_u16kp_reg[1]) * pow(2.0, 32.0);
				s8_engrExp = int8_t((k_u16kp_reg[0] >> 8));
				s8_sclExp = int8_t((k_u16kp_reg[0] & 0xff));
			}
			else{  // no ws, no adjustments needed
				f_regVal = float(k_u16kp_reg[0]);
				f_regVal += float(k_u16kp_reg[1]) * pow(2.0, 16.0);
				f_regVal += float(k_u16kp_reg[2]) * pow(2.0, 32.0);
				s8_engrExp = int8_t((k_u16kp_reg[3] >> 8));
				s8_sclExp = int8_t((k_u16kp_reg[3] & 0xff));
			}
			
			f_regVal *= pow(10.0, s8_engrExp);
			f_regVal *= pow(2.0, s8_sclExp);
			return f_regVal;
			break;
		}
		case FloatConv::DOUBLE:  // double to float
		case FloatConv::DOUBLE_WS: { // double to float
			union cnvtUnion {
				uint16_t u16[4];
				double dbl;
			} int2dbl;
			
			if (regDataType == FloatConv::DOUBLE_WS) {  // WORDSWAP
				int2dbl.u16[3] = k_u16kp_reg[0];
				int2dbl.u16[2] = k_u16kp_reg[1];
				int2dbl.u16[1] = k_u16kp_reg[2];
				int2dbl.u16[0] = k_u16kp_reg[3];
			}
			else {
				int2dbl.u16[0] = k_u16kp_reg[0];
				int2dbl.u16[1] = k_u16kp_reg[1];
				int2dbl.u16[2] = k_u16kp_reg[2];
				int2dbl.u16[3] = k_u16kp_reg[3];
			}
			
			return float(int2dbl.dbl);
			break;
		}
		default:
			break;
	} // end special switch
	
	return 0.0f;  // should not fire
}






























