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

#ifndef __MODBLENDOP_SYMBOLS_H
#define __MODBLENDOP_SYMBOLS_H

#include <bgddl.h>

#ifdef __BGDC__
/* ---------------------------------------------------------------------- */

DLSYSFUNCS __bgdexport( mod_blendop, functions_exports)[] =
{
    /* Blendops */
    { "BLENDOP_NEW"          , ""      , TYPE_INT   , 0 },
    { "BLENDOP_IDENTITY"     , "I"     , TYPE_INT   , 0 },
    { "BLENDOP_TINT"         , "IFIII" , TYPE_INT   , 0 },
    { "BLENDOP_TRANSLUCENCY" , "IF"    , TYPE_INT   , 0 },
    { "BLENDOP_INTENSITY"    , "IF"    , TYPE_INT   , 0 },
    { "BLENDOP_SWAP"         , "I"     , TYPE_INT   , 0 },
    { "BLENDOP_ASSIGN"       , "III"   , TYPE_INT   , 0 },
    { "BLENDOP_APPLY"        , "III"   , TYPE_INT   , 0 },
    { "BLENDOP_FREE"         , "I"     , TYPE_INT   , 0 },
    { "BLENDOP_GRAYSCALE"    , "II"    , TYPE_INT   , 0 },

    { 0                      , 0       , 0          , 0                                 }
};

/* --------------------------------------------------------------------------- */

char * __bgdexport( mod_blendop, modules_dependency )[] =
{
    "libgrbase",
    NULL
};
#else
extern DLSYSFUNCS __bgdexport( mod_blendop, functions_exports)[];
extern char __bgdexport( mod_blendop, modules_dependency )[];
#endif

#endif
