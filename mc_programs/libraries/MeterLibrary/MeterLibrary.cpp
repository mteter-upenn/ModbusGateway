/* _____PROJECT INCLUDES_____________________________________________________ */
#include "MeterLibrary.h"
#include "ModbusMaster.h"

/* _____GLOBAL VARIABLES_____________________________________________________ */



/*______STATIC VARIABLES_____________________________________________________ */



/* _____PUBLIC FUNCTIONS_____________________________________________________ */
/**
Constructor.

@ingroup setup
*/
MeterLibrary::MeterLibrary(uint16_t u16_reqReg, uint16_t u16_numRegs, uint8_t u8_mtrType) {
	// address for meter register library, this will never change
	m_u16_regBlkStart = word(EEPROM.read(6), EEPROM.read(7));	
		
	changeInputs(u16_reqReg, u16_numRegs, u8_mtrType);
	
}

int MeterLibrary::changeInputs(uint16_t u16_reqReg, uint16_t u16_numRegs, uint8_t u8_mtrType) {
	m_u16_reqReg = u16_reqReg;
	m_u16_numRegs = u16_numRegs;
	m_b_allSameType = false;
	
	if ((u8_mtrType < EEPROM.read(m_u16_regBlkStart + 2)) || (u8_mtrType == 0)) {
		m_u8_mtrType = u8_mtrType;
	}
	else {
		m_u8_mtrType = 0;
		return 1;
	}
	
	m_u16_mtrLibStart = word(EEPROM.read(m_u16_regBlkStart + 4 * u8_mtrType - 1), 
		EEPROM.read(m_u16_regBlkStart + 4 * u8_mtrType));
	m_u16_blkStrtInd = word(EEPROM.read(m_u16_mtrLibStart), EEPROM.read(m_u16_mtrLibStart + 1));
	m_u8_numBlks = EEPROM.read(m_u16_mtrLibStart + 2);
	m_u16_grpStrtInd = word(EEPROM.read(m_u16_mtrLibStart + 3), EEPROM.read(m_u16_mtrLibStart + 4));
	m_u8_numGrps = EEPROM.read(m_u16_mtrLibStart + 5);
	
	for (int ii = 0; ii < m_u8_numBlks; ++ii) {
		uint16_t u16_blkFirstReg;
		uint16_t u16_blkLastReg;
	
		u16_blkFirstReg = word(EEPROM.read(((5 * ii) + m_u16_blkStrtInd)), EEPROM.read(((5 * ii) + 
			m_u16_blkStrtInd + 1)));
		u16_blkLastReg = word(EEPROM.read(((5 * ii) + m_u16_blkStrtInd + 2)), EEPROM.read(((5 * ii) + 
			m_u16_blkStrtInd + 3)));

		if ((m_u16_reqReg >= u16_blkFirstReg) && (m_u16_reqReg <= u16_blkLastReg)) {
			m_reqRegDataType = static_cast<FloatConv>(static_cast<int8_t>(EEPROM.read(((5 * ii) + 
				m_u16_blkStrtInd + 4))));
				
			// check if the last register will be in this block, this will be used if the user wishes to 
			//   check all registers for their data types (a costly operation, though it should be easier
			//   given how the blocks are organized)
			if (((m_u16_reqReg + m_u16_numRegs - 1) >= u16_blkFirstReg) && 
					 ((m_u16_reqReg + m_u16_numRegs - 1) <= u16_blkLastReg)) {
				m_b_allSameType = true;
		  }
			return 0;
		} 
	} // end for
	return 2;  // could not find register in blocks listed
}


// bool MeterLibrary::changeMeterType(uint8_t u8_mtrType){
	// if ((u8_mtrType < EEPROM.read(m_u16_regBlkStart + 2)) || (u8_mtrType == 0)) {
		// m_u8_mtrType = u8_mtrType;
		
		// // reset meter library pointers
		// m_u16_mtrLibStart = word(EEPROM.read(m_u16_regBlkStart + 4 * u8_mtrType - 1), 
			// EEPROM.read(m_u16_regBlkStart + 4 * u8_mtrType));
		// m_u16_blkStrtInd = word(EEPROM.read(m_u16_mtrLibStart), EEPROM.read(m_u16_mtrLibStart + 1));
		// m_u8_numBlks = EEPROM.read(m_u16_mtrLibStart + 2);
		// m_u16_grpStrtInd = word(EEPROM.read(m_u16_mtrLibStart + 3), EEPROM.read(m_u16_mtrLibStart + 4));
		// m_u8_numGrps = EEPROM.read(m_u16_mtrLibStart + 5);
		// return true;
	// }
	// else {
		// return false;
	// }
