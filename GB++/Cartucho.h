#ifndef __CARTUCHO_H__
#define __CARTUCHO_H__

#include "StdAfx.h"

class Cartucho
{
private:
	char * mem_cartucho;
	unsigned int size;

public:
	Cartucho(string ruta);
	~Cartucho();
	void Imprimir(int inicio, int fin);
	char *GetData();
	unsigned int GetSize();
};

#endif