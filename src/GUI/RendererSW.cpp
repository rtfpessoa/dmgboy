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
#include "IDControls.h"
#include "../Def.h"
#include "../Settings.h"
#include "RendererSW.h"

inline void RendererSW::OnEraseBackground(wxEraseEvent &) { /* do nothing */ }

IMPLEMENT_CLASS(RendererSW, wxPanel)

BEGIN_EVENT_TABLE(RendererSW, wxPanel)
EVT_PAINT(RendererSW::OnPaint)
EVT_ERASE_BACKGROUND(RendererSW::OnEraseBackground)
END_EVENT_TABLE()

RendererSW::RendererSW(wxWindow *parent):
	wxPanel(parent, ID_MAINPANEL)
{
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
	SetWinRenderer(parent, this);
}

RendererSW::~RendererSW() {
    
}

void RendererSW::OnPaint(wxPaintEvent &) {
    
	wxImage img = wxImage(GB_SCREEN_W, GB_SCREEN_H, frontBuffer, true);
	
	int winZoom = SettingsGetWindowZoom();
	if (winZoom > 1)
		img.Rescale(GB_SCREEN_W*winZoom, GB_SCREEN_H*winZoom, wxIMAGE_QUALITY_NORMAL);
    wxBitmap bmp(img);
    
    // paint the screen
    wxAutoBufferedPaintDC dc(this);
	dc.DrawBitmap(bmp, 0, 0);
		
	// dc.DrawText(wxString("Pokemon"), 0, 0);
}

