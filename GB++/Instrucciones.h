#ifndef __INSTRUCCIONES_H__
#define __INSTRUCCIONES_H__

#include "Registers.h"
#include "Memory.h"

class Instrucciones
{
private:
	Registers *reg;
	Memory *mem;

	void PUSH_PC();
public:
	Instrucciones(Registers *reg, Memory *mem);
	~Instrucciones(void);
	
	void ADC_A_n(e_registers lugar);
	void ADD_A_n(e_registers lugar);
	void ADD_HL_n(e_registers lugar);
	void ADD_SP_n();
	void AND(e_registers lugar);
	void BIT_b_r(BYTE bit, e_registers lugar);
	void CALL_nn();
	void CALL_cc_nn(e_registers flag, BYTE value2check);
	void CCF();
	void CP_n(e_registers lugar);
	void CPL();
	void DAA();
	void DEC_n(e_registers lugar);
    void DEC_nn(e_registers lugar);
	void DI();
	void EI();
	void HALT();
	void INC_n(e_registers lugar);
    void INC_nn(e_registers lugar);
	void JP_nn();
	void JP_cc_nn(e_registers flag, BYTE value2check);
	void JP_HL();
	void JR();
    void JR_CC_n(e_registers flag, BYTE value2check);
	void LDD_A_HL();
    void LDD_HL_A();
	void LDH_n_A();
	void LDH_A_n();
	void LDHL_SP_n();
	void LDI_A_HL();
    void LDI_HL_A();
	void LD_n_A(e_registers lugar);
    void LD_n_nn(e_registers lugar);
	void LD_nn_n(e_registers lugar);
	void LD_A_n(e_registers lugar);
	void LD_A_C();
    void LD_C_A();
	void LD_nn_SP();
	void LD_r1_r2(e_registers e_reg1, e_registers e_reg2);
	void LD_SP_HL();
	void NOP();
    void OR_n(e_registers lugar);
	void POP_nn(e_registers lugar);
    void PUSH_nn(e_registers lugar);
	void RES_b_r(BYTE bit, e_registers lugar);
	void RET();
	void RETI();
    void RET_cc(e_registers flag, BYTE value2check);
	void RL_n(e_registers lugar);
	//void RLA();
	void RLCA();
	void RLC_n(e_registers lugar);
	void RR_n(e_registers lugar);
	void RRC_n(e_registers lugar);
	//void RRCA();
	void RST_n(BYTE desp);
    void SBC_A(e_registers lugar);
	void SET_b_r(BYTE bit, e_registers lugar);
	void SCF();
    void SLA_n(e_registers lugar);
	void SRA_n(e_registers lugar);
    void SRL_n(e_registers lugar);
	void STOP();
	void SUB_n(e_registers lugar);
	void SWAP(e_registers lugar);
	void XOR_n(e_registers lugar);
};

#endif