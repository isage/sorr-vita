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

#ifndef __TYPEDEF_ST_H
#define __TYPEDEF_ST_H

/* Tipos de dato */

typedef enum {
	TYPE_UNDEFINED = 0,

	TYPE_INT	 = 1,
	TYPE_DWORD   = 2,
	TYPE_SHORT	 = 3,
	TYPE_WORD	 = 4,
	TYPE_SBYTE	 = 5,
	TYPE_BYTE	 = 6,

	TYPE_CHAR    = 8,
	TYPE_FLOAT	 = 9,

	TYPE_STRING  = 16,
	TYPE_ARRAY	 = 17,
	TYPE_STRUCT	 = 18,
	TYPE_POINTER = 19,
	TYPE_MX = 0xFFFFFFF1
}
BASETYPE ;

typedef struct _typechunk
{
	BASETYPE   type ;
	int	   count ;	/* Para type == TYPE_ARRAY */
}
TYPECHUNK ;

#define MAX_TYPECHUNKS 8

typedef struct _typedef
{
	TYPECHUNK	   chunk[MAX_TYPECHUNKS] ;
	int		   depth ;
	struct _varspace * varspace ;
}
TYPEDEF ;

#define typedef_is_numeric(t)   ((t).chunk[0].type < 16)
#define typedef_is_integer(t)   ((t).chunk[0].type < 8)
#define typedef_is_float(t)     ((t).chunk[0].type == TYPE_FLOAT)
#define typedef_is_string(t)    ((t).chunk[0].type == TYPE_STRING)
#define typedef_is_struct(t)    ((t).chunk[0].type == TYPE_STRUCT)
#define typedef_is_array(t)     ((t).chunk[0].type == TYPE_ARRAY)
#define typedef_is_pointer(t)   ((t).chunk[0].type == TYPE_POINTER)
#define typedef_count(t)        ((t).chunk[0].count)
#define typedef_base(t)         ((t).chunk[0].type)
#define typedef_members(t)      ((t).varspace)
#define typedef_is_unsigned(t)	((t).chunk[0].type <= 8 && (t).chunk[0].type > 0 && !((t).chunk[0].type & 1))

#endif
