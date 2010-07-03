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

#include <wx/dcbuffer.h>
#include <wx/image.h>
#include "wxSDLScreen.h"
#include "wxIDControls.h"
#include "Def.h"

inline void SDLScreen::onEraseBackground(wxEraseEvent &) { /* do nothing */ }

IMPLEMENT_CLASS(SDLScreen, wxPanel)

BEGIN_EVENT_TABLE(SDLScreen, wxPanel)
EVT_PAINT(SDLScreen::onPaint)
EVT_ERASE_BACKGROUND(SDLScreen::onEraseBackground)
EVT_IDLE(SDLScreen::onIdle)
END_EVENT_TABLE()

SDLScreen::SDLScreen(wxWindow *parent) : wxPanel(parent, IDP_PANEL), screen(0) {
    // ensure the size of the wxPanel
    wxSize size(SCREEN_W, SCREEN_H);
    
    SetMinSize(size);
    SetMaxSize(size);
	
	createScreen();
}

SDLScreen::~SDLScreen() {
    if (screen) {
        SDL_FreeSurface(screen);
    }
}

void SDLScreen::onPaint(wxPaintEvent &) {
    // can't draw if the screen doesn't exist yet
    if (!screen) {
        return;
    }
    
    // lock the surface if necessary
    if (SDL_MUSTLOCK(screen)) {
        if (SDL_LockSurface(screen) < 0) {
            return;
        }
    }
    
    // create a bitmap from our pixel data
    wxBitmap bmp(wxImage(screen->w, screen->h, 
						 static_cast<unsigned char *>(screen->pixels), true));
    
    // unlock the screen
    if (SDL_MUSTLOCK(screen)) {
        SDL_UnlockSurface(screen);
    }
    
    // paint the screen
    wxBufferedPaintDC dc(this, bmp);
}

void SDLScreen::onVideoPreDraw()
{
	if ( SDL_MUSTLOCK(screen) )
    {
        if ( SDL_LockSurface(screen) < 0 )
		{
            return;
        }
    }
}

void SDLScreen::onVideoPostDraw()
{
	if ( SDL_MUSTLOCK(screen) )
    {
        SDL_UnlockSurface(screen);
    }
}

void SDLScreen::onIdle(wxIdleEvent &) {
    
    // refresh the panel
    Refresh(false);
    
    // throttle to keep from flooding the event queue
    wxMilliSleep(33);
}

void SDLScreen::onVideoRefreshScreen()
{
	//Copiar la superficie oculta a la principal
	SDL_BlitSurface(hideScreen, NULL, screen, NULL);
	//Hacer el cambio
	SDL_Flip(screen);
}

void SDLScreen::createScreen() {
    if (!screen) {
        int width, height;
        GetSize(&width, &height);
        
        screen = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 
                                      24, 0, 0, 0, 0);
		
		//Inicializar un array con los 4 colores posibles (negro, gris oscuro, gris claro, blanco)
		for (int i=0; i<4; i++)
			colors[i] = SDL_MapRGB(screen->format, i*85, i*85, i*85);
		
		hideScreen = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 16, 0,0,0,0);
    }
}

//idColor = 0, 1, 2, 3 = negro, gris oscuro, gris claro, blanco
void SDLScreen::onVideoDrawPixel(int idColor, int x, int y)
{
	Uint32 color = colors[idColor];
    int bpp = hideScreen->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)hideScreen->pixels + y * hideScreen->pitch + x * bpp;
	
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