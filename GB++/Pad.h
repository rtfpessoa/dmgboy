#ifndef __PAD_H__
#define __PAD_H__

#include "SDL.h"
#include "Memory.h"

enum e_joypad {jRIGHT, jLEFT, jUP, jDOWN, jA, jB, jSELECT, jSTART};

class Pad
{
private:
	BYTE joypad[8];
	Memory *mem;
public:
	Pad(void);
	~Pad(void);
	void SetMem(Memory *mem);
	void updateKey(int eventType, SDLKey key);
};

#endif