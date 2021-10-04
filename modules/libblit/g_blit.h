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

#ifndef __BLIT_H
#define __BLIT_H

/* --------------------------------------------------------------------------- */

#include "libgrbase.h"

/* --------------------------------------------------------------------------- */
/* Flags para gr_blit                                                          */

#define B_HMIRROR       0x0001
#define B_VMIRROR       0x0002
#define B_TRANSLUCENT   0x0004
#define B_ALPHA         0x0008
#define B_ABLEND        0x0010
#define B_SBLEND        0x0020
#define B_NOCOLORKEY    0x0080

#define B_ALPHA_MASK    0xFF00
#define B_ALPHA_SHIFT   8

/* --------------------------------------------------------------------------- */

extern void gr_blit( GRAPH * dest, REGION * clip, int x, int y, int flags, GRAPH * gr ) ;
extern void gr_get_bbox( REGION * dest, REGION * clip, int x, int y, int flags, int angle, int scalex, int scaley, GRAPH * gr ) ;
extern void gr_rotated_blit( GRAPH * dest, REGION * clip, int x, int y, int flags, int angle, int scalex, int scaley, GRAPH * gr ) ;

/* --------------------------------------------------------------------------- */

#endif
