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

#ifndef __VARSPACE_H
#define __VARSPACE_H

#include <stdint.h>

#include "typedef.h"

/* Un VARSPACE es una zona de definición de variables */

typedef struct _varspace
{
	struct _variable * vars ;
	int	size ;
	int	count ;
	int	reserved ;
	int	last_offset ;

	int	* stringvars ;
	int	stringvar_reserved ;
	int	stringvar_count ;
}
VARSPACE ;

typedef struct _variable
{
	TYPEDEF type ;
	int	code ;
	int	offset ;
}
VARIABLE ;

extern VARSPACE * varspace_new () ;
extern void       varspace_alloc (VARSPACE * n, int count) ;
extern void       varspace_init (VARSPACE * n) ;
extern void       varspace_add (VARSPACE * n, VARIABLE v) ;
extern VARIABLE * varspace_search (VARSPACE * n, int code) ;
extern void       varspace_dump (VARSPACE * n, int indent) ;
extern void       varspace_destroy (VARSPACE * n) ;
extern void	      varspace_varstring (VARSPACE * n, int offset) ;

/* Datos globales y locales */

extern VARSPACE global ;
extern VARSPACE local ;

#endif
