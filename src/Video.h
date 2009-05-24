#ifndef __VIDEO_H__
#define __VIDEO_H__

#include "Definiciones.h"
#include "Memory.h"
#include "map"
#include <SDL.h>

class Video
{
private:
	Memory *mem;
	SDL_Surface *screen, *hideScreen;
	multimap<BYTE, WORD> orderedOAM;
	Uint32 colours[4];
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
	inline void DrawPixel(SDL_Surface *screen, Uint32 colour, BYTE x, BYTE y);
	void GetPalette(Uint32 * palette, WORD dir);
};

#endif
