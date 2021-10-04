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

#ifndef __MODJOY_SYMBOLS_H
#define __MODJOY_SYMBOLS_H

#include <bgddl.h>

#ifdef __BGDC__
DLSYSFUNCS  __bgdexport( mod_joy, functions_exports )[] =
{
    { "JOY_AXES"            , ""      , TYPE_INT    , 0 },
    { "JOY_AXES"            , "I"     , TYPE_INT    , 0 },
    { "JOY_NUMAXES"         , ""      , TYPE_INT    , 0 },
    { "JOY_NUMAXES"         , "I"     , TYPE_INT    , 0 },
    { "JOY_GETAXIS"         , "I"     , TYPE_INT    , 0 },
    { "JOY_GETAXIS"         , "II"    , TYPE_INT    , 0 },
    { "JOY_BUTTONS"         , ""      , TYPE_INT    , 0 },
    { "JOY_BUTTONS"         , "I"     , TYPE_INT    , 0 },
    { "JOY_NAME"            , "I"     , TYPE_STRING , 0 },
    { "JOY_NUMBUTTONS"      , ""      , TYPE_INT    , 0 },
    { "JOY_NUMBUTTONS"      , "I"     , TYPE_INT    , 0 },
    { "JOY_NUMBER"          , ""      , TYPE_INT    , 0 },
    { "JOY_NUMJOYSTICKS"    , ""      , TYPE_INT    , 0 },
    { "JOY_SELECT"          , "I"     , TYPE_INT    , 0 },
    { "JOY_GETBUTTON"       , "I"     , TYPE_INT    , 0 },
    { "JOY_GETBUTTON"       , "II"    , TYPE_INT    , 0 },
    { "JOY_GETPOSITION"     , "I"     , TYPE_INT    , 0 },
    { "JOY_GETPOSITION"     , "II"    , TYPE_INT    , 0 },
    { "JOY_NUMHATS"         , ""      , TYPE_INT    , 0 },
    { "JOY_NUMHATS"         , "I"     , TYPE_INT    , 0 },   /* Added by Sandman */
    { "JOY_NUMBALLS"        , ""      , TYPE_INT    , 0 },
    { "JOY_NUMBALLS"        , "I"     , TYPE_INT    , 0 },   /* Added by Sandman */
    { "JOY_GETHAT"          , "I"     , TYPE_INT    , 0 },
    { "JOY_GETHAT"          , "II"    , TYPE_INT    , 0 },   /* Added by Sandman */
    { "JOY_GETBALL"         , "IPP"   , TYPE_INT    , 0 },
    { "JOY_GETBALL"         , "IIPP"  , TYPE_INT    , 0 },   /* Added by Sandman */
    { "JOY_GETACCEL"        , "PPP"   , TYPE_INT    , 0 },
    { "JOY_GETACCEL"        , "IPPP"  , TYPE_INT    , 0 },
    /* Compatibility */
    { "NUMBER_JOY"          , ""      , TYPE_INT    , 0 },
    { "SELECT_JOY"          , "I"     , TYPE_INT    , 0 },
    { "GET_JOY_BUTTON"      , "I"     , TYPE_INT    , 0 },
    { "GET_JOY_BUTTON"      , "II"    , TYPE_INT    , 0 },
    { "GET_JOY_POSITION"    , "I"     , TYPE_INT    , 0 },
    { "GET_JOY_POSITION"    , "II"    , TYPE_INT    , 0 },
    { 0                     , 0       , 0           , 0 }
};

char * __bgdexport( mod_joy, modules_dependency )[] =
{
    "libjoy",
    NULL
};
#else
extern DLSYSFUNCS  __bgdexport( mod_joy, functions_exports )[];
extern char * __bgdexport( mod_joy, modules_dependency )[];
#endif

#endif
