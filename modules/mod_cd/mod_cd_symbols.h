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

#ifndef __MODCD_SYMBOLS_H
#define __MODCD_SYMBOLS_H

#include <bgddl.h>

#ifdef __BGDC__
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
char __bgdexport( mod_cd, globals_def )[] =
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

/* --------------------------------------------------------------------------- */

DLSYSFUNCS  __bgdexport( mod_cd, functions_exports )[] =
{
    /* Funciones de manejo de CD */
    { "CD_DRIVES"   , ""      , TYPE_INT    , SYSMACRO(modcd_drives)     },
    { "CD_STATUS"   , "I"     , TYPE_INT    , SYSMACRO(modcd_status)     },
    { "CD_NAME"     , "I"     , TYPE_STRING , SYSMACRO(modcd_name)       },
    { "CD_GETINFO"  , "I"     , TYPE_INT    , SYSMACRO(modcd_getinfo)    },
    { "CD_PLAY"     , "II"    , TYPE_INT    , SYSMACRO(modcd_play)       },
    { "CD_PLAY"     , "III"   , TYPE_INT    , SYSMACRO(modcd_playtracks) },
    { "CD_STOP"     , "I"     , TYPE_INT    , SYSMACRO(modcd_stop)       },
    { "CD_PAUSE"    , "I"     , TYPE_INT    , SYSMACRO(modcd_pause)      },
    { "CD_RESUME"   , "I"     , TYPE_INT    , SYSMACRO(modcd_resume)     },
    { "CD_EJECT"    , "I"     , TYPE_INT    , SYSMACRO(modcd_eject)      },
    { 0             , 0       , 0           , 0                }
};
#else
extern DLCONSTANT  __bgdexport( mod_cd, constants_def )[];
extern DLVARFIXUP  __bgdexport( mod_cd, globals_fixup )[];
extern DLSYSFUNCS  __bgdexport( mod_cd, functions_exports )[];
extern char  __bgdexport( mod_cd, globals_def )[];
extern void  __bgdexport( mod_cd, module_initialize )();
extern void  __bgdexport( mod_cd, module_finalize )();
#endif

#endif
