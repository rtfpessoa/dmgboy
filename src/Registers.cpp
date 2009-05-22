#include "Registers.h"
#include <sstream>
using namespace std;

Registers::Registers() {ResetRegs();}

Registers::~Registers()
{
}

Registers *Registers::GetPtrRegisters() {return this;}

BYTE Registers::Get_A()				{return this->af.simple[1];}
void Registers::Set_A(BYTE valor)	{this->af.simple[1] = valor;}
BYTE Registers::Get_B()				{return this->bc.simple[1];}
void Registers::Set_B(BYTE valor)	{this->bc.simple[1] = valor;}
BYTE Registers::Get_C()				{return this->bc.simple[0];}
void Registers::Set_C(BYTE valor)	{this->bc.simple[0] = valor;}
BYTE Registers::Get_D()				{return this->de.simple[1];}
void Registers::Set_D(BYTE valor)	{this->de.simple[1] = valor;}
BYTE Registers::Get_E()				{return this->de.simple[0];}
void Registers::Set_E(BYTE valor)	{this->de.simple[0] = valor;}
BYTE Registers::Get_F()				{return this->af.simple[0];}
void Registers::Set_F(BYTE valor)	{this->af.simple[0] = valor;}
BYTE Registers::Get_H()				{return this->hl.simple[1];}
void Registers::Set_H(BYTE valor)	{this->hl.simple[1] = valor;}
BYTE Registers::Get_L()				{return this->hl.simple[0];}
void Registers::Set_L(BYTE valor)	{this->hl.simple[0] = valor;}

WORD Registers::Get_AF()			{return this->af.doble;}
void Registers::Set_AF(WORD valor)	{this->af.doble = valor;}
WORD Registers::Get_BC()			{return this->bc.doble;}
void Registers::Set_BC(WORD valor)	{this->bc.doble = valor;}
WORD Registers::Get_DE()			{return this->de.doble;}
void Registers::Set_DE(WORD valor)	{this->de.doble = valor;}
WORD Registers::Get_HL()			{return this->hl.doble;}
void Registers::Set_HL(WORD valor)	{this->hl.doble = valor;}

inline WORD Registers::Get_PC()			{return this->pc;}
void Registers::Set_PC(WORD valor)  {this->pc = valor;}
void Registers::Add_PC(int valor)  {this->pc += valor;};
WORD Registers::Get_SP()			{return this->sp;}
void Registers::Set_SP(WORD valor)  {this->sp = valor;}
void Registers::Add_SP(int valor)  {this->sp += valor;};

bool Registers::Get_IME()			{return this->IME;}
void Registers::Set_IME(bool valor)	{this->IME = valor;}

bool Registers::Get_Halt()				{return this->halt;}
void Registers::Set_Halt(bool valor)	{this->halt = valor;}

bool Registers::Get_Stop()				{return this->stop;}
void Registers::Set_Stop(bool valor)	{this->stop = valor;}

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
		/*
		case c_bc: return this->Get_BC(); break;	//Realmente estos hacen los mismo que los anteriores,
		case c_de: return this->Get_DE(); break;	//pero se usan para saber que hay que irse a la direccion
		case c_hl: return this->Get_HL(); break;	//en memoria contenida en estos registros
		*/
		default:
			stringstream out;
			out << "Get_Reg - Error, registro incorrecto: " << reg << "\n";
			//throw exception(out.str().data());
			throw exception();
	}
}

void Registers::Set_Reg(e_registers reg, WORD valor)
{
	switch (reg){
		case A: this->Set_A(valor); break;
		case B: this->Set_B(valor); break;
		case C: this->Set_C(valor); break;
		case D: this->Set_D(valor); break;
		case E: this->Set_E(valor); break;
		case F: this->Set_F(valor); break;
		case H: this->Set_H(valor); break;
		case L: this->Set_L(valor); break;
		case AF: this->Set_AF(valor); break;
		case BC: this->Set_BC(valor); break;
		case DE: this->Set_DE(valor); break;
		case HL: this->Set_HL(valor); break;
		case PC: this->Set_PC(valor); break;
		case SP: this->Set_SP(valor); break;
		default:
			stringstream out;
			out << "Set_Reg - Error, registro incorrecto: " << reg << "\n";
			//throw exception(out.str().data());
			throw exception();
	}
}

BYTE Registers::Get_flagZ() {return (this->af.simple[0] >> 7);}
void Registers::Set_flagZ(BYTE valor) {this->af.simple[0] = (this->af.simple[0] & 0x7F) | (valor << 7);}
BYTE Registers::Get_flagN() {return ((this->af.simple[0] & 0x40) >> 6);}
void Registers::Set_flagN(BYTE valor) {this->af.simple[0] = (this->af.simple[0] & 0xBF) | (valor << 6);}
BYTE Registers::Get_flagH() {return ((this->af.simple[0] & 0x20) >> 5);}
void Registers::Set_flagH(BYTE valor) {this->af.simple[0] = (this->af.simple[0] & 0xDF) | (valor << 5);}
BYTE Registers::Get_flagC() {return ((this->af.simple[0] & 0x10) >> 4);}
void Registers::Set_flagC(BYTE valor) {this->af.simple[0] = (this->af.simple[0] & 0xEF) | (valor << 4);}

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

void Registers::Set_Flag(e_registers flag, BYTE valor)
{
	switch (flag){
		case f_C: this->Set_flagC(valor);
		case f_H: this->Set_flagH(valor);
		case f_N: this->Set_flagN(valor);
		case f_Z: this->Set_flagZ(valor);
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
