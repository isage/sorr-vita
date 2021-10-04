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

#ifndef __VIDEO_SYMBOLS_H
#define __VIDEO_SYMBOLS_H

#include <bgddl.h>
#include "g_video.h"

#ifdef __BGDC__
/* --------------------------------------------------------------------------- */
/* Definicion de variables globales (usada en tiempo de compilacion) */

char __bgdexport( libvideo, globals_def )[] =
    "graph_mode = 0;\n"
    "scale_mode = 0;\n"
    "full_screen = 0;\n"
    "scale_resolution = 0;\n"
    "scale_resolution_aspectratio = 0;\n"
    "scale_resolution_orientation = 0;\n"
    ;

/* --------------------------------------------------------------------------- */

char *__bgdexport( libvideo, modules_dependency )[] =
{
    "libgrbase",
    NULL
};

/* --------------------------------------------------------------------------- */

DLCONSTANT  __bgdexport( libvideo, constants_def )[] =
{
    { "M320X200",           TYPE_DWORD, 3200200             },
    { "M320X240",           TYPE_DWORD, 3200240             },
    { "M320X400",           TYPE_DWORD, 3200400             },
    { "M360X240",           TYPE_DWORD, 3600240             },
    { "M376X282",           TYPE_DWORD, 3760282             },
    { "M400X300",           TYPE_DWORD, 4000300             },
    { "M512X384",           TYPE_DWORD, 5120384             },
    { "M640X400",           TYPE_DWORD, 6400400             },
    { "M640X480",           TYPE_DWORD, 6400480             },
    { "M800X600",           TYPE_DWORD, 8000600             },
    { "M1024X768",          TYPE_DWORD, 10240768            },
    { "M1280X1024",         TYPE_DWORD, 12801024            },

    { "MODE_WINDOW",        TYPE_DWORD, MODE_WINDOW         },
    { "MODE_2XSCALE",       TYPE_DWORD, MODE_2XSCALE        },
    { "MODE_FULLSCREEN",    TYPE_DWORD, MODE_FULLSCREEN     },
    { "MODE_DOUBLEBUFFER",  TYPE_DWORD, MODE_DOUBLEBUFFER   },
    { "MODE_HARDWARE",      TYPE_DWORD, MODE_HARDWARE       },

    { "MODE_WAITVSYNC",     TYPE_DWORD, MODE_WAITVSYNC      },
    { "WAITVSYNC",          TYPE_DWORD, MODE_WAITVSYNC      },

    { "DOUBLE_BUFFER",      TYPE_DWORD, MODE_DOUBLEBUFFER   },  /* Obsolete */
    { "HW_SURFACE",         TYPE_DWORD, MODE_HARDWARE       },  /* Obsolete */

    { "MODE_8BITS",         TYPE_DWORD, 8                   },
    { "MODE_16BITS",        TYPE_DWORD, 16                  },
    { "MODE_32BITS",        TYPE_DWORD, 32                  },

    { "MODE_8BPP",          TYPE_DWORD, 8                   },
    { "MODE_16BPP",         TYPE_DWORD, 16                  },
    { "MODE_32BPP",         TYPE_DWORD, 32                  },

    { "MODE_MODAL",         TYPE_DWORD, MODE_MODAL          },  /* GRAB INPUT */
    { "MODE_FRAMELESS",     TYPE_DWORD, MODE_FRAMELESS      },  /* FRAMELESS window */

    { "SCALE_NONE",         TYPE_DWORD, SCALE_NONE          },

	{ "SRO_NORMAL",         TYPE_DWORD, SRO_NORMAL          },
	{ "SRO_LEFT",           TYPE_DWORD, SRO_LEFT            },
	{ "SRO_DOWN",           TYPE_DWORD, SRO_DOWN            },
	{ "SRO_RIGHT",          TYPE_DWORD, SRO_RIGHT           },
	
	{ "SRA_STRETCH",        TYPE_DWORD, SRA_STRETCH         },
	{ "SRA_PRESERVE",       TYPE_DWORD, SRA_PRESERVE        },
	
    { NULL          , 0         ,  0  }
} ;
#else
extern char __bgdexport( libvideo, globals_def )[];
extern char __bgdexport( libvideo, modules_dependency )[];
extern DLCONSTANT  __bgdexport( libvideo, constants_def )[];
extern DLVARFIXUP __bgdexport( libvideo, globals_fixup )[];
extern void __bgdexport( libvideo, module_initialize )();
extern void __bgdexport( libvideo, module_finalize )();
#endif

#endif
