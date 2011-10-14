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

#include "SoundPortaudio.h"

#include <assert.h>
#include <string.h>

static bool paInitialized = false;

// Return current SDL_GetError() string, or str if SDL didn't have a string
static const char* sdl_error( const char* str )
{
	const char* sdl_str = SDL_GetError();
	if ( sdl_str && *sdl_str )
		str = sdl_str;
	return str;
}

SoundPortaudio::SoundPortaudio()
{
	bufs = NULL;
	free_sem = NULL;
	sound_open = false;
    fullBuffers = 0;
}

SoundPortaudio::~SoundPortaudio()
{
	stop();
}

const char* SoundPortaudio::start( long sample_rate, int chan_count )
{
	assert( !bufs ); // can only be initialized once
	
	write_buf = 0;
	write_pos = 0;
	read_buf = 0;
    fullBuffers = 0;
	
	bufs = new sample_t [(long) buf_size * buf_count];
	if ( !bufs )
		return "Out of memory";
	currently_playing_ = bufs;
	
	//free_sem = SDL_CreateSemaphore( buf_count - 1 );
	//if ( !free_sem )
	//	return sdl_error( "Couldn't create semaphore" );
	
    if (!paInitialized)
    {
        PaError err = Pa_Initialize();
        if( err != paNoError )
            printf("PortAudio error: %s\n", Pa_GetErrorText( err ));
        
        paInitialized = true;
    }
    
	PaError err;
    
    /* Open an audio I/O stream. */
    err = Pa_OpenDefaultStream( &stream,
                               0,          /* no input channels */
                               2,          /* stereo output */
                               paInt16,     /* 8 bit floating point output */
                               sample_rate,
                               buf_size / chan_count,        /* frames per buffer, i.e. the number
                                            of sample frames that PortAudio will
                                            request from the callback. Many apps
                                            may want to use
                                            paFramesPerBufferUnspecified, which
                                            tells PortAudio to pick the best,
                                            possibly changing, buffer size.*/
                               fill_buffer_, /* this is your callback function */
                               this ); /*This is a pointer that will be passed to
                                         your callback*/
    if( err != paNoError )
        printf("PortAudio error: %s\n", Pa_GetErrorText( err ));
    
    err = Pa_StartStream( stream );
    if( err != paNoError )
        printf("PortAudio error: %s\n", Pa_GetErrorText( err ));
    
	sound_open = true;
	
	return NULL;
}

void SoundPortaudio::stop()
{
	if ( sound_open )
	{
		sound_open = false;
        
        PaError err;
        
		err = Pa_StopStream( stream );
        if( err != paNoError )
            printf("PortAudio error: %s\n", Pa_GetErrorText( err ));
	}
	
	//if ( free_sem )
	//{
	//	SDL_DestroySemaphore( free_sem );
	//	free_sem = NULL;
	//}
	
	delete [] bufs;
	bufs = NULL;
}

int SoundPortaudio::sample_count() const
{
	int buf_free = SDL_SemValue( free_sem ) * buf_size + (buf_size - write_pos);
	return buf_size * buf_count - buf_free;
}

inline SoundPortaudio::sample_t* SoundPortaudio::buf( int index )
{
	assert( (unsigned) index < buf_count );
	return bufs + (long) index * buf_size;
}

void SoundPortaudio::write( const sample_t* in, int count )
{
	while ( count )
	{
		// n = espacio disponible en el buffer actual
		int n = buf_size - write_pos;
		// si es mayor a lo que queremos copiar
		// n = count
		if ( n > count )
			n = count;
		
		// copiar en el buffer actual (write_buf) n samples
		memcpy( buf( write_buf ) + write_pos, in, n * sizeof (sample_t) );
		in += n;
		write_pos += n;
		count -= n;
		
		// si el buffer ya está lleno
		if ( write_pos >= buf_size )
		{
			write_pos = 0;
			// seleccionar el buffer siguiente
			write_buf = (write_buf + 1) % buf_count;
			// si todos los buffers estan a la espera de ser
			// reproducidos se suspende este hilo
			//SDL_SemWait( free_sem );
            fullBuffers++;
            if (fullBuffers > buf_count)
                fullBuffers = buf_count;
		}
		
		// si aun no se han copiado todos los datos se hace
		// una nueva pasada
	}
}

int SoundPortaudio::fill_buffer(void *outputBuffer,
                                 unsigned long framesPerBuffer,
                                 const PaStreamCallbackTimeInfo* timeInfo,
                                 PaStreamCallbackFlags statusFlags)
{
    sample_t *out = (sample_t*)outputBuffer;
    
	// si hay por lo menos un buffer para reproducir
	if ( fullBuffers > 0 )
    //if ( SDL_SemValue( free_sem ) < buf_count - 1 )
	{
		currently_playing_ = buf( read_buf );
		memcpy( out, buf( read_buf ), framesPerBuffer*sizeof(sample_t) );
		read_buf = (read_buf + 1) % buf_count;
		//SDL_SemPost( free_sem );
        fullBuffers--;
	}
	// si no hay nada reproducir silencio
	else
	{
		memset(out, 0, framesPerBuffer*sizeof(sample_t));
	}
    
    return 0;
}

int SoundPortaudio::fill_buffer_( const void *inputBuffer, void *outputBuffer,
                                                    unsigned long framesPerBuffer,
                                                    const PaStreamCallbackTimeInfo* timeInfo,
                                                    PaStreamCallbackFlags statusFlags,
                                                    void *userData )
{
	return ((SoundPortaudio*) userData)->fill_buffer(outputBuffer,
                                              framesPerBuffer, timeInfo,
                                              statusFlags);
}

