
#include "ModbusStack.h"

const ModbusRequest ModbusStack::mk_mrInvalid = {0, 46, 0, 0, 0, 0, 0, 0}; // , false, false};
ModbusRequest ModbusStack::m_mrDummy = {0, 0, 0, 0, 0, 0, 0, 0};  //, false, false};
const uint8_t ModbusStack::k_u8_maxSize = MODBUSSTACK_MAXSIZE;
uint16_t ModbusStack::m_u16_idGen = 0;


uint16_t ModbusStack::add(uint8_t u8_flags, uint8_t u8_id, uint8_t u8_vid, uint8_t u8_func, 
												 uint16_t u16_start, uint16_t u16_length, uint8_t u8_mtrType, // bool b_adjReq, 
												 uint8_t u8_priority) {
	uint8_t u8_ind;
	
	if (m_u8_length >= k_u8_maxSize) {  // stack already maxed out
		return 0;  // 0 is an invalid unique id
	}
	
	switch (u8_priority) {
		case 1:
			pushBack(m_u8_1end);
			m_u8_1end++;
			m_u8_2end++;
			m_u8_length++;
			
			u8_ind = m_u8_1end - 1;
			break;
		case 2:
			pushBack(m_u8_2end);
			m_u8_2end++;
			m_u8_length++;
			
			u8_ind = m_u8_2end - 1;
			break;
		default: // anything else just assume last
			pushBack(m_u8_length);
			m_u8_length++;
			
			u8_ind = m_u8_length - 1;
			break;
	}
	
	++m_u16_idGen; // = (m_u16_idGen != 65535) ? m_u16_idGen + 1: 1;
	if (m_u16_idGen == 0) m_u16_idGen = 1;
	m_mbStack[u8_ind].u16_unqId    = m_u16_idGen;	

	m_mbStack[u8_ind].u8_flags     = u8_flags;
	m_mbStack[u8_ind].u8_id        = u8_id;
	m_mbStack[u8_ind].u8_vid       = u8_vid;
	m_mbStack[u8_ind].u8_func      = u8_func;
	m_mbStack[u8_ind].u16_start    = u16_start;
	m_mbStack[u8_ind].u16_length   = u16_length;
	m_mbStack[u8_ind].u8_mtrType   = u8_mtrType;
	
	return m_mbStack[u8_ind].u16_unqId;
}


uint16_t ModbusStack::add(const ModbusRequest mbReq, uint8_t u8_priority) {
	return add(mbReq.u8_flags, mbReq.u8_id, mbReq.u8_vid, mbReq.u8_func, mbReq.u16_start, 
	           mbReq.u16_length, mbReq.u8_mtrType, u8_priority);
}


bool ModbusStack::removeByUnqId(uint16_t u16_unqId) {
	for (int ii = 0; ii < m_u8_length; ++ii) {
		if (m_mbStack[ii].u16_unqId == u16_unqId) {
			return removeByInd(ii);
		}
	}
	return false;
}


bool ModbusStack::removeByInd(uint8_t u8_ind) {
	Serial.println("removeByInd");
	if (u8_ind < m_u8_length) {
		pullForward(u8_ind);
			
		if ((u8_ind < m_u8_1end) && (m_u8_1end > 0)) {
			m_u8_1end--;
		}
		if ((u8_ind < m_u8_2end) && (m_u8_2end > 0)) {
			m_u8_2end--;
		}
		m_u8_length--;
		return true;
	}
	return false;
}


bool ModbusStack::flagSentMsg(uint16_t u16_unqId){
	for (int ii = 0; ii < m_u8_length; ++ii) {
		if (m_mbStack[ii].u16_unqId == u16_unqId) {
			m_mbStack[ii].u8_flags |= 0x40;  // mark sent flag
			return true;
		}
	}
	
	return false;
}


bool ModbusStack::getMbReqCopy(uint16_t u16_unqId, ModbusRequest &p_mbReq){
	for (int ii = 0; ii < m_u8_length; ++ii) {
		if (m_mbStack[ii].u16_unqId == u16_unqId) {
			p_mbReq = m_mbStack[ii];
			return true;
		}
	}
	
	return false;
}


