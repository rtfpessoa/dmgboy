#ifndef __CPU_H__
#define __CPU_H__

#include "Registers.h"
#include "Instrucciones.h"
#include "Memory.h"
#include "Video.h"
#include "Pad.h"

class CPU: public Registers, public Memory, public Pad
{
private:
	BYTE lastCycles;
	WORD cyclesLCD;
	WORD cyclesTimer;
	WORD cyclesDIV;
	Video *v;
	Pad *p;
public:
	CPU(Video *v, Pad *p);
	~CPU();
	
	void Run();
	void Reset();
private:
	void Interpreter();
	BYTE CiclosInstruccion(WORD OpCode);
	void OpCodeCB(Instrucciones * inst);
	void TareasRutinarias();
	void UpdateStateLCD();
	void UpdateTimer();
	void Interrupciones(Instrucciones * inst);
	void eventsSDL();
};

#endif