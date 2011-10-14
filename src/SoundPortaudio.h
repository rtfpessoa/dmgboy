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

#ifndef SOUNDPORTAUDIO_H
#define SOUNDPORTAUDIO_H

#include "SDL.h"
#include "portaudio.h"

// Simple SDL sound wrapper that has a synchronous interface
class SoundPortaudio {
public:
	SoundPortaudio();
	~SoundPortaudio();
	
	// Initialize with specified sample rate and channel count.
	// Returns NULL on success, otherwise error string.
	const char* start( long sample_rate, int chan_count = 1 );
	
	// Number of samples in buffer waiting to be played
	int sample_count() const;
	
	// Write samples to buffer and block until enough space is available
	typedef short sample_t;
	void write( const sample_t*, int count );
	
	// Pointer to samples currently playing (for showing waveform display)
	sample_t const* currently_playing() const { return currently_playing_; }
	
	// Stop audio output
	void stop();
	
private:
    PaStream *stream;
    
    enum { buf_size = 1024 };
	enum { buf_count = 3 };
	sample_t* volatile bufs;
	SDL_sem* volatile free_sem;
	sample_t* volatile currently_playing_;
	int volatile read_buf;
	int write_buf;	// id del buffer actual
	int write_pos;
    int fullBuffers;
	bool sound_open;
	
	sample_t* buf( int index );
	int fill_buffer(  void *outputBuffer,
                       unsigned long framesPerBuffer,
                       const PaStreamCallbackTimeInfo* timeInfo,
                       PaStreamCallbackFlags statusFlags);
    
	static int fill_buffer_( const void *inputBuffer, void *outputBuffer,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void *userData );
};

#endif

