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

#ifndef SOUNDPORTAUDIO_H
#define SOUNDPORTAUDIO_H

#include "portaudio.h"

class wxSemaphore;
class wxMutex;

// Simple Portaudio sound wrapper that has a synchronous interface
class SoundPortaudio {
public:
	SoundPortaudio();
	~SoundPortaudio();
	
	// Initialize with specified sample rate and number of channels.
	// Returns true on success, otherwise false.
	bool Start(long sampleRate, int numChannels);
	
	// Write samples to buffer and block until enough space is available
	void Write( const short * data, int count);
	
	// Stop audio output
	void Stop();
	
private:
    PaStream *stream;
    
    enum { bufSize = 2048 };
	enum { numBuffers = 10 };
	short* volatile bufs;
	wxSemaphore* volatile semaphore;
    wxMutex* mutex;
	int volatile readBuf;
	int writeBuf;	// id del buffer actual
	int writePos;
    int fullBuffers;
	bool soundOpen;
	
	short* GetBufPtr(int index);
	int FillBuffer(void *outputBuffer, unsigned long framesPerBuffer);
    
	static int PortaudioCallback( const void *inputBuffer, void *outputBuffer,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void *userData );
};

#endif

