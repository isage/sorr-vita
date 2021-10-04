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

#ifndef __I_PROCDEF_ST_H
#define __I_PROCDEF_ST_H
#include "instance_st.h"

/* Process definitions, more simple that in the compiler */

typedef struct _procdef
{
	int * pridata ;
	int * pubdata ;

	int * code ;

	int exitcode ;
	int errorcode ;

	int * strings ;
	int * pubstrings ;

	int private_size ;
	int public_size ;

	int code_size ;

	int string_count ;
	int pubstring_count ;

	int params ;
	int id ;
	int type ;
	int flags ;
	char * name ;

    int breakpoint;
}
PROCDEF ;

#define PROC_USES_FRAME 	0x01
#define PROC_USES_LOCALS	0x02
#define PROC_FUNCTION   	0x04
#define PROC_USES_PUBLICS   0x08

/* System functions */

typedef int SYSFUNC (INSTANCE *, int *) ;
typedef struct _sysproc
{
	int       code ;
	char    * name ;
	char    * paramtypes ;
	int       type ;
	int       params ;
	SYSFUNC * func ;
	int       id ;
}
SYSPROC ;

#endif
