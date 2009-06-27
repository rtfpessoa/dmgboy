#ifndef __PAD_H__
#define __PAD_H__

#include <SDL.h>
#include "Def.h"

enum e_keyConf
{
	kR = SDLK_RIGHT,
	kL = SDLK_LEFT,
	kU = SDLK_UP,
	kD = SDLK_DOWN,
	kA = SDLK_a,
	kB = SDLK_s,
	kSE = SDLK_q,
	kST = SDLK_w
};

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