// }

// void MeterLibrary::changeRegisters(uint16_t u16_reqReg, uint16_t u16_numRegs){
	// m_u16_reqReg = u16_reqReg;
	// m_u16_numRegs = u16_numRegs;
// }

uint16_t MeterLibrary::getNumReqVals() {
	switch (m_reqRegDataType) {
		case FloatConv::UINT16:
		case FloatConv::INT16:
			// return m_u16_numRegs;
			return 1;
		case FloatConv::MOD20K:
		case FloatConv::MOD20K_WS:
			// return (m_u16_numRegs / 3);
			return 3;
		case FloatConv::UINT64:
		case FloatConv::UINT64_WS:
		case FloatConv::MOD30K:
		case FloatConv::MOD30K_WS:
		case FloatConv::ENERGY:
		case FloatConv::ENERGY_WS:
		case FloatConv::DOUBLE:
		case FloatConv::DOUBLE_WS:
			// return (m_u16_numRegs / 4);
			return 4;
		default:
			// return (m_u16_numRegs / 2);
			return 2;
	}
}



/**
Convert full data request to float.

u8a_data must be address where floats will start (no header spacing accounted for)
*/
void MeterLibrary::convertToFloat(ModbusMaster &node, uint8_t *const u8p_data, bool checkType) {
	uint16_t u16_numReqVals;
	// multiplier such that u16_regMult * (number of requested values) = (number of requested 
	//     registers)
	uint16_t u16_regMult;  
	uint16_t * const u16p_data = (uint16_t*)u8p_data;
	// float *const fp_data = (float*)u8p_data;
	uint16_t u16_reg;
	union cnvtUnion {
		uint16_t u16[2];
		float fl;
	} int2flt;

	
	if (!checkType) {
		u16_regMult = getNumReqVals();
		u16_numReqVals = m_u16_numRegs / u16_regMult;
		
		for (int ii = 0, jj = 0; ii < u16_numReqVals; ++ii, jj += 2) {
			u16_reg = m_u16_reqReg + ii * u16_regMult;
			
			int2flt.fl = convertToFloat(node, u16_reg, false);
			
			// Serial.print(ii, DEC); Serial.print(": "); Serial.println(int2flt.fl);
			
			// words are in correct order, but the bytes in the words need to be swapped
			u16p_data[jj] = swapBytes(int2flt.u16[0]);
			u16p_data[jj + 1] = swapBytes(int2flt.u16[1]);
		}
		
	}
	
}


float MeterLibrary::convertToFloat(ModbusMaster &node, uint16_t u16_reg, bool checkType) {
	FloatConv regDataType(m_reqRegDataType);
	uint8_t u8_regInd;
	
	// check if register is within requested data range
	if ((u16_reg >= m_u16_reqReg) && (u16_reg < (m_u16_reqReg + m_u16_numRegs))) {
		u8_regInd = u16_reg - m_u16_reqReg;
	}
	else {
		return 0.0f;
	}
	
	// if user requests to check individual registers and the data request spans multiple blocks
	if (checkType && !m_b_allSameType) {
		for (int ii = 0; ii < m_u8_numBlks; ++ii) {
			uint16_t u16_blkFirstReg;
			uint16_t u16_blkLastReg;
		
			u16_blkFirstReg = word(EEPROM.read(((5 * ii) + m_u16_blkStrtInd)), EEPROM.read(((5 * ii) + 
				m_u16_blkStrtInd + 1)));
			u16_blkLastReg = word(EEPROM.read(((5 * ii) + m_u16_blkStrtInd + 2)), EEPROM.read(((5 * ii) + 
				m_u16_blkStrtInd + 3)));

			if ((u16_reg >= u16_blkFirstReg) && (u16_reg <= u16_blkLastReg)) {
				regDataType = static_cast<FloatConv>(static_cast<int8_t>(EEPROM.read(((5 * ii) + 
					m_u16_blkStrtInd + 4))));
				break;
			} 
		} // end for
	}
	
	// Serial.println(static_cast<int8_t>(regDataType), DEC);
	
	return g_convertToFloat(&(node._u16ResponseBuffer[u8_regInd]), regDataType);
}


