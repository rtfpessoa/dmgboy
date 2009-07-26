#include "MBC.h"
#include "GBException.h"

static BYTE * _memCartridge = NULL;
static BYTE * _memRamMBC = NULL;
static int _memMode = 0;

static int _ROMBank = 1;
static int _ROMSize = 0;	//Bytes

static int _RAMBank = 0;
static int _RAMSize = 0;	//Bytes
static int _RAMEnabled = 0;

void InitMBC(BYTE * mem_cartridge, int RomSize)
{
	_memCartridge = mem_cartridge;
	_memMode = 0;

	_ROMBank = 1;
	_ROMSize = RomSize;

	_RAMBank = 0;
	_RAMEnabled = 0;
	_RAMSize = 0;
	_memRamMBC = NULL;
}

void InitMBCNone(BYTE * mem_cartridge, int ROMSize)
{
	InitMBC(mem_cartridge, ROMSize);
}

void InitMBC1(BYTE * mem_cartridge, int ROMSize, int RamHeaderSize)
{
	InitMBC(mem_cartridge, ROMSize);

	if (RamHeaderSize == 0x01)
		_RAMSize = 2048;		//2KB
	else if (RamHeaderSize == 0x02)
		_RAMSize = 8192;		//8KB
	else if (RamHeaderSize == 0x03)
		_RAMSize = 32768;		//32KB

	if (_RAMSize)
		_memRamMBC = new BYTE[_RAMSize];
}

void InitMBC2(BYTE * mem_cartridge, int ROMSize)
{
	InitMBC(mem_cartridge, ROMSize);

	_RAMSize = 512;

	_memRamMBC = new BYTE[_RAMSize];
}

void InitMBC3(BYTE * mem_cartridge, int ROMSize, int RamHeaderSize)
{
	InitMBC1(mem_cartridge, ROMSize, RamHeaderSize);
}

void InitMBC5(BYTE * mem_cartridge, int ROMSize, int RamHeaderSize)
{
	InitMBC1(mem_cartridge, ROMSize, RamHeaderSize);
}

void DestroyMBC()
{
	if (_memRamMBC)
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
			_memRamMBC[direction - 0xA000 + (0x2000*_RAMBank)] = value;
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
		return _memRamMBC[direction - 0xA000 + (0x2000*_RAMBank)];
}

void MBC2Write(WORD direction, BYTE value)
{
	if (direction < 0x2000)	//Habilitar/Deshabilitar RAM
	{
		if (! (direction & 0x10))
			_RAMEnabled = !_RAMEnabled;
	}
	else if (direction < 0x4000)	//Cambiar ROMBank
	{
		if (!(direction & 0x100))
			return;

		if ((value & 0x0F)== 0)
			value++;

		_ROMBank = value & 0x0F;
	}
	else if (direction < 0x6000)
	{
		return;
	}
	else if (direction < 0x8000)	//Seleccionar modo
	{
		return;
	}
	else if ((direction >=0xA000) && (direction < 0xA200))	//Intenta escribir en RAM
	{
		if (_RAMEnabled)
			_memRamMBC[direction - 0xA000] = value & 0x0F;
		//throw GBException("Intenta escribir en RAM de cartucho");
	}
}

BYTE MBC2Read(WORD direction)
{
	if (direction < 0x4000)
		return _memCartridge[direction];
	else if (direction < 0x8000)
		return _memCartridge[(direction - 0x4000) + (0x4000*_ROMBank)];
	else if ((direction >=0xA000) && (direction < 0xC000))
		return _memRamMBC[direction - 0xA000];
}

void MBC3Write(WORD direction, BYTE value)
{
	if (direction < 0x2000)	//Habilitar/Deshabilitar RAM
	{
		_RAMEnabled = ((value & 0x0F) == 0x0A);
	}
	else if (direction < 0x4000)	//Cambiar ROMBank
	{
		value = value ? value : 1;

		_ROMBank = value & 0x7F;
	}
	else if (direction < 0x6000)	//Cambiar RAMBank o RTC
	{
		if (value < 4)			//(Seleccionar RAMBank)
		{
			_RAMBank = value;
		}
		else if ((value >= 0x08) && (value <=0x0C))	//Seleccionar RTC
		{
			//throw GBException("RTC no implementado");
		}
	}
	else if (direction < 0x8000)	
	{
		//throw GBException("RTC no implementado");
	}
	else if ((direction >=0xA000) && (direction < 0xC000))	//Intenta escribir en RAM
	{
		if (_RAMEnabled)
			_memRamMBC[direction - 0xA000 + (0x2000*_RAMBank)] = value;
	}
}

BYTE MBC3Read(WORD direction)
{
	if (direction < 0x4000)
		return _memCartridge[direction];
	else if (direction < 0x8000)
		return _memCartridge[(direction - 0x4000) + (0x4000*_ROMBank)];
	else if ((direction >=0xA000) && (direction < 0xC000))
		return _memRamMBC[direction - 0xA000 + (0x2000*_RAMBank)];
}

void MBC5Write(WORD direction, BYTE value)
{
	if (direction < 0x2000)	//Habilitar/Deshabilitar RAM
	{
		_RAMEnabled = ((value & 0x0F) == 0x0A);
	}
	else if (direction < 0x3000)	//Cambiar ROMBank
	{
		_ROMBank = (_ROMBank & 0x100) | value;
	}
	else if (direction < 0x4000)	//Cambiar ROMBank
	{
		_ROMBank = ((value & 0x01) << 8) | (_ROMBank & 0xFF);
	}
	else if (direction < 0x6000)	//Cambiar RAMBank o RTC
	{
		_RAMBank = value & 0x0F;
	}
	else if (direction < 0x8000)	
	{
		//throw GBException("Zona no conocida");
	}
	else if ((direction >=0xA000) && (direction < 0xC000))	//Intenta escribir en RAM
	{
		if (_RAMEnabled)
			_memRamMBC[direction - 0xA000 + (0x2000*_RAMBank)] = value;
	}
}

BYTE MBC5Read(WORD direction)
{
	if (direction < 0x4000)
		return _memCartridge[direction];
	else if (direction < 0x8000)
		return _memCartridge[(direction - 0x4000) + (0x4000*_ROMBank)];
	else if ((direction >=0xA000) && (direction < 0xC000))
		return _memRamMBC[direction - 0xA000 + (0x2000*_RAMBank)];
}