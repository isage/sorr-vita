/*
 *  Copyright (c) 2011 Joseba García Echebarria. All rights reserved.
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

#ifndef __MODFMODEX_SYMBOLS_H
#define __MODFMODEX_SYMBOLS_H

#ifdef __BGDC__
char __bgdexport( mod_fmodex, globals_def )[] =
"float fmodex_spectrum[256];\n"
"int   sound_freq = 48000;\n"
"int   fmodex_spectrumsize = 0;\n";

DLSYSFUNCS __bgdexport( mod_fmodex, functions_exports )[] =
{
    {"FMODEX_SONG_LOAD",         "S", TYPE_INT,       0 },
    {"FMODEX_SONG_PLAY",         "I", TYPE_INT,       0 },
    {"FMODEX_SONG_PAUSE",        "I", TYPE_INT,       0 },
    {"FMODEX_SONG_RESUME",       "I", TYPE_INT,       0 },
    {"FMODEX_SONG_PLAYING",      "I", TYPE_INT,       0 },
    {"FMODEX_SONG_STOP",         "I", TYPE_INT,       0 },
    {"FMODEX_SONG_GET_SPECTRUM", "I", TYPE_INT,       0 },
    {"FMODEX_MIC_NUM",           "",  TYPE_INT,       0 },
    {"FMODEX_MIC_NAME",          "I", TYPE_STRING,    0 },
    {"FMODEX_MIC_GET_SPECTRUM",  "I", TYPE_UNDEFINED, 0 },
    {"FMODEX_STOP_SPECTRUM",     "",  TYPE_UNDEFINED, 0 },
	{0, 0, 0, 0}
};
#else
extern char __bgdexport( mod_fmodex, globals_def )[];
extern DLVARFIXUP __bgdexport( mod_fmodex, globals_fixup )[];
extern DLSYSFUNCS __bgdexport( mod_fmodex, functions_exports )[];
extern void __bgdexport( mod_fmodex, module_initialize )();
extern void __bgdexport( mod_fmodex, module_finalize )();
extern HOOK __bgdexport( mod_fmodex, handler_hooks )[];
#endif

#endif
