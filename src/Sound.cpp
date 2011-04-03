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

#include <iostream>
#include "Sound.h"

using namespace std;

enum SoundError { ERROR, NO_ERROR };

int Sound::HandleError( const char* str )
{
	if ( str )
	{
		cerr << "Error: " << str << endl;
		return ERROR;
	}
	else
		return NO_ERROR;
}

Sound::Sound()
{
	soundOn = false;
	initialized = true;
	sampleRate = 44100;//22050;
	
	if ( SDL_Init( SDL_INIT_AUDIO ) < 0 )
	{
		initialized = false;
		return;
	}
	
	atexit( SDL_Quit );
	
	if (ChangeSampleRate(sampleRate) == ERROR)
	{
		initialized = false;
		return;
	}
	
	if (Start() == ERROR)
	{
		initialized = false;
		return;
	}
}

Sound::~Sound()
{

}

int Sound::ChangeSampleRate(long newSampleRate)
{
	if (!initialized)
		return NO_ERROR;
	
	sampleRate = newSampleRate;
	bool soundWasOn = soundOn;
	
	if (soundWasOn)
		Stop();
	
	// Set sample rate and check for out of memory error
	if (HandleError( apu.set_sample_rate(sampleRate) ) == ERROR)
		return ERROR;
	
	if (soundWasOn)
	{
		if (Start() == ERROR)
			return ERROR;
	}
	
	return NO_ERROR;
}

int Sound::Start()
{
	if (!initialized)
		return NO_ERROR;
	
	if (!soundOn)
	{
		// Generate a few seconds of sound and play using SDL
		if (HandleError( sound.start(sampleRate, 2) ) == ERROR)
			return ERROR;
	}
	soundOn = true;
	
	return NO_ERROR;
}

int Sound::Stop()
{
	if (!initialized)
		return NO_ERROR;
	
	if (soundOn)
		sound.stop();
	
	soundOn = false;
	
	return NO_ERROR;
}

void Sound::SetState(bool enabled)
{
	if (enabled)
		Start();
	else
		Stop();
}

void Sound::EndFrame()
{
	if ((!initialized) || (!soundOn))
		return;
	
	apu.end_frame();
	
	int const buf_size = 2048;
	static blip_sample_t buf [buf_size];
	
	if ( apu.samples_avail() >= buf_size )
	{
		// Play whatever samples are available
		long count = apu.read_samples( buf, buf_size );
		sound.write( buf, count );
	}
}
