#include "StdAfx.h"
#include "Instrucciones.h"

Instrucciones::Instrucciones(Registers* reg, Memory* mem)
{
	this->reg = reg;
	this->mem = mem;
}

Instrucciones::~Instrucciones(void)
{
}

void Instrucciones::NOP(){reg->Add_PC(1);}

void Instrucciones::LD_r1_r2(e_registers e_reg1, e_registers e_reg2)
{
	BYTE longitud = 1;
	
	if (e_reg1 == c_HL)
	{
		if (e_reg2 == n)
		{
			mem->MemW(reg->Get_HL(), mem->MemR(reg->Get_PC() + 1));
			longitud = 2;
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
	
	reg->Add_PC(longitud);
}


void Instrucciones::LD_A_n(e_registers lugar)
{
    WORD address;
	BYTE valor;
    BYTE longitud = 1;
	
	switch(lugar)
	{
		case $:
			valor = mem->MemR(reg->Get_PC()+1);
			longitud = 2;
			break;
		case c_nn:
			address = (mem->MemR((reg->Get_PC() + 2)) << 8) | mem->MemR(reg->Get_PC() + 1);
			valor = mem->MemR(address);
			longitud = 3;
			break;
		case c_BC:
			valor = mem->MemR(reg->Get_BC());
			break;
		case c_DE:
			valor = mem->MemR(reg->Get_DE());
			break;
		case c_HL:
			valor = mem->MemR(reg->Get_HL());
			break;
		default:
			valor = reg->Get_Reg(lugar);
	}

	reg->Set_A(valor);

    reg->Add_PC(longitud);
}


void Instrucciones::LD_n_A(e_registers lugar)
{
    WORD direccion;
    BYTE longitud = 1;

	switch (lugar)
	{
		case c_nn:
			direccion = ((mem->MemR(reg->Get_PC() + 2)) << 8) | mem->MemR(reg->Get_PC() + 1);
			mem->MemW(direccion, reg->Get_A());
			longitud = 3;
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
			reg->Set_Reg(lugar, reg->Get_A());
	}

	reg->Add_PC(longitud);
}

void Instrucciones::JP_nn()
{
	reg->Set_PC((mem->MemR((reg->Get_PC()+2)) << 8) | mem->MemR(reg->Get_PC() + 1));
}

void Instrucciones::LDH_A_n()
{
	reg->Set_A(mem->MemR(0xFF00 + mem->MemR(reg->Get_PC() + 1)));
	reg->Add_PC(2);
}

void Instrucciones::LDH_n_A()
{
	mem->MemW(0xFF00 + mem->MemR(reg->Get_PC() + 1), reg->Get_A());
	reg->Add_PC(2);
}

void Instrucciones::CCF()
{
	reg->Set_flagN(0);
	reg->Set_flagH(0);
	reg->Set_flagC(!reg->Get_flagC());
	reg->Add_PC(1);
}

void Instrucciones::CP_n(e_registers lugar)
{
	BYTE valor;
	BYTE longitud = 1;

	switch (lugar)
	{
		case $:
			valor = mem->MemR(reg->Get_PC() + 1);
			longitud = 2;
			break;
		case c_HL:
			valor = mem->MemR(reg->Get_HL());
			break;
		default:
			valor = reg->Get_Reg(lugar);
	}

	if (reg->Get_A() == valor) reg->Set_flagZ(1); else reg->Set_flagZ(0);
	reg->Set_flagN(1);
	if ((reg->Get_A() & 0x0F) < (valor & 0x0F)) reg->Set_flagH(1); else reg->Set_flagH(0);
    if (reg->Get_A() < valor) reg->Set_flagC(1); else reg->Set_flagC(0);

	reg->Add_PC(longitud);
}

void Instrucciones::CPL()
{
	reg->Set_A(~reg->Get_A());

	reg->Set_flagN(1);
	reg->Set_flagH(1);

	reg->Add_PC(1);
}

void Instrucciones::LD_n_nn(e_registers lugar)
{
	reg->Set_Reg(lugar, ((mem->MemR(reg->Get_PC() + 2)) << 8) | mem->MemR(reg->Get_PC() + 1));
    reg->Add_PC(3);
}

void Instrucciones::LD_nn_SP()
{
	WORD destAddress = ((mem->MemR(reg->Get_PC() + 2)) << 8) | mem->MemR(reg->Get_PC() + 1);
	mem->MemW(destAddress, reg->Get_SP() & 0x00FF);
	mem->MemW(destAddress + 1, reg->Get_SP() >> 8);
    reg->Add_PC(3);
}

void Instrucciones::JR()
{
    char n;

	n = mem->MemR(reg->Get_PC() + 1);

	//El "2 +" es porque antes de saltar ha tenido que ver cuales eran los dos opcodes de la instrucción
	//y tiene importancia al ser un salto relativo con respecto al actual PC.
    reg->Add_PC(2 + n);
}

void Instrucciones::JR_CC_n(e_registers flag, BYTE value2check)
{
	char n;
	
	n = mem->MemR(reg->Get_PC() + 1);

	reg->Add_PC(2);

	if (reg->Get_Flag(flag) == value2check)
		reg->Add_PC(n);
}

void Instrucciones::CALL_nn()
{
	reg->Add_SP(-1);
	mem->MemW(reg->Get_SP(),((reg->Get_PC() + 3) & 0xFF00) >> 8);
	reg->Add_SP(-1);
	mem->MemW(reg->Get_SP(),(reg->Get_PC() + 3) & 0x00FF);
	reg->Set_PC(((mem->MemR(reg->Get_PC() + 2)) << 8) | mem->MemR(reg->Get_PC() + 1));
}
 
void Instrucciones::CALL_cc_nn(e_registers flag, BYTE value2check)
{
	if (reg->Get_Flag(flag) == value2check)
		CALL_nn();
	else
		reg->Add_PC(3);
}

void Instrucciones::LDI_A_HL()
{
	reg->Set_A(mem->MemR(reg->Get_HL()));
	reg->Set_HL(reg->Get_HL() + 1);
	reg->Add_PC(1);
}

void Instrucciones::LDI_HL_A()
{
	mem->MemW(reg->Get_HL(), reg->Get_A());
	reg->Set_HL(reg->Get_HL() + 1);
	reg->Add_PC(1);
}

void Instrucciones::LDD_A_HL()
{
	reg->Set_A(mem->MemR(reg->Get_HL()));
	reg->Set_HL(reg->Get_HL() - 1);
	reg->Add_PC(1);
}

void Instrucciones::LDD_HL_A()
{
	mem->MemW(reg->Get_HL(), reg->Get_A());
	reg->Set_HL(reg->Get_HL() - 1);
	reg->Add_PC(1);
}

void Instrucciones::LD_SP_HL()
{
	reg->Set_SP(reg->Get_HL());

	reg->Add_PC(1);
}

void Instrucciones::INC_n(e_registers lugar)
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

void Instrucciones::INC_nn(e_registers lugar)
{
	reg->Set_Reg(lugar, reg->Get_Reg(lugar) + 1);
	reg->Add_PC(1);
}

void Instrucciones::DAA()
{
	(reg->Get_A() > 99) ? reg->Set_flagC(1) : reg->Set_flagC(0);
	reg->Set_flagH(0);
	if (!reg->Get_A()) reg->Set_flagZ(1);

	reg->Set_A(((reg->Get_A() / 10) << 4) | (reg->Get_A() % 10));

	reg->Add_PC(1);
}

void Instrucciones::DEC_n(e_registers lugar)
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

void Instrucciones::DEC_nn(e_registers lugar)
{
	reg->Set_Reg(lugar, reg->Get_Reg(lugar) - 1);
	reg->Add_PC(1);
}

void Instrucciones::OR_n(e_registers lugar)
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

	if (reg->Get_A() == 0x00) reg->Set_flagZ(1); else reg->Set_flagZ(0);
	reg->Set_flagN(0);
	reg->Set_flagH(0);
	reg->Set_flagC(0);

	reg->Add_PC(longitud);
}

void Instrucciones::XOR_n(e_registers lugar)
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

void Instrucciones::RET()
{
	reg->Set_PC((mem->MemR(reg->Get_SP() + 1) << 8) | mem->MemR(reg->Get_SP()));
	reg->Add_SP(2);
}

void Instrucciones::RETI()
{
	EI();
	RET();
}

void Instrucciones::RET_cc(e_registers flag, BYTE value2check)
{
	reg->Add_PC(1);
	if (reg->Get_Flag(flag) == value2check)
		RET();
}
/*
void Instrucciones::PUSH_nn(e_registers lugar)
{

}
*/
void Instrucciones::LD_nn_n(e_registers lugar)
{
	reg->Set_Reg(lugar, mem->MemR(reg->Get_PC() + 1));
	reg->Add_PC(2);
}

void Instrucciones::LD_A_C()
{
	reg->Set_A(mem->MemR(0xFF00 + reg->Get_C()));
	reg->Add_PC(1);
}

void Instrucciones::LD_C_A()
{
	mem->MemW(0xFF00 + reg->Get_C(), reg->Get_A());
	reg->Add_PC(1);
}

void Instrucciones::SET_b_r(BYTE bit, e_registers lugar)
{
	if (lugar == c_HL)
		mem->MemW(reg->Get_HL(), mem->MemR(reg->Get_HL()) | 1 << bit);
	else
		reg->Set_Reg(lugar, reg->Get_Reg(lugar) | 1 << bit);

	reg->Add_PC(2);
}

void Instrucciones::BIT_b_r(BYTE bit, e_registers lugar)
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

void Instrucciones::RES_b_r(BYTE bit, e_registers lugar)
{
    if (lugar == c_HL)
		mem->MemW(reg->Get_HL(), mem->MemR(reg->Get_HL()) & ~(1 << bit));
	else
		reg->Set_Reg(lugar, reg->Get_Reg(lugar) & ~(1 << bit));

	reg->Add_PC(2);
}

void Instrucciones::DI()
{
	reg->Set_IME(0);
	reg->Add_PC(1);
}

void Instrucciones::EI()
{
	reg->Set_IME(1);
	reg->Add_PC(1);
}

void Instrucciones::SBC_A(e_registers lugar)
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

void Instrucciones::AND(e_registers lugar)
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
			reg->Set_A(reg->Get_A() | reg->Get_Reg(lugar));
	}

	if (reg->Get_A() == 0x00) reg->Set_flagZ(1); else reg->Set_flagZ(0);
	reg->Set_flagN(0);
	reg->Set_flagH(1);
	reg->Set_flagC(0);

	reg->Add_PC(longitud);
}


