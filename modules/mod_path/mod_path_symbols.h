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

#ifndef __MODPATH_SYMBOLS_H
#define __MODPATH_SYMBOLS_H

#include <bgddl.h>

#define PF_NODIAG       1
#define PF_REVERSE      2

#ifdef __BGDC__
DLCONSTANT __bgdexport( mod_path, constants_def )[] =
{
    { "PF_NODIAG"   , TYPE_INT, PF_NODIAG   },
    { "PF_REVERSE"  , TYPE_INT, PF_REVERSE  },
    { NULL          , 0       , 0           }
} ;

DLSYSFUNCS __bgdexport( mod_path, functions_exports )[] =
{
    /* Path finding */
    { "PATH_FIND"   , "IIIIIII", TYPE_INT   , 0 },
    { "PATH_GETXY"  , "PP"     , TYPE_INT   , 0 },
    { "PATH_WALL"   , "I"      , TYPE_INT   , 0 },
    { 0             , 0        , 0          , 0 }
};

char * __bgdexport( mod_path, modules_dependency )[] =
{
    "libgrbase",
    NULL
};
#else
extern DLCONSTANT __bgdexport( mod_path, constants_def )[];
extern DLSYSFUNCS __bgdexport( mod_path, functions_exports )[];
extern char * __bgdexport( mod_path, modules_dependency )[];
#endif

#endif
