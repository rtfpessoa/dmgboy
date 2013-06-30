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

#ifndef __IDCONSTROLS_H__
#define __IDCONSTROLS_H__

/*******************************************************************************
 * Global Declarations
 *******************************************************************************/

enum {
    ID_MAINFRAME = wxID_HIGHEST + 1,
    ID_MAINPANEL,
	ID_LOAD,
	ID_START,
	ID_PAUSE,
	ID_STOP,
    ID_FULLSCREEN,
	ID_TIMER,
	ID_OPEN_RECENT,
	
	ID_RECENT0,
	ID_RECENT1,
	ID_RECENT2,
	ID_RECENT3,
	ID_RECENT4,
	ID_RECENT5,
	ID_RECENT6,
	ID_RECENT7,
	ID_RECENT8,
	ID_RECENT9,
	ID_CLEAR_RECENT,
	
	ID_SAVESTATE0,
	ID_SAVESTATE1,
	ID_SAVESTATE2,
	ID_SAVESTATE3,
	ID_SAVESTATE4,
	ID_SAVESTATE5,
	ID_SAVESTATE6,
	ID_SAVESTATE7,
	ID_SAVESTATE8,
	ID_SAVESTATE9,
	
	ID_LOADSTATE0,
	ID_LOADSTATE1,
	ID_LOADSTATE2,
	ID_LOADSTATE3,
	ID_LOADSTATE4,
	ID_LOADSTATE5,
	ID_LOADSTATE6,
	ID_LOADSTATE7,
	ID_LOADSTATE8,
	ID_LOADSTATE9,
	
	ID_SETTINGSFRAME,
    ID_RENDERMETHOD,
	ID_GREENSCALE,
	ID_WINZOOM,
	ID_SOUND_ENABLED,
	ID_SOUND_SR,
	ID_TEXTCTRL_UP,
	ID_TEXTCTRL_DOWN,
	ID_TEXTCTRL_LEFT,
	ID_TEXTCTRL_RIGHT,
	ID_TEXTCTRL_A,
	ID_TEXTCTRL_B,
	ID_TEXTCTRL_SELECT,
	ID_TEXTCTRL_START,
    
    ID_LANG_ENGLISH,
    ID_LANG_SPANISH
};

#endif
