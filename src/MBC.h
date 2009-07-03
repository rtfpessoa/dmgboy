#ifndef __MBC_H__
#define __MBC_H__

#include "Def.h"

void InitMBC(BYTE * mem_cartridge, int ROMSize, int RamHeaderSize);

BYTE NoneRead(WORD direction);
void NoneWrite(WORD direction, BYTE value);

BYTE MBC1Read(WORD direction);
void MBC1Write(WORD direction, BYTE value);

#endif