#ifndef __INSTRUCCIONES_H__
#define __INSTRUCCIONES_H__

#include "Registers.h"
#include "Memory.h"

class Instructions
{
private:
	Registers *reg;
	Memory *mem;

	void PUSH_PC();
public:
	Instructions(Registers *reg, Memory *mem);
	~Instructions(void);
	
	void ADC_A_n(e_registers place);
	void ADD_A_n(e_registers place);
	void ADD_HL_n(e_registers place);
	void ADD_SP_n();
	void AND(e_registers place);
	void BIT_b_r(BYTE bit, e_registers place);
	void CALL_nn();
	void CALL_cc_nn(e_registers flag, BYTE value2check);
	void CCF();
	void CP_n(e_registers place);
	void CPL();
	void DAA();
	void DEC_n(e_registers place);
    void DEC_nn(e_registers place);
	void DI();
	void EI();
	void HALT();
	void INC_n(e_registers place);
    void INC_nn(e_registers place);
	void JP_nn();
	void JP_cc_nn(e_registers flag, BYTE value2check);
	void JP_HL();
	void JR();
    void JR_CC_n(e_registers flag, BYTE value2check);
	void LDD_A_cHL();
    void LDD_cHL_A();
	void LDH_c$_A();
	void LDH_A_n();
	void LDHL_SP_n();
	void LDI_A_cHL();
    void LDI_cHL_A();
	void LD_n_A(e_registers place);
    void LD_n_nn(e_registers place);
	void LD_nn_n(e_registers place);
	void LD_A_n(e_registers place);
	void LD_A_cC();
    void LD_cC_A();
	void LD_nn_SP();
	void LD_r1_r2(e_registers e_reg1, e_registers e_reg2);
	void LD_SP_HL();
	void NOP();
    void OR_n(e_registers place);
	void POP_nn(e_registers place);
    void PUSH_nn(e_registers place);
	void RES_b_r(BYTE bit, e_registers place);
	void RET();
	void RETI();
    void RET_cc(e_registers flag, BYTE value2check);
	void RL_n(e_registers place);
	//void RLA();
	void RLCA();
	void RLC_n(e_registers place);
	void RR_n(e_registers place);
	void RRC_n(e_registers place);
	//void RRCA();
	void RST_n(BYTE desp);
    void SBC_A(e_registers place);
	void SET_b_r(BYTE bit, e_registers place);
	void SCF();
    void SLA_n(e_registers place);
	void SRA_n(e_registers place);
    void SRL_n(e_registers place);
	void STOP();
	void SUB_n(e_registers place);
	void SWAP(e_registers place);
	void XOR_n(e_registers place);
};

#endif