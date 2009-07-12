#include "Instructions.h"
#include <iostream>
#include <assert.h>
#include "GBException.h"

using namespace std;

Instructions::Instructions(Registers* reg, Memory* mem)
{
	this->reg = reg;
	this->mem = mem;
}

Instructions::~Instructions(void)
{
}

void Instructions::NOP(){reg->Add_PC(1);}

void Instructions::LD_r1_r2(e_registers e_reg1, e_registers e_reg2)
{
	BYTE length = 1;

	if (e_reg1 == c_HL)
	{
		if (e_reg2 == n)
		{
			mem->MemW(reg->Get_HL(), mem->MemR(reg->Get_PC() + 1));
			length = 2;
		}
		else
			mem->MemW(reg->Get_HL(), reg->Get_Reg(e_reg2));
	}
	else
	{
		if (e_reg2 == c_HL)
		{
			reg->Set_Reg(e_reg1, mem->MemR(reg->Get_HL()));
		}
		else
		{
			reg->Set_Reg(e_reg1, reg->Get_Reg(e_reg2));
		}
	}

	reg->Add_PC(length);
}


void Instructions::LD_A_n(e_registers place)
{
    WORD address;
	BYTE value;
    BYTE length = 1;

	switch(place)
	{
		case $:
			value = mem->MemR(reg->Get_PC()+1);
			length = 2;
			break;
		case c_nn:
			address = (mem->MemR((reg->Get_PC() + 2)) << 8) | mem->MemR(reg->Get_PC() + 1);
			value = mem->MemR(address);
			length = 3;
			break;
		case c_BC:
			value = mem->MemR(reg->Get_BC());
			break;
		case c_DE:
			value = mem->MemR(reg->Get_DE());
			break;
		case c_HL:
			value = mem->MemR(reg->Get_HL());
			break;
		default:
			value = reg->Get_Reg(place);
	}

	reg->Set_A(value);

    reg->Add_PC(length);
}


void Instructions::LD_n_A(e_registers place)
{
    WORD address;
    BYTE length = 1;

	switch (place)
	{
		case c_nn:
			address = ((mem->MemR(reg->Get_PC() + 2)) << 8) | mem->MemR(reg->Get_PC() + 1);
			mem->MemW(address, reg->Get_A());
			length = 3;
			break;
		case c_BC:
			mem->MemW(reg->Get_BC(), reg->Get_A());
			break;
		case c_DE:
			mem->MemW(reg->Get_DE(), reg->Get_A());
			break;
		case c_HL:
			mem->MemW(reg->Get_HL(), reg->Get_A());
			break;
		default:
			reg->Set_Reg(place, reg->Get_A());
	}

	reg->Add_PC(length);
}

void Instructions::JP_nn()
{
	reg->Set_PC((mem->MemR((reg->Get_PC()+2)) << 8) | mem->MemR(reg->Get_PC() + 1));
}

void Instructions::LDH_A_n()
{
	reg->Set_A(mem->MemR(0xFF00 + mem->MemR(reg->Get_PC() + 1)));
	reg->Add_PC(2);
}

void Instructions::LDH_n_A()
{
	mem->MemW(0xFF00 + mem->MemR(reg->Get_PC() + 1), reg->Get_A());
	reg->Add_PC(2);
}

void Instructions::CCF()
{
	reg->Set_flagN(0);
	reg->Set_flagH(0);
	reg->Set_flagC(!reg->Get_flagC());
	reg->Add_PC(1);
}

void Instructions::CP_n(e_registers place)
{
	BYTE value;
	BYTE length = 1;

	switch (place)
	{
		case $:
			value = mem->MemR(reg->Get_PC() + 1);
			length = 2;
			break;
		case c_HL:
			value = mem->MemR(reg->Get_HL());
			break;
		default:
			value = reg->Get_Reg(place);
	}

	reg->Set_flagZ((reg->Get_A() == value) ? 1 : 0);
	reg->Set_flagN(1);
	reg->Set_flagH(((reg->Get_A() & 0x0F) < (value & 0x0F)) ? 1 : 0);
	reg->Set_flagC((reg->Get_A() < value) ? 1 : 0);

	reg->Add_PC(length);
}

void Instructions::CPL()
{
	reg->Set_A(~reg->Get_A());

	reg->Set_flagN(1);
	reg->Set_flagH(1);

	reg->Add_PC(1);
}

