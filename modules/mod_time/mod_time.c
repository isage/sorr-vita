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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "bgddl.h"
#include "files.h"
#include "xstrings.h"

#include <SDL.h>

#include <time.h>

/* --------------------------------------------------------------------------- */
/* Timer                                                                       */

static int modtime_get_timer( INSTANCE * my, int * params )
{
    return SDL_GetTicks() ;
}

/* --------------------------------------------------------------------------- */
/* Hora del día                                                                */

static int modtime_time( INSTANCE * my, int * params )
{
    return time( 0 ) ;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : modtime_ftime
 *
 *  Returns parts of the date
 *
 *  PARAMS:
 *      no params
 *
 *  RETURN VALUE:
 *      pointer to a string value...
 *
 */

static int modtime_ftime( INSTANCE * my, int * params )
{
    char buffer[128] ;
    char * format ;
    struct tm * t ;
    int ret ;
    time_t tim ;
    char * base ;

#ifdef _WIN32
    /* aux buffer to make all changes... */
    char aux[128] ;
    unsigned char pos ;
#endif

    format = base = strdup( string_get( params[0] ) ) ;
    string_discard( params[0] ) ;

#ifdef _WIN32
    /* Addapting win32 strftime formats to linux formats */
    /* HEAVY PATCH... :( */
    pos = 0 ;
    while ( *format && pos < 127 )
    {
        switch ( *format )
        {
            case '%': /* MIGHT NEED CONVERSION... */
                aux[pos] = *format ;
                pos++ ;
                format++ ;
                switch ( *format )
                {
                    case 'e':
                        aux[pos++] = '#' ;
                        aux[pos] = 'd' ;
                        break ;
                    case 'l':
                        aux[pos++] = '#' ;
                        aux[pos] = 'I' ;
                        break ;
                    case 'k':
                        aux[pos++] = '#' ;
                        aux[pos] = 'H' ;
                        break ;
                    case 'P':
                        aux[pos] = 'p' ;
                        break ;

                    case 'C':
                        aux[pos++] = '%' ;
                        aux[pos++] = *format ;
                        aux[pos++] = '%' ;
                        aux[pos] = 'Y' ;
                        break ;

                    case 'u':
                        aux[pos++] = '%' ;
                        aux[pos++] = *format ;
                        aux[pos++] = '%' ;
                        aux[pos] = 'w' ;
                        break ;

                    case '%':   //MUST BE %%%% TO KEEP 2 IN POSTPROCESS
                        aux[pos++] = '%' ;
                        aux[pos++] = '%' ;
                        aux[pos] = '%' ;
                        break ;

                    default:
                        aux[pos] = *format ;
                        break ;
                }
                break ;

            default: aux[pos] = *format ;
                break ;
        }
        format++ ;
        pos++ ;
    }
    aux[pos] = 0 ;
    format = aux ;
#endif

    tim = ( time_t ) params[1] ;
    t = localtime( &tim ) ;
    strftime( buffer, sizeof( buffer ), format, t ) ;

#ifdef _WIN32
    /* win32 postprocess */
    aux[0] = '\0' ;
    format = buffer ;
    pos = 0 ;
    while ( *format )
    {
        switch ( *format )
        {
            case '%':
                format++ ;
                switch ( *format )
                {
                    case 'u':
                        format++ ;
                        if ( *format == '0' ) *format = '7' ;
                        aux[pos] = *format ;
                        break ;

                    case 'C':
                        format++ ;
                        aux[pos++] = *format ;
                        format++ ;
                        aux[pos] = *format ;
                        format++ ;
                        format++ ;
                        break ;

                    default:
                        aux[pos] = *format ;
                        break ;
                }
                break ;

            default:
                aux[pos] = *format ;
                break ;
        }
        format++ ;
        pos++;
    }
    aux[pos] = '\0' ;
    strcpy( buffer, aux ) ;
#endif

    ret = string_new( buffer ) ;
    string_use( ret ) ;

    free( base ) ;

    return ret ;
}

/* --------------------------------------------------------------------------- */
/* Declaracion de funciones                                                    */

DLSYSFUNCS __bgdexport( mod_time, functions_exports )[] =
{
    /* Fecha/Hora */
    { "GET_TIMER"   , ""    , TYPE_INT      , modtime_get_timer     },
    { "TIME"        , ""    , TYPE_INT      , modtime_time          },
    { "FTIME"       , "SI"  , TYPE_STRING   , modtime_ftime         },
    { 0             , 0     , 0             , 0                     }
};

/* --------------------------------------------------------------------------- */

void __bgdexport( mod_time, module_initialize )()
{
#ifndef TARGET_DINGUX_A320
    if ( !SDL_WasInit( SDL_INIT_TIMER ) ) SDL_InitSubSystem( SDL_INIT_TIMER );
#endif
}

/* --------------------------------------------------------------------------- */

void __bgdexport( mod_time, module_finalize )()
{
#ifndef TARGET_DINGUX_A320
    if ( SDL_WasInit( SDL_INIT_TIMER ) ) SDL_QuitSubSystem( SDL_INIT_TIMER );
#endif
}

/* --------------------------------------------------------------------------- */
