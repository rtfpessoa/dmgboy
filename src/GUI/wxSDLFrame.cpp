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

#include "wxSDLFrame.h"
#include "wxIDControls.h"

IMPLEMENT_CLASS(SDLFrame, wxFrame)

BEGIN_EVENT_TABLE(SDLFrame, wxFrame)
EVT_MENU(wxID_EXIT, SDLFrame::onFileExit)
EVT_MENU(IDM_LOAD, SDLFrame::onFileLoad)
END_EVENT_TABLE()

SDLFrame::SDLFrame() {
    // Create the SDLFrame
    Create(0, IDF_FRAME, wxT("gbpablog"), wxDefaultPosition,
           wxDefaultSize, wxCAPTION | wxSYSTEM_MENU | 
           wxMINIMIZE_BOX | wxCLOSE_BOX);
	
    // create the main menubar
    wxMenuBar *mb = new wxMenuBar;
    
    // create the file menu
    wxMenu *fileMenu = new wxMenu;
    fileMenu->Append(wxID_EXIT, wxT("E&xit"));
	fileMenu->Append(IDM_LOAD, wxT("&Load"));
    
    // add the file menu to the menu bar
    mb->Append(fileMenu, wxT("&File"));
	
	// create the emulation menu
    wxMenu *emulationMenu = new wxMenu;
    emulationMenu->Append(IDM_START, wxT("&Start"));
	emulationMenu->Append(IDM_PAUSE, wxT("&Pause"));
	emulationMenu->Append(IDM_STOP, wxT("S&top"));
    
    // add the file menu to the menu bar
    mb->Append(emulationMenu, wxT("&Emulation"));
    
    // create the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(wxID_ABOUT, wxT("&About"));
    
    // add the help menu to the menu bar
    mb->Append(helpMenu, wxT("&Help"));
    
    // add the menu bar to the SDLFrame
    SetMenuBar(mb);
    
    // create the SDLPanel
    panel = new SDLScreen(this);
	
	
	Cartridge * c = new Cartridge("/Users/pablo/Documents/Programacion/GB/ROMS/MBC1/Super Mario Land (JUE) (v1.1).gb");
	
    Video * v = new Video(panel);
	
	cpu = new CPU(v, c);
}

void SDLFrame::onFileLoad(wxCommandEvent &) {
    wxMessageBox(wxT("wx-sdl tutorial\nCopyright (C) 2005,2007 John Ratliff"),
                 wxT("about wx-sdl tutorial"), wxOK | wxICON_INFORMATION);
}

