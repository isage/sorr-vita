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

#ifndef __PROCDEF_H
#define __PROCDEF_H

/* Procesos (un "PROCDEF" es, en realidad, simplemente su definición) */

#include "segment.h"
#include "varspace.h"
#include "codeblock.h"
#include "typedef.h"

#define MAX_PARAMS          256

#define PROC_USES_FRAME     0x01
#define PROC_USES_LOCALS    0x02
#define PROC_FUNCTION       0x04
#define PROC_USES_PUBLICS   0x08

typedef struct _sentence
{
    int file ;
    int line ;
    int col ;
    int offset ;
}
SENTENCE ;

typedef struct _procdef
{
    VARSPACE    * privars ;
    segment     * pridata ;

    /* (2006/11/19 23:15 GMT-03:00, Splinter - jj_arg@yahoo.com) */
    VARSPACE    * pubvars ;
    segment     * pubdata ;
    /* (2006/11/19 23:15 GMT-03:00, Splinter - jj_arg@yahoo.com) */

    int         typeid ;
    int         identifier ;
    int         params ;
    int         defined ;
    int         declared ;
    int         flags ;

    int         imported; /* this proc is a libproc */

    BASETYPE    paramname[MAX_PARAMS] ;
    BASETYPE    paramtype[MAX_PARAMS] ;
    BASETYPE    type ;

    CODEBLOCK   code ;

    int         exitcode;
    int         errorcode;

    SENTENCE    * sentences ;
    int         sentence_count ;
}
PROCDEF ;

extern int procdef_count ;
extern int procdef_maxid ;

extern int       procdef_getid() ;
extern PROCDEF * procdef_new (int typeid, int identifier) ;
extern PROCDEF * procdef_get (int typeid) ;
extern PROCDEF * procdef_search (int identifier) ;
extern PROCDEF * procdef_search_by_codeblock (CODEBLOCK * p);
extern void      procdef_destroy(PROCDEF *) ;

extern void      procdef_dump( PROCDEF * proc );

/* Proceso "principal", el primero en definirse y ejecutarse */
extern PROCDEF * mainproc ;

extern void program_dumpprocesses();

#endif
