/* _____PROJECT INCLUDES_____________________________________________________ */
#include <ModbusStructs.h>

/* _____PUBLIC FUNCTIONS_____________________________________________________ */

uint16_t FloatConvEnumNumRegs(FloatConv dataType) {
	switch (dataType) {
		case FloatConv::UINT16:
		case FloatConv::INT16:
			// return m_u16_numRegs;
			return 1;
		case FloatConv::MOD20K:
		case FloatConv::MOD20K_WS:
			// return (m_u16_numRegs / 3);
			return 3;
		case FloatConv::INT64:
		case FloatConv::INT64_WS:
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

int8_t FloatConv2Int8(FloatConv dataType){
	switch (dataType) {
		case FloatConv::SKIP:
			return -1;
		case FloatConv::FLOAT:
			return 0;
		case FloatConv::UINT16:
			return 1;
		case FloatConv::INT16:
			return 2;
		case FloatConv::UINT32:
			return 3;
		case FloatConv::INT32:
			return 4;
		case FloatConv::MOD1K:
			return 5;
		case FloatConv::MOD10K:
			return 6;
		case FloatConv::MOD20K:
			return 7;
		case FloatConv::MOD30K:
			return 8;
		case FloatConv::INT64:
			return 9;
		case FloatConv::UINT64:
			return 10;
		case FloatConv::ENERGY:
			return 11;
		case FloatConv::DOUBLE:
			return 12;
		case FloatConv::FLOAT_WS:
			return 64;
		case FloatConv::UINT32_WS:
			return 67;
		case FloatConv::INT32_WS:
			return 68;
		case FloatConv::MOD1K_WS:
			return 69;
		case FloatConv::MOD10K_WS:
			return 70;
		case FloatConv::MOD20K_WS:
			return 71;
		case FloatConv::MOD30K_WS:
			return 72;
		case FloatConv::INT64_WS:
			return 73;
		case FloatConv::UINT64_WS:
			return 74;
		case FloatConv::ENERGY_WS:
			return 75;
		case FloatConv::DOUBLE_WS:
			return 76;
		default:
			return 0;
	}
}


uint8_t FloatConv2Uint8(FloatConv dataType) {
	switch (dataType) {
		case FloatConv::SKIP:
			return 255;
		case FloatConv::FLOAT:
			return 0;
		case FloatConv::UINT16:
			return 1;
		case FloatConv::INT16:
			return 2;
		case FloatConv::UINT32:
			return 3;
		case FloatConv::INT32:
			return 4;
		case FloatConv::MOD1K:
			return 5;
		case FloatConv::MOD10K:
			return 6;
		case FloatConv::MOD20K:
			return 7;
		case FloatConv::MOD30K:
			return 8;
		case FloatConv::INT64:
			return 9;
		case FloatConv::UINT64:
			return 10;
		case FloatConv::ENERGY:
			return 11;
		case FloatConv::DOUBLE:
			return 12;
		case FloatConv::FLOAT_WS:
			return 64;
		case FloatConv::UINT32_WS:
			return 67;
		case FloatConv::INT32_WS:
			return 68;
		case FloatConv::MOD1K_WS:
			return 69;
		case FloatConv::MOD10K_WS:
			return 70;
		case FloatConv::MOD20K_WS:
			return 71;
		case FloatConv::MOD30K_WS:
			return 72;
		case FloatConv::INT64_WS:
			return 73;
		case FloatConv::UINT64_WS:
			return 74;
		case FloatConv::ENERGY_WS:
			return 75;
		case FloatConv::DOUBLE_WS:
			return 76;
		default:
			return 0;
	}
}


FloatConv Int8_2_FloatConv(int8_t s8_dataType){
	switch (s8_dataType) {
		case -1:
			return FloatConv::SKIP;
		case 0:
			return FloatConv::FLOAT;
		case 1:
			return FloatConv::UINT16;
		case 2:
			return FloatConv::INT16;
		case 3:
			return FloatConv::UINT32;
		case 4:
			return FloatConv::INT32;
		case 5:
			return FloatConv::MOD1K;
		case 6:
			return FloatConv::MOD10K;
		case 7:
			return FloatConv::MOD20K;
		case 8:
			return FloatConv::MOD30K;
		case 9:
			return FloatConv::INT64;
		case 10:
			return FloatConv::UINT64;
		case 11:
			return FloatConv::ENERGY;
		case 12:
			return FloatConv::DOUBLE;
		case 64:
			return FloatConv::FLOAT_WS;
		case 67:
			return FloatConv::UINT32_WS;
		case 68:
			return FloatConv::INT32_WS;
		case 69:
			return FloatConv::MOD1K_WS;
		case 70:
			return FloatConv::MOD10K_WS;
		case 71:
			return FloatConv::MOD20K_WS;
		case 72:
			return FloatConv::MOD30K_WS;
		case 73:
			return FloatConv::INT64_WS;
		case 74:
			return FloatConv::UINT64_WS;
		case 75:
			return FloatConv::ENERGY_WS;
		case 76:
			return FloatConv::DOUBLE_WS;
		default:
			return FloatConv::FLOAT;
	}
}

FloatConv Char_2_FloatConv(const char *k_cp_dataType) {
  if (strcmp_P(k_cp_dataType, PSTR("SKIP"))) {
		return FloatConv::SKIP;
	}
  else if (strcmp_P(k_cp_dataType, PSTR("FLOAT"))) {
		return FloatConv::FLOAT;
	}
  else if (strcmp_P(k_cp_dataType, PSTR("UINT16"))) {
		return FloatConv::UINT16;
	}
  else if (strcmp_P(k_cp_dataType, PSTR("INT16"))) {
		return FloatConv::INT16;
	}
  else if (strcmp_P(k_cp_dataType, PSTR("UINT32"))) {
		return FloatConv::UINT32;
	}
  else if (strcmp_P(k_cp_dataType, PSTR("INT32"))) {
		return FloatConv::INT32;
	}
  else if (strcmp_P(k_cp_dataType, PSTR("MOD1K"))) {
		return FloatConv::MOD1K;
	}
  else if (strcmp_P(k_cp_dataType, PSTR("MOD10K"))) {
		return FloatConv::MOD10K;
	}
  else if (strcmp_P(k_cp_dataType, PSTR("MOD20K"))) {
		return FloatConv::MOD20K;
	}
  else if (strcmp_P(k_cp_dataType, PSTR("MOD30K"))) {
		return FloatConv::MOD30K;
	}
  else if (strcmp_P(k_cp_dataType, PSTR("INT64"))) {
		return FloatConv::INT64;
	}
  else if (strcmp_P(k_cp_dataType, PSTR("UINT64"))) {
		return FloatConv::UINT64;
	}
  else if (strcmp_P(k_cp_dataType, PSTR("ENERGY"))) {
		return FloatConv::ENERGY;
	}
  else if (strcmp_P(k_cp_dataType, PSTR("DOUBLE"))) {
		return FloatConv::DOUBLE;
	}
  else if (strcmp_P(k_cp_dataType, PSTR("FLOAT_WS"))) {
		return FloatConv::FLOAT_WS;
	}
  else if (strcmp_P(k_cp_dataType, PSTR("UINT32_WS"))) {
		return FloatConv::UINT32_WS;
	}
  else if (strcmp_P(k_cp_dataType, PSTR("INT32_WS"))) {
		return FloatConv::INT32_WS;
	}
  else if (strcmp_P(k_cp_dataType, PSTR("MOD1K_WS"))) {
		return FloatConv::MOD1K_WS;
	}
  else if (strcmp_P(k_cp_dataType, PSTR("MOD10K_WS"))) {
		return FloatConv::MOD10K_WS;
	}
  else if (strcmp_P(k_cp_dataType, PSTR("MOD20K_WS"))) {
		return FloatConv::MOD20K_WS;
	}
  else if (strcmp_P(k_cp_dataType, PSTR("MOD30K_WS"))) {
		return FloatConv::MOD30K_WS;
	}
  else if (strcmp_P(k_cp_dataType, PSTR("INT64_WS"))) {
		return FloatConv::INT64_WS;
	}
  else if (strcmp_P(k_cp_dataType, PSTR("UINT64_WS"))) {
		return FloatConv::UINT64_WS;
	}
  else if (strcmp_P(k_cp_dataType, PSTR("ENERGY_WS"))) {
		return FloatConv::ENERGY_WS;
	}
  else if (strcmp_P(k_cp_dataType, PSTR("DOUBLE_WS"))) {
		return FloatConv::DOUBLE_WS;
	}
	else {
		return FloatConv::FLOAT;
	}
}
