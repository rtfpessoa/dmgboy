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
#include "SettingsDialog.h"
#include "MainPanel.h"
#include "../CPU.h"

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
	wxMenu *recentMenu;
	wxToolBar* toolBar;
    MainPanel *panel;
	SettingsDialog * settingsDialog;
	Video * video;
	Cartridge * cartridge;
	
	RecentFile recentFiles[MAX_RECENT_FILES];
	int numRecentFiles;
	
	enum enumEmuStates { NotStartedYet, Stopped, Paused, Playing };
	
	enumEmuStates emuState;
    
    /**
     * Called when exit from the file menu is selected.
     */
    void OnFileExit(wxCommandEvent &);
	void OnFileOpen(wxCommandEvent &);
	void OnRecent(wxCommandEvent &event);
	void OnClearRecent(wxCommandEvent &event);
	void OnSettings(wxCommandEvent &);
	void OnAbout(wxCommandEvent &);
	void OnPlay(wxCommandEvent &);
	void OnPause(wxCommandEvent &);
	void OnStop(wxCommandEvent &);
	void OnPlayUpdate(wxUpdateUIEvent& event);
	void OnPauseUpdate(wxUpdateUIEvent& event);
	void OnStopUpdate(wxUpdateUIEvent& event);
	void OnIdle(wxIdleEvent &event);
	void CreateMenuBar();
	void CreateToolBar();
	void LoadZip(const wxString zipPath, BYTE ** buffer, unsigned long * size);
	void UpdateRecentMenu(wxString fileName);
	
	void Clean();
    
public:
	CPU * cpu;
	
	wxStopWatch swExecution;
	wxStopWatch swFrame;
	
    /**
     * Creates a new MainFrame.
     */
    MainFrame(wxString fileName);
	~MainFrame();
	
    /**
     * Gets the MainPanel within this MainFrame.
     *
     * @return The MainPanel.
     */
	inline MainPanel &GetPanel() { return *panel; }
	void ChangeFile(const wxString fileName);
};

#endif
