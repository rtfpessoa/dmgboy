#include "CPU.h"
#include "Instructions.h"
#include <iomanip>
#include <sstream>
#include "Registers.h"
#include "GBException.h"

using namespace std;

CPU::CPU(Video *v, Pad *p, Cartridge *c)
{
	cyclesLCD = 0;
	this->v = v;
	v->SetMem(this->GetPtrMemory());
	this->p = p;
	SetPad(p);
	LoadCartridge(c);
}

CPU::~CPU()
{
}

void CPU::Run()
{
	Interpreter();
}

void CPU::Reset()
{
	ResetRegs();
	ResetMem();
}

void CPU::Interpreter()
{
	BYTE OpCode = 0, NextOpcode = 0, lastOpCode = 0;

	Instructions inst(this->GetPtrRegisters(), this->GetPtrMemory());

    for(;;)
    {
		lastOpCode = OpCode;
		OpCode = MemR(Get_PC());
        NextOpcode = MemR(Get_PC() + 1);
        //Counter-=Cycles[OpCode];
		if (!Get_Halt() && !Get_Stop())
			switch(OpCode)
			{
				case (0x00): inst.NOP(); break;
				case (0x01): inst.LD_n_nn(BC); break;
				case (0x02): inst.LD_n_A(c_BC); break;
				case (0x03): inst.INC_nn(BC); break;
				case (0x04): inst.INC_n(B); break;
				case (0x05): inst.DEC_n(B); break;
				case (0x06): inst.LD_nn_n(B); break;
				case (0x07): inst.RLCA(); break;
				case (0x08): inst.LD_nn_SP(); break;
				case (0x09): inst.ADD_HL_n(BC); break;
				case (0x0A): inst.LD_A_n(c_BC); break;
				case (0x0B): inst.DEC_nn(BC); break;
				case (0x0C): inst.INC_n(C); break;
				case (0x0D): inst.DEC_n(C); break;
				case (0x0E): inst.LD_nn_n(C); break;
				case (0x0F): inst.RRC_n(A); break;

				case (0x10): inst.STOP(); break;
				case (0x11): inst.LD_n_nn(DE); break;
				case (0x12): inst.LD_n_A(c_DE); break;
				case (0x13): inst.INC_nn(DE); break;
				case (0x14): inst.INC_n(D); break;
				case (0x15): inst.DEC_n(D); break;
				case (0x16): inst.LD_nn_n(D); break;
				case (0x17): inst.RL_n(A); break;
				case (0x18): inst.JR(); break;
				case (0x19): inst.ADD_HL_n(DE); break;
				case (0x1A): inst.LD_A_n(c_DE); break;
				case (0x1B): inst.DEC_nn(DE); break;
				case (0x1C): inst.INC_n(E); break;
				case (0x1D): inst.DEC_n(E); break;
				case (0x1E): inst.LD_nn_n(E); break;
				case (0x1F): inst.RR_n(A); break;

				case (0x20): inst.JR_CC_n(f_Z, 0); break;
				case (0x21): inst.LD_n_nn(HL); break;
				case (0x22): inst.LDI_cHL_A(); break;
				case (0x23): inst.INC_nn(HL); break;
				case (0x24): inst.INC_n(H); break;
				case (0x25): inst.DEC_n(H); break;
				case (0x26): inst.LD_nn_n(H); break;
				case (0x27): inst.DAA(); break;
				case (0x28): inst.JR_CC_n(f_Z, 1); break;
				case (0x29): inst.ADD_HL_n(HL); break;
				case (0x2A): inst.LDI_A_cHL(); break;
				case (0x2B): inst.DEC_nn(HL); break;
				case (0x2C): inst.INC_n(L); break;
				case (0x2D): inst.DEC_n(L); break;
				case (0x2E): inst.LD_nn_n(L); break;
				case (0x2F): inst.CPL(); break;

				case (0x30): inst.JR_CC_n(f_C, 0); break;
				case (0x31): inst.LD_n_nn(SP); break;
				case (0x32): inst.LDD_cHL_A(); break;
				case (0x33): inst.INC_nn(SP); break;
				case (0x34): inst.INC_n(c_HL); break;
				case (0x35): inst.DEC_n(c_HL); break;
				case (0x36): inst.LD_r1_r2(c_HL, $); break;
				case (0x37): inst.SCF(); break;
				case (0x38): inst.JR_CC_n(f_C, 1); break;
				case (0x39): inst.ADD_HL_n(SP); break;
				case (0x3A): inst.LDD_A_cHL(); break;
				case (0x3B): inst.DEC_nn(SP); break;
				case (0x3C): inst.INC_n(A); break;
				case (0x3D): inst.DEC_n(A); break;
				case (0x3E): inst.LD_A_n($); break;
				case (0x3F): inst.CCF(); break;

				case (0x40): inst.LD_r1_r2(B, B); break;
				case (0x41): inst.LD_r1_r2(B, C); break;
				case (0x42): inst.LD_r1_r2(B, D); break;
				case (0x43): inst.LD_r1_r2(B, E); break;
				case (0x44): inst.LD_r1_r2(B, H); break;
				case (0x45): inst.LD_r1_r2(B, L); break;
				case (0x46): inst.LD_r1_r2(B, c_HL); break;
				case (0x47): inst.LD_n_A(B); break;
				case (0x48): inst.LD_r1_r2(C, B); break;
				case (0x49): inst.LD_r1_r2(C, C); break;
				case (0x4A): inst.LD_r1_r2(C, D); break;
				case (0x4B): inst.LD_r1_r2(C, E); break;
				case (0x4C): inst.LD_r1_r2(C, H); break;
				case (0x4D): inst.LD_r1_r2(C, L); break;
				case (0x4E): inst.LD_r1_r2(C, c_HL); break;
				case (0x4F): inst.LD_n_A(C); break;

				case (0x50): inst.LD_r1_r2(D, B); break;
				case (0x51): inst.LD_r1_r2(D, C); break;
				case (0x52): inst.LD_r1_r2(D, D); break;
				case (0x53): inst.LD_r1_r2(D, E); break;
				case (0x54): inst.LD_r1_r2(D, H); break;
				case (0x55): inst.LD_r1_r2(D, L); break;
				case (0x56): inst.LD_r1_r2(D, c_HL); break;
				case (0x57): inst.LD_n_A(D); break;
				case (0x58): inst.LD_r1_r2(E, B); break;
				case (0x59): inst.LD_r1_r2(E, C); break;
				case (0x5A): inst.LD_r1_r2(E, D); break;
				case (0x5B): inst.LD_r1_r2(E, E); break;
				case (0x5C): inst.LD_r1_r2(E, H); break;
				case (0x5D): inst.LD_r1_r2(E, L); break;
				case (0x5E): inst.LD_r1_r2(E, c_HL); break;
				case (0x5F): inst.LD_n_A(E); break;

				case (0x60): inst.LD_r1_r2(H, B); break;
				case (0x61): inst.LD_r1_r2(H, C); break;
				case (0x62): inst.LD_r1_r2(H, D); break;
				case (0x63): inst.LD_r1_r2(H, E); break;
				case (0x64): inst.LD_r1_r2(H, H); break;
				case (0x65): inst.LD_r1_r2(H, L); break;
				case (0x66): inst.LD_r1_r2(H, c_HL); break;
				case (0x67): inst.LD_n_A(H); break;
				case (0x68): inst.LD_r1_r2(L, B); break;
				case (0x69): inst.LD_r1_r2(L, C); break;
				case (0x6A): inst.LD_r1_r2(L, D); break;
				case (0x6B): inst.LD_r1_r2(L, E); break;
				case (0x6C): inst.LD_r1_r2(L, H); break;
				case (0x6D): inst.LD_r1_r2(L, L); break;
				case (0x6E): inst.LD_r1_r2(L, c_HL); break;
				case (0x6F): inst.LD_n_A(L); break;

				case (0x70): inst.LD_r1_r2(c_HL, B); break;
				case (0x71): inst.LD_r1_r2(c_HL, C); break;
				case (0x72): inst.LD_r1_r2(c_HL, D); break;
				case (0x73): inst.LD_r1_r2(c_HL, E); break;
				case (0x74): inst.LD_r1_r2(c_HL, H); break;
				case (0x75): inst.LD_r1_r2(c_HL, L); break;
				case (0x76): inst.HALT(); break;
				case (0x77): inst.LD_n_A(c_HL); break;
				case (0x78): inst.LD_A_n(B); break;
				case (0x79): inst.LD_A_n(C); break;
				case (0x7A): inst.LD_A_n(D); break;
				case (0x7B): inst.LD_A_n(E); break;
				case (0x7C): inst.LD_A_n(H); break;
				case (0x7D): inst.LD_A_n(L); break;
				case (0x7E): inst.LD_A_n(c_HL); break;
				case (0x7F): inst.LD_n_A(A); break;

				case (0x80): inst.ADD_A_n(B); break;
				case (0x81): inst.ADD_A_n(C); break;
				case (0x82): inst.ADD_A_n(D); break;
				case (0x83): inst.ADD_A_n(E); break;
				case (0x84): inst.ADD_A_n(H); break;
				case (0x85): inst.ADD_A_n(L); break;
				case (0x86): inst.ADD_A_n(c_HL); break;
				case (0x87): inst.ADD_A_n(A); break;
				case (0x88): inst.ADC_A_n(B); break;
				case (0x89): inst.ADC_A_n(C); break;
				case (0x8A): inst.ADC_A_n(D); break;
				case (0x8B): inst.ADC_A_n(E); break;
				case (0x8C): inst.ADC_A_n(H); break;
				case (0x8D): inst.ADC_A_n(L); break;
				case (0x8E): inst.ADC_A_n(c_HL); break;
				case (0x8F): inst.ADC_A_n(A); break;

				case (0x90): inst.SUB_n(B); break;
				case (0x91): inst.SUB_n(C); break;
				case (0x92): inst.SUB_n(D); break;
				case (0x93): inst.SUB_n(E); break;
				case (0x94): inst.SUB_n(H); break;
				case (0x95): inst.SUB_n(L); break;
				case (0x96): inst.SUB_n(c_HL); break;
				case (0x97): inst.SUB_n(A); break;
				case (0x98): inst.SBC_A(B); break;
				case (0x99): inst.SBC_A(C); break;
				case (0x9A): inst.SBC_A(D); break;
				case (0x9B): inst.SBC_A(E); break;
				case (0x9C): inst.SBC_A(H); break;
				case (0x9D): inst.SBC_A(L); break;
				case (0x9E): inst.SBC_A(c_HL); break;
				case (0x9F): inst.SBC_A(A); break;

				case (0xA0): inst.AND(B); break;
				case (0xA1): inst.AND(C); break;
				case (0xA2): inst.AND(D); break;
				case (0xA3): inst.AND(E); break;
				case (0xA4): inst.AND(H); break;
				case (0xA5): inst.AND(L); break;
				case (0xA6): inst.AND(c_HL); break;
				case (0xA7): inst.AND(A); break;
				case (0xA8): inst.XOR_n(B); break;
				case (0xA9): inst.XOR_n(C); break;
				case (0xAA): inst.XOR_n(D); break;
				case (0xAB): inst.XOR_n(E); break;
				case (0xAC): inst.XOR_n(H); break;
				case (0xAD): inst.XOR_n(L); break;
				case (0xAE): inst.XOR_n(c_HL); break;
				case (0xAF): inst.XOR_n(A); break;

				case (0xB0): inst.OR_n(B); break;
				case (0xB1): inst.OR_n(C); break;
				case (0xB2): inst.OR_n(D); break;
				case (0xB3): inst.OR_n(E); break;
				case (0xB4): inst.OR_n(H); break;
				case (0xB5): inst.OR_n(L); break;
				case (0xB6): inst.OR_n(c_HL); break;
				case (0xB7): inst.OR_n(A); break;
				case (0xB8): inst.CP_n(B); break;
				case (0xB9): inst.CP_n(C); break;
				case (0xBA): inst.CP_n(D); break;
				case (0xBB): inst.CP_n(E); break;
				case (0xBC): inst.CP_n(H); break;
				case (0xBD): inst.CP_n(L); break;
				case (0xBE): inst.CP_n(c_HL); break;
				case (0xBF): inst.CP_n(A); break;

				case (0xC0): inst.RET_cc(f_Z, 0); break;
				case (0xC1): inst.POP_nn(BC); break;
				case (0xC2): inst.JP_cc_nn(f_Z, 0); break;
				case (0xC3): inst.JP_nn(); break;
				case (0xC4): inst.CALL_cc_nn(f_Z, 0); break;
				case (0xC5): inst.PUSH_nn(BC); break;
				case (0xC6): inst.ADD_A_n($); break;
				case (0xC7): inst.RST_n(0x00); break;
				case (0xC8): inst.RET_cc(f_Z, 1); break;
				case (0xC9): inst.RET(); break;
				case (0xCA): inst.JP_cc_nn(f_Z, 1); break;
				case (0xCB): OpCodeCB(&inst); break;
				case (0xCC): inst.CALL_cc_nn(f_Z, 1); break;
				case (0xCD): inst.CALL_nn(); break;
				case (0xCE): inst.ADC_A_n($); break;
				case (0xCF): inst.RST_n(0x08); break;

				case (0xD0): inst.RET_cc(f_C, 0); break;
				case (0xD1): inst.POP_nn(DE); break;
				case (0xD2): inst.JP_cc_nn(f_C, 0); break;
				case (0xD4): inst.CALL_cc_nn(f_C, 0); break;
				case (0xD5): inst.PUSH_nn(DE); break;
				case (0xD6): inst.SUB_n($); break;
				case (0xD7): inst.RST_n(0x10); break;
				case (0xD8): inst.RET_cc(f_C, 1); break;
				case (0xD9): inst.RETI(); break;
				case (0xDA): inst.JP_cc_nn(f_C, 1); break;
				case (0xDC): inst.CALL_cc_nn(f_C, 1); break;
				case (0xDE): inst.SBC_A($); break;
				case (0xDF): inst.RST_n(0x18); break;

				case (0xE0): inst.LDH_c$_A(); break;
				case (0xE1): inst.POP_nn(HL); break;
				case (0xE2): inst.LD_cC_A(); break;
				case (0xE5): inst.PUSH_nn(HL); break;
				case (0xE6): inst.AND($); break;
				case (0xE7): inst.RST_n(0x20); break;
				case (0xE8): inst.ADD_SP_n(); break;
				case (0xE9): inst.JP_HL(); break;
				case (0xEA): inst.LD_n_A(c_$$); break;
				case (0xEE): inst.XOR_n($); break;
				case (0xEF): inst.RST_n(0x28); break;

				case (0xF0): inst.LDH_A_n(); break;
				case (0xF1): inst.POP_nn(AF); break;
				case (0xF2): inst.LD_A_cC(); break;
				case (0xF3): inst.DI(); break;
				case (0xF5): inst.PUSH_nn(AF); break;
				case (0xF6): inst.OR_n($); break;
				case (0xF7): inst.RST_n(0x30); break;
				case (0xF8): inst.LDHL_SP_n(); break;
				case (0xF9): inst.LD_SP_HL(); break;
				case (0xFA): inst.LD_A_n(c_$$); break;
				case (0xFB): inst.EI(); break;
				case (0xFE): inst.CP_n($); break;
				case (0xFF): inst.RST_n(0x38); break;
				default:
					stringstream out;
					out << "Error, instruccion no implementada: 0x";
					out << setfill('0') << setw(2) << uppercase << hex << (int)OpCode << "\n";
					throw GBException(out.str().data());
			}

        if (OpCode == 0xCB)
            lastCycles = CiclosInstruccion((OpCode << 8) | NextOpcode);
        else
            lastCycles = CiclosInstruccion(OpCode);

		cyclesLCD += lastCycles;
		cyclesTimer += lastCycles;
		cyclesDIV += lastCycles;

        TareasRutinarias();
        Interrupciones(&inst);
	}
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
			out << "Error, instruccion no implementada: 0xCB";
			out << setfill('0') << setw(2) << uppercase << hex << (int)OpCode << "\n";
			throw GBException(out.str().data());
    }
}

