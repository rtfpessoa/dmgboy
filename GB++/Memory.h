#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "Cartridge.h"

#define SIZE_MEM 65536

class Memory
{
private:
	BYTE memory[SIZE_MEM];
public:
	Memory();
	~Memory();
	Memory *GetPtrMemory();
	void ResetMem();
	void CargarCartridge(Cartridge *c);
	void MemW(WORD direccion, BYTE value, bool checkDirAndValue);
	void MemW(WORD direccion, BYTE value);
	BYTE MemR(WORD direccion);
private:
	void DmaTransfer(BYTE direccion);
};

#endif