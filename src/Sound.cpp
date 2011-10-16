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
// Definir la siguiente linea para que en Visual Studio no haya conflicto
// entre SDL y GB_Snd_Emu al definir tipos basicos
#define BLARGG_COMPILER_HAS_NAMESPACE 1
//#include "SoundPortaudio.h"
#include "SoundSDL.h"
#include "Basic_Gb_Apu.h"
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
	enabled = false;
	initialized = true;
	sampleRate = 44100;//22050;
    
    //sound = new Sound_Queue();
    sound = new SoundSDL();
    apu = new Basic_Gb_Apu();
	
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
    delete sound;
    delete apu;
}

int Sound::ChangeSampleRate(long newSampleRate)
{
	if (!initialized)
		return NO_ERROR;
	
	sampleRate = newSampleRate;
	bool wasEnabled = enabled;
	
	if (wasEnabled)
		Stop();
	
	// Set sample rate and check for out of memory error
	if (HandleError( apu->set_sample_rate(sampleRate) ) == ERROR)
		return ERROR;
	
	if (wasEnabled)
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
	
	if (!enabled)
	{
		// Generate a few seconds of sound and play using SDL
		if (sound->Start(sampleRate, 2) == false)
			return ERROR;
	}
	enabled = true;
	
	return NO_ERROR;
}

int Sound::Stop()
{
	if (!initialized)
		return NO_ERROR;
	
	if (enabled)
		sound->Stop();
	
	enabled = false;
	
	return NO_ERROR;
}

bool Sound::GetEnabled()
{
	return enabled;
}

void Sound::SetEnabled(bool enabled)
{
	if (enabled)
		Start();
	else
		Stop();
}

void Sound::EndFrame()
{
	if ((!initialized) || (!enabled))
		return;
	
	apu->end_frame();
	
	int const bufSize = apu->samples_avail();
	blip_sample_t buf [bufSize];
	
    // Play whatever samples are available
    long count = apu->read_samples(buf, bufSize);
    
    sound->Write(buf, count);
}
void Sound::WriteRegister(WORD dir, BYTE value)
{
    if (enabled) apu->write_register(dir, value);
}

BYTE Sound::ReadRegister(WORD dir)
{
    return enabled ? apu->read_register(dir) : 0;
}
