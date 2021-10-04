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

#ifndef __WM_SYMBOLS_H
#define __WM_SYMBOLS_H

#include <bgddl.h>

#ifdef __BGDC__
/* --------------------------------------------------------------------------- */
/* Definicion de variables globales (usada en tiempo de compilacion) */

char __bgdexport( libwm, globals_def )[] =
"exit_status = 0;\n"                /* SDL_QUIT status */
"window_status = 1;\n"              /* MINIMIZED:0 VISIBLE:1 */
"focus_status = 1;\n"               /* FOCUS status */
"mouse_status = 1;\n";              /* MOUSE status (INSIDE WINDOW:1) */

/* --------------------------------------------------------------------------- */

char * __bgdexport( libwm, modules_dependency )[] =
{
    "libsdlhandler",
    NULL
};
#else
extern char __bgdexport( libwm, globals_def )[];
extern char __bgdexport( libwm, modules_dependency )[];
extern DLVARFIXUP  __bgdexport( libwm, globals_fixup )[];
extern HOOK __bgdexport( libwm, handler_hooks )[];
#endif

#endif
