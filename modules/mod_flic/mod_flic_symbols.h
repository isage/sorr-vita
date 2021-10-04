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

#ifndef __MODFLIC_SYMBOLS_H
#define __MODFLIC_SYMBOLS_H

#include <bgddl.h>

#ifdef __BGDC__
DLSYSFUNCS  __bgdexport( mod_flic, functions_exports )[] =
{
    /* Animaciones FLI */
    { "START_FLI"       , "SII"         , TYPE_INT , 0 },
    { "END_FLI"         , ""            , TYPE_INT , 0 },
    { "FRAME_FLI"       , ""            , TYPE_INT , 0 },
    { "RESET_FLI"       , ""            , TYPE_INT , 0 },

    { "FLI_START"       , "SII"         , TYPE_INT , 0 },
    { "FLI_START"       , "SIIIIII"     , TYPE_INT , 0 },
    { "FLI_END"         , "I"           , TYPE_INT , 0 },
    { "FLI_FRAME"       , "I"           , TYPE_INT , 0 },
    { "FLI_RESET"       , "I"           , TYPE_INT , 0 },

    { "FLI_PARAMS"      , "IIIIIII"     , TYPE_INT , 0 },
    { "FLI_MOVE"        , "III"         , TYPE_INT , 0 },
    { "FLI_ANGLE"       , "II"          , TYPE_INT , 0 },
    { "FLI_SIZE"        , "II"          , TYPE_INT , 0 },
    { "FLI_FLAGS"       , "II"          , TYPE_INT , 0 },
    { "FLI_Z"           , "II"          , TYPE_INT , 0 },
    { "FLI_GETINFO"     , "IPPPPPPPPP"  , TYPE_INT , 0 },

    { NULL              , NULL          , 0        , NULL }
};
#else
extern DLSYSFUNCS  __bgdexport( mod_flic, functions_exports )[];
#endif

#endif
