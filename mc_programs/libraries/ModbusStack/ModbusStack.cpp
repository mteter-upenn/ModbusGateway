
// #include "Arduino.h"
#include "ModbusStack.h"

const ModbusRequest ModbusStack::mk_mrInvalid = {0, 128, 0, 0, 0, 0, 0, false, false};
ModbusRequest ModbusStack::m_mrDummy = {0, 0, 0, 0, 0, 0, 0, false, false};
const uint8_t mk_maxSize = MODBUSSTACK_MAXSIZE;



uint8_t ModbusStack::add(uint8_t u8_tcp485Req, uint8_t u8_id, uint8_t u8_vid, uint8_t u8_func, 
												 uint16_t u16_start, uint16_t u16_length, bool b_adjReq, 
												 uint8_t u8_priority) {
	uint8_t u8_ind;
	
	if (m_u8_length >= mk_maxSize) {  // stack already maxed out
		return 0;  // 0 is an invalid unique id
	}
	
	switch (u8_priority) {
		case 1:
			pushBack(m_u8_1end);
			m_u8_1end++;
			m_u8_2end++;
			m_u8_length++;
			
			u8_ind = m_u8_1end;
			break;
		case 2:
			pushBack(m_u8_2end);
			// m_u8_1end++;
			m_u8_2end++;
			m_u8_length++;
			
			u8_ind = m_u8_2end;
			break;
		default: // anything else just assume last
			pushBack(m_u8_length);
			// m_u8_1end++;
			// m_u8_2end++;
			m_u8_length++;
			
			u8_ind = m_u8_length;
			break;
	}
	
	m_mbStack[u8_ind].u16_unqId    = (m_u16_idGen != 65535) ? ++m_u16_idGen : 1;
	m_mbStack[u8_ind].u8_tcp485Req = u8_tcp485Req;
	m_mbStack[u8_ind].u8_id        = u8_id;
	m_mbStack[u8_ind].u8_vid       = u8_vid;
	m_mbStack[u8_ind].u8_func      = u8_func;
	m_mbStack[u8_ind].u16_start    = u16_start;
	m_mbStack[u8_ind].u16_length   = u16_length;
	m_mbStack[u8_ind].b_adjReq     = b_adjReq;
	// m_mbStack[u8_ind].b_sentReq    = false;	
	
	return m_mbStack[u8_ind].u16_unqId;
}


// uint8_t ModbusStack::add(uint8_t u8_vid, uint8_t *u8p_mbHdr, uint8_t u8_priority) {
	// uint8_t u8_ind;
	// uint8_t   u8_tcp485Req;
	// uint8_t   u8_id;
	// uint8_t   u8_func;
	// uint16_t  u16_start;
	// uint16_t  u16_length;
	// bool      b_adjReq;
	
	// switch (u8_priority) {
		// case 1:
			// pushBack(m_u8_1end);
			// m_u8_1end++;
			// m_u8_2end++;
			// m_u8_length++;
			
			// u8_ind = m_u8_1end;
			// break;
		// case 2:
			// pushBack(m_u8_2end);
			// // m_u8_1end++;
			// m_u8_2end++;
			// m_u8_length++;
			
			// u8_ind = m_u8_2end;
			// break;
		// default: // anything else just assume last
			// pushBack(m_u8_length);
			// // m_u8_1end++;
			// // m_u8_2end++;
			// m_u8_length++;
			
			// u8_ind = m_u8_length;
			// break;
	// }
	
	// u8_id = u8p_mbHdr[0];
	// u8_func = u8p_mbHdr[1];
	// u8_length
	
	// m_mbStack[u8_ind].u16_unqId  = ++m_u16_idGen;
	// // m_mbStack[u8_ind].u8_tcp485Req   = u8_tcp485Req;
	// m_mbStack[u8_ind].u8_id      = u8_id;
	// m_mbStack[u8_ind].u8_vid     = u8_vid;
	// // m_mbStack[u8_ind].u8_func    = u8_func;
	// // m_mbStack[u8_ind].u16_start  = u16_start;
	// // m_mbStack[u8_ind].u16_length = u16_length;
	// // m_mbStack[u8_ind].b_adjReq   = b_adjReq;
	// m_mbStack[u8_ind].b_sentReq  = false;	
	
	// return m_mbStack[u8_ind].u16_unqId;
