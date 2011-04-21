/*
 This file is part of gbpablog.

 gbpablog is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 gbpablog is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with gbpablog.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string.h>
#include "Memory.h"
#include "Cartridge.h"

using namespace std;

Memory::Memory(Sound * s)
{
	this->c = NULL;
	this->s = s;
	ResetMem();
}

Memory::~Memory()
{
}

Memory *Memory::GetPtrMemory() { return this; }

void Memory::LoadCartridge(Cartridge *c)
{
	this->c = c;
}

void Memory::ResetMem()
{
	memset(&memory, 0x00, SIZE_MEM);

	memory[TIMA] = 0x00; //TIMA
    memory[TMA]  = 0x00; //TMA
    memory[TAC]  = 0x00; //TAC
	
	memory[NR10]  = 0x80; //NR10
	memory[NR11]  = 0xBF; //NR11
	memory[NR12]  = 0xF3; //NR12
	memory[NR14]  = 0xBF; //NR14
	memory[NR21]  = 0x3F; //NR21
	memory[NR22]  = 0x00; //NR22
	memory[NR24]  = 0xBF; //NR24
	memory[NR30]  = 0x7F; //NR30
	memory[NR31]  = 0xFF; //NR31
	memory[NR32]  = 0x9F; //NR32
	memory[NR33]  = 0xBF; //NR33
	memory[NR41]  = 0xFF; //NR41
	memory[NR42]  = 0x00; //NR42
	memory[NR43]  = 0x00; //NR43
	memory[NR30]  = 0xBF; //NR30
	memory[NR50]  = 0x77; //NR50
	memory[NR51]  = 0xF3; //NR51
	memory[NR52]  = 0xF1; //NR52
	
	if (s)
	{
		s->WriteRegister(NR10, 0x80); //NR10
		s->WriteRegister(NR11, 0xBF); //NR11
		s->WriteRegister(NR12, 0xF3); //NR12
		s->WriteRegister(NR14, 0xBF); //NR14
		s->WriteRegister(NR21, 0x3F); //NR21
		s->WriteRegister(NR22, 0x00); //NR22
		s->WriteRegister(NR24, 0xBF); //NR24
		s->WriteRegister(NR30, 0x7F); //NR30
		s->WriteRegister(NR31, 0xFF); //NR31
		s->WriteRegister(NR32, 0x9F); //NR32
		s->WriteRegister(NR33, 0xBF); //NR33
		s->WriteRegister(NR41, 0xFF); //NR41
		s->WriteRegister(NR42, 0x00); //NR42
		s->WriteRegister(NR43, 0x00); //NR43
		s->WriteRegister(NR30, 0xBF); //NR30
		s->WriteRegister(NR50, 0x77); //NR50
		s->WriteRegister(NR51, 0xF3); //NR51
		s->WriteRegister(NR52, 0xF1); //NR52
	}
	
    memory[LCDC] = 0x91; //LCDC
    memory[SCY]  = 0x00; //SCY
    memory[SCX]  = 0x00; //SCX
    memory[LYC]  = 0x00; //LYC
    memory[BGP]  = 0xFC; //BGP
    memory[OBP0] = 0xFF; //OBP0
    memory[OBP1] = 0xFF; //OBP1
    memory[WY]   = 0x00; //WY
    memory[WX]   = 0x00; //WX
    memory[IE]   = 0x00; //IE
	
	memory[STAT] = 0x02; //LCD_STAT
}

void Memory::MemW(WORD direction, BYTE value)
{
	if ((direction < 0x8000) || ((direction >= 0xA000)&&(direction < 0xC000)))
	{
		c->Write(direction, value);
		return;
	}
	else if ((direction >= 0xC000) && (direction < 0xDE00))//C000-DDFF
		memory[direction + 0x2000] = value;
	else if ((direction >= 0xE000) && (direction < 0xFE00))//E000-FDFF
		memory[direction - 0x2000] = value;
	else if ((direction >= 0xFF10) && (direction <= 0xFF3F))
	{
		if(s)
			s->WriteRegister(direction, value);
	}
	else
	{
		switch (direction)
		{
			case DMA:
				DmaTransfer(value);
				break;
			case P1:
				BYTE oldP1;
				oldP1 = memory[P1];
				value = (value & 0xF0) | (oldP1 & ~0xF0);
				value = PadUpdateInput(value);
				if ((value != oldP1) && ((value & 0x0F) != 0x0F))
				{
					//Debe producir una interrupcion
					memory[IF] |=  0x10;
				}
				break;
			case STAT: value = (value & ~0x07) | (memory[STAT] & 0x07); break;
			case LY:
			case DIV: value = 0; break;
		}
	}

	memory[direction] = value;
}

void Memory::DmaTransfer(BYTE direction)
{
	BYTE i;

	for (i=0; i<0xA0; i++)
		MemWNoCheck(0xFE00 + i, MemR((direction << 8) + i));
}

void Memory::SaveMemory(ofstream * file)
{
	file->write((char *)&memory[0x8000], 0x8000);
}

void Memory::LoadMemory(ifstream * file)
{
	file->read((char *)&memory[0x8000], 0x8000);
	if (s)
	{
		for (int dir=0xFF10; dir<0xFF40; dir++)
			s->WriteRegister(dir, memory[dir]);
	}
	
}
