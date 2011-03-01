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

static int _romBank = 1;
static int _romSize = 0;	//Bytes

static int _ramBank = 0;
static int _ramSize = 0;	//Bytes
static int _ramEnabled = 0;

static string _romName = "";
static string _pathBatteries = "";

void MBCSaveRam();
void MBCLoadRam();

void InitMBC(string romName, BYTE * memCartridge, int romSize)
{
	_romName = romName;
	_memCartridge = memCartridge;
	_memMode = 0;

	_romBank = 1;
	_romSize = romSize;

	_ramBank = 0;
	_ramEnabled = 0;
	_ramSize = 0;
	_memRamMBC = NULL;
}

void InitMBCNone(string romName, BYTE * memCartridge, int romSize)
{
	InitMBC(romName, memCartridge, romSize);
}

void InitMBC1(string romName, BYTE * memCartridge, int romSize, int ramHeaderSize)
{
	InitMBC(romName, memCartridge, romSize);

	if (ramHeaderSize == 0x01)
		_ramSize = 2048;		//2KB
	else if (ramHeaderSize == 0x02)
		_ramSize = 8192;		//8KB
	else if (ramHeaderSize == 0x03)
		_ramSize = 32768;		//32KB

	if (_ramSize)
		_memRamMBC = new BYTE[_ramSize];
	
	MBCLoadRam();
}

void InitMBC2(string romName, BYTE * memCartridge, int romSize)
{
	InitMBC(romName, memCartridge, romSize);

	_ramSize = 512;

	_memRamMBC = new BYTE[_ramSize];
	
	MBCLoadRam();
}

void InitMBC3(string romName, BYTE * memCartridge, int romSize, int ramHeaderSize)
{
	InitMBC(romName, memCartridge, romSize);

	if (ramHeaderSize == 0x01)
		_ramSize = 8192;		//8KB = 64Kb
	else if (ramHeaderSize == 0x02)
		_ramSize = 32768;		//32KB = 256Kb
	else if (ramHeaderSize == 0x03)
		_ramSize = 131072;		//128KB = 1Mb

	if (_ramSize)
		_memRamMBC = new BYTE[_ramSize];
	
	MBCLoadRam();
}

void InitMBC5(string romName, BYTE * memCartridge, int romSize, int ramHeaderSize)
{
	InitMBC1(romName, memCartridge, romSize, ramHeaderSize);
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
		_ramEnabled = ((value & 0x0F) == 0x0A);
		if (!_ramEnabled)
			MBCSaveRam();
	}
	else if (direction < 0x4000)	//Cambiar romBank
	{
		if ((value & 0x1F)== 0)
			value++;

		_romBank = (_romBank & 0x60) | (value & 0x1F);
	}
	else if (direction < 0x6000)	//Cambiar romBank o ramBank dependiendo del modo
	{
		if (!_memMode)	//Modo 16/8 (Seleccionar romBank)
		{
			_romBank = ((value & 0x03) << 5) | (_romBank & 0x1F);
		}
		else			//Modo 4/32 (Seleccionar ramBank)
		{
			_ramBank = value & 0x03;
		}
	}
	else if (direction < 0x8000)	//Seleccionar modo
	{
		_memMode = value & 0x01;
	}
	else if ((direction >=0xA000) && (direction < 0xC000))	//Intenta escribir en RAM
	{
		if (_ramEnabled)
			_memRamMBC[direction - 0xA000 + (0x2000*_ramBank)] = value;
	}
}

BYTE MBC1Read(WORD direction)
{
	if (direction < 0x4000)
		return _memCartridge[direction];
	else if (direction < 0x8000)
		return _memCartridge[(direction - 0x4000) + (0x4000*_romBank)];
	else if ((direction >=0xA000) && (direction < 0xC000) && _ramEnabled)
		return _memRamMBC[direction - 0xA000 + (0x2000*_ramBank)];

	return 0;
}