void Instrucciones::SLA_n(e_registers lugar)
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

void Instrucciones::SRA_n(e_registers lugar)
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

void Instrucciones::SRL_n(e_registers lugar)
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

void Instrucciones::SUB_n(e_registers lugar)
{
	WORD value;
	BYTE length = 1;

	if (lugar == c_HL)
		value = mem->MemR(reg->Get_HL());
	else if (lugar == $)
	{
		value = mem->MemR(reg->Get_PC() + 1);
		length = 2;
	}
	else
		value = reg->Get_Reg(lugar);

	((reg->Get_A() - value) == 0) ? reg->Set_flagZ(1) : reg->Set_flagZ(0);
	reg->Set_flagN(1);
	(((reg->Get_A() & 0x00FF) - (value & 0x00FF)) < 0) ? reg->Set_flagH(1) : reg->Set_flagH(0);
	((reg->Get_A() - value) < 0) ? reg->Set_flagC(1) : reg->Set_flagC(0);

	reg->Set_A(reg->Get_A() - value);

	reg->Add_PC(length);
}

void Instrucciones::ADD_A_n(e_registers lugar)
{
	WORD valor;
	BYTE valor_reg, longitud = 1;
	
	switch (lugar)
	{
		case $:		valor_reg = mem->MemR(reg->Get_PC() + 1); longitud = 2; break;
		case c_HL:	valor_reg = reg->Get_HL(); break;
		default:	valor_reg = reg->Get_Reg(lugar); break;
	}		

	valor = reg->Get_A() + valor_reg;

	if ((valor & 0xFF)== 0) reg->Set_flagZ(1); else reg->Set_flagZ(0);
	if (((reg->Get_A() & 0x0F) + (valor_reg & 0x0F)) > 0x0F) reg->Set_flagN(1); else reg->Set_flagN(0);
	reg->Set_flagH(0);
	if (valor > 0xFF) reg->Set_flagC(1); else reg->Set_flagC(0);

	reg->Set_A(valor & 0xFF);

	reg->Add_PC(longitud);
}

