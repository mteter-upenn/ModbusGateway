
#ifndef ModbusStack_h
#define ModbusStack_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <ModbusStructs.h>
	
#define MODBUSSTACK_MAXSIZE 128	

class ModbusStack {
private:
	uint8_t m_u8_length;  // total length of stack
	uint8_t m_u8_1end;    // where priority 1 ends
	uint8_t m_u8_2end;
	// uint8_t m_u8_3end;
	
	uint16_t m_u16_idGen;
	
	
	ModbusRequest m_mbStack[MODBUSSTACK_MAXSIZE];  // created array of structs instead of pointers to structs, since
	                             //   the same amount of memory will be used either way if we get 
															 //   close to a 128 high stack.  also no chance of leaks this way
	static const ModbusRequest mk_mrInvalid;
	static ModbusRequest m_mrDummy;  // dummy ModbusRequest for left side of expressions
	
	// create empty space at index
	void pushBack(uint8_t u8_ind);
	// remove index and replace with following struct in array
	void pullForward(uint8_t u8_ind);
	
	uint8_t getMrSocket(ModbusRequest mbReq);
public:
	// constructor ###################################################################################
	ModbusStack(): m_u8_length(0), m_u8_1end(0), m_u8_2end(0), m_u16_idGen(1) {}
	
	// public constants ##############################################################################
	static const uint8_t k_u8_maxSize;
	
	// class functions ###############################################################################
	// add struct to list with given values and priority
	uint16_t add(uint8_t u8_flags, uint8_t u8_id, uint8_t u8_vid, uint8_t u8_func, uint16_t u16_start,
	         uint16_t u16_length, uint8_t u8_mtrType, uint8_t u8_priority);
  uint16_t add(const ModbusRequest mbReq, uint8_t u8_priority);
	// uint8_t add(uint8_t u8_vid, uint8_t *u8p_mbHdr, uint8_t u8_priority);
	
	// remove struct with given unique id
	bool removeByUnqId(uint16_t u16_unqId);
	bool removeByInd(uint8_t u8_ind);
	
	// set b_sentReq to true
	bool flagSentMsg(uint16_t u16_unqId);  // NECESSARY?
	
	// get request
	bool getMbReqCopy(uint16_t u16_unqId, ModbusRequest &p_mbReq);
	
	// get request index by unique id, be aware that this is volatile
	// could overload [] operator, but that would entail searches every call which could get very costly
	uint8_t getReqInd(uint16_t u16_unqId);
	
	// get index of next available request for given protocol
	uint8_t getNext485();
	uint8_t getNextTcp();
	
	// get index of live requests over protocol
	uint8_t getLive485();
	uint8_t getLiveTcp(uint8_t u8_sock);
	
	// set all lengths to 0, thereby removing all requests, nothing else need to be done
	void clearAll();
	
	// return total requests in stack
	uint8_t getLength();
	
	// overloads #####################################################################################
	// Overloaded index operator to allow getting and setting 
	ModbusRequest operator[](int index) const;
	ModbusRequest& operator[](int index);

	void printReqByInd(uint8_t u8_ind);
};

#endif

