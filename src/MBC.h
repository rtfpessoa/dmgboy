#ifndef __MBC_H__
#define __MBC_H__

#include "Def.h"

void SetMBCMemCart(BYTE * mem_cartridge);

BYTE NoneRead(WORD direction);
void NoneWrite(WORD direction, BYTE value);

BYTE MBC1Read(WORD direction);
void MBC1Write(WORD direction, BYTE value);

#endif