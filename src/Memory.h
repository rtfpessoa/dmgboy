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

#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "Cartridge.h"
#include "Pad.h"

#define SIZE_MEM 65536

class Memory
{
protected:
	Cartridge *c;
private:
	void DmaTransfer(BYTE direction);
public:
	BYTE memory[SIZE_MEM];
public:
	Memory();
	~Memory();
	Memory *GetPtrMemory();
	void ResetMem();
	void LoadCartridge(Cartridge *c);
	void MemW(WORD direction, BYTE value, bool checkDirAndValue);
	void MemW(WORD direction, BYTE value){ MemW(direction, value, true); };
	inline BYTE MemR(WORD direction)
	{
		if ((direction < 0x8000) || ((direction >=0xA000) && (direction < 0xC000)))
		{
			return c->Read(direction);
		}
		return memory[direction];
	}
};

#endif
