#ifndef __VIDEO_H__
#define __VIDEO_H__

#include "Def.h"
#include "Memory.h"
#include "map"

#define SCREEN_W 160
#define SCREEN_H 144

class Video
{
private:
	Memory *mem;
	std::multimap<int, int> orderedOAM;	//posicion x, dir. memoria
	int indexColorsBGWnd[SCREEN_W][SCREEN_H];	//Indice de color en pantalla pintadas por background y window 
public:
	Video();
	~Video(void);
	void SetMem(Memory *mem);
	void RefreshScreen();
	void UpdateLine(BYTE line);
	void Close();
private:
	void UpdateBG(int line);
	void UpdateWin(int line);
	void OrderOAM(int line);
	void UpdateOAM(int line);
	void GetPalette(int * palette, int dir);
};

#endif
