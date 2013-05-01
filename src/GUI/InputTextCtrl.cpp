/*
 This file is part of DMGBoy.
 
 DMGBoy is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 DMGBoy is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with DMGBoy.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "InputTextCtrl.h"

BEGIN_EVENT_TABLE(InputTextCtrl, wxTextCtrl)
	EVT_KEY_DOWN(InputTextCtrl::OnKeyDown)
END_EVENT_TABLE()

wxString InputTextCtrl::keyNames[NUM_KEYNAMES];
bool InputTextCtrl::keyNamesInitialized = false;

void InputTextCtrl::InitializeKeyNames()
{
	if (InputTextCtrl::keyNamesInitialized)
		return;
	
	for (int i=0; i<NUM_KEYNAMES; i++)
		InputTextCtrl::keyNames[i] = wxT("");
	
	/* ASCII */
	/*
	InputTextCtrl::keyNames['!']		= wxT("!");
	InputTextCtrl::keyNames['\"']		= wxT("\"");
	InputTextCtrl::keyNames['#']		= wxT("#");
	InputTextCtrl::keyNames['$']		= wxT("$");
	InputTextCtrl::keyNames['%']		= wxT("%");
	InputTextCtrl::keyNames['&']		= wxT("&");
	InputTextCtrl::keyNames['\'']		= wxT("'");
	InputTextCtrl::keyNames['(']		= wxT("(");
	InputTextCtrl::keyNames[')']		= wxT(")");
	InputTextCtrl::keyNames['*']		= wxT("*");
	InputTextCtrl::keyNames['+']		= wxT("+");
	InputTextCtrl::keyNames[',']		= wxT(",");
	InputTextCtrl::keyNames['-']		= wxT("-");
	InputTextCtrl::keyNames['.']		= wxT(".");
	InputTextCtrl::keyNames['/']		= wxT("/");
	*/
	
	InputTextCtrl::keyNames['0']		= wxT("0");
	InputTextCtrl::keyNames['1']		= wxT("1");
	InputTextCtrl::keyNames['2']		= wxT("2");
	InputTextCtrl::keyNames['3']		= wxT("3");
	InputTextCtrl::keyNames['4']		= wxT("4");
	InputTextCtrl::keyNames['5']		= wxT("5");
	InputTextCtrl::keyNames['6']		= wxT("6");
	InputTextCtrl::keyNames['7']		= wxT("7");
	InputTextCtrl::keyNames['8']		= wxT("8");
	InputTextCtrl::keyNames['9']		= wxT("9");
	
	/*
	InputTextCtrl::keyNames[':']		= wxT(":");
	InputTextCtrl::keyNames[';']		= wxT(";");
	InputTextCtrl::keyNames['<']		= wxT("<");
	InputTextCtrl::keyNames['=']		= wxT("=");
	InputTextCtrl::keyNames['>']		= wxT(">");
	InputTextCtrl::keyNames['?']		= wxT("?");
	InputTextCtrl::keyNames['@']		= wxT("@");
	*/
	
	InputTextCtrl::keyNames['A']		= wxT("A");
	InputTextCtrl::keyNames['B']		= wxT("B");
	InputTextCtrl::keyNames['C']		= wxT("C");
	InputTextCtrl::keyNames['D']		= wxT("D");
	InputTextCtrl::keyNames['E']		= wxT("E");
	InputTextCtrl::keyNames['F']		= wxT("F");
	InputTextCtrl::keyNames['G']		= wxT("G");
	InputTextCtrl::keyNames['H']		= wxT("H");
	InputTextCtrl::keyNames['I']		= wxT("I");
	InputTextCtrl::keyNames['J']		= wxT("J");
	InputTextCtrl::keyNames['K']		= wxT("K");
	InputTextCtrl::keyNames['L']		= wxT("L");
	InputTextCtrl::keyNames['M']		= wxT("M");
	InputTextCtrl::keyNames['N']		= wxT("N");
	InputTextCtrl::keyNames['O']		= wxT("O");
	InputTextCtrl::keyNames['P']		= wxT("P");
	InputTextCtrl::keyNames['Q']		= wxT("Q");
	InputTextCtrl::keyNames['R']		= wxT("R");
	InputTextCtrl::keyNames['S']		= wxT("S");
	InputTextCtrl::keyNames['T']		= wxT("T");
	InputTextCtrl::keyNames['U']		= wxT("U");
	InputTextCtrl::keyNames['V']		= wxT("V");
	InputTextCtrl::keyNames['W']		= wxT("W");
	InputTextCtrl::keyNames['X']		= wxT("X");
	InputTextCtrl::keyNames['Y']		= wxT("Y");
	InputTextCtrl::keyNames['Z']		= wxT("Z");
	
	/*
	InputTextCtrl::keyNames['[']		= wxT("[");
	InputTextCtrl::keyNames['\\']		= wxT("\\");
	InputTextCtrl::keyNames[']']		= wxT("]");
	InputTextCtrl::keyNames['^']		= wxT("^");
	InputTextCtrl::keyNames['_']		= wxT("_");
	InputTextCtrl::keyNames['`']		= wxT("`");
	
	InputTextCtrl::keyNames['{']		= wxT("{");
	InputTextCtrl::keyNames['|']		= wxT("|");
	InputTextCtrl::keyNames['}']		= wxT("}");
	InputTextCtrl::keyNames['~']		= wxT("~");
	*/
	
	
	/* NO ASCII */
	InputTextCtrl::keyNames[WXK_BACK]		= wxT("Backspace");
	InputTextCtrl::keyNames[WXK_TAB]		= wxT("Tab");
	InputTextCtrl::keyNames[WXK_RETURN]		= wxT("Return");
	InputTextCtrl::keyNames[WXK_ESCAPE]		= wxT("Escape");
	InputTextCtrl::keyNames[WXK_SPACE]		= wxT("Space");
	InputTextCtrl::keyNames[WXK_DELETE]		= wxT("Delete");
	InputTextCtrl::keyNames[WXK_START]		= wxT("Start");
	InputTextCtrl::keyNames[WXK_LBUTTON]	= wxT("Left Mouse Button");
	InputTextCtrl::keyNames[WXK_RBUTTON]	= wxT("Right Mouse Button");
	InputTextCtrl::keyNames[WXK_CANCEL]		= wxT("Control-Break");
	InputTextCtrl::keyNames[WXK_MBUTTON]	= wxT("Middle Mouse Button");
	InputTextCtrl::keyNames[WXK_CLEAR]		= wxT("Clear");
	InputTextCtrl::keyNames[WXK_SHIFT]		= wxT("Shift");
	InputTextCtrl::keyNames[WXK_ALT]		= wxT("Alt");
	InputTextCtrl::keyNames[WXK_CONTROL]	= wxT("Control");
	InputTextCtrl::keyNames[WXK_MENU]		= wxT("Menu");
	InputTextCtrl::keyNames[WXK_PAUSE]		= wxT("Pause");
	InputTextCtrl::keyNames[WXK_CAPITAL]	= wxT("CapsLock");
	InputTextCtrl::keyNames[WXK_END]		= wxT("End");
	InputTextCtrl::keyNames[WXK_HOME]		= wxT("Home");
	InputTextCtrl::keyNames[WXK_LEFT]		= wxT("Left");
	InputTextCtrl::keyNames[WXK_UP]			= wxT("Up");
	InputTextCtrl::keyNames[WXK_RIGHT]		= wxT("Right");
	InputTextCtrl::keyNames[WXK_DOWN]		= wxT("Down");
	InputTextCtrl::keyNames[WXK_SELECT]		= wxT("Select");
	InputTextCtrl::keyNames[WXK_PRINT]		= wxT("Print");
	InputTextCtrl::keyNames[WXK_EXECUTE]	= wxT("Execute");
	InputTextCtrl::keyNames[WXK_SNAPSHOT]	= wxT("Print Screen");
	InputTextCtrl::keyNames[WXK_INSERT]		= wxT("Insert");
	InputTextCtrl::keyNames[WXK_HELP]		= wxT("Help");
	InputTextCtrl::keyNames[WXK_NUMPAD0]	= wxT("Numpad 0");
	InputTextCtrl::keyNames[WXK_NUMPAD1]	= wxT("Numpad 1");
	InputTextCtrl::keyNames[WXK_NUMPAD2]	= wxT("Numpad 2");
	InputTextCtrl::keyNames[WXK_NUMPAD3]	= wxT("Numpad 3");
	InputTextCtrl::keyNames[WXK_NUMPAD4]	= wxT("Numpad 4");
	InputTextCtrl::keyNames[WXK_NUMPAD5]	= wxT("Numpad 5");
	InputTextCtrl::keyNames[WXK_NUMPAD6]	= wxT("Numpad 6");
	InputTextCtrl::keyNames[WXK_NUMPAD7]	= wxT("Numpad 7");
	InputTextCtrl::keyNames[WXK_NUMPAD8]	= wxT("Numpad 8");
	InputTextCtrl::keyNames[WXK_NUMPAD9]	= wxT("Numpad 9");
	InputTextCtrl::keyNames[WXK_MULTIPLY]	= wxT("*");
	InputTextCtrl::keyNames[WXK_ADD]		= wxT("+");
	InputTextCtrl::keyNames[WXK_SEPARATOR]	= wxT("'");
	InputTextCtrl::keyNames[WXK_SUBTRACT]	= wxT("-");
	InputTextCtrl::keyNames[WXK_DECIMAL]	= wxT(".");
	InputTextCtrl::keyNames[WXK_DIVIDE]		= wxT("/");
	
	InputTextCtrl::keyNames[WXK_F1]			= wxT("F1");
	InputTextCtrl::keyNames[WXK_F2]			= wxT("F2");
	InputTextCtrl::keyNames[WXK_F3]			= wxT("F3");
	InputTextCtrl::keyNames[WXK_F4]			= wxT("F4");
	InputTextCtrl::keyNames[WXK_F5]			= wxT("F5");
	InputTextCtrl::keyNames[WXK_F6]			= wxT("F6");
	InputTextCtrl::keyNames[WXK_F7]			= wxT("F7");
	InputTextCtrl::keyNames[WXK_F8]			= wxT("F8");
	InputTextCtrl::keyNames[WXK_F9]			= wxT("F9");
	InputTextCtrl::keyNames[WXK_F10]		= wxT("F10");
	InputTextCtrl::keyNames[WXK_F11]		= wxT("F11");
	InputTextCtrl::keyNames[WXK_F12]		= wxT("F12");
	InputTextCtrl::keyNames[WXK_F13]		= wxT("F13");
	InputTextCtrl::keyNames[WXK_F14]		= wxT("F14");
	InputTextCtrl::keyNames[WXK_F15]		= wxT("F15");
	InputTextCtrl::keyNames[WXK_F16]		= wxT("F16");
	InputTextCtrl::keyNames[WXK_F17]		= wxT("F17");
	InputTextCtrl::keyNames[WXK_F18]		= wxT("F18");
	InputTextCtrl::keyNames[WXK_F19]		= wxT("F19");
	InputTextCtrl::keyNames[WXK_F20]		= wxT("F20");
	InputTextCtrl::keyNames[WXK_F21]		= wxT("F21");
	InputTextCtrl::keyNames[WXK_F22]		= wxT("F22");
	InputTextCtrl::keyNames[WXK_F23]		= wxT("F23");
	InputTextCtrl::keyNames[WXK_F24]		= wxT("F24");
	
	InputTextCtrl::keyNames[WXK_NUMLOCK]	= wxT("NumLock");
	InputTextCtrl::keyNames[WXK_SCROLL]		= wxT("ScrollLock");
	InputTextCtrl::keyNames[WXK_PAGEUP]		= wxT("PageUp");
	InputTextCtrl::keyNames[WXK_PAGEDOWN]	= wxT("PageDown");
	//InputTextCtrl::keyNames[WXK_PRIOR]		= wxT("PageUp");
	//InputTextCtrl::keyNames[WXK_NEXT]		= wxT("PageDown");
	
	InputTextCtrl::keyNames[WXK_NUMPAD_SPACE]		= wxT("Numpad Space");
	InputTextCtrl::keyNames[WXK_NUMPAD_TAB]			= wxT("Numpad Tab");
	InputTextCtrl::keyNames[WXK_NUMPAD_ENTER]		= wxT("Numpad Enter");
	InputTextCtrl::keyNames[WXK_NUMPAD_F1]			= wxT("Numpad F1");
	InputTextCtrl::keyNames[WXK_NUMPAD_F2]			= wxT("Numpad F2");
	InputTextCtrl::keyNames[WXK_NUMPAD_F3]			= wxT("Numpad F3");
	InputTextCtrl::keyNames[WXK_NUMPAD_F4]			= wxT("Numpad F4");
	InputTextCtrl::keyNames[WXK_NUMPAD_HOME]		= wxT("Numpad Home");
	InputTextCtrl::keyNames[WXK_NUMPAD_LEFT]		= wxT("Numpad Left");
	InputTextCtrl::keyNames[WXK_NUMPAD_UP]			= wxT("Numpad Up");
	InputTextCtrl::keyNames[WXK_NUMPAD_RIGHT]		= wxT("Numpad Right");
	InputTextCtrl::keyNames[WXK_NUMPAD_DOWN]		= wxT("Numpad Down");
	InputTextCtrl::keyNames[WXK_NUMPAD_PAGEUP]		= wxT("Numpad PageUp");
	InputTextCtrl::keyNames[WXK_NUMPAD_PAGEDOWN]	= wxT("Numpad PageDown");
	//InputTextCtrl::keyNames[WXK_NUMPAD_PRIOR]		= wxT("Numpad PageUp");
	//InputTextCtrl::keyNames[WXK_NUMPAD_NEXT]		= wxT("Numpad PageDown");
	InputTextCtrl::keyNames[WXK_NUMPAD_END]			= wxT("Numpad End");
	InputTextCtrl::keyNames[WXK_NUMPAD_BEGIN]		= wxT("Numpad Begin");
	InputTextCtrl::keyNames[WXK_NUMPAD_INSERT]		= wxT("Numpad Insert");
	InputTextCtrl::keyNames[WXK_NUMPAD_DELETE]		= wxT("Numpad Delete");
	InputTextCtrl::keyNames[WXK_NUMPAD_EQUAL]		= wxT("Numpad =");
	InputTextCtrl::keyNames[WXK_NUMPAD_MULTIPLY]	= wxT("Numpad *");
	InputTextCtrl::keyNames[WXK_NUMPAD_ADD]			= wxT("Numpad +");
	InputTextCtrl::keyNames[WXK_NUMPAD_SEPARATOR]	= wxT("Numpad Separator");
	InputTextCtrl::keyNames[WXK_NUMPAD_SUBTRACT]	= wxT("Numpad -");
	InputTextCtrl::keyNames[WXK_NUMPAD_DECIMAL]		= wxT("Numpad .");
	InputTextCtrl::keyNames[WXK_NUMPAD_DIVIDE]		= wxT("Numpad /");
	
	InputTextCtrl::keyNames[WXK_WINDOWS_LEFT]		= wxT("Windows Left");
	InputTextCtrl::keyNames[WXK_WINDOWS_RIGHT]		= wxT("Windows Right");
	InputTextCtrl::keyNames[WXK_WINDOWS_MENU]		= wxT("Application");
	InputTextCtrl::keyNames[WXK_COMMAND]			= wxT("Command");
	
	InputTextCtrl::keyNames[WXK_SPECIAL1]			= wxT("Special 1");
	InputTextCtrl::keyNames[WXK_SPECIAL2]			= wxT("Special 2");
	InputTextCtrl::keyNames[WXK_SPECIAL3]			= wxT("Special 3");
	InputTextCtrl::keyNames[WXK_SPECIAL4]			= wxT("Special 4");
	InputTextCtrl::keyNames[WXK_SPECIAL5]			= wxT("Special 5");
	InputTextCtrl::keyNames[WXK_SPECIAL6]			= wxT("Special 6");
	InputTextCtrl::keyNames[WXK_SPECIAL7]			= wxT("Special 7");
	InputTextCtrl::keyNames[WXK_SPECIAL8]			= wxT("Special 8");
	InputTextCtrl::keyNames[WXK_SPECIAL9]			= wxT("Special 9");
	InputTextCtrl::keyNames[WXK_SPECIAL10]			= wxT("Special 10");
	InputTextCtrl::keyNames[WXK_SPECIAL11]			= wxT("Special 11");
	InputTextCtrl::keyNames[WXK_SPECIAL12]			= wxT("Special 12");
	InputTextCtrl::keyNames[WXK_SPECIAL13]			= wxT("Special 13");
	InputTextCtrl::keyNames[WXK_SPECIAL14]			= wxT("Special 14");
	InputTextCtrl::keyNames[WXK_SPECIAL15]			= wxT("Special 15");
	InputTextCtrl::keyNames[WXK_SPECIAL16]			= wxT("Special 16");
	InputTextCtrl::keyNames[WXK_SPECIAL17]			= wxT("Special 17");
	InputTextCtrl::keyNames[WXK_SPECIAL18]			= wxT("Special 18");
	InputTextCtrl::keyNames[WXK_SPECIAL19]			= wxT("Special 19");
	InputTextCtrl::keyNames[WXK_SPECIAL20]			= wxT("Special 20");
	
	InputTextCtrl::keyNamesInitialized = true;
}

InputTextCtrl::InputTextCtrl(wxWindow* parent, wxWindowID id)
{
	keyCode = 0;
	InputTextCtrl::InitializeKeyNames();
	Create(parent, id, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER|wxTE_PROCESS_TAB );
}

void InputTextCtrl::OnKeyDown(wxKeyEvent& event)
{
	int keyCode = event.GetKeyCode();
	OnChangeKey(keyCode);
}

void InputTextCtrl::OnChangeKey(int keyCode)
{
	if (InputTextCtrl::keyNames[keyCode] == wxT(""))
	{
		if (!keyCode)
		{
			this->SetBackgroundColour(*wxRED);
			this->ChangeValue(wxT("Invalid key"));
		}
		else
		{
			this->SetBackgroundColour(*wxWHITE);
			this->ChangeValue(wxString::Format(wxT("Key %i"),keyCode));
		}
	}
	else
	{
		this->SetBackgroundColour(*wxWHITE);
		this->ChangeValue(InputTextCtrl::keyNames[keyCode]);
	}
	this->keyCode = keyCode;
}

