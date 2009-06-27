#include "MBC.h"
#include "GBException.h"

static BYTE * _memCartridge;
static int _ROMBank = 1;
static int _memMode = 0;

void SetMBCMemCart(BYTE * mem_cartridge)
{
	_memCartridge = mem_cartridge;
}

BYTE NoneRead(WORD direction)
{
	return _memCartridge[direction];
}

void NoneWrite(WORD direction, BYTE value)
{
	//No hacer nada
	return;
}

void MBC1Write(WORD direction, BYTE value)
{
	if ((direction >=0x2000)&&(direction < 0x4000))
	{
		//Habria que tener en cuenta tambien los bits superiores (4000-5FFF)!!!!!!!!!!!!!!!!!!!
		if ((value & 0x1F)== 0)
			value++;

		_ROMBank = value & 0x1F;
	}else if ((direction >=0x6000)&&(direction < 0x8000))
	{
		_memMode = value & 0x01;
	}
}

BYTE MBC1Read(WORD direction)
{
	if (direction < 0x4000)
		return _memCartridge[direction];
	else
		return _memCartridge[(direction - 0x4000) + (0x4000*_ROMBank)];
}