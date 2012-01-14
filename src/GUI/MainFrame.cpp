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
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include "MainFrame.h"
#include "AboutDialog.h"
#include "SettingsDialog.h"
#include "IDControls.h"
#include "../Settings.h"
#include "../GBException.h"
#include "Xpm/open.xpm"
#include "Xpm/play.xpm"
#include "Xpm/pause.xpm"
#include "Xpm/stop.xpm"
#include "Xpm/recent.xpm"
#include "Xpm/gb16.xpm"
#include "Xpm/gb32.xpm"
#include "RendererOGL.h"
#include "RendererSW.h"
#include "EmulationThread.h"

using namespace std;

IMPLEMENT_CLASS(MainFrame, wxFrame)

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_MENU(wxID_EXIT, MainFrame::OnFileExit)
EVT_MENU(wxID_OPEN, MainFrame::OnFileOpen)
EVT_MENU(ID_OPEN_RECENT, MainFrame::OnRecent)
EVT_MENU_RANGE(ID_RECENT0, ID_RECENT9, MainFrame::OnRecentItem)
EVT_MENU(ID_CLEAR_RECENT, MainFrame::OnClearRecent)
EVT_MENU_RANGE(ID_LOADSTATE0, ID_LOADSTATE9, MainFrame::OnLoadState)
EVT_MENU_RANGE(ID_SAVESTATE0, ID_SAVESTATE9, MainFrame::OnSaveState)
EVT_MENU(wxID_PREFERENCES, MainFrame::OnSettings)
EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
EVT_MENU(ID_START, MainFrame::OnPlay)
EVT_MENU(ID_PAUSE, MainFrame::OnPause)
EVT_MENU(ID_STOP, MainFrame::OnStop)
EVT_MENU(ID_FULLSCREEN, MainFrame::OnFullScreen)
EVT_UPDATE_UI( ID_START, MainFrame::OnPlayUpdateUI )
EVT_UPDATE_UI( ID_PAUSE, MainFrame::OnPauseUpdateUI )
EVT_UPDATE_UI( ID_STOP, MainFrame::OnStopUpdateUI )
EVT_UPDATE_UI( ID_FULLSCREEN, MainFrame::OnFullScreenUpdateUI )
EVT_UPDATE_UI_RANGE(ID_LOADSTATE0, ID_LOADSTATE9, MainFrame::OnLoadStateUpdateUI)
EVT_UPDATE_UI_RANGE(ID_SAVESTATE0, ID_SAVESTATE9, MainFrame::OnSaveStateUpdateUI)
EVT_LEFT_DCLICK(MainFrame::OnDoubleClick)
EVT_COMMAND(wxID_ANY, wxEVT_RENDERER_REFRESHSCREEN, MainFrame::OnRefreshScreen)
EVT_TIMER(ID_TIMER, MainFrame::OnTimer)
EVT_CLOSE(MainFrame::OnClose)
END_EVENT_TABLE()

MainFrame::MainFrame(wxString fileName)
{
    // Create the MainFrame
#ifdef __WXGTK__
    // En linux parece ser que no permitir modificar el tamaño del frame al usuario
    // afecta a los posteriores redimensionados por codigo
    this->Create(0, ID_MAINFRAME, wxT("gbpablog"), wxDefaultPosition,
           wxDefaultSize, wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxRESIZE_BORDER | wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX);
#else
    this->Create(0, ID_MAINFRAME, wxT("gbpablog"), wxDefaultPosition,
           wxDefaultSize, wxCAPTION | wxSYSTEM_MENU |
           wxMINIMIZE_BOX | wxCLOSE_BOX | wxCLIP_CHILDREN);
#endif
	wxIconBundle * icons = new wxIconBundle(wxIcon(gb16_xpm));
	icons->AddIcon(wxIcon(gb32_xpm));
	this->SetIcons(*icons);

    this->CreateMenuBar();
	this->CreateToolBar();
	
	numRecentFiles = 0;

	settingsDialog = new SettingsDialog(this);
	settingsDialog->CentreOnScreen();
	settingsDialog->LoadFromFile();
	SettingsSetNewValues(settingsDialog->settings);
	this->CreateRecentMenu(SettingsGetRecentRoms());
	
    // create the emulation
    emulation = new EmulationThread();
    
    wxThreadError err = emulation->Create();
    if (err != wxTHREAD_NO_ERROR)
        wxMessageBox( _("Couldn't create thread!") );
    
    err = emulation->Run();
    if (err != wxTHREAD_NO_ERROR)
        wxMessageBox( _("Couldn't run thread!") );
    
    fullScreen = false;
    renderer = NULL;
    ChangeRenderer();
    
	if (fileName != wxT(""))
		ChangeFile(fileName);
		
    timer = new wxTimer(this, ID_TIMER);
	timer->Start(16);
}

