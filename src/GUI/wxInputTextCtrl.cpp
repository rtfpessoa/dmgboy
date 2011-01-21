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

#include "wxInputTextCtrl.h"

IMPLEMENT_CLASS(InputTextCtrl, wxTextCtrl)
BEGIN_EVENT_TABLE(InputTextCtrl, wxTextCtrl)
	EVT_KEY_DOWN(InputTextCtrl::OnKeyDown)
END_EVENT_TABLE()

InputTextCtrl::InputTextCtrl(wxWindow* parent, wxWindowID id)
{
	Create(parent, id);
}

void InputTextCtrl::OnKeyDown(wxKeyEvent& event)
{
	int keyCode = event.GetKeyCode();
	this->SetValue(wxString::Format(wxT("%i"), keyCode));
}