BYTE CPU::CiclosInstruccion(WORD OpCode)
{
    BYTE valor;

    switch (OpCode)
    {
        case (0x01): valor = 12; break;
        case (0x02): valor = 8; break;
        case (0x03): valor = 8; break;
        case (0x06): valor = 8; break;
        case (0x08): valor = 20; break;
        case (0x09): valor = 8; break;
        case (0x0A): valor = 8; break;
        case (0x0B): valor = 8; break;
        case (0x0E): valor = 8; break;

        case (0x11): valor = 12; break;
        case (0x12): valor = 8; break;
        case (0x13): valor = 8; break;
        case (0x16): valor = 8; break;
        case (0x18): valor = 8; break;
        case (0x19): valor = 8; break;
        case (0x1A): valor = 8; break;
        case (0x1B): valor = 8; break;
        case (0x1E): valor = 8; break;

        case (0x20): valor = 8; break;
        case (0x21): valor = 12; break;
        case (0x22): valor = 8; break;
        case (0x23): valor = 8; break;
        case (0x26): valor = 8; break;
        case (0x28): valor = 8; break;
        case (0x29): valor = 8; break;
        case (0x2A): valor = 8; break;
        case (0x2B): valor = 8; break;
        case (0x2E): valor = 8; break;

        case (0x30): valor = 8; break;
        case (0x31): valor = 12; break;
        case (0x32): valor = 8; break;
        case (0x33): valor = 8; break;
        case (0x34): valor = 12; break;
        case (0x35): valor = 12; break;
        case (0x36): valor = 12; break;
        case (0x38): valor = 8; break;
        case (0x39): valor = 8; break;
        case (0x3A): valor = 8; break;
        case (0x3B): valor = 8; break;
        case (0x3E): valor = 8; break;

        case (0x46): valor = 8; break;

        case (0x4E): valor = 8; break;

        case (0x56): valor = 8; break;

        case (0x5E): valor = 8; break;

        case (0x66): valor = 8; break;

        case (0x70): valor = 8; break;
        case (0x71): valor = 8; break;
        case (0x72): valor = 8; break;
        case (0x73): valor = 8; break;
        case (0x74): valor = 8; break;
        case (0x77): valor = 8; break;
        case (0x7E): valor = 8; break;

        case (0x86): valor = 8; break;
        case (0x8E): valor = 8; break;

        case (0x96): valor = 8; break;
        case (0x9E): valor = 8; break;

        case (0xA6): valor = 8; break;
        case (0xAE): valor = 8; break;

        case (0xB6): valor = 8; break;
        case (0xDE): valor = 8; break;

        case (0xC0): valor = 8; break;
        case (0xC1): valor = 12; break;
        case (0xC2): valor = 12; break;
        case (0xC4): valor = 12; break;
        case (0xC3): valor = 12; break;
        case (0xC5): valor = 16; break;
        case (0xC6): valor = 8; break;
        case (0xC7): valor = 32; break;
        case (0xC8): valor = 8; break;
        case (0xC9): valor = 8; break;
        case (0xCA): valor = 12; break;
        case (0xCB06): valor = 16; break;
        case (0xCB0E): valor = 16; break;
        case (0xCB16): valor = 16; break;
        case (0xCB1E): valor = 16; break;
        case (0xCB26): valor = 16; break;
        case (0xCB2E): valor = 16; break;
        case (0xCB36): valor = 16; break;
        case (0xCB3E): valor = 16; break;
        case (0xCB46): valor = 12; break;//�16?
        case (0xCB4E): valor = 12; break;//�16?
        case (0xCB56): valor = 12; break;//�16?
        case (0xCB5E): valor = 12; break;//�16?
        case (0xCB66): valor = 12; break;//�16?
        case (0xCB6E): valor = 12; break;//�16?
        case (0xCB76): valor = 12; break;//�16?
        case (0xCB7E): valor = 12; break;//�16?
        case (0xCB86): valor = 16; break;
        case (0xCB8E): valor = 16; break;
        case (0xCB96): valor = 16; break;
        case (0xCB9E): valor = 16; break;
        case (0xCBA6): valor = 16; break;
        case (0xCBAE): valor = 16; break;
        case (0xCBB6): valor = 16; break;
        case (0xCBBE): valor = 16; break;
        case (0xCBC6): valor = 16; break;
        case (0xCBCE): valor = 16; break;
        case (0xCBD6): valor = 16; break;
        case (0xCBDE): valor = 16; break;
        case (0xCBE6): valor = 16; break;
        case (0xCBEE): valor = 16; break;
        case (0xCBF6): valor = 16; break;
        case (0xCBFE): valor = 16; break;
        case (0xCC): valor = 12; break;
        case (0xCD): valor = 12; break;
        case (0xCE): valor = 8; break;
        case (0xCF): valor = 32; break;

        case (0xD0): valor = 8; break;
        case (0xD1): valor = 12; break;
        case (0xD2): valor = 12; break;
        case (0xD4): valor = 12; break;
        case (0xD5): valor = 16; break;
        case (0xD6): valor = 8; break;
        case (0xD7): valor = 32; break;
        case (0xD8): valor = 8; break;
        case (0xD9): valor = 8; break;
        case (0xDA): valor = 12; break;
        case (0xDC): valor = 12; break;
        case (0xDF): valor = 32; break;

        case (0xE0): valor = 12; break;
        case (0xE1): valor = 12; break;
        case (0xE2): valor = 8; break;
        case (0xE5): valor = 16; break;
        case (0xE6): valor = 8; break;
        case (0xE7): valor = 32; break;
        case (0xE8): valor = 16; break;
        case (0xEA): valor = 16; break;
        case (0xEE): valor = 8; break;
        case (0xEF): valor = 32; break;

        case (0xF0): valor = 12; break;
        case (0xF1): valor = 12; break;
        case (0xF2): valor = 8; break;
        case (0xF5): valor = 16; break;
        case (0xF6): valor = 8; break;
        case (0xF7): valor = 32; break;
        case (0xF8): valor = 12; break;
        case (0xF9): valor = 8; break;
        case (0xFA): valor = 16; break;
        case (0xFE): valor = 8; break;
        case (0xFF): valor = 32; break;

        default:
            if (((OpCode & 0xFF00) >> 8) == 0xCB)
                valor = 8;
            else
                valor = 4; break;
    }
    return valor;
}


