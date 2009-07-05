#ifndef __MBC_H__
#define __MBC_H__

#include "Def.h"

void InitMBCNone(BYTE * mem_cartridge, int ROMSize);
void InitMBC1(BYTE * mem_cartridge, int ROMSize, int RamHeaderSize);
void InitMBC2(BYTE * mem_cartridge, int ROMSize);
void DestroyMBC();

BYTE NoneRead(WORD direction);
void NoneWrite(WORD direction, BYTE value);

BYTE MBC1Read(WORD direction);
void MBC1Write(WORD direction, BYTE value);

BYTE MBC2Read(WORD direction);
void MBC2Write(WORD direction, BYTE value);

#endif