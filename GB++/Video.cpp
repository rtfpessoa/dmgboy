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

void Video::Close()
{
	atexit(SDL_Quit);
}

Video::~Video(void)
{
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



void Video::SetMem(Memoria *mem)
{
	this->mem = mem;
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
	if (BIT3(mem->MemR(LCDC)) == 0)
		map_ini = 0x9800;
	else
		map_ini = 0x9C00;

	ObtainPalette(palette);

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
		if ((BIT7(mem->MemR(LCDC)) == 0) || (BIT0(mem->MemR(LCDC)) == 0))
		{
			DrawPixel(hideScreen, 255, 255, 255, x, y);
			//DrawPixel(screen, 0, 0, 0, x, y);
			continue;
		}

		map = map_ini + ((yScrolled/8 * 32) + x/8);
		if (BIT4(mem->MemR(LCDC)) == 0)	//Seleccionar el tile data
		{
			//0x8800 = 0x9000 - (128 * 16)
			//dir_tile = (WORD)(((short)mem->MemR(map))*16 + 0x9000);	//Se multiplica por 16 porque cada tile ocupa 16 bytes
			dir_tile = (WORD)(0x9000 + ((char)mem->MemR(map))*16);	//Se multiplica por 16 porque cada tile ocupa 16 bytes
		}
		else
		{
			//dir_tile = mem->MemR(map)*16 + 0x8000;
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
	//RefreshScreen();
}

void Video::ObtainPalette(BYTE * palette)
{
	BYTE palette_data;

	palette_data = mem->MemR(BGP);
	
	palette[0] = abs((BITS01(palette_data) - 3)) * 85;
	palette[1] = abs(((BITS23(palette_data) >> 2) - 3)) * 85;
	palette[2] = abs(((BITS45(palette_data) >> 4) - 3)) * 85;
	palette[3] = abs(((BITS67(palette_data) >> 6) - 3)) * 85;
}