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
#include <stdlib.h>
#include <string.h>

#include "bgdc.h"

/* Fast access sysproc list, by identifier code */

SYSPROC  ** sysproc_list = NULL ;
int         sysproc_maxid = 0 ;

/* Este fichero contiene sólo las definiciones de las funciones del sistema */

#define SYSMACRO(a) 0
#include "sysprocs.h"

/*
 *  FUNCTION : sysproc_init
 *
 *  Initilize the module
 *
 *  PARAMS:
 *  none
 *
 *  RETURN VALUE:
 *      none
 */

void sysproc_init( void )
{
}

/*
 *  FUNCTION : sysproc_add
 *
 *  Add a new system function to the internal system function list
 *
 *  PARAMS:
 *  name   Name of the process
 *  paramtypes  String representation of the parameter
 *  type   Type of the returning value
 *  func   Pointer to the function itself or a stub
 *
 *  RETURN VALUE:
 *      Identifier code allocated for the function
 */

int sysproc_add( char * name, char * paramtypes, int type, void * func )
{
    static SYSPROC * sysproc_new = 0 ;
    static int sysproc_maxcode = 0 ;
    static int sysproc_count = 0 ;

    if ( !sysproc_new )
    {
        sysproc_new = sysprocs ;
        while ( sysproc_new->name )
        {
            if ( sysproc_new->code > sysproc_maxcode ) sysproc_maxcode = sysproc_new->code ;
            sysproc_new++ ;
            sysproc_count++ ;
        }
    }

    if ( sysproc_count >= MAX_SYSPROCS ) compile_error( MSG_TOO_MANY_SYSPROCS ) ;

    sysproc_maxcode++;

    sysproc_new->code = sysproc_maxcode ;
    sysproc_new->name = name ;
    sysproc_new->paramtypes = paramtypes ;
    sysproc_new->params = strlen( paramtypes ) ;
    sysproc_new->type = type ;
    sysproc_new->id   = identifier_search_or_add( name ) ;
    sysproc_new->next = NULL ;

    sysproc_new++ ;
    sysproc_count++ ;

    /* If the fast-access list is already filled, free it to fill it again
     * in sysproc_get. We should add the new process to the list, but this
     * is a very rare possibility and we're lazy */

    if ( sysproc_list != NULL )
    {
        free( sysproc_list );
        sysproc_list = NULL;
        sysproc_maxid = 0;
    }

    return sysproc_new->code ;
}

/*
 *  FUNCTION : sysproc_get
 *
 *  Search for a system function by name and return a pointer
 *  to the first ocurrence or NULL if none exists
 *
 *  PARAMS:
 *  id   Unique code of the identifier of the name
 *
 *  RETURN VALUE:
 *      Pointer to the SYSPROC object or NULL if none exists
 */

SYSPROC * sysproc_get( int id )
{
    SYSPROC * s ;

    /* If the table is filled, get the process with direct access */

    if ( id < sysproc_maxid ) return sysproc_list[id];

    /* Fill the table */

    if ( sysproc_list == NULL )
    {
        /* Alloc IDs if necessary and get the maximum one */

        for ( s = sysprocs ; s->name ; s++ )
        {
            if ( s->id == 0 ) s->id = identifier_search_or_add( s->name ) ;
            if ( s->id > sysproc_maxid ) sysproc_maxid = s->id;

            s->next = NULL;
        }

        /* Alloc the table */

        sysproc_maxid = (( sysproc_maxid + 1 ) & ~31 ) + 32;
        sysproc_list  = ( SYSPROC ** ) calloc( sysproc_maxid, sizeof( SYSPROC * ) );
        if ( sysproc_list == NULL ) abort();

        /* Fill it */

        for ( s = sysprocs ; s->name ; s++ )
        {
            if ( s->id > 0 )
            {
                s->next = sysproc_list[s->id];
                sysproc_list[s->id] = s;
            }
        }

        if ( id < sysproc_maxid ) return sysproc_list[id];
    }

    return NULL;
}

/*
 *  FUNCTION : sysproc_getall
 *
 *  Search for a system function by name and return a pointer
 *  to a zero-terminated table with pointers to all ocurrences.
 *  Up to 31 occurences can be found with this function.
 *
 *  PARAMS:
 *  id   Unique code of the identifier of the name
 *
 *  RETURN VALUE:
 *      Pointer to a new SYSPROC table allocated with malloc()
 *      NULL if no process with this id exists
 */

SYSPROC ** sysproc_getall( int id )
{
    SYSPROC * s = sysproc_get( id ) ;
    SYSPROC ** table;
    int found = 0 ;

    if ( s == NULL ) return NULL;

    table = calloc( 32, sizeof( SYSPROC * ) ) ;
    do
    {
        table[found++] = s;
        s = s->next;
    }
    while ( s && found <= 31 );
    table[found] = NULL;
    return table ;
}

/*
 *  FUNCTION : sysproc_name
 *
 *  Return the name of a given system function
 *
 *  PARAMS:
 *  code  Internal code of the function
 *
 *  RETURN VALUE:
 *      Pointer to the name or NULL if it was not found
 */

/* ---------------------------------------------------------------------- */

char * sysproc_name( int code )
{
    SYSPROC * s = sysprocs ;

    while ( s->name )
    {
        if ( s->code == code ) return s->name ;
        s++ ;
    }
    return 0 ;
}

