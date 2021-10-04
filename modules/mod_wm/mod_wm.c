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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bgddl.h"
#include "bgdrtm.h"
#include "xstrings.h"

#include "libgrbase.h"
#include "libvideo.h"

#include <SDL.h>

#if defined( WIN32 ) || ( __linux && ( defined( SDL_VIDEO_DRIVER_X11 ) ) )
#include <SDL_syswm.h>
#endif

/* --------------------------------------------------------------------------- */
/* Window Manager                                                              */
/* --------------------------------------------------------------------------- */

static int bgd_set_title( INSTANCE * my, int * params )
{
    const char *title = string_get(params[0]);
    SDL_Log("set_title(%s)", title);
    gr_set_caption( ( char * )title ) ;
    string_discard(params[0]);
    return 1 ;
}

/* --------------------------------------------------------------------------- */

static int bgd_set_icon( INSTANCE * my, int * params )
{
    gr_set_icon( bitmap_get( params[0], params[1] ) );
    return 1 ;
}

/* --------------------------------------------------------------------------- */

static int bgd_minimize( INSTANCE * my, int * params )
{
    SDL_MinimizeWindow(window);
    return 1;
}

/* --------------------------------------------------------------------------- */

static int bgd_move_window( INSTANCE * my, int * params )
{
    int res = 0;
    if ( full_screen ) return 0;

    SDL_SetWindowPosition(window, params[0], params[1]);

    return res ;
}

/* --------------------------------------------------------------------------- */

static int bgd_get_window_pos( INSTANCE * my, int * params )
{
    if ( full_screen ) return -1;

    int x,y;

    SDL_GetWindowPosition(window, &x, &y );
    if ( params[0] ) *(( int * )( params[0] ) ) = x;
    if ( params[1] ) *(( int * )( params[1] ) ) = y;

    return 1 ;
}

/* --------------------------------------------------------------------------- */

static int bgd_get_window_size( INSTANCE * my, int * params )
{
    int w,h;

    SDL_GetWindowSize(window, &w, &h );
    if ( params[0] ) *(( int * )( params[0] ) ) = w;
    if ( params[1] ) *(( int * )( params[1] ) ) = h;
    //FIXME: Get this working
    if ( params[2] ) *(( int * )( params[2] ) ) = w;
    if ( params[3] ) *(( int * )( params[3] ) ) = h;

    return 1 ;
}

/* --------------------------------------------------------------------------- */

static int bgd_get_desktop_size( INSTANCE * my, int * params )
{
    SDL_DisplayMode mode;

    if(SDL_GetDesktopDisplayMode(0, &mode) < 0 ) return -1;

    if ( params[0] ) *(( int * )( params[0] ) ) = mode.w;
    if ( params[1] ) *(( int * )( params[1] ) ) = mode.h;

    return 1 ;
}

/* --------------------------------------------------------------------------- */

DLSYSFUNCS __bgdexport( mod_wm, functions_exports )[] =
{
    /* Funciones de ventana */
    { "SET_TITLE"           , "S"   , TYPE_INT , bgd_set_title          },
    { "SET_ICON"            , "II"  , TYPE_INT , bgd_set_icon           },
    { "MINIMIZE"            , ""    , TYPE_INT , bgd_minimize           },
    { "MOVE_WINDOW"         , "II"  , TYPE_INT , bgd_move_window        },
    { "SET_WINDOW_POS"      , "II"  , TYPE_INT , bgd_move_window        },
    { "GET_WINDOW_POS"      , "PP"  , TYPE_INT , bgd_get_window_pos     },
    { "GET_WINDOW_SIZE"     , "PPPP", TYPE_INT , bgd_get_window_size    },
    { "GET_DESKTOP_SIZE"    , "PP"  , TYPE_INT , bgd_get_desktop_size   },
    { 0                     , 0     , 0        , 0                      }
};

/* --------------------------------------------------------------------------- */

char * __bgdexport( mod_wm, modules_dependency )[] =
{
    "libgrbase",
    "libvideo",
    "libwm",
    NULL
};

/* --------------------------------------------------------------------------- */
