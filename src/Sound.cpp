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

#include <stdlib.h>
#include "Sound.h"

static void handle_error( const char* str )
{
	if ( str )
	{
		fprintf( stderr, "Error: %s\n", str );
		exit( EXIT_FAILURE );
	}
}

Sound::Sound()
{
	long const sample_rate = 44100;
	
	if ( SDL_Init( SDL_INIT_AUDIO ) < 0 )
		//return EXIT_FAILURE;
		return;
	atexit( SDL_Quit );
	
	// Set sample rate and check for out of memory error
	handle_error( apu.set_sample_rate( sample_rate ) );
	
	// Generate a few seconds of sound and play using SDL
	
	handle_error( sound.start( sample_rate, 2 ) );
}

Sound::~Sound()
{

}

void Sound::EndFrame()
{
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
