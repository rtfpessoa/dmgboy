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

#ifndef __MAINPANEL_H__
#define __MAINPANEL_H__

#include <wx/wx.h>
#include "SDL.h"


/*******************************************************************************
 * MainPanel Class
 *******************************************************************************/

class MainPanel : public wxPanel {
    DECLARE_CLASS(MainPanel)
    DECLARE_EVENT_TABLE()

private:
	wxWindow * windowParent;
	Uint32 colors[4];
    SDL_Surface *screen;
	
    void OnPaint(wxPaintEvent &);
    void OnEraseBackground(wxEraseEvent &);
    
    /**
     * Creates the SDL_Surface used by this MainPanel.
     */
    void CreateScreen();
    
public:
    MainPanel(wxWindow *parent);
    ~MainPanel();
	
	void ChangeSize();
	void ChangePalette(bool original);
	void OnPreDraw();
	void OnPostDraw();
	void OnDrawPixel(int idColor, int x, int y);
	void OnRefreshScreen();
	void OnClear();
};

#endif
