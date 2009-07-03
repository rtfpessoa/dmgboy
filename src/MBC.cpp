#include "MBC.h"
#include "GBException.h"

static BYTE * _memCartridge;
static BYTE * _memRamMBC;
static int _memMode = 0;

static int _ROMBank = 1;
static int _ROMSize = 0;	//Bytes

static int _RAMBank = 0;
static int _RAMSize = 0;	//Bytes
static int _RAMEnabled = 0;

void InitMBC(BYTE * mem_cartridge, int ROMSize, int RamHeaderSize)
{
	_memCartridge = mem_cartridge;
	_memMode = 0;

	_ROMBank = 1;
	_RAMBank = 0;
	_RAMEnabled = 0;

	if (RamHeaderSize == 0x00)
	{
		_RAMSize = 0;			//0KB
		_memRamMBC = NULL;
	}
	else if (RamHeaderSize == 0x01)
		_RAMSize = 2048;		//2KB
	else if (RamHeaderSize == 0x02)
		_RAMSize = 8192;		//8KB
	else if (RamHeaderSize == 0x03)
		_RAMSize = 32768;		//32KB

	if (_RAMSize)
		_memRamMBC = new BYTE[_RAMSize];
}

void DestroyMBC()
{
	delete [] _memRamMBC;
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
	if (direction < 0x2000)	//Habilitar/Deshabilitar RAM
	{
		_RAMEnabled = ((value & 0x0F) == 0x0A);
	}
	else if (direction < 0x4000)	//Cambiar ROMBank
	{
		if ((value & 0x1F)== 0)
			value++;

		_ROMBank = (_ROMBank & 0x60) | (value & 0x1F);
	}
	else if (direction < 0x6000)	//Cambiar ROMBank o RAMBank dependiendo del modo
	{
		if (!_memMode)	//Modo 16/8 (Seleccionar ROMBank)
		{
			_ROMBank = ((value & 0x03) << 5) | (_ROMBank & 0x1F);
		}
		else			//Modo 4/32 (Seleccionar RAMBank)
		{
			_RAMBank = value & 0x03;
		}
	}
	else if (direction < 0x8000)	//Seleccionar modo
	{
		_memMode = value & 0x01;
	}
	else if ((direction >=0xA000) && (direction < 0xC000))	//Intenta escribir en RAM
	{
		if (_RAMEnabled)
			_memRamMBC[direction - 0xA000 + (0x8000*_RAMBank)] = value;
		//throw GBException("Intenta escribir en RAM de cartucho");
	}
}

BYTE MBC1Read(WORD direction)
{
	if (direction < 0x4000)
		return _memCartridge[direction];
	else if (direction < 0x8000)
		return _memCartridge[(direction - 0x4000) + (0x4000*_ROMBank)];
	else if ((direction >=0xA000) && (direction < 0xC000))
		return _memRamMBC[direction - 0xA000 + (0x8000*_RAMBank)];
}