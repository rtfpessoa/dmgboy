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

#include "Def.h"

class Basic_Gb_Apu;
class Sound_Queue;

class Sound
{
private:
	Basic_Gb_Apu * apu;
	Sound_Queue * sound;
	bool initialized;
	bool enabled;
	long sampleRate;
	
	int HandleError( const char* str );
public:
	Sound();
	~Sound();
	
	int ChangeSampleRate(long newSampleRate);
	int Start();
	int Stop();
	bool GetEnabled();
	void SetEnabled(bool enabled);
	void WriteRegister(WORD dir, BYTE value);
	BYTE ReadRegister(WORD dir);
	void EndFrame();
};

#endif
