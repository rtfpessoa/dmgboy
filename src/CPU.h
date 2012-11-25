/*
 This file is part of DMGBoy.
 
 DMGBoy is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 DMGBoy is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with DMGBoy.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __CPU_H__
#define __CPU_H__

#include "Registers.h"
#include "Memory.h"

class QueueLog;
class Video;
class Instructions;
class Cartridge;

class CPU: public Registers, public Memory
{
private:
	unsigned long numInstructions;
	unsigned long actualCycles;
	BYTE lastCycles;
	int cyclesLCD;
	WORD cyclesTimer;
	WORD cyclesDIV;
	WORD cyclesSerial;
	int bitSerial;
	Video *v;
#ifdef MAKEGBLOG
	QueueLog *log;
#endif
	bool frameCompleted;
	bool VBlankIntPending;
    bool newInterrupt;
public:
	CPU(Video *v, Sound * s);
	CPU(Video *v, Cartridge *c, Sound * s);
	~CPU();
	
	void ExecuteOneFrame();
	void UpdatePad();
    void OnWriteLCDC(BYTE value);
    BYTE TACChanged(BYTE newValue);
    BYTE DIVChanged(BYTE newValue);
    BYTE P1Changed(BYTE newValue);
	void Reset();
#ifdef MAKEGBLOG
	void SaveLog();
#endif
	void SaveState(std::string saveDirectory, int numSlot);
	void LoadState(std::string loadDirectory, int numSlot);
private:
	void Init(Video *v);
    void ResetGlobalVariables();
	void OpCodeCB(Instructions * inst);
	void UpdateStateLCD(int cycles);
    void UpdateStateLCDOn();
	void UpdateTimer(int cycles);
	void UpdateSerial(int cycles);
    void SetIntFlag(int bit);
	void Interrupts(Instructions * inst);
	void CheckLYC();
	void OnEndFrame();
    void ChangeSpeed();
};

#endif
