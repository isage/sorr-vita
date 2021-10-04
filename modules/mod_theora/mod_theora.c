/*  Theora module for BennuGD
 *
 *  Originally written by Joseba García Etxebarria <joseba.gar@gmail.com>
 *  based on the simplesdl.c example included with theoraplay
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
*/


#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>

#include <theoraplay.h>
#include <SDL.h>
#include <SDL_mixer.h>

/* BennuGD stuff */
#include <bgddl.h>
#include <xstrings.h>
#include <libgrbase.h>
#include <g_video.h>

struct ctx
{
    GRAPH *graph;
    SDL_AudioCVT cvt;
    const THEORAPLAY_VideoFrame *frame;
    const THEORAPLAY_AudioPacket *audio;
    THEORAPLAY_Decoder *decoder;
    Uint32 baseticks;
    Uint32 framems;
    int convertaudio;
};

typedef struct AudioQueue
{
    const THEORAPLAY_AudioPacket *audio;
    int offset;
    struct AudioQueue *next;
} AudioQueue;

static volatile AudioQueue *audio_queue = NULL;
static volatile AudioQueue *audio_queue_tail = NULL;

int mixer_freq, mixer_channels;
Uint16 mixer_format;

struct ctx video;

int playing_video=0;

static void SDLCALL audio_callback(void *userdata, Uint8 *stream, int len)
{
    if(! playing_video) {
        return;
    }
    
    Sint16 *dst = (Sint16 *) stream;
    
    float remaining = len, parsed_len = 0.0f;
    
    while (audio_queue && (remaining > 0.0f))
    {
        const Uint32 now = SDL_GetTicks() - video.baseticks;
        volatile AudioQueue *item = audio_queue;
        AudioQueue *next = item->next;

        const int channels = item->audio->channels;
            
        const float *src = item->audio->samples + (item->offset * channels);
        int cpy = (item->audio->frames - item->offset) * channels;
        
        if (cpy > (len / sizeof (Sint16)))
            cpy = len / sizeof (Sint16);

        // Drop audio packets if we're out of sync
        if ( next->audio->playms > now ) {
            int i;
            
            for (i = 0; i < cpy; i++)
            {
                const float val = *(src++);
                if (val < -1.0f)
                    *(dst++) = -32768;
                else if (val > 1.0f)
                    *(dst++) = 32767;
                else
                    *(dst++) = (Sint16) (val * 32767.0f);
            }
            
            parsed_len = cpy * sizeof (Sint16);
            
            if (video.convertaudio) {
                dst -= cpy;
                video.cvt.buf = malloc(cpy * sizeof (Sint16) * video.cvt.len_mult);
                video.cvt.len = cpy * sizeof (Sint16) ;
                memcpy(video.cvt.buf, dst, cpy * sizeof (Sint16));
                SDL_ConvertAudio(&video.cvt);
                if(video.cvt.len_cvt) {
                    memcpy(dst, video.cvt.buf, video.cvt.len_cvt);
                    dst += (video.cvt.len_cvt) / sizeof (Sint16);
                }
                free(video.cvt.buf);
                
                parsed_len = video.cvt.len_cvt;
            }
            
            remaining -= parsed_len;
        }
        
        item->offset += (cpy / channels);
        
        if (item->offset >= item->audio->frames)
        {
            THEORAPLAY_freeAudio(item->audio);
            free((void *) item);
            audio_queue = next;
        } // if
    } // while
    
    if (!audio_queue)
        audio_queue_tail = NULL;
    
    if (remaining > 0)
        memset(dst, '\0', (int)remaining);
} // audio_callback


static void queue_audio(const THEORAPLAY_AudioPacket *audio)
{
    AudioQueue *item = (AudioQueue *) malloc(sizeof (AudioQueue));
    if (!item)
    {
        THEORAPLAY_freeAudio(audio);
        return;  // oh well.
    } // if
    
    item->audio = audio;
    item->offset = 0;
    item->next = NULL;
    
    if (audio_queue_tail)
        audio_queue_tail->next = item;
    else
        audio_queue = item;
    audio_queue_tail = item;
} // queue_audio