void Instructions::LD_n_nn(e_registers place)
{
	assert((place == BC) || (place == DE) || (place == HL) || (place == SP));
	reg->Set_Reg(place, ((mem->MemR(reg->Get_PC() + 2)) << 8) | mem->MemR(reg->Get_PC() + 1));
    reg->Add_PC(3);
}

void Instructions::LD_nn_SP()
{
	WORD destAddress = ((mem->MemR(reg->Get_PC() + 2)) << 8) | mem->MemR(reg->Get_PC() + 1);
	mem->MemW(destAddress, reg->Get_SP() & 0x00FF);
	mem->MemW(destAddress + 1, reg->Get_SP() >> 8);
    reg->Add_PC(3);
}

void Instructions::JR()
{
    char address;	//Con signo

	address = mem->MemR(reg->Get_PC() + 1);

	//El "2 +" es porque antes de saltar ha tenido que ver cuales eran los dos opcodes de la instrucción
	//y tiene importancia al ser un salto relativo con respecto al actual PC.
    reg->Add_PC(2 + address);
}

void Instructions::JR_CC_n(e_registers flag, BYTE value2check)
{
	if (reg->Get_Flag(flag) == value2check)
		JR();
	else
		reg->Add_PC(2);
}

void Instructions::CALL_nn()
{
	reg->Add_SP(-1);
	mem->MemW(reg->Get_SP(),((reg->Get_PC() + 3) & 0xFF00) >> 8);
	reg->Add_SP(-1);
	mem->MemW(reg->Get_SP(),(reg->Get_PC() + 3) & 0x00FF);
	reg->Set_PC(((mem->MemR(reg->Get_PC() + 2)) << 8) | mem->MemR(reg->Get_PC() + 1));
}

void Instructions::CALL_cc_nn(e_registers flag, BYTE value2check)
{
	if (reg->Get_Flag(flag) == value2check)
		CALL_nn();
	else
		reg->Add_PC(3);
}

void Instructions::LDI_A_HL()
{
	reg->Set_A(mem->MemR(reg->Get_HL()));
	reg->Set_HL(reg->Get_HL() + 1);
	reg->Add_PC(1);
}

void Instructions::LDI_HL_A()
{
	mem->MemW(reg->Get_HL(), reg->Get_A());
	reg->Set_HL(reg->Get_HL() + 1);
	reg->Add_PC(1);
}

void Instructions::LDD_A_HL()
{
	reg->Set_A(mem->MemR(reg->Get_HL()));
	reg->Set_HL(reg->Get_HL() - 1);
	reg->Add_PC(1);
}

void Instructions::LDD_HL_A()
{
	mem->MemW(reg->Get_HL(), reg->Get_A());
	reg->Set_HL(reg->Get_HL() - 1);
	reg->Add_PC(1);
}

void Instructions::LD_SP_HL()
{
	reg->Set_SP(reg->Get_HL());

	reg->Add_PC(1);
}

void Instructions::SUB_n(e_registers place)
{
	int value, length = 1;

	if (place == c_HL)
		value = mem->MemR(reg->Get_HL());
	else if (place == $)
	{
		value = mem->MemR(reg->Get_PC() + 1);
		length = 2;
	}
	else
		value = reg->Get_Reg(place);

	reg->Set_flagZ((reg->Get_A() - value) ? 0 : 1);
	reg->Set_flagN(1);
	((((int)reg->Get_A() & 0x0F) - (value & 0x0F)) < 0) ? reg->Set_flagH(1) : reg->Set_flagH(0);
	((reg->Get_A() - value) < 0) ? reg->Set_flagC(1) : reg->Set_flagC(0);

	reg->Set_A(reg->Get_A() - value);

	reg->Add_PC(length);
}

void Instructions::ADD_A_n(e_registers place)
{
	int value, length = 1;
	BYTE valueReg;

	switch (place)
	{
		case $:		valueReg = mem->MemR(reg->Get_PC() + 1); length = 2; break;
		case c_HL:	valueReg = mem->MemR(reg->Get_HL()); break;
		default:	valueReg = reg->Get_Reg(place); break;
	}

	value = reg->Get_A() + valueReg;

	reg->Set_flagZ(!(value & 0xFF) ? 1 : 0);
	if (((reg->Get_A() & 0x0F) + (valueReg & 0x0F)) > 0x0F) reg->Set_flagH(1); else reg->Set_flagH(0);
	reg->Set_flagN(0);
	reg->Set_flagC((value > 0xFF) ? 1 : 0);

	reg->Set_A(value & 0xFF);

	reg->Add_PC(length);
}

