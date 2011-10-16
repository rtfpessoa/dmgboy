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

#include <assert.h>
#include <string.h>
#include "SoundSDL.h"

static bool sdlInitialized = false;

SoundSDL::SoundSDL()
{
	bufs = NULL;
	semaphore = NULL;
	soundOpen = false;
}

SoundSDL::~SoundSDL()
{
	Stop();
}

bool SoundSDL::Start(long sampleRate, int numChannels)
{
	assert( !bufs ); // can only be initialized once
	
	writeBuf = 0;
	writePos = 0;
	readBuf = 0;
	
	bufs = new short[(long) bufSize * numBuffers];
	if ( !bufs )
		return false;
	
	semaphore = SDL_CreateSemaphore(numBuffers - 1);
	if (!semaphore)
		return false;
	
    if (!sdlInitialized)
    {
        if ( SDL_Init( SDL_INIT_AUDIO ) < 0 )
            return false;
        
        atexit(SDL_Quit);
        
        sdlInitialized = true;
    }
    
	SDL_AudioSpec as;
	as.freq = sampleRate;
	as.format = AUDIO_S16SYS;
	as.channels = numChannels;
	as.silence = 0;
	// como el buffer en estereo necesita el doble de datos
	// que en mono, se divide el tamaño por el numero de
	// canales para obtener el numero de samples 
	as.samples = bufSize / numChannels;
	as.size = 0;
	as.callback = SDLCallback;
	as.userdata = this;
	if (SDL_OpenAudio( &as, NULL ) < 0)
		return false;
	SDL_PauseAudio(false);
	soundOpen = true;
	
	return true;
}

void SoundSDL::Stop()
{
	if (soundOpen)
	{
		soundOpen = false;
		SDL_PauseAudio( true );
		SDL_CloseAudio();
	}
	
	if (semaphore)
	{
		SDL_DestroySemaphore(semaphore);
		semaphore = NULL;
	}
	
	delete [] bufs;
	bufs = NULL;
}

inline short* SoundSDL::GetBufPtr(int index)
{
	assert((unsigned) index < numBuffers);
	return bufs + (long) index * bufSize;
}

void SoundSDL::Write(const short * in, int count)
{
	while (count)
	{
		// n = espacio disponible en el buffer actual
		int n = bufSize - writePos;
		// si es mayor a lo que queremos copiar
		// n = count
		if (n > count)
			n = count;
		
		// copiar en el buffer actual (write_buf) n samples
		memcpy(GetBufPtr(writeBuf) + writePos, in, n * sizeof (short));
		in += n;
		writePos += n;
		count -= n;
		
		// si el buffer ya está lleno
		if (writePos >= bufSize)
		{
			writePos = 0;
			// seleccionar el buffer siguiente
			writeBuf = (writeBuf + 1) % numBuffers;
			// si todos los buffers estan a la espera de ser
			// reproducidos se suspende este hilo
			SDL_SemWait(semaphore);
		}
		
		// si aun no se han copiado todos los datos se hace
		// una nueva pasada
	}
}

void SoundSDL::FillBuffer(Uint8* out, int count)
{
	// si hay por lo menos un buffer para reproducir
	if ( SDL_SemValue(semaphore) < numBuffers - 1 )
	{
		memcpy(out, GetBufPtr(readBuf), count);
		readBuf = (readBuf + 1) % numBuffers;
		SDL_SemPost(semaphore);
	}
	// si no hay nada reproducir silencio
	else
	{
		memset( out, 0, count );
	}
}

void SoundSDL::SDLCallback(void* user_data, Uint8* out, int count)
{
	((SoundSDL*) user_data)->FillBuffer(out, count);
}

