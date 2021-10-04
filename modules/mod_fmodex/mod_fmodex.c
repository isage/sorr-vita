/*
 *  Copyright © 2009-2012 Joseba García Etxebarria <joseba.gar@gmail.com>
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

#ifdef WIN32
#include <windows.h>
#include <winbase.h>
#endif

#include <stdio.h>
#include <string.h>

/* FMOD headers */
#include <fmod.h>
#include <fmod_errors.h>

/* BennuGD stuff */
#include <bgddl.h>
#include <xstrings.h>
#include <dlvaracc.h>

/* Definitions for limits */
#define MAXCHANNELS         64
#define SPECTRUMSIZE        256

/* Constant definitions for BennuGD vars */
enum {
    FMODEX_SPECTRUM = 0,
    SOUND_FREQ,
    FMODEX_SPECTRUMSIZE
};

/* Error checking, no return value */
#define ERRCHECK(FUNCTION) \
{ \
    if (result != FMOD_OK) \
        fprintf(stderr, "mod_fmodex: %s error! (%d) %s\n", FUNCTION, result, \
            FMOD_ErrorString(result)); \
} \

/* Error checking, return a value in case of failure */
#define ERRCHECK_RETURN(FUNCTION, retval) \
{ \
    if (result != FMOD_OK) { \
        fprintf(stderr, "mod_fmodex: %s error! (%d) %s\n", FUNCTION, result, \
            FMOD_ErrorString(result)); \
        return retval; \
    } \
}

/* These var should stay static */
static FMOD_SYSTEM     *fsystem;
static FMOD_SOUND      *mic_sound = 0;
static FMOD_CHANNEL    *spectrum_channel = 0;
static int              analyzing_spectrum;

/* Global info (where spectrum is stored) */
char __bgdexport( mod_fmodex, globals_def )[] =
    "float fmodex_spectrum[256];\n"
    "int   sound_freq = 48000;\n"
    "int   fmodex_spectrumsize = 0;\n";

/* Globals fixup (so that the interpreter knows where to look for vars) */
DLVARFIXUP __bgdexport( mod_fmodex, globals_fixup )[] =
{
    { "fmodex_spectrum" , NULL, -1, -1 },
    { "sound_freq" , NULL, -1, -1 },
    { "fmodex_spectrumsize" , NULL, -1, -1 },
    { NULL, NULL, -1, -1 }
};