MainFrame::~MainFrame()
{
	// No hacer nada aqui mejor hacerlo todo
    // en el OnClose
}

void MainFrame::CreateMenuBar()
{
	// create the main menubar
    mb = new wxMenuBar();

    // create the file menu
    wxMenu *fileMenu = new wxMenu;
	fileMenu->Append(wxID_OPEN, wxT("&Open\tCtrl+O"));
	
	recentMenuFile = new wxMenu;
	recentMenuFile->AppendSeparator();
	recentMenuFile->Append(ID_CLEAR_RECENT, wxT("Clear recent roms"));
	fileMenu->AppendSubMenu(recentMenuFile, wxT("Open Recent"));
	
	// Se crea un wxMenu que se tratará exactamente igual que a recentMenuFile
	// para poder tener uno en el menuBar y otro como popUp
	recentMenuPopup = new wxMenu;
	recentMenuPopup->AppendSeparator();
	recentMenuPopup->Append(ID_CLEAR_RECENT, wxT("Clear recent roms"));
	
	wxMenu * loadMenuFile = new wxMenu;
	wxMenu * saveMenuFile = new wxMenu;
	wxString slotMenu;
	for (int i=1; i<11; i++)
	{
		int id = i % 10;
		slotMenu = wxT("");
		slotMenu << wxT("Slot ") << id << wxT("\tCtrl+Alt+") << id;
		loadMenuFile->Append(ID_LOADSTATE0+id, slotMenu);
		slotMenu = wxT("");
		slotMenu << wxT("Slot ") << id << wxT("\tCtrl+") << id;
		saveMenuFile->Append(ID_SAVESTATE0+id, slotMenu);
	}
	fileMenu->AppendSubMenu(loadMenuFile, wxT("Load State"));
	fileMenu->AppendSubMenu(saveMenuFile, wxT("Save State"));
	
	fileMenu->Append(wxID_EXIT, wxT("E&xit"));

    // add the file menu to the menu bar
    mb->Append(fileMenu, wxT("&File"));

	// create the emulation menu
    wxMenu *emulationMenu = new wxMenu;
	emulationMenu->Append(wxID_PREFERENCES, wxT("&Settings\tCtrl+E"));
    emulationMenu->Append(ID_START, wxT("&Start\tCtrl+S"));
	emulationMenu->Append(ID_PAUSE, wxT("&Pause\tCtrl+P"));
	emulationMenu->Append(ID_STOP, wxT("S&top\tCtrl+T"));
    emulationMenu->Append(ID_FULLSCREEN, wxT("&FullScreen\tCtrl+F"));

    // add the file menu to the menu bar
    mb->Append(emulationMenu, wxT("&Emulation"));

    // create the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(wxID_ABOUT, wxT("&About"));

    // add the help menu to the menu bar
    mb->Append(helpMenu, wxT("&Help"));

    // add the menu bar to the MainFrame
    this->SetMenuBar(mb);
}

