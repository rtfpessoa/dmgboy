#ifndef __INSTRUCCIONES_H__
#define __INSTRUCCIONES_H__

#include "Registros.h"
#include "Memoria.h"

class Instrucciones
{
private:
	Registros *reg;
	Memoria *mem;

	void PUSH_PC();
public:
	Instrucciones(Registros *reg, Memoria *mem);
	~Instrucciones(void);
	
	void ADC_A_n(e_registros lugar);
	void ADD_A_n(e_registros lugar);
	void ADD_HL_n(e_registros lugar);
	void ADD_SP_n();
	void AND(e_registros lugar);
	void BIT_b_r(BYTE bit, e_registros lugar);
	void CALL_nn();
	void CALL_cc_nn(e_registros flag, BYTE value2check);
	void CCF();
	void CP_n(e_registros lugar);
	void CPL();
	void DAA();
	void DEC_n(e_registros lugar);
    void DEC_nn(e_registros lugar);
	void DI();
	void EI();
	void HALT();
	void INC_n(e_registros lugar);
    void INC_nn(e_registros lugar);
	void JP_nn();
	void JP_cc_nn(e_registros flag, BYTE value2check);
	void JP_HL();
	void JR();
    void JR_CC_n(e_registros flag, BYTE value2check);
	void LDD_A_HL();
    void LDD_HL_A();
	void LDH_n_A();
	void LDH_A_n();
	void LDHL_SP_n();
	void LDI_A_HL();
    void LDI_HL_A();
	void LD_n_A(e_registros lugar);
    void LD_n_nn(e_registros lugar);
	void LD_nn_n(e_registros lugar);
	void LD_A_n(e_registros lugar);
	void LD_A_C();
    void LD_C_A();
	void LD_nn_SP();
	void LD_r1_r2(e_registros e_reg1, e_registros e_reg2);
	void LD_SP_HL();
	void NOP();
    void OR_n(e_registros lugar);
	void POP_nn(e_registros lugar);
    void PUSH_nn(e_registros lugar);
	void RES_b_r(BYTE bit, e_registros lugar);
	void RET();
	void RETI();
    void RET_cc(e_registros flag, BYTE value2check);
	void RL_n(e_registros lugar);
	//void RLA();
	void RLCA();
	void RLC_n(e_registros lugar);
	void RR_n(e_registros lugar);
	void RRC_n(e_registros lugar);
	//void RRCA();
	void RST_n(BYTE desp);
    void SBC_A(e_registros lugar);
	void SET_b_r(BYTE bit, e_registros lugar);
	void SCF();
    void SLA_n(e_registros lugar);
	void SRA_n(e_registros lugar);
    void SRL_n(e_registros lugar);
	void STOP();
	void SUB_n(e_registros lugar);
	void SWAP(e_registros lugar);
	void XOR_n(e_registros lugar);
};

#endif