// }


bool ModbusStack::remove(uint8_t u8_unqId) {
	for (int ii = 0; ii < m_u8_length; ++ii) {
		if (m_mbStack[ii].u16_unqId == u8_unqId) {
			pullForward(ii);
			
			if ((ii < m_u8_1end) && (m_u8_1end > 0)) {
				m_u8_1end--;
			}
			if ((ii < m_u8_2end) && (m_u8_2end > 0)) {
				m_u8_2end--;
			}
			m_u8_length--;
			return true;
		}
	}
	return false;
}


bool ModbusStack::flagSentMsg(uint8_t u8_unqId){
	for (int ii = 0; ii < m_u8_length; ++ii) {
		if (m_mbStack[ii].u16_unqId == u8_unqId) {
			// m_mbStack[ii].b_sentReq = true;
			m_mbStack[ii].u8_tcp485Req |= 0x40;  // mark sent flag
			return true;
		}
	}
	
	return false;
}


bool ModbusStack::getMbReq(uint8_t u8_unqId, ModbusRequest *p_mbReq){
	for (int ii = 0; ii < m_u8_length; ++ii) {
		if (m_mbStack[ii].u16_unqId == u8_unqId) {
			*p_mbReq = m_mbStack[ii];
			return true;
		}
	}
	
	return false;
}


uint8_t ModbusStack::getNext485() {
	for (int ii = 0; ii < m_u8_length; ++ii) {
		// if (!(m_mbStack[ii].u8_tcp485Req & 0x01) && (!m_mbStack[ii].b_sentReq)) {
		if (!(m_mbStack[ii].u8_tcp485Req & 0x01) && !(m_mbStack[ii].u8_tcp485Req & 0x40)) {
			return ii;
		}
	}
	
	return 255;
}


uint8_t ModbusStack::getNextTcp() {
	for (int ii = 0; ii < m_u8_length; ++ii) {
		// if ((m_mbStack[ii].u8_tcp485Req & 0x01) && (!m_mbStack[ii].b_sentReq)) {
		if ((m_mbStack[ii].u8_tcp485Req & 0x01) && !(m_mbStack[ii].u8_tcp485Req & 0x40)) {
			return ii;
		}
	}
	
	return 255;
}


uint8_t ModbusStack::getLive485() {
	for (int ii = 0; ii < m_u8_length; ++ii) {
		// if (!(m_mbStack[ii].u8_tcp485Req & 0x01) && (m_mbStack[ii].b_sentReq)) {
		if (!(m_mbStack[ii].u8_tcp485Req & 0x01) && (m_mbStack[ii].u8_tcp485Req & 0x40)) {
			return ii;
		}
	}
	
	return 255;
}


uint8_t ModbusStack::getLiveTcp(uint8_t u8_sock) {
	for (int ii = 0; ii < m_u8_length; ++ii) {
		// if ((m_mbStack[ii].b_sentReq) && (getMrSocket(m_mbStack[ii]) == u8_sock) && 
		if ((m_mbStack[ii].u8_tcp485Req & 0x40) && (getMrSocket(m_mbStack[ii]) == u8_sock) && 
		    (m_mbStack[ii].u8_tcp485Req & 0x01)) {
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


uint8_t getMrSocket(ModbusRequest mbReq) {
	return ((mbReq.u8_tcp485Req >> 1) & 0x07);
}


ModbusRequest ModbusStack::operator[](int index) const {
	if (index >= mk_maxSize || index < 0) {		
		return mk_mrInvalid;
	}
	return m_mbStack[index];
};
	
	
ModbusRequest& ModbusStack::operator[](int index) {
	if (index >= mk_maxSize || index < 0) {
		// need to return something for the left side, probably a better way of handling this though
		return m_mrDummy;  
	}
	
	return m_mbStack[index];
};