/* Takes care of initializing the module */
void fmodex_init()
{
    FMOD_RESULT       result;
    unsigned int      version;
    int               outputrate=0;

    /*
        Create a System object and initialize.
    */
    result = FMOD_System_Create(&fsystem);
    ERRCHECK("FMODEx_Init");

    result = FMOD_System_GetVersion(fsystem, &version);
    ERRCHECK("FMODEx_Init");

    if (version < FMOD_VERSION)
    {
        printf("Error! You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);
    }

#ifdef TARGET_LINUX
    result = FMOD_System_SetOutput(fsystem, FMOD_OUTPUTTYPE_PULSEAUDIO);
    ERRCHECK("FMODEx_Init");
#endif

    result = FMOD_System_Init(fsystem, MAXCHANNELS, FMOD_INIT_NORMAL, NULL);
    ERRCHECK("FMODEx_Init");
    
    // Set the global sound_freq variable
    result = FMOD_System_GetSoftwareFormat(fsystem, &outputrate, 0, 0, 0, 0, 0);
    ERRCHECK("FMODEX_GET_SPECTRUM");
    GLODWORD(mod_fmodex, SOUND_FREQ) = outputrate;
    
    GLODWORD(mod_fmodex, FMODEX_SPECTRUMSIZE) = SPECTRUMSIZE;
}

/* Load a song as a stream (for decoding it at runtime) */
int mod_fmodex_load_song(INSTANCE * i, int * params)
{
    FMOD_SOUND       *sound;
    FMOD_CHANNEL     *channel = 0;
    FMOD_RESULT       result;
    int channel_index=0;
    
    FMOD_System_Update(fsystem);

    /* Load the song as a stream */
    result = FMOD_System_CreateSound(fsystem, string_get(params[0]), FMOD_SOFTWARE | FMOD_2D | FMOD_CREATESTREAM, 0, &sound);
    ERRCHECK_RETURN("FMODEX_LOAD_SONG", -1);
    /* Now start playback of the song, inmediately pause it */
    result = FMOD_System_PlaySound(fsystem, FMOD_CHANNEL_FREE, sound, 1, &channel);
    ERRCHECK_RETURN("FMODEX_LOAD_SONG", -1);
    // Return the internal FMOD Ex channel id
    result = FMOD_Channel_GetIndex(channel, &channel_index);
    ERRCHECK_RETURN("FMODEX_LOAD_SONG", -1);
    string_discard(params[0]);

    return channel_index;
}

/* Pause/Unpause a song   */
/* Also used as PLAY_SONG */
int mod_fmodex_pause_song(INSTANCE * i, int * params)
{
    FMOD_RESULT              result;
    FMOD_CHANNEL            *channel = 0;
    int paused;

    /* Retrieve the channel from the given ID */
    result = FMOD_System_GetChannel(fsystem, params[0], &channel);
    ERRCHECK_RETURN("FMODEX_PAUSE_SONG", -1);

    /* Now set the channel status to paused/unpaused */
    result = FMOD_Channel_GetPaused(channel, &paused);
    if ( result == FMOD_OK )
        result = FMOD_Channel_SetPaused(channel, !paused);
    else
        ERRCHECK_RETURN("FMODEX_PAUSE_SONG", -1);
    return 0;
}

/* Check wether a given song is playing */
int mod_fmodex_is_playing_song(INSTANCE * i, int * params)
{
    FMOD_RESULT              result;
    FMOD_CHANNEL            *channel = 0;
    int playing=0;
    
    /* Retrieve the channel from the given ID */
    result = FMOD_System_GetChannel(fsystem, params[0], &channel);
    ERRCHECK_RETURN("FMODEX_IS_PLAYING_SONG", -1);

    result = FMOD_Channel_IsPlaying(channel, &playing);
    ERRCHECK_RETURN("FMODEX_IS_PLAYING_SONG", -1);
    
    return playing;
}

/* Stop song playback */
int mod_fmodex_stop_song(INSTANCE * i, int * params)
{
    FMOD_RESULT              result;
    FMOD_CHANNEL            *channel = 0;
    FMOD_SOUND              *sound;

    /* Get the channel number */
    result = FMOD_System_GetChannel(fsystem, params[0], &channel);
    ERRCHECK_RETURN("FMODEX_SONG_STOP", -1);

    /* Get the FMOD_SOUND from the FMOD_CHANNEL */
    result = FMOD_Channel_GetCurrentSound(channel, &sound);
    ERRCHECK_RETURN("FMODEX_SONG_STOP", -1);

    /* Tell the channel to stop playback */
    result = FMOD_Channel_Stop(channel);
    ERRCHECK_RETURN("FMODEX_SONG_STOP", -1);

    /* Release the FMOD_SOUND */
    result = FMOD_Sound_Release( sound );
    ERRCHECK_RETURN("FMODEX_SONG_STOP", -1);
    
    return 0;
}

/* Initialize spectrum analysis for a song that's already playing
 http://www.cs.cf.ac.uk/Dave/Multimedia/node271.html */
int mod_fmodex_song_get_spectrum(INSTANCE * i, int * params)
{
    FMOD_RESULT              result;
    FMOD_CHANNEL            *channel = 0;
    int                      playing = 0;
    
    if( analyzing_spectrum != 0 )
        return -1;
    
    // Check the given channel is actually playing a song
    result = FMOD_System_GetChannel(fsystem, params[0], &channel);
    ERRCHECK_RETURN("FMODEX_IS_PLAYING_SONG", -1);
    
    result = FMOD_Channel_IsPlaying(channel, &playing);
    ERRCHECK_RETURN("FMODEX_IS_PLAYING_SONG", -1);
    
    spectrum_channel = channel;
    
    analyzing_spectrum = 2;
    
    return 0;
}

// Returns the total number of recording devices
int mic_num()
{
    FMOD_RESULT result;
    int         numdrivers=0;

    /* Get the number of recording devices */
    result = FMOD_System_GetRecordNumDrivers(fsystem, &numdrivers);
    ERRCHECK_RETURN("FMODEX_MIC_NUM", -1);

    return numdrivers;
}

/* Get number of available input devices (mikes) */
int mod_fmodex_mic_num(INSTANCE * i, int * params)
{
    return mic_num();
}

/* Return the name for the n-th input device */
int mod_fmodex_mic_name(INSTANCE * i, int * params)
{
    FMOD_RESULT result;
    int         strid=0;
    char        micname[256];

    /* Check we've not been asked to get info from a nonexistant microphone */
    if(params[0] >= mic_num()) {
        strid = string_new("Invalid microphone or mic. not present");
        string_use(strid);
        return strid;
    }

    /* Get the name for the input source */
    result = FMOD_System_GetRecordDriverInfo(fsystem, params[0], micname, 256, 0);
    ERRCHECK_RETURN("FMODEX_MIC_NAME", -1);

    /* Return that name as a string */
    strid = string_new(micname);
    string_use(strid);

    return strid;
}

/* Update the spectrum */
void spectrum_update()
{
    FMOD_RESULT   result;
    int           i=0;
    float        *spectrum, fmodex_spectrum[SPECTRUMSIZE];

    if (analyzing_spectrum == 0)
        return;

    result = FMOD_Channel_GetSpectrum(spectrum_channel, fmodex_spectrum, SPECTRUMSIZE, 0,
                        FMOD_DSP_FFT_WINDOW_TRIANGLE ); 
    ERRCHECK("FMODEX_GET_SPECTRUM");

    spectrum = ( float * ) ( &GLODWORD( mod_fmodex, FMODEX_SPECTRUM ) );
    for(i=0; i < SPECTRUMSIZE; i++) {
        // Choose this is you want the spectrum in dBs
        // spectrum[i] = 10.0f * (float)log10(fmodex_spectrum[i]) * 2.0f;
        // Absolute value (0.0 <-> 1.0)
        spectrum[i] = fmodex_spectrum[i];
    }
}

/* Stop spectrum analysis */
void fmodex_stop_spectrum_analysis()
{
    FMOD_RESULT result;

    if (analyzing_spectrum == 0)
        return;
    else if (analyzing_spectrum == 1) {
        /* Stop the recording */
        result = FMOD_Sound_Release(mic_sound);
        ERRCHECK("FMODEX_STOP_SPECTRUM");
    
        FMOD_System_RecordStop(fsystem, 0);
    }

    analyzing_spectrum = 0;
}

/* Initialize spectrum analysis
   http://www.cs.cf.ac.uk/Dave/Multimedia/node271.html */
int mod_fmodex_mic_get_spectrum(INSTANCE * i, int * params)
{
    FMOD_RESULT             result;
    FMOD_CREATESOUNDEXINFO  exinfo;
    int                     outputrate=0;
    
    if( analyzing_spectrum != 0 )
        return -1;

    // Get the current output frequency range
    result = FMOD_System_GetSoftwareFormat(fsystem, &outputrate, 0, 0, 0, 0, 0);
    ERRCHECK_RETURN("FMODEX_GET_SPECTRUM", -1);

    // Compute the spectrum range
    // http://en.wikipedia.org/wiki/Nyquist%E2%80%93Shannon_sampling_theorem

    // Create an FMOD_SOUND with the data read from the mike
    memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));

    exinfo.cbsize           = sizeof(FMOD_CREATESOUNDEXINFO);
    exinfo.numchannels      = 1;
    exinfo.format           = FMOD_SOUND_FORMAT_PCM16;
    exinfo.defaultfrequency = outputrate;
    exinfo.length           = exinfo.defaultfrequency * exinfo.numchannels ;

    result = FMOD_System_CreateSound(fsystem, 0, FMOD_2D | FMOD_SOFTWARE | FMOD_LOOP_NORMAL | FMOD_OPENUSER, &exinfo, &mic_sound);
    ERRCHECK_RETURN("FMODEX_GET_SPECTRUM", -1);

    /* Now, start recording, updating the spectrum matrix is done separately */
    result = FMOD_System_RecordStart(fsystem, params[0], mic_sound, 1);
    ERRCHECK("FMODEX_GET_SPECTRUM");
    
    result = FMOD_System_PlaySound(fsystem, FMOD_CHANNEL_FREE,
                                   mic_sound, 0, &spectrum_channel);
    ERRCHECK("FMODEX_GET_SPECTRUM");
    
    /* Dont hear what is being recorded otherwise it will feedback.
     Spectrum analysis is done before volume scaling in the DSP chain */
    result = FMOD_Channel_SetVolume(spectrum_channel, 0);
    ERRCHECK("FMODEX_GET_SPECTRUM");

    // Announce we're doing spectrum analysis, so that we don't start
    // two separate ones.
    analyzing_spectrum = 1;
    
    return 0;
}

