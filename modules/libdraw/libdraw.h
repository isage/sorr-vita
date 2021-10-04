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

#ifndef __DRAW_H
#define __DRAW_H

#include "libgrbase.h"

/* --------------------------------------------------------------------------- */

extern uint32_t drawing_stipple;

/* --------------------------------------------------------------------------- */

void draw_vline( GRAPH * dest, REGION * clip, int x, int y, int h );
void draw_hline( GRAPH * dest, REGION * clip, int x, int y, int w );
void draw_box( GRAPH * dest, REGION * clip, int x, int y, int w, int h );
void draw_rectangle( GRAPH * dest, REGION * clip, int x, int y, int w, int h );
void draw_circle( GRAPH * dest, REGION * clip, int x, int y, int r );
void draw_fcircle( GRAPH * dest, REGION * clip, int x, int y, int r );
void draw_line( GRAPH * dest, REGION * clip, int x, int y, int w, int h );
void draw_bezier( GRAPH * dest, REGION * clip, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int level );

/* --------------------------------------------------------------------------- */

#endif
