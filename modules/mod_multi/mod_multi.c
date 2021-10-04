/*
 *  Copyright (C) 2013 Joseba García Etxebarria. All rights reserved.
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

#include <stdio.h>
#include <strings.h>
#include <bgddl.h>
#include <libvideo.h>
#include <g_video.h>
#include <libmouse_symbols.h>
#include <SDL.h>
#include "bgddl.h"
#include "dlvaracc.h"

#ifndef MAX_POINTERS
#   define MAX_POINTERS 10
#endif

/* --------------------------------------------------------------------------- */

typedef struct {
    SDL_bool active;
    int x;
    int y;
    int pressure;
    SDL_FingerID fingerid;
} multi_pointer;

multi_pointer pointers[MAX_POINTERS];

int numpointers=0;

// Required for mouse emulation
enum {
    MOUSEX = 0,
    MOUSEY,
    MOUSELEFT = 9
};

/* Return the position of finger in the pointers array, if it's not there,
return the first unused entry.
If there are none available, return -1 */
int get_sdlfinger_index(SDL_FingerID finger) {
    int n=0;

    // First, try to see if the given ID matches any active one
    for(n=0; n<MAX_POINTERS; n++) {
        if(finger == pointers[n].fingerid) {
            return n;
        }
    }

    // ID not found, try to find a free spot
    for(n=0; n<MAX_POINTERS; n++) {
        if(pointers[n].active == SDL_FALSE) {
            return n;
        }
    }

    // Fail
    return -1;
}

/* Process SDL events looking for multitouch-related ones */
void parse_input_events() {
    int n=0;
    float x=0.0, y=0.0;
    double width=0, height=0;
    double w_width=0, w_height=0;
    SDL_DisplayMode mode;
    SDL_Event e;

    // SDL will give us the touch position relative to the whole window
    // but we might have set a different virtual resolution
    if(screen) {
        width  = screen->w;
        height = screen->h;
    } else {
        // This'll avoid division-by-zero below
        SDL_Log("Unexpected condition getting resolution, refusing to parse events");
        return;
    }

    // Get the size of the SDL window to handle the case when the
    // bennugd window doesn't use all the available screen space
    if (SDL_GetWindowDisplayMode(window, &mode)) {
        // Nasty error
        SDL_Log("Couldn't get current window size, using defaults");
        w_width = width;
        w_height = height;
    } else {
        w_width = mode.w;
        w_height = mode.h;
    }

    // Parse the SDL event
    while ( SDL_PeepEvents( &e, 1, SDL_GETEVENT, SDL_FINGERDOWN, SDL_FINGERMOTION ) > 0 ) {
        switch ( e.type ) {
            case SDL_FINGERDOWN:
                // Retrive the the position in the array
                n = get_sdlfinger_index(e.tfinger.fingerId);

                // Quit if fingerId not found or array full
                if (n == -1) {
                    break;
                }

                // Store the amount of fingers onscreen
                numpointers = SDL_GetNumTouchFingers(e.tfinger.touchId);
                // Store the data about this finger's position
                pointers[n].fingerid = e.tfinger.fingerId;
                pointers[n].active = SDL_TRUE;
                x = e.tfinger.x * width;
                y = e.tfinger.y * height;
                pointers[n].pressure = (float)e.tfinger.pressure * 255;

                // Convert the touch location taking scaling/rotations into account
                pointers[n].x = (int)x;
                pointers[n].y = (int)y;

                // Fake a mouse click, but only for the first pointer and
                // if libmouse has been imported
                /*if (n == 0) {
                    if ( GLOEXISTS( libmouse, MOUSEX ) ) {
                        GLOINT32( libmouse, MOUSEX )    = pointers[n].x;
                        GLOINT32( libmouse, MOUSEY )    = pointers[n].y;
                        GLODWORD( libmouse, MOUSELEFT ) = 1 ;
                    }
                }*/
                break;

            case SDL_FINGERMOTION:
                // Retrive the touch state, the finger id and the position in the array
                n = get_sdlfinger_index(e.tfinger.fingerId);

                // Quit if fingerid not found
                if (n == -1)
                    break;

                // Update the data about this finger's position
                x = e.tfinger.x * width;
                y = e.tfinger.y * height;
                pointers[n].pressure = (float)e.tfinger.pressure * 255;

                // Convert the touch location taking scaling/rotations into account
                pointers[n].x = (int)x;
                pointers[n].y = (int)y;

                // Fake a mouse move, but only if libmouse has been imported
                /*if (n == 0) {
                    if ( GLOEXISTS( libmouse, MOUSEX ) ) {
                        GLOINT32( libmouse, MOUSEX ) = pointers[n].x;
                        GLOINT32( libmouse, MOUSEY ) = pointers[n].y;
                    }
                }*/
                break;

            case SDL_FINGERUP:
                // Retrive the touch state, the finger id and the position in the array
                n = get_sdlfinger_index(e.tfinger.fingerId);

                // Refresh the total number of fingers onscreen
                numpointers = SDL_GetNumTouchFingers(e.tfinger.touchId);

                // Quit if fingerid not found
                if (n == -1)
                    break;

                // Remove the data about this finger's position
                pointers[n].active = SDL_FALSE;
                pointers[n].pressure = 0.0;

                // Fake a mouse release, but only for the first pointer and
                // if libmouse is imported
                /*if (n == 0) {
                    if ( GLOEXISTS( libmouse, MOUSEX ) ) {
                        GLODWORD( libmouse, MOUSELEFT ) = 0 ;
                    }
                }*/
                break;
        }
    }
}

// Return the total number of active pointers
static int modmulti_numpointers(INSTANCE * my, int * params) {
    return numpointers;
}

// Get some info about the given pointer
static int modmulti_info(INSTANCE * my, int * params) {
    const unsigned char *info = (unsigned char *) string_get(params[1]);
    int n=params[0];

    string_discard(params[1]);

    // Check for failure
    if (n >= MAX_POINTERS || n < 0) {
        return -1;
    }

    // Return the info we were asked for
    if(strncasecmp(info, "X", 1) == 0) {
        return pointers[n].x;
    } else if(strncasecmp(info, "Y", 1) == 0) {
        return pointers[n].y;
    } else if(strncasecmp(info, "PRESSURE", 8) == 0) {
        return pointers[n].pressure;
    } else if(strncasecmp(info, "ACTIVE", 6) == 0) {
        if(pointers[n].active == SDL_TRUE) {
            return 1;
        } else {

            return 0;
        }
    }

    return -1;
}

/* ----------------------------------------------------------------- */

DLSYSFUNCS __bgdexport( mod_multi, functions_exports )[] =
{
    { "MULTI_NUMPOINTERS" , ""      , TYPE_INT    , modmulti_numpointers },
    { "MULTI_INFO"        , "IS"    , TYPE_INT    , modmulti_info        },
    {0, 0, 0, 0}
};

HOOK __bgdexport( mod_multi, handler_hooks )[] =
{
    { 5500, parse_input_events                },
    {    0, NULL                              }
} ;

char * __bgdexport( mod_multi, modules_dependency )[] =
{
    "libsdlhandler",
    NULL
};

/* ----------------------------------------------------------------- */

/*
 SDL_FINGERDOWN      = 0x700,
 SDL_FINGERUP,
 SDL_FINGERMOTION,
 SDL_TOUCHBUTTONDOWN,
 SDL_TOUCHBUTTONUP,

 // Gesture events
SDL_DOLLARGESTURE   = 0x800,
SDL_DOLLARRECORD,
SDL_MULTIGESTURE,
*/
