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
#include "wxSDLFrame.h"
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
	
	windowParent = parent;
	
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

void SDLScreen::onIdle(wxIdleEvent &event) {
    
	//event.RequestMore(true);
    // throttle to keep from flooding the event queue
    //wxMilliSleep(33);
}

void SDLScreen::onVideoRefreshScreen()
{
	// refresh the panel
    Refresh(false);
}

void SDLScreen::createScreen() {
    if (!screen) {
        screen = SDL_CreateRGBSurface(SDL_SWSURFACE, SCREEN_W, SCREEN_H, 
                                      24, 0, 0, 0, 0);
		
		//Inicializar un array con los 4 colores posibles (negro, gris oscuro, gris claro, blanco)
		for (int i=0; i<4; i++)
			colors[i] = SDL_MapRGB(screen->format, i*85, i*85, i*85);
    }
}

//idColor = 0, 1, 2, 3 = negro, gris oscuro, gris claro, blanco
void SDLScreen::onVideoDrawPixel(int idColor, int x, int y)
{
	Uint32 color = colors[idColor];
	
	wxUint8 *pixels = static_cast<wxUint8 *>(screen->pixels) + 
	(y * screen->pitch) +
	(x * screen->format->BytesPerPixel);
	
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	pixels[0] = color & 0xFF;
	pixels[1] = (color >> 8) & 0xFF;
	pixels[2] = (color >> 16) & 0xFF;
#else
	pixels[0] = (color >> 16) & 0xFF;
	pixels[1] = (color >> 8) & 0xFF;
	pixels[2] = color & 0xFF;
#endif
	
}
