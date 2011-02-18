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

#ifndef __MBC_H__
#define __MBC_H__

#include <string>
#include "Def.h"

void InitMBCNone(char * nameROM, BYTE * mem_cartridge, int ROMSize);
void InitMBC1(char * nameROM, BYTE * mem_cartridge, int ROMSize, int RamHeaderSize);
void InitMBC2(char * nameROM, BYTE * mem_cartridge, int ROMSize);
void InitMBC3(char * nameROM, BYTE * mem_cartridge, int ROMSize, int RamHeaderSize);
void InitMBC5(char * nameROM, BYTE * mem_cartridge, int ROMSize, int RamHeaderSize);
void DestroyMBC();

BYTE NoneRead(WORD direction);
void NoneWrite(WORD direction, BYTE value);

BYTE MBC1Read(WORD direction);
void MBC1Write(WORD direction, BYTE value);

BYTE MBC2Read(WORD direction);
void MBC2Write(WORD direction, BYTE value);

BYTE MBC3Read(WORD direction);
void MBC3Write(WORD direction, BYTE value);

BYTE MBC5Read(WORD direction);
void MBC5Write(WORD direction, BYTE value);

void MBCPathBatteries(std::string path);

#endif
