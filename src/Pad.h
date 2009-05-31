#ifndef __PAD_H__
#define __PAD_H__

#include <SDL.h>
#include "Def.h"

enum e_joypad {jRIGHT, jLEFT, jUP, jDOWN, jA, jB, jSELECT, jSTART};

class Pad
{
private:
	BYTE joypad[8];
public:
	Pad(void);
	~Pad(void);
	BYTE updateInput(BYTE valueP1);
	BYTE updateKey(int eventType, SDLKey key, BYTE *valueP1);
};

#endif
