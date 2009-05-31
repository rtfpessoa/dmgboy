#ifndef __CARTRIDGE_H__
#define __CARTRIDGE_H__

#include <string>
#include "Def.h"
using namespace std;

#define CART_NAME		0x0134
#define CART_TYPE		0x0147
#define CART_ROM_SIZE	0x0148
#define CART_RAM_SIZE	0x0149

class Cartridge
{
private:
	BYTE * mem_cartridge;
	unsigned int size;
	bool _isLoaded;
	BYTE mbc;
	BYTE ROMBank;

	BYTE MBC1Read(WORD direction);
	void MBC1Write(WORD direction, BYTE value);
public:
	Cartridge(string path);
	~Cartridge();
	void Print(int beg, int end);
	BYTE *GetData();
	unsigned int GetSize();
	BYTE Read(WORD direction);
	void Write(WORD direction, BYTE value);
	bool isLoaded();
};

#endif
