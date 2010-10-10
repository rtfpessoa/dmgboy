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
#include "../Def.h"
#include "../Settings.h"

inline void SDLScreen::onEraseBackground(wxEraseEvent &) { /* do nothing */ }

IMPLEMENT_CLASS(SDLScreen, wxPanel)

BEGIN_EVENT_TABLE(SDLScreen, wxPanel)
EVT_PAINT(SDLScreen::onPaint)
EVT_ERASE_BACKGROUND(SDLScreen::onEraseBackground)
END_EVENT_TABLE()

SDLScreen::SDLScreen(wxWindow *parent) : wxPanel(parent, ID_SDLPANEL), screen(0) {
    
	ChangeSize();
    windowParent = parent;
	
	CreateScreen();
}

SDLScreen::~SDLScreen() {
    if (screen) {
        SDL_FreeSurface(screen);
    }
}

void SDLScreen::ChangeSize()
{
	// ensure the size of the wxPanel
	wxSize size(GB_SCREEN_W*SettingsGetWindowZoom(), GB_SCREEN_H*SettingsGetWindowZoom());
    
    SetMinSize(size);
    SetMaxSize(size);
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
	wxImage img = wxImage(screen->w, screen->h, static_cast<unsigned char *>(screen->pixels), true);
	
	int winZoom = SettingsGetWindowZoom();
	if (winZoom > 1)
		img.Rescale(GB_SCREEN_W*winZoom, GB_SCREEN_H*winZoom, wxIMAGE_QUALITY_NORMAL);
    wxBitmap bmp(img);
    
    // unlock the screen
    if (SDL_MUSTLOCK(screen)) {
        SDL_UnlockSurface(screen);
    }
    
    // paint the screen
    wxBufferedPaintDC dc(this, bmp);
	//wxAutoBufferedPaintDC dc(this);
	//dc.DrawBitmap(bmp, 0, 0, false);
}

void SDLScreen::onPreDraw()
{
	if ( SDL_MUSTLOCK(screen) )
    {
        if ( SDL_LockSurface(screen) < 0 )
		{
            return;
        }
    }
}

void SDLScreen::onPostDraw()
{
	if ( SDL_MUSTLOCK(screen) )
    {
        SDL_UnlockSurface(screen);
    }
}

void SDLScreen::onRefreshScreen()
{
	// refresh the panel
    Refresh(false);
}

void SDLScreen::CreateScreen() {
    if (!screen) {
        screen = SDL_CreateRGBSurface(SDL_SWSURFACE, GB_SCREEN_W, GB_SCREEN_H, 
                                      24, 0, 0, 0, 0);
		
		ChangePalette(SettingsGetGreenScale());
    }
}

void SDLScreen::ChangePalette(bool original)
{
	if (!screen)
		return;
	
	if (original)
	{
		colors[0] = SDL_MapRGB(screen->format,  16,  57, 16);
		colors[1] = SDL_MapRGB(screen->format,  49,  99, 49);
		colors[2] = SDL_MapRGB(screen->format, 140, 173, 16);
		colors[3] = SDL_MapRGB(screen->format, 156, 189, 16);
	}
	else {
		//Inicializar un array con los 4 colores posibles (negro, gris oscuro, gris claro, blanco)
		for (int i=0; i<4; i++)
			colors[i] = SDL_MapRGB(screen->format, i*85, i*85, i*85);
	}

}

void SDLScreen::onClear()
{
	SDL_FillRect( screen, NULL, 0 );
}

//idColor = 0, 1, 2, 3 = negro, gris oscuro, gris claro, blanco
void SDLScreen::onDrawPixel(int idColor, int x, int y)
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
