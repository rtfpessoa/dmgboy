#ifndef __CPU_H__
#define __CPU_H__

#include "Registers.h"
#include "Instructions.h"
#include "Memory.h"
#include "Video.h"
#include "Pad.h"
#include "Cartridge.h"

class CPU: public Registers, public Memory
{
private:
	BYTE lastCycles;
	WORD cyclesLCD;
	WORD cyclesTimer;
	WORD cyclesDIV;
	Video *v;
	Pad *p;
public:
	CPU(Video *v, Pad *p, Cartridge *c);
	~CPU();
	
	void Run();
	void Reset();
private:
	void Interpreter();
	BYTE CiclosInstruccion(WORD OpCode);
	void OpCodeCB(Instructions * inst);
	void TareasRutinarias();
	void UpdateStateLCD();
	void UpdateTimer();
	void Interrupciones(Instructions * inst);
	void eventsSDL();
};

#endif