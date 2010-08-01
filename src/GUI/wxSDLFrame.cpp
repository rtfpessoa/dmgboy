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
#include "open.xpm"
#include "play.xpm"
#include "pause.xpm"
#include "stop.xpm"

IMPLEMENT_CLASS(SDLFrame, wxFrame)

BEGIN_EVENT_TABLE(SDLFrame, wxFrame)
EVT_MENU(wxID_EXIT, SDLFrame::onFileExit)
EVT_MENU(wxID_OPEN, SDLFrame::onFileOpen)
EVT_MENU(ID_START, SDLFrame::onStart)
EVT_MENU(ID_PAUSE, SDLFrame::onPause)
EVT_MENU(ID_STOP, SDLFrame::onStop)
EVT_UPDATE_UI( ID_START, SDLFrame::onStartUpdate )
EVT_UPDATE_UI( ID_PAUSE, SDLFrame::onPauseUpdate )
EVT_UPDATE_UI( ID_STOP, SDLFrame::onStopUpdate )
EVT_TIMER(ID_TIMER, SDLFrame::onProgressTimer)
END_EVENT_TABLE()

SDLFrame::SDLFrame() {
    // Create the SDLFrame
    Create(0, IDF_FRAME, wxT("gbpablog"), wxDefaultPosition,
           wxDefaultSize, wxCAPTION | wxSYSTEM_MENU | 
           wxMINIMIZE_BOX | wxCLOSE_BOX);
	
    createMenuBar();
	createToolBar();
	
    // create the SDLPanel
    panel = new SDLScreen(this);
	
	
    video = new Video(panel);
	cpu = new CPU(video);
	
	m_timer = new wxTimer(this, ID_TIMER);
	m_timer->Start(17);
	
	emuState = NotStartedYet;
	
}

void SDLFrame::createMenuBar()
{
	// create the main menubar
    mb = new wxMenuBar();
    
    // create the file menu
    wxMenu *fileMenu = new wxMenu;
    fileMenu->Append(wxID_EXIT, wxT("E&xit"));
	fileMenu->Append(wxID_OPEN, wxT("&Open"));
    
    // add the file menu to the menu bar
    mb->Append(fileMenu, wxT("&File"));
	
	// create the emulation menu
    wxMenu *emulationMenu = new wxMenu;
    emulationMenu->Append(ID_START, wxT("&Start"));
	emulationMenu->Append(ID_PAUSE, wxT("&Pause"));
	emulationMenu->Append(ID_STOP, wxT("S&top"));
    
    // add the file menu to the menu bar
    mb->Append(emulationMenu, wxT("&Emulation"));
    
    // create the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(wxID_ABOUT, wxT("&About"));
    
    // add the help menu to the menu bar
    mb->Append(helpMenu, wxT("&Help"));
    
    // add the menu bar to the SDLFrame
    SetMenuBar(mb);
    
	/*mb->Enable(ID_START, false);
	mb->Enable(ID_PAUSE, false);
	mb->Enable(ID_STOP, false);*/
}

void SDLFrame::createToolBar()
{
	
	toolBar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL|wxNO_BORDER);
	wxBitmap bmpOpen(open_xpm);
	toolBar->AddTool(wxID_OPEN, bmpOpen, wxT("Open"));
	toolBar->AddSeparator();
	wxBitmap bmpPlay(play_xpm);
	toolBar->AddTool(ID_START, bmpPlay, wxT("Start"));
	//toolBar->EnableTool(ID_START, false);
	wxBitmap bmpPause(pause_xpm);
	toolBar->AddTool(ID_PAUSE, bmpPause, wxT("Pause"));
	//toolBar->EnableTool(ID_PAUSE, false);
	wxBitmap bmpStop(stop_xpm);
	toolBar->AddTool(ID_STOP, bmpStop, wxT("Stop"));
	//toolBar->EnableTool(ID_STOP, false);
	toolBar->Realize();
	SetToolBar(toolBar);
}

void SDLFrame::onFileOpen(wxCommandEvent &) {
	wxFileDialog* OpenDialog = new wxFileDialog(this, _("Choose a gameboy rom to open"), wxEmptyString, wxEmptyString, 
												_("Gameboy roms (*.gb)|*.gb"),
												wxFD_OPEN, wxDefaultPosition);
	
	// Creates a "open file" dialog with 4 file types
	if (OpenDialog->ShowModal() == wxID_OK) // if the user click "Open" instead of "Cancel"
	{
		cpu->Reset();
		if (cartridge)
			delete cartridge;
		cartridge = new Cartridge(OpenDialog->GetPath().c_str());
		cpu->LoadCartridge(cartridge);
		emuState = Playing;
	}
	
	// Clean up after ourselves
	OpenDialog->Destroy();
}

void SDLFrame::onFileExit(wxCommandEvent &)
{
	Clean();
}

void SDLFrame::Clean()
{
	emuState = Stopped;
	m_timer->Stop();
	delete cpu;
	delete video;
	if (cartridge)
		delete cartridge;
	
	Close();
}

void SDLFrame::onStart(wxCommandEvent &)
{
	emuState = Playing;
}

void SDLFrame::onPause(wxCommandEvent &)
{
	if (emuState == Playing)
		emuState = Paused;
	else if (emuState == Paused)
		emuState = Playing;
}

void SDLFrame::onStop(wxCommandEvent &)
{
	cpu->Reset();
	panel->onRefreshScreen();
	emuState = Stopped;
}

void SDLFrame::onStartUpdate(wxUpdateUIEvent& event)
{
	if (emuState == NotStartedYet) {
		event.Enable(false);
	}
	else {
		event.Enable(true);
	}
	
}

void SDLFrame::onPauseUpdate(wxUpdateUIEvent& event)
{
	if ((emuState == NotStartedYet) || (emuState == Stopped)){
		event.Enable(false);
	}
	else {
		event.Enable(true);
	}
	
}

void SDLFrame::onStopUpdate(wxUpdateUIEvent& event)
{
	if ((emuState == Stopped)||(emuState == NotStartedYet)) {
		event.Enable(false);
	}
	else {
		event.Enable(true);
	}

}

void SDLFrame::onProgressTimer(wxTimerEvent& event)
{
	if (emuState == Playing)
	{
		cpu->UpdatePad();
		
		cpu->Run(100000);
	}
	
}
