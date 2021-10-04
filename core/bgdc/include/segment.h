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

#ifndef __SEGMENT_H
#define __SEGMENT_H

#include <stdint.h>

#include "typedef.h"

/* Un segmento es una zona lineal de datos que puede crecer dinámicamente */

typedef struct _segment
{
	void    * bytes ;
	int	current ;
	int	reserved ;
	int	id ;
}
segment ;

extern segment * segment_new() ;
extern segment * segment_duplicate(segment * s) ;
extern segment * segment_get(int id) ;

/* Devuelven el offset del nuevo dato */
extern int       segment_add_as    (segment * s, int32_t value, BASETYPE t) ;
extern int       segment_add_dword (segment * s, int32_t value) ;
extern int       segment_add_word  (segment * s, int16_t value) ;
extern int       segment_add_byte  (segment * s, int8_t  value) ;
extern int       segment_add_from  (segment * s, segment * from) ;

extern void      segment_dump(segment *) ;
extern void      segment_destroy(segment *) ;
extern void      segment_copy(segment *, int base_offset, int total_length) ;
extern void      segment_alloc (segment * n, int count) ;

extern segment  * globaldata ;
extern segment  * localdata ;

/* Segmentos nombrados (para tipos definidos por el usuario */

extern segment * segment_by_name (int code) ;
extern void      segment_name    (segment * s, int code) ;

#endif