// Paint the current video frame onscreen, skipping those that we already missed
void refresh_video()
{
    if(! playing_video) {
        return;
    }
        
    const Uint32 now = SDL_GetTicks() - video.baseticks;

    if (!video.frame) {
        video.frame = THEORAPLAY_getVideo(video.decoder);
    }
    
    // Play video frames when it's time.
    if (video.frame && (video.frame->playms <= now))
    {
        if ( video.framems && ((now - video.frame->playms) >= video.framems) )
        {
            // Skip frames to catch up, but keep track of the last one
            // in case we catch up to a series of dupe frames, which
            // means we'd have to draw that final frame and then wait for
            // more.
            const THEORAPLAY_VideoFrame *last = video.frame;
            while ((video.frame = THEORAPLAY_getVideo(video.decoder)) != NULL)
            {
                THEORAPLAY_freeVideo(last);
                last = video.frame;
                if ((now - video.frame->playms) < video.framems)
                    break;
            } // while
            
            if (!video.frame)
                video.frame = last;
        } // if
        
        if (!video.frame)  // do nothing; we're far behind and out of options.
        {
            static int warned = 0;
            if (!warned)
            {
                warned = 1;
                fprintf(stderr, "WARNING: Playback can't keep up!\n");
            } // if
        } // if        
        else
        {
            memcpy(video.graph->data, video.frame->pixels, video.graph->height * video.graph->pitch);
            // Mark the video GRAPH as dirty so that BennuGD redraws it
            video.graph->modified = 1;
            video.graph->info_flags &=~GI_CLEAN;
        }
        THEORAPLAY_freeVideo(video.frame);
        video.frame = NULL;
    } // if
    
    while ((video.audio = THEORAPLAY_getAudio(video.decoder)) != NULL)
        queue_audio(video.audio);

    
    return;
}

/* Checks wether the current video is being played */
static int video_is_playing() {
    return playing_video;
}

static int video_play(INSTANCE *my, int * params)
{
    int bpp;
    const int MAX_FRAMES = 30;

    bpp = screen->format->BitsPerPixel;

    if(playing_video == 1)
        return -1;

	if(! scr_initialized) return (-1);
	
    playing_video = 1;
 
    /* Start the decoding, 8bpp not supported */
    switch (bpp) {
        case 16:
            video.decoder = THEORAPLAY_startDecodeFile(string_get(params[0]), MAX_FRAMES, THEORAPLAY_VIDFMT_RGB565);
            string_discard(params[0]);
            break;
        /* Just in case this ever gets supported in BennuGD */
        case 24:
            video.decoder = THEORAPLAY_startDecodeFile(string_get(params[0]), MAX_FRAMES, THEORAPLAY_VIDFMT_RGB);
            string_discard(params[0]);
            break;
        case 32:
            video.decoder = THEORAPLAY_startDecodeFile(string_get(params[0]), MAX_FRAMES, THEORAPLAY_VIDFMT_RGBA);
            string_discard(params[0]);
            break;
    }
    
    if (!video.decoder)
    {
        fprintf(stderr, "Failed to start decoding '%s'!\n", string_get(params[0]));
        string_discard(params[0]);
        return -1;
    }
    
    // Wait until the decoder has parsed out some basic truths from the
    //  file. In a video game, you could choose not to block on this, and
    //  instead do something else until the file is ready.
    while (!THEORAPLAY_isInitialized(video.decoder)) {
        SDL_Delay(10);
    }
    
    if(! THEORAPLAY_hasVideoStream(video.decoder)) {
        THEORAPLAY_stopDecode(video.decoder);
        return -1;
    }

    while (!video.frame || !video.audio) {
        if (!video.frame) video.frame = THEORAPLAY_getVideo(video.decoder);
        if (!video.audio) video.audio = THEORAPLAY_getAudio(video.decoder);
        SDL_Delay(10);
    }
    
    video.framems = (video.frame->fps == 0.0) ? 0 : ((Uint32) (1000.0 / video.frame->fps));
    
    Mix_QuerySpec(&mixer_freq, &mixer_format, &mixer_channels);
    
    video.convertaudio = 0;
    
    if ( video.audio && (video.audio->freq != mixer_freq || video.audio->channels != mixer_channels) ) {
        if (SDL_BuildAudioCVT(&video.cvt,
                              AUDIO_S16, video.audio->channels, video.audio->freq,
                              mixer_format, mixer_channels, mixer_freq) == -1) {
            fprintf(stderr, "Couldn't create required audio conversion SDL_AudioCVT, sorry\n");
        } else {
            video.convertaudio = 1;
        }
    }
    
    while (video.audio)
    {
        queue_audio(video.audio);
        video.audio = THEORAPLAY_getAudio(video.decoder);
    } // while
    
    video.baseticks = SDL_GetTicks();
    
    // Create the graph holding the video surface
    video.graph = bitmap_new_syslib(video.frame->width, video.frame->height, bpp);
    THEORAPLAY_freeVideo(video.frame);
    video.frame = NULL;
    if(! video.graph) {
        THEORAPLAY_stopDecode(video.decoder);
        video.decoder = NULL;
    }
    
    Mix_HookMusic(audio_callback, NULL);
    
    playing_video = 1;
    
    return video.graph->code;
}

