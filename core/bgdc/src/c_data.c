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
#ifdef TARGET_BEOS
#include <posix/assert.h>
#else
#include <assert.h>
#endif
#include <string.h>
#include <stdlib.h>

#include "bgdc.h"
#include "xctype.h"

/* ---------------------------------------------------------------------- */
/* Este módulo contiene las funciones que compilan definiciones de datos: */
/* declaraciones de variables e inicialización de las mismas              */
/* ---------------------------------------------------------------------- */

int compile_array_data( VARSPACE * n, segment * data, int size, int subsize, BASETYPE *t )
{
    int block, count = 0, base, remaining = size ;
    int base_offset, total_length ;

    expresion_result res ;

    for ( ;; )
    {
        if ( !remaining && size )
        {
            token_back();
            break;
//            compile_error( MSG_TOO_MANY_INIT ) ;
        }

        token_next() ;
        /*
        if (*t == TYPE_UNDEFINED && token.type == STRING)
        {
            *t = typedef_base(typedef_new(TYPE_STRING)) ;
        }
        */
        if ( token.type == STRING && *t == TYPE_CHAR )
        {
            const char * str = string_get( token.code ) ;
            int subcount = 0 ;

            if ( subsize == 0 ) subsize = strlen( str ) + 1;

            if (( int )strlen( str ) > subsize ) compile_error( MSG_TOO_MANY_INIT ) ;
            while ( *str )
            {
                segment_add_as( data, *str++, *t ) ;
                subcount++ ;
            }
            while ( subcount++ < subsize ) segment_add_as( data, 0, *t ) ;
            count += subsize ;
            remaining -= subsize ;
        }
        else if ( token.type == IDENTIFIER && ( token.code == identifier_rightp || token.code == identifier_semicolon ) )
        {
            token_back() ;
            break ;
        }
        else if ( token.type == IDENTIFIER && token.code == identifier_leftp )
        {
            block = compile_array_data( n, data, remaining, remaining, t ) ;
            remaining -= block ;
            count += block ;
            token_next() ;
            if ( token.type != IDENTIFIER || token.code != identifier_rightp )
                compile_error( MSG_EXPECTED, ")" ) ;
        }
        else
        {
            token_back() ;
            res = compile_expresion( 1, 0, 0, *t ) ;
            if ( *t == TYPE_UNDEFINED )
            {
                *t = typedef_base( res.type ) ;
                if ( *t == TYPE_UNDEFINED )
                {
                    compile_error( MSG_INCOMP_TYPE ) ;
                }
            }
            base = res.value ;
            if ( *t == TYPE_FLOAT ) base = *( int * ) & res.fvalue ;

            token_next() ;
            if ( token.type == IDENTIFIER && token.code == identifier_dup )
            {
                token_next() ;
                if ( token.type != IDENTIFIER || token.code != identifier_leftp ) compile_error( MSG_EXPECTED, "(" ) ;
                base_offset = data->current ;
                block = compile_array_data( n, data, remaining, remaining, t ) ;
                total_length = data->current - base_offset ;
                if ( size && block * base > remaining )
                {
                    break; /* MSG_TOO_MANY_INIT */
                }
                count += block * base ;
                if ( size ) remaining -= block * base ;
                while ( base-- > 1 )
                {
                    segment_copy( data, base_offset, total_length ) ;
                    base_offset += total_length ;
                }
                token_next() ;
                if ( token.type != IDENTIFIER || token.code != identifier_rightp )
                    compile_error( MSG_EXPECTED, ")" ) ;
            }
            else
            {
                token_back() ;
                if ( *t == TYPE_STRING ) varspace_varstring( n, data->current ) ;
                segment_add_as( data, base, *t ) ;
                count ++ ;
                if ( size ) remaining -- ;
            }
        }
        token_next() ;
        if ( token.type == IDENTIFIER && token.code == identifier_comma )
        {
            if ( !size && *t == TYPE_CHAR ) compile_error( MSG_TOO_MANY_INIT );
            continue ;
        }
        token_back() ;
        break ;
    }

    return count ;
}

