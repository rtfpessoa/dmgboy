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
#include "MainPanel.h"
#include "MainFrame.h"
#include "IDControls.h"
#include "../Def.h"
#include "../Settings.h"

inline void MainPanel::OnEraseBackground(wxEraseEvent &) { /* do nothing */ }

IMPLEMENT_CLASS(MainPanel, wxPanel)

BEGIN_EVENT_TABLE(MainPanel, wxPanel)
EVT_PAINT(MainPanel::OnPaint)
EVT_ERASE_BACKGROUND(MainPanel::OnEraseBackground)
END_EVENT_TABLE()

MainPanel::MainPanel(wxWindow *parent) : wxPanel(parent, ID_MAINPANEL), screen(0) {
    
	SetBackgroundStyle(wxBG_STYLE_CUSTOM);
	ChangeSize();
    windowParent = parent;
	
	CreateScreen();
	
	this->SetDropTarget(new DnDFile(parent));
}

MainPanel::~MainPanel() {
    if (screen) {
        SDL_FreeSurface(screen);
    }
}

void MainPanel::ChangeSize()
{
	// ensure the size of the wxPanel
	wxSize size(GB_SCREEN_W*SettingsGetWindowZoom(), GB_SCREEN_H*SettingsGetWindowZoom());
    
    SetMinSize(size);
    SetMaxSize(size);
}

void MainPanel::OnPaint(wxPaintEvent &) {
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
    wxAutoBufferedPaintDC dc(this);
	dc.DrawBitmap(bmp, 0, 0);
	// dc.DrawText(wxString("Pokemon"), 0, 0);
}

void MainPanel::OnPreDraw()
{
	if ( SDL_MUSTLOCK(screen) )
    {
        if ( SDL_LockSurface(screen) < 0 )
            return;
    }
}

void MainPanel::OnPostDraw()
{
	if ( SDL_MUSTLOCK(screen) )
        SDL_UnlockSurface(screen);
}

void MainPanel::OnRefreshScreen()
{
	// refresh the panel
    Refresh(false);
}

void MainPanel::CreateScreen() {
    if (!screen) {
        screen = SDL_CreateRGBSurface(SDL_SWSURFACE, GB_SCREEN_W, GB_SCREEN_H, 
                                      24, 0, 0, 0, 0);
		
		ChangePalette(SettingsGetGreenScale());
    }
}

void MainPanel::ChangePalette(bool original)
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

void MainPanel::OnClear()
{
	SDL_FillRect( screen, NULL, 0 );
}

//idColor = 0, 1, 2, 3 = negro, gris oscuro, gris claro, blanco
void MainPanel::OnDrawPixel(int idColor, int x, int y)
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

DnDFile::DnDFile(wxWindow * parent)
{
	this->parent = parent;
}

bool DnDFile::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames)
{
	MainFrame * frame = (MainFrame *)parent;
	frame->ChangeFile(filenames[0]);

	return true;
}