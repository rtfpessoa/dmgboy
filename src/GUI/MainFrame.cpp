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
#include <wx/wfstream.h>
#include <wx/zipstrm.h>
#include "MainFrame.h"
#include "AboutDialog.h"
#include "IDControls.h"
#include "../Settings.h"
#include "../Pad.h"
#include "Xpm/open.xpm"
#include "Xpm/play.xpm"
#include "Xpm/pause.xpm"
#include "Xpm/stop.xpm"
#include "Xpm/gb16.xpm"

IMPLEMENT_CLASS(MainFrame, wxFrame)

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_MENU(wxID_EXIT, MainFrame::onFileExit)
EVT_MENU(wxID_OPEN, MainFrame::onFileOpen)
EVT_MENU(wxID_PREFERENCES, MainFrame::onSettings)
EVT_MENU(wxID_ABOUT, MainFrame::onAbout)
EVT_MENU(ID_START, MainFrame::onPlay)
EVT_MENU(ID_PAUSE, MainFrame::onPause)
EVT_MENU(ID_STOP, MainFrame::onStop)
EVT_UPDATE_UI( ID_START, MainFrame::onPlayUpdate )
EVT_UPDATE_UI( ID_PAUSE, MainFrame::onPauseUpdate )
EVT_UPDATE_UI( ID_STOP, MainFrame::onStopUpdate )
EVT_IDLE(MainFrame::onIdle)
END_EVENT_TABLE()

MainFrame::MainFrame()
{
    // Create the MainFrame
    Create(0, ID_MAINFRAME, wxT("gbpablog"), wxDefaultPosition,
           wxDefaultSize, wxCAPTION | wxSYSTEM_MENU |
           wxMINIMIZE_BOX | wxCLOSE_BOX);

	SetIcon(wxIcon(gb16_xpm));

    createMenuBar();
	createToolBar();

	settingsDialog = new SettingsDialog(this);
	settingsDialog->CentreOnScreen();
	settingsDialog->LoadFromFile();
	SettingsSetNewValues(settingsDialog->settings);
	PadSetKeys(SettingsGetInput());	

    // create the SDLPanel
    panel = new MainPanel(this);


    video = new Video(panel);
	cpu = new CPU(video);

	cartridge = NULL;

	emuState = NotStartedYet;

	SetClientSize(GB_SCREEN_W*SettingsGetWindowZoom(), GB_SCREEN_H*SettingsGetWindowZoom());
}

MainFrame::~MainFrame()
{
	Clean();
}

void MainFrame::createMenuBar()
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
	emulationMenu->Append(wxID_PREFERENCES, wxT("&Settings\tCtrl+E"));
    emulationMenu->Append(ID_START, wxT("&Start\tCtrl+S"));
	emulationMenu->Append(ID_PAUSE, wxT("&Pause\tCtrl+P"));
	emulationMenu->Append(ID_STOP, wxT("S&top\tCtrl+T"));

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

void MainFrame::createToolBar()
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

void MainFrame::onFileOpen(wxCommandEvent &) {
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
		wxString fileLower = fileName.Lower();
		if (fileLower.EndsWith(wxT(".zip")))
		{
			isZip = true;
			LoadZip(fileName, &buffer, &size);
			if ((buffer == NULL) || (size == 0))
				return;
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

/*
 * Carga un fichero comprimido con zip y busca una rom de gameboy (un fichero con extension gb).
 * Si existe mas de una rom solo carga la primera. Si se ha encontrado, la rom se devuelve en un buffer
 * junto con su tamaño, sino las variables se dejan intactas
 */
void MainFrame::LoadZip(wxString zipPath, BYTE ** buffer, unsigned long * size)
{
	wxString fileInZip, fileLower;
	wxZipEntry* entry;
	wxFFileInputStream in(zipPath);
	wxZipInputStream zip(in);
	while (entry = zip.GetNextEntry())
	{
		fileInZip = entry->GetName();
		
		fileLower = fileInZip.Lower();
		if (fileLower.EndsWith(wxT(".gb")))
		{
			*size = zip.GetSize();
			*buffer = new BYTE[*size];
			zip.Read(*buffer, *size);
			delete entry;
			return;
		}
		else
		{
			delete entry;
			continue;
		}
	}
	
	// Archivo no encontrado
	wxMessageBox(wxT("GameBoy rom not found in the file:\n")+zipPath, wxT("Error"));
	return;
}

void MainFrame::onFileExit(wxCommandEvent &)
{
	Close();
}

void MainFrame::Clean()
{
	emuState = Stopped;
	delete cpu;
	delete video;
	if (cartridge)
		delete cartridge;
	if (settingsDialog)
		settingsDialog->Destroy();
}

/*
 * Abre un dialogo de configuracion. Cuando se cierra se encarga de aplicar ciertos cambios a la emulacion
 */
void MainFrame::onSettings(wxCommandEvent &)
{
	enumEmuStates lastState = emuState;
	if (emuState == Playing)
		emuState = Paused;


    if (settingsDialog->ShowModal() == wxID_OK)
	{
		SettingsSetNewValues(settingsDialog->settings);
		panel->ChangePalette(SettingsGetGreenScale());
		panel->ChangeSize();
		SetClientSize(GB_SCREEN_W*SettingsGetWindowZoom(), GB_SCREEN_H*SettingsGetWindowZoom());
		PadSetKeys(SettingsGetInput());
	}

	emuState = lastState;
}

void MainFrame::onAbout(wxCommandEvent &)
{
	AboutDialog(this);
}

void MainFrame::onPlay(wxCommandEvent &)
{
	emuState = Playing;
}

void MainFrame::onPause(wxCommandEvent &)
{
	if (emuState == Playing)
		emuState = Paused;
	else if (emuState == Paused)
		emuState = Playing;
}

void MainFrame::onStop(wxCommandEvent &)
{
	cpu->Reset();
	panel->onRefreshScreen();
	emuState = Stopped;
}

void MainFrame::onPlayUpdate(wxUpdateUIEvent& event)
{
	if ((emuState == NotStartedYet) || (emuState == Playing)){
		event.Enable(false);
	}
	else {
		event.Enable(true);
	}

}

void MainFrame::onPauseUpdate(wxUpdateUIEvent& event)
{
	if ((emuState == NotStartedYet) || (emuState == Stopped)){
		event.Enable(false);
	}
	else {
		event.Enable(true);
	}

}

void MainFrame::onStopUpdate(wxUpdateUIEvent& event)
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
void MainFrame::onIdle(wxIdleEvent &event)
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
