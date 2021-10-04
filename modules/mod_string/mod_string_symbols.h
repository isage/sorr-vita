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

#ifndef __MODSTRING_H
#define __MODSTRING_H

#include <bgddl.h>

#ifdef __BGDC__
DLSYSFUNCS  __bgdexport( mod_string, functions_exports )[] =
{
    { "STRLEN"     , "S"   , TYPE_INT   , 0 },
    { "LEN"        , "S"   , TYPE_INT   , 0 },
    { "UCASE"      , "S"   , TYPE_STRING, 0 },
    { "LCASE"      , "S"   , TYPE_STRING, 0 },
    { "STRCASECMP" , "SS"  , TYPE_INT   , 0 },
    { "SUBSTR"     , "SII" , TYPE_STRING, 0 },
    { "SUBSTR"     , "SI"  , TYPE_STRING, 0 },
    { "FIND"       , "SS"  , TYPE_INT   , 0 },
    { "FIND"       , "SSI" , TYPE_INT   , 0 },
    { "LPAD"       , "SI"  , TYPE_STRING, 0 },
    { "RPAD"       , "SI"  , TYPE_STRING, 0 },
    { "ITOA"       , "I"   , TYPE_STRING, 0 },
    { "FTOA"       , "F"   , TYPE_STRING, 0 },
    { "ATOI"       , "S"   , TYPE_INT   , 0 },
    { "ATOF"       , "S"   , TYPE_FLOAT , 0 },
    { "ASC"        , "S"   , TYPE_BYTE  , 0 },
    { "CHR"        , "I"   , TYPE_STRING, 0 },
    { "TRIM"       , "S"   , TYPE_STRING, 0 },
    { "STRREV"     , "S"   , TYPE_STRING, 0 },
    { "FORMAT"     , "I"   , TYPE_STRING, 0 },
    { "FORMAT"     , "F"   , TYPE_STRING, 0 },
    { "FORMAT"     , "FI"  , TYPE_STRING, 0 },
    { 0            , 0     , 0          , 0 }
};
#else
extern DLSYSFUNCS  __bgdexport( mod_string, functions_exports )[];
#endif

#endif
