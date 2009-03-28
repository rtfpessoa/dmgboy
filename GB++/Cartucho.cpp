#include "StdAfx.h"
#include "Cartucho.h"

Cartucho::Cartucho(string ruta)
{
	//string s = "Hola";
	ifstream::pos_type size;
	ifstream file (ruta.c_str(), ios::in|ios::binary|ios::ate);
	if (file.is_open())
	{
		size = file.tellg();
		this->size = size;
		mem_cartucho = new char [size];
		file.seekg (0, ios::beg);
		file.read (mem_cartucho, size);
		file.close();

		cout << ruta << ":\nArchivo cargado en memoria correctamente" << endl;
		char name[17];
		memcpy_s(name, 17, &mem_cartucho[CART_NAME], 17);
		name[16] = '\0';
		cout << "Nombre de cartucho: " << name << endl;
		cout << "Tamano de ROM:\t\t0x" << setfill('0') << setw(2) << uppercase << hex << (int)mem_cartucho[CART_ROM_SIZE] << endl;
		cout << "Tamano de RAM:\t\t0x" << setfill('0') << setw(2) << uppercase << hex << (int)mem_cartucho[CART_RAM_SIZE] << endl;
		cout << "Tipo de cartucho:\t0x" << setfill('0') << setw(2) << uppercase << hex << (int)mem_cartucho[CART_TYPE] << endl;

		//delete[] memblock;
	}
	else
		cout << ruta << ": Error al intentar abrir el archivo" << endl;
}

Cartucho::~Cartucho(void)
{
}

void Cartucho::Imprimir(int inicio, int fin)
{
	int i, j;
	unsigned char byte;
	
	for (i=inicio;i<=fin;i=i+16){
		cout << setfill('0') << setw(6) << hex << i << "h: ";
		for (j=i;j<i+16;j++){
			byte = this->mem_cartucho[j];
			cout << setfill('0') << setw(2) << hex << (int)byte << " ";
		}
		cout << endl;
	}
}

char *Cartucho::GetData()
{
	return mem_cartucho;
}

unsigned int Cartucho::GetSize()
{
	return size;
}