void Instrucciones::ADC_A_n(e_registers lugar)
{
	WORD value;
	BYTE valor_reg, longitud = 1;
	
	switch (lugar)
	{
		case $:		valor_reg = mem->MemR(reg->Get_PC() + 1); longitud = 2; break;
		case c_HL:	valor_reg = reg->Get_HL(); break;
		default:	valor_reg = reg->Get_Reg(lugar); break;
	}		

	value = reg->Get_flagC() + valor_reg + reg->Get_A();

	if ((value & 0xFF)== 0) reg->Set_flagZ(1); else reg->Set_flagZ(0);
	reg->Set_flagN(0);
	if ((reg->Get_flagC() + (valor_reg & 0x0F) + (reg->Get_A() & 0x0F)) > 0x0F)
		reg->Set_flagH(1);
	else
		reg->Set_flagH(0);
	if (value > 0xFF) reg->Set_flagC(1); else reg->Set_flagC(0);

	reg->Set_A(value & 0xFF);

	reg->Add_PC(longitud);
}

void Instrucciones::ADD_HL_n(e_registers lugar)
{
	int valor;
	WORD valor_reg;
	
	switch (lugar)
	{
		case BC:
		case DE:
		case HL:
		case SP: valor_reg = reg->Get_Reg(lugar); break;
		default: throw exception("Error, registro incorrecto. Instrucción: ADD_HL");
	}		

	valor = reg->Get_HL() + valor_reg;
	
	reg->Set_flagN(0);
	if (((reg->Get_HL() & 0x0FFF) + (valor_reg & 0x0FFF)) > 0x0FFF) reg->Set_flagH(1); else reg->Set_flagH(0);
	if (valor > 0xFFFF) reg->Set_flagC(1); else reg->Set_flagC(0);

	reg->Set_HL(valor & 0xFFFF);

	reg->Add_PC(1);
}

