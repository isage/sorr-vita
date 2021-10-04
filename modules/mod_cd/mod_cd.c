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

#include <SDL.h>

#include "bgddl.h"
#include "bgdrtm.h"
#include "xstrings.h"
#include "dlvaracc.h"

/* --------------------------------------------------------------------------- */

static SDL_CD * sdl_cd = NULL;
static int      sdl_cdnum = -1;

/* ----------------------------------------------------------------- */

enum {
    CD_TRACK = 0,
    CD_FRAME,
    CD_TRACKS,
    CD_MINUTE,
    CD_SECOND,
    CD_SUBFRAME,
    CD_MINUTES,
    CD_SECONDS,
    CD_FRAMES,
    CD_TRACKINFO
};

/* ----------------------------------------------------------------- */
/* Definicion de constantes (usada en tiempo de compilacion)         */
DLCONSTANT  __bgdexport( mod_cd, constants_def )[] =
{
    { "CD_TRAYEMPTY", TYPE_INT, 0  },
    { "CD_STOPPED"  , TYPE_INT, 1  },
    { "CD_PLAYING"  , TYPE_INT, 2  },
    { "CD_PAUSED"   , TYPE_INT, 3  },
    { "CD_ERROR"    , TYPE_INT, -1 },
    { NULL          , 0       , 0  }
} ;

/* ----------------------------------------------------------------- */
/* Definicion de variables globales (usada en tiempo de compilacion) */
char * __bgdexport( mod_cd, globals_def ) =
    "STRUCT cdinfo\n"
    " current_track;\n"
    " current_frame;\n"
    " tracks;\n"
    " minute;\n"
    " second;\n"
    " subframe;\n"
    " minutes;\n"
    " seconds;\n"
    " subframes;\n"
    " STRUCT track[99]\n"
    "  audio;\n"
    "  minutes;\n"
    "  seconds;\n"
    "  subframes;\n"
    " END;\n"
    "END;\n" ;

/* ----------------------------------------------------------------- */
/* Son las variables que se desea acceder.                           */
/* El interprete completa esta estructura, si la variable existe.    */
/* (usada en tiempo de ejecucion)                                    */
DLVARFIXUP  __bgdexport( mod_cd, globals_fixup )[] =
{
    /* Nombre de variable global, puntero al dato, tamaño del elemento, cantidad de elementos */
    { "cdinfo.current_track", NULL, -1, -1 },
    { "cdinfo.current_frame", NULL, -1, -1 },
    { "cdinfo.tracks", NULL, -1, -1 },
    { "cdinfo.minute", NULL, -1, -1 },
    { "cdinfo.second", NULL, -1, -1 },
    { "cdinfo.subframe", NULL, -1, -1 },
    { "cdinfo.minutes", NULL, -1, -1 },
    { "cdinfo.seconds", NULL, -1, -1 },
    { "cdinfo.subframes", NULL, -1, -1 },
    { "cdinfo.track", NULL, -1, -1 },
    { NULL, NULL, -1, -1 }
};

/* ----------------------------------------------------------------- */
/**
   int CD_DRIVES()
   Returns the number of CD drives in the system
 **/

static int modcd_drives( INSTANCE * my, int * params )
{
    return SDL_CDNumDrives();
}

/* --------------------------------------------------------------------------- */
/**
   int CD_STATUS (int CD)
   Returns the status of a CD (using SDL constants)
 **/

static int modcd_status( INSTANCE * my, int * params )
{
    if ( params[0] < 0 || params[0] >= SDL_CDNumDrives() ) return 0;

    if ( sdl_cd == NULL || sdl_cdnum != params[0] )
    {
        if ( sdl_cd ) SDL_CDClose( sdl_cd );
        sdl_cd = SDL_CDOpen( params[0] );
        if ( sdl_cd == NULL ) return 0;
        sdl_cdnum = params[0];
    }

    return SDL_CDStatus( sdl_cd );
}

/* --------------------------------------------------------------------------- */
/**
   string CD_NAME (int CD)
   Returns a human-readable string with the name of a CD drive
 **/

