#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "Cartridge.h"
#include "Pad.h"

#define SIZE_MEM 65536

class Memory
{
private:
	BYTE memory[SIZE_MEM];
	Cartridge *c;
private:
	void DmaTransfer(BYTE direction);
public:
	Memory();
	~Memory();
	Memory *GetPtrMemory();
	void ResetMem();
	void LoadCartridge(Cartridge *c);
	void MemW(WORD direction, BYTE value, bool checkDirAndValue);
	inline void MemW(WORD direction, BYTE value){ MemW(direction, value, true); };
	BYTE MemR(WORD direction);
};

#endif
