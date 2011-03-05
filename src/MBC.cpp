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
	ofstream file(fileName.c_str(), ios::out|ios::trunc|ios::binary);
	
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

void MBCSaveState(ofstream * file)
{
	file->write((char *)&_memMode, sizeof(int));
	file->write((char *)&_romBank, sizeof(int));
	file->write((char *)&_romSize, sizeof(int));
	file->write((char *)&_ramBank, sizeof(int));
	file->write((char *)&_ramSize, sizeof(int));
	file->write((char *)&_ramEnabled, sizeof(int));
	file->write((char *)_memRamMBC, _ramSize);
}

void MBCLoadState(ifstream * file)
{
	file->read((char *)&_memMode, sizeof(int));
	file->read((char *)&_romBank, sizeof(int));
	file->read((char *)&_romSize, sizeof(int));
	file->read((char *)&_ramBank, sizeof(int));
	file->read((char *)&_ramSize, sizeof(int));
	file->read((char *)&_ramEnabled, sizeof(int));
	file->read((char *)_memRamMBC, _ramSize);
}

/*
Game Boy MMM01 emulation
Author: byuu
Date: 2010-01-04


Games that use MMM01:
=====================
Momotarou Collection 2
Taito Variety Pack


Word of warning:
================
There are bad dumps of the above games floating around.
Verify correct dumps by examining in hex editor:

Momotarou Collection 2:
00000 = Momotarou Collection 2 [menu]
08000 = Momotarou Dengeki
88000 = Momotarou Gaiden

Taito Variety Pack:
00000 = Taito Variety Pack [menu]
08000 = Sagaia
28000 = Chase HQ
48000 = Bubble Bobble
68000 = Elevator Action

The bad dumps place the menu at the very end of the ROM.
If this is the case, header detection will fail.

Assuming you have a bad dump, you will have to fix it yourself.
Here is example code to fix Momotarou Collection 2:

#include <stdio.h>

int main() {
	FILE *fp = fopen("momo.gb", "rb");
	char g1[0x80000]; fread(g1, 1, 0x80000, fp);
	char g2[0x78000]; fread(g2, 1, 0x78000, fp);
	char g3[0x08000]; fread(g3, 1, 0x08000, fp);
	fclose(fp);
	
	fp = fopen("momo-out.gb", "wb");
	fwrite(g3, 1, 0x08000, fp);
	fwrite(g1, 1, 0x80000, fp);
	fwrite(g2, 1, 0x78000, fp);
	fclose(fp);
	
	return 0;
}


Detecting MMM01:
================
switch(romdata[0x0147]) {
		...
	case 0x0b: info.mapper = Mapper::MMM01; break;
	case 0x0c: info.mapper = Mapper::MMM01; info.ram = true; break;
	case 0x0d: info.mapper = Mapper::MMM01; info.ram = true; info.battery = true; break;
		...
}


Emulating MMM01:
================
The MMM01 is a meta-mapper, it allows one to map 0000-3fff.
Once a sub-game is mapped in, the MMM01 reverts to an ordinary
mapper, allowing mapping of 4000-7fff and a000-bfff.

Here is the source code for an MMM01 emulator.
I cannot guarantee its completeness.

struct MMM01 : MMIO {
	bool rom_mode;
	uint8 rom_base;
	
	bool ram_enable;
	uint8 rom_select;
	uint8 ram_select;
	
	uint8 mmio_read(uint16 addr);
	void mmio_write(uint16 addr, uint8 data);
	void power();
} mmm01;

uint8 Cartridge::MMM01::mmio_read(uint16 addr) {
	if((addr & 0x8000) == 0x0000) {
		if(rom_mode == 0) return cartridge.rom_read(addr);
	}
	
	if((addr & 0xc000) == 0x0000) {
		return cartridge.rom_read(0x8000 + (rom_base << 14) + (addr & 0x3fff));
	}
	
	if((addr & 0xc000) == 0x4000) {
		return cartridge.rom_read(0x8000 + (rom_base << 14) + (rom_select << 14) + (addr & 0x3fff));
	}
	
	if((addr & 0xe000) == 0xa000) {
		if(ram_enable) return cartridge.ram_read((ram_select << 13) + (addr & 0x1fff));
		return 0x00;
	}
	
	return 0x00;
}

void Cartridge::MMM01::mmio_write(uint16 addr, uint8 data) {
	if((addr & 0xe000) == 0x0000) {  //0000-1fff
		if(rom_mode == 0) {
			rom_mode = 1;
		} else {
			ram_enable = (data & 0x0f) == 0x0a;
		}
	}
	
	if((addr & 0xe000) == 0x2000) {  //2000-3fff
		if(rom_mode == 0) {
			rom_base = data & 0x3f;
		} else {
			rom_select = data;
		}
	}
	
	if((addr & 0xe000) == 0x4000) {  //4000-5fff
		if(rom_mode == 1) {
			ram_select = data;
		}
	}
	
	if((addr & 0xe000) == 0x6000) {  //6000-7fff
		//unknown purpose
	}
	
	if((addr & 0xe000) == 0xa000) {  //a000-bfff
		if(ram_enable) cartridge.ram_write((ram_select << 13) + (addr & 0x1fff), data);
	}
}

void Cartridge::MMM01::power() {
  rom_mode = 0;
  rom_base = 0x00;

  ram_enable = false;
  rom_select = 0x01;
  ram_select = 0x00;
}
*/