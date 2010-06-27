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

#include <wx/wx.h>
#include "SDL.h"

/*******************************************************************************
 * SDLPanel Class
 *******************************************************************************/

class SDLPanel : public wxPanel {
    DECLARE_CLASS(SDLPanel)
    DECLARE_EVENT_TABLE()
    
private:
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
    void onIdle(wxIdleEvent &);
    
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
    SDLPanel(wxWindow *parent);
    
    /**
     * Destructor for the SDLPanel.
     */
    ~SDLPanel();
};
