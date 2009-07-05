#include "Video.h"
#include <iostream>

using namespace std;

Video::Video(void)
{
	//if ( SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO) < 0 ) {
	if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		cerr << "No se puede iniciar SDL: " << SDL_GetError() << endl;
        exit(1);
    }

	screen = SDL_SetVideoMode(SCREEN_W, SCREEN_H, 16, SDL_HWSURFACE | SDL_DOUBLEBUF);
    if ( screen == NULL )
    {
		cerr << "No se puede establecer el modo de video: " << SDL_GetError() << endl;
        exit(1);
    }

	hideScreen = SDL_CreateRGBSurface(SDL_SWSURFACE, SCREEN_W, SCREEN_H, 16, 0,0,0,0);

	for (int i=0; i<4; i++)
		colors[i] = SDL_MapRGB(hideScreen->format, i*85, i*85, i*85);

	SDL_WM_SetCaption("GB++", "icon");
}

Video::~Video(void)
{
}

void Video::SetMem(Memory *mem)
{
	this->mem = mem;
}

void Video::Close()
{
	atexit(SDL_Quit);
}

void Video::UpdateLine(BYTE y)
{
	if ( SDL_MUSTLOCK(screen) )
    {
        if ( SDL_LockSurface(screen) < 0 )
		{
            return;
        }
    }

	OrderOAM(y);
	UpdateBG(y);
	UpdateWin(y);
	UpdateOAM(y);

	if ( SDL_MUSTLOCK(screen) )
    {
        SDL_UnlockSurface(screen);
    }
}

void Video::RefreshScreen()
{
	//Copiar la superficie oculta a la principal
	SDL_BlitSurface(hideScreen, NULL, screen, NULL);
	//Hacer el cambio
	SDL_Flip(screen);
	cout << ".";
}

void Video::UpdateBG(int y)
{
	int x, indexColor;
	BYTE x_tile, y_tile, valueLCDC, valueSCX;
	int line[2];
	Uint32 color;
	Uint32 palette[4];
	int xScrolled, yScrolled;
	WORD map_ini, map, dir_tile;

	valueLCDC = mem->MemR(LCDC);
	valueSCX = mem->MemR(SCX);

	//Seleccionar el tile map
	map_ini = BIT3(valueLCDC) ? 0x9C00 : 0x9800;

	GetPalette(palette, BGP);

	yScrolled = (y + mem->MemR(SCY));
	if (yScrolled < 0)
		yScrolled += 256;
	else if (yScrolled > 255)
		yScrolled -= 256;

	for (x=0; x<SCREEN_W; x++)
	{
		//Si el LCD o Background desactivado
		//pintamos la linea de blanco
		if (!BIT7(valueLCDC) || !BIT0(valueLCDC))
		{
			DrawPixel(hideScreen, colors[3], x, y);
			//DrawPixel(hideScreen, colors[0], x, y);
			continue;
		}

		xScrolled = (x + valueSCX);
		if (xScrolled > 256)
			xScrolled -= 256;

		map = map_ini + ((yScrolled/8 * 32) + xScrolled/8);
		if (!BIT4(valueLCDC))	//Seleccionar el tile data
		{
			//0x8800 = 0x9000 - (128 * 16)
			dir_tile = (WORD)(0x9000 + ((char)mem->MemR(map))*16);	//Se multiplica por 16 porque cada tile ocupa 16 bytes
		}
		else
		{
			dir_tile = 0x8000 + mem->MemR(map)*16;
		}

		y_tile = yScrolled % 8;
		x_tile = xScrolled % 8;

		line[0] = mem->MemR(dir_tile + (y_tile * 2));	//y_tile * 2 porque cada linea de 1 tile ocupa 2 bytes
		line[1] = mem->MemR(dir_tile + (y_tile * 2) + 1);

		int pixX = (BYTE)abs((int)x_tile - 7);
		//Un pixel lo componen 2 bits. Seleccionar la posicion del bit en los dos bytes (line[0] y line[1])
		//Esto devolverá un numero de color que junto a la paleta de color nos dará el color requerido
		indexColor = (((line[1] & (0x01 << pixX)) >> pixX) << 1) | ((line[0] & (0x01 << pixX)) >> pixX);
		color = palette[indexColor];

		DrawPixel(hideScreen, color, x, y);
		indexColorsBGWnd[x][y] = indexColor;
	}
}

void Video::UpdateWin(int y)
{
	int x;
	WORD map_ini, map, dir_tile, wndPosY;
	BYTE x_tile, y_tile, xIni, xScrolled, yScrolled, indexColor;
	BYTE line[2];
	Uint32 color;
	Uint32 palette[4];
	short wndPosX;

	//Si la ventana está desactivada no hacemos nada
	if (!BIT5(mem->MemR(LCDC)))
		return;

	wndPosX = mem->MemR(WX) - 7;
	wndPosY = mem->MemR(WY);

	if (y < wndPosY)
		return;

	if (wndPosX < 0) xIni = 0;
	else if (wndPosX > SCREEN_W) xIni = SCREEN_W;
	else xIni = wndPosX;

	GetPalette(palette, BGP);

	map_ini = BIT6(mem->MemR(LCDC)) ? 0x9C00 : 0x9800;

	for (x=xIni; x<SCREEN_W; x++)
	{
		xScrolled = x - wndPosX;
		yScrolled = y - wndPosY;

		map = map_ini + ((yScrolled/8 * 32) + xScrolled/8);

		if (!BIT4(mem->MemR(LCDC)))	//Seleccionar el tile data
		{
			//0x8800 = 0x9000 - (128 * 16)
			dir_tile = (WORD)(0x9000 + ((char)mem->MemR(map))*16);	//Se multiplica por 16 porque cada tile ocupa 16 bytes
		}
		else
		{
			dir_tile = 0x8000 + mem->MemR(map)*16;
		}

		y_tile = yScrolled % 8;
		x_tile = xScrolled % 8;

		line[0] = mem->MemR(dir_tile + (y_tile * 2));	//y_tile * 2 porque cada linea de 1 tile ocupa 2 bytes
		line[1] = mem->MemR(dir_tile + (y_tile * 2) + 1);

		BYTE pixX = (BYTE)abs((int)x_tile - 7);
		//Un pixel lo componen 2 bits. Seleccionar la posicion del bit en los dos bytes (line[0] y line[1])
		//Esto devolverá un numero de color que junto a la paleta de color nos dará el color requerido
		indexColor = (((line[1] & (0x01 << pixX)) >> pixX) << 1) | ((line[0] & (0x01 << pixX)) >> pixX);
		color = palette[indexColor];

		DrawPixel(hideScreen, color, x, y);
		indexColorsBGWnd[x][y] = indexColor;
	}
}

