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

#include <string>
#include <wx/filesys.h>
#include <wx/fs_arc.h>
#include "wxSDLFrame.h"
#include "wxIDControls.h"
#include "wxSettings.h"
#include "open.xpm"
#include "play.xpm"
#include "pause.xpm"
#include "stop.xpm"
#include "gb16.xpm"
#include "../Settings.h"

IMPLEMENT_CLASS(SDLFrame, wxFrame)

BEGIN_EVENT_TABLE(SDLFrame, wxFrame)
EVT_MENU(wxID_EXIT, SDLFrame::onFileExit)
EVT_MENU(wxID_OPEN, SDLFrame::onFileOpen)
EVT_MENU(wxID_PREFERENCES, SDLFrame::onSettings)
EVT_MENU(ID_START, SDLFrame::onPlay)
EVT_MENU(ID_PAUSE, SDLFrame::onPause)
EVT_MENU(ID_STOP, SDLFrame::onStop)
EVT_UPDATE_UI( ID_START, SDLFrame::onPlayUpdate )
EVT_UPDATE_UI( ID_PAUSE, SDLFrame::onPauseUpdate )
EVT_UPDATE_UI( ID_STOP, SDLFrame::onStopUpdate )
EVT_IDLE(SDLFrame::onIdle)
END_EVENT_TABLE()

SDLFrame::SDLFrame()
{
    // Create the SDLFrame
    Create(0, ID_SDLFRAME, wxT("gbpablog"), wxDefaultPosition,
           wxDefaultSize, wxCAPTION | wxSYSTEM_MENU |
           wxMINIMIZE_BOX | wxCLOSE_BOX);

	SetIcon(wxIcon(gb16_xpm));

    createMenuBar();
	createToolBar();

    // create the SDLPanel
    panel = new SDLScreen(this);


    video = new Video(panel);
	cpu = new CPU(video);

	cartridge = NULL;

	emuState = NotStartedYet;
	
	SetClientSize(GB_SCREEN_W*SettingsGetWindowZoom(), GB_SCREEN_H*SettingsGetWindowZoom());
}

SDLFrame::~SDLFrame()
{
	Clean();
}

void SDLFrame::createMenuBar()
{
	// create the main menubar
    mb = new wxMenuBar();

    // create the file menu
    wxMenu *fileMenu = new wxMenu;
	fileMenu->Append(wxID_OPEN, wxT("&Open\tCtrl+O"));
	fileMenu->Append(wxID_EXIT, wxT("E&xit"));

    // add the file menu to the menu bar
    mb->Append(fileMenu, wxT("&File"));

	// create the emulation menu
    wxMenu *emulationMenu = new wxMenu;
	emulationMenu->Append(wxID_PREFERENCES, wxT("&Settings"));
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
}

void SDLFrame::createToolBar()
{

	toolBar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL|wxNO_BORDER);
	wxBitmap bmpOpen(open_xpm);
	toolBar->AddTool(wxID_OPEN, bmpOpen, wxT("Open"));

	toolBar->AddSeparator();

	wxBitmap bmpPlay(play_xpm);
	toolBar->AddTool(ID_START, bmpPlay, wxT("Start"));

	wxBitmap bmpPause(pause_xpm);
	toolBar->AddTool(ID_PAUSE, bmpPause, wxT("Pause"));

	wxBitmap bmpStop(stop_xpm);
	toolBar->AddTool(ID_STOP, bmpStop, wxT("Stop"));
	
	toolBar->Realize();
	SetToolBar(toolBar);
}

void SDLFrame::onFileOpen(wxCommandEvent &) {
	BYTE * buffer = NULL;
	unsigned long size = 0;
	bool isZip = false;
	
	wxFileDialog* OpenDialog = new wxFileDialog(this, wxT("Choose a gameboy rom to open"), wxEmptyString, wxEmptyString,
												wxT("Gameboy roms (*.gb; *.zip)|*.gb;*.zip"),
												wxFD_OPEN, wxDefaultPosition);

	// Creates a "open file" dialog
	if (OpenDialog->ShowModal() == wxID_OK) // if the user click "Open" instead of "Cancel"
	{
		wxString fileName = OpenDialog->GetPath();
		if (fileName.EndsWith(".zip"))
		{
			isZip = true;
			wxFileSystem::AddHandler(new wxArchiveFSHandler);
			wxFileSystem fs;
			wxString filter = wxT(fileName+"#zip:*.gb");
			wxString fileInZip;
			fileInZip = fs.FindFirst(filter, wxFILE);
			if (!fileInZip.IsEmpty())
			{
				wxFSFile * file = fs.OpenFile(fileInZip);
				if(file)
				{
					wxInputStream * stream = file->GetStream();
					size = stream->GetSize();
					buffer = new BYTE[size];
					stream->Read(buffer, size);
					wxDELETE(file);
				}
				else {
					wxMessageBox("The zip seems corrupt", "Error");
					return;
				}

			}
			else {
				wxMessageBox("GameBoy rom not found in the file:\n"+fileName, "Error");
				return;
			}
		}
		
		cpu->Reset();
		if (cartridge)
			delete cartridge;

		if (isZip) {
			cartridge = new Cartridge(buffer, size);
		}else {
			cartridge = new Cartridge(std::string(fileName.mb_str()));
		}

		
		cpu->LoadCartridge(cartridge);
		emuState = Playing;
	}

	// Clean up after ourselves
	OpenDialog->Destroy();
}

void SDLFrame::onFileExit(wxCommandEvent &)
{
	Close();
}

void SDLFrame::Clean()
{
	emuState = Stopped;
	delete cpu;
	delete video;
	if (cartridge)
		delete cartridge;
}

void SDLFrame::onSettings(wxCommandEvent &)
{
	enumEmuStates lastState = emuState;
	if (emuState == Playing)
		emuState = Paused;

	SettingsDialog dialog(this);
    if (dialog.ShowModal() == wxID_OK)
	{
		SettingsSetNewValues(dialog.settings);
		panel->ChangePalette(SettingsGetGreenscale());
		panel->ChangeSize();
		SetClientSize(GB_SCREEN_W*SettingsGetWindowZoom(), GB_SCREEN_H*SettingsGetWindowZoom());
	}
	
	dialog.Destroy();
	
	emuState = lastState;
}

void SDLFrame::onPlay(wxCommandEvent &)
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

void SDLFrame::onPlayUpdate(wxUpdateUIEvent& event)
{
	if ((emuState == NotStartedYet) || (emuState == Playing)){
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

/**
 * Esta funcion se encarga de dar la orden de hacer aproximadamente los calculos
 * de la emulacion de un frame, pero lo hara cuando la interfaz haya terminado de
 * procesar todos los eventos. Se quiere que pinte los frames cada 16.6 milisegundos
 * (60 frames por segundo). Para ello se tiene en cuenta el tiempo que hace que se
 * llamo por ultima vez a esta funcion y el tiempo que ha tardado la emulacion en
 * calcularse.
 */
void SDLFrame::onIdle(wxIdleEvent &event)
{
	long duration = 16;

	long lastDuration = swFrame.Time();
	swFrame.Start();

	long delay = 0;

	if (lastDuration > duration)
		delay = lastDuration - duration;

    if (emuState == Playing)
	{
		swExecution.Start();

		cpu->UpdatePad();

		// Valor obtenido comparandolo con otros emuladores
		cpu->Run(113000);

		delay += swExecution.Time();
	}

	if (delay > duration)
		delay = duration;

	event.RequestMore(true);
    wxMilliSleep(duration-delay);
}