void CPU::TareasRutinarias()
{
	UpdateStateLCD();
	UpdateTimer();
	eventsSDL();
}

void CPU::UpdateStateLCD()
{
	BYTE mode;

    mode = BITS01(MemR(STAT));

    switch (mode)
    {
        case (0):	//Durante H-Blank
            if (cyclesLCD > MAX_LCD_MODE_0)
            {
                if (MemR(LY) == 144) //Si estamos en la linea 144, cambiamos al modo 1 (V-Blank)
                {
                    //Poner a 01 el flag (bits 0-1) del modo 1.
					MemW(STAT, (MemR(STAT) & ~0x03) | 0x01, false);
					//Interrupcion V-Blank
					MemW(IF, MemR(IF) | 0x01);
					//Si interrupcion V-Blank habilitada, marcar peticion de interrupcion
					//en 0xFF0F. Bit 1, flag de interrupcion de LCD STAT.
					if (BIT4(MemR(STAT)))
						MemW(IF, MemR(IF) | 0x02);
					v->RefreshScreen();
                }
                else    //Sino, cambiamos al modo 2
                {
                    //Poner a 10 el flag (bits 0-1) del modo 2.
					MemW(STAT, (MemR(STAT) & ~0x03) | 0x02, false);
					//Si interrupcion OAM habilitada, marcar peticion de interrupcion
					//en 0xFF0F. Bit 1, flag de interrupcion de LCD STAT
					if (BIT5(MemR(STAT)))
						MemW(IF, MemR(IF) | 0x02);

					v->UpdateLine(MemR(LY));
                }
                MemW(LY, MemR(LY) + 1, false);
                cyclesLCD = 0;
            }
            break;
        case (1):	//Durante V-Blank
            if (cyclesLCD > MAX_LCD_MODE_1)
            {
                //Si hemos llegado al final
                if (MemR(LY) == 153)
                {
                    MemW(LY, 0, false);
                    //Poner a 10 el flag (bits 0-1) del modo 2.
					MemW(STAT, (MemR(STAT) & ~0x03) | 0x02, false);
					//Si interrupcion OAM habilitada, marcar peticion de interrupcion
					//en 0xFF0F. Bit 1, flag de interrupcion de LCD STAT
					if (BIT5(MemR(STAT)))
						MemW(IF, MemR(IF) | 0x02);
                }
                else
                    MemW(LY, MemR(LY) + 1, false);

                cyclesLCD = 0;
            }
            break;
        case (2):	//Cuando OAM se est� usando
            if (cyclesLCD > MAX_LCD_MODE_2)
            {
				//Poner a 11 el flag (bits 0-1) del modo 3.
				MemW(STAT, (MemR(STAT) & ~0x03) | 0x03, false);

                cyclesLCD = 0;
            }
            break;
        case (3):	//Cuando OAM y memoria de video se est�n usando (Se est� pasando informaci�n al LCD)
            if (cyclesLCD > MAX_LCD_MODE_3)
            {
				//Poner a 00 el flag (bits 0-1) del modo 0.
				MemW(STAT, (MemR(STAT) & ~0x03) | 0x00, false);
				//Si interrupcion H-Blank habilitada, marcar peticion de interrupcion
				//en 0xFF0F. Bit 1, flag de interrupcion de LCD STAT
				if (BIT3(MemR(STAT)))
					MemW(IF, MemR(IF) | 0x02);

                cyclesLCD = 0;
            }
            break;
	}

	if (MemR(LY) == MemR(LYC))
	{
		MemW(STAT, MemR(STAT) | 0x40, false);
		if (BIT6(MemR(STAT)))
			MemW(IF, MemR(IF) | 0x02);
	}
	else
		MemW(STAT, MemR(STAT) & ~0x40, false);
}

