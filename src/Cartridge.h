#ifndef __CARTRIDGE_H__
#define __CARTRIDGE_H__

#include <string>
#include "Def.h"
#include "MBC.h"

#define CART_NAME		0x0134
#define CART_TYPE		0x0147
#define CART_ROM_SIZE	0x0148
#define CART_RAM_SIZE	0x0149

class Cartridge
{
private:
	unsigned int _RomSize;
	bool _isLoaded;
	BYTE * _memCartridge;

	BYTE (*ptrRead)(WORD);
	void (*ptrWrite)(WORD, BYTE);
	int CheckRomSize(int numHeaderSize, int fileSize);
public:
	char nameROM[17];
	Cartridge(std::string path);
	~Cartridge();
	void Print(int beg, int end);
	BYTE *GetData();
	unsigned int GetSize();
	inline BYTE Read(WORD direction) { return ptrRead(direction); };
	inline void Write(WORD direction, BYTE value) { ptrWrite(direction, value); };
	bool IsLoaded();
};

#endif
