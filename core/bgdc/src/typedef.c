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

TYPEDEF typedef_new( BASETYPE type )
{
    TYPEDEF t ;

    memset (&t, '\0', sizeof(t));

    t.chunk[0].type = type ;
    if ( type == TYPE_POINTER )
    {
        t.depth = 2 ;
        t.chunk[1].type = TYPE_UNDEFINED ;
    }
    else t.depth = 1 ;
    t.varspace = 0 ;
    return t ;
}

TYPEDEF typedef_reduce( TYPEDEF base )
{
    TYPEDEF t = base ;

    memmove( &t.chunk[0], &t.chunk[1], sizeof( TYPECHUNK ) * ( MAX_TYPECHUNKS - 1 ) ) ;
    t.depth-- ;
    return t ;
}

TYPEDEF typedef_enlarge( TYPEDEF base )
{
    TYPEDEF t = base ;

    memmove( &t.chunk[1], &t.chunk[0], sizeof( TYPECHUNK ) * ( MAX_TYPECHUNKS - 1 ) ) ;
    t.depth++ ;
    return t ;
}

TYPEDEF typedef_pointer( TYPEDEF base )
{
    TYPEDEF t = typedef_enlarge( base ) ;

    t.chunk[0].type = TYPE_POINTER ;
    return t ;
}

void typedef_describe( char * buffer, TYPEDEF t )
{
    switch ( t.chunk[0].type )
    {
        case TYPE_INT:
            sprintf( buffer, "INT" ) ;
            return ;

        case TYPE_DWORD:
            sprintf( buffer, "DWORD" ) ;
            return ;

        case TYPE_SHORT:
            sprintf( buffer, "SHORT" ) ;
            return ;

        case TYPE_WORD:
            sprintf( buffer, "WORD" ) ;
            return ;

        case TYPE_BYTE:
            sprintf( buffer, "BYTE" ) ;
            return ;

        case TYPE_CHAR:
            sprintf( buffer, "CHAR" ) ;
            return ;

        case TYPE_SBYTE:
            sprintf( buffer, "SIGNED BYTE" ) ;
            return ;

        case TYPE_STRING:
            sprintf( buffer, "STRING" ) ;
            return ;

        case TYPE_FLOAT:
            sprintf( buffer, "FLOAT" ) ;
            return ;

        case TYPE_STRUCT:
            if ( t.chunk[0].count > 1 )
                sprintf( buffer, "STRUCT [%d]", t.chunk[0].count ) ;
            else
                sprintf( buffer, "STRUCT" ) ;
            return ;

        case TYPE_ARRAY:
            sprintf( buffer, "ARRAY [%d] OF ", t.chunk[0].count ) ;
            typedef_describe( buffer + strlen( buffer ), typedef_reduce( t ) ) ;
            return ;

        case TYPE_POINTER:
            sprintf( buffer, "POINTER TO " ) ;
            typedef_describe( buffer + strlen( buffer ), typedef_reduce( t ) ) ;
            return ;

        case TYPE_UNDEFINED:
        default:
            sprintf( buffer, "<UNDEFINED>" ) ;
            return ;
    }
}

int typedef_subsize( TYPEDEF t, int c )
{
    switch ( t.chunk[c].type )
    {
        case TYPE_BYTE:
        case TYPE_SBYTE:
        case TYPE_CHAR:
            return 1 ;

        case TYPE_WORD:
        case TYPE_SHORT:
            return 2 ;

        case TYPE_DWORD:
        case TYPE_INT:
        case TYPE_FLOAT:
        case TYPE_STRING:
        case TYPE_POINTER:
            return 4 ;

        case TYPE_ARRAY:
            return t.chunk[c].count * typedef_subsize( t, c + 1 ) ;

        case TYPE_STRUCT:
            return t.varspace->size ;

        default:
            compile_error( MSG_INCOMP_TYPE ) ;
            return 0 ;
    }
}

int typedef_size( TYPEDEF t )
{
    return typedef_subsize( t, 0 ) ;
}

/* Tipos nombrados (structs, típicamente) */

static TYPEDEF * named_types = NULL;
static int     * named_codes = NULL;
static int     named_count = 0 ;
static int     named_reserved = 0;

TYPEDEF * typedef_by_name( int code )
{
    int n ;

    for ( n = 0 ; n < named_count ; n++ )
        if ( named_codes[n] == code ) return &named_types[n] ;
    return 0 ;
}

void typedef_name( TYPEDEF t, int code )
{
    if ( named_count >= named_reserved )
    {
        named_reserved += 16;
        named_types = ( TYPEDEF * ) realloc( named_types, named_reserved * sizeof( TYPEDEF ) );
        named_codes = ( int * ) realloc( named_codes, named_reserved * sizeof( int ) );
        if ( !named_types || !named_codes ) compile_error( "typedef_name: out of memory\n" ) ;
    }
    named_codes[named_count] = code ;
    named_types[named_count] = t ;
    named_count++ ;
}

int typedef_tcount( TYPEDEF t )
{
    int n ;
    int count = 1 ;

    if ( t.chunk[0].type == TYPE_STRUCT ) return t.chunk[0].count ;
    for ( n = 0; t.chunk[n].type == TYPE_ARRAY; n++ ) count *= t.chunk[n].count ;
    return count ;
}

int typedef_is_equal( TYPEDEF a, TYPEDEF b )
{
    int n;

    if ( a.depth != b.depth ) return 0;

    for ( n = 0; n < a.depth; n++ )
    {
        if ( a.chunk[n].type == TYPE_STRUCT && b.chunk[n].type == TYPE_STRUCT && a.varspace != b.varspace )
        {
            int m;
            if ( a.varspace->count != b.varspace->count ) return 0;
            for ( m = 0; m < a.varspace->count; m++ )
            {
                if ( !typedef_is_equal( a.varspace->vars[m].type, b.varspace->vars[m].type ) ) return 0;
            }
            return 1;
        }
        if ( a.chunk[n].type != b.chunk[n].type ) return 0;
        if ( a.chunk[n].type == TYPE_ARRAY && a.chunk[n].count != b.chunk[n].count ) return 0;
    }
    return 1;
}

