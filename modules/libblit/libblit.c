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

/* --------------------------------------------------------------------------- */

#include <stdlib.h>

/* --------------------------------------------------------------------------- */

#include "bgddl.h"

#include "libblit.h"

/* --------------------------------------------------------------------------- */

DLCONSTANT __bgdexport( libblit, constants_def )[] =
{
    /* Flags para gr_blit */
    { "B_HMIRROR"           , TYPE_DWORD, B_HMIRROR     },
    { "B_VMIRROR"           , TYPE_DWORD, B_VMIRROR     },
    { "B_TRANSLUCENT"       , TYPE_DWORD, B_TRANSLUCENT },
    { "B_ALPHA"             , TYPE_DWORD, B_ALPHA       },
    { "B_ABLEND"            , TYPE_DWORD, B_ABLEND      },
    { "B_SBLEND"            , TYPE_DWORD, B_SBLEND      },
    { "B_NOCOLORKEY"        , TYPE_DWORD, B_NOCOLORKEY  },

    { NULL                  , 0         ,  0            }
} ;

/* --------------------------------------------------------------------------- */
