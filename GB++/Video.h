#ifndef __VIDEO_H__
#define __VIDEO_H__

#include "SDL.h"
#include "Memory.h"

class Video
{
private:
	Memory *mem;
	SDL_Surface *screen, *hideScreen;
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
	void DrawPixel(SDL_Surface *screen, Uint8 R, Uint8 G, Uint8 B, BYTE x, BYTE y);
	void ObtainPalette(BYTE * palette);
};

#endif