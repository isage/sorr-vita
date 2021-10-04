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

#ifndef __KEY_SYMBOLS_H
#define __KEY_SYMBOLS_H

#include <bgddl.h>

#ifdef __BGDC__
#define STAT_RSHIFT             0x0000001
#define STAT_LSHIFT             0x0000002
#define STAT_CTRL               0x0000004
#define STAT_ALT                0x0000008
#define STAT_RCTRL              0x0000010
#define STAT_LCTRL              0x0000020
#define STAT_RALT               0x0000040
#define STAT_LALT               0x0000080
#define STAT_NUM                0x0000100
#define STAT_CAPS               0x0000200
#define STAT_SHIFT              0x0000400
/* ----------------------------------------------------------------- */

DLCONSTANT  __bgdexport( libkey, constants_def )[] =
{
    { "_ESC",         TYPE_DWORD,  1 },
    { "_1",           TYPE_DWORD,  2 },
    { "_2",           TYPE_DWORD,  3 },
    { "_3",           TYPE_DWORD,  4 },
    { "_4",           TYPE_DWORD,  5 },
    { "_5",           TYPE_DWORD,  6 },
    { "_6",           TYPE_DWORD,  7 },
    { "_7",           TYPE_DWORD,  8 },
    { "_8",           TYPE_DWORD,  9 },
    { "_9",           TYPE_DWORD,  10 },
    { "_0",           TYPE_DWORD,  11 },
    { "_MINUS",       TYPE_DWORD,  12 },
    { "_PLUS",        TYPE_DWORD,  13 },
    { "_BACKSPACE",   TYPE_DWORD,  14 },
    { "_TAB",         TYPE_DWORD,  15 },
    { "_Q",           TYPE_DWORD,  16 },
    { "_W",           TYPE_DWORD,  17 },
    { "_E",           TYPE_DWORD,  18 },
    { "_R",           TYPE_DWORD,  19 },
    { "_T",           TYPE_DWORD,  20 },
    { "_Y",           TYPE_DWORD,  21 },
    { "_U",           TYPE_DWORD,  22 },
    { "_I",           TYPE_DWORD,  23 },
    { "_O",           TYPE_DWORD,  24 },
    { "_P",           TYPE_DWORD,  25 },
    { "_L_BRACKET",   TYPE_DWORD,  26 },
    { "_R_BRACKET",   TYPE_DWORD,  27 },
    { "_ENTER",       TYPE_DWORD,  28 },
    { "_C_ENTER",     TYPE_DWORD,  28 },
    { "_CONTROL",     TYPE_DWORD,  29 },
    { "_A",           TYPE_DWORD,  30 },
    { "_S",           TYPE_DWORD,  31 },
    { "_D",           TYPE_DWORD,  32 },
    { "_F",           TYPE_DWORD,  33 },
    { "_G",           TYPE_DWORD,  34 },
    { "_H",           TYPE_DWORD,  35 },
    { "_J",           TYPE_DWORD,  36 },
    { "_K",           TYPE_DWORD,  37 },
    { "_L",           TYPE_DWORD,  38 },
    { "_SEMICOLON",   TYPE_DWORD,  39 },
    { "_APOSTROPHE",  TYPE_DWORD,  40 },
    { "_WAVE",        TYPE_DWORD,  41 },
    { "_L_SHIFT",     TYPE_DWORD,  42 },
    { "_BACKSLASH",   TYPE_DWORD,  43 },
    { "_Z",           TYPE_DWORD,  44 },
    { "_X",           TYPE_DWORD,  45 },
    { "_C",           TYPE_DWORD,  46 },
    { "_V",           TYPE_DWORD,  47 },
    { "_B",           TYPE_DWORD,  48 },
    { "_N",           TYPE_DWORD,  49 },
    { "_M",           TYPE_DWORD,  50 },
    { "_COMMA",       TYPE_DWORD,  51 },
    { "_POINT",       TYPE_DWORD,  52 },
    { "_SLASH",       TYPE_DWORD,  53 },
    { "_C_BACKSLASH", TYPE_DWORD,  53 },
    { "_R_SHIFT",     TYPE_DWORD,  54 },
    { "_C_ASTERISK",  TYPE_DWORD,  55 },
    { "_PRN_SCR",     TYPE_DWORD,  55 },
    { "_ALT",         TYPE_DWORD,  56 },
    { "_SPACE",       TYPE_DWORD,  57 },
    { "_CAPS_LOCK",   TYPE_DWORD,  58 },
    { "_F1",          TYPE_DWORD,  59 },
    { "_F2",          TYPE_DWORD,  60 },
    { "_F3",          TYPE_DWORD,  61 },
    { "_F4",          TYPE_DWORD,  62 },
    { "_F5",          TYPE_DWORD,  63 },
    { "_F6",          TYPE_DWORD,  64 },
    { "_F7",          TYPE_DWORD,  65 },
    { "_F8",          TYPE_DWORD,  66 },
    { "_F9",          TYPE_DWORD,  67 },
    { "_F10",         TYPE_DWORD,  68 },
    { "_NUM_LOCK",    TYPE_DWORD,  69 },
    { "_SCROLL_LOCK", TYPE_DWORD,  70 },
    { "_HOME",        TYPE_DWORD,  71 },
    { "_C_HOME",      TYPE_DWORD,  71 },
    { "_UP",          TYPE_DWORD,  72 },
    { "_C_UP",        TYPE_DWORD,  72 },
    { "_PGUP",        TYPE_DWORD,  73 },
    { "_C_PGUP",      TYPE_DWORD,  73 },
    { "_C_MINUS",     TYPE_DWORD,  74 },
    { "_LEFT",        TYPE_DWORD,  75 },
    { "_C_LEFT",      TYPE_DWORD,  75 },
    { "_C_CENTER",    TYPE_DWORD,  76 },
    { "_RIGHT",       TYPE_DWORD,  77 },
    { "_C_RIGHT",     TYPE_DWORD,  77 },
    { "_C_PLUS",      TYPE_DWORD,  78 },
    { "_END",         TYPE_DWORD,  79 },
    { "_C_END",       TYPE_DWORD,  79 },
    { "_DOWN",        TYPE_DWORD,  80 },
    { "_C_DOWN",      TYPE_DWORD,  80 },
    { "_PGDN",        TYPE_DWORD,  81 },
    { "_C_PGDN",      TYPE_DWORD,  81 },
    { "_INS",         TYPE_DWORD,  82 },
    { "_C_INS",       TYPE_DWORD,  82 },
    { "_DEL",         TYPE_DWORD,  83 },
    { "_C_DEL",       TYPE_DWORD,  83 },
    { "_F11",         TYPE_DWORD,  87 },
    { "_F12",         TYPE_DWORD,  88 },
    { "_LESS",        TYPE_DWORD,  89 },
    { "_EQUALS",      TYPE_DWORD,  90 },
    { "_GREATER",     TYPE_DWORD,  91 },
    { "_ASTERISK",    TYPE_DWORD,  92 },
    { "_R_ALT",       TYPE_DWORD,  93 },
    { "_R_CONTROL",   TYPE_DWORD,  94 },
    { "_L_ALT",       TYPE_DWORD,  95 },
    { "_L_CONTROL",   TYPE_DWORD,  96 },
    { "_MENU",        TYPE_DWORD,  97 },
    { "_L_WINDOWS",   TYPE_DWORD,  98 },
    { "_R_WINDOWS",   TYPE_DWORD,  99 },

    { "STAT_RSHIFT",  TYPE_DWORD,  STAT_RSHIFT },
    { "STAT_LSHIFT",  TYPE_DWORD,  STAT_LSHIFT },
    { "STAT_CTRL",    TYPE_DWORD,  STAT_CTRL   },
    { "STAT_ALT",     TYPE_DWORD,  STAT_ALT    },
    { "STAT_RCTRL",   TYPE_DWORD,  STAT_RCTRL  },
    { "STAT_LCTRL",   TYPE_DWORD,  STAT_LCTRL  },
    { "STAT_RALT",    TYPE_DWORD,  STAT_RALT   },
    { "STAT_LALT",    TYPE_DWORD,  STAT_LALT   },
    { "STAT_NUM",     TYPE_DWORD,  STAT_NUM    },
    { "STAT_CAPS",    TYPE_DWORD,  STAT_CAPS   },
    { "STAT_SHIFT",   TYPE_DWORD,  STAT_SHIFT  },

    { NULL          , 0         ,  0           }
} ;

/* ----------------------------------------------------------------- */
/* Definicion de variables globales (usada en tiempo de compilacion) */

char __bgdexport( libkey, globals_def )[] =
    "shift_status;\n"
    "ascii;\n"
    "scan_code;\n";

/* ----------------------------------------------------------------- */

char * __bgdexport( libkey, modules_dependency )[] =
{
    "libsdlhandler",
    NULL
};
#else
extern DLCONSTANT  __bgdexport( libkey, constants_def )[];
extern char __bgdexport( libkey, globals_def )[];
extern DLVARFIXUP  __bgdexport( libkey, globals_fixup )[];
extern char * __bgdexport( libkey, modules_dependency )[];
extern HOOK __bgdexport( libkey, handler_hooks )[];
extern void __bgdexport( libkey, module_initialize )();
extern void __bgdexport( libkey, module_finalize )();
#endif

#endif
