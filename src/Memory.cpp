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
#include "Cartridge.h"
#include "Sound.h"
#include "Pad.h"
#include "Memory.h"
#include "CPU.h"

using namespace std;

Memory::Memory(CPU * cpu, Sound * s)
{
    this->cpu = cpu;
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

	memory[TIMA] = 0x00;
    memory[TMA]  = 0x00;
    memory[TAC]  = 0x00;
	
	memory[NR10]  = 0x80;
	memory[NR11]  = 0xBF;
	memory[NR12]  = 0xF3;
	memory[NR14]  = 0xBF;
	memory[NR21]  = 0x3F;
	memory[NR22]  = 0x00;
	memory[NR24]  = 0xBF;
	memory[NR30]  = 0x7F;
	memory[NR31]  = 0xFF;
	memory[NR32]  = 0x9F;
	memory[NR33]  = 0xBF;
	memory[NR41]  = 0xFF;
	memory[NR42]  = 0x00;
	memory[NR43]  = 0x00;
	memory[NR30]  = 0xBF;
	memory[NR50]  = 0x77;
	memory[NR51]  = 0xF3;
	memory[NR52]  = 0xF1;
	
	if (s)
	{
		s->WriteRegister(NR10, 0x80);
		s->WriteRegister(NR11, 0xBF);
		s->WriteRegister(NR12, 0xF3);
		s->WriteRegister(NR14, 0xBF);
		s->WriteRegister(NR21, 0x3F);
		s->WriteRegister(NR22, 0x00);
		s->WriteRegister(NR24, 0xBF);
		s->WriteRegister(NR30, 0x7F);
		s->WriteRegister(NR31, 0xFF);
		s->WriteRegister(NR32, 0x9F);
		s->WriteRegister(NR33, 0xBF);
		s->WriteRegister(NR41, 0xFF);
		s->WriteRegister(NR42, 0x00);
		s->WriteRegister(NR43, 0x00);
		s->WriteRegister(NR30, 0xBF);
		s->WriteRegister(NR50, 0x77);
		s->WriteRegister(NR51, 0xF3);
		s->WriteRegister(NR52, 0xF1);
	}
	
    memory[LCDC] = 0x91;
    memory[SCY]  = 0x00;
    memory[SCX]  = 0x00;
    memory[LYC]  = 0x00;
    memory[BGP]  = 0xFC;
    memory[OBP0] = 0xFF;
    memory[OBP1] = 0xFF;
    memory[WY]   = 0x00;
    memory[WX]   = 0x00;
    memory[IE]   = 0x00;
	
	memory[STAT] = 0x05;
}

void Memory::MemW(WORD address, BYTE value)
{
	if ((address < 0x8000) || ((address >= 0xA000)&&(address < 0xC000)))
	{
		c->Write(address, value);
		return;
	}
	else if ((address >= 0xC000) && (address < 0xDE00))//C000-DDFF
		memory[address + 0x2000] = value;
	else if ((address >= 0xE000) && (address < 0xFE00))//E000-FDFF
		memory[address - 0x2000] = value;
	else if ((address >= 0xFF10) && (address <= 0xFF3F))
        s->WriteRegister(address, value);
	else
	{
		switch (address)
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
            case LCDC: cpu->OnWriteLCDC(value); return;
            //case IF: value = (memory[IF] & 0xF0) | (value & 0x0F); break;
		}
	}

	memory[address] = value;
}

void Memory::DmaTransfer(BYTE address)
{
	BYTE i;

	for (i=0; i<0xA0; i++)
		MemWNoCheck(0xFE00 + i, MemR((address << 8) + i));
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
