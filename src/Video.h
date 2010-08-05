/*
 This file is part of gbpablog.
 
 gbpablog is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 gbpablog is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with gbpablog.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __VIDEO_H__
#define __VIDEO_H__

#include "Def.h"
#include "Memory.h"
#include "map"
#include "GUI/wxSDLScreen.h"

class Video
{
private:
	Memory *mem;
	std::multimap<int, int> orderedOAM;	//posicion x, dir. memoria
	int indexColorsBGWnd[SCREEN_W][SCREEN_H];	//Indice de color en pantalla pintadas por background y window
	SDLScreen * screen;
public:
	Video(SDLScreen * screen);
	~Video(void);
	void SetMem(Memory *mem);
	void RefreshScreen();
	void ClearScreen();
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