/* Stop the currently being played video and release theoraplay stuff */
static int video_stop(INSTANCE *my, int * params)
{
    if(! playing_video) {
        return 0;
    }
    
    /* Release the video playback lock */
    playing_video = 0;

    if(video.graph) {
        grlib_unload_map(0, video.graph->code);
        video.graph = NULL;
    }
    
    if(video.decoder) {
        THEORAPLAY_stopDecode(video.decoder);
        video.decoder = NULL;
    }
    
    /* Empty the audio queue */
    while (audio_queue) {
        volatile AudioQueue *item = audio_queue;
        AudioQueue *next = item->next;
        THEORAPLAY_freeAudio(item->audio);
        free((void *) item);
        audio_queue = next;
    } // while
    
    if (!audio_queue)
        audio_queue_tail = NULL;

    Mix_HookMusic(NULL, NULL);

    return 0;
}

/* TODO: Pause the currently playing video */
static int video_pause() {
    return 0;
}

DLSYSFUNCS __bgdexport( mod_theora, functions_exports )[] =
{
	{"VIDEO_PLAY"                  , "S"    , TYPE_DWORD , video_play       },
	{"VIDEO_STOP"                  , ""     , TYPE_DWORD , video_stop       },
    {"VIDEO_PAUSE"                 , ""     , TYPE_DWORD , video_pause      },
	{"VIDEO_IS_PLAYING"            , ""     , TYPE_DWORD , video_is_playing },
	{ NULL        , NULL , 0         , NULL              }
};
 
char * __bgdexport( mod_theora, modules_dependency )[] =
{
	"libgrbase",
	"libvideo",
    "mod_sound",
	NULL
};

void __bgdexport( mod_theora, module_initialize )()
{
    if ( !SDL_WasInit( SDL_INIT_AUDIO ) ) SDL_InitSubSystem( SDL_INIT_AUDIO );
    //if ( !audio_initialized ) sound_init();
}
 
void __bgdexport( mod_theora, module_finalize )()
{
    video_stop(NULL, NULL);
}

/* ----------------------------------------------------------------- */

/* Bigest priority first execute
 Lowest priority last execute */

HOOK __bgdexport( mod_theora, handler_hooks )[] =
{
    { 3000, refresh_video                     },
    {    0, NULL                              }
} ;
