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

#ifndef __BLEND_H
#define __BLEND_H

/* Blend ops */
/* --------- */

extern void blend_apply( GRAPH * bitmap, int16_t * blend ) ;
extern void blend_assign( GRAPH * bitmap, int16_t * blend ) ;
extern int16_t * blend_create( void ) ;
extern void blend_free( int16_t * blend );
extern void blend_grayscale( int16_t * blend, int method ) ;
extern void blend_init( int16_t * blend ) ;
extern void blend_intensity( int16_t * blend, float ammount ) ;
extern void blend_swap( int16_t * blend ) ;
extern void blend_tint( int16_t * blend, float ammount, uint8_t cr, uint8_t cg, uint8_t cb ) ;
extern void blend_translucency( int16_t * blend, float ammount ) ;

#endif
