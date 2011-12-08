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

#ifndef __MAINFRAME_H__
#define __MAINFRAME_H__

#define MAX_RECENT_FILES 10

#include <wx/wx.h>
#include "../Def.h"

class RendererBase;
class SettingsDialog;
class EmulationThread;

struct RecentFile
{
	wxString shortName;
	wxString fullName;
};

/*******************************************************************************
 // MainFrame Class
 *******************************************************************************/

class MainFrame : public wxFrame {
    DECLARE_CLASS(MainFrame)
    DECLARE_EVENT_TABLE()
    
private:
	wxMenuBar *mb;
	wxMenu *recentMenuFile;
	wxMenu *recentMenuPopup;
	wxToolBar* toolBar;
	wxTimer * timer;
    bool fullScreen;
    RendererBase *renderer;
    int typeRenderer;
	SettingsDialog * settingsDialog;
    EmulationThread * emulation;
	
	RecentFile recentFiles[MAX_RECENT_FILES];
	int numRecentFiles;
    
    /**
     * Called when exit from the file menu is selected.
     */
    void OnFileExit(wxCommandEvent &);
	void OnFileOpen(wxCommandEvent &);
	void OnRecent(wxCommandEvent &event);
	void OnRecentItem(wxCommandEvent &event);
	void OnClearRecent(wxCommandEvent &event);
	void OnLoadState(wxCommandEvent &event);
	void OnSaveState(wxCommandEvent &event);
	void OnSettings(wxCommandEvent &);
    void OnFullScreen(wxCommandEvent &);
	void OnAbout(wxCommandEvent &);
	void OnPlay(wxCommandEvent &);
	void OnPause(wxCommandEvent &);
	void OnStop(wxCommandEvent &);
	void OnPlayUpdateUI(wxUpdateUIEvent& event);
	void OnPauseUpdateUI(wxUpdateUIEvent& event);
	void OnStopUpdateUI(wxUpdateUIEvent& event);
	void OnLoadStateUpdateUI(wxUpdateUIEvent& event);
	void OnSaveStateUpdateUI(wxUpdateUIEvent& event);
    void OnFullScreenUpdateUI(wxUpdateUIEvent& event);
    void OnDoubleClick(wxMouseEvent &event);
    void OnRefreshScreen(wxCommandEvent& evt);
    void OnTimer(wxTimerEvent &event);
	void CreateMenuBar();
	void CreateToolBar();
	void CreateRecentMenu(std::string * roms);
	void RecentRomsToSettings();
	void UpdateRecentMenu(wxString fileName);
	void LoadZip(const wxString zipPath, BYTE ** buffer, unsigned long * size);
    void ToggleFullScreen();
    void ChangeRenderer();
	
	void OnClose(wxCloseEvent&);
    
public:
	
    /**
     * Creates a new MainFrame.
     */
    MainFrame(wxString fileName);
	~MainFrame();
	
	void ChangeFile(const wxString fileName);
};

#endif
