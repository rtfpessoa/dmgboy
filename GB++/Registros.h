#ifndef __REGISTROS_H__
#define __REGISTROS_H__

#include "StdAfx.h"

class Registros
{
private:
	u_registro af, bc, de, hl;
	WORD pc; //Program Counter
	WORD sp; //Stack Pointer
	bool IME;
	bool halt;
	bool stop;
public:
	Registros();
	~Registros();

	Registros *GetPtrRegistros();

	BYTE Get_A();
	void Set_A(BYTE valor);
	BYTE Get_B();
	void Set_B(BYTE valor);
	BYTE Get_C();
	void Set_C(BYTE valor);
	BYTE Get_D();
	void Set_D(BYTE valor);
	BYTE Get_E();
	void Set_E(BYTE valor);
	BYTE Get_F();
	void Set_F(BYTE valor);
	BYTE Get_H();
	void Set_H(BYTE valor);
	BYTE Get_L();
	void Set_L(BYTE valor);

	WORD Get_AF();
	void Set_AF(WORD valor);
	WORD Get_BC();
	void Set_BC(WORD valor);
	WORD Get_DE();
	void Set_DE(WORD valor);
	WORD Get_HL();
	void Set_HL(WORD valor);

	WORD Get_PC();
	void Set_PC(WORD valor);
	void Add_PC(int valor);
	WORD Get_SP();
	void Set_SP(WORD valor);
	void Add_SP(int valor);

	bool Get_IME();
	void Set_IME(bool valor);

	bool Get_Halt();
	void Set_Halt(bool valor);

	bool Get_Stop();
	void Set_Stop(bool valor);
	
	WORD Get_Reg(e_registros registro);
	void Set_Reg(e_registros registro, WORD valor);
	
	BYTE Get_flagZ();
	void Set_flagZ(BYTE valor);
	BYTE Get_flagN();
	void Set_flagN(BYTE valor);
	BYTE Get_flagH();
	void Set_flagH(BYTE valor);
	BYTE Get_flagC();
	void Set_flagC(BYTE valor);

	BYTE Get_Flag(e_registros flag);
	void Set_Flag(e_registros flag, BYTE valor);

	void ResetRegs();
};

#endif