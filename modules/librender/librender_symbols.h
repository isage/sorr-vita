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

#ifndef __RENDER_SYMBOLS_H
#define __RENDER_SYMBOLS_H

#include <bgddl.h>
#include "librender.h"

#ifdef __BGDC__
/* --------------------------------------------------------------------------- */

char * __bgdexport( librender, modules_dependency )[] =
{
    "libgrbase",
    "libvideo",
    "libblit",
    NULL
};

/* --------------------------------------------------------------------------- */

DLCONSTANT __bgdexport( librender, constants_def )[] =
{
    { "C_SCREEN",           TYPE_DWORD, C_SCREEN },

    { "PARTIAL_DUMP",       TYPE_DWORD, 0 },
    { "COMPLETE_DUMP",      TYPE_DWORD, 1 },
    { "NO_RESTORE",         TYPE_DWORD, -1 },
    { "PARTIAL_RESTORE",    TYPE_DWORD, 0 },
    { "COMPLETE_RESTORE",   TYPE_DWORD, 1 },

    { "BACKGROUND",         TYPE_DWORD, 0 },
    { "SCREEN",             TYPE_DWORD, -1 },

    { NULL          , 0         ,  0  }
} ;

/* --------------------------------------------------------------------------- */
/* Definicion de variables globales (usada en tiempo de compilacion) */

char __bgdexport( librender, globals_def )[] =

    /* Frame */

    "fps;\n"
    "speed_gauge = 0;\n"
    "FLOAT frame_time = 0;\n"

    /* Screen */

    "restore_type;\n"
    "dump_type;\n"

    /* Fade */

    "fading;\n"
    "alpha_steps = 16;\n"
    ;

/* --------------------------------------------------------------------------- */

char __bgdexport( librender, locals_def )[] =

    /* Render */

    "ctype;\n"
    "cnumber;\n"

    "x;\n"
    "y;\n"
    "z;\n"
    "file;\n"
    "graph;\n"
    "size=100;\n"
    "angle;\n"
    "flags;\n"
    "alpha=255;\n"
    "palette=0;\n"
    "region;\n"
    "resolution;\n"
    "size_x=100;\n"
    "size_y=100;\n"
    "blendop=0;\n"
    "pointer xgraph;\n"

    "STRUCT _render_reserved_\n"
    "object_id=0;\n"
    "graph_ptr=0;\n"
    "xgraph_flags;\n"
    "STRUCT _saved_\n"
    "x;\n"
    "y;\n"
    "z;\n"
    "file;\n"
    "graph;\n"
    "size;\n"
    "angle;\n"
    "flags;\n"
    "alpha;\n"
    "palette;\n"
    "size_x;\n"
    "size_y;\n"
    "blendop;\n"
    "pointer xgraph;\n"
    "centerx;\n"
    "centery;\n"
    "END\n"
    "END\n"
    ;
#else
extern char __bgdexport( librender, modules_dependency )[];
extern DLCONSTANT __bgdexport( librender, constants_def )[];
extern DLVARFIXUP __bgdexport( librender, globals_fixup )[];
extern DLVARFIXUP __bgdexport( librender, locals_fixup )[];
extern char __bgdexport( librender, globals_def )[];
extern char __bgdexport( librender, locals_def )[];
extern HOOK __bgdexport( librender, handler_hooks )[];
extern void __bgdexport( librender, module_initialize )();
extern void __bgdexport( librender, module_finalize )();
extern void __bgdexport( librender, instance_create_hook )( INSTANCE * r );
extern void __bgdexport( librender, instance_destroy_hook )( INSTANCE * r );
#endif

#endif