uint16_t swapBytes(uint16_t u16_word) {
	return ((u16_word >> 8) | (u16_word << 8));
}

float g_convertToFloat(uint16_t *const u16p_reg, FloatConv regDataType) {
	/** the following commented out is needed for boards other than teensy to handle doubles */
// #if defined(__arm__) && defined(CORE_TEENSY)  // if teensy3.0 or greater
// #else
  // struct stFloat {
    // uint32_t m : 23;
    // uint8_t e : 8;
    // uint8_t s : 1;
  // };

  // struct stDbl {
    // uint32_t filler : 29;
    // uint32_t m : 23;
    // uint16_t e : 11;
    // uint8_t s : 1;
  // };

  // union dblConv {
    // stDbl sD;
    // uint16_t u16[4];
  // }  dblC;
// #endif
	// Serial.println(static_cast<int8_t>(regDataType), DEC);
	
	switch (regDataType) {
		case FloatConv::FLOAT:  // float
		case FloatConv::FLOAT_WS: { // float
			union cnvtUnion {
				uint16_t u16[2];
				float fl;
			} int2flt;
			if (regDataType == FloatConv::FLOAT_WS) {
				int2flt.u16[1] = u16p_reg[0];
				int2flt.u16[0] = u16p_reg[1];
			}
			else{  // no ws, no adjustments needed
				int2flt.u16[0] = u16p_reg[0];
				int2flt.u16[1] = u16p_reg[1];
			}
			return int2flt.fl;
			break;
		}
		case FloatConv::UINT16: // u16 to float
			return float(u16p_reg[0]);
			break;
		case FloatConv::INT16: // s16 to float
			return float(static_cast<int16_t>(u16p_reg[0]));
			break;
		case FloatConv::UINT32: // u32 to float
		case FloatConv::UINT32_WS: { // u32 to float
			union cnvtUnion {
				uint16_t u16[2];
				uint32_t u32;
			} int2u32;
			
			if (regDataType == FloatConv::UINT32_WS) {
				int2u32.u16[1] = u16p_reg[0];
				int2u32.u16[0] = u16p_reg[1];
			}
			else{  // no ws, no adjustments needed
				int2u32.u16[0] = u16p_reg[0];
				int2u32.u16[1] = u16p_reg[1];
			}
			return float(int2u32.u32);
			break;
		}
		case FloatConv::INT32: // s32 to float
		case FloatConv::INT32_WS: {// s32 to float
			
			// Serial.println("firing");
			
			union cnvtUnion {
				uint16_t u16[2];
				int32_t s32;
			} int2s32;
			
			if (regDataType == FloatConv::INT32_WS) {
				int2s32.u16[1] = u16p_reg[0];
				int2s32.u16[0] = u16p_reg[1];
			}
			else{  // no ws, no adjustments needed
				int2s32.u16[0] = u16p_reg[0];
				int2s32.u16[1] = u16p_reg[1];
			}
			return float(int2s32.s32);
			break;
		}
		case FloatConv::MOD1K: // mod1k to float
		case FloatConv::MOD1K_WS: { // mod1k to float
			int32_t s32_regVal;
			uint32_t u32_highWord;
			
			if (regDataType == FloatConv::MOD1K_WS) {
				s32_regVal = u16p_reg[1];
				u32_highWord = u16p_reg[0];
			}
			else{  // no ws, no adjustments needed
				s32_regVal = u16p_reg[0];
				u32_highWord = u16p_reg[1];
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
				s32_regVal = u16p_reg[1];
				u32_highWord = u16p_reg[0];
			}
			else{  // no ws, no adjustments needed
				s32_regVal = u16p_reg[0];
				u32_highWord = u16p_reg[1];
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
				f_regVal = float(u16p_reg[2]);
				f_regVal += float(u16p_reg[1]) * pow(10.0, 4.0);
				u32_highWord = u16p_reg[0];
			}
			else{  // no ws, no adjustments needed
				f_regVal = float(u16p_reg[0]);
				f_regVal += float(u16p_reg[1]) * pow(10.0, 4.0);
				u32_highWord = u16p_reg[2];
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
				f_regVal = float(u16p_reg[3]);
				f_regVal += float(u16p_reg[2]) * pow(10.0, 4.0);
				f_regVal += float(u16p_reg[1]) * pow(10.0, 8.0);
				u32_highWord = u16p_reg[0];
			}
			else{  // no ws, no adjustments needed
				f_regVal = float(u16p_reg[0]);
				f_regVal += float(u16p_reg[1]) * pow(10.0, 4.0);
				f_regVal += float(u16p_reg[2]) * pow(10.0, 8.0);
				u32_highWord = u16p_reg[3];
			}
			
			f_regVal += float(u32_highWord & 0x7fff) * pow(10.0, 12.0);
			if (u32_highWord >> 15) {  // if there is a negative bit
				f_regVal *= (-1);
			}
			return f_regVal;
			break;
		}
		case FloatConv::UINT64: // u64 to float
		case FloatConv::UINT64_WS: { // u64 to float
			float f_regVal;
			
			if (regDataType == FloatConv::UINT64_WS) {
				f_regVal = float(u16p_reg[3]);
				f_regVal += float(u16p_reg[2]) * pow(2.0, 16.0);
				f_regVal += float(u16p_reg[1]) * pow(2.0, 32.0);
				f_regVal += float(u16p_reg[0]) * pow(2.0, 48.0);
			}
			else{  // no ws, no adjustments needed
				f_regVal = float(u16p_reg[0]);
				f_regVal += float(u16p_reg[1]) * pow(2.0, 16.0);
				f_regVal += float(u16p_reg[2]) * pow(2.0, 32.0);
				f_regVal += float(u16p_reg[3]) * pow(2.0, 48.0);
			}

			return f_regVal;
			break;
		}
		case FloatConv::ENERGY:  // energy units  (eaton designed unit)
		case FloatConv::ENERGY_WS: { // energy units  (eaton designed unit)
			float f_regVal;
			int8_t s8_engrExp;
			
			if (regDataType == FloatConv::ENERGY_WS) {
				f_regVal = float(u16p_reg[3]);
				f_regVal += float(u16p_reg[2]) * pow(2.0, 16.0);
				f_regVal += float(u16p_reg[1]) * pow(2.0, 32.0);
				s8_engrExp = int8_t((u16p_reg[0] >> 8));
			}
			else{  // no ws, no adjustments needed
				f_regVal = float(u16p_reg[0]);
				f_regVal += float(u16p_reg[1]) * pow(2.0, 16.0);
				f_regVal += float(u16p_reg[2]) * pow(2.0, 32.0);
				s8_engrExp = int8_t((u16p_reg[3] >> 8));
			}
			
			f_regVal *= pow(10.0, s8_engrExp);
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
				int2dbl.u16[3] = u16p_reg[0];
				int2dbl.u16[2] = u16p_reg[1];
				int2dbl.u16[1] = u16p_reg[2];
				int2dbl.u16[0] = u16p_reg[3];
			}
			else {
				int2dbl.u16[0] = u16p_reg[0];
				int2dbl.u16[1] = u16p_reg[1];
				int2dbl.u16[2] = u16p_reg[2];
				int2dbl.u16[3] = u16p_reg[3];
			}
			
			return float(int2dbl.dbl);
			break;
		}
		// default: // pass on data with no adjustments
			// for (int jj = 0, ii = 9; jj < u16_adjNumRegs; ++jj, ii+=2){
				// u16_tempReg = g_mm_node.getResponseBuffer(jj);
				// u8a_mbResp[ii] = (u16_tempReg >> 8);
				// u8a_mbResp[ii + 1] = u16_tempReg;
			// }

			// break;
		default:
			break;
	} // end special switch
	
	return 0.0f;  // should not fire
}






























