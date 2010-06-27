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
#include "wxSDLFrame.h"
#undef main

/*******************************************************************************
 // SDLApp Class
 *******************************************************************************/

class SDLApp : public wxApp {
    DECLARE_CLASS(SDLApp)
    
private:
    SDLFrame *frame;
    
public:
    /**
     * Called to initialize this SDLApp.
     *
     * @return true if initialization succeeded; false otherwise.
     */
    bool OnInit();
    
    /**
     * Called to run this SDLApp.
     *
     * @return The status code (0 if good, non-0 if bad).
     */
    int OnRun();
    
    /**
     * Called when this SDLApp is ready to exit.
     *
     * @return The exit code.
     */
    int OnExit();
};