static BASETYPE get_basetype( VARSPACE * v )
{
    TYPEDEF t ;
    BASETYPE type = TYPE_UNDEFINED, newtype ;
    int n ;

    for ( n = 0 ; n < v->count ; n++ )
    {
        t = v->vars[n].type ;
        while ( typedef_is_array( t ) )
        {
            t = typedef_reduce( t ) ;
        }

        if ( typedef_is_struct( t ) )
        {
            newtype = get_basetype( typedef_members( t ) )  ;
        }
        else
        {
            newtype = typedef_base( t ) ;
        }

        if ( type != TYPE_UNDEFINED && type != newtype )
        {
            return TYPE_UNDEFINED ;
        }

        type = newtype ;
    }
    return type ;
}

/*
 *  FUNCTION : compile_struct_data
 *
 *  Compile the values of an struct initialization
 *  Does not suppor parenthized sections or the DUP operator
 *
 *  PARAMS:
 *      n       Current variable space
 *      data    Current data segment
 *      size    Size (total number of variables) of the array
 *              including all dimensions (1 for single struct)
 *      sub     1 if the struct we'are initializing is part of
 *              another one
 *             (the function will then ignore a trailing comma)
 *
 *  RETURN VALUE:
 *      Number of structs initialized
 */

int compile_struct_data( VARSPACE * n, segment * data, int size, int sub )
{
    int elements = 0 ;
    int position = 0 ;
    expresion_result res ;

    for ( ;; )
    {
        token_next() ;

        /* Allow parenthized struct initialization */
        if ( token.type == IDENTIFIER && token.code == identifier_leftp )
        {
            if (( elements % n->count ) != 0 ) compile_error( MSG_NOT_ENOUGH_INIT );

            /* Note - don't ignore a trailing comma! */
            elements = compile_struct_data( n, data, size, 0 ) ;

            if ( elements >= n->count ) size -= ( elements / n->count );

            token_next() ;
            if ( token.type != IDENTIFIER || token.code != identifier_rightp ) compile_error( MSG_EXPECTED, ")" ) ;

            token_next() ;
            if (( elements % n->count ) == 0 && size > 0 && token.type == IDENTIFIER && token.code == identifier_comma )
                continue;

            token_back() ;
            return elements ;
        }

        /* Allow empty initialization */
        if ( token.type == IDENTIFIER && token.code == identifier_semicolon ) return 0 ;

        token_back() ;

        for ( ;; )
        {
            TYPEDEF next_type = n->vars[position].type ;

            /* Next variable is a pointer */

            if ( typedef_is_pointer( next_type ) )
            {
                res = compile_expresion( 1, 0, 0, TYPE_DWORD ) ;
                if ( !res.constant ) compile_error( MSG_INCORRECT_PTR_INIT );
                segment_add_as( data, 0, TYPE_POINTER ) ;
            }
            else if ( typedef_is_array( next_type ) )   /* Next variable is an array */
            {
                int elements = typedef_tcount( next_type ) ;
                BASETYPE base;

                /* Get the array base type */

                while ( typedef_is_array( next_type ) ) next_type = typedef_reduce( next_type );

                base = typedef_base( next_type );

                /* Special case: array of structs */

                if ( base == TYPE_STRUCT )
                {
                    compile_struct_data( next_type.varspace, data, elements, 1 );

                }
                else
                {
                    token_next();

                    /* Special case: intializing char[] strings */

                    if ( token.type == STRING && next_type.chunk[1].type == TYPE_CHAR )
                    {
                        const char * str = string_get( token.code ) ;
                        int subcount = 0 ;

                        if (( int )strlen( str ) > typedef_count( next_type ) - 1 ) compile_error( MSG_TOO_MANY_INIT ) ;

                        while ( *str )
                        {
                            segment_add_as( data, *str++, TYPE_CHAR ) ;
                            subcount++ ;
                        }

                        while ( subcount++ < typedef_count( next_type ) ) segment_add_as( data, 0, TYPE_CHAR ) ;

                    }
                    else   /* Initializing normal arrays */
                    {
                        int has_parents = 1;

                        if ( token.type != IDENTIFIER || token.code != identifier_leftp )
                        {
                            has_parents = 0;
                            token_back();
                        }

                        compile_array_data( n, data, elements, elements, &base ) ;

                        if ( has_parents )
                        {
                            token_next() ;
                            if ( token.type != IDENTIFIER || token.code != identifier_rightp ) compile_error( MSG_EXPECTED, ")" ) ;
                        }
                    }
                }
            }
            else if ( typedef_is_struct( next_type ) )   /* Next variable is another struct */
            {
                compile_struct_data( next_type.varspace, data, 1, 1 ) ;
            }
            else   /* Next variable is a single type */
            {
                res = compile_expresion( 1, 0, 0, typedef_base( next_type ) ) ;
                if ( !res.constant ) compile_error( MSG_CONSTANT_EXP );
                segment_add_as( data, typedef_base( next_type ) == TYPE_FLOAT ? *( int* )&res.fvalue : res.value, typedef_base( next_type ) ) ;
            }

            position++ ;
            elements++ ;

            if ( position == n->count && size < 2 && sub ) break ;

            /* A comma should be here */

            token_next() ;
            if ( token.type == IDENTIFIER && token.code == identifier_semicolon )
            {
                token_back();
                break ;
            }

            if ( token.type == IDENTIFIER && token.code == identifier_rightp )
            {
                token_back() ;
                break ;
            }

            if ( token.type != IDENTIFIER || token.code != identifier_comma ) compile_error( MSG_EXPECTED, "," ) ;

            /* Wrap around for the next struct */

            if ( position == n->count )
            {
                if ( size == 1 && !sub ) compile_error( MSG_TOO_MANY_INIT ) ;
                size--;
                position = 0;
            }
        }
        break;
    }

    return elements ;
}

