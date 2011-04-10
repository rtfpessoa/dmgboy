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

BYTE palettes[][4][3] =		{
								{
									{ 16,  57,  16},
									{ 49,  99,  49},
									{140, 173,  16},
									{156, 189,  16}
								},
								{
									{  0,   0,   0},
									{ 85,  85,  85},
									{170, 170, 170},
									{255, 255, 255}
								}
							};


MainPanel::MainPanel(wxWindow *parent) : wxPanel(parent, ID_MAINPANEL) {
    
	imgBuf = NULL;
	SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    windowParent = parent;
	
	CreateScreen();
	ChangeSize();
	
	this->SetDropTarget(new DnDFile(parent));
}

MainPanel::~MainPanel() {
    delete[] imgBuf;
}

void MainPanel::CreateScreen() {
	imgBuf = new BYTE[GB_SCREEN_W*GB_SCREEN_H*3];
	OnClear();
	ChangePalette(SettingsGetGreenScale());
}

void MainPanel::ChangeSize()
{
	int zoom = SettingsGetWindowZoom();
    
	wxSize size(GB_SCREEN_W*zoom, GB_SCREEN_H*zoom);
    SetMinSize(size);
    SetMaxSize(size);
}

void MainPanel::OnPaint(wxPaintEvent &) {
    
	wxImage img = wxImage(GB_SCREEN_W, GB_SCREEN_H, imgBuf, true);
	
	int winZoom = SettingsGetWindowZoom();
	if (winZoom > 1)
		img.Rescale(GB_SCREEN_W*winZoom, GB_SCREEN_H*winZoom, wxIMAGE_QUALITY_NORMAL);
    wxBitmap bmp(img);
    
    // paint the screen
    wxAutoBufferedPaintDC dc(this);
	dc.DrawBitmap(bmp, 0, 0);
		
	// dc.DrawText(wxString("Pokemon"), 0, 0);
}

void MainPanel::OnPreDraw()
{
	
}

void MainPanel::OnPostDraw()
{
	
}

void MainPanel::OnRefreshScreen()
{
	// refresh the panel
    Refresh(false);
	Update();
}

void MainPanel::ChangePalette(bool original)
{
	if (original)
		selPalette = 0;
	else
		selPalette = 1;
}

void MainPanel::OnClear()
{
	int size = GB_SCREEN_W*GB_SCREEN_H*3;
	memset(imgBuf, 0, size);
}

void MainPanel::OnDrawPixel(int idColor, int x, int y)
{
	/*
	int zoom = SettingsGetWindowZoom();
	
	void * ptrPalette = &palettes[selPalette][idColor][0];
	unsigned long offsetBuf = 0;
	int sizeLine = GB_SCREEN_W * 3 * zoom;
	int offsetX = x * 3 * zoom;
	int offsetY = 0;
	
	int i, j;
	
	for (i=0; i<zoom; i++)
	{
		offsetY = (y * zoom + i) * sizeLine;
		offsetBuf = offsetY + offsetX;
		
		for (j=0; j<zoom; j++)
		{
			memcpy(&imgBuf[offsetBuf], ptrPalette, 3);
			offsetBuf += 3;
		}
	}
	 */

	BYTE colorR = palettes[selPalette][idColor][0];
	BYTE colorG = palettes[selPalette][idColor][1];
	BYTE colorB = palettes[selPalette][idColor][2];
	
	int sizeLine = GB_SCREEN_W * 3;
	int offsetX = x * 3;
	int offsetY = y * sizeLine;
	int offsetBuf = offsetY + offsetX;
	
	imgBuf[offsetBuf + 0] = colorR;
	imgBuf[offsetBuf + 1] = colorG;
	imgBuf[offsetBuf + 2] = colorB;
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