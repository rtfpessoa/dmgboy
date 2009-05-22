#include "StdAfx.h"
#include "Cartridge.h"

Cartridge::Cartridge(string path)
{
	//string s = "Hola";
	ifstream::pos_type size;
	ifstream file (path.c_str(), ios::in|ios::binary|ios::ate);
	if (file.is_open())
	{
		size = file.tellg();
		this->size = size;
		mem_cartridge = new char [size];
		file.seekg (0, ios::beg);
		file.read (mem_cartridge, size);
		file.close();

		cout << path << ":\nArchivo cargado en memoria correctamente" << endl;
		char name[17];
		memcpy_s(name, 17, &mem_cartridge[CART_NAME], 17);
		name[16] = '\0';
		cout << "Nombre de cartucho: " << name << endl;
		cout << "Tamano de ROM:\t\t0x" << setfill('0') << setw(2) << uppercase << hex << (int)mem_cartridge[CART_ROM_SIZE] << endl;
		cout << "Tamano de RAM:\t\t0x" << setfill('0') << setw(2) << uppercase << hex << (int)mem_cartridge[CART_RAM_SIZE] << endl;
		cout << "Tipo de cartucho:\t0x" << setfill('0') << setw(2) << uppercase << hex << (int)mem_cartridge[CART_TYPE] << endl;

		//delete[] memblock;
	}
	else
		cout << path << ": Error al intentar abrir el archivo" << endl;
}

Cartridge::~Cartridge(void)
{
}

void Cartridge::Print(int beg, int end)
{
	int i, j;
	unsigned char byte;
	
	for (i=beg;i<=end;i=i+16){
		cout << setfill('0') << setw(6) << hex << i << "h: ";
		for (j=i;j<i+16;j++){
			byte = this->mem_cartridge[j];
			cout << setfill('0') << setw(2) << hex << (int)byte << " ";
		}
		cout << endl;
	}
}

char *Cartridge::GetData()
{
	return mem_cartridge;
}

unsigned int Cartridge::GetSize()
{
	return size;
}