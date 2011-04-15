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
#include "Sound.h"
#include "Pad.h"
#include "Cartridge.h"
#include "Log.h"

class CPU: public Registers, public Memory
{
private:
	unsigned long numInstructions;
	unsigned long actualCycles;
	BYTE lastCycles;
	WORD cyclesLCD;
	WORD cyclesTimer;
	WORD cyclesDIV;
	WORD cyclesSerial;
	int bitSerial;
	Video *v;
	QueueLog *log;
	BYTE instructionCycles[0xFF];
	BYTE instructionCyclesCB[0xFF];
	bool frameCompleted;
public:
	CPU(Video *v, Sound * s);
	CPU(Video *v, Cartridge *c, Sound * s);
	~CPU();
	
	void ExecuteOneFrame();
	void UpdatePad();
	void Reset();
	void SaveLog();
	void SaveState(std::string saveDirectory, int numSlot);
	void LoadState(std::string loadDirectory, int numSlot);
private:
	void Init(Video *v);
	void FillInstructionCycles();
	void FillInstructionCyclesCB();
	void OpCodeCB(Instructions * inst);
	void UpdateStateLCD();
	void UpdateTimer();
	void UpdateSerial();
	void Interrupts(Instructions * inst);
	void CheckLYC();
	void OnEndFrame();
};

#endif