void CPU::Interrupciones(Instructions * inst)
{
	if (!Get_IME())
		return;

	BYTE valueIE, valueIF;

	valueIE = MemR(IE);
	valueIF = MemR(IF);

	if (BIT0(valueIE) && BIT0(valueIF))	//V-Blank
	{
		Set_IME(false);
		Set_Halt(false);
		Add_PC(-1);
		inst->RST_n(0x40);
		MemW(IF, valueIF & ~0x01);
	}
	else if (BIT1(valueIE) && BIT1(valueIF))	//LCD-Stat
	{
		Set_IME(false);
		Set_Halt(false);
		Add_PC(-1);
		inst->RST_n(0x48);
		MemW(IF, valueIF & ~0x02);
	}
	else if (BIT2(valueIE) && BIT2(valueIF))	//Timer
	{
		Set_IME(false);
		Set_Halt(false);
		Add_PC(-1);
		inst->RST_n(0x50);
		MemW(IF, valueIF & ~0x04);
	}
	else if(BIT3(valueIE) && BIT3(valueIF))	//Serial
	{
		Set_IME(false);
		Set_Halt(false);
		Add_PC(-1);
		inst->RST_n(0x58);
		MemW(IF, valueIF & ~0x08);
	}
	else if (BIT4(valueIE) && BIT4(valueIF))	//Joypad
	{
		Set_IME(false);
		Set_Halt(false);
		Set_Stop(false);
		Add_PC(-1);
		inst->RST_n(0x60);
		MemW(IF, valueIF & ~0x10);
	}
}

