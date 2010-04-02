#ifndef __MBC_H__
#define __MBC_H__

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

#endif