void MainFrame::CreateToolBar()
{

	toolBar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL|wxNO_BORDER);
	wxBitmap bmpOpen(open_xpm);
	toolBar->AddTool(wxID_OPEN, bmpOpen, wxT("Open"));
	
	wxBitmap bmpRecent(recent_xpm);
	toolBar->AddTool(ID_OPEN_RECENT, bmpRecent, wxT("Recent"));

	toolBar->AddSeparator();

	wxBitmap bmpPlay(play_xpm);
	toolBar->AddTool(ID_START, bmpPlay, wxT("Start"));

	wxBitmap bmpPause(pause_xpm);
	toolBar->AddTool(ID_PAUSE, bmpPause, wxT("Pause"));

	wxBitmap bmpStop(stop_xpm);
	toolBar->AddTool(ID_STOP, bmpStop, wxT("Stop"));
	
	toolBar->EnableTool(ID_START, false);
	toolBar->EnableTool(ID_PAUSE, false);
	toolBar->EnableTool(ID_STOP, false);

	toolBar->Realize();
	this->SetToolBar(toolBar);
}

void MainFrame::OnRecent(wxCommandEvent &event)
{
	this->PopupMenu(recentMenuPopup, 0, 0);
}

void MainFrame::OnFileOpen(wxCommandEvent &) {

	enumEmuStates copyState = emulation->GetState();
    emulation->SetState(Paused);
	
	wxFileDialog* openDialog = new wxFileDialog(this, wxT("Choose a gameboy rom to open"), wxEmptyString, wxEmptyString,
												wxT("Gameboy roms (*.gb; *.zip)|*.gb;*.zip"),
												wxFD_OPEN, wxDefaultPosition);

	
	// Creates a "open file" dialog
	if (openDialog->ShowModal() == wxID_OK) // if the user click "Open" instead of "Cancel"
		this->ChangeFile(openDialog->GetPath());
	else
		emulation->SetState(copyState);

	// Clean up after ourselves
	openDialog->Destroy();
}
 
void MainFrame::OnRecentItem(wxCommandEvent &event)
{
	int idAux = event.GetId();
	int id = idAux - ID_RECENT0;
	ChangeFile(recentFiles[id].fullName);
}

void MainFrame::OnLoadState(wxCommandEvent &event)
{
	int idAux = event.GetId();
	int id = idAux - ID_LOADSTATE0;
	
	wxString savesDir = wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator()
	+ wxT("SaveStates") + wxFileName::GetPathSeparator();

	try
	{
		emulation->LoadState(string(savesDir.mb_str()), id);
	}
	catch(GBException e)
	{
		wxMessageBox(wxString(e.what(), wxConvUTF8), wxT("Error"), wxICON_WARNING);
	}
}

void MainFrame::OnSaveState(wxCommandEvent &event)
{
	int idAux = event.GetId();
	int id = idAux - ID_SAVESTATE0;
	
	wxString savesDir = wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator()
	+ wxT("SaveStates");
	
	if (!wxFileName::DirExists(savesDir))
		wxFileName::Mkdir(savesDir, 0777, wxPATH_MKDIR_FULL);
	
	savesDir += wxFileName::GetPathSeparator();
	try
	{
		emulation->SaveState(string(savesDir.mb_str()), id);
	}
	catch(GBException e)
	{
		wxMessageBox(wxString(e.what(), wxConvUTF8), wxT("Error"), wxICON_WARNING);
	}
}

void MainFrame::OnClearRecent(wxCommandEvent &)
{
	for (int i=0; i<numRecentFiles; i++)
	{
		recentMenuFile->Delete(ID_RECENT0+i);
		recentMenuPopup->Delete(ID_RECENT0+i);
	}
	numRecentFiles = 0;
	
	this->RecentRomsToSettings();
	settingsDialog->SaveToFile(true);
}

void MainFrame::ChangeFile(const wxString fileName)
{
	if (emulation->ChangeFile(fileName))
        this->UpdateRecentMenu(fileName);
}

