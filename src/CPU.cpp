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

  void *dispatch[] = {&&NOP, &&LD_n_nn_BC, &&LD_n_A_c_BC, &&INC_nn_BC, &&INC_n_B,
    &&DEC_n_B, &&LD_nn_n_B, &&RLC_n_A, &&LD_nn_SP, &&ADD_HL_n_BC, &&LD_A_n_c_BC,
    &&DEC_nn_BC, &&INC_n_C, &&DEC_n_C, &&LD_nn_n_C, &&RRC_n_A, &&STOP,
    &&LD_n_nn_DE, &&LD_n_A_c_DE, &&INC_nn_DE, &&INC_n_D, &&DEC_n_D, &&LD_nn_n_D,
    &&RL_n_A, &&JR, &&ADD_HL_n_DE, &&LD_A_n_c_DE, &&DEC_nn_DE, &&INC_n_E,
    &&DEC_n_E, &&LD_nn_n_E, &&RR_n_A, &&JR_CC_n_f_Z_0, &&LD_n_nn_HL,
    &&LDI_cHL_A, &&INC_nn_HL, &&INC_n_H, &&DEC_n_H, &&LD_nn_n_H, &&DAA,
    &&JR_CC_n_f_Z_1, &&ADD_HL_n_HL, &&LDI_A_cHL, &&DEC_nn_HL, &&INC_n_L,
    &&DEC_n_L, &&LD_nn_n_L, &&CPL, &&JR_CC_n_f_C_0, &&LD_n_nn_SP,
    &&LDD_cHL_A, &&INC_nn_SP, &&INC_n_c_HL, &&DEC_n_c_HL, &&LD_r1_r2_c_HL_$,
    &&SCF, &&JR_CC_n_f_C_1, &&ADD_HL_n_SP, &&LDD_A_cHL, &&DEC_nn_SP, &&INC_n_A,
    &&DEC_n_A, &&LD_A_n_$, &&CCF, &&LD_r1_r2_B_B, &&LD_r1_r2_B_C, &&LD_r1_r2_B_D,
    &&LD_r1_r2_B_E, &&LD_r1_r2_B_H, &&LD_r1_r2_B_L, &&LD_r1_r2_B_c_HL, &&LD_n_A_B,
    &&LD_r1_r2_C_B, &&LD_r1_r2_C_C, &&LD_r1_r2_C_D, &&LD_r1_r2_C_E, &&LD_r1_r2_C_H,
    &&LD_r1_r2_C_L, &&LD_r1_r2_C_c_HL, &&LD_n_A_C, &&LD_r1_r2_D_B, &&LD_r1_r2_D_C,
    &&LD_r1_r2_D_D, &&LD_r1_r2_D_E, &&LD_r1_r2_D_H, &&LD_r1_r2_D_L, &&LD_r1_r2_D_c_HL,
    &&LD_n_A_D, &&LD_r1_r2_E_B, &&LD_r1_r2_E_C, &&LD_r1_r2_E_D, &&LD_r1_r2_E_E,
    &&LD_r1_r2_E_H, &&LD_r1_r2_E_L, &&LD_r1_r2_E_c_HL, &&LD_n_A_E, &&LD_r1_r2_H_B,
    &&LD_r1_r2_H_C, &&LD_r1_r2_H_D, &&LD_r1_r2_H_E, &&LD_r1_r2_H_H, &&LD_r1_r2_H_L,
    &&LD_r1_r2_H_c_HL, &&LD_n_A_H, &&LD_r1_r2_L_B, &&LD_r1_r2_L_C, &&LD_r1_r2_L_D,
    &&LD_r1_r2_L_E, &&LD_r1_r2_L_H, &&LD_r1_r2_L_L, &&LD_r1_r2_L_c_HL, &&LD_n_A_L,
    &&LD_r1_r2_c_HL_B, &&LD_r1_r2_c_HL_C, &&LD_r1_r2_c_HL_D, &&LD_r1_r2_c_HL_E,
    &&LD_r1_r2_c_HL_H, &&LD_r1_r2_c_HL_L, &&HALT, &&LD_n_A_c_HL, &&LD_A_n_B,
    &&LD_A_n_C, &&LD_A_n_D, &&LD_A_n_E, &&LD_A_n_H, &&LD_A_n_L, &&LD_A_n_c_HL,
    &&LD_n_A_A, &&ADD_A_n_B, &&ADD_A_n_C, &&ADD_A_n_D, &&ADD_A_n_E, &&ADD_A_n_H,
    &&ADD_A_n_L, &&ADD_A_n_c_HL, &&ADD_A_n_A, &&ADC_A_n_B, &&ADC_A_n_C, &&ADC_A_n_D,
    &&ADC_A_n_E, &&ADC_A_n_H, &&ADC_A_n_L, &&ADC_A_n_c_HL, &&ADC_A_n_A, &&SUB_n_B,
    &&SUB_n_C, &&SUB_n_D, &&SUB_n_E, &&SUB_n_H, &&SUB_n_L, &&SUB_n_c_HL, &&SUB_n_A,
    &&SBC_A_B, &&SBC_A_C, &&SBC_A_D, &&SBC_A_E, &&SBC_A_H, &&SBC_A_L, &&SBC_A_c_HL,
    &&SBC_A_A, &&AND_B, &&AND_C, &&AND_D, &&AND_E, &&AND_H, &&AND_L, &&AND_c_HL,
    &&AND_A, &&XOR_n_B, &&XOR_n_C, &&XOR_n_D, &&XOR_n_E, &&XOR_n_H, &&XOR_n_L,
    &&XOR_n_c_HL, &&XOR_n_A, &&OR_n_B, &&OR_n_C, &&OR_n_D, &&OR_n_E, &&OR_n_H,
    &&OR_n_L, &&OR_n_c_HL, &&OR_n_A, &&CP_n_B, &&CP_n_C, &&CP_n_D, &&CP_n_E, &&CP_n_H,
    &&CP_n_L, &&CP_n_c_HL, &&CP_n_A, &&RET_cc_f_Z_0, &&POP_nn_BC, &&JP_cc_nn_f_Z_0,
    &&JP_nn, &&CALL_cc_nn_f_Z_0, &&PUSH_nn_BC, &&ADD_A_n_$, &&RST_n_0x00, &&RET_cc_f_Z_1,
    &&RET, &&JP_cc_nn_f_Z_1, &&OpCodeCB, &&CALL_cc_nn_f_Z_1, &&CALL_nn, &&ADC_A_n_$,
    &&RST_n_0x08, &&RET_cc_f_C_0, &&POP_nn_DE, &&JP_cc_nn_f_C_0, &&NOT_IMPL, &&CALL_cc_nn_f_C_0,
    &&PUSH_nn_DE, &&SUB_n_$, &&RST_n_0x10, &&RET_cc_f_C_1, &&RETI, &&JP_cc_nn_f_C_1,
    &&NOT_IMPL, &&CALL_cc_nn_f_C_1, &&NOT_IMPL, &&SBC_A_$, &&RST_n_0x18, &&LDH_c$_A, &&POP_nn_HL, &&LD_cC_A,
    &&NOT_IMPL, &&NOT_IMPL, &&PUSH_nn_HL, &&AND_$, &&RST_n_0x20, &&ADD_SP_n, &&JP_HL, &&LD_n_A_c_$$,
    &&NOT_IMPL, &&NOT_IMPL, &&NOT_IMPL, &&XOR_n_$,
    &&RST_n_0x28, &&LDH_A_n, &&POP_nn_AF, &&LD_A_cC, &&DI, &&NOT_IMPL, &&PUSH_nn_AF, &&OR_n_$,
    &&RST_n_0x30, &&LDHL_SP_n, &&LD_SP_HL, &&LD_A_n_c_$$, &&EI, &&NOT_IMPL, &&NOT_IMPL,
    &&CP_n_$, &&RST_n_0x38};

  NOP:  inst.NOP(); goto end;
  LD_n_nn_BC:  inst.LD_n_nn(BC); goto end;
  LD_n_A_c_BC:  inst.LD_n_A(c_BC); goto end;
  INC_nn_BC:  inst.INC_nn(BC); goto end;
  INC_n_B:  inst.INC_n(B); goto end;
  DEC_n_B:  inst.DEC_n(B); goto end;
  LD_nn_n_B:  inst.LD_nn_n(B); goto end;
  RLC_n_A:  inst.RLC_n(A); goto end;
  LD_nn_SP:  inst.LD_nn_SP(); goto end;
  ADD_HL_n_BC:  inst.ADD_HL_n(BC); goto end;
  LD_A_n_c_BC:  inst.LD_A_n(c_BC); goto end;
  DEC_nn_BC:  inst.DEC_nn(BC); goto end;
  INC_n_C:  inst.INC_n(C); goto end;
  DEC_n_C:  inst.DEC_n(C); goto end;
  LD_nn_n_C:  inst.LD_nn_n(C); goto end;
  RRC_n_A:  inst.RRC_n(A); goto end;

  STOP: inst.STOP(); ChangeSpeed(); goto end;
  LD_n_nn_DE:  inst.LD_n_nn(DE); goto end;
  LD_n_A_c_DE:  inst.LD_n_A(c_DE); goto end;
  INC_nn_DE:  inst.INC_nn(DE); goto end;
  INC_n_D:  inst.INC_n(D); goto end;
  DEC_n_D:  inst.DEC_n(D); goto end;
  LD_nn_n_D:  inst.LD_nn_n(D); goto end;
  RL_n_A:  inst.RL_n(A); goto end;
  JR:  inst.JR(); goto end;
  ADD_HL_n_DE:  inst.ADD_HL_n(DE); goto end;
  LD_A_n_c_DE:  inst.LD_A_n(c_DE); goto end;
  DEC_nn_DE:  inst.DEC_nn(DE); goto end;
  INC_n_E:  inst.INC_n(E); goto end;
  DEC_n_E:  inst.DEC_n(E); goto end;
  LD_nn_n_E:  inst.LD_nn_n(E); goto end;
  RR_n_A:  inst.RR_n(A); goto end;

  JR_CC_n_f_Z_0:  inst.JR_CC_n(f_Z, 0); goto end;
  LD_n_nn_HL:  inst.LD_n_nn(HL); goto end;
  LDI_cHL_A:  inst.LDI_cHL_A(); goto end;
  INC_nn_HL:  inst.INC_nn(HL); goto end;
  INC_n_H:  inst.INC_n(H); goto end;
  DEC_n_H:  inst.DEC_n(H); goto end;
  LD_nn_n_H:  inst.LD_nn_n(H); goto end;
  DAA:  inst.DAA(); goto end;
  JR_CC_n_f_Z_1:  inst.JR_CC_n(f_Z, 1); goto end;
  ADD_HL_n_HL:  inst.ADD_HL_n(HL); goto end;
  LDI_A_cHL:  inst.LDI_A_cHL(); goto end;
  DEC_nn_HL:  inst.DEC_nn(HL); goto end;
  INC_n_L:  inst.INC_n(L); goto end;
  DEC_n_L:  inst.DEC_n(L); goto end;
  LD_nn_n_L:  inst.LD_nn_n(L); goto end;
  CPL:  inst.CPL(); goto end;

  JR_CC_n_f_C_0:  inst.JR_CC_n(f_C, 0); goto end;
  LD_n_nn_SP:  inst.LD_n_nn(SP); goto end;
  LDD_cHL_A:  inst.LDD_cHL_A(); goto end;
  INC_nn_SP:  inst.INC_nn(SP); goto end;
  INC_n_c_HL:  inst.INC_n(c_HL); goto end;
  DEC_n_c_HL:  inst.DEC_n(c_HL); goto end;
  LD_r1_r2_c_HL_$:  inst.LD_r1_r2(c_HL, $); goto end;
  SCF:  inst.SCF(); goto end;
  JR_CC_n_f_C_1:  inst.JR_CC_n(f_C, 1); goto end;
  ADD_HL_n_SP:  inst.ADD_HL_n(SP); goto end;
  LDD_A_cHL:  inst.LDD_A_cHL(); goto end;
  DEC_nn_SP:  inst.DEC_nn(SP); goto end;
  INC_n_A:  inst.INC_n(A); goto end;
  DEC_n_A:  inst.DEC_n(A); goto end;
  LD_A_n_$:  inst.LD_A_n($); goto end;
  CCF:  inst.CCF(); goto end;

  LD_r1_r2_B_B:  inst.LD_r1_r2(B, B); goto end;
  LD_r1_r2_B_C:  inst.LD_r1_r2(B, C); goto end;
  LD_r1_r2_B_D:  inst.LD_r1_r2(B, D); goto end;
  LD_r1_r2_B_E:  inst.LD_r1_r2(B, E); goto end;
  LD_r1_r2_B_H:  inst.LD_r1_r2(B, H); goto end;
  LD_r1_r2_B_L:  inst.LD_r1_r2(B, L); goto end;
  LD_r1_r2_B_c_HL:  inst.LD_r1_r2(B, c_HL); goto end;
  LD_n_A_B:  inst.LD_n_A(B); goto end;
  LD_r1_r2_C_B:  inst.LD_r1_r2(C, B); goto end;
  LD_r1_r2_C_C:  inst.LD_r1_r2(C, C); goto end;
  LD_r1_r2_C_D:  inst.LD_r1_r2(C, D); goto end;
  LD_r1_r2_C_E:  inst.LD_r1_r2(C, E); goto end;
  LD_r1_r2_C_H:  inst.LD_r1_r2(C, H); goto end;
  LD_r1_r2_C_L:  inst.LD_r1_r2(C, L); goto end;
  LD_r1_r2_C_c_HL:  inst.LD_r1_r2(C, c_HL); goto end;
  LD_n_A_C:  inst.LD_n_A(C); goto end;

  LD_r1_r2_D_B:  inst.LD_r1_r2(D, B); goto end;
  LD_r1_r2_D_C:  inst.LD_r1_r2(D, C); goto end;
  LD_r1_r2_D_D:  inst.LD_r1_r2(D, D); goto end;
  LD_r1_r2_D_E:  inst.LD_r1_r2(D, E); goto end;
  LD_r1_r2_D_H:  inst.LD_r1_r2(D, H); goto end;
  LD_r1_r2_D_L:  inst.LD_r1_r2(D, L); goto end;
  LD_r1_r2_D_c_HL:  inst.LD_r1_r2(D, c_HL); goto end;
  LD_n_A_D:  inst.LD_n_A(D); goto end;
  LD_r1_r2_E_B:  inst.LD_r1_r2(E, B); goto end;
  LD_r1_r2_E_C:  inst.LD_r1_r2(E, C); goto end;
  LD_r1_r2_E_D:  inst.LD_r1_r2(E, D); goto end;
  LD_r1_r2_E_E:  inst.LD_r1_r2(E, E); goto end;
  LD_r1_r2_E_H:  inst.LD_r1_r2(E, H); goto end;
  LD_r1_r2_E_L:  inst.LD_r1_r2(E, L); goto end;
  LD_r1_r2_E_c_HL:  inst.LD_r1_r2(E, c_HL); goto end;
  LD_n_A_E:  inst.LD_n_A(E); goto end;

  LD_r1_r2_H_B:  inst.LD_r1_r2(H, B); goto end;
  LD_r1_r2_H_C:  inst.LD_r1_r2(H, C); goto end;
  LD_r1_r2_H_D:  inst.LD_r1_r2(H, D); goto end;
  LD_r1_r2_H_E:  inst.LD_r1_r2(H, E); goto end;
  LD_r1_r2_H_H:  inst.LD_r1_r2(H, H); goto end;
  LD_r1_r2_H_L:  inst.LD_r1_r2(H, L); goto end;
  LD_r1_r2_H_c_HL:  inst.LD_r1_r2(H, c_HL); goto end;
  LD_n_A_H:  inst.LD_n_A(H); goto end;
  LD_r1_r2_L_B:  inst.LD_r1_r2(L, B); goto end;
  LD_r1_r2_L_C:  inst.LD_r1_r2(L, C); goto end;
  LD_r1_r2_L_D:  inst.LD_r1_r2(L, D); goto end;
  LD_r1_r2_L_E:  inst.LD_r1_r2(L, E); goto end;
  LD_r1_r2_L_H:  inst.LD_r1_r2(L, H); goto end;
  LD_r1_r2_L_L:  inst.LD_r1_r2(L, L); goto end;
  LD_r1_r2_L_c_HL:  inst.LD_r1_r2(L, c_HL); goto end;
  LD_n_A_L:  inst.LD_n_A(L); goto end;

  LD_r1_r2_c_HL_B:  inst.LD_r1_r2(c_HL, B); goto end;
  LD_r1_r2_c_HL_C:  inst.LD_r1_r2(c_HL, C); goto end;
  LD_r1_r2_c_HL_D:  inst.LD_r1_r2(c_HL, D); goto end;
  LD_r1_r2_c_HL_E:  inst.LD_r1_r2(c_HL, E); goto end;
  LD_r1_r2_c_HL_H:  inst.LD_r1_r2(c_HL, H); goto end;
  LD_r1_r2_c_HL_L:  inst.LD_r1_r2(c_HL, L); goto end;
  HALT:  inst.HALT(); goto end;
  LD_n_A_c_HL:  inst.LD_n_A(c_HL); goto end;
  LD_A_n_B:  inst.LD_A_n(B); goto end;
  LD_A_n_C:  inst.LD_A_n(C); goto end;
  LD_A_n_D:  inst.LD_A_n(D); goto end;
  LD_A_n_E:  inst.LD_A_n(E); goto end;
  LD_A_n_H:  inst.LD_A_n(H); goto end;
  LD_A_n_L:  inst.LD_A_n(L); goto end;
  LD_A_n_c_HL:  inst.LD_A_n(c_HL); goto end;
  LD_n_A_A:  inst.LD_n_A(A); goto end;

  ADD_A_n_B:  inst.ADD_A_n(B); goto end;
  ADD_A_n_C:  inst.ADD_A_n(C); goto end;
  ADD_A_n_D:  inst.ADD_A_n(D); goto end;
  ADD_A_n_E:  inst.ADD_A_n(E); goto end;
  ADD_A_n_H:  inst.ADD_A_n(H); goto end;
  ADD_A_n_L:  inst.ADD_A_n(L); goto end;
  ADD_A_n_c_HL:  inst.ADD_A_n(c_HL); goto end;
  ADD_A_n_A:  inst.ADD_A_n(A); goto end;
  ADC_A_n_B:  inst.ADC_A_n(B); goto end;
  ADC_A_n_C:  inst.ADC_A_n(C); goto end;
  ADC_A_n_D:  inst.ADC_A_n(D); goto end;
  ADC_A_n_E:  inst.ADC_A_n(E); goto end;
  ADC_A_n_H:  inst.ADC_A_n(H); goto end;
  ADC_A_n_L:  inst.ADC_A_n(L); goto end;
  ADC_A_n_c_HL:  inst.ADC_A_n(c_HL); goto end;
  ADC_A_n_A:  inst.ADC_A_n(A); goto end;

  SUB_n_B:  inst.SUB_n(B); goto end;
  SUB_n_C:  inst.SUB_n(C); goto end;
  SUB_n_D:  inst.SUB_n(D); goto end;
  SUB_n_E:  inst.SUB_n(E); goto end;
  SUB_n_H:  inst.SUB_n(H); goto end;
  SUB_n_L:  inst.SUB_n(L); goto end;
  SUB_n_c_HL:  inst.SUB_n(c_HL); goto end;
  SUB_n_A:  inst.SUB_n(A); goto end;
  SBC_A_B:  inst.SBC_A(B); goto end;
  SBC_A_C:  inst.SBC_A(C); goto end;
  SBC_A_D:  inst.SBC_A(D); goto end;
  SBC_A_E:  inst.SBC_A(E); goto end;
  SBC_A_H:  inst.SBC_A(H); goto end;
  SBC_A_L:  inst.SBC_A(L); goto end;
  SBC_A_c_HL:  inst.SBC_A(c_HL); goto end;
  SBC_A_A:  inst.SBC_A(A); goto end;

  AND_B:  inst.AND(B); goto end;
  AND_C:  inst.AND(C); goto end;
  AND_D:  inst.AND(D); goto end;
  AND_E:  inst.AND(E); goto end;
  AND_H:  inst.AND(H); goto end;
  AND_L:  inst.AND(L); goto end;
  AND_c_HL:  inst.AND(c_HL); goto end;
  AND_A:  inst.AND(A); goto end;
  XOR_n_B:  inst.XOR_n(B); goto end;
  XOR_n_C:  inst.XOR_n(C); goto end;
  XOR_n_D:  inst.XOR_n(D); goto end;
  XOR_n_E:  inst.XOR_n(E); goto end;
  XOR_n_H:  inst.XOR_n(H); goto end;
  XOR_n_L:  inst.XOR_n(L); goto end;
  XOR_n_c_HL:  inst.XOR_n(c_HL); goto end;
  XOR_n_A:  inst.XOR_n(A); goto end;

  OR_n_B:  inst.OR_n(B); goto end;
  OR_n_C:  inst.OR_n(C); goto end;
  OR_n_D:  inst.OR_n(D); goto end;
  OR_n_E:  inst.OR_n(E); goto end;
  OR_n_H:  inst.OR_n(H); goto end;
  OR_n_L:  inst.OR_n(L); goto end;
  OR_n_c_HL:  inst.OR_n(c_HL); goto end;
  OR_n_A:  inst.OR_n(A); goto end;
  CP_n_B:  inst.CP_n(B); goto end;
  CP_n_C:  inst.CP_n(C); goto end;
  CP_n_D:  inst.CP_n(D); goto end;
  CP_n_E:  inst.CP_n(E); goto end;
  CP_n_H:  inst.CP_n(H); goto end;
  CP_n_L:  inst.CP_n(L); goto end;
  CP_n_c_HL:  inst.CP_n(c_HL); goto end;
  CP_n_A:  inst.CP_n(A); goto end;

  RET_cc_f_Z_0:  inst.RET_cc(f_Z, 0); goto end;
  POP_nn_BC:  inst.POP_nn(BC); goto end;
  JP_cc_nn_f_Z_0:  inst.JP_cc_nn(f_Z, 0); goto end;
  JP_nn:  inst.JP_nn(); goto end;
  CALL_cc_nn_f_Z_0:  inst.CALL_cc_nn(f_Z, 0); goto end;
  PUSH_nn_BC:  inst.PUSH_nn(BC); goto end;
  ADD_A_n_$:  inst.ADD_A_n($); goto end;
  RST_n_0x00:  inst.RST_n(0x00); goto end;
  RET_cc_f_Z_1:  inst.RET_cc(f_Z, 1); goto end;
  RET:  inst.RET(); goto end;
  JP_cc_nn_f_Z_1:  inst.JP_cc_nn(f_Z, 1); goto end;
  OpCodeCB:  OpCodeCB(&inst); goto end;
  CALL_cc_nn_f_Z_1:  inst.CALL_cc_nn(f_Z, 1); goto end;
  CALL_nn:  inst.CALL_nn(); goto end;
  ADC_A_n_$:  inst.ADC_A_n($); goto end;
  RST_n_0x08:  inst.RST_n(0x08); goto end;

  RET_cc_f_C_0:  inst.RET_cc(f_C, 0); goto end;
  POP_nn_DE:  inst.POP_nn(DE); goto end;
  JP_cc_nn_f_C_0:  inst.JP_cc_nn(f_C, 0); goto end;
  CALL_cc_nn_f_C_0:  inst.CALL_cc_nn(f_C, 0); goto end;
  PUSH_nn_DE:  inst.PUSH_nn(DE); goto end;
  SUB_n_$:  inst.SUB_n($); goto end;
  RST_n_0x10:  inst.RST_n(0x10); goto end;
  RET_cc_f_C_1:  inst.RET_cc(f_C, 1); goto end;
  RETI:  inst.RETI(); goto end;
  JP_cc_nn_f_C_1:  inst.JP_cc_nn(f_C, 1); goto end;
  CALL_cc_nn_f_C_1:  inst.CALL_cc_nn(f_C, 1); goto end;
  SBC_A_$:  inst.SBC_A($); goto end;
  RST_n_0x18:  inst.RST_n(0x18); goto end;

  LDH_c$_A:  inst.LDH_c$_A(); goto end;
  POP_nn_HL:  inst.POP_nn(HL); goto end;
  LD_cC_A:  inst.LD_cC_A(); goto end;
  PUSH_nn_HL:  inst.PUSH_nn(HL); goto end;
  AND_$:  inst.AND($); goto end;
  RST_n_0x20:  inst.RST_n(0x20); goto end;
  ADD_SP_n:  inst.ADD_SP_n(); goto end;
  JP_HL:  inst.JP_HL(); goto end;
  LD_n_A_c_$$:  inst.LD_n_A(c_$$); goto end;
  XOR_n_$:  inst.XOR_n($); goto end;
  RST_n_0x28:  inst.RST_n(0x28); goto end;

  LDH_A_n:  inst.LDH_A_n(); goto end;
  POP_nn_AF:  inst.POP_nn(AF); goto end;
  LD_A_cC:  inst.LD_A_cC(); goto end;
  DI:  inst.DI(); goto end;
  PUSH_nn_AF:  inst.PUSH_nn(AF); goto end;
  OR_n_$:  inst.OR_n($); goto end;
  RST_n_0x30:  inst.RST_n(0x30); goto end;
  LDHL_SP_n:  inst.LDHL_SP_n(); goto end;
  LD_SP_HL:  inst.LD_SP_HL(); goto end;
  LD_A_n_c_$$:  inst.LD_A_n(c_$$); goto end;
  EI:  inst.EI(); goto end;
  CP_n_$:  inst.CP_n($); goto end;
  RST_n_0x38:  inst.RST_n(0x38); goto end;
  NOT_IMPL: goto end;

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

      ptr = dispatch[OpCode];
      goto *ptr;
    } // end if (!Get_Halt())

end:
    #ifdef INSTLOG
      if (OpCode != 0xCB)
      {
          logFile << setfill('0') << setw(2) << uppercase << hex << (int)OpCode << std::endl;
      } else {
          logFile << setfill('0') << setw(2) << uppercase << hex << (int)OpCode
          << setfill('0') << setw(2) << uppercase << hex << (int)NextOpcode << std::endl;
      }
      logFile.flush();
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