static void set_type( TYPEDEF * t, BASETYPE type )
{
/*
    int n ;

    for ( n = 0 ; n < t->depth ; n++ )
        if ( t->chunk[n].type == TYPE_UNDEFINED )
        {
            t->chunk[n].type = type ;
            break ;
        }
    return ;
*/
    t->chunk[t->depth-1].type = type ;
}

/*
 *  FUNCTION : compile_varspace
 *
 *  Compile a variable space (a LOCAL, PRIVATE or GLOBAL section, or part of a STRUCT data)
 *
 *  PARAMS :
 *      n               Pointer to the VARSPACE object (already initialized)
 *      data            Pointer to the data segment. All variables are added at end.
 *      additive        1 if the compiling is additive (the struct members have non-local offset; GLOBAL case)
 *                      or not (STRUCT case)
 *      copies          Number of data copies (if the STRUCT is part of an array)
 *      collision       Check this varspace array for name collisions (NULL = end of array),
 *                      if nonzero (2006/11/19 19:34 GMT-03:00, Splinter - jj_arg@yahoo.com)
 *      alignment       Byte size for member alignment. Empty data will be added
 *                      to the varspace if its size is not multiple of this value.
 *
 *  In any case, nested varspaces (structs) will have an alignment of 0.
 *
 *  RETURN VALUE :
 *      None
 *
 */