void MainFrame::CreateRecentMenu(std::string * roms)
{
	for (int i=0; i<MAX_RECENT_FILES; i++)
	{
		if (roms[i] == "")
			break;
		
		recentFiles[i].fullName = wxString(roms[i].c_str(), wxConvUTF8);
		recentFiles[i].shortName = recentFiles[i].fullName.substr(recentFiles[i].fullName.rfind(wxFileName::GetPathSeparator())+1);
		
		int id = ID_RECENT0 + numRecentFiles;
		recentMenuFile->Insert(numRecentFiles, id, recentFiles[i].shortName);
		recentMenuPopup->Insert(numRecentFiles, id, recentFiles[i].shortName);
		
		numRecentFiles++;
	}
	
}

void MainFrame::UpdateRecentMenu(wxString fileName)
{
	wxString shortName = fileName.substr(fileName.rfind(wxFileName::GetPathSeparator())+1);
	int previousIndex = -1;
	for (int i=0; i<numRecentFiles; i++)
	{
		if (recentFiles[i].fullName == fileName)
		{
			previousIndex = i;
			break;
		}
	}
	
	int startFrom;
	
	// Si ya existia de antes y es el primero
	if (previousIndex == 0)
	{
		return;
	}
	// Si ya existia de antes y no es el primero
	else if (previousIndex > 0)
	{
		startFrom = previousIndex-1;
	}
	// Si no existia pero no hemos llegado al limite
	else if (numRecentFiles < MAX_RECENT_FILES)
	{
		startFrom = numRecentFiles-1;
		int id = ID_RECENT0 + numRecentFiles;
		recentMenuFile->Insert(numRecentFiles, id, wxT(""));
		recentMenuPopup->Insert(numRecentFiles, id, wxT(""));
		numRecentFiles++;
	}
	// Si no existia pero hemos llegado al limite
	else
	{
		startFrom = MAX_RECENT_FILES-2;
	}
	
	for (int i=startFrom; i>=0; i--)
	{
		recentFiles[i+1].shortName = recentFiles[i].shortName;
		recentFiles[i+1].fullName = recentFiles[i].fullName;
	}
	recentFiles[0].shortName = shortName;
	recentFiles[0].fullName = fileName;
	
	for (int i=0; i<numRecentFiles; i++)
	{
		recentMenuFile->SetLabel(ID_RECENT0+i, recentFiles[i].shortName);
		recentMenuPopup->SetLabel(ID_RECENT0+i, recentFiles[i].shortName);
	}
	
	this->RecentRomsToSettings();
	settingsDialog->SaveToFile(true);
}


void MainFrame::RecentRomsToSettings()
{
	std::string recentRomsSettings[10];
	
	for (int i=0; i<numRecentFiles; i++)
	{
		recentRomsSettings[i] = recentFiles[i].fullName.mb_str();
	}
	
	for(int i=numRecentFiles; i<MAX_RECENT_FILES; i++)
	{
		recentRomsSettings[i] = "";
	}
	
	SettingsSetRecentRoms(recentRomsSettings);
}

void MainFrame::OnFileExit(wxCommandEvent &)
{
	this->Close();
}

void MainFrame::OnClose(wxCloseEvent&)
{
    timer->Stop();
    
	if (settingsDialog)
		settingsDialog->Destroy();
    
    emulation->Delete();
    
    Destroy();
}

/*
 * Abre un dialogo de configuracion. Cuando se cierra se encarga de aplicar ciertos cambios a la emulacion
 */
void MainFrame::OnSettings(wxCommandEvent &)
{
	enumEmuStates lastState = emulation->GetState();
	if (emulation->GetState() == Playing)
		emulation->SetState(Paused);


    if (settingsDialog->ShowModal() == wxID_OK)
	{
		SettingsSetNewValues(settingsDialog->settings);
        if (SettingsGetRenderMethod() != typeRenderer)
        {
            ChangeRenderer();    
        }
        
        if (renderer)
            renderer->ChangePalette(SettingsGetGreenScale());
        
        if (!fullScreen)
        {
            if (renderer)
                renderer->ChangeSize();
            this->SetClientSize(GB_SCREEN_W*SettingsGetWindowZoom(), GB_SCREEN_H*SettingsGetWindowZoom());
        }
		emulation->ApplySettings();
	}

    emulation->SetState(lastState);
}

