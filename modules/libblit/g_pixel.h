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

#ifndef __PIXEL_H
#define __PIXEL_H

/* --------------------------------------------------------------------------- */

#include <stdint.h>

/* --------------------------------------------------------------------------- */

extern int pixel_color8 ;
extern uint16_t pixel_color16 ;
extern uint32_t pixel_color32 ;

extern uint16_t pixel_color16_alpha ;

extern int pixel_alpha ;
extern uint8_t * pixel_alpha8 ;
extern uint16_t * pixel_alpha16 ;

/* --------------------------------------------------------------------------- */
/* Pixels */
/* ------ */

/* --------------------------------------------------------------------------- */

#define _Pixel8(ptr,color)              * ((uint8_t *)(ptr)) = (( pixel_alpha == 255 ) ? (color) : (pixel_alpha8[(uint8_t)(((color) << 8) + *(ptr))]))
#define _Pixel16(ptr,color,color_alpha) * ((uint16_t *)(ptr)) = (( pixel_alpha == 255 ) ? (color) : (pixel_alpha16[(uint16_t)*(ptr)] + (color_alpha)))
#define _Pixel32(ptr,color)             * ((uint32_t *)(ptr)) = (( pixel_alpha == 255 ) ? (color) : (color))

/* --------------------------------------------------------------------------- */

extern int gr_get_pixel( GRAPH * dest, int x, int y );
extern void gr_put_pixel( GRAPH * dest, int x, int y, int color );
extern void gr_put_pixelc( GRAPH * dest, REGION * clip, int x, int y, int color );
extern void gr_setalpha( int value );
extern void gr_setcolor( int c );

/* --------------------------------------------------------------------------- */
#endif
