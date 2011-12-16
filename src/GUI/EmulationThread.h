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

#ifndef __EMULATIONTHREAD_H__
#define __EMULATIONTHREAD_H__

#include <wx/wx.h>

enum enumEmuStates { NotStartedYet, Stopped, Paused, Playing };

class Cartridge;
class Video;
class Sound;
class CPU;
class IGBScreenDrawable;
class wxMutex;

class EmulationThread : public wxThread {

private:
	Video * video;
	Sound * sound;
	Cartridge * cartridge;
    CPU * cpu;
    wxMutex * mutex;
    wxStopWatch swFrame;
    
	enumEmuStates emuState;
    
    void LoadZip(const wxString zipPath, BYTE ** buffer, unsigned long * size);
    
public:
	EmulationThread();
    ~EmulationThread();
    
    virtual ExitCode Entry();
    
    bool ChangeFile(wxString fileName);
    void LoadState(std::string fileName, int id);
    void SaveState(std::string fileName, int id);
    void ApplySettings();
    void SetScreen(IGBScreenDrawable * screen);
    void UpdatePad();
    
    enumEmuStates GetState();
    void SetState(enumEmuStates state);
    
};

#endif
