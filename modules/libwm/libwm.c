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

/* --------------------------------------------------------------------------- */

#include <SDL.h>

#define __LIB_RENDER
#include <g_video.h>
#include <librender.h>

#include "bgdrtm.h"

#include "bgddl.h"
#include "dlvaracc.h"

/* --------------------------------------------------------------------------- */

#define EXIT_STATUS         0
#define WINDOW_STATUS       1
#define FOCUS_STATUS        2
#define MOUSE_STATUS        3

/* --------------------------------------------------------------------------- */
/* Definicion de variables globales (usada en tiempo de compilacion) */

char * __bgdexport( libwm, globals_def ) =
    "exit_status = 0;\n"                /* SDL_QUIT status */
    "window_status = 1;\n"              /* MINIMIZED:0 VISIBLE:1 */
    "focus_status = 1;\n"               /* FOCUS status */
    "mouse_status = 1;\n";              /* MOUSE status (INSIDE WINDOW:1) */

/* --------------------------------------------------------------------------- */
/* Son las variables que se desea acceder.                           */
/* El interprete completa esta estructura, si la variable existe.    */
/* (usada en tiempo de ejecucion)                                    */

DLVARFIXUP  __bgdexport( libwm, globals_fixup )[] =
{
    /* Nombre de variable global, puntero al dato, tamaño del elemento, cantidad de elementos */
    { "exit_status"     , NULL, -1, -1 },
    { "window_status"   , NULL, -1, -1 },
    { "focus_status"    , NULL, -1, -1 },
    { "mouse_status"    , NULL, -1, -1 },
    { NULL              , NULL, -1, -1 }
};

/* --------------------------------------------------------------------------- */
/* Gestión de eventos de ventana                                               */
/* --------------------------------------------------------------------------- */

/*
 *  FUNCTION : wm_events
 *
 *  Process all pending wm SDL events, updating all global variables
 *
 *  PARAMS :
 *      None
 *
 *  RETURN VALUE :
 *      None
 */

static void wm_events()
{
    SDL_Event e ;
    
    /* Procesa los eventos de ventana pendientes */

    GLODWORD( libwm, EXIT_STATUS ) = 0 ;

    /* Handle the plethora of events different systems can produce here */
    while ( SDL_PeepEvents( &e, 1, SDL_GETEVENT, SDL_QUIT, SDL_WINDOWEVENT ) > 0 )
    {
        switch ( e.type )
        {
            case SDL_QUIT:
                /* UPDATE  exit status... initilized each frame */
                GLODWORD( libwm, EXIT_STATUS ) = 1 ;
                break ;
            case SDL_WINDOWEVENT:
                switch (e.window.event) {
                    case SDL_WINDOWEVENT_MINIMIZED:
                        GLODWORD(libwm, WINDOW_STATUS) = 0;
                        break;
                    case SDL_WINDOWEVENT_RESTORED:
                        gr_set_mode(screen->w, screen->h, screen->format->BitsPerPixel);
                        GLODWORD(libwm, WINDOW_STATUS) = 1;
                        break;
                    case SDL_WINDOWEVENT_ENTER:
                        GLODWORD(libwm, MOUSE_STATUS) = 1;
                        break;
                    case SDL_WINDOWEVENT_LEAVE:
                        GLODWORD(libwm, MOUSE_STATUS) = 0;
                        break;
                    case SDL_WINDOWEVENT_HIDDEN:
                        GLODWORD(libwm, WINDOW_STATUS) = 0;
                        break;
                    case SDL_WINDOWEVENT_SHOWN:
                        GLODWORD(libwm, WINDOW_STATUS) = 1;
                        break;
                    case SDL_WINDOWEVENT_FOCUS_LOST:
                        GLODWORD(libwm, FOCUS_STATUS) = 0;
                        break;
                    case SDL_WINDOWEVENT_FOCUS_GAINED:
                        GLODWORD(libwm, FOCUS_STATUS) = 1;
                        break;
                }
            
            case SDL_SYSWMEVENT:
                break ;
        }
    }
}

/* --------------------------------------------------------------------------- */

/* Bigest priority first execute
   Lowest priority last execute */

HOOK __bgdexport( libwm, handler_hooks )[] =
{
    { 4700, wm_events   },
    {    0, NULL        }
} ;

/* --------------------------------------------------------------------------- */

char * __bgdexport( libwm, modules_dependency )[] =
{
#if defined(TARGET_IOS)
    "libvideo",
    "librender",
#endif
    "libsdlhandler",
    NULL
};

/* --------------------------------------------------------------------------- */
