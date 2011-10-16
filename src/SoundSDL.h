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

#ifndef SOUND_QUEUE_H
#define SOUND_QUEUE_H

#include "SDL.h"

// Simple SDL sound wrapper that has a synchronous interface
class SoundSDL {
public:
	SoundSDL();
	~SoundSDL();
	
	// Initialize with specified sample rate and channel count.
	// Returns true on success, otherwise false.
	bool Start(long sampleRate, int numChannels);
	
	// Write samples to buffer and block until enough space is available
	void Write( const short * data, int count);
	
	// Stop audio output
	void Stop();
	
private:
	enum { bufSize = 1024 };
	enum { numBuffers = 10 };
	short * volatile bufs;
	SDL_sem* volatile semaphore;
	int volatile readBuf;
	int writeBuf;	// id del buffer actual
	int writePos;
	bool soundOpen;
	
	short * GetBufPtr(int index);
	void FillBuffer(Uint8*, int);
	static void SDLCallback(void*, Uint8*, int);
};

#endif

