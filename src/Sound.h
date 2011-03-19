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

#ifndef __SOUND_H__
#define __SOUND_H__

#include "Basic_Gb_Apu.h"
#include "Sound_Queue.h"
#include "Def.h"

class Sound
{
private:
	Basic_Gb_Apu apu;
	Sound_Queue sound;
public:
	Sound();
	~Sound();
	
	inline void WriteRegister(WORD dir, BYTE value){ apu.write_register( dir, value ); }
	inline BYTE ReadRegister(WORD dir){ return apu.read_register( dir ); }
	void EndFrame();
};

#endif