void Instrucciones::ADD_SP_n()
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

void Instrucciones::JP_HL()
{
	reg->Set_PC(reg->Get_HL());
}

void Instrucciones::SCF()
{
	reg->Set_flagN(0);
	reg->Set_flagH(0);
	reg->Set_flagC(1);

	reg->Add_PC(1);
}

void Instrucciones::HALT()
{
	if (reg->Get_IME())
	{
		reg->Set_Halt(true);
		reg->Add_PC(1);
	}
	else	//Si no están habilitadas las interrupciones se evita el siguiente opcode
		reg->Add_PC(2);
}

void Instrucciones::STOP()
{
	reg->Set_Stop(true);

	reg->Add_PC(2);

	cout << "Instruccion 0x10 (Stop)\n";
}

void Instrucciones::SWAP(e_registers lugar)
{
	BYTE valor;

	if (lugar == c_HL)
	{
		valor = mem->MemR(reg->Get_HL());
		valor = ((valor & 0x0F) << 4) | ((valor & 0xF0) >> 4);
		mem->MemW(reg->Get_HL(), valor);
	}
	else
	{
		valor = reg->Get_Reg(lugar);
		valor = ((valor & 0x0F) << 4) | ((valor & 0xF0) >> 4);
		reg->Set_Reg(lugar, valor);
	}

	if (lugar == 0) reg->Set_flagZ(1); else reg->Set_flagZ(0);
	reg->Set_flagN(0);
	reg->Set_flagH(0);
	reg->Set_flagC(0);

	reg->Add_PC(2);
}

