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

#ifndef __SCROLL_SYMBOLS_H
#define __SCROLL_SYMBOLS_H

#include <bgddl.h>
#include "libscroll.h"

#ifdef __BGDC__
/* --------------------------------------------------------------------------- */

DLCONSTANT __bgdexport( libscroll, constants_def )[] =
{
    { "C_SCROLL",   TYPE_DWORD,     C_SCROLL    },

    { "C_0",        TYPE_DWORD,     0x0001      },
    { "C_1",        TYPE_DWORD,     0x0002      },
    { "C_2",        TYPE_DWORD,     0x0004      },
    { "C_3",        TYPE_DWORD,     0x0008      },
    { "C_4",        TYPE_DWORD,     0x0010      },
    { "C_5",        TYPE_DWORD,     0x0020      },
    { "C_6",        TYPE_DWORD,     0x0040      },
    { "C_7",        TYPE_DWORD,     0x0080      },
    { "C_8",        TYPE_DWORD,     0x0100      },
    { "C_9",        TYPE_DWORD,     0x0200      },

    { NULL,         0,              0           }
};

/* --------------------------------------------------------------------------- */

char __bgdexport( libscroll, locals_def )[] =
    "ctype;\n"
    "cnumber;\n";

/* --------------------------------------------------------------------------- */

char __bgdexport( libscroll, globals_def )[] =
    "STRUCT scroll[9]\n"
    "x0, y0;\n"
    "x1, y1;\n"
    "z = 512;\n"
    "camera;\n"
    "ratio = 200;\n"
    "speed;\n"
    "region1 = -1;\n"
    "region2 = -1;\n"
    "flags1;\n"
    "flags2;\n"
    "follow = -1;\n"
    "reserved[6];\n"  /* size: 20 dwords */
    "END \n";

/* --------------------------------------------------------------------------- */

char * __bgdexport( libscroll, modules_dependency )[] =
{
    "libgrbase",
    "libblit",
    "librender",
    "libvideo",
    NULL
};
#else
extern DLCONSTANT __bgdexport( libscroll, constants_def )[];
extern char __bgdexport( libscroll, locals_def )[];
extern char __bgdexport( libscroll, globals_def )[];
extern DLVARFIXUP __bgdexport( libscroll, locals_fixup )[];
extern DLVARFIXUP __bgdexport( libscroll, globals_fixup )[];
extern char __bgdexport( libscroll, modules_dependency )[];
#endif

#endif
