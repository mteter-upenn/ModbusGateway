


/* _____PROJECT INCLUDES_____________________________________________________ */
#include "RegisterBlock.h"


/* _____GLOBAL VARIABLES_____________________________________________________ */


/* _____PUBLIC FUNCTIONS_____________________________________________________ */
/**
Constructor.

Creates class object using array of registers, the function they represent, the
first register, and the length of the array.

@overload void RegisterBlock::RegisterBlock(uint16_t* u16register_arg, uint8_t 
  u8func_arg, uint16_t u16first_reg_arg, uint16_t u16length_arg)
@param u16register_arg
@param u8func_arg
@param u16first_reg_arg
@param u16length_arg
@ingroup setup
*/
RegisterBlock::RegisterBlock(uint16_t* u16register_arg, uint8_t u8func_arg, 
  uint16_t u16first_reg_arg, uint16_t u16length_arg)
{
	block.u16regs = u16register_arg;
	block.u8func = u8func_arg;
	block.u16strt = u16first_reg_arg;
	block.u16lgth = u16length_arg;
}

RegisterBlock::RegisterBlock(uint16_t* u16register_arg, uint8_t u8func_arg, 
  uint16_t u16first_reg_arg, uint16_t u16length_arg, uint8_t u8flags)
{
	bool _bs = 2 & u8flags;
	
	block.u16regs = u16register_arg;
	block.u8func = u8func_arg;
	block.u16strt = u16first_reg_arg;
	block.u16lgth = u16length_arg;
	
	if (_bs)
		_byteSwap();
}


RegisterBlock::RegisterBlock(int16_t *s16register_arg, uint16_t *u16dummy, 
  uint8_t u8func_arg, uint16_t u16first_reg_arg, uint16_t u16length_arg)
{	
	block.u16regs = u16dummy;
	block.u8func = u8func_arg;
	block.u16strt = u16first_reg_arg;
	block.u16lgth = u16length_arg;
	
	memcpy(block.u16regs, s16register_arg, (u16length_arg * 2));
}

RegisterBlock::RegisterBlock(int16_t *s16register_arg, uint16_t *u16dummy, 
  uint8_t u8func_arg, uint16_t u16first_reg_arg, uint16_t u16length_arg, 
  uint8_t u8flags)
{
	bool _bs = 2 & u8flags;
	
	block.u16regs = u16dummy;
	block.u8func = u8func_arg;
	block.u16strt = u16first_reg_arg;
	block.u16lgth = u16length_arg;
	
	memcpy(block.u16regs, s16register_arg, (u16length_arg * 2));
	
	if (_bs)
		_byteSwap();
}


RegisterBlock::RegisterBlock(uint32_t *u32register_arg, uint16_t *u16dummy, 
  uint8_t u8func_arg, uint16_t u16first_reg_arg, uint16_t u16length_arg)
{
	block.u16regs = u16dummy;
	block.u8func = u8func_arg;
	block.u16strt = u16first_reg_arg;
	block.u16lgth = (u16length_arg * 2);
	
	memcpy(block.u16regs, u32register_arg, (u16length_arg * 4));
}

RegisterBlock::RegisterBlock(uint32_t *u32register_arg, uint16_t *u16dummy, 
  uint8_t u8func_arg, uint16_t u16first_reg_arg, uint16_t u16length_arg, 
  uint8_t u8flags)
{
	bool _bs = 2 & u8flags;
	bool _ws = 1 & u8flags;
	
	block.u16regs = u16dummy;
	block.u8func = u8func_arg;
	block.u16strt = u16first_reg_arg;
	block.u16lgth = (u16length_arg * 2);
	
	memcpy(block.u16regs, u32register_arg, (u16length_arg * 4));
	
	if (_ws)
		_wordSwap();
	
	if (_bs)
		_byteSwap();
}


RegisterBlock::RegisterBlock(int32_t *s32register_arg, uint16_t *u16dummy, 
  uint8_t u8func_arg, uint16_t u16first_reg_arg, uint16_t u16length_arg)
{
	block.u16regs = u16dummy;
	block.u8func = u8func_arg;
	block.u16strt = u16first_reg_arg;
	block.u16lgth = (u16length_arg * 2);
	
	memcpy(block.u16regs, s32register_arg, (u16length_arg * 4));
}

RegisterBlock::RegisterBlock(int32_t *s32register_arg, uint16_t *u16dummy, 
  uint8_t u8func_arg, uint16_t u16first_reg_arg, uint16_t u16length_arg, 
  uint8_t u8flags)
{
	bool _bs = 2 & u8flags;
	bool _ws = 1 & u8flags;
	
	block.u16regs = u16dummy;
	block.u8func = u8func_arg;
	block.u16strt = u16first_reg_arg;
	block.u16lgth = (u16length_arg * 2);
	
	memcpy(block.u16regs, s32register_arg, (u16length_arg * 4));
	
	if (_ws)
		_wordSwap();
	
	if (_bs)
		_byteSwap();
}


RegisterBlock::RegisterBlock(float *fl_register_arg, uint16_t *u16dummy, 
  uint8_t u8func_arg, uint16_t u16first_reg_arg, uint16_t u16length_arg)
{
	block.u16regs = u16dummy;
	block.u8func = u8func_arg;
	block.u16strt = u16first_reg_arg;
	block.u16lgth = (u16length_arg * 2);
	
	memcpy(block.u16regs, fl_register_arg, (u16length_arg * 4));
}

RegisterBlock::RegisterBlock(float *fl_register_arg, uint16_t *u16dummy, 
  uint8_t u8func_arg, uint16_t u16first_reg_arg, uint16_t u16length_arg, 
  uint8_t u8flags)
{
	bool _bs = 2 & u8flags;
	bool _ws = 1 & u8flags;
	
	block.u16regs = u16dummy;
	block.u8func = u8func_arg;
	block.u16strt = u16first_reg_arg;
	block.u16lgth = (u16length_arg * 2);
	
	memcpy(block.u16regs, fl_register_arg, (u16length_arg * 4));
	
	if (_ws)
		_wordSwap();
	
	if (_bs)
		_byteSwap();
}


/* _____PRIVATE FUNCTIONS___________________________________________________ */
void RegisterBlock::_byteSwap()
{
	uint16_t _register;
	uint16_t _byte;
	
	for (int i; i < block.u16lgth; i++)
	{
		_register = block.u16regs[i];
		_byte = _register << 8;
		block.u16regs[i] = ((_byte) | (_register >> 8));		
	}
}

void RegisterBlock::_wordSwap()
{
	uint16_t _register;
	
	for (int i; i < (block.u16lgth / 2); i++)
	{
		_register = block.u16regs[(2 * i)];
		block.u16regs[(2 * i)] = block.u16regs[((2 * i) + 1)];
		block.u16regs[((2 * i) + 1)] = _register;		
	}
}
	

