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

#ifndef __WXSDLSCREEN_H__
#define __WXSDLSCREEN_H__

#include <wx/wx.h>
#include "SDL.h"


/*******************************************************************************
 * SDLPanel Class
 *******************************************************************************/

class SDLScreen : public wxPanel {
    DECLARE_CLASS(SDLScreen)
    DECLARE_EVENT_TABLE()

private:
	wxWindow * windowParent;
	Uint32 colors[4];
    SDL_Surface *screen;
	
    /**
     * Called to paint the panel.
     */
    void onPaint(wxPaintEvent &);
    
    /**
     * Called to erase the background.
     */
    void onEraseBackground(wxEraseEvent &);
    
    /**
     * Called to update the panel in idle time.
     */
    void onIdle(wxIdleEvent &event);
    
    /**
     * Creates the SDL_Surface used by this SDLPanel.
     */
    void createScreen();
    
public:
    /**
     * Creates a new SDLPanel.
     *
     * @param parent The wxWindow parent.
     */
    SDLScreen(wxWindow *parent);
    
    /**
     * Destructor for the SDLPanel.
     */
    ~SDLScreen();
	
	void onVideoPreDraw();
	
	void onVideoPostDraw();
	
	void onVideoDrawPixel(int idColor, int x, int y);
	
	void onVideoRefreshScreen();
};

#endif
