#include "StdAfx.h"
#include "Memoria.h"
#include "Cartucho.h"

Memoria::Memoria()
{
	ResetMem();
}

Memoria::~Memoria()
{
}

Memoria *Memoria::GetPtrMemoria() {return this;}

void Memoria::CargarCartucho(Cartucho *c)
{
	memcpy(&memoria, c->GetData(), c->GetSize());
}

void Memoria::ResetMem()
{
	/*for (WORD i=0; i<(SIZE_MEM - 1); i++ )
		memoria[i] = 0x00;*/
	memset(&memoria, 0x00, SIZE_MEM);

	memoria[TIMA] = 0x00; //TIMA
    memoria[TMA]  = 0x00; //TMA
    memoria[TAC]  = 0x00; //TAC
    memoria[NR10] = 0x80; //NR10
    memoria[NR11] = 0xBF; //NR11
    memoria[NR12] = 0xF3; //NR12
    memoria[NR14] = 0xBF; //NR14
    memoria[NR21] = 0x3F; //NR21
    memoria[NR22] = 0x00; //NR22
    memoria[NR24] = 0xBF; //NR24
    memoria[NR30] = 0x7F; //NR30
    memoria[NR31] = 0xFF; //NR31
    memoria[NR32] = 0x9F; //NR32
    memoria[NR33] = 0xBF; //NR33
    memoria[NR41] = 0xFF; //NR41
    memoria[NR42] = 0x00; //NR42
    memoria[NR43] = 0x00; //NR43
    memoria[NR30] = 0xBF; //NR30
    memoria[NR50] = 0x77; //NR50
    memoria[NR51] = 0xF3; //NR51
    memoria[NR52] = 0xF1; //NR52
    memoria[LCDC] = 0x91; //LCDC
    memoria[STAT] = 0x02; //LCD_STAT
    memoria[SCY]  = 0x00; //SCY
    memoria[SCX]  = 0x00; //SCX
    memoria[LYC]  = 0x00; //LYC
    memoria[BGP]  = 0xFC; //BGP
    memoria[OBP0] = 0xFF; //OBP0
    memoria[OBP1] = 0xFF; //OBP1
    memoria[WY]   = 0x00; //WY
    memoria[WX]   = 0x00; //WX
    memoria[IE]   = 0x00; //IE

	//Puesto por mi
	memoria[P1]	  = 0x3F;
}

inline void Memoria::MemW(WORD direccion, BYTE value, bool checkDirAndValue)
{
	if (checkDirAndValue)
	{
		switch (direccion)
		{
			case DMA:
				//cout << "W DMA: 0x" << setfill('0') << setw(2) << uppercase << hex << (int)valor << endl;
				DmaTransfer(value);
				break;
			//case TIMA: cout << "W TIMA: 0x" << setfill('0') << setw(2) << uppercase << hex << (int)valor << endl; break;
			//case TMA: cout << "W TMA: 0x" << setfill('0') << setw(2) << uppercase << hex << (int)valor << endl; break;
			//case TAC: cout << "W TAC: 0x" << setfill('0') << setw(2) << uppercase << hex << (int)valor << endl; break;
			//case BGP: cout << "W BGP: 0x" << setfill('0') << setw(2) << uppercase << hex << (int)value << endl; break;
			case P1:
				//cout << "W P1: 0x" << setfill('0') << setw(2) << uppercase << hex << (int)value << endl;
				value = (value & 0x30) | (MemR(P1) & ~0x30);
				break;
			//case LCDC: cout << "W LCDC: 0x" << setfill('0') << setw(2) << uppercase << hex << (int)valor << endl; break;
			//case SCX: cout << "W SCX: 0x" << setfill('0') << setw(2) << uppercase << hex << (int)value << endl; break;
			//case SCY: cout << "W SCY: 0x" << setfill('0') << setw(2) << uppercase << hex << (int)value << endl; break;
			case STAT: value = (value & ~0x07) | (MemR(STAT) & 0x07); break;
			case LY:
			case DIV: value = 0; break;
			//case 0xC6E8: cout << "0xC6E8" << endl; break;
		}
		//if ((direccion >= 0x8000) && (direccion < 0xA000))
		//	cout << "W VRAM: 0x" << setfill('0') << setw(4) << uppercase << hex << (int)direccion << "=0x" << setfill('0') << setw(2) << uppercase << hex << (int)valor << endl;

		if ((direccion > 0xBFFF) && (direccion < 0xDE00))//C000-DDFF
			memoria[direccion + 0x2000] = value;
		if ((direccion > 0xDFFF) && (direccion < 0xFE00))//E000-FDFF
			memoria[direccion - 0x2000] = value;

		if (direccion < 0x8000)
		{
			//cout << "Error: Intentando escribir en la ROM. ";
			//cout << "0x" << setfill('0') << setw(4) << uppercase << hex << (int)direccion << "=0x" << setfill('0') << setw(2) << uppercase << hex << (int)valor << endl;
			return;
		}
	}//Fin if

	memoria[direccion] = value;
}

inline void Memoria::MemW(WORD direccion, BYTE value)
{
	MemW(direccion, value, true);
}

inline BYTE Memoria::MemR(WORD direccion)
{
	/*switch (direccion)
	{
		case DMA: cout << "R DMA\n"; break;
		//case TIMA: cout << "R TIMA\n"; break;
		//case TMA: cout << "R TMA\n"; break;
		//case DIV: cout << "R DIV\n"; break;
		//case TAC: cout << "R TAC\n"; break;
		//case BGP: cout << "R BGP\n"; break;
		//case P1: cout << "R P1: 0x" << setfill('0') << setw(2) << uppercase << hex << (int)memoria[direccion] << endl; break;
	}*/
	return memoria[direccion];
}

void Memoria::DmaTransfer(BYTE direccion)
{
	BYTE i;

	for (i=0; i<0xA0; i++)
		MemW(0xFE00 + i, MemR((direccion << 8) + i));
}