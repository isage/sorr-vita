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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef TARGET_BEOS
#include <posix/assert.h>
#else
#include <assert.h>
#endif

#include "bgdc.h"

/* ---------------------------------------------------------------------- */
/* Gestor de procesos y bloques de código. Este módulo contiene funciones */
/* de utilidad para crear procesos y bloques de código así como otras que */
/* se emplean durante y después del compilado.                            */
/* ---------------------------------------------------------------------- */

PROCDEF * mainproc = 0 ;
int procdef_count = 0 ;

int procdef_maxid = -1 ;
PROCDEF ** procs = 0 ;
int procs_allocated = 0 ;

int procdef_getid()
{
    return ++procdef_maxid ;
}

PROCDEF * procdef_new (int typeid, int id)
{
    PROCDEF * proc = (PROCDEF *) calloc (1, sizeof(PROCDEF)) ;
    int n ;

    if (!proc)
    {
        fprintf (stdout, "procdef_new: out of memory\n") ;
        exit (1) ;
    }

    proc->pridata = segment_new() ;
    proc->privars = varspace_new() ;

    /* (2006/11/19 23:15 GMT-03:00, Splinter - jj_arg@yahoo.com) */
    proc->pubdata = segment_new() ;
    proc->pubvars = varspace_new() ;
    /* (2006/11/19 23:15 GMT-03:00, Splinter - jj_arg@yahoo.com) */

    proc->params    = -1 ;
    proc->defined   = 0 ;
    proc->declared  = 0 ;
    proc->type      = TYPE_DWORD ;
    proc->flags     = 0 ;
    proc->imported  = 0 ;

    proc->sentence_count = 0 ;
    proc->sentences      = 0 ;

    if (typeid >= procs_allocated)
    {
        procs_allocated = typeid + 15 ;
        procs = (PROCDEF **) realloc (procs, sizeof(PROCDEF **) * procs_allocated) ;
        if (!procs)
        {
            fprintf (stdout, "procdef_new: out of memory\n") ;
            exit (1) ;
        }
    }
    proc->typeid     = typeid ;
    proc->identifier = id ;
    procs[typeid]    = proc ;

    for (n = 0 ; n < MAX_PARAMS ; n++)
        proc->paramtype[n] = TYPE_UNDEFINED ;

    proc->exitcode  = 0 ;
    proc->errorcode = 0 ;

    codeblock_init (&proc->code) ;
    procdef_count++ ;
    return proc ;
}

PROCDEF * procdef_search (int id)
{
    int n ;

    for (n = 0 ; n <= procdef_maxid; n++)
        if (procs[n]->identifier == id) return procs[n] ;

    return 0 ;
}

PROCDEF * procdef_search_by_codeblock (CODEBLOCK * p)
{
    int n ;

    for (n = 0 ; n <= procdef_maxid; n++)
        if (&procs[n]->code == p) return procs[n] ;

    return 0 ;
}

PROCDEF * procdef_get (int typeid)
{
    return procs_allocated > typeid ? procs[typeid] : 0 ;
}

void procdef_destroy (PROCDEF * proc)
{
    varspace_destroy (proc->privars) ;
    segment_destroy  (proc->pridata) ;

    /* (2006/11/20 01:09 GMT-03:00, Splinter - jj_arg@yahoo.com) */
    varspace_destroy (proc->pubvars);
    segment_destroy  (proc->pubdata) ;

    procs[proc->typeid] = 0 ;
    free (proc->code.data) ;
    free (proc->code.loops) ;
    free (proc->code.labels) ;
    free (proc) ;

    procdef_count-- ;
}


/* Realiza acciones posteriores al compilado sobre el código:
 * - Convierte saltos de código de etiqueta a offset
 * - Convierte identificador de procesos en CALL o TYPE a typeid */

void program_postprocess ()
{
    int n ;
    for (n = 0; n <= procdef_maxid; n++) codeblock_postprocess (&procs[n]->code) ;
}

void program_dumpprocesses()
{
    int n;
    for (n = 0; n <= procdef_maxid; n++) procdef_dump( procs[n] );
}

void procdef_dump( PROCDEF * proc )
{
    printf( "\n\n---------- Process %d (%s)\n\n", proc->typeid, identifier_name( proc->identifier ) ) ;

    if ( proc->privars->count )
    {
        printf( "---- Private variables\n" ) ;
        varspace_dump( proc->privars, 0 ) ;
        printf( "\n" ) ;
    }

    if ( proc->pubvars->count )
    {
        printf( "---- Public variables\n" ) ;
        varspace_dump( proc->pubvars, 0 ) ;
        printf( "\n" ) ;
    }

    /* segment_dump  (proc->pridata) ; */
    codeblock_dump( &proc->code ) ;
}
