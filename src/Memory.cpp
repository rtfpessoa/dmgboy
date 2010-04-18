#include "Memory.h"
#include "Cartridge.h"

Memory::Memory()
{
	ResetMem();
}

Memory::~Memory()
{
}

Memory *Memory::GetPtrMemory() {return this;}

void Memory::LoadCartridge(Cartridge *c)
{
	this->c = c;
}

void Memory::ResetMem()
{
	/*for (WORD i=0; i<(SIZE_MEM - 1); i++ )
		memory[i] = 0x00;*/
	memset(&memory, 0x00, SIZE_MEM);

	memory[TIMA] = 0x00; //TIMA
    memory[TMA]  = 0x00; //TMA
    memory[TAC]  = 0x00; //TAC
    memory[NR10] = 0x80; //NR10
    memory[NR11] = 0xBF; //NR11
    memory[NR12] = 0xF3; //NR12
    memory[NR14] = 0xBF; //NR14
    memory[NR21] = 0x3F; //NR21
    memory[NR22] = 0x00; //NR22
    memory[NR24] = 0xBF; //NR24
    memory[NR30] = 0x7F; //NR30
    memory[NR31] = 0xFF; //NR31
    memory[NR32] = 0x9F; //NR32
    memory[NR33] = 0xBF; //NR33
    memory[NR41] = 0xFF; //NR41
    memory[NR42] = 0x00; //NR42
    memory[NR43] = 0x00; //NR43
    memory[NR30] = 0xBF; //NR30
    memory[NR50] = 0x77; //NR50
    memory[NR51] = 0xF3; //NR51
    memory[NR52] = 0xF1; //NR52
    memory[LCDC] = 0x91; //LCDC
    memory[STAT] = 0x02; //LCD_STAT
    memory[SCY]  = 0x00; //SCY
    memory[SCX]  = 0x00; //SCX
    memory[LYC]  = 0x00; //LYC
    memory[BGP]  = 0xFC; //BGP
    memory[OBP0] = 0xFF; //OBP0
    memory[OBP1] = 0xFF; //OBP1
    memory[WY]   = 0x00; //WY
    memory[WX]   = 0x00; //WX
    memory[IE]   = 0x00; //IE

	//Puesto por mi
	//memory[P1]	  = 0x3F;
}

void Memory::MemW(WORD direction, BYTE value, bool checkDirAndValue)
{
	if (checkDirAndValue)
	{
		switch (direction)
		{
			case DMA:
				DmaTransfer(value);
				break;
			case P1:
				BYTE oldP1;
				oldP1 = memory[P1];
				value = (value & 0x30) | (oldP1 & ~0x30);
				value = updateInput(value);
				if ((value != oldP1) && ((value & 0x0F) != 0x0F))
				{
					//Debe producir una interrupcion
					memory[IF] |=  0x10;
				}
				break;
			//case LCDC: cout << "W LCDC: 0x" << setfill('0') << setw(2) << uppercase << hex << (int)valor << endl; break;
			//case SCX: cout << "W SCX: 0x" << setfill('0') << setw(2) << uppercase << hex << (int)value << endl; break;
			//case SCY: cout << "W SCY: 0x" << setfill('0') << setw(2) << uppercase << hex << (int)value << endl; break;
			case STAT: value = (value & ~0x07) | (memory[STAT] & 0x07); break;
			case LY:
			case DIV: value = 0; break;
			//case 0xC6E8: cout << "0xC6E8" << endl; break;
		}

		if ((direction >= 0xC000) && (direction < 0xDE00))//C000-DDFF
			memory[direction + 0x2000] = value;
		if ((direction >= 0xE000) && (direction < 0xFE00))//E000-FDFF
			memory[direction - 0x2000] = value;

		if ((direction < 0x8000) || ((direction >= 0xA000)&&(direction < 0xC000)))
		{
			c->Write(direction, value);
			return;
		}
	}//Fin if

	memory[direction] = value;
}

/*BYTE Memory::MemR(WORD direction)
{
	//switch (direction)
	//{
		//case DMA: cout << "R DMA\n"; break;
		//case TIMA: cout << "R TIMA\n"; break;
		//case TMA: cout << "R TMA\n"; break;
		//case DIV: cout << "R DIV\n"; break;
		//case TAC: cout << "R TAC\n"; break;
		//case BGP: cout << "R BGP\n"; break;
		//case P1: cout << "R P1: 0x" << setfill('0') << setw(2) << uppercase << hex << (int)memory[direction] << endl; break;
	//}
	if ((direction < 0x8000) || ((direction >=0xA000) && (direction < 0xC000)))
	{
		return c->Read(direction);
	}
	return memory[direction];
}*/

void Memory::DmaTransfer(BYTE direction)
{
	BYTE i;

	for (i=0; i<0xA0; i++)
		MemW(0xFE00 + i, MemR((direction << 8) + i));
}
