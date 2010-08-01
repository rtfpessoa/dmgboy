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

#include <iostream>
#include "wxSDLApp.h"
#include "Def.h"

IMPLEMENT_CLASS(SDLApp, wxApp)
IMPLEMENT_APP(SDLApp)

bool SDLApp::OnInit() {
    // create the SDLFrame
    frame = new SDLFrame;
    frame->SetClientSize(SCREEN_W, SCREEN_H);
    frame->Centre();
    frame->Show();
    
    // Our SDLFrame is the Top Window
    SetTopWindow(frame);
	
    // initialization should always succeed
    return true;
}

int SDLApp::OnRun() {
    // initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "unable to init SDL: " << SDL_GetError() << '\n';
        
        return -1;
    }
    
    // generate an initial idle event to start things
    //wxIdleEvent event;
    //event.SetEventObject(&frame->getPanel());
    //frame->getPanel().AddPendingEvent(event);
	
    // start the main loop
    return wxApp::OnRun();
}

int SDLApp::OnExit() {
	frame->Clean();
    // cleanup SDL
    SDL_Quit();
    
    // return the standard exit code
    return wxApp::OnExit();
}



