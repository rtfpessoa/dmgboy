#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "Cartridge.h"
#include "Pad.h"

#define SIZE_MEM 65536

class Memory
{
private:
	BYTE memory[SIZE_MEM];
	Pad *p;
public:
	Memory();
	~Memory();
	Memory *GetPtrMemory();
	void SetPad(Pad *p);
	void ResetMem();
	void LoadCartridge(Cartridge *c);
	void MemW(WORD direction, BYTE value, bool checkDirAndValue);
	void MemW(WORD direction, BYTE value);
	BYTE MemR(WORD direction);
private:
	void DmaTransfer(BYTE direction);
};

#endif