uint8_t ModbusStack::getReqInd(uint16_t u16_unqId) {
	if (u16_unqId != 0) {
		for (int ii = 0; ii < m_u8_length; ++ii) {
			if (m_mbStack[ii].u16_unqId == u16_unqId) {
				return ii;
			}
		}
	}
	
	return 255;
}


uint8_t ModbusStack::getNext485() {
	for (int ii = 0; ii < m_u8_length; ++ii) {
		if (!(m_mbStack[ii].u8_flags & MRFLAG_isTcp) && !(m_mbStack[ii].u8_flags & MRFLAG_sentMsg)) {
			return ii;
		}
	}
	
	return 255;
}


uint8_t ModbusStack::getNextTcp() {
	for (int ii = 0; ii < m_u8_length; ++ii) {
		if ((m_mbStack[ii].u8_flags & MRFLAG_isTcp) && !(m_mbStack[ii].u8_flags & MRFLAG_sentMsg)) {
			return ii;
		}
	}
	
	return 255;
}


uint8_t ModbusStack::getLive485() {
	for (int ii = 0; ii < m_u8_length; ++ii) {
		if (!(m_mbStack[ii].u8_flags & MRFLAG_isTcp) && (m_mbStack[ii].u8_flags & MRFLAG_sentMsg)) {
			return ii;
		}
	}
	
	return 255;
}


uint8_t ModbusStack::getLiveTcp(uint8_t u8_sock) {
	for (int ii = 0; ii < m_u8_length; ++ii) {
		if ((m_mbStack[ii].u8_flags & MRFLAG_sentMsg) && (m_mbStack[ii].u8_flags & MRFLAG_isTcp) && 
		    ((m_mbStack[ii].u8_flags & MRFLAG_sckMask) == u8_sock)) {
			return ii;
		}
	}
	
	return 255;
}
	
	
void ModbusStack::clearAll() {
	m_u8_1end = 0;
	m_u8_2end = 0;
	m_u8_length = 0;
}


uint8_t ModbusStack::getLength() {
	return m_u8_length;
}


void ModbusStack::pushBack(uint8_t u8_ind) {
	if (u8_ind >= m_u8_length) {
		// if it is equal to the length, then do nothing
		return;
	}
	for (int ii = m_u8_length; ii > u8_ind; --ii) {
		m_mbStack[ii] = m_mbStack[ii - 1];
	}
}


void ModbusStack::pullForward(uint8_t u8_ind) {
	if (u8_ind >= m_u8_length) {
		// if it is equal to the length, then do nothing
		return;
	}
	for (int jj = u8_ind + 1; jj < m_u8_length; ++jj) {
		m_mbStack[jj - 1] = m_mbStack[jj];
	}
}


uint8_t ModbusStack::getMrSocket(ModbusRequest mbReq) {
	return (mbReq.u8_flags & MRFLAG_sckMask);
}


ModbusRequest ModbusStack::operator[](int index) const {
	if (index >= k_u8_maxSize || index < 0) {		
		return mk_mrInvalid;
	}
	return m_mbStack[index];
};
	
	
ModbusRequest& ModbusStack::operator[](int index) {
	if (index >= k_u8_maxSize || index < 0) {
		// need to return something for the left side, probably a better way of handling this though
		return m_mrDummy;  
	}
	
	return m_mbStack[index];
};


void ModbusStack::printReqByInd(uint8_t u8_ind) {
	if (u8_ind >= m_u8_length) {
		Serial.println("Index out of bounds!");
		return;
	}
	
	Serial.print("Unique Stack Id:  "); Serial.print(m_mbStack[u8_ind].u16_unqId, DEC);
	Serial.print("\tFunction:   "); Serial.print(m_mbStack[u8_ind].u8_func, DEC);
	Serial.print("\tMeter Type: "); Serial.println(m_mbStack[u8_ind].u8_mtrType, DEC);
	
	Serial.print("Actual Id:        "); Serial.print(m_mbStack[u8_ind].u8_id, DEC);
	Serial.print("\tStart Reg:  "); Serial.print(m_mbStack[u8_ind].u16_start, DEC);
	Serial.print("\tFlags:      "); Serial.println(m_mbStack[u8_ind].u8_flags, BIN);
	
	Serial.print("Virtual Id:       "); Serial.print(m_mbStack[u8_ind].u8_vid, DEC);
	Serial.print("\tNum Regs:   "); Serial.println(m_mbStack[u8_ind].u16_length, DEC);
}
