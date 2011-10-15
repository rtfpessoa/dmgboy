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
#include "SoundPortaudio.h"

static bool paInitialized = false;

SoundPortaudio::SoundPortaudio()
{
	bufs = NULL;
	free_sem = NULL;
	soundOpen = false;
    fullBuffers = 0;
}

SoundPortaudio::~SoundPortaudio()
{
	Stop();
}

bool SoundPortaudio::Start(long sampleRate, int numChannels)
{
	assert( !bufs ); // can only be initialized once
	
    PaError err;
	writeBuf = 0;
	writePos = 0;
	readBuf = 0;
    fullBuffers = 0;
	
	bufs = new short[(long) bufSize * numBuffers];
	if (!bufs)
		return false;
	
	//free_sem = SDL_CreateSemaphore( buf_count - 1 );
	//if ( !free_sem )
	//	return sdl_error( "Couldn't create semaphore" );
	
    if (!paInitialized)
    {
        err = Pa_Initialize();
        if( err != paNoError )
        {
            printf("PortAudio error: %s\n", Pa_GetErrorText( err ));
            return false;
        }
        
        paInitialized = true;
    }
    
    /* Open an audio I/O stream. */
    err = Pa_OpenDefaultStream( &stream,
                               0,           /* no input channels */
                               numChannels, /* mono, stereo, ..., output */
                               paInt16,     /* 16 bits output */
                               sampleRate,
                               bufSize / numChannels, /* frames per buffer, i.e. the number
                                            of sample frames that PortAudio will
                                            request from the callback. Many apps
                                            may want to use
                                            paFramesPerBufferUnspecified, which
                                            tells PortAudio to pick the best,
                                            possibly changing, buffer size.*/
                               PortaudioCallback, /* this is your callback function */
                               this ); /*This is a pointer that will be passed to
                                         your callback*/
    if( err != paNoError )
    {
        printf("PortAudio error: %s\n", Pa_GetErrorText( err ));
        return false;
    }
    
    err = Pa_StartStream( stream );
    if( err != paNoError )
    {
        printf("PortAudio error: %s\n", Pa_GetErrorText( err ));
        return false;
    }
    
	soundOpen = true;
	
	return true;
}

void SoundPortaudio::Stop()
{
	if (soundOpen)
	{
		soundOpen = false;
        
        PaError err;
        
		err = Pa_StopStream(stream);
        if(err != paNoError)
            printf("PortAudio error: %s\n", Pa_GetErrorText(err));
	}
	
	//if ( free_sem )
	//{
	//	SDL_DestroySemaphore( free_sem );
	//	free_sem = NULL;
	//}
	
    if (bufs)
    {
        delete [] bufs;
        bufs = NULL;
    }
}

inline short* SoundPortaudio::buf( int index )
{
	assert( (unsigned) index < numBuffers );
	return bufs + (long) index * bufSize;
}

void SoundPortaudio::Write(const short* in, int count)
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
		memcpy( buf( writeBuf ) + writePos, in, n * sizeof(short));
		in += n;
		writePos += n;
		count -= n;
		
		// si el buffer ya está lleno
		if (writePos >= bufSize )
		{
			writePos = 0;
			// seleccionar el buffer siguiente
			writeBuf = (writeBuf + 1) % numBuffers;
			// si todos los buffers estan a la espera de ser
			// reproducidos se suspende este hilo
			//SDL_SemWait( free_sem );
            fullBuffers++;
            if (fullBuffers > numBuffers)
            {
                fullBuffers = numBuffers;
                printf("Sobreescribiendo buffer\n");
            }
            //printf("write: fullBuffers: %d\n", fullBuffers);
		}
		
		// si aun no se han copiado todos los datos se hace
		// una nueva pasada
	}
}

int SoundPortaudio::FillBuffer(void *outputBuffer, unsigned long framesPerBuffer)
{
    short * out = (short *)outputBuffer;
    size_t copyBytes = framesPerBuffer*2 * sizeof(short);
    
    // si hay por lo menos un buffer para reproducir
	if ( fullBuffers > 0 )
    //if ( SDL_SemValue( free_sem ) < buf_count - 1 )
	{
		memcpy( out, buf(readBuf), copyBytes);
		readBuf = (readBuf + 1) % numBuffers;
		//SDL_SemPost( free_sem );
        fullBuffers--;
	}
	// si no hay nada reproducir silencio
	else
	{
		memset(out, 0, copyBytes);
	}
    
    //printf("fill: fullBuffers: %d\n", fullBuffers);
    
    return 0;
}

int SoundPortaudio::PortaudioCallback( const void *inputBuffer, void *outputBuffer,
                                        unsigned long framesPerBuffer,
                                        const PaStreamCallbackTimeInfo* timeInfo,
                                        PaStreamCallbackFlags statusFlags,
                                        void *userData )
{
	return ((SoundPortaudio*) userData)->FillBuffer(outputBuffer, framesPerBuffer);
}

