/*
 *  Copyright © 2006-2012 SplinterGU (Fenix/Bennugd)
 *  Copyright © 2002-2006 Fenix Team (Fenix)
 *  Copyright © 1999-2002 José Luis Cebrián Pagüe (Fenix)
 *
 *  This file is part of Bennu - Game Development
 *
 *  This software is provided 'as-is', without any express or implied
 *  warranty. In no event will the authors be held liable for any damages
 *  arising from the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose,
 *  including commercial applications, and to alter it and redistribute it
 *  freely, subject to the following restrictions:
 *
 *     1. The origin of this software must not be misrepresented; you must not
 *     claim that you wrote the original software. If you use this software
 *     in a product, an acknowledgment in the product documentation would be
 *     appreciated but is not required.
 *
 *     2. Altered source versions must be plainly marked as such, and must not be
 *     misrepresented as being the original software.
 *
 *     3. This notice may not be removed or altered from any source
 *     distribution.
 *
 */

#ifdef _MSC_VER
#pragma comment (lib, "SDL_mixer")
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bgddl.h"

#include <SDL.h>

#include "SDL_mixer.h"

#include "files.h"
#include "xstrings.h"

#include "dlvaracc.h"

#include "bgload.h"

/* --------------------------------------------------------------------------- */

static int audio_initialized = 0 ;

/* --------------------------------------------------------------------------- */

#define SOUND_FREQ              0
#define SOUND_MODE              1
#define SOUND_CHANNELS          2

/* --------------------------------------------------------------------------- */
/* Definicion de constantes (usada en tiempo de compilacion)                   */

DLCONSTANT  __bgdexport( mod_sound, constants_def )[] =
{
    { "MODE_MONO"   , TYPE_INT, 0  },
    { "MODE_STEREO" , TYPE_INT, 1  },
    { "ALL_SOUND"   , TYPE_INT, -1 },
    { NULL          , 0       , 0  }
} ;

/* --------------------------------------------------------------------------- */
/* Definicion de variables globales (usada en tiempo de compilacion)           */

char * __bgdexport( mod_sound, globals_def ) =
    "   sound_freq = 22050 ;\n"
    "   sound_mode = MODE_STEREO ;\n"
    "   sound_channels = 8 ;\n";

/* --------------------------------------------------------------------------- */
/* Son las variables que se desea acceder.                                     */
/* El interprete completa esta estructura, si la variable existe.              */
/* (usada en tiempo de ejecucion)                                              */

DLVARFIXUP  __bgdexport( mod_sound, globals_fixup )[] =
{
    /* Nombre de variable global, puntero al dato, tamaño del elemento, cantidad de elementos */
    { "sound_freq", NULL, -1, -1 },
    { "sound_mode", NULL, -1, -1 },
    { "sound_channels", NULL, -1, -1 },
    { NULL, NULL, -1, -1 }
};

/* ------------------------------------- */
/* Interfaz SDL_RWops Bennu              */
/* ------------------------------------- */

static Sint64 SDLCALL __modsound_seek_cb( SDL_RWops *context, Sint64 offset, int whence )
{
    if ( file_seek( context->hidden.unknown.data1, offset, whence ) < 0 ) return ( -1 );
    return( file_pos( context->hidden.unknown.data1 ) );
    //    return ( file_seek( context->hidden.unknown.data1, offset, whence ) );
}

static size_t SDLCALL __modsound_read_cb( SDL_RWops *context, void *ptr, size_t size, size_t maxnum )
{
    int ret = file_read( context->hidden.unknown.data1, ptr, size * maxnum );
    if ( ret > 0 ) ret /= size;
    return( ret );
}

static size_t SDLCALL __modsound_write_cb( SDL_RWops *context, const void *ptr, size_t size, size_t num )
{
    int ret = file_write( context->hidden.unknown.data1, ( void * )ptr, size * num );
    if ( ret > 0 ) ret /= size;
    return( ret );
}

static int SDLCALL __modsound_close_cb( SDL_RWops *context )
{
    if ( context )
    {
        file_close( context->hidden.unknown.data1 );
        SDL_FreeRW( context );
    }
    return( 0 );
}

static SDL_RWops *SDL_RWFromBGDFP( file *fp )
{
    SDL_RWops *rwops = SDL_AllocRW();
    if ( rwops != NULL )
    {
        rwops->seek = __modsound_seek_cb;
        rwops->read = __modsound_read_cb;
        rwops->write = __modsound_write_cb;
        rwops->close = __modsound_close_cb;
        rwops->hidden.unknown.data1 = fp;
    }
    return( rwops );
}

/* --------------------------------------------------------------------------- */

