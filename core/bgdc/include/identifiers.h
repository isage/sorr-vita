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

#ifndef __IDENTIFIERS_H
#define __IDENTIFIERS_H

typedef struct _identifier
{
	const char * name ;
	int    code ;
	int	   line ;		/* First USE for the identifier */
	int    f ;			/* file where the id was found */
	struct _identifier * next ;
}
identifier ;

/* Gestor de identificadores */

extern void identifier_init () ;
extern void identifier_dump () ;
extern int  identifier_add (const char * string) ;
extern int  identifier_add_as (const char * string, int code) ;
extern int  identifier_search (const char * string) ;
extern int  identifier_search_or_add (const char * string) ;
extern const char * identifier_name (int code) ;
extern int identifier_line (int code) ;
extern int identifier_file (int code) ;

extern int  identifier_next_code () ;

extern identifier * identifier_first() ;
extern identifier * identifier_next (identifier * id) ;

#endif