void Instructions::ADC_A_n(e_registers lugar)
{
	int value, length = 1;
	BYTE valueReg;

	switch (lugar)
	{
		case $:		valueReg = mem->MemR(reg->Get_PC() + 1); length = 2; break;
		case c_HL:	valueReg = mem->MemR(reg->Get_HL()); break;
		default:	valueReg = reg->Get_Reg(lugar); break;
	}

	value = reg->Get_flagC() + valueReg + reg->Get_A();

	reg->Set_flagZ(!(value & 0xFF) ? 1 : 0);
	reg->Set_flagN(0);
	if ((reg->Get_flagC() + (valueReg & 0x0F) + (reg->Get_A() & 0x0F)) > 0x0F)
		reg->Set_flagH(1);
	else
		reg->Set_flagH(0);
	reg->Set_flagC((value > 0xFF) ? 1 : 0);

	reg->Set_A(value & 0xFF);

	reg->Add_PC(length);
}

void Instructions::RLCA()
{
	BYTE bit7, value;

	value = reg->Get_A();
	bit7 = BIT7(value) >> 7;
	value = (value << 1) | bit7;
	reg->Set_A(value);

	//¿¿¿¿¿¿¿¿reg->Set_flagZ(0);??????? Este o el que hay?
	reg->Set_flagZ(value ? 0 : 1);
	reg->Set_flagN(0);
	reg->Set_flagH(0);
	reg->Set_flagC(bit7);

	reg->Add_PC(1);
}

void Instructions::RLC_n(e_registers place)
{
	BYTE bit7, value;

	if (place == c_HL)
	{
		value = mem->MemR(reg->Get_HL());
		bit7 = BIT7(value) >> 7;
		value = (value << 1) | bit7;
		mem->MemW(reg->Get_HL(), value);
	}
	else
	{
		value = reg->Get_Reg(place);
		bit7 = BIT7(value) >> 7;
		value = (value << 1) | bit7;
		reg->Set_Reg(place, value);
	}

	reg->Set_flagZ(value ? 0 : 1);
	reg->Set_flagN(0);
	reg->Set_flagH(0);
	reg->Set_flagC(bit7);

	reg->Add_PC(2);
}

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//Revisado hasta aquí
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

void Instructions::INC_n(e_registers lugar)
{
	if (lugar == c_HL)
	{
		mem->MemW(reg->Get_HL(), mem->MemR(reg->Get_HL()) + 1);
		if (mem->MemR(reg->Get_HL()) == 0x00) reg->Set_flagZ(1); else reg->Set_flagZ(0);
		if ((mem->MemR(reg->Get_HL()) & 0x0F) == 0x00) reg->Set_flagH(1); else reg->Set_flagH(0);
	}
	else
	{
		reg->Set_Reg(lugar, reg->Get_Reg(lugar) + 1);
		if (reg->Get_Reg(lugar) == 0x00) reg->Set_flagZ(1); else reg->Set_flagZ(0);
		if ((reg->Get_Reg(lugar) & 0x0F) == 0x00) reg->Set_flagH(1); else reg->Set_flagH(0);
	}
	reg->Set_flagN(0);

	reg->Add_PC(1);
}

void Instructions::INC_nn(e_registers lugar)
{
	reg->Set_Reg(lugar, reg->Get_Reg(lugar) + 1);
	reg->Add_PC(1);
}

void Instructions::DAA()
{
	(reg->Get_A() > 99) ? reg->Set_flagC(1) : reg->Set_flagC(0);
	reg->Set_flagH(0);
	if (!reg->Get_A()) reg->Set_flagZ(1);

	reg->Set_A(((reg->Get_A() / 10) << 4) | (reg->Get_A() % 10));

	reg->Add_PC(1);
}

void Instructions::DEC_n(e_registers lugar)
{
	if (lugar == c_HL)
	{
		mem->MemW(reg->Get_HL(), mem->MemR(reg->Get_HL()) - 1);
		if (mem->MemR(reg->Get_HL()) == 0x00) reg->Set_flagZ(1); else reg->Set_flagZ(0);
		if ((mem->MemR(reg->Get_HL()) & 0x0F) == 0x0F) reg->Set_flagH(1); else reg->Set_flagH(0);
	}
	else
	{
		reg->Set_Reg(lugar, reg->Get_Reg(lugar) - 1);
		if (reg->Get_Reg(lugar) == 0x00) reg->Set_flagZ(1); else reg->Set_flagZ(0);
		if ((reg->Get_Reg(lugar) & 0x0F) == 0x0F) reg->Set_flagH(1); else reg->Set_flagH(0);
	}
	reg->Set_flagN(1);

	reg->Add_PC(1);
}

