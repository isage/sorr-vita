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

#ifndef __MODDRAW_SYMBOLS_H
#define __MODDRAW_SYMBOLS_H

#include <bgddl.h>

#ifdef __BGDC__
DLSYSFUNCS __bgdexport( mod_draw, functions_exports )[] =
{
    /* Funciones de primitivas */
    { "DRAWING_MAP"     , "II"          , TYPE_INT  , 0 },
    { "DRAWING_COLOR"   , "I"           , TYPE_INT  , 0 },
    { "DRAW_LINE"       , "IIII"        , TYPE_INT  , 0 },
    { "DRAW_RECT"       , "IIII"        , TYPE_INT  , 0 },
    { "DRAW_BOX"        , "IIII"        , TYPE_INT  , 0 },
    { "DRAW_CIRCLE"     , "III"         , TYPE_INT  , 0 },
    { "DRAW_FCIRCLE"    , "III"         , TYPE_INT  , 0 },
    { "DRAW_CURVE"      , "IIIIIIIII"   , TYPE_INT  , 0 },
    { "DRAWING_Z"       , "I"           , TYPE_INT  , 0 },
    { "DELETE_DRAW"     , "I"           , TYPE_INT  , 0 },
    { "MOVE_DRAW"       , "III"         , TYPE_INT  , 0 },
    { "DRAWING_ALPHA"   , "I"           , TYPE_INT  , 0 },
    { "DRAWING_STIPPLE" , "I"           , TYPE_INT  , 0 },
    { "PUT_PIXEL"       , "III"         , TYPE_INT  , 0 },
    { "GET_PIXEL"       , "II"          , TYPE_INT  , 0 },
    { "MAP_GET_PIXEL"   , "IIII"        , TYPE_INT  , 0 },
    { "MAP_PUT_PIXEL"   , "IIIII"       , TYPE_INT  , 0 },
    { NULL              , NULL          , 0         , NULL }
};

char * __bgdexport( mod_draw, modules_dependency )[] =
{
    "libgrbase",
    "librender",
    "libdraw",
    NULL
};
#else
extern DLSYSFUNCS __bgdexport( mod_draw, functions_exports )[];
extern char * __bgdexport( mod_draw, modules_dependency )[];
#endif

#endif
