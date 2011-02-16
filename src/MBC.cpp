/*
 This file is part of gbpablog.
 
 gbpablog is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 gbpablog is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with gbpablog.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "MBC.h"
#include "GBException.h"
#include <fstream>
#include <iostream>
using namespace std;

static BYTE * _memCartridge = NULL;
static BYTE * _memRamMBC = NULL;
static int _memMode = 0;

static int _ROMBank = 1;
static int _ROMSize = 0;	//Bytes

static int _RAMBank = 0;
static int _RAMSize = 0;	//Bytes
static int _RAMEnabled = 0;

static char * _ROMName = NULL;

void MBCSaveRam();
void MBCLoadRam();

void InitMBC(char * nameROM, BYTE * mem_cartridge, int RomSize)
{
	_ROMName = nameROM;
	_memCartridge = mem_cartridge;
	_memMode = 0;

	_ROMBank = 1;
	_ROMSize = RomSize;

	_RAMBank = 0;
	_RAMEnabled = 0;
	_RAMSize = 0;
	_memRamMBC = NULL;
}

void InitMBCNone(char * nameROM, BYTE * mem_cartridge, int ROMSize)
{
	InitMBC(nameROM, mem_cartridge, ROMSize);
}

void InitMBC1(char * nameROM, BYTE * mem_cartridge, int ROMSize, int RamHeaderSize)
{
	InitMBC(nameROM, mem_cartridge, ROMSize);

	if (RamHeaderSize == 0x01)
		_RAMSize = 2048;		//2KB
	else if (RamHeaderSize == 0x02)
		_RAMSize = 8192;		//8KB
	else if (RamHeaderSize == 0x03)
		_RAMSize = 32768;		//32KB

	if (_RAMSize)
		_memRamMBC = new BYTE[_RAMSize];
	
	MBCLoadRam();
}

void InitMBC2(char * nameROM, BYTE * mem_cartridge, int ROMSize)
{
	InitMBC(nameROM, mem_cartridge, ROMSize);

	_RAMSize = 512;

	_memRamMBC = new BYTE[_RAMSize];
	
	MBCLoadRam();
}

void InitMBC3(char * nameROM, BYTE * mem_cartridge, int ROMSize, int RamHeaderSize)
{
	InitMBC(nameROM, mem_cartridge, ROMSize);

	if (RamHeaderSize == 0x01)
		_RAMSize = 8192;		//8KB = 64Kb
	else if (RamHeaderSize == 0x02)
		_RAMSize = 32768;		//32KB = 256Kb
	else if (RamHeaderSize == 0x03)
		_RAMSize = 131072;		//128KB = 1Mb

	if (_RAMSize)
		_memRamMBC = new BYTE[_RAMSize];
	
	MBCLoadRam();
}

void InitMBC5(char * nameROM, BYTE * mem_cartridge, int ROMSize, int RamHeaderSize)
{
	InitMBC1(nameROM, mem_cartridge, ROMSize, RamHeaderSize);
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
		if (!_RAMEnabled)
			MBCSaveRam();
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
	}
}

BYTE MBC1Read(WORD direction)
{
	if (direction < 0x4000)
		return _memCartridge[direction];
	else if (direction < 0x8000)
		return _memCartridge[(direction - 0x4000) + (0x4000*_ROMBank)];
	else if ((direction >=0xA000) && (direction < 0xC000) && _RAMEnabled)
		return _memRamMBC[direction - 0xA000 + (0x2000*_RAMBank)];

	return 0;
}

void MBC2Write(WORD direction, BYTE value)
{
	if (direction < 0x2000)	//Habilitar/Deshabilitar RAM
	{
		if (! (direction & 0x10))
			_RAMEnabled = !_RAMEnabled;
		
		if (!_RAMEnabled)
			MBCSaveRam();
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
	else if ((direction >=0xA000) && (direction < 0xC000) && (_RAMEnabled))
		return _memRamMBC[direction - 0xA000];

	return 0;
}

void MBC3Write(WORD direction, BYTE value)
{
	if (direction < 0x2000)	//Habilitar/Deshabilitar RAM
	{
		_RAMEnabled = ((value & 0x0F) == 0x0A);
		if (!_RAMEnabled)
			MBCSaveRam();
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
	else if ((direction >=0xA000) && (direction < 0xC000) && (_RAMEnabled))
		return _memRamMBC[direction - 0xA000 + (0x2000*_RAMBank)];

	return 0;
}

void MBC5Write(WORD direction, BYTE value)
{
	if (direction < 0x2000)	//Habilitar/Deshabilitar RAM
	{
		_RAMEnabled = ((value & 0x0F) == 0x0A);
		if (!_RAMEnabled)
			MBCSaveRam();
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
	else if ((direction >=0xA000) && (direction < 0xC000) && (_RAMEnabled))
		return _memRamMBC[direction - 0xA000 + (0x2000*_RAMBank)];

	return 0;
}

void MBCLoadRam()
{
	string fileName = _ROMName;
	fileName += ".BATT";
	ifstream file(fileName.c_str(), ios::in|ios::binary);
	
	if (file)
	{
		file.read((char *)_memRamMBC, _RAMSize);
		file.close();
	}
}

void MBCSaveRam()
{
	string fileName = _ROMName;
	fileName += ".BATT";
	ofstream file(fileName.c_str(), ios::out|ios::binary);
	
	if (file)
	{
		file.write((char *)_memRamMBC, _RAMSize);
		file.close();
	}
}
