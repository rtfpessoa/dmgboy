#include "StdAfx.h"
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
	if (eventType == SDL_KEYDOWN)
		printf( ">El usuario pulsa la tecla %s.\n", keyName);
	else
		printf( ">El usuario suelta la tecla %s.\n", keyName);

	switch(key)
	{
		case(SDLK_a):
			printf( "PAD: A\n");
			joypad[jA] = (eventType == SDL_KEYDOWN);
			break;
		case(SDLK_s):
			printf( "PAD: B\n");
			joypad[jB] = (eventType == SDL_KEYDOWN);
			break;
		case(SDLK_q):
			printf( "PAD: SELECT\n");
			joypad[jSELECT] = (eventType == SDL_KEYDOWN);
			break;
		case(SDLK_w):
			printf( "PAD: START\n");
			joypad[jSTART] = (eventType == SDL_KEYDOWN);
			break;
		case(SDLK_LEFT):
			printf( "PAD: LEFT\n");
			joypad[jLEFT] = (eventType == SDL_KEYDOWN);
			break;
		case(SDLK_RIGHT):
			printf( "PAD: RIGHT\n");
			joypad[jRIGHT] = (eventType == SDL_KEYDOWN);
			break;
		case(SDLK_UP):
			printf( "PAD: UP\n");
			joypad[jUP] = (eventType == SDL_KEYDOWN);
			break;
		case(SDLK_DOWN):
			printf( "PAD: DOWN\n");
			joypad[jDOWN] = (eventType == SDL_KEYDOWN);
			break;
		default:
			return 0;
	}

	*valueP1 = updateInput(*valueP1);

	if (eventType == SDL_KEYDOWN)
	{
		if(!BIT5(*valueP1))
			if ((key == SDLK_a) || (key == SDLK_s) || (key == SDLK_q) || (key == SDLK_w))
				return 1;

		if(!BIT4(*valueP1))
			if ((key == SDLK_LEFT) || (key == SDLK_RIGHT) || (key == SDLK_UP) || (key == SDLK_DOWN))
				return 1;
	}

	return 0;
}