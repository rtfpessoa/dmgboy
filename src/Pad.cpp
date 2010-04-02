#include "Pad.h"

static int	kR = SDLK_RIGHT;
static int	kL = SDLK_LEFT;
static int	kU = SDLK_UP;
static int	kD = SDLK_DOWN;
static int	kA = SDLK_a;
static int	kB = SDLK_s;
static int	kSE = SDLK_q;
static int	kST = SDLK_w;

static BYTE joypad[8];

int updateInput(int valueP1)
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
int checkKey(int eventType, SDLKey key, int *valueP1)
{
	const char * keyName;

	keyName = SDL_GetKeyName(key);

	if (key == kR)
		joypad[jRIGHT] = (eventType == SDL_KEYDOWN);
	else if (key == kL)
		joypad[jLEFT] = (eventType == SDL_KEYDOWN);
	else if (key == kA)
		joypad[jA] = (eventType == SDL_KEYDOWN);
	else if (key == kB)
		joypad[jB] = (eventType == SDL_KEYDOWN);
	else if (key == kSE)
		joypad[jSELECT] = (eventType == SDL_KEYDOWN);
	else if (key == kST)
		joypad[jSTART] = (eventType == SDL_KEYDOWN);
	else if (key == kU)
		joypad[jUP] = (eventType == SDL_KEYDOWN);
	else if (key == kD)
		joypad[jDOWN] = (eventType == SDL_KEYDOWN);

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

int onCheckKeyPad(int valueP1)
{
	SDL_Event ev;

	while( SDL_PollEvent( &ev ) )
	{
		switch(ev.type)
		{
			case SDL_QUIT:
				printf( ">El usuario quiere salir.\n" );
				exit(0);
				break;
			case SDL_KEYDOWN:
				if (ev.key.keysym.sym == SDLK_ESCAPE)
				{
					printf("Presionar de nuevo ESC para salir o cualquier otra tecla para continuar\n");
					while(true)
					{
						SDL_WaitEvent(&ev);
						if (ev.type == SDL_KEYDOWN)
						{
							if (ev.key.keysym.sym == SDLK_ESCAPE)
								exit(0);
							else
								return 0;
						}
					}
				}
				//!!No hay break. Cuando SDL_KEYDOWN también SDL_KEYUP
			case SDL_KEYUP:
				return checkKey(ev.type, ev.key.keysym.sym, &valueP1);
				break;
		}
	}
	return 0;
}