int compile_varspace( VARSPACE * n, segment * data, int additive, int copies, int padding, VARSPACE ** collision, int alignment, int duplicateignore )
{
    int i, j, total_count, last_count = 0 ;
    int base_offset = data->current ;
    int total_length ;
    int size, count ;
    int code ;
    expresion_result res ;
    VARIABLE * var ;
    int unsigned_prefix = 0;
    int signed_prefix = 0;

    BASETYPE basetype = TYPE_UNDEFINED ;
    TYPEDEF type, typeaux;
    segment * segm = NULL ;
    PROCDEF * proc = NULL;

    /* Backup vars */
    BASETYPE basetypeb = TYPE_UNDEFINED ;
    TYPEDEF typeb;
    segment * segmb = NULL ;

    /* Initialize some stuffs */

    type = typedef_new( TYPE_UNDEFINED ) ;
    typeb = typedef_new( TYPE_UNDEFINED ) ;

    for ( ;; )
    {
        if ( n->reserved == n->count ) varspace_alloc( n, 16 ) ;

        if ( alignment && ( n->size % alignment ) > 0 )
        {
            int extra = alignment - ( n->size % alignment );
            if ( n->reserved <= n->count + extra )
            {
                varspace_alloc( n, extra + 16 );
            }
            if ( data->reserved <= data->current + extra )
            {
                segment_alloc( data, extra + 16 );
            }
            n->size += extra;
            data->current += extra;
        }

        token_next() ;

        /* Se salta comas y puntos y coma */

        if ( token.type == NOTOKEN ) break ;

        if ( token.type != IDENTIFIER ) compile_error( MSG_INCOMP_TYPE ) ;

        if ( token.code == identifier_comma )
        {
            basetype = basetypeb ;
            type = typeb ;

            segm = segmb ;

            continue ;
        }
        else if ( token.code == identifier_semicolon )
        {
            basetype = TYPE_UNDEFINED ;
            set_type( &type, TYPE_UNDEFINED ) ;

            basetypeb = basetype ;
            typeb = type ;

            segm = NULL ;
            proc = NULL ;

            continue ;
        }
        else if ( token.code == identifier_end )
        {
            break ;
        }

        /* "Unsigned" */

        if ( token.code == identifier_unsigned )
        {
            unsigned_prefix = 1;
            token_next();
        }
        else if ( token.code == identifier_signed )
        {
            signed_prefix = 1;
            token_next();
        }

        /* Tipos de datos básicos */

        if ( token.code == identifier_dword )
        {
            basetype = signed_prefix ? TYPE_INT : TYPE_DWORD ;
            signed_prefix = unsigned_prefix = 0;
            token_next() ;
        }
        else if ( token.code == identifier_word )
        {
            basetype = signed_prefix ? TYPE_SHORT : TYPE_WORD ;
            signed_prefix = unsigned_prefix = 0;
            token_next() ;
        }
        else if ( token.code == identifier_byte )
        {
            basetype = signed_prefix ? TYPE_SBYTE : TYPE_BYTE ;
            signed_prefix = unsigned_prefix = 0;
            token_next() ;
        }
        else if ( token.code == identifier_int )
        {
            basetype = unsigned_prefix ? TYPE_DWORD : TYPE_INT;
            signed_prefix = unsigned_prefix = 0;
            token_next() ;
        }
        else if ( token.code == identifier_short )
        {
            basetype = unsigned_prefix ? TYPE_WORD : TYPE_SHORT;
            signed_prefix = unsigned_prefix = 0;
            token_next() ;
        }
        else if ( token.code == identifier_char )
        {
            basetype = TYPE_CHAR ;
            token_next() ;
        }
        else if ( token.code == identifier_float )
        {
            basetype = TYPE_FLOAT ;
            token_next() ;
        }
        else if ( token.code == identifier_string )
        {
            basetype = TYPE_STRING ;
            token_next() ;
        }
        else
        {
            if ( !proc && ( proc = procdef_search( token.code ) ) )    /* Variables tipo proceso, Splinter */
            {
                basetype = TYPE_INT ;
                token_next();
            }
            else
            {
                if (
                    token.type == IDENTIFIER &&
                    token.code >= reserved_words &&
                    !segment_by_name( token.code ) )
                {
                    int code = token.code;
                    token_next();
                    if ( token.type == IDENTIFIER && token.code >= reserved_words )
                    {
                        proc = procdef_new( procdef_getid(), code );
                        basetype = TYPE_INT ;
                    }
                    else
                    {
                        token_back();
                    }
                }
            }
        }

        if ( signed_prefix || unsigned_prefix ) compile_error( MSG_INVALID_TYPE );
        if ( basetype != TYPE_STRUCT ) type = typedef_new( basetype ) ;
        if ( basetype == TYPE_UNDEFINED ) type = typedef_new( TYPE_INT ) ;

        /* Tipos de datos definidos por el usuario */
        if ( basetype != TYPE_STRUCT && ( segm = segment_by_name( token.code ) ) )
        {
            basetype = TYPE_STRUCT ;
            type = * typedef_by_name( token.code ) ;
            token_next() ;
        }

        if ( token.type == IDENTIFIER && token.code == identifier_struct )
        {
            type.chunk[0].type = TYPE_STRUCT ;
            type.chunk[0].count = 1 ;
            type.depth = 1 ;
            token_next() ;
            segm = 0 ;
        }

        basetypeb = basetype ;
        typeb = type ;
        segmb = segm ;

        /* Tipos de datos derivados */
        while ( token.type == IDENTIFIER && ( token.code == identifier_pointer || token.code == identifier_multiply ) )
        {
            type = typedef_enlarge( type ) ;
            type.chunk[0].type = TYPE_POINTER ;
            basetype = TYPE_POINTER ;
            segm = NULL ;
            token_next() ;
        }

        /* Nombre del dato */

        if ( token.type != IDENTIFIER ) compile_error( MSG_IDENTIFIER_EXP ) ;

        if ( token.code < reserved_words )
        {
            if ( proc ) compile_error( MSG_VARIABLE_ERROR );
            token_back() ;
            break ;
        }

        if (( var = varspace_search( n, token.code ) ) )
        {
            if ( duplicateignore )
            {
                int skip_all_until_semicolon = 0;
                int skip_equal = 0;

                if ( debug ) compile_warning( 0, MSG_VARIABLE_REDECLARE ) ;

                for ( ;; )
                {
                    token_next() ;

                    /* Se salta todo hasta el puntos y coma o en el end o la coma si no hay asignacion */

                    if ( token.type == NOTOKEN ) break ;

                    if ( token.type == IDENTIFIER )
                    {
                        if ( !skip_equal && token.code == identifier_equal )
                        {
                            skip_all_until_semicolon = 1;
                            continue;
                        }

                        if ( !skip_all_until_semicolon && token.code == identifier_comma ) break;
                        if ( token.code == identifier_semicolon ) break ;
                        if ( token.code == identifier_end ) break ;
                    }
                }
                token_back();
                continue;
            }
            else
                compile_error( MSG_VARIABLE_REDECLARE ) ;
        }

        /* (2006/11/19 19:34 GMT-03:00, Splinter - jj_arg@yahoo.com) */
        if ( collision )
            for ( i = 0; collision[i];i++ )
                if ( varspace_search( collision[i], token.code ) ) compile_error( MSG_VARIABLE_REDECLARE ) ;
        /* (2006/11/19 19:34 GMT-03:00, Splinter - jj_arg@yahoo.com) */

        if ( constants_search( token.code ) ) compile_error( MSG_CONSTANT_REDECLARED_AS_VARIABLE ) ;

        code = token.code ;
        n->vars[n->count].code = token.code ;
        n->vars[n->count].offset = data->current;

        /* Non-additive STRUCT; use zero-based member offsets */

        if ( !additive ) n->vars[n->count].offset -= base_offset;

        token_next() ;

        /* Compila una estructura no predefinida */

        if ( !segm && typedef_is_struct( type ) )
        {
            VARSPACE * members ;

            type.chunk[0].count = 1 ;
            count = 1 ;
            while ( token.type == IDENTIFIER && token.code == identifier_leftb )
            {
                res = compile_expresion( 1, 0, 0, TYPE_INT ) ;
                if ( !typedef_is_integer( res.type ) ) compile_error( MSG_INTEGER_REQUIRED ) ;
                count *= res.value + 1 ;
                type = typedef_enlarge( type ) ;
                type.chunk[0].type  = TYPE_ARRAY ;
                type.chunk[0].count = res.value + 1 ;
                token_next() ;
                if ( token.type != IDENTIFIER || token.code != identifier_rightb ) compile_error( MSG_EXPECTED, "]" ) ;
                token_next() ;
            }
            token_back() ;

            /* Da la vuelta a los índices [10][5] -> [5][10] */

            for ( i = 0 ; i < type.depth ; i++ ) if ( type.chunk[i].type != TYPE_ARRAY ) break ;
            i--;
            for ( j = 0 ; j <= i ; j++ ) typeaux.chunk[ j ] = type.chunk[ i - j ];
            for ( j = 0 ; j <= i ; j++ ) type.chunk[ j ]    = typeaux.chunk[ j ];

            members = ( VARSPACE * )calloc( 1, sizeof( VARSPACE ) ) ;
            if ( !members )
            {
                fprintf( stdout, "compile_varspace: out of memory\n" ) ;
                exit( 1 ) ;
            }
            varspace_init( members ) ;

            size = compile_varspace( members, data, 0, count, 0, NULL, 0, duplicateignore ) ;

            type.varspace = members ;

            token_next() ;
            if ( token.type == IDENTIFIER && token.code == identifier_equal )
            {
                i = data->current ;
                data->current = n->vars[n->count].offset ;
                compile_struct_data( members, data, count, 0 );
                data->current = i ;
            }
            else
            {
                token_back() ;
            }

            for ( i = 0 ; i < members->stringvar_count ; i++ )
                varspace_varstring( n, members->stringvars[i] ) ;

            n->size += typedef_size( type ) ;
            n->vars[n->count].type = type ;
            n->count++ ;

            continue ;  /* No ; */

        }
        else if ( token.type == IDENTIFIER && token.code == identifier_leftb ) /* Compila un array */
        {
            total_count = 1 ;

            while ( token.type == IDENTIFIER && token.code == identifier_leftb )
            {
                if ( type.depth == MAX_TYPECHUNKS ) compile_error( MSG_TOO_MANY_AL ) ;

                type = typedef_enlarge( type ) ;
                type.chunk[0].type = TYPE_ARRAY ;

                token_next() ;
                if ( token.type == IDENTIFIER && token.code == identifier_rightb )
                {
                    type.chunk[0].count = 0 ;
                    if ( total_count != 1 ) compile_error( MSG_VTA ) ;
                    total_count = 0 ;
                    last_count = 0 ;
                }
                else
                {
                    token_back() ;
                    res = compile_expresion( 1, 0, 0, TYPE_DWORD ) ;
                    if ( !total_count ) compile_error( MSG_VTA ) ;
                    total_count *= res.value + 1 ;
                    last_count = res.value + 1 ;
                    type.chunk[0].count = res.value + 1 ;
                    token_next() ;
                    if ( token.type != IDENTIFIER || token.code != identifier_rightb ) compile_error( MSG_EXPECTED, "]" ) ;
                }

                token_next() ;
            }

            /* Da la vuelta a los índices [10][5] -> [5][10] */

            for ( i = 0 ; i < type.depth ; i++ ) if ( type.chunk[i].type != TYPE_ARRAY ) break ;
            i--;
            for ( j = 0 ; j <= i ; j++ ) typeaux.chunk[ j ] = type.chunk[ i - j ];
            for ( j = 0 ; j <= i ; j++ ) type.chunk[ j ]    = typeaux.chunk[ j ];

            if ( segm && token.type == IDENTIFIER && token.code == identifier_equal )
            {
                for ( i = 0 ; i < total_count ; i++ ) segment_add_from( data, segm ) ;
                i = data->current ;
                data->current = n->vars[n->count].offset ;
                compile_struct_data( type.varspace, data, typedef_count( type ), 0 );

                if ( !type.chunk[0].count )
                    type.chunk[0].count = ( data->current - i ) / typedef_size( typeb );
                else
                    data->current = i; /* Solo si ya habia sido alocada */

            }
            else if ( token.type == IDENTIFIER && token.code == identifier_equal )
            {
                /* if (basetype == TYPE_UNDEFINED) basetype = TYPE_INT; */
                i = compile_array_data( n, data, total_count, last_count, &basetype ) ;
                assert( basetype != TYPE_UNDEFINED ) ;
                set_type( &type, basetype ) ;
                if ( total_count == 0 )
                {
                    type.chunk[0].count = i;
                }
                else if ( i < total_count )
                {
                    for ( ; i < total_count; i++ )
                    {
                        if ( basetype == TYPE_STRING ) varspace_varstring( n, data->current ) ;
                        segment_add_as( data, 0, basetype ) ;
                    }
                }
            }
            else if ( segm )
            {
                int string_offset = 0, j;

                if ( total_count == 0 ) compile_error( MSG_EXPECTED, "=" ) ;

                for ( i = 0; i < total_count; i++ )
                {
                    segment_add_from( data, segm ) ;
                    for ( j = 0; j < type.varspace->stringvar_count; j++ )
                        varspace_varstring( n, type.varspace->stringvars[j] + string_offset );
                    string_offset += type.varspace->size;
                }
                token_back() ;
            }
            else
            {
                if ( basetype == TYPE_UNDEFINED )
                {
                    basetype = TYPE_INT;
                    set_type( &type, basetype ) ;
                }

                if ( type.chunk[0].count == 0 ) compile_error( MSG_EXPECTED, "=" ) ;

                for ( i = 0; i < total_count; i++ )
                {
                    if ( basetype == TYPE_STRING ) varspace_varstring( n, data->current ) ;
                    segment_add_as( data, 0, basetype ) ;
                }
                token_back() ;
            }
        }
        else if ( segm && token.type == IDENTIFIER && token.code == identifier_equal ) /* Compila una asignación de valores por defecto */
        {
            segment_add_from( data, segm ) ;
            i = data->current ;
            data->current = n->vars[n->count].offset ;
            if ( !additive ) data->current += base_offset;
            compile_struct_data( type.varspace, data, 1, 0 );
            data->current = i ;

        }
        else if ( token.type == IDENTIFIER && token.code == identifier_equal )
        {
            res = compile_expresion( 1, 0, 0, basetype ) ;

            if ( basetype == TYPE_UNDEFINED )
            {
                basetype = typedef_base( res.type ) ;
                if ( basetype == TYPE_UNDEFINED ) compile_error( MSG_INCOMP_TYPE ) ;
                set_type( &type, basetype ) ;
            }

            if ( basetype == TYPE_FLOAT )
            {
                segment_add_as( data, *( int * )&res.fvalue, basetype ) ;
            }
            else
            {
                if ( basetype == TYPE_STRING ) varspace_varstring( n, data->current ) ;
                segment_add_as( data, res.value, basetype ) ;
            }

        }
        else if ( !segm ) /* Asigna valores por defecto (0) */
        {
            if ( basetype == TYPE_UNDEFINED )
            {
                basetype = TYPE_INT;
                set_type( &type, basetype ) ;
            }

            if ( basetype == TYPE_STRING ) varspace_varstring( n, data->current ) ;
            segment_add_as( data, 0, basetype ) ;
            token_back() ;

        }
        else
        {
            if ( typedef_is_struct( type ) )
                for ( i = 0 ; i < type.varspace->stringvar_count ; i++ )
                    varspace_varstring( n, type.varspace->stringvars[i] + n->size );
            segment_add_from( data, segm ) ;
            token_back() ;
        }

        n->size += typedef_size( type ) ;
        n->vars[n->count].type = type ;

        /* Variables tipo proceso, asigno varspace al tipo. Splinter */
        if ( proc ) n->vars[n->count].type.varspace = proc->pubvars;

        n->count++ ;

        token_next() ;

        if ( token.type == IDENTIFIER && token.code == identifier_comma )
        {
            token_back() ;
            continue ;
        }

        if ( token.type == IDENTIFIER && token.code == identifier_semicolon )
        {
            token_back() ;
            continue ;
        }

        compile_error( MSG_EXPECTED, ";" ) ;
        token_back() ;
        break ;
    }

    if ( padding && ( data->current % padding ) > 0 )
    {
        padding -= data->current % padding;
        data->current += padding;
        n->size += padding;

        if ( data->reserved <= data->current )
            segment_alloc( data, data->reserved - data->current + 32 );
    }

    n->last_offset = data->current ;
    total_length = data->current - base_offset ;

    /* n->size *= copies ; */
    while ( copies-- > 1 )
    {
        int i ;

        for ( i = 0 ; i < n->stringvar_count ; i++ )
        {
            if ( n->stringvars[i] >= base_offset && n->stringvars[i] < base_offset + total_length )
            {
                varspace_varstring( n, n->stringvars[i] - base_offset + data->current ) ;
            }
        }

        segment_copy( data, base_offset, total_length ) ;
        base_offset += total_length ;
    }

    return total_length ;
}
