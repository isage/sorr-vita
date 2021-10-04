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

#ifndef __MODWM_SYMBOLS_H
#define __MODWM_SYMBOLS_H

#include <bgddl.h>

#ifdef __BGDC__
DLSYSFUNCS __bgdexport( mod_wm, functions_exports )[] =
{
    /* Funciones de ventana */
    { "SET_TITLE"           , "S"   , TYPE_INT , 0 },
    { "SET_ICON"            , "II"  , TYPE_INT , 0 },
    { "MINIMIZE"            , ""    , TYPE_INT , 0 },
    { "MOVE_WINDOW"         , "II"  , TYPE_INT , 0 },
    { "SET_WINDOW_POS"      , "II"  , TYPE_INT , 0 },
    { "GET_WINDOW_POS"      , "PP"  , TYPE_INT , 0 },
    { "GET_WINDOW_SIZE"     , "PPPP", TYPE_INT , 0 },
    { "GET_DESKTOP_SIZE"    , "PP"  , TYPE_INT , 0 },
    { 0                     , 0     , 0        , 0 }
};

char * __bgdexport( mod_wm, modules_dependency )[] =
{
    "libgrbase",
    "libvideo",
    "libwm",
    NULL
};
#else
extern DLSYSFUNCS __bgdexport( mod_wm, functions_exports )[];
extern char * __bgdexport( mod_wm, modules_dependency )[];
#endif

#endif
