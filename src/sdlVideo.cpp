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

#include "sdlVideo.h"
#include <SDL.h>
#include <iostream>

using namespace std;

static SDL_Surface *screen;
static SDL_Surface *hideScreen;
static Uint32 colors[4];

void onVideoInit(int width, int height)
{
	//if ( SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO) < 0 ) {
	if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		cerr << "No se puede iniciar SDL: " << SDL_GetError() << endl;
        exit(1);
    }

	screen = SDL_SetVideoMode(width, height, 16, SDL_HWSURFACE | SDL_DOUBLEBUF);
	//screen = SDL_SetVideoMode(width, height, 16, SDL_HWSURFACE);
    if ( screen == NULL )
    {
		cerr << "No se puede establecer el modo de video: " << SDL_GetError() << endl;
        exit(1);
    }

	hideScreen = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 16, 0,0,0,0);

	//Inicializar un array con los 4 colores posibles (negro, gris oscuro, gris claro, blanco)
	for (int i=0; i<4; i++)
		colors[i] = SDL_MapRGB(hideScreen->format, i*85, i*85, i*85);

	SDL_WM_SetCaption("GB++", "icon");
}

void onVideoClose()
{
	atexit(SDL_Quit);
}