/*
 *  FUNCTION : sound_init
 *
 *  Set the SDL_Mixer library
 *
 *  PARAMS:
 *      no params
 *
 *  RETURN VALUE:
 *
 *  no return
 *
 */

static int sound_init()
{
    int audio_rate;
    Uint16 audio_format;
    int audio_channels;
    int audio_buffers;
    int audio_mix_channels;

    if ( !audio_initialized )
    {
        /* Initialize variables: but limit quality to some fixed options */

        audio_rate = GLODWORD( mod_sound, SOUND_FREQ );

        if ( audio_rate > 22050 )
            audio_rate = 44100;
        else if ( audio_rate > 11025 )
            audio_rate = 22050;
        else
            audio_rate = 11025;

#ifdef TARGET_WII
        /* WII uses a powerpc architecture, which is big-endian */
        audio_format = AUDIO_S16MSB;
        audio_rate = 48000;
#else
        audio_format = AUDIO_S16;
#endif
        audio_channels = GLODWORD( mod_sound, SOUND_MODE ) + 1;
#ifdef TARGET_WII
        audio_buffers = 1024;
#else
        audio_buffers = 1024 * audio_rate / 22050;
#endif

        /* Open the audio device */
        if ( Mix_OpenAudio( audio_rate, audio_format, audio_channels, audio_buffers ) >= 0 )
        {
            GLODWORD( mod_sound, SOUND_CHANNELS ) <= 32 ? Mix_AllocateChannels( GLODWORD( mod_sound, SOUND_CHANNELS ) ) : Mix_AllocateChannels( 32 ) ;
            Mix_QuerySpec( &audio_rate, &audio_format, &audio_channels );
            audio_mix_channels = Mix_AllocateChannels( -1 ) ;
            GLODWORD( mod_sound, SOUND_CHANNELS ) = audio_mix_channels ;

            audio_initialized = 1;
            return 0;
        }
    }

    fprintf( stderr, "[SOUND] Couldn't initialize sound: %s\n", SDL_GetError() ) ;
    audio_initialized = 0;
    return -1 ;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : sound_close
 *
 *  Close all the audio set
 *
 *  PARAMS:
 *      no params
 *
 *  RETURN VALUE:
 *
 *  no return
 *
 */

static void sound_close()
{
    if ( !audio_initialized ) return;

    //falta por comprobar que todo esté descargado

    Mix_CloseAudio();

    audio_initialized = 0;
}


/* ------------------ */
/* Sonido MOD y OGG   */
/* ------------------ */

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : load_song
 *
 *  Load a MOD/OGG from a file
 *
 *  PARAMS:
 *      file name
 *
 *  RETURN VALUE:
 *
 *  mod pointer
 *
 */

static int load_song( const char * filename )
{
    Mix_Music *music = NULL;
    file      *fp;

    if ( !audio_initialized && sound_init() ) return ( 0 );

    if ( !( fp = file_open( filename, "rb0" ) ) ) return ( 0 );

    SDL_RWops * rwops = SDL_RWFromBGDFP( fp );
    if ( !rwops ) {
        file_close( fp );
        return( 0 );
    }

    // Don't need free rwops, SDL will do
    if ( !( music = Mix_LoadMUS_RW( rwops, 1 ) ) ) return ( 0 );

    return (( int )music );
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : play_song
 *
 *  Play a MOD/OGG
 *
 *  PARAMS:
 *      mod pointer
 *      number of loops (-1 infinite loops)
 *
 *  RETURN VALUE:
 *
 * -1 if there is any error
 *
 */

static int play_song( int id, int loops )
{
//    SDL_Log("Called play_song();");
    if ( audio_initialized && id )
    {
        int result = Mix_PlayMusic(( Mix_Music * )id, loops );
        if ( result == -1 ) fprintf( stderr, "%s", Mix_GetError() );
        return result;
    }

    fprintf( stderr, "Play song called with invalid handle" );
    return( -1 );
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : fade_music_in
 *
 *  Play a MOD/OGG fading in it
 *
 *  PARAMS:
 *      mod pointer
 *      number of loops (-1 infinite loops)
 *      ms  microsends of fadding
 *
 *  RETURN VALUE:
 *
 * -1 if there is any error
 *
 */

static int fade_music_in( int id, int loops, int ms )
{
    if ( audio_initialized && id ) return( Mix_FadeInMusic(( Mix_Music * )id, loops, ms ) );
    return( -1 );
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : fade_music_off
 *
 *  Stop the play of a mod
 *
 *  PARAMS:
 *
 *  ms  microsends of fadding
 *
 *  RETURN VALUE:
 *
 * -1 if there is any error
 *
 */

static int fade_music_off( int ms )
{
    if ( !audio_initialized ) return ( 0 );
    return ( Mix_FadeOutMusic( ms ) );
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : unload_song
 *
 *  Play a MOD
 *
 *  PARAMS:
 *
 *  mod id
 *
 *  RETURN VALUE:
 *
 * -1 if there is any error
 *
 */

static int unload_song( int id )
{
    if ( audio_initialized && id )
    {
        if ( Mix_PlayingMusic() ) Mix_HaltMusic();
        Mix_FreeMusic(( Mix_Music * )id );
    }
    return ( 0 ) ;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : stop_song
 *
 *  Stop the play of a mod
 *
 *  PARAMS:
 *
 *  no params
 *
 *  RETURN VALUE:
 *
 * -1 if there is any error
 *
 */

static int stop_song( void )
{
    if ( audio_initialized ) Mix_HaltMusic();
    return ( 0 ) ;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : pause_song
 *
 *  Pause the mod in curse, you can resume it after
 *
 *  PARAMS:
 *
 *  no params
 *
 *  RETURN VALUE:
 *
 * -1 if there is any error
 *
 */

static int pause_song( void )
{
    if ( audio_initialized ) Mix_PauseMusic();
    return ( 0 ) ;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : resume_song
 *
 *  Resume the mod, paused before
 *
 *  PARAMS:
 *
 *  no params
 *
 *  RETURN VALUE:
 *
 * -1 if there is any error
 *
 */

static int resume_song( void )
{
    if ( audio_initialized ) Mix_ResumeMusic();
    return( 0 ) ;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : is_playing_song
 *
 *  Check if there is any mod playing
 *
 *  PARAMS:
 *
 *  no params
 *
 *  RETURN VALUE:
 *
 * -1 if there is any error
 *  TRUE OR FALSE if there is no error
 *
 */

static int is_playing_song( void )
{
    if ( !audio_initialized ) return ( 0 );
    return Mix_PlayingMusic();
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : set_song_volume
 *
 *  Set the volume for mod playing (0-128)
 *
 *  PARAMS:
 *
 *  int volume
 *
 *  RETURN VALUE:
 *
 * -1 if there is any error
 *  0 if there is no error
 *
 */

static int set_song_volume( int volume )
{
    if ( !audio_initialized && sound_init() ) return ( -1 );

    if ( volume < 0 ) volume = 0;
    if ( volume > 128 ) volume = 128;

    Mix_VolumeMusic( volume );
    return 0;
}

/* ------------ */
/* Sonido WAV   */
/* ------------ */

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : load_wav
 *
 *  Load a WAV from a file
 *
 *  PARAMS:
 *      file name
 *
 *  RETURN VALUE:
 *
 *  wav pointer
 *
 */

static int load_wav( const char * filename )
{
    Mix_Chunk *music = NULL;
    file      *fp;

    if ( !audio_initialized && sound_init() ) return ( 0 );

    if ( !( fp = file_open( filename, "rb0" ) ) ) return ( 0 );

    SDL_RWops * rwops = SDL_RWFromBGDFP( fp );
    if ( !rwops ) {
        file_close( fp );
        return( 0 );
    }

    // Don't need free rwops, SDL will do
    if ( !( music = Mix_LoadWAV_RW( rwops, 1 ) ) ) return ( 0 );

    return (( int )music );
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : play_wav
 *
 *  Play a WAV
 *
 *  PARAMS:
 *      wav pointer;
 *      number of loops (-1 infinite loops)
 *      channel (-1 any channel)
 *
 *  RETURN VALUE:
 *
 * -1 if there is any error
 *  else channel where the music plays
 *
 */

static int play_wav( int id, int loops, int channel )
{
    if ( audio_initialized && id ) return ( ( int ) Mix_PlayChannel( channel, ( Mix_Chunk * )id, loops ) );
    return ( -1 );
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : unload_wav
 *
 *  Frees the resources from a wav, unloading it
 *
 *  PARAMS:
 *
 *  wav pointer
 *
 *  RETURN VALUE:
 *
 * -1 if there is any error
 *
 */

static int unload_wav( int id )
{
    if ( audio_initialized && id ) Mix_FreeChunk(( Mix_Chunk * )id );
    return ( 0 );
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : stop_wav
 *
 *  Stop a wav playing
 *
 *  PARAMS:
 *
 *  int channel
 *
 *  RETURN VALUE:
 *
 * -1 if there is any error
 *
 */

static int stop_wav( int canal )
{
    if ( audio_initialized && Mix_Playing( canal ) ) return( Mix_HaltChannel( canal ) );
    return ( -1 ) ;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : pause_wav
 *
 *  Pause a wav playing, you can resume it after
 *
 *  PARAMS:
 *
 *  int channel
 *
 *  RETURN VALUE:
 *
 * -1 if there is any error
 *
 */

static int pause_wav( int canal )
{
    if ( audio_initialized && Mix_Playing( canal ) )
    {
        Mix_Pause( canal );
        return ( 0 ) ;
    }
    return ( -1 ) ;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : resume_wav
 *
 *  Resume a wav playing, paused before
 *
 *  PARAMS:
 *
 *  int channel
 *
 *  RETURN VALUE:
 *
 * -1 if there is any error
 *
 */

static int resume_wav( int canal )
{
    if ( audio_initialized && Mix_Playing( canal ) )
    {
        Mix_Resume( canal );
        return ( 0 ) ;
    }
    return ( -1 ) ;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : is_playing_wav
 *
 *  Check a wav playing
 *
 *  PARAMS:
 *
 *  int channel
 *
 *  RETURN VALUE:
 *
 * -1 if there is any error
 *  TRUE OR FALSE if there is no error
 *
 */

static int is_playing_wav( int canal )
{
    if ( audio_initialized ) return( Mix_Playing( canal ) );
    return ( 0 );
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : set_wav_volume
 *
 *  Set the volume for wav playing (0-128) IN SAMPLE
 *
 *  PARAMS:
 *
 *  channel id
 *  int volume
 *
 *  RETURN VALUE:
 *
 * -1 if there is any error
 *
 */

static int  set_wav_volume( int sample, int volume )
{
    if ( !audio_initialized ) return ( -1 );

    if ( volume < 0 ) volume = 0;
    if ( volume > 128 ) volume = 128;

    if ( sample ) return( Mix_VolumeChunk(( Mix_Chunk * )sample, volume ) );

    return -1 ;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : set_channel_volume
 *
 *  Set the volume for wav playing (0-128) IN CHANNEL
 *
 *  PARAMS:
 *
 *  channel id
 *  int volume
 *
 *  RETURN VALUE:
 *
 * -1 if there is any error
 *
 */

static int  set_channel_volume( int canal, int volume )
{
    if ( !audio_initialized && sound_init() ) return ( -1 );

    if ( volume < 0 ) volume = 0;
    if ( volume > 128 ) volume = 128;

    return( Mix_Volume( canal, volume ) );
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : reserve_channels
 *
 *  Reserve the first channels (0 -> n-1) for the application, i.e. don't allocate
 *  them dynamically to the next sample if requested with a -1 value below.
 *
 *  PARAMS:
 *  number of channels to reserve.
 *
 *  RETURN VALUE:
 *  number of reserved channels.
 * -1 if there is any error
 *
 */

static int reserve_channels( int canales )
{
    if ( !audio_initialized && sound_init() ) return ( -1 );
    return Mix_ReserveChannels( canales );
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : set_panning
 *
 *  Set the panning for a wav channel
 *
 *  PARAMS:
 *
 *  channel
 *  left volume (0-255)
 *  right volume (0-255)
 *
 */

static int set_panning( int canal, int left, int right )
{
    if ( !audio_initialized && sound_init() ) return ( -1 );

    if ( Mix_Playing( canal ) )
    {
        Mix_SetPanning( canal, ( Uint8 )left, ( Uint8 )right );
        return ( 0 ) ;
    }
    return ( -1 ) ;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : set_position
 *
 *  Set the position of a channel. (angle) is an integer from 0 to 360
 *
 *  PARAMS:
 *
 *  channel
 *  angle (0-360)
 *  distance (0-255)
 *
 */

static int set_position( int canal, int angle, int dist )
{
    if ( !audio_initialized && sound_init() ) return ( -1 );

    if ( Mix_Playing( canal ) )
    {
        Mix_SetPosition( canal, ( Sint16 )angle, ( Uint8 )dist );
        return ( 0 ) ;
    }
    return ( -1 ) ;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : set_distance
 *
 *  Set the "distance" of a channel. (distance) is an integer from 0 to 255
 *  that specifies the location of the sound in relation to the listener.
 *
 *  PARAMS:
 *
 *  channel
 *
 *  distance (0-255)
 *
 */

static int set_distance( int canal, int dist )
{
    if ( !audio_initialized && sound_init() ) return ( -1 );

    if ( Mix_Playing( canal ) )
    {
        Mix_SetDistance( canal, ( Uint8 )dist );
        return ( 0 ) ;
    }

    return ( -1 ) ;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : reverse_stereo
 *
 *  Causes a channel to reverse its stereo.
 *
 *  PARAMS:
 *
 *  channel
 *  flip  0 normal  != reverse
 *
 */

static int reverse_stereo( int canal, int flip )
{
    if ( !audio_initialized && sound_init() ) return ( -1 );

    if ( Mix_Playing( canal ) )
    {
        Mix_SetReverseStereo( canal, flip );
        return ( 0 ) ;
    }

    return ( -1 ) ;
}

/* --------------------------------------------------------------------------- */
/* Sonido                                                                      */
/* --------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_load_song
 *
 *  Load a MOD from a file
 *
 *  PARAMS:
 *      file name
 *
 *  RETURN VALUE:
 *
 *      mod id
 *
 */

static int modsound_load_song( INSTANCE * my, int * params )
{
#ifndef TARGET_DINGUX_A320
    int var;
    const char * filename ;

    if ( !( filename = string_get( params[0] ) ) ) return ( 0 ) ;

    var = load_song( filename );
    string_discard( params[0] );

    return ( var );
#else
    return -1;
#endif
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_load_song2
 *
 *  Load a MOD from a file
 *
 *  PARAMS:
 *      file name
 *      pointer mod id
 *
 *  RETURN VALUE:
 *
 *
 */

static int modsound_bgload_song( INSTANCE * my, int * params )
{
#ifndef TARGET_DINGUX_A320
    bgload( load_song, params );
#else
    *(int *)(params[1]) = -1;
#endif
    return 0;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_play_song
 *
 *  Play a MOD
 *
 *  PARAMS:
 *      mod id;
 *      number of loops (-1 infinite loops)
 *
 *  RETURN VALUE:
 *
 *  -1 if there is any error
 *  0 if all goes ok
 *
 */

static int modsound_play_song( INSTANCE * my, int * params )
{
#ifndef TARGET_DINGUX_A320
    if ( params[0] == -1 ) return -1;
    return( play_song( params[0], params[1] ) );
#else
    return -1;
#endif
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_unload_song
 *
 *  Frees the resources from a MOD and unloads it
 *
 *  PARAMS:
 *      mod id;
 *
 *  RETURN VALUE:
 *
 *  -1 if there is any error
 *  0 if all goes ok
 *
 */

static int modsound_unload_song( INSTANCE * my, int * params )
{
#ifndef TARGET_DINGUX_A320
    if ( params[0] == -1 ) return ( -1 );
    return( unload_song( params[0] ) );
#else
    return -1;
#endif
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_unload_song2
 *
 *  Frees the resources from a MOD and unloads it
 *
 *  PARAMS:
 *      mod *id;
 *
 *  RETURN VALUE:
 *
 *  -1 if there is any error
 *  0 if all goes ok
 *
 */

static int modsound_unload_song2( INSTANCE * my, int * params )
{
#ifndef TARGET_DINGUX_A320
    int *s = (int *)(params[0]), r;
    if ( !s || *s == -1 ) return ( -1 );
    r = unload_song( *s );
    *s = 0;
    return( r );
#else
    return -1;
#endif
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_stop_song
 *
 *  Stop the play of a mod
 *
 *  PARAMS:
 *
 *  no params
 *
 *  RETURN VALUE:
 *
 *  -1 if there is any error
 *  0 if all goes ok
 *
 */

static int modsound_stop_song( INSTANCE * my, int * params )
{
#ifndef TARGET_DINGUX_A320
    return( stop_song() );
#else
    return -1;
#endif
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_pause_song
 *
 *  Pause the mod in curse, you can resume it after
 *
 *  PARAMS:
 *
 *  no params
 *
 *  RETURN VALUE:
 *
 *  -1 if there is any error
 *  0 if all goes ok
 *
 */

static int modsound_pause_song( INSTANCE * my, int * params )
{
#ifndef TARGET_DINGUX_A320
    return( pause_song() );
#else
    return -1;
#endif
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_resume_song
 *
 *  Resume the mod, paused before
 *
 *  PARAMS:
 *
 *  no params
 *
 *  RETURN VALUE:
 *
 *  -1 if there is any error
 *  0 if all goes ok
 *
 */

static int modsound_resume_song( INSTANCE * my, int * params )
{
#ifndef TARGET_DINGUX_A320
    return( resume_song() );
#else
    return -1;
#endif
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_is_playing_song
 *
 *  Check if there is any mod playing
 *
 *  PARAMS:
 *
 *  no params
 *
 *  RETURN VALUE:
 *
 *  -1 if there is any error
 *  TRUE OR FALSE if there is no error
 *
 */

static int modsound_is_playing_song( INSTANCE * my, int * params )
{
    return ( is_playing_song() );
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_set_song_volume
 *
 *  Set the volume for mod playing (0-128)
 *
 *  PARAMS:
 *
 *  int volume
 *
 *  RETURN VALUE:
 *
 *  -1 if there is any error
 *  0 if there is no error
 *
 */

static int modsound_set_song_volume( INSTANCE * my, int * params )
{
#ifndef TARGET_DINGUX_A320
    return ( set_song_volume( params[0] ) );
#else
    return -1;
#endif
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_fade_music_in
 *
 *  Play a MOD/OGG fading in it
 *
 *  PARAMS:
 *      mod pointer
 *      number of loops (-1 infinite loops)
 *      ms  microsends of fadding
 *
 *  RETURN VALUE:
 *
 *  -1 if there is any error
 *
 */

static int modsound_fade_music_in( INSTANCE * my, int * params )
{
#ifndef TARGET_DINGUX_A320
    if ( params[0] == -1 ) return -1;
    return ( fade_music_in( params[0], params[1], params[2] ) );
#else
    return -1;
#endif
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_fade_music_off
 *
 *  Stop the play of a mod
 *
 *  PARAMS:
 *
 *  ms  microsends of fadding
 *
 *  RETURN VALUE:
 *
 *  -1 if there is any error
 *
 */

static int modsound_fade_music_off( INSTANCE * my, int * params )
{
#ifndef TARGET_DINGUX_A320
    return ( fade_music_off( params[0] ) );
#endif
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_load_wav
 *
 *  Load a WAV from a file
 *
 *  PARAMS:
 *      file name
 *
 *  RETURN VALUE:
 *
 *      wav id
 *
 */

static int modsound_load_wav( INSTANCE * my, int * params )
{
#ifndef TARGET_DINGUX_A320
    int var;
    const char * filename ;

    if ( !( filename = string_get( params[0] ) ) ) return ( 0 ) ;

    var = load_wav( filename );
    string_discard( params[0] );

    return ( var );
#else
    return -1;
#endif
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_load_wav2
 *
 *  Load a WAV from a file
 *
 *  PARAMS:
 *      file name
 *      pointer wav id
 *
 *  RETURN VALUE:
 *
 *
 */

static int modsound_bgload_wav( INSTANCE * my, int * params )
{
#ifndef TARGET_DINGUX_A320
    bgload( load_wav, params );
#else
    *(int *)(params[1]) = -1;
#endif
    return 0;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_play_wav
 *
 *  Play a WAV
 *
 *  PARAMS:
 *      wav id;
 *      number of loops (-1 infinite loops)
 *
 *  RETURN VALUE:
 *
 *  -1 if there is any error
 *  0 if all goes ok
 *
 */

static int modsound_play_wav( INSTANCE * my, int * params )
{
#ifndef TARGET_DINGUX_A320
    if ( params[0] == -1 ) return -1;
    return( play_wav( params[0], params[1], -1 ) );
#else
    return -1;
#endif
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_play_wav_channel
 *
 *  Play a WAV
 *
 *  PARAMS:
 *      wav id;
 *      number of loops (-1 infinite loops)
 *      channel (-1 like modsound_play_wav)
 *
 *  RETURN VALUE:
 *
 *  -1 if there is any error
 *  0 if all goes ok
 *
 */

static int modsound_play_wav_channel( INSTANCE * my, int * params )
{
#ifndef TARGET_DINGUX_A320
    if ( params[0] == -1 ) return -1;
    return( play_wav( params[0], params[1], params[2] ) );
#else
    return -1;
#endif
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_unload_wav
 *
 *  Frees the resources from a wav, unloading it
 *
 *  PARAMS:
 *
 *  mod id
 *
 *  RETURN VALUE:
 *
 *  -1 if there is any error
 *  0 if all goes ok
 *
 */

static int modsound_unload_wav( INSTANCE * my, int * params )
{
#ifndef TARGET_DINGUX_A320
    if ( params[0] == -1 ) return -1;
    return( unload_wav( params[0] ) );
#else
    return -1;
#endif
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_unload_wav2
 *
 *  Frees the resources from a wav, unloading it
 *
 *  PARAMS:
 *
 *  mod *id
 *
 *  RETURN VALUE:
 *
 *  -1 if there is any error
 *  0 if all goes ok
 *
 */

static int modsound_unload_wav2( INSTANCE * my, int * params )
{
#ifndef TARGET_DINGUX_A320
    int *s = (int *)(params[0]), r;
    if ( !s || *s == -1 ) return ( -1 );
    r = unload_wav( *s );
    *s = 0;
    return( r );
#else
    return -1;
#endif
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_stop_wav
 *
 *  Stop a wav playing
 *
 *  PARAMS:
 *
 *  wav id
 *
 *  RETURN VALUE:
 *
 *  -1 if there is any error
 *  0 if all goes ok
 *
 */

static int modsound_stop_wav( INSTANCE * my, int * params )
{
#ifndef TARGET_DINGUX_A320
    return( stop_wav( params[0] ) );
#else
    return -1;
#endif
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_pause_wav
 *
 *  Pause a wav playing, you can resume it after
 *
 *  PARAMS:
 *
 *  wav id
 *
 *  RETURN VALUE:
 *
 *  -1 if there is any error
 *  0 if all goes ok
 *
 */

static int modsound_pause_wav( INSTANCE * my, int * params )
{
#ifndef TARGET_DINGUX_A320
    return ( pause_wav( params[0] ) );
#else
    return -1;
#endif
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : resume_wav
 *
 *  Resume a wav playing, paused before
 *
 *  PARAMS:
 *
 *  wav id
 *
 *  RETURN VALUE:
 *
 *  -1 if there is any error
 *  0 if all goes ok
 *
 */

static int modsound_resume_wav( INSTANCE * my, int * params )
{
#ifndef TARGET_DINGUX_A320
    return ( resume_wav( params[0] ) );
#else
    return -1;
#endif
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : is_playing_wav
 *
 *  Check a wav playing
 *
 *  PARAMS:
 *
 *  wav id
 *
 *  RETURN VALUE:
 *
 *  -1 if there is any error
 *  TRUE OR FALSE if there is no error
 *
 */


static int modsound_is_playing_wav( INSTANCE * my, int * params )
{
#ifndef TARGET_DINGUX_A320
    return ( is_playing_wav( params[0] ) );
#else
    return -1;
#endif
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_set_channel_volume
 *
 *  Set the volume for a wav playing (0-128)
 *
 *  PARAMS:
 *
 *  wav id
 *  int volume
 *
 *  RETURN VALUE:
 *
 *  -1 if there is any error
 *  0 if there is no error
 *
 */

static int modsound_set_channel_volume( INSTANCE * my, int * params )
{
#ifndef TARGET_DINGUX_A320
    return( set_channel_volume( params[0], params[1] ) );
#else
    return -1;
#endif
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_reserve_channels
 *
 *  Reserve the first channels (0 -> n-1) for the application, i.e. don't allocate
 *  them dynamically to the next sample if requested with a -1 value below.
 *
 *  PARAMS:
 *  number of channels to reserve.
 *
 *  RETURN VALUE:
 *  number of reserved channels.
 *  -1 if there is any error
 *
 */

static int modsound_reserve_channels( INSTANCE * my, int * params )
{
#ifndef TARGET_DINGUX_A320
    return ( reserve_channels( params[0] ) );
#else
    return -1;
#endif
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_set_wav_volume
 *
 *  Set the volume for a wav playing (0-128)
 *
 *  PARAMS:
 *
 *  wav id
 *  int volume
 *
 *  RETURN VALUE:
 *
 *  -1 if there is any error
 *  0 if there is no error
 *
 */

static int modsound_set_wav_volume( INSTANCE * my, int * params )
{
#ifndef TARGET_DINGUX_A320
    return( set_wav_volume( params[0], params[1] ) );
#else
    return -1;
#endif
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_set_panning
 *
 *  Set the panning for a wav channel
 *
 *  PARAMS:
 *
 *  channel
 *  left volume (0-255)
 *  right volume (0-255)
 *
 */

static int modsound_set_panning( INSTANCE * my, int * params )
{
#ifndef TARGET_DINGUX_A320
    return( set_panning( params[0], params[1], params[2] ) );
#else
    return -1;
#endif
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_set_position
 *
 *  Set the position of a channel. (angle) is an integer from 0 to 360
 *
 *  PARAMS:
 *
 *  channel
 *  angle (0-360)
 *  distance (0-255)
 *
 */

static int modsound_set_position( INSTANCE * my, int * params )
{
#ifndef TARGET_DINGUX_A320
    return( set_position( params[0], params[1], params[2] ) );
#else
    return -1;
#endif
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_set_distance
 *
 *  Set the "distance" of a channel. (distance) is an integer from 0 to 255
 *  that specifies the location of the sound in relation to the listener.
 *
 *  PARAMS:
 *
 *  channel
 *
 *  distance (0-255)
 *
 */

static int modsound_set_distance( INSTANCE * my, int * params )
{
#ifndef TARGET_DINGUX_A320
    return( set_distance( params[0], params[1] ) );
#else
    return -1;
#endif
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modsound_reverse_stereo
 *
 *  Causes a channel to reverse its stereo.
 *
 *  PARAMS:
 *
 *  channel
 *
 *  flip 0 normal != reverse
 *
 */

static int modsound_reverse_stereo( INSTANCE * my, int * params )
{
#ifndef TARGET_DINGUX_A320
    return( reverse_stereo( params[0], params[1] ) );
#else
    return -1;
#endif
}

/* --------------------------------------------------------------------------- */

static int modsound_set_music_position( INSTANCE * my, int * params )
{
#ifndef TARGET_DINGUX_A320
    return ( Mix_SetMusicPosition( ( double ) *( float * ) &params[0] ) );
#else
    return -1;
#endif
}

/* --------------------------------------------------------------------------- */

static int modsound_init( INSTANCE * my, int * params )
{
#ifndef TARGET_DINGUX_A320
    return( sound_init() );
#else
    return -1;
#endif
}

/* --------------------------------------------------------------------------- */

static int modsound_close( INSTANCE * my, int * params )
{
#ifndef TARGET_DINGUX_A320
    sound_close();
    return( 0 );
#else
    return -1;
#endif
}

/* --------------------------------------------------------------------------- */

DLSYSFUNCS  __bgdexport( mod_sound, functions_exports )[] =
{
    { "SOUND_INIT"          , ""     , TYPE_INT , modsound_init               },
    { "SOUND_CLOSE"         , ""     , TYPE_INT , modsound_close              },

    { "LOAD_SONG"           , "S"    , TYPE_INT , modsound_load_song          },
    { "LOAD_SONG"           , "SP"   , TYPE_INT , modsound_bgload_song        },
    { "UNLOAD_SONG"         , "I"    , TYPE_INT , modsound_unload_song        },
    { "UNLOAD_SONG"         , "P"    , TYPE_INT , modsound_unload_song2       },

    { "PLAY_SONG"           , "II"   , TYPE_INT , modsound_play_song          },
    { "STOP_SONG"           , ""     , TYPE_INT , modsound_stop_song          },
    { "PAUSE_SONG"          , ""     , TYPE_INT , modsound_pause_song         },
    { "RESUME_SONG"         , ""     , TYPE_INT , modsound_resume_song        },

    { "SET_SONG_VOLUME"     , "I"    , TYPE_INT , modsound_set_song_volume    },

    { "IS_PLAYING_SONG"     , ""     , TYPE_INT , modsound_is_playing_song    },

    { "LOAD_WAV"            , "S"    , TYPE_INT , modsound_load_wav           },
    { "LOAD_WAV"            , "SP"   , TYPE_INT , modsound_bgload_wav         },
    { "UNLOAD_WAV"          , "I"    , TYPE_INT , modsound_unload_wav         },
    { "UNLOAD_WAV"          , "P"    , TYPE_INT , modsound_unload_wav2        },

    { "PLAY_WAV"            , "II"   , TYPE_INT , modsound_play_wav           },
    { "PLAY_WAV"            , "III"  , TYPE_INT , modsound_play_wav_channel   },
    { "STOP_WAV"            , "I"    , TYPE_INT , modsound_stop_wav           },
    { "PAUSE_WAV"           , "I"    , TYPE_INT , modsound_pause_wav          },
    { "RESUME_WAV"          , "I"    , TYPE_INT , modsound_resume_wav         },

    { "IS_PLAYING_WAV"      , "I"    , TYPE_INT , modsound_is_playing_wav     },

    { "FADE_MUSIC_IN"       , "III"  , TYPE_INT , modsound_fade_music_in      },
    { "FADE_MUSIC_OFF"      , "I"    , TYPE_INT , modsound_fade_music_off     },

    { "SET_WAV_VOLUME"      , "II"   , TYPE_INT , modsound_set_wav_volume     },
    { "SET_CHANNEL_VOLUME"  , "II"   , TYPE_INT , modsound_set_channel_volume },

    { "RESERVE_CHANNELS"    , "I"    , TYPE_INT , modsound_reserve_channels   },

    { "SET_PANNING"         , "III"  , TYPE_INT , modsound_set_panning        },
    { "SET_POSITION"        , "III"  , TYPE_INT , modsound_set_position       },
    { "SET_DISTANCE"        , "II"   , TYPE_INT , modsound_set_distance       },

    { "REVERSE_STEREO"      , "II"   , TYPE_INT , modsound_reverse_stereo     },

    { "SET_MUSIC_POSITION"  , "F"    , TYPE_INT , modsound_set_music_position },

    { 0                     , 0      , 0        , 0                           }
};

/* --------------------------------------------------------------------------- */
/* Funciones de inicializacion del modulo/plugin                               */

void  __bgdexport( mod_sound, module_initialize )()
{
#ifndef TARGET_DINGUX_A320
    if ( !SDL_WasInit( SDL_INIT_AUDIO ) ) SDL_InitSubSystem( SDL_INIT_AUDIO );
#endif
}

/* --------------------------------------------------------------------------- */

void __bgdexport( mod_sound, module_finalize )()
{
#ifndef TARGET_DINGUX_A320
    if ( SDL_WasInit( SDL_INIT_AUDIO ) ) SDL_QuitSubSystem( SDL_INIT_AUDIO );
#endif
}

/* --------------------------------------------------------------------------- */
