#include "Cartridge.h"
#include <fstream>
#include <iostream>
#include "Def.h"
#include <string.h>
#include <iomanip>
#include "GBException.h"

using namespace std;

Cartridge::Cartridge(string path)
{
	_memCartridge = NULL;
	int RamSize = 0;
	ifstream::pos_type size;
	ifstream file (path.c_str(), ios::in|ios::binary|ios::ate);
	if (file.is_open())
	{
		size = file.tellg();
		this->_RomSize = size;
		_memCartridge = new BYTE [size];
		file.seekg (0, ios::beg);
		file.read ((char *)_memCartridge, size);
		file.close();

		cout << path << ":\nArchivo cargado en memoria correctamente" << endl;
		char name[17];
		memcpy(name, &_memCartridge[CART_NAME], 17);
		name[16] = '\0';
		cout << "Nombre de cartucho: " << name << endl;
		cout << "Tamano de ROM:\t\t0x" << setfill('0') << setw(2) << uppercase << hex << (int)_memCartridge[CART_ROM_SIZE] << endl;
		cout << "Tamano de RAM:\t\t0x" << setfill('0') << setw(2) << uppercase << hex << (int)_memCartridge[CART_RAM_SIZE] << endl;
		cout << "Tipo de cartucho:\t0x" << setfill('0') << setw(2) << uppercase << hex << (int)_memCartridge[CART_TYPE] << " (";

		CheckRomSize((int)_memCartridge[CART_ROM_SIZE], _RomSize);

		switch(_memCartridge[CART_TYPE])
		{
		case 0x00:						//ROM ONLY
		case 0x08:						//ROM+RAM
		case 0x09:						//ROM+RAM+BATTERY
			cout << "No MBC)" << endl;
			ptrRead = &NoneRead;
			ptrWrite = &NoneWrite;
			InitMBCNone(_memCartridge, _RomSize);
			break;
		case 0x01:						//ROM+MBC1 
		case 0x02:						//ROM+MBC1+RAM 
		case 0x03:						//ROM+MBC1+RAM+BATT
			cout << "MBC1)" << endl;
			ptrRead = &MBC1Read;
			ptrWrite = &MBC1Write;
			InitMBC1(_memCartridge, _RomSize, _memCartridge[CART_RAM_SIZE]);
			break;
		case 0x05:						//ROM+MBC2 
		case 0x06:						//ROM+MBC2+BATTERY
			cout << "MBC2)" << endl;
			ptrRead = &MBC2Read;
			ptrWrite = &MBC2Write;
			InitMBC2(_memCartridge, _RomSize);
			break;
		/*
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
		case 0xFF: mbc = HuC1; break;	//Hudson HuC-1*/
		default: throw GBException("MBC no implementado todavia");
		}
		_isLoaded = true;
	}
	else
	{
		cout << path << ": Error al intentar abrir el archivo" << endl;
		_isLoaded = false;
	}
}

Cartridge::~Cartridge(void)
{
	DestroyMBC();
	if (_memCartridge)
		delete [] _memCartridge;
}

int Cartridge::CheckRomSize(int numHeaderSize, int fileSize)
{
	int headerSize = 32768 << (numHeaderSize & 0x0F);
	if (numHeaderSize & 0xF0)
		headerSize += (32768 << ((numHeaderSize & 0xF0) >> 0x04));
	if (headerSize != fileSize)
	{
		cout << "No coincide la cabecera con el tamaño del fichero" << endl;
		return 0;
	}
	else
		return 1;
}

BYTE *Cartridge::GetData()
{
	return _memCartridge;
}

unsigned int Cartridge::GetSize()
{
	return _RomSize;
}

bool Cartridge::IsLoaded()
{
	return _isLoaded;
}

BYTE Cartridge::Read(WORD direction)
{
	return ptrRead(direction);
}

void Cartridge::Write(WORD direction, BYTE value)
{
	ptrWrite(direction, value);
}