void Instructions::DEC_nn(e_registers lugar)
{
	reg->Set_Reg(lugar, reg->Get_Reg(lugar) - 1);
	reg->Add_PC(1);
}

void Instructions::OR_n(e_registers lugar)
{
	BYTE longitud = 1;

	switch (lugar)
	{
		case $:
			reg->Set_A(reg->Get_A() | mem->MemR(reg->Get_PC() + 1));
			longitud = 2;
			break;
		case c_HL:
			reg->Set_A(reg->Get_A() | mem->MemR(reg->Get_HL()));
			break;
		default:
			reg->Set_A(reg->Get_A() | reg->Get_Reg(lugar));
	}

	//if (reg->Get_A() == 0x00) reg->Set_flagZ(1); else reg->Set_flagZ(0);
	reg->Set_flagZ(!reg->Get_A() ? 1 : 0);
	reg->Set_flagN(0);
	reg->Set_flagH(0);
	reg->Set_flagC(0);

	reg->Add_PC(longitud);
}

void Instructions::XOR_n(e_registers lugar)
{
    BYTE longitud = 1;

	switch (lugar)
	{
		case $:
			reg->Set_A(reg->Get_A() ^ mem->MemR(reg->Get_PC() + 1));
			longitud = 2;
			break;
		case c_HL:
			reg->Set_A(reg->Get_A() ^ mem->MemR(reg->Get_HL()));
			break;
		default:
			reg->Set_A(reg->Get_A() ^ reg->Get_Reg(lugar));
	}

	if (reg->Get_A() == 0x00) reg->Set_flagZ(1); else reg->Set_flagZ(0);
	reg->Set_flagN(0);
	reg->Set_flagH(0);
	reg->Set_flagC(0);

	reg->Add_PC(longitud);
}

void Instructions::RET()
{
	reg->Set_PC((mem->MemR(reg->Get_SP() + 1) << 8) | mem->MemR(reg->Get_SP()));
	reg->Add_SP(2);
}

void Instructions::RETI()
{
	EI();
	RET();
}

void Instructions::RET_cc(e_registers flag, BYTE value2check)
{
	reg->Add_PC(1);
	if (reg->Get_Flag(flag) == value2check)
		RET();
}
/*
void Instructions::PUSH_nn(e_registers lugar)
{

}
*/
void Instructions::LD_nn_n(e_registers lugar)
{
	reg->Set_Reg(lugar, mem->MemR(reg->Get_PC() + 1));
	reg->Add_PC(2);
}

void Instructions::LD_A_C()
{
	reg->Set_A(mem->MemR(0xFF00 + reg->Get_C()));
	reg->Add_PC(1);
}

void Instructions::LD_C_A()
{
	mem->MemW(0xFF00 + reg->Get_C(), reg->Get_A());
	reg->Add_PC(1);
}

void Instructions::SET_b_r(BYTE bit, e_registers lugar)
{
	if (lugar == c_HL)
		mem->MemW(reg->Get_HL(), mem->MemR(reg->Get_HL()) | 1 << bit);
	else
		reg->Set_Reg(lugar, reg->Get_Reg(lugar) | 1 << bit);

	reg->Add_PC(2);
}

void Instructions::BIT_b_r(BYTE bit, e_registers lugar)
{
	BYTE value;

	if (lugar == c_HL)
		value = mem->MemR(reg->Get_HL());
	else
		value = reg->Get_Reg(lugar);

	if (!(value & (1 << bit)))
		reg->Set_flagZ(1);
	else
		reg->Set_flagZ(0);

	reg->Set_flagN(0);
	reg->Set_flagH(1);

	reg->Add_PC(2);
}

void Instructions::RES_b_r(BYTE bit, e_registers lugar)
{
    if (lugar == c_HL)
		mem->MemW(reg->Get_HL(), mem->MemR(reg->Get_HL()) & ~(1 << bit));
	else
		reg->Set_Reg(lugar, reg->Get_Reg(lugar) & ~(1 << bit));

	reg->Add_PC(2);
}

