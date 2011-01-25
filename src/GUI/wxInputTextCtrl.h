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

#ifndef __WXINPUTTEXTCTRL_H__
#define __WXINPUTTEXTCTRL_H__

#define NUM_KEYNAMES 1024

#include <wx/wx.h>

class InputTextCtrl: public wxTextCtrl
{
	DECLARE_CLASS(InputTextCtrl)
	DECLARE_EVENT_TABLE()
	
public:
	int keyCode;
	InputTextCtrl(wxWindow* parent, wxWindowID id);
	void OnChangeKey(int keyCode);
private:
	static wxString keyNames[NUM_KEYNAMES];
	static bool keyNamesInitialized;
	static void InitializeKeyNames();
	
	void OnKeyDown(wxKeyEvent& event);
	
};
#endif