void MBC2Write(WORD direction, BYTE value)
{
	if (direction < 0x2000)	//Habilitar/Deshabilitar RAM
	{
		if (! (direction & 0x10))
			_ramEnabled = !_ramEnabled;
		
		if (!_ramEnabled)
			MBCSaveRam();
	}
	else if (direction < 0x4000)	//Cambiar romBank
	{
		if (!(direction & 0x100))
			return;

		if ((value & 0x0F)== 0)
			value++;

		_romBank = value & 0x0F;
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
		if (_ramEnabled)
			_memRamMBC[direction - 0xA000] = value & 0x0F;
		//throw GBException("Intenta escribir en RAM de cartucho");
	}
}

BYTE MBC2Read(WORD direction)
{
	if (direction < 0x4000)
		return _memCartridge[direction];
	else if (direction < 0x8000)
		return _memCartridge[(direction - 0x4000) + (0x4000*_romBank)];
	else if ((direction >=0xA000) && (direction < 0xC000) && (_ramEnabled))
		return _memRamMBC[direction - 0xA000];

	return 0;
}

void MBC3Write(WORD direction, BYTE value)
{
	if (direction < 0x2000)	//Habilitar/Deshabilitar RAM
	{
		_ramEnabled = ((value & 0x0F) == 0x0A);
		if (!_ramEnabled)
			MBCSaveRam();
	}
	else if (direction < 0x4000)	//Cambiar romBank
	{
		value = value ? value : 1;

		_romBank = value & 0x7F;
	}
	else if (direction < 0x6000)	//Cambiar ramBank o RTC
	{
		if (value < 4)			//(Seleccionar ramBank)
		{
			_ramBank = value;
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
		if (_ramEnabled)
			_memRamMBC[direction - 0xA000 + (0x2000*_ramBank)] = value;
	}
}

BYTE MBC3Read(WORD direction)
{
	if (direction < 0x4000)
		return _memCartridge[direction];
	else if (direction < 0x8000)
		return _memCartridge[(direction - 0x4000) + (0x4000*_romBank)];
	else if ((direction >=0xA000) && (direction < 0xC000) && (_ramEnabled))
		return _memRamMBC[direction - 0xA000 + (0x2000*_ramBank)];

	return 0;
}

void MBC5Write(WORD direction, BYTE value)
{
	if (direction < 0x2000)	//Habilitar/Deshabilitar RAM
	{
		_ramEnabled = ((value & 0x0F) == 0x0A);
		if (!_ramEnabled)
			MBCSaveRam();
	}
	else if (direction < 0x3000)	//Cambiar romBank
	{
		_romBank = (_romBank & 0x100) | value;
	}
	else if (direction < 0x4000)	//Cambiar romBank
	{
		_romBank = ((value & 0x01) << 8) | (_romBank & 0xFF);
	}
	else if (direction < 0x6000)	//Cambiar ramBank o RTC
	{
		_ramBank = value & 0x0F;
	}
	else if (direction < 0x8000)	
	{
		//throw GBException("Zona no conocida");
	}
	else if ((direction >=0xA000) && (direction < 0xC000))	//Intenta escribir en RAM
	{
		if (_ramEnabled)
			_memRamMBC[direction - 0xA000 + (0x2000*_ramBank)] = value;
	}
}

BYTE MBC5Read(WORD direction)
{
	if (direction < 0x4000)
		return _memCartridge[direction];
	else if (direction < 0x8000)
		return _memCartridge[(direction - 0x4000) + (0x4000*_romBank)];
	else if ((direction >=0xA000) && (direction < 0xC000) && (_ramEnabled))
		return _memRamMBC[direction - 0xA000 + (0x2000*_ramBank)];

	return 0;
}

void MBCLoadRam()
{
	string fileName = _pathBatteries + _romName + ".BATT";
	ifstream file(fileName.c_str(), ios::in|ios::binary);
	
	if (file)
	{
		file.read((char *)_memRamMBC, _ramSize);
		file.close();
	}
}

void MBCSaveRam()
{
	string fileName = _pathBatteries + _romName + ".BATT";
	ofstream file(fileName.c_str(), ios::out|ios::binary);
	
	if (file)
	{
		file.write((char *)_memRamMBC, _romSize);
		file.close();
	}
}

void MBCPathBatteries(string path)
{
	_pathBatteries = path;
}