void Instructions::DI()
{
	reg->Set_IME(0);
	reg->Add_PC(1);
}

void Instructions::EI()
{
	reg->Set_IME(1);
	reg->Add_PC(1);
}

void Instructions::SBC_A(e_registers lugar)
{
	WORD aux;
	BYTE length = 1;

	switch(lugar)
	{
		case c_HL:
			aux = mem->MemR(reg->Get_HL()) + reg->Get_flagC();
			break;
		case $:
			aux = mem->MemR(reg->Get_PC() + 1) + reg->Get_flagC();
			length = 2;
			break;
		default:
			aux = reg->Get_Reg(lugar) + reg->Get_flagC();
	}

	if (!(reg->Get_A() - aux)) reg->Set_flagZ(1);
	reg->Set_flagN(1);
	((reg->Get_A() & 0x0F) < (aux & 0x0F)) ? reg->Set_flagH(1) : reg->Set_flagH(0);
	(reg->Get_A() < aux) ? reg->Set_flagC(1) : reg->Set_flagC(0);

	reg->Set_A(reg->Get_A() - aux);

	reg->Add_PC(length);
}

void Instructions::AND(e_registers lugar)
{
	BYTE longitud = 1;

	switch (lugar)
	{
		case $:
			reg->Set_A(reg->Get_A() & mem->MemR(reg->Get_PC() + 1));
			longitud = 2;
			break;
		case c_HL:
			reg->Set_A(reg->Get_A() & mem->MemR(reg->Get_HL()));
			break;
		default:
			reg->Set_A(reg->Get_A() & reg->Get_Reg(lugar));
	}

	if (reg->Get_A() == 0x00) reg->Set_flagZ(1); else reg->Set_flagZ(0);
	reg->Set_flagN(0);
	reg->Set_flagH(1);
	reg->Set_flagC(0);

	reg->Add_PC(longitud);
}


void Instructions::SLA_n(e_registers lugar)
{
	BYTE bit7, valor;

	if (lugar == c_HL)
	{
		bit7 = BIT7(mem->MemR(reg->Get_HL())) >> 7;
		mem->MemW(reg->Get_HL(), mem->MemR(reg->Get_HL()) << 1);
		valor = mem->MemR(reg->Get_HL());
	}
	else
	{
		bit7 = BIT7(reg->Get_Reg(lugar)) >> 7;
		reg->Set_Reg(lugar, reg->Get_Reg(lugar) << 1);
		valor = reg->Get_Reg(lugar);
	}

	if (valor == 0) reg->Set_flagZ(1); else reg->Set_flagZ(0);
	reg->Set_flagN(0);
	reg->Set_flagH(0);
	reg->Set_flagC(bit7);

	reg->Add_PC(2);
}

void Instructions::SRA_n(e_registers lugar)
{
    BYTE bit0, bit7, value;

	if (lugar == c_HL)
	{
		bit0 = BIT0(mem->MemR(reg->Get_HL()));
		bit7 = BIT7(mem->MemR(reg->Get_HL()));
		mem->MemW(reg->Get_HL(), bit7 | (mem->MemR(reg->Get_HL()) >> 1));
		value = mem->MemR(reg->Get_HL());
	}
	else
	{
		bit0 = BIT0(reg->Get_Reg(lugar));
		bit7 = BIT7(reg->Get_Reg(lugar));
		reg->Set_Reg(lugar, bit7 | (reg->Get_Reg(lugar) >> 1));
		value = reg->Get_Reg(lugar);
	}

	//if (value == 0) reg->Set_flagZ(1); else reg->Set_flagZ(0);
	reg->Set_flagZ(!value ? 1 : 0);
	reg->Set_flagN(0);
	reg->Set_flagH(0);
	reg->Set_flagC(bit0);

	reg->Add_PC(2);
}

