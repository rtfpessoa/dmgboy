#include "Registers.h"
#include <sstream>
using namespace std;

Registers::Registers() {ResetRegs();}

Registers::~Registers()
{
}

Registers *Registers::GetPtrRegisters() {return this;}

BYTE Registers::Get_A()				{return this->af.simple[1];}
void Registers::Set_A(BYTE value)	{this->af.simple[1] = value;}
BYTE Registers::Get_B()				{return this->bc.simple[1];}
void Registers::Set_B(BYTE value)	{this->bc.simple[1] = value;}
BYTE Registers::Get_C()				{return this->bc.simple[0];}
void Registers::Set_C(BYTE value)	{this->bc.simple[0] = value;}
BYTE Registers::Get_D()				{return this->de.simple[1];}
void Registers::Set_D(BYTE value)	{this->de.simple[1] = value;}
BYTE Registers::Get_E()				{return this->de.simple[0];}
void Registers::Set_E(BYTE value)	{this->de.simple[0] = value;}
BYTE Registers::Get_F()				{return this->af.simple[0];}
void Registers::Set_F(BYTE value)	{this->af.simple[0] = value;}
BYTE Registers::Get_H()				{return this->hl.simple[1];}
void Registers::Set_H(BYTE value)	{this->hl.simple[1] = value;}
BYTE Registers::Get_L()				{return this->hl.simple[0];}
void Registers::Set_L(BYTE value)	{this->hl.simple[0] = value;}

WORD Registers::Get_AF()			{return this->af.doble;}
void Registers::Set_AF(WORD value)	{this->af.doble = value;}
WORD Registers::Get_BC()			{return this->bc.doble;}
void Registers::Set_BC(WORD value)	{this->bc.doble = value;}
WORD Registers::Get_DE()			{return this->de.doble;}
void Registers::Set_DE(WORD value)	{this->de.doble = value;}
WORD Registers::Get_HL()			{return this->hl.doble;}
void Registers::Set_HL(WORD value)	{this->hl.doble = value;}

WORD Registers::Get_PC()			{return this->pc;}
void Registers::Set_PC(WORD value)  {this->pc = value;}
void Registers::Add_PC(int value)  {this->pc += value;};
WORD Registers::Get_SP()			{return this->sp;}
void Registers::Set_SP(WORD value)  {this->sp = value;}
void Registers::Add_SP(int value)  {this->sp += value;};

bool Registers::Get_IME()			{return this->IME;}
void Registers::Set_IME(bool value)	{this->IME = value;}

bool Registers::Get_Halt()				{return this->halt;}
void Registers::Set_Halt(bool value)	{this->halt = value;}

bool Registers::Get_Stop()				{return this->stop;}
void Registers::Set_Stop(bool value)	{this->stop = value;}

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
			out << "Get_Reg - Error, registro incorrecto: " << reg << "\n";
			//throw exception(out.str().data());
			throw exception();
	}
}

void Registers::Set_Reg(e_registers reg, WORD value)
{
	switch (reg){
		case A: this->Set_A(value); break;
		case B: this->Set_B(value); break;
		case C: this->Set_C(value); break;
		case D: this->Set_D(value); break;
		case E: this->Set_E(value); break;
		case F: this->Set_F(value); break;
		case H: this->Set_H(value); break;
		case L: this->Set_L(value); break;
		case AF: this->Set_AF(value); break;
		case BC: this->Set_BC(value); break;
		case DE: this->Set_DE(value); break;
		case HL: this->Set_HL(value); break;
		case PC: this->Set_PC(value); break;
		case SP: this->Set_SP(value); break;
		default:
			stringstream out;
			out << "Set_Reg - Error, registro incorrecto: " << reg << "\n";
			//throw exception(out.str().data());
			throw exception();
	}
}

BYTE Registers::Get_flagZ() {return (this->af.simple[0] >> 7);}
void Registers::Set_flagZ(BYTE value) {this->af.simple[0] = (this->af.simple[0] & 0x7F) | (value << 7);}
BYTE Registers::Get_flagN() {return ((this->af.simple[0] & 0x40) >> 6);}
void Registers::Set_flagN(BYTE value) {this->af.simple[0] = (this->af.simple[0] & 0xBF) | (value << 6);}
BYTE Registers::Get_flagH() {return ((this->af.simple[0] & 0x20) >> 5);}
void Registers::Set_flagH(BYTE value) {this->af.simple[0] = (this->af.simple[0] & 0xDF) | (value << 5);}
BYTE Registers::Get_flagC() {return ((this->af.simple[0] & 0x10) >> 4);}
void Registers::Set_flagC(BYTE value) {this->af.simple[0] = (this->af.simple[0] & 0xEF) | (value << 4);}

BYTE Registers::Get_Flag(e_registers flag)
{
	switch (flag){
		case f_C: return this->Get_flagC();
		case f_H: return this->Get_flagH();
		case f_N: return this->Get_flagN();
		case f_Z: return this->Get_flagZ();
		default:
			stringstream out;
			out << "Error, flag incorrecto (Get): " << flag << "\n";
			//throw exception(out.str().data());
			throw exception();
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
			out << "Error, flag incorrecto (Set): " << flag << "\n";
			//throw exception(out.str().data());
			throw exception();
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
