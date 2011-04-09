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

#include <iostream>
#include "Video.h"

using namespace std;

Video::Video(IGBScreenDraw * screen)
{
	this->pixel = new VideoPixel();
	this->screen = screen;
}

Video::~Video(void)
{
	
}

void Video::SetMem(Memory *mem)
{
	this->mem = mem;
}

void Video::UpdateLine(BYTE y)
{
	screen->OnPreDraw();

	OrderOAM(y);
	UpdateBG(y);
	UpdateWin(y);
	UpdateOAM(y);

	screen->OnPostDraw();
}

void Video::RefreshScreen()
{
	screen->OnRefreshScreen();
}

void Video::ClearScreen()
{
	screen->OnClear();
}

void Video::UpdateBG(int y)
{
	int x, yScrolled;
	BYTE valueLCDC, valueSCX;
	bool display;

	valueLCDC = mem->memory[LCDC];
	valueSCX = mem->memory[SCX];

	display = BIT7(valueLCDC) && BIT0(valueLCDC);
	
	//Si el LCD o Background desactivado
	//pintamos la linea de blanco
	if (!display)
	{
		for (x=0; x<GB_SCREEN_W; x++)
			screen->OnDrawPixel(3, x, y);
		
		return;
	}
	
	//Seleccionar el tile map
	pixel->mapIni = BIT3(valueLCDC) ? 0x9C00 : 0x9800;

	GetPalette(pixel->palette, BGP);

	yScrolled = (y + mem->memory[SCY]);
	if (yScrolled < 0)
		yScrolled += 256;
	else if (yScrolled > 255)
		yScrolled -= 256;
	
	pixel->yTile = yScrolled % 8;
	pixel->rowMap = (yScrolled/8 * 32);
	
	pixel->tileDataSelect = BIT4(valueLCDC);
	
	for (x=0; x<GB_SCREEN_W; x++)
	{
		pixel->x = x;
		pixel->xScrolled = (x + valueSCX);
		if (pixel->xScrolled > 255)
			pixel->xScrolled -= 256;

		GetColor(pixel);

		screen->OnDrawPixel(pixel->color, x, y);
		indexColorsBGWnd[x][y] = pixel->indexColor;
	}
}

void Video::UpdateWin(int y)
{
	int x, wndPosX, xIni, yScrolled;
	WORD wndPosY;

	//Si la ventana esta desactivada no hacemos nada
	if (!BIT5(mem->memory[LCDC]))
		return;

	wndPosX = mem->memory[WX] - 7;
	wndPosY = mem->memory[WY];

	if (y < wndPosY)
		return;

	if (wndPosX < 0) xIni = 0;
	else if (wndPosX > GB_SCREEN_W) xIni = GB_SCREEN_W;
	else xIni = wndPosX;

	GetPalette(pixel->palette, BGP);

	pixel->mapIni = BIT6(mem->memory[LCDC]) ? 0x9C00 : 0x9800;
	
	yScrolled = y - wndPosY;
	pixel->yTile = yScrolled % 8;
	pixel->rowMap = yScrolled/8 * 32;
	
	pixel->tileDataSelect = BIT4(mem->memory[LCDC]);
	
	pixel->y = y;

	for (x=xIni; x<GB_SCREEN_W; x++)
	{
		pixel->x = x;
		pixel->xScrolled = x - wndPosX;

		GetColor(pixel);

		screen->OnDrawPixel(pixel->color, x, y);
		indexColorsBGWnd[x][y] = pixel->indexColor;
	}
}

inline void Video::GetColor(VideoPixel * p)
{
	BYTE xTile, line[2];
	WORD idTile, dirTile;
	
	idTile = p->mapIni + (p->rowMap + p->xScrolled/8);
	
	if (!p->tileDataSelect)	//Seleccionar el tile data
	{
		//0x8800 = 0x9000 - (128 * 16)
		dirTile = (WORD)(0x9000 + ((char)mem->memory[idTile])*16);	//Se multiplica por 16 porque cada tile ocupa 16 bytes
	}
	else
	{
		dirTile = 0x8000 + mem->memory[idTile]*16;
	}
	
	xTile = p->xScrolled % 8;
	
	line[0] = mem->memory[dirTile + (p->yTile * 2)];	//yTile * 2 porque cada linea de 1 tile ocupa 2 bytes
	line[1] = mem->memory[dirTile + (p->yTile * 2) + 1];
	
	BYTE pixX = (BYTE)abs((int)xTile - 7);
	//Un pixel lo componen 2 bits. Seleccionar la posicion del bit en los dos bytes (line[0] y line[1])
	//Esto devolvera un numero de color que junto a la paleta de color nos dara el color requerido
	p->indexColor = (((line[1] & (0x01 << pixX)) >> pixX) << 1) | ((line[0] & (0x01 << pixX)) >> pixX);
	p->color = p->palette[p->indexColor];
}

