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

#ifndef __PAD_H__
#define __PAD_H__

#include <SDL.h>
#include "Def.h"

//enum e_keyConf
//{
//	kR = SDLK_RIGHT,
//	kL = SDLK_LEFT,
//	kU = SDLK_UP,
//	kD = SDLK_DOWN,
//	kA = SDLK_a,
//	kB = SDLK_s,
//	kSE = SDLK_q,
//	kST = SDLK_w
//};

enum e_joypad {jRIGHT, jLEFT, jUP, jDOWN, jA, jB, jSELECT, jSTART};

int onCheckKeyPad(int * valueP1);
int updateInput(int valueP1);

#endif
