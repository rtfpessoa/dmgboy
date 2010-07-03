/*
 This file is part of gbpablog.
 
 gbpablog is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 gbpablog is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with gbpablog.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __CPU_H__
#define __CPU_H__

#include "Registers.h"
#include "Instructions.h"
#include "Memory.h"
#include "Video.h"
#include "Pad.h"
#include "Cartridge.h"
#include "Log.h"

class CPU: public Registers, public Memory
{
private:
	BYTE lastCycles;
	WORD cyclesLCD;
	WORD cyclesTimer;
	WORD cyclesDIV;
	int cyclesPad;
	Video *v;
	QueueLog *log;
public:
	CPU(Video *v, Cartridge *c);
	~CPU();
	
	void Run(unsigned long exitCycles);
	void Reset();
	void SaveLog();
private:
	BYTE CiclosInstruccion(WORD OpCode);
	void OpCodeCB(Instructions * inst);
	void CyclicTasks();
	void UpdateStateLCD();
	void UpdateTimer();
	void Interruptions(Instructions * inst);
	void eventsSDL();
};

#endif