void Video::OrderOAM(int y)
{
	short ySprite, hSprite;
	int dir;

	orderedOAM.clear();

	if (!BIT1(mem->MemR(LCDC)))	//OAM desactivado
		return;

	hSprite = BIT2(mem->MemR(LCDC)) ? 16 : 8;
	//!!!!Tal vez el modo 8x16 no esté bien

	for(dir=0xFE00; dir<0xFEA0; dir+=0x04)
	{
		ySprite = mem->MemR(dir);

		if ((ySprite==0) || (ySprite>=SCREEN_H+16))	//La y está fuera de la pantalla
			continue;

		ySprite -= 16;	//y en pantalla
		if ((ySprite > (short)y-hSprite) && (ySprite <= y))
			orderedOAM.insert(pair<int, int>(mem->MemR(dir+1), dir));
	}
}

void Video::UpdateOAM(int y)
{
	int x, xSprite;
	int attrSprite, xTile, yTile, xFlip, yFlip, countX, countY, behind, mode16, ySprite;
	WORD dirSprite, tileNumber, dirTile;
	Uint32 color;
	Uint32 palette[4], palette2[4];
	BYTE line[2];

	if (!BIT1(mem->MemR(LCDC)))	//OAM desactivado
		return;

	mode16 = BIT2(mem->MemR(LCDC));

	GetPalette(palette, OBP0);

	multimap<int, int>::iterator it;

	for (it=orderedOAM.begin(); it != orderedOAM.end(); it++)
	{
		dirSprite = (*it).second;
		ySprite = mem->MemR(dirSprite) - 16;	//=mem->MemR(dirSprite + 0);
		xSprite = (*it).first - 8;				//=mem->MemR(dirSprite + 1);
		if (xSprite == -8)
			continue;
		tileNumber = mem->MemR(dirSprite + 2);
		if (mode16)
			tileNumber = tileNumber & 0xF0;
		attrSprite = mem->MemR(dirSprite + 3);
		dirTile = 0x8000 + tileNumber*16;
		xFlip = BIT5(attrSprite);
		yFlip = BIT6(attrSprite);
		behind = BIT7(attrSprite);

		xTile = countX = countY = 0;
		yTile = y - ySprite;
		countY = yTile;
		if (yFlip) yTile = (BYTE)abs((int)yTile - 7);

		int xBeg = (xSprite>0) ? xSprite : 0;
		for (x=xBeg; ((x<xSprite+8) && (x<=SCREEN_W)); x++)
		{
			xTile = xFlip ? (BYTE)abs((int)countX - 7) : countX;

			line[0] = mem->MemR(dirTile + (yTile * 2));	//yTile * 2 porque cada linea de 1 tile ocupa 2 bytes
			line[1] = mem->MemR(dirTile + (yTile * 2) + 1);

			int pixX = abs((int)xTile - 7);
			//Un pixel lo componen 2 bits. Seleccionar la posicion del bit en los dos bytes (line[0] y line[1])
			//Esto devolverá un numero de color que junto a la paleta de color nos dará el color requerido
			BYTE index = (((line[1] & (0x01 << pixX)) >> pixX) << 1) | ((line[0] & (0x01 << pixX)) >> pixX);

			//El 0 es transparente (no pintar)
			if ((index) && ((!behind) || (!indexColorsBGWnd[xSprite + countX][ySprite + countY])))
			{
				color = palette[index];

				DrawPixel(hideScreen, color, xSprite + countX, ySprite + countY);
			}

			countX++;
		}
	}
}

void Video::GetPalette(Uint32 * palette, int dir)
{
	Uint32 paletteData;

	paletteData = mem->MemR(dir);

	palette[0] = colors[abs((int)(BITS01(paletteData) - 3))];
	palette[1] = colors[abs((int)((BITS23(paletteData) >> 2) - 3))];
	palette[2] = colors[abs((int)((BITS45(paletteData) >> 4) - 3))];
	palette[3] = colors[abs((int)((BITS67(paletteData) >> 6) - 3))];
}

void Video::DrawPixel(SDL_Surface *screen, Uint32 color, int x, int y)
{
    int bpp = screen->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)screen->pixels + y * screen->pitch + x * bpp;

    switch(bpp) {
    case 1:
        *p = color;
        break;

    case 2:
        *(Uint16 *)p = color;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (color >> 16) & 0xff;
            p[1] = (color >> 8) & 0xff;
            p[2] = color & 0xff;
        } else {
            p[0] = color & 0xff;
            p[1] = (color >> 8) & 0xff;
            p[2] = (color >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = color;
        break;
    }

}
