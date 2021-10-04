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

#ifndef __MODMATHI_H
#define __MODMATHI_H

#include <bgddl.h>

#ifdef __BGDC__
DLCONSTANT __bgdexport( mod_mathi, constants_def )[] =
{
    { "PI"  , TYPE_INT  , 180000    },
    { NULL  , 0         , 0         }
} ;

DLSYSFUNCS __bgdexport( mod_mathi, functions_exports )[] =
{
    { "ABS"         , "F"       , TYPE_FLOAT    , 0 },
    { "POW"         , "FF"      , TYPE_FLOAT    , 0 },
    { "SQRT"        , "F"       , TYPE_FLOAT    , 0 },
    { "COS"         , "F"       , TYPE_FLOAT    , 0 },
    { "SIN"         , "F"       , TYPE_FLOAT    , 0 },
    { "TAN"         , "F"       , TYPE_FLOAT    , 0 },
    { "ACOS"        , "F"       , TYPE_FLOAT    , 0 },
    { "ASIN"        , "F"       , TYPE_FLOAT    , 0 },
    { "ATAN"        , "F"       , TYPE_FLOAT    , 0 },
    { "ATAN2"       , "FF"      , TYPE_FLOAT    , 0 },
    { "ISINF"       , "F"       , TYPE_INT      , 0 },
    { "ISNAN"       , "F"       , TYPE_INT      , 0 },
    { "FINITE"      , "F"       , TYPE_INT      , 0 },
    { "FGET_ANGLE"  , "IIII"    , TYPE_INT      , 0 },
    { "FGET_DIST"   , "IIII"    , TYPE_INT      , 0 },
    { "NEAR_ANGLE"  , "III"     , TYPE_INT      , 0 },
    { "GET_DISTX"   , "II"      , TYPE_INT      , 0 },
    { "GET_DISTY"   , "II"      , TYPE_INT      , 0 },
    { 0             , 0         , 0             , 0 }
};
#else
extern DLCONSTANT __bgdexport( mod_mathi, constants_def )[];
extern DLSYSFUNCS __bgdexport( mod_mathi, functions_exports )[];
#endif

#endif