void Video::OrderOAM(int y)
{
	int ySprite, hSprite, dir;

	orderedOAM.clear();

	if (!BIT1(mem->memory[LCDC]))	//OAM desactivado
		return;

	hSprite = BIT2(mem->memory[LCDC]) ? 16 : 8;

	for(dir=0xFE00; dir<0xFEA0; dir+=0x04)
	{
		ySprite = mem->memory[dir];

		ySprite -= 16;	//y en pantalla
		if ((ySprite > y-hSprite) && (ySprite <= y))
				orderedOAM.insert(pair<int, int>(mem->memory[dir+1], dir));
	}
}

void Video::UpdateOAM(int y)
{
	int x, xSprite, numSpritesLine, xBeg;
	int attrSprite, xTile, yTile, xFlip, yFlip, countX, countY, behind, mode16, ySprite;
	WORD dirSprite, tileNumber, dirTile;
	int color;
	int palette0[4], palette1[4];
	BYTE line[2];

	if (!BIT1(mem->memory[LCDC]))	//OAM desactivado
		return;

	mode16 = BIT2(mem->memory[LCDC]);

	GetPalette(palette0, OBP0);
	GetPalette(palette1, OBP1);

	multimap<int, int>::iterator it;

	numSpritesLine = 0;

	for (it=orderedOAM.begin(); (it != orderedOAM.end()) && (numSpritesLine < 10); it++)
	{
		numSpritesLine++;

		dirSprite = (*it).second;
		ySprite = mem->memory[dirSprite] - 16;	//=mem->MemR(dirSprite + 0);
		xSprite = (*it).first - 8;				//=mem->MemR(dirSprite + 1);
		if (xSprite == -8)
			continue;
		tileNumber = mem->memory[dirSprite + 2];
		if (mode16)
			tileNumber = tileNumber & 0xFE;
			//!!!!!!!!!Si toca la parte de abajo del tile de 8x16 hay que sumar uno (tileNumber | 0x01)
		attrSprite = mem->memory[dirSprite + 3];
		dirTile = 0x8000 + tileNumber*16;
		xFlip = BIT5(attrSprite);
		yFlip = BIT6(attrSprite);
		behind = BIT7(attrSprite);

		xTile = countX = countY = 0;
		yTile = y - ySprite;
		countY = yTile;
		if (yFlip)
			yTile = abs(yTile - (mode16 ? 15 : 7));

		if (xSprite>0)
		{
			xBeg = xSprite;
			countX = 0;
		}
		else
		{
			xBeg = 0;
			countX = abs(xSprite);
		}

		for (x=xBeg; ((x<xSprite+8) && (x<GB_SCREEN_W)); x++)
		{
			xTile = xFlip ? abs(countX - 7) : countX;

			line[0] = mem->memory[dirTile + (yTile * 2)];	//yTile * 2 porque cada linea de 1 tile ocupa 2 bytes
			line[1] = mem->memory[dirTile + (yTile * 2) + 1];

			int pixX = abs((int)xTile - 7);
			//Un pixel lo componen 2 bits. Seleccionar la posicion del bit en los dos bytes (line[0] y line[1])
			//Esto devolvera un numero de color que junto a la paleta de color nos dara el color requerido
			BYTE index = (((line[1] & (0x01 << pixX)) >> pixX) << 1) | ((line[0] & (0x01 << pixX)) >> pixX);

			//El 0 es transparente (no pintar)
			if ((index) && ((!behind) || (!indexColorsBGWnd[xSprite + countX][ySprite + countY])))
			{
				//color = !BIT4(attrSprite) ? palette0[index] : palette1[index];
				if (!BIT4(attrSprite))
					color = palette0[index];
				else
					color = palette1[index];


				screen->OnDrawPixel(color, xSprite + countX, ySprite + countY);
			}

			countX++;
		}
	}
}

void Video::GetPalette(int * palette, int dir)
{
	BYTE paletteData = mem->memory[dir];

	palette[0] = abs((int)(BITS01(paletteData) - 3));
	palette[1] = abs((int)((BITS23(paletteData) >> 2) - 3));
	palette[2] = abs((int)((BITS45(paletteData) >> 4) - 3));
	palette[3] = abs((int)((BITS67(paletteData) >> 6) - 3));
}
