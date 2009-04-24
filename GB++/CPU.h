#ifndef __CPU_H__
#define __CPU_H__

#include "Registers.h"
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
	void Interprete();
	BYTE CiclosInstruccion(WORD OpCode);
	void OpCodeCB();
	void TareasRutinarias();
	void UpdateStateLCD();
	void UpdateTimer();
	void Interrupciones();
	void eventsSDL();
};

#endif