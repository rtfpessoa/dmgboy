/*
 This file is part of DMGBoy.

 DMGBoy is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 DMGBoy is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with DMGBoy.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "CPU.h"
#include "Instructions.h"
#include <iomanip>
#include <fstream>
#include <sstream>
#include "Registers.h"
#include "Video.h"
#include "Pad.h"
#include "GBException.h"

#ifdef MAKEGBLOG
	#include "Log.h"
#endif

using namespace std;

const BYTE instructionCycles[] = {
    1, 3, 2, 2, 1, 1, 2, 1, 5, 2, 2, 2, 1, 1, 2, 1,
    0, 3, 2, 2, 1, 1, 2, 1, 3, 2, 2, 2, 1, 1, 2, 1,
    2, 3, 2, 2, 1, 1, 2, 1, 2, 2, 2, 2, 1, 1, 2, 1,
    2, 3, 2, 2, 3, 3, 3, 1, 2, 2, 2, 2, 1, 1, 2, 1,
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
    2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 2, 1,
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
    2, 3, 3, 4, 3, 4, 2, 4, 2, 4, 3, 0, 3, 6, 2, 4,
    2, 3, 3, 0, 3, 4, 2, 4, 2, 4, 3, 0, 3, 0, 2, 4,
    3, 3, 2, 0, 0, 4, 2, 4, 4, 1, 4, 0, 0, 0, 2, 4,
    3, 3, 2, 1, 0, 4, 2, 4, 3, 2, 4, 1, 0, 0, 2, 4
};

const BYTE instructionCondicionalCycles[] = {
    1, 3, 2, 2, 1, 1, 2, 1, 5, 2, 2, 2, 1, 1, 2, 1,
    0, 3, 2, 2, 1, 1, 2, 1, 3, 2, 2, 2, 1, 1, 2, 1,
    3, 3, 2, 2, 1, 1, 2, 1, 3, 2, 2, 2, 1, 1, 2, 1,
    3, 3, 2, 2, 3, 3, 3, 1, 3, 2, 2, 2, 1, 1, 2, 1,
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
    2, 2, 2, 2, 2, 2, 0, 2, 1, 1, 1, 1, 1, 1, 2, 1,
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
    5, 3, 4, 4, 6, 4, 2, 4, 5, 4, 4, 0, 6, 6, 2, 4,
    5, 3, 4, 0, 6, 4, 2, 4, 5, 4, 4, 0, 6, 0, 2, 4,
    3, 3, 2, 0, 0, 4, 2, 4, 4, 1, 4, 0, 0, 0, 2, 4,
    3, 3, 2, 1, 0, 4, 2, 4, 3, 2, 4, 1, 0, 0, 2, 4
};

const BYTE instructionCyclesCB[] = {
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
    2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2,
    2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2,
    2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2,
    2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2,
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2
};

CPU::CPU(Video *v, Sound * s): Memory(this, s)
{
	Init(v);
}

CPU::CPU(Video *v, Cartridge *c, Sound * s): Memory(this, s)
{
    LoadCartridge(c);
	Init(v);
}

void CPU::Init(Video *v)
{
    this->v = v;
	v->SetMem(this->GetPtrMemory());
	ResetGlobalVariables();

#ifdef MAKEGBLOG
	this->log = new QueueLog(1000000);
#endif
}


CPU::~CPU() {}

void CPU::ResetGlobalVariables()
{
    numInstructions = 0;
    cyclesLCD = 0;
	bitSerial = -1;
	cyclesTimer = 0;
	cyclesDIV = 0;
	cyclesSerial = 0;
	frameCompleted = false;
	VBlankIntPending = false;
    newInterrupt = false;
    colorMode = false;
    if (c && ((MemR(CART_COLOR) == 0x80) || (MemR(CART_COLOR) == 0xC0)))
        colorMode = true;
    v->SetColorMode(colorMode);

    lcdMode0 = LCD_MODE_0;
    lcdMode1 = LCD_MODE_1;
    lcdMode2 = LCD_MODE_2;
    lcdMode3 = LCD_MODE_3;
    cyclesFrame = FRAME_CYCLES;
}

void CPU::Reset()
{
    ResetGlobalVariables();
	ResetRegs();
	ResetMem();
	v->ClearScreen();
	if (s && (s->GetEnabled()))
	{
		s->Stop();
		s->Start();
	}
}


void CPU::ExecuteOneFrame()
{
  if (!this->c)
    return;

  actualCycles = 0;
  BYTE OpCode = 0, NextOpcode = 0, lastOpCode = 0;

  #ifdef INSTLOG
    ofstream logFile;
    logFile.open ("/tmp/dmgboy.log");
  #endif

  Instructions inst(this->GetPtrRegisters(), this->GetPtrMemory());
  void *ptr;
  int tmpCycles;

  void *dispatch[] = {
    &&_0x00, &&_0x01, &&_0x02, &&_0x03, &&_0x04, &&_0x05, &&_0x06, &&_0x07, &&_0x08, &&_0x09, &&_0x0A, &&_0x0B, &&_0x0C, &&_0x0D, &&_0x0E, &&_0x0F,
    &&_0x10, &&_0x11, &&_0x12, &&_0x13, &&_0x14, &&_0x15, &&_0x16, &&_0x17, &&_0x18, &&_0x19, &&_0x1A, &&_0x1B, &&_0x1C, &&_0x1D, &&_0x1E, &&_0x1F,
    &&_0x20, &&_0x21, &&_0x22, &&_0x23, &&_0x24, &&_0x25, &&_0x26, &&_0x27, &&_0x28, &&_0x29, &&_0x2A, &&_0x2B, &&_0x2C, &&_0x2D, &&_0x2E, &&_0x2F,
    &&_0x30, &&_0x31, &&_0x32, &&_0x33, &&_0x34, &&_0x35, &&_0x36, &&_0x37, &&_0x38, &&_0x39, &&_0x3A, &&_0x3B, &&_0x3C, &&_0x3D, &&_0x3E, &&_0x3F,
    &&_0x40, &&_0x41, &&_0x42, &&_0x43, &&_0x44, &&_0x45, &&_0x46, &&_0x47, &&_0x48, &&_0x49, &&_0x4A, &&_0x4B, &&_0x4C, &&_0x4D, &&_0x4E, &&_0x4F,
    &&_0x50, &&_0x51, &&_0x52, &&_0x53, &&_0x54, &&_0x55, &&_0x56, &&_0x57, &&_0x58, &&_0x59, &&_0x5A, &&_0x5B, &&_0x5C, &&_0x5D, &&_0x5E, &&_0x5F,
    &&_0x60, &&_0x61, &&_0x62, &&_0x63, &&_0x64, &&_0x65, &&_0x66, &&_0x67, &&_0x68, &&_0x69, &&_0x6A, &&_0x6B, &&_0x6C, &&_0x6D, &&_0x6E, &&_0x6F,
    &&_0x70, &&_0x71, &&_0x72, &&_0x73, &&_0x74, &&_0x75, &&_0x76, &&_0x77, &&_0x78, &&_0x79, &&_0x7A, &&_0x7B, &&_0x7C, &&_0x7D, &&_0x7E, &&_0x7F,
    &&_0x80, &&_0x81, &&_0x82, &&_0x83, &&_0x84, &&_0x85, &&_0x86, &&_0x87, &&_0x88, &&_0x89, &&_0x8A, &&_0x8B, &&_0x8C, &&_0x8D, &&_0x8E, &&_0x8F,
    &&_0x90, &&_0x91, &&_0x92, &&_0x93, &&_0x94, &&_0x95, &&_0x96, &&_0x97, &&_0x98, &&_0x99, &&_0x9A, &&_0x9B, &&_0x9C, &&_0x9D, &&_0x9E, &&_0x9F,
    &&_0xA0, &&_0xA1, &&_0xA2, &&_0xA3, &&_0xA4, &&_0xA5, &&_0xA6, &&_0xA7, &&_0xA8, &&_0xA9, &&_0xAA, &&_0xAB, &&_0xAC, &&_0xAD, &&_0xAE, &&_0xAF,
    &&_0xB0, &&_0xB1, &&_0xB2, &&_0xB3, &&_0xB4, &&_0xB5, &&_0xB6, &&_0xB7, &&_0xB8, &&_0xB9, &&_0xBA, &&_0xBB, &&_0xBC, &&_0xBD, &&_0xBE, &&_0xBF,
    &&_0xC0, &&_0xC1, &&_0xC2, &&_0xC3, &&_0xC4, &&_0xC5, &&_0xC6, &&_0xC7, &&_0xC8, &&_0xC9, &&_0xCA, &&_0xCB, &&_0xCC, &&_0xCD, &&_0xCE, &&_0xCF,
    &&_0xD0, &&_0xD1, &&_0xD2, &&_NIMP, &&_0xD4, &&_0xD5, &&_0xD6, &&_0xD7, &&_0xD8, &&_0xD9, &&_0xDA, &&_NIMP, &&_0xDC, &&_NIMP, &&_0xDE, &&_0xDF,
    &&_0xE0, &&_0xE1, &&_0xE2, &&_NIMP, &&_NIMP, &&_0xE5, &&_0xE6, &&_0xE7, &&_0xE8, &&_0xE9, &&_0xEA, &&_NIMP, &&_NIMP, &&_NIMP, &&_0xEE, &&_0xEF,
    &&_0xF0, &&_0xF1, &&_0xF2, &&_0xF3, &&_NIMP, &&_0xF5, &&_0xF6, &&_0xF7, &&_0xF8, &&_0xF9, &&_0xFA, &&_0xFB, &&_NIMP, &&_NIMP, &&_0xFE, &&_0xFF};

  _0x00: inst.NOP(); goto end;
  _0x01: inst.LD_n_nn(BC); goto end;
  _0x02: inst.LD_n_A(c_BC); goto end;
  _0x03: inst.INC_nn(BC); goto end;
  _0x04: inst.INC_n(B); goto end;
  _0x05: inst.DEC_n(B); goto end;
  _0x06: inst.LD_nn_n(B); goto end;
  _0x07: inst.RLC_n(A); goto end;
  _0x08: inst.LD_nn_SP(); goto end;
  _0x09: inst.ADD_HL_n(BC); goto end;
  _0x0A: inst.LD_A_n(c_BC); goto end;
  _0x0B: inst.DEC_nn(BC); goto end;
  _0x0C: inst.INC_n(C); goto end;
  _0x0D: inst.DEC_n(C); goto end;
  _0x0E: inst.LD_nn_n(C); goto end;
  _0x0F: inst.RRC_n(A); goto end;

  _0x10: inst.STOP(); ChangeSpeed(); goto end;
  _0x11: inst.LD_n_nn(DE); goto end;
  _0x12: inst.LD_n_A(c_DE); goto end;
  _0x13: inst.INC_nn(DE); goto end;
  _0x14: inst.INC_n(D); goto end;
  _0x15: inst.DEC_n(D); goto end;
  _0x16: inst.LD_nn_n(D); goto end;
  _0x17: inst.RL_n(A); goto end;
  _0x18: inst.JR(); goto end;
  _0x19: inst.ADD_HL_n(DE); goto end;
  _0x1A: inst.LD_A_n(c_DE); goto end;
  _0x1B: inst.DEC_nn(DE); goto end;
  _0x1C: inst.INC_n(E); goto end;
  _0x1D: inst.DEC_n(E); goto end;
  _0x1E: inst.LD_nn_n(E); goto end;
  _0x1F: inst.RR_n(A); goto end;

  _0x20: inst.JR_CC_n(f_Z, 0); goto end;
  _0x21: inst.LD_n_nn(HL); goto end;
  _0x22: inst.LDI_cHL_A(); goto end;
  _0x23: inst.INC_nn(HL); goto end;
  _0x24: inst.INC_n(H); goto end;
  _0x25: inst.DEC_n(H); goto end;
  _0x26: inst.LD_nn_n(H); goto end;
  _0x27: inst.DAA(); goto end;
  _0x28: inst.JR_CC_n(f_Z, 1); goto end;
  _0x29: inst.ADD_HL_n(HL); goto end;
  _0x2A: inst.LDI_A_cHL(); goto end;
  _0x2B: inst.DEC_nn(HL); goto end;
  _0x2C: inst.INC_n(L); goto end;
  _0x2D: inst.DEC_n(L); goto end;
  _0x2E: inst.LD_nn_n(L); goto end;
  _0x2F: inst.CPL(); goto end;

  _0x30: inst.JR_CC_n(f_C, 0); goto end;
  _0x31: inst.LD_n_nn(SP); goto end;
  _0x32: inst.LDD_cHL_A(); goto end;
  _0x33: inst.INC_nn(SP); goto end;
  _0x34: inst.INC_n(c_HL); goto end;
  _0x35: inst.DEC_n(c_HL); goto end;
  _0x36: inst.LD_r1_r2(c_HL, $); goto end;
  _0x37: inst.SCF(); goto end;
  _0x38: inst.JR_CC_n(f_C, 1); goto end;
  _0x39: inst.ADD_HL_n(SP); goto end;
  _0x3A: inst.LDD_A_cHL(); goto end;
  _0x3B: inst.DEC_nn(SP); goto end;
  _0x3C: inst.INC_n(A); goto end;
  _0x3D: inst.DEC_n(A); goto end;
  _0x3E: inst.LD_A_n($); goto end;
  _0x3F: inst.CCF(); goto end;

  _0x40: inst.LD_r1_r2(B, B); goto end;
  _0x41: inst.LD_r1_r2(B, C); goto end;
  _0x42: inst.LD_r1_r2(B, D); goto end;
  _0x43: inst.LD_r1_r2(B, E); goto end;
  _0x44: inst.LD_r1_r2(B, H); goto end;
  _0x45: inst.LD_r1_r2(B, L); goto end;
  _0x46: inst.LD_r1_r2(B, c_HL); goto end;
  _0x47: inst.LD_n_A(B); goto end;
  _0x48: inst.LD_r1_r2(C, B); goto end;
  _0x49: inst.LD_r1_r2(C, C); goto end;
  _0x4A: inst.LD_r1_r2(C, D); goto end;
  _0x4B: inst.LD_r1_r2(C, E); goto end;
  _0x4C: inst.LD_r1_r2(C, H); goto end;
  _0x4D: inst.LD_r1_r2(C, L); goto end;
  _0x4E: inst.LD_r1_r2(C, c_HL); goto end;
  _0x4F: inst.LD_n_A(C); goto end;

  _0x50: inst.LD_r1_r2(D, B); goto end;
  _0x51: inst.LD_r1_r2(D, C); goto end;
  _0x52: inst.LD_r1_r2(D, D); goto end;
  _0x53: inst.LD_r1_r2(D, E); goto end;
  _0x54: inst.LD_r1_r2(D, H); goto end;
  _0x55: inst.LD_r1_r2(D, L); goto end;
  _0x56: inst.LD_r1_r2(D, c_HL); goto end;
  _0x57: inst.LD_n_A(D); goto end;
  _0x58: inst.LD_r1_r2(E, B); goto end;
  _0x59: inst.LD_r1_r2(E, C); goto end;
  _0x5A: inst.LD_r1_r2(E, D); goto end;
  _0x5B: inst.LD_r1_r2(E, E); goto end;
  _0x5C: inst.LD_r1_r2(E, H); goto end;
  _0x5D: inst.LD_r1_r2(E, L); goto end;
  _0x5E: inst.LD_r1_r2(E, c_HL); goto end;
  _0x5F: inst.LD_n_A(E); goto end;

  _0x60: inst.LD_r1_r2(H, B); goto end;
  _0x61: inst.LD_r1_r2(H, C); goto end;
  _0x62: inst.LD_r1_r2(H, D); goto end;
  _0x63: inst.LD_r1_r2(H, E); goto end;
  _0x64: inst.LD_r1_r2(H, H); goto end;
  _0x65: inst.LD_r1_r2(H, L); goto end;
  _0x66: inst.LD_r1_r2(H, c_HL); goto end;
  _0x67: inst.LD_n_A(H); goto end;
  _0x68: inst.LD_r1_r2(L, B); goto end;
  _0x69: inst.LD_r1_r2(L, C); goto end;
  _0x6A: inst.LD_r1_r2(L, D); goto end;
  _0x6B: inst.LD_r1_r2(L, E); goto end;
  _0x6C: inst.LD_r1_r2(L, H); goto end;
  _0x6D: inst.LD_r1_r2(L, L); goto end;
  _0x6E: inst.LD_r1_r2(L, c_HL); goto end;
  _0x6F: inst.LD_n_A(L); goto end;

  _0x70: inst.LD_r1_r2(c_HL, B); goto end;
  _0x71: inst.LD_r1_r2(c_HL, C); goto end;
  _0x72: inst.LD_r1_r2(c_HL, D); goto end;
  _0x73: inst.LD_r1_r2(c_HL, E); goto end;
  _0x74: inst.LD_r1_r2(c_HL, H); goto end;
  _0x75: inst.LD_r1_r2(c_HL, L); goto end;
  _0x76: inst.HALT(); goto end;
  _0x77: inst.LD_n_A(c_HL); goto end;
  _0x78: inst.LD_A_n(B); goto end;
  _0x79: inst.LD_A_n(C); goto end;
  _0x7A: inst.LD_A_n(D); goto end;
  _0x7B: inst.LD_A_n(E); goto end;
  _0x7C: inst.LD_A_n(H); goto end;
  _0x7D: inst.LD_A_n(L); goto end;
  _0x7E: inst.LD_A_n(c_HL); goto end;
  _0x7F: inst.LD_n_A(A); goto end;

  _0x80: inst.ADD_A_n(B); goto end;
  _0x81: inst.ADD_A_n(C); goto end;
  _0x82: inst.ADD_A_n(D); goto end;
  _0x83: inst.ADD_A_n(E); goto end;
  _0x84: inst.ADD_A_n(H); goto end;
  _0x85: inst.ADD_A_n(L); goto end;
  _0x86: inst.ADD_A_n(c_HL); goto end;
  _0x87: inst.ADD_A_n(A); goto end;
  _0x88: inst.ADC_A_n(B); goto end;
  _0x89: inst.ADC_A_n(C); goto end;
  _0x8A: inst.ADC_A_n(D); goto end;
  _0x8B: inst.ADC_A_n(E); goto end;
  _0x8C: inst.ADC_A_n(H); goto end;
  _0x8D: inst.ADC_A_n(L); goto end;
  _0x8E: inst.ADC_A_n(c_HL); goto end;
  _0x8F: inst.ADC_A_n(A); goto end;

  _0x90: inst.SUB_n(B); goto end;
  _0x91: inst.SUB_n(C); goto end;
  _0x92: inst.SUB_n(D); goto end;
  _0x93: inst.SUB_n(E); goto end;
  _0x94: inst.SUB_n(H); goto end;
  _0x95: inst.SUB_n(L); goto end;
  _0x96: inst.SUB_n(c_HL); goto end;
  _0x97: inst.SUB_n(A); goto end;
  _0x98: inst.SBC_A(B); goto end;
  _0x99: inst.SBC_A(C); goto end;
  _0x9A: inst.SBC_A(D); goto end;
  _0x9B: inst.SBC_A(E); goto end;
  _0x9C: inst.SBC_A(H); goto end;
  _0x9D: inst.SBC_A(L); goto end;
  _0x9E: inst.SBC_A(c_HL); goto end;
  _0x9F: inst.SBC_A(A); goto end;

  _0xA0: inst.AND(B); goto end;
  _0xA1: inst.AND(C); goto end;
  _0xA2: inst.AND(D); goto end;
  _0xA3: inst.AND(E); goto end;
  _0xA4: inst.AND(H); goto end;
  _0xA5: inst.AND(L); goto end;
  _0xA6: inst.AND(c_HL); goto end;
  _0xA7: inst.AND(A); goto end;
  _0xA8: inst.XOR_n(B); goto end;
  _0xA9: inst.XOR_n(C); goto end;
  _0xAA: inst.XOR_n(D); goto end;
  _0xAB: inst.XOR_n(E); goto end;
  _0xAC: inst.XOR_n(H); goto end;
  _0xAD: inst.XOR_n(L); goto end;
  _0xAE: inst.XOR_n(c_HL); goto end;
  _0xAF: inst.XOR_n(A); goto end;

  _0xB0: inst.OR_n(B); goto end;
  _0xB1: inst.OR_n(C); goto end;
  _0xB2: inst.OR_n(D); goto end;
  _0xB3: inst.OR_n(E); goto end;
  _0xB4: inst.OR_n(H); goto end;
  _0xB5: inst.OR_n(L); goto end;
  _0xB6: inst.OR_n(c_HL); goto end;
  _0xB7: inst.OR_n(A); goto end;
  _0xB8: inst.CP_n(B); goto end;
  _0xB9: inst.CP_n(C); goto end;
  _0xBA: inst.CP_n(D); goto end;
  _0xBB: inst.CP_n(E); goto end;
  _0xBC: inst.CP_n(H); goto end;
  _0xBD: inst.CP_n(L); goto end;
  _0xBE: inst.CP_n(c_HL); goto end;
  _0xBF: inst.CP_n(A); goto end;

  _0xC0: inst.RET_cc(f_Z, 0); goto end;
  _0xC1: inst.POP_nn(BC); goto end;
  _0xC2: inst.JP_cc_nn(f_Z, 0); goto end;
  _0xC3: inst.JP_nn(); goto end;
  _0xC4: inst.CALL_cc_nn(f_Z, 0); goto end;
  _0xC5: inst.PUSH_nn(BC); goto end;
  _0xC6: inst.ADD_A_n($); goto end;
  _0xC7: inst.RST_n(0x00); goto end;
  _0xC8: inst.RET_cc(f_Z, 1); goto end;
  _0xC9: inst.RET(); goto end;
  _0xCA: inst.JP_cc_nn(f_Z, 1); goto end;
  _0xCB: OpCodeCB(&inst); goto end;
  _0xCC: inst.CALL_cc_nn(f_Z, 1); goto end;
  _0xCD: inst.CALL_nn(); goto end;
  _0xCE: inst.ADC_A_n($); goto end;
  _0xCF: inst.RST_n(0x08); goto end;

  _0xD0: inst.RET_cc(f_C, 0); goto end;
  _0xD1: inst.POP_nn(DE); goto end;
  _0xD2: inst.JP_cc_nn(f_C, 0); goto end;
  _0xD4: inst.CALL_cc_nn(f_C, 0); goto end;
  _0xD5: inst.PUSH_nn(DE); goto end;
  _0xD6: inst.SUB_n($); goto end;
  _0xD7: inst.RST_n(0x10); goto end;
  _0xD8: inst.RET_cc(f_C, 1); goto end;
  _0xD9: inst.RETI(); goto end;
  _0xDA: inst.JP_cc_nn(f_C, 1); goto end;
  _0xDC: inst.CALL_cc_nn(f_C, 1); goto end;
  _0xDE: inst.SBC_A($); goto end;
  _0xDF: inst.RST_n(0x18); goto end;

  _0xE0: inst.LDH_c$_A(); goto end;
  _0xE1: inst.POP_nn(HL); goto end;
  _0xE2: inst.LD_cC_A(); goto end;
  _0xE5: inst.PUSH_nn(HL); goto end;
  _0xE6: inst.AND($); goto end;
  _0xE7: inst.RST_n(0x20); goto end;
  _0xE8: inst.ADD_SP_n(); goto end;
  _0xE9: inst.JP_HL(); goto end;
  _0xEA: inst.LD_n_A(c_$$); goto end;
  _0xEE: inst.XOR_n($); goto end;
  _0xEF: inst.RST_n(0x28); goto end;

  _0xF0: inst.LDH_A_n(); goto end;
  _0xF1: inst.POP_nn(AF); goto end;
  _0xF2: inst.LD_A_cC(); goto end;
  _0xF3: inst.DI(); goto end;
  _0xF5: inst.PUSH_nn(AF); goto end;
  _0xF6: inst.OR_n($); goto end;
  _0xF7: inst.RST_n(0x30); goto end;
  _0xF8: inst.LDHL_SP_n(); goto end;
  _0xF9: inst.LD_SP_HL(); goto end;
  _0xFA: inst.LD_A_n(c_$$); goto end;
  _0xFB: inst.EI(); goto end;
  _0xFE: inst.CP_n($); goto end;
  _0xFF: inst.RST_n(0x38); goto end;
  _NIMP: throw GBException("something"); goto end;

  #ifdef MAKEGBLOG
  	log->Enqueue("\n\nStartFrame", NULL, "");
  #endif

  frameCompleted = false;
  while (!frameCompleted)
  {
    numInstructions++;
    lastOpCode = OpCode;
    OpCode = MemR(Get_PC());
    NextOpcode = MemR(Get_PC() + 1);

    if (!Get_Halt())
    {
      #ifdef MAKEGBLOG
    		stringstream ssOpCode;
    		ssOpCode << numInstructions << ", ";
    		ssOpCode << "Op: " << setfill('0') << setw(2) << uppercase << hex << (int)OpCode;
    		if (OpCode == 0xCB)
    			ssOpCode << setfill('0') << setw(2) << uppercase << hex << (int)NextOpcode;
        ssOpCode << ", ";
    		log->Enqueue(ssOpCode.str(), this->GetPtrRegisters(), "");
      #endif

      std::cout << setfill('0') << setw(2) << uppercase << hex << (int)OpCode << std::endl;
      std::cout << "NumInstr: " << numInstructions << std::endl;
      ptr = dispatch[OpCode];
      goto *ptr;
    end: ;
    } // end if (!Get_Halt())

    #ifdef INSTLOG
      if (OpCode != 0xCB)
      {
          logFile << setfill('0') << setw(2) << uppercase << hex << (int)OpCode << std::endl;
      } else {
          logFile << setfill('0') << setw(2) << uppercase << hex << (int)OpCode
          << setfill('0') << setw(2) << uppercase << hex << (int)NextOpcode << std::endl;
      }
    #endif

    if (OpCode == 0xCB)
        lastCycles += instructionCyclesCB[NextOpcode]*4;
    else if (Get_ConditionalTaken())
    {
        lastCycles += instructionCondicionalCycles[OpCode]*4;
        Set_ConditionalTaken(false);
    }
    else
        lastCycles = instructionCycles[OpCode]*4;

    if (newInterrupt)
    {
        lastCycles += 20;
        newInterrupt = false;
    }

    tmpCycles = lastCycles;
    actualCycles += lastCycles;

    UpdateStateLCD(lastCycles);
    UpdateTimer(lastCycles);
    UpdateSerial(lastCycles);
    Interrupts(&inst);

    lastCycles -= tmpCycles;

  } //end while

  #ifdef INSTLOG
    logFile.close();
  #endif
}

void CPU::OpCodeCB(Instructions * inst)
{
    BYTE OpCode;

    OpCode = MemR(Get_PC() + 1);

    switch (OpCode)
    {
		case (0x00): inst->RLC_n(B); break;
		case (0x01): inst->RLC_n(C); break;
		case (0x02): inst->RLC_n(D); break;
		case (0x03): inst->RLC_n(E); break;
		case (0x04): inst->RLC_n(H); break;
		case (0x05): inst->RLC_n(L); break;
		case (0x06): inst->RLC_n(c_HL); break;
		case (0x07): inst->RLC_n(A); break;
		case (0x08): inst->RRC_n(B); break;
		case (0x09): inst->RRC_n(C); break;
		case (0x0A): inst->RRC_n(D); break;
		case (0x0B): inst->RRC_n(E); break;
		case (0x0C): inst->RRC_n(H); break;
		case (0x0D): inst->RRC_n(L); break;
		case (0x0E): inst->RRC_n(c_HL); break;
		case (0x0F): inst->RRC_n(A); break;

		case (0x10): inst->RL_n(B); break;
		case (0x11): inst->RL_n(C); break;
		case (0x12): inst->RL_n(D); break;
		case (0x13): inst->RL_n(E); break;
		case (0x14): inst->RL_n(H); break;
		case (0x15): inst->RL_n(L); break;
		case (0x16): inst->RL_n(c_HL); break;
		case (0x17): inst->RL_n(A); break;
		case (0x18): inst->RR_n(B); break;
		case (0x19): inst->RR_n(C); break;
		case (0x1A): inst->RR_n(D); break;
		case (0x1B): inst->RR_n(E); break;
		case (0x1C): inst->RR_n(H); break;
		case (0x1D): inst->RR_n(L); break;
		case (0x1E): inst->RR_n(c_HL); break;
		case (0x1F): inst->RR_n(A); break;

        case (0x20): inst->SLA_n(B); break;
        case (0x21): inst->SLA_n(C); break;
        case (0x22): inst->SLA_n(D); break;
        case (0x23): inst->SLA_n(E); break;
        case (0x24): inst->SLA_n(H); break;
        case (0x25): inst->SLA_n(L); break;
        case (0x26): inst->SLA_n(c_HL); break;
        case (0x27): inst->SLA_n(A); break;
		case (0x28): inst->SRA_n(B); break;
		case (0x29): inst->SRA_n(C); break;
		case (0x2A): inst->SRA_n(D); break;
		case (0x2B): inst->SRA_n(E); break;
		case (0x2C): inst->SRA_n(H); break;
		case (0x2D): inst->SRA_n(L); break;
		case (0x2E): inst->SRA_n(c_HL); break;
		case (0x2F): inst->SRA_n(A); break;

		case (0x30): inst->SWAP(B); break;
		case (0x31): inst->SWAP(C); break;
		case (0x32): inst->SWAP(D); break;
		case (0x33): inst->SWAP(E); break;
		case (0x34): inst->SWAP(H); break;
		case (0x35): inst->SWAP(L); break;
		case (0x36): inst->SWAP(c_HL); break;
		case (0x37): inst->SWAP(A); break;
        case (0x38): inst->SRL_n(B); break;
        case (0x39): inst->SRL_n(C); break;
        case (0x3A): inst->SRL_n(D); break;
        case (0x3B): inst->SRL_n(E); break;
        case (0x3C): inst->SRL_n(H); break;
        case (0x3D): inst->SRL_n(L); break;
        case (0x3E): inst->SRL_n(c_HL); break;
        case (0x3F): inst->SRL_n(A); break;

		case (0x40): inst->BIT_b_r(0, B); break;
		case (0x41): inst->BIT_b_r(0, C); break;
		case (0x42): inst->BIT_b_r(0, D); break;
		case (0x43): inst->BIT_b_r(0, E); break;
		case (0x44): inst->BIT_b_r(0, H); break;
		case (0x45): inst->BIT_b_r(0, L); break;
		case (0x46): inst->BIT_b_r(0, c_HL); break;
		case (0x47): inst->BIT_b_r(0, A); break;
		case (0x48): inst->BIT_b_r(1, B); break;
		case (0x49): inst->BIT_b_r(1, C); break;
		case (0x4A): inst->BIT_b_r(1, D); break;
		case (0x4B): inst->BIT_b_r(1, E); break;
		case (0x4C): inst->BIT_b_r(1, H); break;
		case (0x4D): inst->BIT_b_r(1, L); break;
		case (0x4E): inst->BIT_b_r(1, c_HL); break;
		case (0x4F): inst->BIT_b_r(1, A); break;

		case (0x50): inst->BIT_b_r(2, B); break;
		case (0x51): inst->BIT_b_r(2, C); break;
		case (0x52): inst->BIT_b_r(2, D); break;
		case (0x53): inst->BIT_b_r(2, E); break;
		case (0x54): inst->BIT_b_r(2, H); break;
		case (0x55): inst->BIT_b_r(2, L); break;
		case (0x56): inst->BIT_b_r(2, c_HL); break;
		case (0x57): inst->BIT_b_r(2, A); break;
		case (0x58): inst->BIT_b_r(3, B); break;
		case (0x59): inst->BIT_b_r(3, C); break;
		case (0x5A): inst->BIT_b_r(3, D); break;
		case (0x5B): inst->BIT_b_r(3, E); break;
		case (0x5C): inst->BIT_b_r(3, H); break;
		case (0x5D): inst->BIT_b_r(3, L); break;
		case (0x5E): inst->BIT_b_r(3, c_HL); break;
		case (0x5F): inst->BIT_b_r(3, A); break;

		case (0x60): inst->BIT_b_r(4, B); break;
		case (0x61): inst->BIT_b_r(4, C); break;
		case (0x62): inst->BIT_b_r(4, D); break;
		case (0x63): inst->BIT_b_r(4, E); break;
		case (0x64): inst->BIT_b_r(4, H); break;
		case (0x65): inst->BIT_b_r(4, L); break;
		case (0x66): inst->BIT_b_r(4, c_HL); break;
		case (0x67): inst->BIT_b_r(4, A); break;
		case (0x68): inst->BIT_b_r(5, B); break;
		case (0x69): inst->BIT_b_r(5, C); break;
		case (0x6A): inst->BIT_b_r(5, D); break;
		case (0x6B): inst->BIT_b_r(5, E); break;
		case (0x6C): inst->BIT_b_r(5, H); break;
		case (0x6D): inst->BIT_b_r(5, L); break;
		case (0x6E): inst->BIT_b_r(5, c_HL); break;
		case (0x6F): inst->BIT_b_r(5, A); break;

		case (0x70): inst->BIT_b_r(6, B); break;
		case (0x71): inst->BIT_b_r(6, C); break;
		case (0x72): inst->BIT_b_r(6, D); break;
		case (0x73): inst->BIT_b_r(6, E); break;
		case (0x74): inst->BIT_b_r(6, H); break;
		case (0x75): inst->BIT_b_r(6, L); break;
		case (0x76): inst->BIT_b_r(6, c_HL); break;
		case (0x77): inst->BIT_b_r(6, A); break;
		case (0x78): inst->BIT_b_r(7, B); break;
		case (0x79): inst->BIT_b_r(7, C); break;
		case (0x7A): inst->BIT_b_r(7, D); break;
		case (0x7B): inst->BIT_b_r(7, E); break;
		case (0x7C): inst->BIT_b_r(7, H); break;
		case (0x7D): inst->BIT_b_r(7, L); break;
		case (0x7E): inst->BIT_b_r(7, c_HL); break;
		case (0x7F): inst->BIT_b_r(7, A); break;

        case (0x80): inst->RES_b_r(0, B); break;
        case (0x81): inst->RES_b_r(0, C); break;
        case (0x82): inst->RES_b_r(0, D); break;
        case (0x83): inst->RES_b_r(0, E); break;
        case (0x84): inst->RES_b_r(0, H); break;
        case (0x85): inst->RES_b_r(0, L); break;
        case (0x86): inst->RES_b_r(0, c_HL); break;
        case (0x87): inst->RES_b_r(0, A); break;
        case (0x88): inst->RES_b_r(1, B); break;
        case (0x89): inst->RES_b_r(1, C); break;
        case (0x8A): inst->RES_b_r(1, D); break;
        case (0x8B): inst->RES_b_r(1, E); break;
        case (0x8C): inst->RES_b_r(1, H); break;
        case (0x8D): inst->RES_b_r(1, L); break;
        case (0x8E): inst->RES_b_r(1, c_HL); break;
        case (0x8F): inst->RES_b_r(1, A); break;

        case (0x90): inst->RES_b_r(2, B); break;
        case (0x91): inst->RES_b_r(2, C); break;
        case (0x92): inst->RES_b_r(2, D); break;
        case (0x93): inst->RES_b_r(2, E); break;
        case (0x94): inst->RES_b_r(2, H); break;
        case (0x95): inst->RES_b_r(2, L); break;
        case (0x96): inst->RES_b_r(2, c_HL); break;
        case (0x97): inst->RES_b_r(2, A); break;
        case (0x98): inst->RES_b_r(3, B); break;
        case (0x99): inst->RES_b_r(3, C); break;
        case (0x9A): inst->RES_b_r(3, D); break;
        case (0x9B): inst->RES_b_r(3, E); break;
        case (0x9C): inst->RES_b_r(3, H); break;
        case (0x9D): inst->RES_b_r(3, L); break;
        case (0x9E): inst->RES_b_r(3, c_HL); break;
        case (0x9F): inst->RES_b_r(3, A); break;

        case (0xA0): inst->RES_b_r(4, B); break;
        case (0xA1): inst->RES_b_r(4, C); break;
        case (0xA2): inst->RES_b_r(4, D); break;
        case (0xA3): inst->RES_b_r(4, E); break;
        case (0xA4): inst->RES_b_r(4, H); break;
        case (0xA5): inst->RES_b_r(4, L); break;
        case (0xA6): inst->RES_b_r(4, c_HL); break;
        case (0xA7): inst->RES_b_r(4, A); break;
        case (0xA8): inst->RES_b_r(5, B); break;
        case (0xA9): inst->RES_b_r(5, C); break;
        case (0xAA): inst->RES_b_r(5, D); break;
        case (0xAB): inst->RES_b_r(5, E); break;
        case (0xAC): inst->RES_b_r(5, H); break;
        case (0xAD): inst->RES_b_r(5, L); break;
        case (0xAE): inst->RES_b_r(5, c_HL); break;
        case (0xAF): inst->RES_b_r(5, A); break;

        case (0xB0): inst->RES_b_r(6, B); break;
        case (0xB1): inst->RES_b_r(6, C); break;
        case (0xB2): inst->RES_b_r(6, D); break;
        case (0xB3): inst->RES_b_r(6, E); break;
        case (0xB4): inst->RES_b_r(6, H); break;
        case (0xB5): inst->RES_b_r(6, L); break;
        case (0xB6): inst->RES_b_r(6, c_HL); break;
        case (0xB7): inst->RES_b_r(6, A); break;
        case (0xB8): inst->RES_b_r(7, B); break;
        case (0xB9): inst->RES_b_r(7, C); break;
        case (0xBA): inst->RES_b_r(7, D); break;
        case (0xBB): inst->RES_b_r(7, E); break;
        case (0xBC): inst->RES_b_r(7, H); break;
        case (0xBD): inst->RES_b_r(7, L); break;
        case (0xBE): inst->RES_b_r(7, c_HL); break;
        case (0xBF): inst->RES_b_r(7, A); break;

        case (0xC0): inst->SET_b_r(0, B); break;
        case (0xC1): inst->SET_b_r(0, C); break;
        case (0xC2): inst->SET_b_r(0, D); break;
        case (0xC3): inst->SET_b_r(0, E); break;
        case (0xC4): inst->SET_b_r(0, H); break;
        case (0xC5): inst->SET_b_r(0, L); break;
        case (0xC6): inst->SET_b_r(0, c_HL); break;
        case (0xC7): inst->SET_b_r(0, A); break;
        case (0xC8): inst->SET_b_r(1, B); break;
        case (0xC9): inst->SET_b_r(1, C); break;
        case (0xCA): inst->SET_b_r(1, D); break;
        case (0xCB): inst->SET_b_r(1, E); break;
        case (0xCC): inst->SET_b_r(1, H); break;
        case (0xCD): inst->SET_b_r(1, L); break;
        case (0xCE): inst->SET_b_r(1, c_HL); break;
        case (0xCF): inst->SET_b_r(1, A); break;

        case (0xD0): inst->SET_b_r(2, B); break;
        case (0xD1): inst->SET_b_r(2, C); break;
        case (0xD2): inst->SET_b_r(2, D); break;
        case (0xD3): inst->SET_b_r(2, E); break;
        case (0xD4): inst->SET_b_r(2, H); break;
        case (0xD5): inst->SET_b_r(2, L); break;
        case (0xD6): inst->SET_b_r(2, c_HL); break;
        case (0xD7): inst->SET_b_r(2, A); break;
        case (0xD8): inst->SET_b_r(3, B); break;
        case (0xD9): inst->SET_b_r(3, C); break;
        case (0xDA): inst->SET_b_r(3, D); break;
        case (0xDB): inst->SET_b_r(3, E); break;
        case (0xDC): inst->SET_b_r(3, H); break;
        case (0xDD): inst->SET_b_r(3, L); break;
        case (0xDE): inst->SET_b_r(3, c_HL); break;
        case (0xDF): inst->SET_b_r(3, A); break;

        case (0xE0): inst->SET_b_r(4, B); break;
        case (0xE1): inst->SET_b_r(4, C); break;
        case (0xE2): inst->SET_b_r(4, D); break;
        case (0xE3): inst->SET_b_r(4, E); break;
        case (0xE4): inst->SET_b_r(4, H); break;
        case (0xE5): inst->SET_b_r(4, L); break;
        case (0xE6): inst->SET_b_r(4, c_HL); break;
        case (0xE7): inst->SET_b_r(4, A); break;
        case (0xE8): inst->SET_b_r(5, B); break;
        case (0xE9): inst->SET_b_r(5, C); break;
        case (0xEA): inst->SET_b_r(5, D); break;
        case (0xEB): inst->SET_b_r(5, E); break;
        case (0xEC): inst->SET_b_r(5, H); break;
        case (0xED): inst->SET_b_r(5, L); break;
        case (0xEE): inst->SET_b_r(5, c_HL); break;
        case (0xEF): inst->SET_b_r(5, A); break;

        case (0xF0): inst->SET_b_r(6, B); break;
        case (0xF1): inst->SET_b_r(6, C); break;
        case (0xF2): inst->SET_b_r(6, D); break;
        case (0xF3): inst->SET_b_r(6, E); break;
        case (0xF4): inst->SET_b_r(6, H); break;
        case (0xF5): inst->SET_b_r(6, L); break;
        case (0xF6): inst->SET_b_r(6, c_HL); break;
        case (0xF7): inst->SET_b_r(6, A); break;
        case (0xF8): inst->SET_b_r(7, B); break;
        case (0xF9): inst->SET_b_r(7, C); break;
        case (0xFA): inst->SET_b_r(7, D); break;
        case (0xFB): inst->SET_b_r(7, E); break;
        case (0xFC): inst->SET_b_r(7, H); break;
        case (0xFD): inst->SET_b_r(7, L); break;
        case (0xFE): inst->SET_b_r(7, c_HL); break;
        case (0xFF): inst->SET_b_r(7, A); break;

        default:
			stringstream out;
			out << "Error, instruction not implemented: 0xCB";
			out << setfill('0') << setw(2) << uppercase << hex << (int)OpCode << "\n";
#ifdef MAKEGBLOG
            SaveLog();
#endif
			throw GBException(out.str().data());
    }
}

void CPU::UpdatePad()
{
	int interrupt = PadCheckKeyboard(&memory[P1]);
	if (interrupt)
        SetIntFlag(4);
}

void CPU::OnWriteLCDC(BYTE value)
{
    BYTE lastScreenOn = BIT7(memory[LCDC]);
    BYTE screenOn = BIT7(value);
    if (lastScreenOn && !screenOn)
    {
        memory[LY] = 0;
        // Poner a 00 el flag (bits 0-1) del modo 2.
        memory[STAT] = memory[STAT] & ~0x03;

        CheckLYC();

        cyclesLCD = 0;
    }
    else if (!lastScreenOn && screenOn)
    {
        cyclesLCD = 0;
    }

    memory[LCDC] = value;
}

void CPU::UpdateStateLCD(int cycles)
{
    cyclesLCD += cycles;

	BYTE screenOn = BIT7(memory[LCDC]);
    if (screenOn)
        UpdateStateLCDOn();
    else
    {
        if (cyclesLCD > cyclesFrame)
        {
            OnEndFrame();
            cyclesLCD -= cyclesFrame;
        }
    }
}

void CPU::UpdateStateLCDOn()
{
    BYTE mode = BITS01(memory[STAT]);

    switch (mode)
    {
        case (0):	// Durante H-Blank
            if (cyclesLCD >= lcdMode0)
            {
				memory[LY]++;
				CheckLYC();
                cyclesLCD -= lcdMode0;

                if (memory[LY] == 144) // Si estamos en la linea 144, cambiamos al modo 1 (V-Blank)
                {
                    // Poner a 01 el flag (bits 0-1) del modo 1.
					memory[STAT] = (memory[STAT] & ~0x03) | 0x01;
					VBlankIntPending = true;
                }
                else // Sino, cambiamos al modo 2
                {
                    // Poner a 10 el flag (bits 0-1) del modo 2.
					memory[STAT] = (memory[STAT] & ~0x03) | 0x02;
					// Si interrupcion OAM habilitada, marcar peticion de interrupcion
					// en 0xFF0F. Bit 1, flag de interrupcion de LCD STAT
					if (BIT5(memory[STAT]))
                        SetIntFlag(1);
                }

                if (colorMode)
                    UpdateHDMA();

				v->UpdateLine(memory[LY]-1);
            }
            break;
        case (1):	// Durante V-Blank
			// Se retrasa la interrupcion VBlank 24 ciclos.
			// (Algunos juegos con VBlank activado en IE, hacen la comprobacion
			// SCY == 144. Sin este retraso, antes de la interrupcion SCY = 143 y
			// despues SCY > 144. Con lo que nunca se cumple la condicion)
			if ((VBlankIntPending) && (cyclesLCD >= 24))
			{
				// Interrupcion V-Blank
                SetIntFlag(0);
                // Si interrupcion V-Blank habilitada, marcar peticion de interrupcion
                // en 0xFF0F. Bit 1, flag de interrupcion de LCD STAT.
                if (BIT4(memory[STAT]))
                    SetIntFlag(1);
				VBlankIntPending = false;
			}

            if (cyclesLCD >= lcdMode1)
            {
				cyclesLCD -= lcdMode1;

                // Si hemos llegado al final
                if (memory[LY] == 153)
                {
                    memory[LY] = 0;
                    // Poner a 10 el flag (bits 0-1) del modo 2.
					memory[STAT] = (memory[STAT] & ~0x03) | 0x02;
					// Si interrupcion OAM habilitada, marcar peticion de interrupcion
					// en 0xFF0F. Bit 1, flag de interrupcion de LCD STAT
					if (BIT5(memory[STAT]))
                        SetIntFlag(1);

					OnEndFrame();
                }
                else
                    memory[LY]++;

				CheckLYC();
            }
            break;
        case (2):	// Cuando OAM se esta usando
            if (cyclesLCD >= lcdMode2)
            {
				cyclesLCD -= lcdMode2;

				// Poner a 11 el flag (bits 0-1) del modo 3.
				memory[STAT] = (memory[STAT] & ~0x03) | 0x03;
            }
            break;
        case (3):	// Cuando OAM y memoria de video se estan usando (Se esta pasando informacion al LCD)
            if (cyclesLCD >= lcdMode3)
            {
				cyclesLCD -= lcdMode3;

				// Poner a 00 el flag (bits 0-1) del modo 0.
				memory[STAT] &= ~0x03;
				// Si interrupcion H-Blank habilitada, marcar peticion de interrupcion
				// en 0xFF0F. Bit 1, flag de interrupcion de LCD STAT
				if (BIT3(memory[STAT]))
                    SetIntFlag(1);
            }
            break;
	}
}

inline void CPU::CheckLYC()
{
	if (memory[LY] == memory[LYC])
	{
		memory[STAT] |= 0x04;
		if (BIT6(memory[STAT]))
            SetIntFlag(1);
	}
	else
		memory[STAT] &= ~0x04;
}

inline void CPU::UpdateSerial(int cycles)
{
    if (BIT7(memory[SC]) && BIT0(memory[SC]))
	{
        cyclesSerial += cycles;

		if (bitSerial < 0)
		{
			bitSerial = 0;
			cyclesSerial = 0;
			return;
		}

		if (cyclesSerial >= 512)
		{
			if (bitSerial > 7)
			{
				memory[SC] &= 0x7F;
				SetIntFlag(3);
				bitSerial = -1;
				return;
			}

			memory[SB] = memory[SB] << 1;
			memory[SB] |= 0x01;

			cyclesSerial -= 512;
			bitSerial++;
		}
	}
}

void CPU::SetIntFlag(int bit)
{
    int mask = 1 << bit;

    memory[IF] |= mask;
    if (memory[IE] & mask)
        Set_Halt(false);
}

void CPU::Interrupts(Instructions * inst)
{
	if (!Get_IME())
		return;

	BYTE interrupts = memory[IE] & memory[IF];
	if (!interrupts)
		return;

	Set_IME(false);
	Set_Halt(false);
	inst->PUSH_PC();
    newInterrupt = true;

	if (BIT0(interrupts))	//V-Blank
	{
		Set_PC(0x40);
		memory[IF] &= ~0x01;
	}
	else if (BIT1(interrupts))	//LCD-Stat
	{
		Set_PC(0x48);
		memory[IF] &= ~0x02;
	}
	else if (BIT2(interrupts))	//Timer
	{
		Set_PC(0x50);
		memory[IF] &= ~0x04;
	}
	else if(BIT3(interrupts))	//Serial
	{
		Set_PC(0x58);
		memory[IF] &= ~0x08;
	}
	else if (BIT4(interrupts))	//Joypad
	{
		Set_PC(0x60);
		memory[IF] &= ~0x10;
	}
}

void CPU::UpdateTimer(int cycles)
{
	// Estos serian los valores en Hz que puede tomar TAC:
	// 4096, 262144, 65536, 16384
	// En overflowTimer se encuentran estos valores en ciclos
	// maquina
	WORD overflowTimer[] = {1024, 16, 64, 256};

	if (BIT2(memory[TAC])) //Si esta habilitado el timer
	{
        cyclesTimer += cycles;

        WORD cyclesOverflow = overflowTimer[BITS01(memory[TAC])];
		while (cyclesTimer >= cyclesOverflow)
		{
			if (memory[TIMA] == 0xFF)
			{
				memory[TIMA] = memory[TMA];
                SetIntFlag(2);
			}
            else
                memory[TIMA]++;

			cyclesTimer -= cyclesOverflow;
		}
	}

    cyclesDIV += cycles;

	while (cyclesDIV >= 256)
	{
		memory[DIV]++;
		cyclesDIV -= 256;
	}
}

BYTE CPU::TACChanged(BYTE newValue)
{
    newValue &= 0x07;
    if (((newValue & 0x03) != (memory[TAC] & 0x03)) ||
        ((newValue & 0x04) == 0))
    {
        cyclesTimer = 0;
        memory[TIMA] = memory[TMA];
    }
    return newValue;
}

BYTE CPU::DIVChanged(BYTE newValue)
{
    cyclesDIV = 0;

    return 0;
}

BYTE CPU::P1Changed(BYTE newValue)
{
    BYTE oldP1 = memory[P1];
    newValue = (newValue & 0xF0) | (oldP1 & ~0xF0);
    newValue = PadUpdateInput(newValue);
    if ((newValue != oldP1) && ((newValue & 0x0F) != 0x0F))
    {
        //Debe producir una interrupcion
        memory[IF] |=  0x10;
    }
    return newValue;
}

void CPU::OnEndFrame()
{
	v->RefreshScreen();
	if (s)
		s->EndFrame();
	frameCompleted = true;
}

void CPU::ChangeSpeed()
{
    if (colorMode)
    {
        // Se cambia a la velocidad contraria a la que estamos
        if (memory[KEY1] & 0x80) {
            // Velocidad normal
            memory[KEY1] = 0;

            lcdMode0 = LCD_MODE_0;
            lcdMode1 = LCD_MODE_1;
            lcdMode2 = LCD_MODE_2;
            lcdMode3 = LCD_MODE_3;
            cyclesFrame = FRAME_CYCLES;
        }
        else {
            // Velocidad doble
            memory[KEY1] = 0x80;

            lcdMode0 = LCD_MODE_0 * 2;
            lcdMode1 = LCD_MODE_1 * 2;
            lcdMode2 = LCD_MODE_2 * 2;
            lcdMode3 = LCD_MODE_3 * 2;
            cyclesFrame = FRAME_CYCLES * 2;
        }
    }
}

void CPU::AddCycles(int cycles)
{
    lastCycles += cycles;
}

#ifdef MAKEGBLOG

void CPU::SaveLog()
{
	log->Save("log.txt");
}

#endif

void CPU::SaveState(string saveDirectory, int numSlot)
{
	if (c == NULL)
	{
		throw GBException("There is not rom loaded. The process can't continue.");
	}

	stringstream st;
	st << saveDirectory << c->GetName().c_str();
	st << "-" << numSlot << ".sav";
	string fileName = st.str();
	ofstream * file = new ofstream(fileName.c_str(), ios::out|ios::binary|ios::trunc);

	if (file && file->is_open())
	{
		int version = SAVE_STATE_VERSION;
		file->write((char *)&version, sizeof(int));
		file->write(c->GetName().c_str(), 16);

		SaveRegs(file);
		SaveMemory(file);
		c->SaveMBC(file);

		file->close();
	}

	if (file)
		delete file;
}

void CPU::LoadState(string loadDirectory, int numSlot)
{
	if (c == NULL)
	{
		throw GBException("There is not rom loaded. The process can't continue.");
	}

	stringstream st;
	st << loadDirectory << c->GetName().c_str();
	st << "-" << numSlot << ".sav";
	string fileName = st.str();
	ifstream * file = new ifstream(fileName.c_str(), ios::in|ios::binary);

	if (file && file->is_open())
	{
		int version = 0;
		file->read((char *)&version, sizeof(int));
		if (version != SAVE_STATE_VERSION)
		{
			file->close();
			delete file;
			throw GBException("This filesave is not compatible and can't be loaded.");
		}

		char * buffer = new char[16];
		file->read(buffer, 16);
		string cartName = string(buffer, 16);
		delete[] buffer;
		if (cartName != c->GetName())
		{
			file->close();
			delete file;
			throw GBException("This filesave does not belong to this rom and can't be loaded.");
		}

		LoadRegs(file);
		LoadMemory(file);
		c->LoadMBC(file);

		file->close();

		delete file;
	}
	else
	{
		if (file)
			delete file;

		throw GBException("Unable to open the filesave.");
	}
}
