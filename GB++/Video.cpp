#include "StdAfx.h"
#include "Video.h"
#include "math.h"
#include "windows.h"

Video::Video(void)
{
	//if ( SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO) < 0 ) {
	if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		cerr << "No se puede iniciar SDL: " << SDL_GetError() << endl;
        exit(1);
    }

    //screen = SDL_SetVideoMode(160, 144, 16, SDL_SWSURFACE);
	screen = SDL_SetVideoMode(256, 256, 16, SDL_HWSURFACE | SDL_DOUBLEBUF);
    if ( screen == NULL ) 
    {
		cerr << "No se puede establecer el modo de video: " << SDL_GetError() << endl;
        exit(1);
    }

	hideScreen = SDL_CreateRGBSurface(SDL_SWSURFACE, 256, 256, 16, 0,0,0,0);
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
	OrderOAM(y);
	UpdateBG(y);
	UpdateWin(y);
	UpdateOAM(y);
}

void Video::RefreshScreen()
{
	//Copiar la superficie oculta a la principal
	SDL_BlitSurface(hideScreen, NULL, screen, NULL);
	//Hacer el cambio
	SDL_Flip(screen);
	//cout << "Refresco de pantalla: ";
	//system("PAUSE");
	cout << ".";
	//Sleep(20);
}

void Video::UpdateBG(BYTE y)
{
	WORD x;
	BYTE x_tile, y_tile, colour;
	BYTE line[2];
	BYTE palette[4];
	int xScrolled, yScrolled;
	WORD map_ini, map, dir_tile;

	//Seleccionar el tile map
	map_ini = BIT3(mem->MemR(LCDC)) ? 0x9C00 : 0x9800;

	GetPalette(palette, BGP);

	yScrolled = (y + mem->MemR(SCY));
	if (yScrolled < 0)
		yScrolled += 256;
	else if (yScrolled > 255)
		yScrolled -= 256;

	//for (x=0; x<160; x++)
	for (x=0; x<256; x++)
	{
		//Si el LCD o Background desactivado 
		//pintamos la linea de negro
		if (!BIT7(mem->MemR(LCDC)) || !BIT0(mem->MemR(LCDC)))
		{
			DrawPixel(hideScreen, 255, 255, 255, x, y);
			//DrawPixel(screen, 0, 0, 0, x, y);
			continue;
		}

		map = map_ini + ((yScrolled/8 * 32) + x/8);
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
		x_tile = x % 8;

		line[0] = mem->MemR(dir_tile + (y_tile * 2));	//y_tile * 2 porque cada linea de 1 tile ocupa 2 bytes
		line[1] = mem->MemR(dir_tile + (y_tile * 2) + 1);
		
		BYTE pixX = (BYTE)abs((int)x_tile - 7);
		//Un pixel lo componen 2 bits. Seleccionar la posicion del bit en los dos bytes (line[0] y line[1])
		//Esto devolverá un numero de color que junto a la paleta de color nos dará el color requerido
		colour = palette[(((line[1] & ((BYTE)pow(2.0f, pixX))) >> pixX) << 1) |
						  ((line[0] & (BYTE)(pow(2.0f, pixX))) >> pixX)];

		xScrolled = (x - mem->MemR(SCX));
		if (xScrolled < 0)
			xScrolled += 256;

		DrawPixel(hideScreen, colour, colour, colour, xScrolled, y);
	}
}

void Video::UpdateWin(BYTE y)
{
	WORD map_ini, map, x, dir_tile, wndPosY;
	BYTE x_tile, y_tile, colour, xIni, xScrolled, yScrolled;
	BYTE line[2];
	BYTE palette[4];
	unsigned short wndPosX;

	//Si la ventana está desactivada no hacemos nada
	if (!BIT5(mem->MemR(LCDC)))
		return;

	wndPosX = mem->MemR(WX) - 7;
	wndPosY = mem->MemR(WY);

	if (y < wndPosY)
		return;

	if (wndPosX < 0) xIni = 0;
	else if (wndPosX > 160) xIni = 160;
	else xIni = wndPosX;

	GetPalette(palette, BGP);

	map_ini = BIT6(mem->MemR(LCDC)) ? 0x9C00 : 0x9800;

	for (x=xIni; x<160; x++)
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
		colour = palette[(((line[1] & ((BYTE)pow(2.0f, pixX))) >> pixX) << 1) |
						  ((line[0] & (BYTE)(pow(2.0f, pixX))) >> pixX)];

		DrawPixel(hideScreen, colour, colour, colour, x, y);
	}
}

void Video::OrderOAM(BYTE y)
{
	BYTE ySprite, hSprite;
	WORD dir;

	orderedOAM.clear();

	if (!BIT1(mem->MemR(LCDC)))	//OAM desactivado
		return;

	hSprite = BIT2(mem->MemR(LCDC)) ? 16 : 8;
	//!!!!Tal vez el modo 8x16 no esté bien

	for(dir=0xFE00; dir<0xFEA0; dir+=0x04)
	{
		ySprite = mem->MemR(dir);

		if ((ySprite==0) || (ySprite>=160))	//La y está fuera de la pantalla
			continue;

		ySprite -= 16;
		if ((ySprite > y-hSprite) && (ySprite <= y))
			orderedOAM.insert(pair<BYTE, WORD>(mem->MemR(dir+1), dir));
	}
}

