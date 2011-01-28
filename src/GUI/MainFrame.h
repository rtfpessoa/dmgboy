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

#include <wx/wx.h>
#include "SettingsDialog.h"
#include "MainPanel.h"
#include "../CPU.h"

/*******************************************************************************
 // MainFrame Class
 *******************************************************************************/

class MainFrame : public wxFrame {
    DECLARE_CLASS(MainFrame)
    DECLARE_EVENT_TABLE()
    
private:
	wxMenuBar *mb;
	wxToolBar* toolBar;
    MainPanel *panel;
	SettingsDialog * settingsDialog;
	Video * video;
	Cartridge * cartridge;
	
	enum enumEmuStates { NotStartedYet, Stopped, Paused, Playing };
	
	enumEmuStates emuState;
    
    /**
     * Called when exit from the file menu is selected.
     */
    void onFileExit(wxCommandEvent &);
	void onFileOpen(wxCommandEvent &);
	void onSettings(wxCommandEvent &);
	void onAbout(wxCommandEvent &);
	void onPlay(wxCommandEvent &);
	void onPause(wxCommandEvent &);
	void onStop(wxCommandEvent &);
	void onPlayUpdate(wxUpdateUIEvent& event);
	void onPauseUpdate(wxUpdateUIEvent& event);
	void onStopUpdate(wxUpdateUIEvent& event);
	void onIdle(wxIdleEvent &event);
	void createMenuBar();
	void createToolBar();
	void LoadZip(wxString zipPath, BYTE ** buffer, unsigned long * size);
	
	void Clean();
    
public:
	CPU * cpu;
	
	wxStopWatch swExecution;
	wxStopWatch swFrame;
	
    /**
     * Creates a new MainFrame.
     */
    MainFrame();
	~MainFrame();
	
    /**
     * Gets the MainPanel within this MainFrame.
     *
     * @return The MainPanel.
     */
	
	inline MainPanel &getPanel() { return *panel; }
};

#endif
