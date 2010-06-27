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
#include "wxSDLPanel.h"

/*******************************************************************************
 // SDLFrame Class
 *******************************************************************************/

class SDLFrame : public wxFrame {
    DECLARE_CLASS(SDLFrame)
    DECLARE_EVENT_TABLE()
    
private:
    SDLPanel *panel;
    
    /**
     * Called when exit from the file menu is selected.
     */
    inline void onFileExit(wxCommandEvent &) { Close(); }
    
    /**
     * Called when about from the help menu is selected.
     */
    void onHelpAbout(wxCommandEvent &);
    
public:
    /**
     * Creates a new SDLFrame.
     */
    SDLFrame();
    
    /**
     * Gets the SDLPanel within this SDLFrame.
     *
     * @return The SDLPanel.
     */
	
	inline SDLPanel &getPanel() { return *panel; }
};