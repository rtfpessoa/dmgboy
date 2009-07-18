#ifndef __REGISTERS_H__
#define __REGISTERS_H__

#include "Def.h"

enum e_registers {
	A = 0x00, B, C, D, E, F, H, L,	//registros simples
	AF = 0x10, BC, DE, HL,			//registros dobles
	f_Z = 0x20, f_N, f_H, f_C,		//flags
	PC = 0x30, SP, $, c_$$,
	c_BC = 0x40, c_DE, c_HL,		//memoria apuntada por el registro doble
};

union u_register{
	WORD doble;
	BYTE simple[2];
};

class Registers
{
private:
	u_register af, bc, de, hl;
	WORD pc; //Program Counter
	WORD sp; //Stack Pointer
	bool IME;
	bool halt;
	bool stop;
public:
	Registers();
	~Registers();

	Registers *GetPtrRegisters();

	inline BYTE Get_A();
	inline void Set_A(BYTE value);
	inline BYTE Get_B();
	inline void Set_B(BYTE value);
	inline BYTE Get_C();
	inline void Set_C(BYTE value);
	inline BYTE Get_D();
	inline void Set_D(BYTE value);
	inline BYTE Get_E();
	inline void Set_E(BYTE value);
	inline BYTE Get_F();
	inline void Set_F(BYTE value);
	inline BYTE Get_H();
	inline void Set_H(BYTE value);
	inline BYTE Get_L();
	inline void Set_L(BYTE value);

	inline WORD Get_AF();
	inline void Set_AF(WORD value);
	inline WORD Get_BC();
	inline void Set_BC(WORD value);
	inline WORD Get_DE();
	inline void Set_DE(WORD value);
	inline WORD Get_HL();
	inline void Set_HL(WORD value);

	inline WORD Get_PC();
	inline void Set_PC(WORD value);
	void Add_PC(int value);
	inline WORD Get_SP();
	inline void Set_SP(WORD value);
	void Add_SP(int value);

	bool Get_IME();
	inline void Set_IME(bool value);

	bool Get_Halt();
	inline void Set_Halt(bool value);

	bool Get_Stop();
	inline void Set_Stop(bool value);

	WORD Get_Reg(e_registers reg);
	void Set_Reg(e_registers reg, WORD value);

	inline BYTE Get_flagZ();
	inline void Set_flagZ(BYTE value);
	inline BYTE Get_flagN();
	inline void Set_flagN(BYTE value);
	inline BYTE Get_flagH();
	inline void Set_flagH(BYTE value);
	inline BYTE Get_flagC();
	inline void Set_flagC(BYTE value);

	BYTE Get_Flag(e_registers flag);
	void Set_Flag(e_registers flag, BYTE value);

	void ResetRegs();
};

#endif