static int modcd_name( INSTANCE * my, int * params )
{
    int result;

    if ( params[0] < 0 || params[0] >= SDL_CDNumDrives() ) return 0;

    result = string_new( SDL_CDName( params[0] ) );
    string_use( result );
    return result;
}

/* --------------------------------------------------------------------------- */
/**
   CD_GETINFO(int CD)
   Fills the global structure CD with information about the current CD
   Returns 1 if there is a valid CD in the drive or 0 otherwise
 **/

static int modcd_getinfo( INSTANCE * my, int * params )
{
    int i, total = 0;
    char * trackinfo;

    if ( params[0] < 0 || params[0] >= SDL_CDNumDrives() ) return 0;

    if ( sdl_cd == NULL || sdl_cdnum != params[0] )
    {
        if ( sdl_cd ) SDL_CDClose( sdl_cd );
        sdl_cd = SDL_CDOpen( params[0] );
        if ( sdl_cd == NULL ) return 0;
        sdl_cdnum = params[0];
    }

    GLODWORD( mod_cd, CD_TRACKS ) = sdl_cd->numtracks;
    GLODWORD( mod_cd, CD_TRACK )  = sdl_cd->cur_track;
    FRAMES_TO_MSF( sdl_cd->cur_frame, &GLODWORD( mod_cd, CD_MINUTE ), &GLODWORD( mod_cd, CD_SECOND ), &GLODWORD( mod_cd, CD_SUBFRAME ) );

    trackinfo = ( char * ) & GLODWORD( mod_cd, CD_TRACKINFO );

    for ( i = 0; i < sdl_cd->numtracks ; i++, trackinfo += 16 )
    {
        total += sdl_cd->track[i].length;
        *( Uint32 * ) trackinfo = ( sdl_cd->track[i].type == SDL_AUDIO_TRACK );
        FRAMES_TO_MSF( sdl_cd->track[i].length, trackinfo + 4, trackinfo + 8, trackinfo + 12 );
    }
    FRAMES_TO_MSF( total, &GLODWORD( mod_cd, CD_MINUTES ), &GLODWORD( mod_cd, CD_SECONDS ), &GLODWORD( mod_cd, CD_FRAMES ) );
    return 1;
}

/* --------------------------------------------------------------------------- */
/**
   CD_PLAY (int CD, int TRACK)
   Starts playing a track of the given CD
 **/

static int modcd_play( INSTANCE * my, int * params )
{
    if ( params[0] < 0 || params[0] >= SDL_CDNumDrives() ) return 0;

    if ( sdl_cd == NULL || sdl_cdnum != params[0] )
    {
        if ( sdl_cd ) SDL_CDClose( sdl_cd );
        sdl_cd = SDL_CDOpen( params[0] );
        if ( sdl_cd == NULL ) return 0;
        sdl_cdnum = params[0];
    }

    if ( CD_INDRIVE( SDL_CDStatus( sdl_cd ) ) )
        return !SDL_CDPlayTracks( sdl_cd, params[1], 0, 1, 0 );

    return 0;
}

/* --------------------------------------------------------------------------- */
/**
   CD_PLAY (int CD, int TRACK, int NUMTRACKS)
   Plays a series of tracks of the CD
 **/

static int modcd_playtracks( INSTANCE * my, int * params )
{
    if ( params[0] < 0 || params[0] >= SDL_CDNumDrives() ) return 0;

    if ( sdl_cd == NULL || sdl_cdnum != params[0] )
    {
        if ( sdl_cd ) SDL_CDClose( sdl_cd );
        sdl_cd = SDL_CDOpen( params[0] );
        if ( sdl_cd == NULL ) return 0;
        sdl_cdnum = params[0];
    }

    if ( CD_INDRIVE( SDL_CDStatus( sdl_cd ) ) )
        return !SDL_CDPlayTracks( sdl_cd, params[1], 0, params[2], 0 );

    return 0;
}

/* --------------------------------------------------------------------------- */
/**
   CD_EJECT (int CD)
   Ejects a CD
 **/

