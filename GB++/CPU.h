#ifndef __CPU_H__
#define __CPU_H__

#include "Registros.h"
#include "Memoria.h"
#include "Video.h"

class CPU: public Registros, public Memoria
{
private:
	BYTE lastCycles;
	WORD cyclesLCD;
	WORD cyclesTimer;
	WORD cyclesDIV;
	Video *v;
	BYTE keys[8];
public:
	CPU(Video *v);
	~CPU();
	
	void Run();
	void Reset();
private:
	void Interprete();
	BYTE CiclosInstruccion(WORD OpCode);
	void OpCodeCB();
	void TareasRutinarias();
	void ActualizarEstadoLCD();
	void ActualizarTimer();
	void Interrupciones();
	void eventsSDL();
};

#endif