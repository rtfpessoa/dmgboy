#include "Cartridge.h"
#include <fstream>
#include <iostream>
#include "Def.h"
#include <string.h>
#include <iomanip>
#include "GBException.h"

enum e_MBC { None, MBC1, MBC2, MBC3, MBC5, MMM01, HuC1, RumbleCart, Other};

Cartridge::Cartridge(string path)
{
	ifstream::pos_type size;
	ifstream file (path.c_str(), ios::in|ios::binary|ios::ate);
	if (file.is_open())
	{
		size = file.tellg();
		this->size = size;
		mem_cartridge = new BYTE [size];
		file.seekg (0, ios::beg);
		file.read ((char *)mem_cartridge, size);
		file.close();

		cout << path << ":\nArchivo cargado en memoria correctamente" << endl;
		char name[17];
		memcpy(name, &mem_cartridge[CART_NAME], 17);
		name[16] = '\0';
		cout << "Nombre de cartucho: " << name << endl;
		cout << "Tamano de ROM:\t\t0x" << setfill('0') << setw(2) << uppercase << hex << (int)mem_cartridge[CART_ROM_SIZE] << endl;
		cout << "Tamano de RAM:\t\t0x" << setfill('0') << setw(2) << uppercase << hex << (int)mem_cartridge[CART_RAM_SIZE] << endl;
		cout << "Tipo de cartucho:\t0x" << setfill('0') << setw(2) << uppercase << hex << (int)mem_cartridge[CART_TYPE] << endl;

		switch(mem_cartridge[CART_TYPE])
		{
		case 0x00:						//ROM ONLY
		case 0x08:						//ROM+RAM
		case 0x09: mbc = None; break;	//ROM+RAM+BATTERY 
		case 0x01:						//ROM+MBC1 
		case 0x02:						//ROM+MBC1+RAM 
		case 0x03: mbc = MBC1; break;	//ROM+MBC1+RAM+BATT 
		case 0x05:						//ROM+MBC2 
		case 0x06: mbc = MBC2; break;	//ROM+MBC2+BATTERY
		case 0x0B:						//ROM+MMM01
		case 0x0C:						//ROM+MMM01+SRAM
		case 0x0D: mbc = MMM01; break;	//ROM+MMM01+SRAM+BATT
		case 0x0F:						//ROM+MBC3+TIMER+BATT
		case 0x10:						//ROM+MBC3+TIMER+RAM+BATT
		case 0x11:						//ROM+MBC3
		case 0x12:						//ROM+MBC3+RAM
		case 0x13: mbc = MBC3; break;	//ROM+MBC3+RAM+BATT
		case 0x19:						//ROM+MBC5
		case 0x1A:						//ROM+MBC5+RAM
		case 0x1B:						//ROM+MBC5+RAM+BATT
		case 0x1C:						//ROM+MBC5+RUMBLE
		case 0x1D:						//ROM+MBC5+RUMBLE+SRAM
		case 0x1E: mbc = MBC5; break;	//ROM+MBC5+RUMBLE+SRAM+BATT
		case 0x1F:						//Pocket Camera
		case 0xFD:						//Bandai TAMA5
		case 0xFE: mbc = Other; break;	//Hudson HuC-3
		case 0xFF: mbc = HuC1; break;	//Hudson HuC-1
		}

		_isLoaded = true;
	}
	else
	{
		cout << path << ": Error al intentar abrir el archivo" << endl;
		_isLoaded = false;
	}
	ROMBank = 1;
}

Cartridge::~Cartridge(void)
{
}

void Cartridge::Print(int beg, int end)
{
	int i, j;
	BYTE byte;

	for (i=beg;i<=end;i=i+16){
		cout << setfill('0') << setw(6) << hex << i << "h: ";
		for (j=i;j<i+16;j++){
			byte = this->mem_cartridge[j];
			cout << setfill('0') << setw(2) << hex << (int)byte << " ";
		}
		cout << endl;
	}
}

BYTE *Cartridge::GetData()
{
	return mem_cartridge;
}

unsigned int Cartridge::GetSize()
{
	return size;
}

bool Cartridge::isLoaded()
{
	return _isLoaded;
}

BYTE Cartridge::Read(WORD direction)
{
	switch(mbc)
	{
		case None: return mem_cartridge[direction];
		case MBC1: return MBC1Read(direction);
		default:
			throw GBException("The type of MBC not yet suported");
	}
}

void Cartridge::Write(WORD direction, BYTE value)
{
	if ((direction >=0x2000)&&(direction < 0x4000))
	{
		//Habria que tener en cuenta tambien los bits superiores (4000-5FFF)
		if ((value & 0x1F)== 0)
			value++;

		ROMBank = value & 0x1F;
	}
}

BYTE Cartridge::MBC1Read(WORD direction)
{
	if (direction < 0x4000)
		return mem_cartridge[direction];
	else
		return mem_cartridge[(direction - 0x4000) + (0x4000*ROMBank)];
}