static int modcd_eject( INSTANCE * my, int * params )
{
    if ( params[0] < 0 || params[0] >= SDL_CDNumDrives() ) return 0;

    if ( sdl_cd == NULL || sdl_cdnum != params[0] )
    {
        if ( sdl_cd ) SDL_CDClose( sdl_cd );
        sdl_cd = SDL_CDOpen( params[0] );
        if ( sdl_cd == NULL ) return 0;
        sdl_cdnum = params[0];
    }

    return !SDL_CDEject( sdl_cd );
}

/* --------------------------------------------------------------------------- */
/**
   CD_PAUSE (int CD)
   Pauses the CD playing
 **/

static int modcd_pause( INSTANCE * my, int * params )
{
    if ( params[0] < 0 || params[0] >= SDL_CDNumDrives() ) return 0;

    if ( sdl_cd == NULL || sdl_cdnum != params[0] )
    {
        if ( sdl_cd ) SDL_CDClose( sdl_cd );
        sdl_cd = SDL_CDOpen( params[0] );
        if ( sdl_cd == NULL ) return 0;
        sdl_cdnum = params[0];
    }

    return !SDL_CDPause( sdl_cd );
}

/* --------------------------------------------------------------------------- */
/**
   CD_RESUME (int CD)
   Resumes a CD in pause
 **/

static int modcd_resume( INSTANCE * my, int * params )
{
    if ( params[0] < 0 || params[0] >= SDL_CDNumDrives() ) return 0;

    if ( sdl_cd == NULL || sdl_cdnum != params[0] )
    {
        if ( sdl_cd ) SDL_CDClose( sdl_cd );
        sdl_cd = SDL_CDOpen( params[0] );
        if ( sdl_cd == NULL ) return 0;
        sdl_cdnum = params[0];
    }

    return !SDL_CDResume( sdl_cd );
}

/* --------------------------------------------------------------------------- */
/**
   CD_STOP (int CD)
   Stops the CD
 **/

static int modcd_stop( INSTANCE * my, int * params )
{
    if ( params[0] < 0 || params[0] >= SDL_CDNumDrives() ) return 0;

    if ( sdl_cd == NULL || sdl_cdnum != params[0] )
    {
        if ( sdl_cd ) SDL_CDClose( sdl_cd );
        sdl_cd = SDL_CDOpen( params[0] );
        if ( sdl_cd == NULL ) return 0;
        sdl_cdnum = params[0];
    }

    return !SDL_CDStop( sdl_cd );
}

/* --------------------------------------------------------------------------- */

DLSYSFUNCS  __bgdexport( mod_cd, functions_exports )[] =
{
    /* Funciones de manejo de CD */
    { "CD_DRIVES"   , ""      , TYPE_INT    , modcd_drives     },
    { "CD_STATUS"   , "I"     , TYPE_INT    , modcd_status     },
    { "CD_NAME"     , "I"     , TYPE_STRING , modcd_name       },
    { "CD_GETINFO"  , "I"     , TYPE_INT    , modcd_getinfo    },
    { "CD_PLAY"     , "II"    , TYPE_INT    , modcd_play       },
    { "CD_PLAY"     , "III"   , TYPE_INT    , modcd_playtracks },
    { "CD_STOP"     , "I"     , TYPE_INT    , modcd_stop       },
    { "CD_PAUSE"    , "I"     , TYPE_INT    , modcd_pause      },
    { "CD_RESUME"   , "I"     , TYPE_INT    , modcd_resume     },
    { "CD_EJECT"    , "I"     , TYPE_INT    , modcd_eject      },
    { 0             , 0       , 0           , 0                }
};

/* --------------------------------------------------------------------------- */
/* Funciones de inicializacion del modulo/plugin                               */

void  __bgdexport( mod_cd, module_initialize )()
{
    if ( !SDL_WasInit( SDL_INIT_CDROM ) ) SDL_InitSubSystem( SDL_INIT_CDROM );
}

/* --------------------------------------------------------------------------- */

void  __bgdexport( mod_cd, module_finalize )()
{
    if ( SDL_WasInit( SDL_INIT_CDROM ) ) SDL_QuitSubSystem( SDL_INIT_CDROM );
}

/* --------------------------------------------------------------------------- */
