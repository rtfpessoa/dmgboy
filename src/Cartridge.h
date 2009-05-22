#ifndef __CARTRIDGE_H__
#define __CARTRIDGE_H__

#include "StdAfx.h"

class Cartridge
{
private:
	char * mem_cartridge;
	unsigned int size;

public:
	Cartridge(string path);
	~Cartridge();
	void Print(int beg, int end);
	char *GetData();
	unsigned int GetSize();
};

#endif