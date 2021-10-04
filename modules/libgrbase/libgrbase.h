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

#ifndef __LIBGRBASE_H
#define __LIBGRBASE_H

/* --------------------------------------------------------------------------- */

#include <stdlib.h>
#include <stdint.h>

/* --------------------------------------------------------------------------- */

#include "g_pal.h"
#include "g_bitmap.h"
#include "g_clear.h"
#include "g_grlib.h"
#include "g_region.h"
#include "g_blendop.h"
#include "g_conversion.h"

/* --------------------------------------------------------------------------- */

extern PIXEL_FORMAT * std_pixel_format8 ;
extern PIXEL_FORMAT * std_pixel_format16 ;
extern PIXEL_FORMAT * std_pixel_format32 ;

extern PIXEL_FORMAT * sys_pixel_format ;

extern GRAPH * background ;
extern GRAPH * scrbitmap ;

/* --------------------------------------------------------------------------- */

#endif