void CPU::UpdateTimer()
{
	//int FreqTimer = {4096, 262144, 65536, 16384};
	WORD overflowTimer[] = {1024, 16, 64, 256};

	if (BIT2(MemR(TAC))) //Si est� habilitado el timer
	{
		if (cyclesTimer >= overflowTimer[MemR(TAC) & 0x11])
		{
			if (MemR(TIMA) == 0xFF)
			{
				MemW(TIMA, MemR(TMA));
				MemW(IF, MemR(IF) | 0x04);
			}
			else
				MemW(TIMA, MemR(TIMA) + 1);

			cyclesTimer = 0;
		}
	}

	if (cyclesDIV >= 256)
	{
		MemW(DIV, MemR(DIV) + 1);
		cyclesDIV = 0;
	}
}

void CPU::eventsSDL()
{
	SDL_Event ev;

	while( SDL_PollEvent( &ev ) )
	{
		switch(ev.type)
		{
			case SDL_QUIT:
				printf( ">El usuario quiere salir.\n" );
				exit(0);
				break;
			case SDL_KEYDOWN:
				if (ev.key.keysym.sym == SDLK_ESCAPE)
				{
					printf("Presionar de nuevo ESC para salir o cualquier otra tecla para continuar\n");
					while(true)
					{
						SDL_WaitEvent(&ev);
						if (ev.type == SDL_KEYDOWN)
						{
							if (ev.key.keysym.sym == SDLK_ESCAPE)
								exit(0);
							else
								return;
						}
					}
				}
				//!!No hay break. Cuando SDL_KEYDOWN tambi�n SDL_KEYUP
			case SDL_KEYUP:
				BYTE valueP1 = MemR(P1);
				BYTE interrupt = p->updateKey(ev.type, ev.key.keysym.sym, &valueP1);
				MemW(P1, valueP1, false);
				if (interrupt)
					MemW(IF, MemR(IF) | 0x10);
				break;
		}
	}
}
