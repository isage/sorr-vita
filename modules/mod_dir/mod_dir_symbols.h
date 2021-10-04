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

#ifndef __MODDIR_SYMBOLS_H
#define __MODDIR_SYMBOLS_H

#include <bgddl.h>

#ifdef __BGDC__
char  __bgdexport( mod_dir, globals_def )[] =
    "STRUCT fileinfo\n"
    "    STRING path;\n"
    "    STRING name;\n"
    "    directory;\n"
    "    hidden;\n"
    "    readonly;\n"
    "    size;\n"
    "    STRING created;\n"
    "    STRING modified;\n"
    "END\n";

DLSYSFUNCS __bgdexport( mod_dir, functions_exports)[] =
    {
        /* Archivos y directorios */
        { "CD"      , ""  , TYPE_STRING , 0 },
        { "CD"      , "S" , TYPE_STRING , 0 },
        { "CHDIR"   , "S" , TYPE_INT    , 0 },
        { "MKDIR"   , "S" , TYPE_INT    , 0 },
        { "RMDIR"   , "S" , TYPE_INT    , 0 },
        { "GLOB"    , "S" , TYPE_STRING , 0 },
        { "CD"      , "S" , TYPE_STRING , 0 },
        { "RM"      , "S" , TYPE_INT    , 0 },
        { "DIROPEN" , "S" , TYPE_INT    , 0 },
        { "DIRCLOSE", "I" , TYPE_INT    , 0 },
        { "DIRREAD" , "I" , TYPE_STRING , 0 },
        { 0         , 0   , 0           , 0 }
    };
#else
extern char  __bgdexport( mod_dir, globals_def )[];
extern DLVARFIXUP __bgdexport( mod_dir, globals_fixup)[];
extern DLSYSFUNCS __bgdexport( mod_dir, functions_exports)[];
#endif

#endif
