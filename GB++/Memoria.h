#ifndef __MEMORIA_H__
#define __MEMORIA_H__

#include "Cartucho.h"

#define SIZE_MEM 65536

class Memoria
{
private:
	BYTE memoria[SIZE_MEM];
public:
	Memoria();
	~Memoria();
	Memoria *GetPtrMemoria();
	void ResetMem();
	void CargarCartucho(Cartucho *c);
	void MemW(WORD direccion, BYTE value, bool checkDirAndValue);
	void MemW(WORD direccion, BYTE value);
	BYTE MemR(WORD direccion);
private:
	void DmaTransfer(BYTE direccion);
};

#endif