void Video::UpdateOAM(BYTE y)
{
	BYTE xSprite, ySprite, attrSprite, x, xTile, yTile, colour, xFlip, yFlip, countX, countY;
	WORD dirSprite, tileNumber, dirTile;
	BYTE palette[4], palette2[4];
	BYTE line[2];

	if (!BIT1(mem->MemR(LCDC)))	//OAM desactivado
		return;

	//if (BIT2(mem->MemR(LCDC)))
	//	cout << "8x16" << endl;

	GetPalette(palette, OBP0);

	multimap<BYTE, WORD>::iterator it;

	for (it=orderedOAM.begin(); it != orderedOAM.end(); it++)
	{
		dirSprite = (*it).second;
		ySprite = mem->MemR(dirSprite) - 16;	//=mem->MemR(dirSprite + 0);
		xSprite = (*it).first - 8;			//=mem->MemR(dirSprite + 1);
		tileNumber = mem->MemR(dirSprite + 2);
		attrSprite = mem->MemR(dirSprite + 3);
		dirTile = 0x8000 + tileNumber*16;
		xFlip = BIT5(attrSprite);
		yFlip = BIT6(attrSprite);

		xTile = countX = countY = 0;
		yTile = y - ySprite;
		countY = yTile;
		if (yFlip) yTile = (BYTE)abs((int)yTile - 7);

		for (x=xSprite; ((x<xSprite+8) && (x<0xff)); x++)
		{
			xTile = xFlip ? (BYTE)abs((int)countX - 7) : countX;

			line[0] = mem->MemR(dirTile + (yTile * 2));	//yTile * 2 porque cada linea de 1 tile ocupa 2 bytes
			line[1] = mem->MemR(dirTile + (yTile * 2) + 1);
			
			BYTE pixX = (BYTE)abs((int)xTile - 7);
			//Un pixel lo componen 2 bits. Seleccionar la posicion del bit en los dos bytes (line[0] y line[1])
			//Esto devolverá un numero de color que junto a la paleta de color nos dará el color requerido
			BYTE index = (((line[1] & ((BYTE)pow(2.0f, pixX))) >> pixX) << 1) |
							  ((line[0] & (BYTE)(pow(2.0f, pixX))) >> pixX);

			//El 0 es transparente (no pintar)
			if (index)
			{
				colour = palette[index];

				DrawPixel(hideScreen, colour, colour, colour, xSprite + countX, ySprite + countY);
			}

			countX++;
		}
	}
}

void Video::GetPalette(BYTE * palette, WORD dir)
{
	BYTE paletteData;

	paletteData = mem->MemR(dir);
	
	palette[0] = abs((BITS01(paletteData) - 3)) * 85;
	palette[1] = abs(((BITS23(paletteData) >> 2) - 3)) * 85;
	palette[2] = abs(((BITS45(paletteData) >> 4) - 3)) * 85;
	palette[3] = abs(((BITS67(paletteData) >> 6) - 3)) * 85;
}

void Video::DrawPixel(SDL_Surface *screen, Uint8 R, Uint8 G, Uint8 B, BYTE x, BYTE y)
{
    Uint32 color = SDL_MapRGB(screen->format, R, G, B);

    if ( SDL_MUSTLOCK(screen) ) 
    {
        if ( SDL_LockSurface(screen) < 0 ) 
		{
            return;
        }
    }

    switch (screen->format->BytesPerPixel) 
    {    
		case 1: 
		{ /* Asumimos 8-bpp */
			Uint8 *bufp;

			bufp = (Uint8 *)screen->pixels + y*screen->pitch + x;
			*bufp = color;
        }
        break;

		case 2: 
		{ /* Probablemente 15-bpp o 16-bpp */
            Uint16 *bufp;

            bufp = (Uint16 *)screen->pixels + y*screen->pitch/2 + x;
            *bufp = color;
        }
        break;

		case 3: 
		{ /* Modo lento 24-bpp, normalmente no usado */
            Uint8 *bufp;

            bufp = (Uint8 *)screen->pixels + y*screen->pitch + x;
            *(bufp+screen->format->Rshift/8) = R;
            *(bufp+screen->format->Gshift/8) = G;
            *(bufp+screen->format->Bshift/8) = B;
        }
        break;

		case 4: 
		{ /* Probablemente 32-bpp */
            Uint32 *bufp;

            bufp = (Uint32 *)screen->pixels + y * screen->pitch/4 + x;
            *bufp = color;
        }
        break;
    }

    if ( SDL_MUSTLOCK(screen) ) 
    {
        SDL_UnlockSurface(screen);
    }
}