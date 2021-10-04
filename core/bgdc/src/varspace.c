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

/* ---------------------------------------------------------------------- */
/* Este módulo contiene funciones de utilidad para crear e ir rellenando  */
/* varspaces (tablas con identificador, offset y tipo de cada VARIABLE)   */
/* y segmentos de datos                                                   */
/* ---------------------------------------------------------------------- */

VARSPACE global, local ;

/*
 *  FUNCTION : varspace_dump
 *
 *  Dumps descriptively the variables of a given varspace to the
 *  standard output. Recursively describes struct types.
 *
 *  PARAMS :
 *      n    Pointer to the varspace
 *  indent   Left indentation in chars (internal; use 0)
 *
 *  RETURN VALUE :
 *      None
 */

void varspace_dump( VARSPACE * n, int indent )
{
    int i, t, to ;
    char buffer[128] ;

    for ( i = 0 ; i < n->count ; i++ )
    {
        if ( i < n->count - 1 )
            to = n->vars[i+1].offset - 1 ;
        else
            to = n->last_offset - 1 ;

        printf( "[%04d:%04d]\t", n->vars[i].offset, to ) ;
        for ( t = 0 ; t < indent ; t++ ) printf( " + " ) ;
        typedef_describe( buffer, n->vars[i].type ) ;
        printf( "%s %s", buffer, identifier_name( n->vars[i].code ) ) ;

        /* Describe arrays of structs */

        if ( typedef_is_array( n->vars[i].type ) )
        {
            TYPEDEF r = typedef_reduce( n->vars[i].type );
            while ( typedef_is_array( r ) )
                r = typedef_reduce( r );
            if ( typedef_is_struct( r ) )
            {
                printf( ":\n" ) ;
                varspace_dump( typedef_members( r ), indent + 1 ) ;
            }
            else
                printf( "\n" );
        }

        /* Describe structs */

        else if ( typedef_is_struct( n->vars[i].type ) )
        {
            printf( ":\n" ) ;
            varspace_dump( typedef_members( n->vars[i].type ), indent + 1 ) ;
        }

        else printf( "\n" ) ;
    }
}

/*
 *  FUNCTION : varspace_new
 *
 *  Create a new varspace in dynamic memory, and initialize
 *  it using varspace_init
 *
 *  PARAMS :
 *      None
 *
 *  RETURN VALUE :
 *      Pointer to the new varspace
 */

VARSPACE * varspace_new()
{
    VARSPACE * v = ( VARSPACE * ) calloc( 1, sizeof( VARSPACE ) ) ;
    if ( !v ) compile_error( "varspace_new: out of memory\n" ) ;
    varspace_init( v ) ;
    return v ;
}


/*
 *  FUNCTION : varspace_destroy
 *
 *  Destroy a dynamic varspace created with varspace_new
 *
 *  PARAMS :
 *      v    Pointer to the varspace
 *
 *  RETURN VALUE :
 *      None
 */

void varspace_destroy( VARSPACE * v )
{
    free( v->vars ) ;
    free( v ) ;
}

/*
 *  FUNCTION : varspace_init
 *
 *  Initialize all members of a varspace. Use this function
 *  to initialize a varspace that is created in local
 * memory instead of a dynamic one created with varspace_new.
 *
 *  PARAMS :
 *      n    Pointer to the not-initialized varspace
 *
 *  RETURN VALUE :
 *      None
 */

void varspace_init( VARSPACE * n )
{
    n->vars = ( VARIABLE * ) calloc( 16, sizeof( VARIABLE ) ) ;
    n->reserved = 16 ;
    n->count = 0 ;
    n->size = 0 ;
    n->stringvars = 0 ;
    n->stringvar_reserved = 0 ;
    n->stringvar_count = 0 ;
    if ( !n->vars ) compile_error( "varspace_init: out of memory\n" ) ;
}

/*
 *  FUNCTION : varspace_varstring
 *
 *  Add a new string offset to the varspace. A varspace has
 *  a count of every string variable it contains, but this
 * list is not updated automatically. You should mark
 * every string you create using this function.
 *
 *  PARAMS :
 *      n    Pointer to the varspace
 *  offset   Offset of the new string
 *
 *  RETURN VALUE :
 *      None
 */

void varspace_varstring( VARSPACE * n, int offset )
{
    if ( n->stringvar_reserved == n->stringvar_count )
    {
        n->stringvars = ( int * ) realloc( n->stringvars, ( n->stringvar_reserved += 16 ) * sizeof( int ) ) ;
        if ( !n->stringvars ) compile_error( "varspace_varstring: out of memory\n" ) ;
    }
    n->stringvars[n->stringvar_count++] = offset ;
}

/*
 *  FUNCTION : varspace_alloc
 *
 *  Allocate space for new variables in the varspace. This
 *  is an internal function. Use varspace_add instead.
 *
 *  PARAMS :
 *      n    Pointer to the varspace
 *  count   Number of new variables of space to reserve
 *
 *  RETURN VALUE :
 *      None
 */

void varspace_alloc( VARSPACE * n, int count )
{
    n->vars = ( VARIABLE * ) realloc( n->vars, sizeof( VARIABLE ) * ( n->reserved += count ) ) ;
    if ( !n->vars ) compile_error( "varspace_alloc: out of memory\n" ) ;
}

/*
 *  FUNCTION : varspace_add
 *
 *  Add a new variable to a given varspace. This function does not
 * mark string variables. You should use varspace_varstring after
 * adding new strings to a varspace.
 *
 *  PARAMS :
 *      n    Pointer to the varspace
 *  v    Variable to add
 *
 *  RETURN VALUE :
 *      None
 */

void varspace_add( VARSPACE * n, VARIABLE v )
{
    if ( n->count == n->reserved ) varspace_alloc( n, 16 ) ;
    n->vars[n->count++] = v ;
    n->size += typedef_size( v.type ) ;
}

/*
 *  FUNCTION : varspace_search
 *
 *  Search a variable in a varspace by its identifier.
 *
 *  PARAMS :
 *      n    Pointer to the varspace
 *  code   Identifier of the variable
 *
 *  RETURN VALUE :
 *      Pointer to the variable found or NULL if none
 */

VARIABLE * varspace_search( VARSPACE * n, int code )
{
    int i ;

    for ( i = 0 ; i < n->count ; i++ )
        if ( n->vars[i].code == code ) return &n->vars[i] ;
    return 0 ;
}