void MainFrame::ChangeRenderer()
{
    typeRenderer = SettingsGetRenderMethod();
    
    if (renderer)
    {
        ((wxWindow *)renderer->GetWinRenderer())->Destroy();
    }
    
    if (typeRenderer == 0)
    {
        renderer = new RendererSW(this);
    }
    else
    {
        renderer = new RendererOGL(this);
    }
    
    // Redimensionar el frame para que el dibujado se realize correctamente
    this->SetClientSize(GB_SCREEN_W*SettingsGetWindowZoom()+1, GB_SCREEN_H*SettingsGetWindowZoom()+1);
    this->SetClientSize(GB_SCREEN_W*SettingsGetWindowZoom(), GB_SCREEN_H*SettingsGetWindowZoom());
    
    emulation->SetScreen(renderer);
}

void MainFrame::OnFullScreen(wxCommandEvent &)
{
    ToggleFullScreen();
}

void MainFrame::OnAbout(wxCommandEvent &)
{
	AboutDialog(this);
}

void MainFrame::OnPlay(wxCommandEvent &)
{
    emulation->SetState(Playing);
}

void MainFrame::OnPause(wxCommandEvent &)
{
	if (emulation->GetState() == Playing)
		emulation->SetState(Paused);
	else if (emulation->GetState() == Paused)
		emulation->SetState(Playing);
}

void MainFrame::OnStop(wxCommandEvent &)
{
    emulation->SetState(Stopped);
}

void MainFrame::OnPlayUpdateUI(wxUpdateUIEvent& event)
{
	if ((emulation->GetState() == NotStartedYet) || (emulation->GetState() == Playing))
		event.Enable(false);
	else
		event.Enable(true);
}

void MainFrame::OnPauseUpdateUI(wxUpdateUIEvent& event)
{
	if ((emulation->GetState() == NotStartedYet) || (emulation->GetState() == Stopped))
		event.Enable(false);
	else
		event.Enable(true);
}

void MainFrame::OnStopUpdateUI(wxUpdateUIEvent& event)
{
	if ((emulation->GetState() == Stopped)||(emulation->GetState() == NotStartedYet))
		event.Enable(false);
	else
		event.Enable(true);
}

void MainFrame::OnLoadStateUpdateUI(wxUpdateUIEvent& event)
{
	if ((emulation->GetState() == Stopped)||(emulation->GetState() == NotStartedYet))
		event.Enable(false);
	else
		event.Enable(true);
}

void MainFrame::OnSaveStateUpdateUI(wxUpdateUIEvent& event)
{
	if ((emulation->GetState() == Stopped)||(emulation->GetState() == NotStartedYet))
		event.Enable(false);
	else
		event.Enable(true);
}

void MainFrame::OnFullScreenUpdateUI(wxUpdateUIEvent& event)
{
    event.Enable(typeRenderer == 1);
}

void MainFrame::OnDoubleClick(wxMouseEvent &event)
{
    ToggleFullScreen();
    event.Skip();
}

void MainFrame::OnRefreshScreen(wxCommandEvent& event)
{
    renderer->OnRefreshScreen();
}

void MainFrame::ToggleFullScreen()
{
    fullScreen = !fullScreen;
    ShowFullScreen(fullScreen, wxFULLSCREEN_ALL);
    
    if (!fullScreen)
    {
        renderer->ChangeSize();
        this->SetClientSize(GB_SCREEN_W*SettingsGetWindowZoom(), GB_SCREEN_H*SettingsGetWindowZoom());
    }
}

void MainFrame::OnTimer(wxTimerEvent &event)
{
    renderer->OnRefreshScreen();
    emulation->UpdatePad();
}
