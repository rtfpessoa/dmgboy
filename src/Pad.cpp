#include "Pad.h"

Pad::Pad(void)
{
	for (int i=0; i<8; i++)
		joypad[i] = 0;
}

Pad::~Pad(void)
{
}

BYTE Pad::updateInput(BYTE valueP1)
{
	//valueP1 = valueP1 | 0x0F;
	if(!BIT5(valueP1))
		return ((valueP1 & 0x30) |
			(!joypad[jSTART] << 3) | (!joypad[jSELECT] << 2) | (!joypad[jB] << 1) | (!joypad[jA]));

	if(!BIT4(valueP1))
		return ((valueP1 & 0x30) |
			(!joypad[jDOWN] << 3) | (!joypad[jUP] << 2) | (!joypad[jLEFT] << 1) | (!joypad[jRIGHT]));

	return ((valueP1 & 0x30) | 0x0F);
}

//Si devuelve 1 debe producirse una petición de interrupción
//En caso negativo devolverá 0
BYTE Pad::updateKey(int eventType, SDLKey key, BYTE *valueP1)
{
	char * keyName;

	keyName = SDL_GetKeyName(key);
	/*if (eventType == SDL_KEYDOWN)
		printf( ">El usuario pulsa la tecla %s.\n", keyName);
	else
		printf( ">El usuario suelta la tecla %s.\n", keyName);*/

	switch(key)
	{
		case(kA):
			joypad[jA] = (eventType == SDL_KEYDOWN);
			break;
		case(kB):
			joypad[jB] = (eventType == SDL_KEYDOWN);
			break;
		case(kSE):
			joypad[jSELECT] = (eventType == SDL_KEYDOWN);
			break;
		case(kST):
			joypad[jSTART] = (eventType == SDL_KEYDOWN);
			break;
		case(kL):
			joypad[jLEFT] = (eventType == SDL_KEYDOWN);
			break;
		case(kR):
			joypad[jRIGHT] = (eventType == SDL_KEYDOWN);
			break;
		case(kU):
			joypad[jUP] = (eventType == SDL_KEYDOWN);
			break;
		case(kD):
			joypad[jDOWN] = (eventType == SDL_KEYDOWN);
			break;
		default:
			return 0;
	}

	*valueP1 = updateInput(*valueP1);

	if (eventType == SDL_KEYDOWN)
	{
		if(!BIT5(*valueP1))
			if ((key == kA) || (key == kB) || (key == kSE) || (key == kST))
				return 1;

		if(!BIT4(*valueP1))
			if ((key == kL) || (key == kR) || (key == kU) || (key == kD))
				return 1;
	}

	return 0;
}
