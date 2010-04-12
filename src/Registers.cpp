#include "Registers.h"
#include <sstream>
#include "GBException.h"
#include <iomanip>
using namespace std;

Registers::Registers() {ResetRegs();}

Registers::~Registers()
{
}

Registers *Registers::GetPtrRegisters() {return this;}

WORD Registers::Get_Reg(e_registers reg)
{
	switch (reg){
		case A: return this->Get_A(); break;
		case B: return this->Get_B(); break;
		case C: return this->Get_C(); break;
		case D: return this->Get_D(); break;
		case E: return this->Get_E(); break;
		case F: return this->Get_F(); break;
		case H: return this->Get_H(); break;
		case L: return this->Get_L(); break;
		case AF: return this->Get_AF(); break;
		case BC: return this->Get_BC(); break;
		case DE: return this->Get_DE(); break;
		case HL: return this->Get_HL(); break;
		case PC: return this->Get_PC(); break;
		case SP: return this->Get_SP(); break;
		default:
			stringstream out;
			out << "Get_Reg - Error, registro incorrecto: " << reg << endl;
			throw GBException(out.str().data());
	}
}

void Registers::Set_Reg(e_registers reg, WORD value)
{
	switch (reg){
		case A: this->Set_A((BYTE)value); break;
		case B: this->Set_B((BYTE)value); break;
		case C: this->Set_C((BYTE)value); break;
		case D: this->Set_D((BYTE)value); break;
		case E: this->Set_E((BYTE)value); break;
		case F: this->Set_F((BYTE)value); break;
		case H: this->Set_H((BYTE)value); break;
		case L: this->Set_L((BYTE)value); break;
		case AF: this->Set_AF(value); break;
		case BC: this->Set_BC(value); break;
		case DE: this->Set_DE(value); break;
		case HL: this->Set_HL(value); break;
		case PC: this->Set_PC(value); break;
		case SP: this->Set_SP(value); break;
		default:
			stringstream out;
			out << "Set_Reg - Error, registro incorrecto: " << reg << endl;
			throw GBException(out.str().data());
	}
}

BYTE Registers::Get_Flag(e_registers flag)
{
	switch (flag){
		case f_C: return this->Get_flagC();
		case f_H: return this->Get_flagH();
		case f_N: return this->Get_flagN();
		case f_Z: return this->Get_flagZ();
		default:
			stringstream out;
			out << "Error, flag incorrecto (Get): " << flag << endl;
			throw GBException(out.str().data());
	}
}

void Registers::Set_Flag(e_registers flag, BYTE value)
{
	switch (flag){
		case f_C: this->Set_flagC(value);
		case f_H: this->Set_flagH(value);
		case f_N: this->Set_flagN(value);
		case f_Z: this->Set_flagZ(value);
		default:
			stringstream out;
			out << "Error, flag incorrecto (Set): " << flag << endl;
			throw GBException(out.str().data());
	}
}

void Registers::ResetRegs()
{
	this->Set_AF(0x01B0);
	this->Set_BC(0x0013);
	this->Set_DE(0x00D8);
	this->Set_HL(0x014D);
	this->Set_PC(0x0100);
	this->Set_SP(0xFFFE);
	this->Set_Halt(false);
	this->Set_Stop(false);
	this->Set_IME(false);
}

string Registers::ToString()
{
	stringstream out;
	
	out << "PC:" << setfill('0') << setw(4) << uppercase << hex << (int)Get_PC()
		<< ", AF: " << setfill('0') << setw(4) << uppercase << hex << (int)Get_AF()
		<< ", BC: " << setfill('0') << setw(4) << uppercase << hex << (int)Get_BC()
		<< ", DE: " << setfill('0') << setw(4) << uppercase << hex << (int)Get_DE()
		<< ", HL:"  << setfill('0') << setw(4) << uppercase << hex << (int)Get_HL()
		<< ", SP: " << setfill('0') << setw(4) << uppercase << hex << (int) Get_SP()
		<< ", H: " << Get_Halt() << ", S: " << Get_Stop() << ", I: " << Get_IME();
	
	return out.str();
}