void Instructions::SRL_n(e_registers lugar)
{
    BYTE bit0, bit7, valor;

	if (lugar == c_HL)
	{
		bit0 = BIT0(mem->MemR(reg->Get_HL()));
		bit7 = BIT7(mem->MemR(reg->Get_HL())) << 7;
		mem->MemW(reg->Get_HL(), bit7 | (mem->MemR(reg->Get_HL()) >> 1));
		valor = mem->MemR(reg->Get_HL());
	}
	else
	{
		bit0 = BIT0(reg->Get_Reg(lugar));
		bit7 = BIT7(reg->Get_Reg(lugar)) << 7;
		reg->Set_Reg(lugar, bit7 | (reg->Get_Reg(lugar) >> 1));
		valor = reg->Get_Reg(lugar);
	}

	if (valor == 0) reg->Set_flagZ(1); else reg->Set_flagZ(0);
	reg->Set_flagN(0);
	reg->Set_flagH(0);
	reg->Set_flagC(bit0);

	reg->Add_PC(2);
}

void Instructions::ADD_HL_n(e_registers lugar)
{
	int valor;
	WORD valor_reg;

	switch (lugar)
	{
		case BC:
		case DE:
		case HL:
		case SP: valor_reg = reg->Get_Reg(lugar); break;
		default: throw GBException("Error, registro incorrecto. Instrucción: ADD_HL");
	}

	valor = reg->Get_HL() + valor_reg;

	reg->Set_flagN(0);
	if (((reg->Get_HL() & 0x0FFF) + (valor_reg & 0x0FFF)) > 0x0FFF) reg->Set_flagH(1); else reg->Set_flagH(0);
	if (valor > 0xFFFF) reg->Set_flagC(1); else reg->Set_flagC(0);

	reg->Set_HL(valor & 0xFFFF);

	reg->Add_PC(1);
}

void Instructions::ADD_SP_n()
{
	BYTE lNibbleSP;
	char lNibbleN;

	reg->Set_flagZ(0);
	reg->Set_flagN(0);

	lNibbleSP = reg->Get_SP() & 0x00FF;
	lNibbleN = mem->MemR(reg->Get_PC() + 1);

	(((lNibbleSP & 0x0F) + (lNibbleN & 0x0F)) > 0x0F) ? reg->Set_flagH(1) : reg->Set_flagH(0);

	reg->Set_flagC(0);
	if ((reg->Get_SP() + lNibbleN) > 0xFFFF)
		reg->Set_flagC(1);
	if ((reg->Get_SP() + lNibbleN) < 0x0000)
		reg->Set_flagC(1);

	reg->Add_SP(lNibbleN);

	reg->Add_PC(2);
}

void Instructions::JP_HL()
{
	reg->Set_PC(reg->Get_HL());
}

void Instructions::SCF()
{
	reg->Set_flagN(0);
	reg->Set_flagH(0);
	reg->Set_flagC(1);

	reg->Add_PC(1);
}

void Instructions::HALT()
{
	if (reg->Get_IME())
	{
		reg->Set_Halt(true);
		reg->Add_PC(1);
	}
	else	//Si no están habilitadas las interrupciones se evita el siguiente opcode
		reg->Add_PC(2);
}

void Instructions::STOP()
{
	reg->Set_Stop(true);

	reg->Add_PC(2);

	cout << "Instruccion 0x10 (Stop)\n";
}

void Instructions::SWAP(e_registers place)
{
	BYTE value;

	if (place == c_HL)
	{
		value = mem->MemR(reg->Get_HL());
		value = ((value & 0x0F) << 4) | ((value & 0xF0) >> 4);
		mem->MemW(reg->Get_HL(), value);
	}
	else
	{
		value = reg->Get_Reg(place);
		value = ((value & 0x0F) << 4) | ((value & 0xF0) >> 4);
		reg->Set_Reg(place, value);
	}

	reg->Set_flagZ(value ? 0 : 1);
	reg->Set_flagN(0);
	reg->Set_flagH(0);
	reg->Set_flagC(0);

	reg->Add_PC(2);
}

void Instructions::PUSH_nn(e_registers lugar)
{
	reg->Add_SP(-1);
	mem->MemW(reg->Get_SP(), (reg->Get_Reg(lugar) & 0xFF00) >> 8);
	reg->Add_SP(-1);
	mem->MemW(reg->Get_SP(), reg->Get_Reg(lugar) & 0x00FF);

	reg->Add_PC(1);
}

void Instructions::POP_nn(e_registers lugar)
{
	reg->Set_Reg(lugar, (mem->MemR(reg->Get_SP() + 1) << 8) | mem->MemR(reg->Get_SP()));
	reg->Add_SP(2);

	reg->Add_PC(1);
}

