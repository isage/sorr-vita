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

#ifndef __MODTEXT_SYMBOLS_H
#define __MODTEXT_SYMBOLS_H

#include <bgddl.h>

#ifdef __BGDC__
#define ALIGN_TOP_LEFT      0
#define ALIGN_TOP           1
#define ALIGN_TOP_RIGHT     2
#define ALIGN_CENTER_LEFT   3
#define ALIGN_CENTER        4
#define ALIGN_CENTER_RIGHT  5
#define ALIGN_BOTTOM_LEFT   6
#define ALIGN_BOTTOM        7
#define ALIGN_BOTTOM_RIGHT  8

DLCONSTANT __bgdexport( mod_text, constants_def)[] =
{
    { "ALL_TEXT"            , TYPE_INT  , 0                     },
    { "ALIGN_TOP_LEFT"      , TYPE_INT  , ALIGN_TOP_LEFT        },
    { "ALIGN_TOP"           , TYPE_INT  , ALIGN_TOP             },
    { "ALIGN_TOP_RIGHT"     , TYPE_INT  , ALIGN_TOP_RIGHT       },
    { "ALIGN_CENTER_LEFT"   , TYPE_INT  , ALIGN_CENTER_LEFT     },
    { "ALIGN_CENTER"        , TYPE_INT  , ALIGN_CENTER          },
    { "ALIGN_CENTER_RIGHT"  , TYPE_INT  , ALIGN_CENTER_RIGHT    },
    { "ALIGN_BOTTOM_LEFT"   , TYPE_INT  , ALIGN_BOTTOM_LEFT     },
    { "ALIGN_BOTTOM"        , TYPE_INT  , ALIGN_BOTTOM          },
    { "ALIGN_BOTTOM_RIGHT"  , TYPE_INT  , ALIGN_BOTTOM_RIGHT    },
    { NULL                  , 0         , 0                     }
} ;

DLSYSFUNCS  __bgdexport( mod_text, functions_exports )[] = {
    { "WRITE"               , "IIIIS"   , TYPE_INT  , 0 },
    { "WRITE"               , "IIIIIS"  , TYPE_INT  , 0 },
    { "WRITE_INT"           , "IIIIP"   , TYPE_INT  , 0 },
    { "WRITE_INT"           , "IIIIIP"  , TYPE_INT  , 0 },
    { "MOVE_TEXT"           , "III"     , TYPE_INT  , 0 },
    { "MOVE_TEXT"           , "IIII"    , TYPE_INT  , 0 },
    { "DELETE_TEXT"         , "I"       , TYPE_INT  , 0 },
    { "WRITE_IN_MAP"        , "ISI"     , TYPE_INT  , 0 },
    { "TEXT_WIDTH"          , "IS"      , TYPE_INT  , 0 },
    { "TEXT_HEIGHT"         , "IS"      , TYPE_INT  , 0 },
    { "GET_TEXT_COLOR"      , ""        , TYPE_INT  , 0 },
    { "GET_TEXT_COLOR"      , "I"       , TYPE_INT  , 0 },
    { "SET_TEXT_COLOR"      , "I"       , TYPE_INT  , 0 },
    { "SET_TEXT_COLOR"      , "II"      , TYPE_INT  , 0 },
    { "WRITE_VAR"           , "IIIIV++" , TYPE_INT  , 0 },
    { "WRITE_VAR"           , "IIIIIV++", TYPE_INT  , 0 },
    { "WRITE_FLOAT"         , "IIIIP"   , TYPE_INT  , 0 },
    { "WRITE_FLOAT"         , "IIIIIP"  , TYPE_INT  , 0 },
    { "WRITE_STRING"        , "IIIIP"   , TYPE_INT  , 0 },
    { "WRITE_STRING"        , "IIIIIP"  , TYPE_INT  , 0 },
    { 0                     , 0         , 0         , 0 }
};

char * __bgdexport( mod_text, modules_dependency)[] =
{
    "libgrbase",
    "libblit",
    "libtext",
    "libfont",
    NULL
};
#else
extern DLCONSTANT __bgdexport( mod_text, constants_def)[];
extern DLSYSFUNCS __bgdexport( mod_text, functions_exports)[];
extern char *     __bgdexport( mod_text, modules_dependency)[];
#endif

#endif
