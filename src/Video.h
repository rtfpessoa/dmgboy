#ifndef __VIDEO_H__
#define __VIDEO_H__

#include "Def.h"
#include "Memory.h"
#include "map"
#include <SDL.h>

#define SCREEN_W 160
#define SCREEN_H 144

class Video
{
private:
	Memory *mem;
	SDL_Surface *screen, *hideScreen;
	multimap<BYTE, WORD> orderedOAM;
	Uint32 colors[4];
	BYTE indexColorsBGWnd[SCREEN_W][SCREEN_H];	//Indice de color en pantalla pintadas por background y window 
public:
	Video();
	~Video(void);
	void SetMem(Memory *mem);
	void RefreshScreen();
	void UpdateLine(BYTE line);
	void Close();
private:
	void UpdateBG(BYTE line);
	void UpdateWin(BYTE line);
	void OrderOAM(BYTE line);
	void UpdateOAM(BYTE line);
	inline void DrawPixel(SDL_Surface *screen, Uint32 color, BYTE x, BYTE y);
	void GetPalette(Uint32 * palette, WORD dir);
};

#endif