void Instructions::JP_cc_nn(e_registers flag, BYTE value2check)
{
	WORD nn;

	nn = (mem->MemR((reg->Get_PC()+2)) << 8) | mem->MemR(reg->Get_PC() + 1);

	reg->Add_PC(3);

	if (reg->Get_Flag(flag) == value2check)
		reg->Set_PC(nn);
}

void Instructions::RL_n(e_registers lugar)
{
	BYTE oldBit7;

	if (lugar == c_HL)
	{
		oldBit7 = BIT7(mem->MemR(reg->Get_HL())) >> 7;
		mem->MemW(reg->Get_HL(), (mem->MemR(reg->Get_HL()) << 1) | reg->Get_flagC());
		//if (!mem->MemR(reg->Get_HL())) reg->Set_flagZ(1);
		reg->Set_flagZ(reg->Get_HL() ? 0 : 1);
	}
	else
	{
		oldBit7 = BIT7(reg->Get_Reg(lugar)) >> 7;
		reg->Set_Reg(lugar, (reg->Get_Reg(lugar) << 1) | reg->Get_flagC());
		//if (!reg->Get_Reg(lugar)) reg->Set_flagZ(1);
		reg->Set_flagZ(reg->Get_Reg(lugar) ? 0 : 1);
	}


	reg->Set_flagN(0);
	reg->Set_flagH(0);
	reg->Set_flagC(oldBit7);

	if (mem->MemR(reg->Get_PC()) == 0xCB)
		reg->Add_PC(2);
	else
		reg->Add_PC(1);
}

void Instructions::RR_n(e_registers lugar)
{
	BYTE bit0, value;

	if (lugar == c_HL)
	{
		value = mem->MemR(reg->Get_HL());
		bit0 = BIT0(value);
		value = (reg->Get_flagC() << 7) | (value >> 1);
		mem->MemW(reg->Get_HL(), value);
	}
	else
	{
		value = reg->Get_Reg(lugar);
		bit0 = BIT0(value);
		value = (reg->Get_flagC() << 7) | (value >> 1);
		reg->Set_Reg(lugar, value);
	}

	if (!value) reg->Set_flagZ(1);
	reg->Set_flagN(0);
	reg->Set_flagH(0);
	reg->Set_flagC(bit0);

	if (mem->MemR(reg->Get_PC()) == 0xCB)
		reg->Add_PC(2);
	else
		reg->Add_PC(1);
}

void Instructions::RRC_n(e_registers lugar)
{
	BYTE bit0, valor;

	if (lugar == c_HL)
	{
		valor = mem->MemR(reg->Get_HL());
		bit0 = BIT0(valor);
		valor = (valor >> 1) | (bit0 << 7);
		mem->MemW(reg->Get_HL(), valor);
	}
	else
	{
		valor = reg->Get_Reg(lugar);
		bit0 = BIT0(valor);
		valor = (valor >> 1) | (bit0 << 7);
		reg->Set_Reg(lugar, valor);
	}

	if (valor == 0) reg->Set_flagZ(1); else reg->Set_flagZ(0);
	reg->Set_flagN(0);
	reg->Set_flagH(0);
	reg->Set_flagC(bit0);

	if (mem->MemR(reg->Get_PC()) == 0xCB)
		reg->Add_PC(2);
	else
		reg->Add_PC(1);
}

//void Instructions::RRCA()
//{
//	RRC_n(A);
//}

void Instructions::PUSH_PC()
{
	reg->Add_SP(-1);
	mem->MemW(reg->Get_SP(), (reg->Get_PC() & 0xFF00) >> 8);
	reg->Add_SP(-1);
	mem->MemW(reg->Get_SP(), reg->Get_PC() & 0x00FF);
}

void Instructions::RST_n(BYTE desp)
{
	//Queremos que se guarde la siquiente instruccion a ejecutar
	reg->Add_PC(1);

	PUSH_PC();

	reg->Set_PC(0x0000 + desp);
}

void Instructions::LDHL_SP_n()
{
	char n;

	n = mem->MemR(reg->Get_PC() + 1);

	reg->Set_flagZ(0);
	reg->Set_flagN(0);
	(((reg->Get_SP() & 0x00FF) + (n & 0x00FF)) > 0x00FF) ? reg->Set_flagH(1) : reg->Set_flagH(0);
	((reg->Get_SP() + n) > 0xFFFF) ? reg->Set_flagC(1) : reg->Set_flagC(0);

	reg->Set_HL(reg->Get_SP() + n);

	reg->Add_PC(2);
}
