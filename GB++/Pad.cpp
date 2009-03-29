#include "StdAfx.h"
#include "Pad.h"

Pad::Pad(void)
{
}

Pad::~Pad(void)
{
}

void Pad::SetMem(Memory *mem)
{
	this->mem = mem;
}

void Pad::updateKey(int eventType, SDLKey key)
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
			joypad[jUP] = (eventType == SDL_KEYDOWN);
			break;
	}

	if (eventType == SDL_KEYDOWN)
	{
		if(!BIT5(mem->MemR(P1)))
			if ((key == SDLK_a) || (key == SDLK_s) || (key == SDLK_q) || (key == SDLK_w))
				mem->MemW(IF, mem->MemR(IF) | 0x10);

		if(!BIT4(mem->MemR(P1)))
			if ((key == SDLK_LEFT) || (key == SDLK_RIGHT) || (key == SDLK_UP) || (key == SDLK_DOWN))
				mem->MemW(IF, mem->MemR(IF) | 0x10);
	}

}