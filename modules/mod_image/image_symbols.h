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

#ifndef __IMAGE_SYMBOLS_H
#define __IMAGE_SYMBOLS_H

#include <bgddl.h>

#ifndef __BGDC__
extern int bgd_load_image (INSTANCE * i, int * params);
extern int bgd_is_PCX (INSTANCE * i, int * params);
extern int bgd_is_GIF (INSTANCE * i, int * params);
extern int bgd_is_JPG (INSTANCE * i, int * params);
extern int bgd_is_PNG (INSTANCE * i, int * params);
extern int bgd_is_XPM (INSTANCE * i, int * params);
extern int bgd_is_BMP (INSTANCE * i, int * params);
extern int bgd_is_PNM (INSTANCE * i, int * params);
extern int bgd_is_TIF (INSTANCE * i, int * params);
extern int bgd_is_LBM (INSTANCE * i, int * params);
extern int bgd_is_XCF (INSTANCE * i, int * params);
#endif

DLSYSFUNCS __bgdexport( image, functions_exports )[] =
{
	{"LOAD_IMAGE", "S", TYPE_DWORD, SYSMACRO(bgd_load_image) },
	{"IS_BMP", "S", TYPE_DWORD, SYSMACRO(bgd_is_BMP)},
	{"IS_GIF", "S", TYPE_DWORD, SYSMACRO(bgd_is_GIF)},
	{"IS_JPG", "S", TYPE_DWORD, SYSMACRO(bgd_is_JPG)},
	{"IS_LBM", "S", TYPE_DWORD, SYSMACRO(bgd_is_LBM)},
	{"IS_PCX", "S", TYPE_DWORD, SYSMACRO(bgd_is_PCX)},
	{"IS_PNG", "S", TYPE_DWORD, SYSMACRO(bgd_is_PNG)},
	{"IS_PNM", "S", TYPE_DWORD, SYSMACRO(bgd_is_PNM)},
	{"IS_TIF", "S", TYPE_DWORD, SYSMACRO(bgd_is_TIF)},
	{"IS_XCF", "S", TYPE_DWORD, SYSMACRO(bgd_is_XCF)},
	{"IS_XPM", "S", TYPE_DWORD, SYSMACRO(bgd_is_XPM)},
	{0, 0, 0, 0}
};

char * __bgdexport( image, modules_dependency )[] =
{
	"libgrbase",
	"libvideo",
	NULL
};

#endif