void Instrucciones::PUSH_nn(e_registers lugar)
{
	reg->Add_SP(-1);
	mem->MemW(reg->Get_SP(), (reg->Get_Reg(lugar) & 0xFF00) >> 8);
	reg->Add_SP(-1);
	mem->MemW(reg->Get_SP(), reg->Get_Reg(lugar) & 0x00FF);

	reg->Add_PC(1);
}

void Instrucciones::POP_nn(e_registers lugar)
{	
	reg->Set_Reg(lugar, (mem->MemR(reg->Get_SP() + 1) << 8) | mem->MemR(reg->Get_SP()));
	reg->Add_SP(2);

	reg->Add_PC(1);
}

void Instrucciones::JP_cc_nn(e_registers flag, BYTE value2check)
{
	WORD nn;
	
	nn = (mem->MemR((reg->Get_PC()+2)) << 8) | mem->MemR(reg->Get_PC() + 1);

	reg->Add_PC(3);

	if (reg->Get_Flag(flag) == value2check)
		reg->Set_PC(nn);
}

void Instrucciones::RL_n(e_registers lugar)
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

void Instrucciones::RLCA()
{
	BYTE bit7, valor;
	
	valor = reg->Get_A();
	bit7 = BIT7(valor) >> 7;
	valor = (valor << 1) | bit7;
	reg->Set_A(valor);

	reg->Set_flagZ(0);
	reg->Set_flagN(0);
	reg->Set_flagH(0);
	reg->Set_flagC(bit7);

	reg->Add_PC(1);
}

void Instrucciones::RLC_n(e_registers lugar)
{
	BYTE bit7, valor;
	
	if (lugar == c_HL)
	{
		valor = mem->MemR(reg->Get_HL());
		bit7 = BIT7(valor) >> 7;
		valor = (valor << 1) | bit7;
		mem->MemW(reg->Get_HL(), valor);
	}
	else
	{
		valor = reg->Get_Reg(lugar); 
		bit7 = BIT7(valor) >> 7;
		valor = (valor << 1) | bit7;
		reg->Set_Reg(lugar, valor);
	}

	if (valor == 0) reg->Set_flagZ(1); else reg->Set_flagZ(0);
	reg->Set_flagN(0);
	reg->Set_flagH(0);
	reg->Set_flagC(bit7);

	reg->Add_PC(2);
}

//void Instrucciones::RLCA()
//{
//	RLC_n(A);
//}

void Instrucciones::RR_n(e_registers lugar)
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

void Instrucciones::RRC_n(e_registers lugar)
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

//void Instrucciones::RRCA()
//{
//	RRC_n(A);
//}

void Instrucciones::PUSH_PC()
{
	reg->Add_SP(-1);
	mem->MemW(reg->Get_SP(), (reg->Get_PC() & 0xFF00) >> 8);
	reg->Add_SP(-1);
	mem->MemW(reg->Get_SP(), reg->Get_PC() & 0x00FF);
}

void Instrucciones::RST_n(BYTE desp)
{
	//Queremos que se guarde la siquiente instruccion a ejecutar
	reg->Add_PC(1);

	PUSH_PC();
	
	reg->Set_PC(0x0000 + desp);
}

void Instrucciones::LDHL_SP_n()
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