/* Stop performing spectrum analysis */
int mod_fmodex_stop_spectrum(INSTANCE * i, int * params)
{
    fmodex_stop_spectrum_analysis();
    return 0;
}

/* Function declaration */
DLSYSFUNCS __bgdexport( mod_fmodex, functions_exports )[] =
{
    {"FMODEX_SONG_LOAD",         "S", TYPE_INT,       mod_fmodex_load_song },
    {"FMODEX_SONG_PLAY",         "I", TYPE_INT,       mod_fmodex_pause_song },
    {"FMODEX_SONG_PAUSE",        "I", TYPE_INT,       mod_fmodex_pause_song },
    {"FMODEX_SONG_RESUME",       "I", TYPE_INT,       mod_fmodex_pause_song },
    {"FMODEX_SONG_PLAYING",      "I", TYPE_INT,       mod_fmodex_is_playing_song },
    {"FMODEX_SONG_STOP",         "I", TYPE_INT,       mod_fmodex_stop_song },
    {"FMODEX_SONG_GET_SPECTRUM", "I", TYPE_INT,       mod_fmodex_song_get_spectrum },
    {"FMODEX_MIC_NUM",           "",  TYPE_INT,       mod_fmodex_mic_num },
    {"FMODEX_MIC_NAME",          "I", TYPE_STRING,    mod_fmodex_mic_name },
    {"FMODEX_MIC_GET_SPECTRUM",  "I", TYPE_UNDEFINED, mod_fmodex_mic_get_spectrum },
    {"FMODEX_STOP_SPECTRUM",     "",  TYPE_UNDEFINED, mod_fmodex_stop_spectrum },
    {0, 0, 0, 0}
};

/* Routines to be run when the library is imported */
void __bgdexport( mod_fmodex, module_initialize )()
{
    fmodex_init();
}

/* Routines to be run when the library is closed */
void __bgdexport( mod_fmodex, module_finalize )()
{
    fmodex_stop_spectrum_analysis();
    FMOD_System_Release(fsystem);
}

/* Bigest priority first execute
   Lowest priority last execute */

HOOK __bgdexport( mod_fmodex, handler_hooks )[] =
{
    { 1000, spectrum_update    },
    {    0, NULL               }
} ;

