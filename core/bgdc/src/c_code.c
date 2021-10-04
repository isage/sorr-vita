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

extern int autodeclare ;

int reduce_arrays = 1;

/* ---------------------------------------------------------------------- */
/* Compilador de expresiones y sentencias. En este módulo están todas las */
/* funciones de compilado que generan código efectivo.                    */
/* ---------------------------------------------------------------------- */

PROCDEF * proc ;
CODEBLOCK * code ;

/* Comprueba que los parámetros de una expresión binaria sean datos
 * numéricos. Devuelve el tipo de operación(MN_FLOAT o MN_DWORD) */

static int check_integer_type( expresion_result *exp )
{
    if ( typedef_is_pointer( exp->type ) || typedef_base( exp->type ) == TYPE_CHAR )
    {
/*        codeblock_add(code, MN_POINTER2BOL, 0) ; */
        exp->type = typedef_new( TYPE_DWORD ) ;
    }

    if ( typedef_is_integer( exp->type ) )
    {
        BASETYPE t = typedef_base( exp->type ) ;
        switch (t)
        {
            case TYPE_SHORT	 :
            case TYPE_WORD	 :
                return MN_WORD ;

            case TYPE_SBYTE	 :
            case TYPE_BYTE	 :
                return MN_BYTE ;

            case TYPE_UNDEFINED :
            case TYPE_INT       :
            case TYPE_DWORD     :
            case TYPE_CHAR      :
            case TYPE_FLOAT     :
            case TYPE_STRING    :
            case TYPE_ARRAY     :
            case TYPE_STRUCT    :
            case TYPE_POINTER   :
                break;
        }
        return MN_DWORD ;
    }

    compile_error( MSG_INTEGER_REQUIRED ) ;
    return 0;
}

static int check_integer_types( expresion_result *left, expresion_result *right )
{
    if ( typedef_is_pointer( left->type ) || typedef_base( left->type ) == TYPE_CHAR )
    {
/*        codeblock_add(code, MN_POINTER2BOL, 1) ; */
        left->type = typedef_new( TYPE_DWORD ) ;
    }

    if ( typedef_is_pointer( right->type ) || typedef_base( right->type ) == TYPE_CHAR )
    {
/*        codeblock_add(code, MN_POINTER2BOL, 0) ; */
        right->type = typedef_new( TYPE_DWORD ) ;
    }

    if ( typedef_is_integer( left->type ) )
    {
        if ( typedef_is_integer( right->type ) )
        {
            if ( typedef_base( left->type ) == typedef_base( right->type ) )
            {
                BASETYPE t = typedef_base( left->type ) ;
                switch (t)
                {
                    case TYPE_SHORT	 :
                    case TYPE_WORD	 :
                        return MN_WORD ;

                    case TYPE_SBYTE	 :
                    case TYPE_BYTE	 :
                        return MN_BYTE ;

                    case TYPE_UNDEFINED :
                    case TYPE_INT       :
                    case TYPE_DWORD     :
                    case TYPE_CHAR      :
                    case TYPE_FLOAT     :
                    case TYPE_STRING    :
                    case TYPE_ARRAY     :
                    case TYPE_STRUCT    :
                    case TYPE_POINTER   :
                        break;
                }
            }
            return MN_DWORD ;
        }
    }

    compile_error( MSG_INTEGER_REQUIRED ) ;
    return 0;
}

static int check_numeric_types( expresion_result *left, expresion_result *right )
{
    if ( typedef_base( left->type ) == TYPE_FLOAT )
    {
        if ( typedef_base( right->type ) == TYPE_FLOAT ) return MN_FLOAT ;

        if ( typedef_is_integer( right->type ) )
        {
            codeblock_add( code, MN_INT2FLOAT | mntype( right->type, 0 ), 0 ) ;
            right->fvalue = ( float )right->value ;
            return MN_FLOAT ;
        }
    }

    if ( typedef_is_integer( left->type ) || typedef_is_pointer( left->type ) )
    {
        if ( typedef_is_integer( right->type ) || typedef_is_pointer( right->type ) )
        {
            if ( typedef_base( left->type ) == typedef_base( right->type ) ) return mntype( left->type, 0 );
            if ( typedef_base( left->type ) < typedef_base( right->type ) ) return mntype( left->type, 0 ) ;
            return mntype( right->type, 0 ) ;
        }

        if ( typedef_base( right->type ) == TYPE_FLOAT )
        {
            codeblock_add( code, MN_INT2FLOAT, 1 ) ;
            left->fvalue = ( float )left->value ;
            return MN_FLOAT ;
        }
    }

    if ( typedef_base( left->type ) == TYPE_CHAR )
    {
        if ( typedef_base( right->type ) == TYPE_STRING )
        {
            codeblock_add( code, MN_STR2CHR, 0 );
            if ( right->constant == 1 ) right->value = ( unsigned char ) * ( string_get( right->value ) );
            right->type = typedef_new( TYPE_CHAR );
            return MN_BYTE;
        }
        if ( typedef_base( right->type ) == TYPE_CHAR || typedef_is_integer( right->type ) ) return MN_BYTE;
    }

    if ( typedef_base( right->type ) == TYPE_CHAR )
    {
        if ( typedef_base( left->type ) == TYPE_STRING )
        {
            codeblock_add( code, MN_STR2CHR, 1 );
            if ( left->constant == 1 ) left->value = ( unsigned char ) * ( string_get( left->value ) );
            left->type = typedef_new( TYPE_CHAR );
            return MN_BYTE;
        }
        if ( typedef_base( left->type ) == TYPE_CHAR || typedef_is_integer( left->type ) ) return MN_BYTE;
    }

    compile_error( MSG_INCOMP_TYPES ) ;
    return 0 ;
}

/* Comprueba que los parámetros de una expresión binaria sean cadenas
 * o datos numéricos. Devuelve MN_STRING o el tipo de dato numérico */

static int check_numeric_or_string_types( expresion_result * left, expresion_result * right )
{
    if ( typedef_is_array( left->type ) && left->type.chunk[1].type == TYPE_CHAR && typedef_is_string( right->type ) )
    {
        left->type = typedef_new( TYPE_STRING );
        left->lvalue = 0;
        codeblock_add( code, MN_A2STR, 1 ) ;
        return MN_STRING;
    }
    if ( typedef_is_array( right->type ) && right->type.chunk[1].type == TYPE_CHAR && typedef_is_string( left->type ) )
    {
        right->type = typedef_new( TYPE_STRING );
        right->lvalue = 0;
        codeblock_add( code, MN_A2STR, 0 ) ;
        return MN_STRING;
    }
    if (
        typedef_is_array( right->type ) && right->type.chunk[1].type == TYPE_CHAR &&
        typedef_is_array( left->type )  &&  left->type.chunk[1].type == TYPE_CHAR )
    {
        left->type = typedef_new( TYPE_STRING );
        right->type = typedef_new( TYPE_STRING );
        left->lvalue = 0;
        right->lvalue = 0;
        codeblock_add( code, MN_A2STR, 0 ) ;
        codeblock_add( code, MN_A2STR, 1 ) ;
        return MN_STRING;
    }

    if ( typedef_base( left->type ) == TYPE_CHAR && typedef_base( right->type ) == TYPE_STRING )
    {
        codeblock_add( code, MN_CHR2STR, 1 );
        left->lvalue = 0;
        left->type = typedef_new( TYPE_STRING );
        return MN_STRING;
    }

    if ( typedef_base( right->type ) == TYPE_CHAR && typedef_base( left->type ) == TYPE_STRING )
    {
        codeblock_add( code, MN_CHR2STR, 0 );
        right->lvalue = 0;
        right->type = typedef_new( TYPE_STRING );
        return MN_STRING;
    }

    if ( typedef_is_string( left->type ) && typedef_is_string( right->type ) ) return MN_STRING ;
    if ( typedef_is_string( left->type ) || typedef_is_string( right->type ) ) compile_error( MSG_INCOMP_TYPES ) ;

    return check_numeric_types( left, right ) ;
}

/* Devuelve el código que hay que adjuntar a un mnemónico para producir
 * una variante del mismo, adecuada al tipo de dato concreto */

int mntype( TYPEDEF type, int accept_structs )
{
    BASETYPE t ;

    while ( typedef_is_array( type ) ) type = typedef_reduce( type ) ;

    t = typedef_base( type ) ;
    switch (t)
    {
        case TYPE_DWORD   : return MN_DWORD | MN_UNSIGNED;
        case TYPE_INT     : return MN_DWORD;
        case TYPE_WORD    : return MN_WORD | MN_UNSIGNED;
        case TYPE_SHORT   : return MN_WORD ;
        case TYPE_BYTE    : return MN_BYTE | MN_UNSIGNED;
        case TYPE_SBYTE   : return MN_BYTE ;
        case TYPE_CHAR    : return MN_BYTE ;
        case TYPE_FLOAT   : return MN_FLOAT ;
        case TYPE_STRING  : return MN_STRING;
        case TYPE_POINTER : return MN_DWORD ;
        case TYPE_UNDEFINED :
        case TYPE_ARRAY     :
        case TYPE_STRUCT    :
            break;
    }

    if ( t == TYPE_STRUCT && accept_structs ) return 0;

    compile_error( MSG_INCOMP_TYPE ) ;
    return 0;
}

/* Compila el acceso a una VARIABLE global, local, privada o publica */

expresion_result compile_sublvalue( VARSPACE * from, int base_offset, VARSPACE * remote )
{
    VARIABLE           * var = NULL ;
    VARSPACE           * here = from ;
    VARSPACE           * privars = ( proc ? proc->privars : NULL ) ;
    VARSPACE           * pubvars = ( proc ? proc->pubvars : NULL ) ;
    expresion_result    res, ind ;

    if ( here ) token_next() ;

    if ( token.type != IDENTIFIER ) compile_error( MSG_IDENTIFIER_EXP ) ;

    if ( !here && !remote )
    {
        /* Splinter, se agrega localidad... */
        if ( proc )
        {
            here = privars ;
            var = varspace_search( here, token.code ) ;
            if ( !var )
            {
                here = pubvars ;
                var = varspace_search( here, token.code ) ;
            }
        }
        if ( !var )
        {
            here = &local ;
            var = varspace_search( here, token.code ) ;
        }
        if ( !var )
        {
            here = &global ;
            var = varspace_search( here, token.code ) ;
        }
    }
    else
    {
        if ( remote )
        {
            here = remote ;
            var = varspace_search( here, token.code ) ;
        }

        if ( !var )
        {
            here = from;
            var = varspace_search( here, token.code ) ;
        }
    }
    if ( !var ) compile_error( MSG_UNKNOWN_IDENTIFIER ) ;

    if ( var->offset - base_offset != 0 ||
            here == &global ||
            here == &local  ||
            ( privars && ( here == privars ) ) ||
            ( pubvars && ( here == pubvars ) ) ||
            ( remote  && ( here == remote ) )
       )
    {
        codeblock_add( code, (
                    ( remote && ( here == remote ) )            ? MN_REMOTE_PUBLIC  :
                    here == &global                             ? MN_GLOBAL         :
                    (( here == &local ) && ( here == from ) )   ? MN_REMOTE         :
                    here == &local                              ? MN_LOCAL          :
                    ( privars && ( here == privars ) )          ? MN_PRIVATE        :
                    ( pubvars && ( here == pubvars ) )          ? MN_PUBLIC         :
                    MN_INDEX
                ) | mntype( var->type, 1 ), var->offset - base_offset ) ;


        if (( pubvars && ( here == pubvars ) ) || ( remote  && ( here == remote ) ) )  /* Tambien las locales remotas ? */
        {
            proc->flags |= PROC_USES_PUBLICS;
        }

        if ( here == &local )
        {
            proc->flags |= PROC_USES_LOCALS;
        }
    }

    token_next() ;

    res.type       = var->type ;
    res.lvalue     = 1 ;
    res.asignation = 0 ;
    res.constant   = 0 ;
    res.call       = 0 ;
    res.value      = 0 ;
    res.count      = 1 ;

    /* Indexado vía [...] */

    while ( token.type == IDENTIFIER && token.code == identifier_leftb ) /* "[" */
    {
        /* De estructuras o arrays */
        if ( typedef_is_struct( res.type )  && typedef_count( res.type ) == 1 ) compile_error( MSG_NOT_AN_ARRAY ) ;

        /* Cadenas y punteros se indexan en otro nivel */
        if ( typedef_is_pointer( res.type ) || typedef_is_string( res.type ) ) break ;

        if ( !typedef_is_struct( res.type ) && !typedef_is_array( res.type ) ) compile_error( MSG_NOT_AN_ARRAY ) ;

        ind = compile_expresion( 0, 0, 0, TYPE_DWORD ) ;
        if ( ind.lvalue ) codeblock_add( code, MN_PTR | mntype( ind.type, 0 ), 0 ) ;

        if ( ind.constant && ( ind.value < 0 || ind.value >= typedef_count( res.type ) ) ) compile_error( MSG_BOUND ) ;

        token_next() ;
        if ( token.type != IDENTIFIER || token.code != identifier_rightb ) compile_error( MSG_EXPECTED, "]" ) ; /* "]" */

        if ( typedef_is_array( res.type ) )
        {
            res.type = typedef_reduce( res.type ) ;
            codeblock_add( code, MN_ARRAY, typedef_size( res.type ) ) ;
        }
        else /* estructura */
        {
            codeblock_add( code, MN_ARRAY, typedef_size( res.type ) / typedef_count( res.type ) ) ;
        }

        token_next() ;
    }

    /* Un acceso a un array es un acceso a su primer elemento */
/*
    res.count = 1;
    if ( reduce_arrays == 1 && typedef_is_array( res.type ) )
    {
        if ( res.type.chunk[1].type != TYPE_CHAR )
        {
            while ( typedef_is_array( res.type ) )
            {
                res.count *= ( typedef_count( res.type ) ? typedef_count( res.type ) : 1 );
                res.type = typedef_reduce( res.type ) ;
            }
        }
    }
*/

    token_back() ;

    return res ;
}

/* Compila el tamaño de una VARIABLE o estructura local, global, privada o publica */

#define MAX_EXPR_LEVEL 256

int compile_sizeof( VARSPACE * here, int * content_size, char * content_type, int * parent_count )
{
    VARIABLE * var  = NULL ;
    expresion_result ind ;
    int base = 0 ;
    TYPEDEF type, * usertype ;
    int _content_size = 0;
    char * _content_type = NULL;
    int _parent_count = 0;
    int check_datatype = 0;
    int tcode;
    int index_pointer = 0;

    if ( !content_size ) content_size = &_content_size;
    if ( !content_type ) content_type = _content_type = (char *)calloc( MAX_EXPR_LEVEL, sizeof( int ) );
    if ( !parent_count ) parent_count = &_parent_count;

    /* Basics datatypes */

    token_next() ;
    if ( token.type != IDENTIFIER && ( token.code == identifier_unsigned || token.code == identifier_signed ) ) /* "UNSIGNED" or "SIGNED" */
    {
        check_datatype = 1;
        token_next();
    }

    if ( token.type != IDENTIFIER ) compile_error( MSG_INCOMP_TYPE ) ;

    /* Base datatypes */

    if ( token.code == identifier_dword )    base = 4 ;
    else if ( token.code == identifier_int )      base = 4 ;
    else if ( token.code == identifier_string )   base = 4 ;
    else if ( token.code == identifier_float )    base = 4 ;
    else if ( token.code == identifier_short )    base = 2 ;
    else if ( token.code == identifier_word )     base = 2 ;
    else if ( token.code == identifier_char )     base = 1 ;
    else if ( token.code == identifier_byte )     base = 1 ;
    else
    {
        usertype = typedef_by_name( token.code ) ;
        if ( usertype ) base = typedef_size( *usertype ) ;
    }

    if ( base )
    {
          token_next() ;
        if ( token.type == IDENTIFIER && token.code != identifier_point )
        {
            token_back() ;
            for ( ;; )
            {
                token_next() ;
                if ( token.type == IDENTIFIER && ( token.code == identifier_pointer || token.code == identifier_multiply ) ) /* "POINTER" */
                {
                    base = 4 ;
                    continue ;
                }
                token_back() ;
                break ;
            }
            return base ;
        }
        token_back() ;
    }

    if ( check_datatype ) compile_error( MSG_INVALID_TYPE ) ;

    /* Expressions datatypes */

    /* Count "*" or "[" */

    while ( token.type == IDENTIFIER && ( token.code == identifier_multiply || token.code == identifier_leftb || token.code == identifier_leftp ) )
    {
        if ( *content_size == MAX_EXPR_LEVEL ) compile_error( MSG_TOO_COMPLEX );
        if ( token.code == identifier_leftp )( *parent_count )++;
        content_type[( *content_size )++] = token.code;
        token_next() ;
    }

    if ( !here )
    {
        /* Splinter, se agrega localidad... */
        here = proc->privars ;
        var = varspace_search( here, token.code ) ;
        if ( !var )
        {
            here = proc->pubvars ;
            var = varspace_search( here, token.code ) ;
        }
        if ( !var )
        {
            here = &local ;
            var = varspace_search( here, token.code ) ;
        }
        if ( !var )
        {
            here = &global ;
            var = varspace_search( here, token.code ) ;
        }
    }
    else
    {
        var = varspace_search( here, token.code ) ;
    }

    if ( !var ) compile_error( MSG_UNKNOWN_IDENTIFIER ) ;

    type = var->type;

    token_next() ;

    /* Indexado de arrays */

    while ( token.type == IDENTIFIER && token.code == identifier_leftb ) /* "[" */
    {
        CODEBLOCK_POS p = codeblock_pos( code );

        /* Cadenas y punteros se indexan en otro nivel */
        if ( typedef_is_pointer( type ) || typedef_is_string( type ) ) break ;

        /* De estructuras o arrays */
        if ( typedef_is_struct( type ) && typedef_count( type ) == 1 ) compile_error( MSG_NOT_AN_ARRAY ) ;

        if ( !typedef_is_struct( type ) && !typedef_is_array( type ) ) compile_error( MSG_NOT_AN_ARRAY ) ;

        ind = compile_expresion( 0, 0, 0, TYPE_DWORD ) ;
        if ( !ind.constant || ( ind.value < 0 || ind.value >= typedef_count( type ) ) ) compile_error( MSG_BOUND ) ;

        codeblock_setpos( code, p );

        token_next() ;
        if ( token.type != IDENTIFIER || token.code != identifier_rightb ) compile_error( MSG_EXPECTED, "]" ) ; /* "]" */

        if ( typedef_is_array( type ) )
            type = typedef_reduce( type ) ;
        else
            break ;

        token_next() ;

    }

    if ( token.type == IDENTIFIER && token.code == identifier_leftb &&
         ( typedef_is_pointer( type ) || typedef_is_string( type ) ) ) /* Indexado de punteros ptr[0] o cadenas */
    {
        /* Ningun array entra por aca */
        CODEBLOCK_POS p = codeblock_pos( code );

        ind = compile_subexpresion() ;
        if ( !typedef_is_integer( ind.type ) ) compile_error( MSG_INTEGER_REQUIRED ) ;

        token_next() ;
        if ( token.type != IDENTIFIER || token.code != identifier_rightb ) compile_error( MSG_EXPECTED, "]" ) ; /* "]" */

        codeblock_setpos( code, p );

        if ( typedef_is_pointer( type ) )
        {
            type = typedef_reduce( type );
            token_next();
            index_pointer = 1;
        }
        else if ( typedef_is_string( type ) )
            return 1; /* Indexado de cadenas */
    }

    /* Check for pointer by ']' or ')' */
    while ( token.type == IDENTIFIER && ( token.code == identifier_rightb || token.code == identifier_rightp ) )
    {
        if ( token.code == identifier_rightp ) /* ')' */
        {
            if ( !*content_size || !*parent_count ) break;

            while (( *content_size ) && content_type[( *content_size )-1] == identifier_multiply ) /* only '*', '[]' must be already process */
            {
                if ( !typedef_is_pointer( type ) ) compile_error( MSG_NOT_A_POINTER ) ;
                type = typedef_reduce( type );
                ( *content_size )--;
            }

            if ( content_type[--( *content_size )] != identifier_leftp ) compile_error( MSG_INVALID_EXP );
            ( *parent_count )--;
            token_next() ;
        }
        else
        {
            if ( !*content_size ) compile_error( MSG_EXTRA_CHAR );

            while ( *content_size &&
                    (
                        ( tcode = content_type[( *content_size )-1] ) == identifier_leftb ||
                        tcode == identifier_multiply
                    )
                  )
            {
                /* '[' or '*' */
                if ( !typedef_is_pointer( type ) ) compile_error( MSG_NOT_A_POINTER ) ;

                type = typedef_reduce( type );
                ( *content_size )--;
                if ( tcode == identifier_leftb )
                {
                    token_next() ;
                    break ;
                }
            }
        }
    }

    if ( index_pointer && ( token.type != IDENTIFIER || token.code != identifier_point ) ) /* "." */
    {
        token_back();
        return typedef_size( type ); /* Indexado de punteros ptr[0] */
    }

    if ( token.type == IDENTIFIER && token.code == identifier_point ) /* "." */
    {
        if ( typedef_is_pointer( type ) ) type = typedef_reduce( type ) ;
        if ( !typedef_is_struct( type ) &&
              typedef_base( type ) != TYPE_DWORD && typedef_base( type ) != TYPE_INT  /* Soporte de process type para publicas */
           )
            compile_error( MSG_STRUCT_REQUIRED ) ;

        if ( typedef_is_struct( type ) || typedef_is_pointer( type ) ||
             typedef_base( type ) == TYPE_DWORD || typedef_base( type ) == TYPE_INT )
            return compile_sizeof( typedef_members( type ), content_size, content_type, parent_count ) ;

        return compile_sizeof( &local, content_size, content_type, parent_count ) ;
    }

    /*   Process "*ptr|**ptr|***ptr|..."      */
    while (( *content_size )-- )
    {
        if ( content_type[*content_size] != identifier_multiply ) compile_error( MSG_INVALID_EXP );
        if ( !typedef_is_pointer( type ) ) compile_error( MSG_NOT_A_POINTER ) ;
        type = typedef_reduce( type );
    }

    if ( _content_type ) free( _content_type );

    token_back() ;
    return typedef_size( type ) ;

}

/* Static utility function for compile_bestproc */

static void strdelchars( char * str, char * chars )
{
    while ( *str )
    {
        if ( strchr( chars, *str ) )
            strcpy( str, str + 1 ) ;
        else
            str++ ;
    }
}

/*
 *  FUNCTION : compile_bestproc
 *
 *  Compile a system call, given a list of system functions
 *  with the same name
 *
 *  PARAMS:
 *      procs           List of system functions
 *
 *  RETURN VALUE:
 *      Identifier code allocated for the function
 */

SYSPROC * compile_bestproc( SYSPROC ** procs )
{
    int n, proc_count = 0 ;
    expresion_result res ;
    int count = 0 ;
    char validtypes[32] ;
    char type = -1 ;
    int min_params = 0 ;
    const char * proc_name = procs[0]->name ;

    int param_diff, params = 0;
    CODEBLOCK_POS code_pos_code;
    tok_pos token_save;

    /* --------------------------------------------------- */

    while ( procs[proc_count] ) proc_count++ ;

    /* Get the minimum number of parameters */

    for ( n = 0 ; n < proc_count ; n++ )
        if ( procs[n]->params > min_params )
            min_params = procs[n]->params ;

    /* --------------------------------------------------- */

    /* count function params */

    code_pos_code = codeblock_pos( code );
    token_save = token_pos();

    params = 0;

    for ( ;; )
    {
        token_next() ;
        if ( token.type == IDENTIFIER && token.code == identifier_rightp ) /* ")" */
        {
            token_back() ;
            break ;
        }
        token_back() ;

        params++ ;

        res = compile_expresion( 0, 0, 0, TYPE_UNDEFINED ) ;

        token_next() ;
        if ( token.type != IDENTIFIER || token.code != identifier_comma ) /* "," */
        {
            token_back() ;
            break ;
        }
    }

    codeblock_setpos(code, code_pos_code);
    token_set_pos(token_save);

    /* Eliminate any process that has not as many parameters */

    param_diff = 0;
    for ( n = 0 ; n < proc_count ; n++ )
    {
        char * p = procs[n]->paramtypes;

        param_diff = 0;

        while( ( *p ) ) if ( *(p++) == '+' ) param_diff++;

        if ( procs[n]->params - param_diff != params )
        {
            memmove( &procs[n], &procs[n+1], sizeof( SYSPROC* ) *( proc_count - n ) ) ;
            proc_count-- ;
            n-- ;
        }
    }

    count = 0;

    for ( ;; )
    {
        token_next() ;
        if ( token.type == IDENTIFIER && token.code == identifier_rightp ) /* ")" */
        {
            token_back() ;
            break ;
        }
        token_back() ;

        count++ ;

        /* Eliminate any process that has not as many parameters */
/*
        for ( n = 0 ; n < proc_count ; n++ )
        {
            if ( procs[n]->params < count )
            {
                memmove( &procs[n], &procs[n+1], sizeof( SYSPROC* ) *( proc_count - n ) ) ;
                proc_count-- ;
                n-- ;
            }
        }
*/
        if ( proc_count == 0 ) compile_error( MSG_INCORRECT_PARAMC, proc_name, min_params ) ;

        /* Find all the available types */

        validtypes[0] = 0 ;
        for ( n = 0 ; n < proc_count ; n++ )
        {
            if ( !strchr( validtypes, procs[n]->paramtypes[count-1] ) )
            {
                validtypes[strlen( validtypes )+1] = 0 ;
                validtypes[strlen( validtypes )] = procs[n]->paramtypes[count-1];
            }
        }

        if ( strlen( validtypes ) == 1 )
        {
            /* Same type for any function variant */

            if ( validtypes[0] == 'V' )
            {
                /* Function will receive a varspace struct */
                reduce_arrays = 0;
                res = compile_expresion( 0, 1, 0, TYPE_UNDEFINED );
                reduce_arrays = 1;
                /*
                                while ( typedef_is_pointer( res.type ) )
                                {
                                    codeblock_add( code, MN_PTR, 0 );
                                    res.type = typedef_reduce( res.type );
                                }
                */
                if ( typedef_is_struct( res.type ) )
                {
                    int size = res.type.varspace->count * sizeof( DCB_TYPEDEF );
                    int nvar;

                    segment_alloc( globaldata, size );
                    codeblock_add( code, MN_GLOBAL, globaldata->current ) ;
                    for ( nvar = 0 ; nvar < res.type.varspace->count ; nvar++ )
                    {
                        DCB_TYPEDEF type;
                        dcb_settype( &type, &res.type.varspace->vars[nvar].type );
                        memcpy(( uint8_t* )globaldata->bytes + globaldata->current, &type, sizeof( DCB_TYPEDEF ) );
                        globaldata->current += sizeof( DCB_TYPEDEF );
                    }
                    codeblock_add( code, MN_PUSH, res.type.varspace->count );
                    count += 2;
                }
                else
                {
                    DCB_TYPEDEF type;
                    dcb_settype( &type, &res.type );
                    segment_alloc( globaldata, sizeof( TYPEDEF ) );
                    codeblock_add( code, MN_GLOBAL, globaldata->current ) ;
                    memcpy(( uint8_t* )globaldata->bytes + globaldata->current, &type, sizeof( DCB_TYPEDEF ) );
                    globaldata->current += sizeof( DCB_TYPEDEF );
                    codeblock_add( code, MN_PUSH, 1 );
                    count += 2;
                }
            }
            else
            {
                switch ( validtypes[0] )
                {
                    case 'I':
                        type = TYPE_DWORD   ;
                        break ;
                    case 'B':
                        type = TYPE_BYTE    ;
                        break ;
                    case 'W':
                        type = TYPE_WORD    ;
                        break ;
                    case 'S':
                        type = TYPE_STRING  ;
                        break ;
                    case 'P':
                        type = TYPE_POINTER ;
                        break ;
                    case 'F':
                        type = TYPE_FLOAT   ;
                        break ;
                    default:
                        compile_error( MSG_INVALID_PARAMT ) ;
                }

                res = compile_expresion( 0, 0, 0, (BASETYPE)type ) ;
                if ( res.lvalue ) codeblock_add( code, mntype( res.type, 0 ) | MN_PTR, 0 ) ;
            }
        }
        else
        {
            /* Different types availables */

            res = compile_expresion( 0, 0, 0, TYPE_UNDEFINED ) ;
            if ( res.lvalue ) codeblock_add( code, mntype( res.type, 0 ) | MN_PTR, 0 ) ;

            /* Eliminate any incompatible data type, but allow some
             * conversions if no exact match is available */

            switch ( typedef_base( res.type ) )
            {
                case TYPE_DWORD:
                case TYPE_SHORT:
                case TYPE_BYTE:
                case TYPE_SBYTE:
                case TYPE_WORD:
                case TYPE_INT:
                    strdelchars( validtypes, "SFP" ) ;
                    break ;
                case TYPE_FLOAT:
                    if ( strchr( validtypes, 'F' ) )
                        strdelchars( validtypes, "SPIWB" ) ;
                    else
                        strdelchars( validtypes, "SP" ) ;
                    break ;
                case TYPE_STRING:
                    if ( strchr( validtypes, 'S' ) )
                        strdelchars( validtypes, "FPIWB" ) ;
                    else
                        strdelchars( validtypes, "P" ) ;
                    break ;
                case TYPE_POINTER:
                    strdelchars( validtypes, "SFIWB" ) ;
                    break ;
                default:
                    break ;
            }

            if ( strlen( validtypes ) != 1 ) compile_error( MSG_INVALID_PARAMT ) ;

            /* Eliminate all functions that are not selected */

            for ( n = 0 ; n < proc_count ; n++ )
            {
                if ( procs[n]->paramtypes[count-1] != validtypes[0] )
                {
                    memmove( &procs[n], &procs[n+1], sizeof( SYSPROC* ) *( proc_count - n ) ) ;
                    proc_count-- ;
                    n-- ;
                }
            }

            if ( strlen( validtypes ) > 1 ) continue ;

            /* Convert the result to the appropiate type, if needed */

            switch ( validtypes[0] )
            {
                case 'I':
                    type = TYPE_DWORD   ;
                    break ;
                case 'B':
                    type = TYPE_BYTE    ;
                    break ;
                case 'W':
                    type = TYPE_WORD    ;
                    break ;
                case 'S':
                    type = TYPE_STRING  ;
                    break ;
                case 'P':
                    type = TYPE_POINTER ;
                    break ;
                case 'F':
                    type = TYPE_FLOAT   ;
                    break ;
                default:
                    compile_error( MSG_INVALID_PARAMT ) ;
            }
            res = convert_result_type( res, (BASETYPE)type ) ;
        }

        token_next() ;
        if ( token.type != IDENTIFIER || token.code != identifier_comma ) /* "," */
        {
            token_back() ;
            break ;
        }
    }

    /* Eliminate any process that has too many parameters */

    for ( n = 0 ; n < proc_count ; n++ )
    {
        if ( procs[n]->params != count )
        {
            memmove( &procs[n], &procs[n+1], sizeof( SYSPROC* ) *( proc_count - n ) ) ;
            proc_count-- ;
            n-- ;
        }
    }

    if ( proc_count > 1 ) compile_error( MSG_MULTIPLE_PROCS_FOUND, proc_name );
    if ( proc_count == 0 ) compile_error( MSG_INCORRECT_PARAMC, proc_name, min_params ) ;
    codeblock_add( code, MN_SYSCALL, procs[0]->code ) ;

    return procs[0] ;
}

/* Compila una lista de parámetros */

int compile_paramlist( BASETYPE * types, const char * paramtypes )
{
    expresion_result res ;
    int count = 0, type ;

    for ( ;; )
    {
        type = types ? *types : TYPE_UNDEFINED ;
        if ( paramtypes )
        {
            switch ( *paramtypes++ )
            {
                case 'I':
                    type = TYPE_DWORD   ;
                    break ;
                case 'B':
                    type = TYPE_BYTE    ;
                    break ;
                case 'W':
                    type = TYPE_WORD    ;
                    break ;
                case 'S':
                    type = TYPE_STRING  ;
                    break ;
                case 'P':
                    type = TYPE_POINTER ;
                    break ;
                case 'F':
                    type = TYPE_FLOAT   ;
                    break ;
                default:
                    compile_error( MSG_INVALID_PARAMT ) ;
            }
        }

        res = compile_expresion( 0, 0, 0, (BASETYPE)type ) ;

        if ( types )
        {
            if ( *types == TYPE_UNDEFINED )
                *types = typedef_base( res.type ) ;
            types++ ;
        }
        if ( res.lvalue ) codeblock_add( code, mntype( res.type, 0 ) | MN_PTR, 0 ) ;
        count++ ;

        token_next() ;
        if ( token.type == IDENTIFIER && token.code == identifier_comma ) continue ; /* "," */
        token_back() ;
        break ;
    }
    return count ;
}

/*
 *  FUNCTION : compile_cast
 *
 *  Compiles a cast operator(c-like type conversion:([type])value)
 *
 *  PARAMS :
 *      None
 *
 *  RETURN VALUE :
 *      Returns the expression result
 *
 */

expresion_result compile_cast()
{
    TYPEDEF  type;
    BASETYPE basetype = TYPE_INT;
    int      tokens   = 0;

    int      signed_prefix = 0;
    int      unsigned_prefix = 0;

    expresion_result res;

    token_next();

    /* Check for signed/unsigned prefix */

    if ( token.type == IDENTIFIER )
    {
        if ( token.code == identifier_signed )
        {
            signed_prefix = 1;
            tokens++;
            token_next();
        }
        else if ( token.code == identifier_unsigned )
        {
            unsigned_prefix = 1;
            tokens++;
            token_next();
        }
    }

    /* Create the data type definition */

    if ( token.type == IDENTIFIER )
    {
        if ( token.code == identifier_dword )
        {
            basetype = signed_prefix ? TYPE_INT : TYPE_DWORD;
            signed_prefix = unsigned_prefix = 0;
            tokens++;
            token_next() ;
        }
        else if ( token.code == identifier_word )
        {
            basetype = signed_prefix ? TYPE_SHORT : TYPE_WORD;
            signed_prefix = unsigned_prefix = 0;
            tokens++;
            token_next() ;
        }
        else if ( token.code == identifier_byte )
        {
            basetype = signed_prefix ? TYPE_SBYTE : TYPE_BYTE;
            signed_prefix = unsigned_prefix = 0;
            tokens++;
            token_next() ;
        }
        else if ( token.code == identifier_int )
        {
            basetype = unsigned_prefix ? TYPE_DWORD : TYPE_INT;
            signed_prefix = unsigned_prefix = 0;
            tokens++;
            token_next() ;
        }
        else if ( token.code == identifier_short )
        {
            basetype = unsigned_prefix ? TYPE_WORD : TYPE_SHORT;
            signed_prefix = unsigned_prefix = 0;
            tokens++;
            token_next() ;
        }
        else if ( token.code == identifier_char )
        {
            basetype = TYPE_CHAR;
            tokens++;
            token_next() ;
        }
        else if ( token.code == identifier_float )
        {
            basetype = TYPE_FLOAT ;
            tokens++;
            token_next() ;
        }
        else if ( token.code == identifier_string )
        {
            basetype = TYPE_STRING ;
            tokens++;
            token_next() ;
        }
    }

    /* Don't allow a signed/unsigned prefix in non-integer types */

    if ( signed_prefix || unsigned_prefix ) compile_error( MSG_INVALID_TYPE );

    /* If type is not a basic one: check for user-defined types */

    if ( tokens == 0 )
    {
        TYPEDEF * typep = typedef_by_name( token.code );
        if ( typep == NULL )
        {
/*            type = typedef_new(TYPE_INT); */
            compile_error( MSG_INVALID_TYPE );
        }
        type = *typep;
        token_next();
    }
    else
        type = typedef_new( basetype );

    /* Check for pointers to defined types */

    while ( token.type == IDENTIFIER && ( token.code == identifier_pointer || token.code == identifier_multiply ) ) /* "POINTER" or "*" */
    {
        type = typedef_pointer( type );
/*        tokens++; */
        token_next() ;
    }

    /* Check for the right parent */

    if ( token.type != IDENTIFIER || token.code != identifier_rightp ) compile_error( MSG_EXPECTED, ")" ); /* ")" */

    /* Do the cast */

    res = compile_value();

    if ( typedef_is_equal( res.type, type ) ) return res;

    if ( typedef_is_pointer( type ) )
    {
        /* Conversion of pointer to pointer of another type */

        if ( typedef_is_pointer( res.type ) || typedef_is_integer( res.type ) )
        {
            res.type = type;
            return res;
        }
        else if ( typedef_is_float( res.type ) )
        {
            codeblock_add( code, MN_FLOAT2INT, 0 );
            res.type = type;
            return res;
        }
        compile_error( MSG_PTR_CONVERSION );
    }
    else if ( typedef_is_float( type ) )
    {
        /* Conversion of integer to float */

        if ( typedef_is_integer( res.type ) )
        {
            if ( res.lvalue )
            {
                codeblock_add( code,  mntype( res.type, 0 ) | MN_PTR, 0 );
                res.lvalue = 0;
            }
            codeblock_add( code, MN_INT2FLOAT, 0 );
            res.type = type;
        }
        else
            compile_error( MSG_CONVERSION );
    }
    else if ( typedef_base( type ) == TYPE_CHAR )
    {
        if ( typedef_is_string( res.type ) )
        {
            if ( res.lvalue )
            {
                codeblock_add( code, MN_PTR | MN_STRING, 0 );
                res.lvalue = 0;
            }
            /*
            codeblock_add(code, MN_STR2INT, 0);
            res.type = typedef_new(TYPE_INT);
            */
            codeblock_add( code, MN_STR2CHR, 0 );
            res.type = typedef_new( TYPE_CHAR );
        }
        else if ( typedef_is_array( res.type ) && res.type.chunk[1].type == TYPE_CHAR )
        {
            codeblock_add( code, MN_A2STR, 0 );
            codeblock_add( code, MN_STR2CHR, 0 );
            res.type = typedef_new( TYPE_CHAR );
        }
        else
        {
            compile_error( MSG_CONVERSION );
        }
    }
    else if ( typedef_is_integer( type ) )
    {
        /* Conversion of float, string or integer to integer */

        if ( typedef_is_float( res.type ) )
        {
            if ( res.lvalue )
            {
                codeblock_add( code, MN_PTR | MN_FLOAT, 0 );
                res.lvalue = 0;
            }
            codeblock_add( code, MN_FLOAT2INT, 0 );
            res.type = typedef_new( TYPE_INT );
        }
        else if ( typedef_is_string( res.type ) )
        {
            if ( res.lvalue )
            {
                codeblock_add( code, MN_PTR | MN_STRING, 0 );
                res.lvalue = 0;
            }
            codeblock_add( code, MN_STR2INT, 0 );
            res.type = typedef_new( TYPE_INT );
        }
        else if ( typedef_is_array( res.type ) && res.type.chunk[1].type == TYPE_CHAR )
        {
            codeblock_add( code, MN_A2STR, 0 );
            codeblock_add( code, MN_STR2INT, 0 );
            res.type = typedef_new( TYPE_INT );
        }
        else if ( typedef_is_integer( res.type ) || typedef_is_pointer( res.type ) || typedef_base( res.type ) == TYPE_CHAR )
        {
            if ( res.lvalue )
            {
                codeblock_add( code,  mntype( res.type, 0 ) | MN_PTR, 0 );
                res.lvalue = 0;
            }
        }
        else
        {
            compile_error( MSG_CONVERSION );
        }

        if ( type.chunk[0].type == TYPE_BYTE && typedef_is_integer( res.type ) )
        {
            codeblock_add( code, MN_INT2BYTE, 0 );
            res.type = type;
        }
        else if ( type.chunk[0].type == TYPE_SBYTE && typedef_is_integer( res.type ) )
        {
            codeblock_add( code, MN_INT2BYTE, 0 );
            res.type = type;
        }
        else if ( type.chunk[0].type == TYPE_WORD && typedef_is_integer( res.type ) )
        {
            codeblock_add( code, MN_INT2WORD, 0 );
            res.type = type;
        }
        else if ( type.chunk[0].type == TYPE_SHORT && typedef_is_integer( res.type ) )
        {
            codeblock_add( code, MN_INT2WORD, 0 );
            res.type = type;
        }
    }
    else if ( typedef_is_string( type ) )
    {
        /* Conversión de puntero, float, entero o cadena de ancho fijo a cadena */

        if ( typedef_is_array( res.type ) && res.type.chunk[1].type == TYPE_CHAR )
        {
            codeblock_add( code, MN_A2STR, 0 );
            res.type = typedef_new( TYPE_STRING );
        }
        else if ( typedef_is_pointer( res.type ) )
        {
            if ( res.lvalue )
            {
                codeblock_add( code, mntype( res.type, 0 ) | MN_PTR, 0 );
                res.lvalue = 0;
            }
            codeblock_add( code, MN_A2STR, 0 );
            res.type = typedef_new( TYPE_STRING );
        }
        else if ( res.type.chunk[0].type == TYPE_CHAR )
        {
            if ( res.lvalue )
            {
                codeblock_add( code, mntype( res.type, 0 ) | MN_PTR, 0 );
                res.lvalue = 0;
            }
            codeblock_add( code, MN_CHR2STR, 0 );
            res.type = typedef_new( TYPE_STRING );
        }
        else if ( typedef_is_integer( res.type ) )
        {
            if ( res.lvalue )
            {
                codeblock_add( code, mntype( res.type, 0 ) | MN_PTR, 0 );
                res.lvalue = 0;
            }
            codeblock_add( code, MN_INT2STR | mntype( res.type, 0 ), 0 );
            res.type = typedef_new( TYPE_STRING );
        }
        else if ( typedef_is_float( res.type ) )
        {
            if ( res.lvalue )
            {
                codeblock_add( code, MN_PTR, 0 );
                res.lvalue = 0;
            }
            codeblock_add( code, MN_FLOAT2STR, 0 );
            res.type = typedef_new( TYPE_STRING );
        }
        else
            compile_error( MSG_CONVERSION );
    }
    else if ( typedef_is_struct( type ) )
    {
        res.type = type;
    }
    else
        compile_error( MSG_CONVERSION );

    return res;
}

/* Compila un valor (elemento más pequeño del lenguaje) */

expresion_result compile_value()
{
    CONSTANT * c ;
    SYSPROC * sysproc ;
    PROCDEF * cproc ;
    int param_count, id ;

    expresion_result res ;

    token_next() ;

    /* ( ... ) */

    if ( token.type == IDENTIFIER )
    {

        if ( token.code == identifier_leftp ) /* "(" */
        {
            /* Check for cast-type expressions */

            token_next();
            if ( token.type == IDENTIFIER && identifier_is_type( token.code ) )
            {
                token_back();
                return compile_cast();
            }
            token_back();

            res = compile_subexpresion() ;
            token_next() ;
            if ( token.type != IDENTIFIER || token.code != identifier_rightp ) compile_error( MSG_EXPECTED, ")" ) ; /* ")" */
            return res ;
        }

        if ( token.code == identifier_type ) /* "TYPE" */
        {
            token_next( );
            /* "TYPE mouse" */
            if ( token.type == IDENTIFIER && token.code == identifier_mouse ) /* "MOUSE" - Hack */
            {
                codeblock_add( code, MN_PUSH, -1 ) ;
                res.value      = -1 ;
                res.fvalue     = 0.0 ;
                res.lvalue     = 0 ;
                res.constant   = 1 ;
                res.asignation = 0 ;
                res.call       = 0 ;
                res.type       = typedef_new( TYPE_INT ) ;
                return res ;
            }
            if ( token.type != IDENTIFIER || token.code < reserved_words ) compile_error( MSG_PROCESS_NAME_EXP ) ;

            codeblock_add( code, MN_TYPE, token.code ) ;
            res.fvalue     = 0.0 ;
            res.value      = 0 ;
            res.lvalue     = 0 ;
            res.constant   = 0 ;
            res.asignation = 0 ;
            res.call       = 0 ;
            res.type       = typedef_new( TYPE_INT ) ;
            return res ;
        }

        if ( token.code == identifier_offset || token.code == identifier_bandoffset ) /* "OFFSET" or "&" */
        {
            res = compile_factor() ; /* Para permitir &a.b */
            if ( !res.lvalue ) compile_error( MSG_NOT_AN_LVALUE ) ;

            res.lvalue = 0 ;
            res.type   = typedef_pointer( res.type ) ;
            return res ;
        }

        if ( token.code == identifier_leftb ) /* "[" */  /* POINTER */
        {
            res = compile_subexpresion() ;
            if ( !typedef_is_pointer( res.type ) ) compile_error( MSG_NOT_A_POINTER ) ;

            if ( res.lvalue ) codeblock_add( code, mntype( res.type, 0 ) | MN_PTR, 0 ) ;

            res.type = typedef_reduce( res.type ) ;
            token_next() ;

            if ( token.type != IDENTIFIER || token.code != identifier_rightb ) compile_error( MSG_EXPECTED, "]" ) ; /* "]" */

            res.lvalue = 1 ;
            return res ;
        }

        if ( token.code == identifier_multiply ) /* "*" */
        {
            res = compile_factor() ; /* Para aceptar *ptr++ */
            if ( !typedef_is_pointer( res.type ) ) compile_error( MSG_NOT_A_POINTER ) ;

            if ( res.lvalue ) codeblock_add( code, mntype( res.type, 0 ) | MN_PTR, 0 ) ;

            res.type = typedef_reduce( res.type ) ;
            res.lvalue = 1 ;
            return res ;
        }

        /* SIZEOF */

        if ( token.code == identifier_sizeof ) /* "SIZEOF" */
        {
            token_next() ;
            if ( token.type != IDENTIFIER || token.code != identifier_leftp ) compile_error( MSG_EXPECTED, "(" ) ; /* "(" */

            res.value      = compile_sizeof( 0, NULL, NULL, NULL ) ;
            res.fvalue     = 0.0 ;
            res.lvalue     = 0 ;
            res.constant   = 1 ;
            res.asignation = 0 ;
            res.call       = 0 ;
            res.type       = typedef_new( TYPE_DWORD ) ;
            codeblock_add( code, MN_PUSH, res.value ) ;
            token_next() ;

            if ( token.type != IDENTIFIER || token.code != identifier_rightp ) compile_error( MSG_EXPECTED, ")" ) ; /* ")" */

            return res ;
        }
    }

    switch ( token.type )
    {
        case    NUMBER:         /* Numbers */
            codeblock_add( code, MN_PUSH, token.code ) ;
            res.fvalue     = 0.0 ;
            res.lvalue     = 0 ;
            res.asignation = 0 ;
            res.constant   = 1 ;
            res.call       = 0 ;
            res.value      = token.code ;
            res.type       = typedef_new( TYPE_INT ) ;
            return res ;

        case    FLOAT:
            codeblock_add( code, MN_PUSH, *( int * )&token.value ) ;
            res.value      = 0 ;
            res.lvalue     = 0 ;
            res.asignation = 0 ;
            res.constant   = 1 ;
            res.call       = 0 ;
            res.fvalue     = token.value ;
            res.type       = typedef_new( TYPE_FLOAT ) ;
            return res ;

        case    STRING:         /* Strings */
            codeblock_add( code, MN_PUSH | MN_STRING, token.code ) ;
            res.fvalue     = 0.0 ;
            res.lvalue     = 0 ;
            res.asignation = 0 ;
            res.constant   = 1 ;
            res.call       = 0 ;
            res.value      = token.code ;
            res.type       = typedef_new( TYPE_STRING ) ;
            return res ;

        case    IDENTIFIER:     /* Constants */
            c = constants_search( token.code ) ;
            if ( c )
            {
                if ( typedef_is_string( c->type ) )
                    codeblock_add( code, MN_PUSH | MN_STRING, c->value ) ;
                else
                    codeblock_add( code, MN_PUSH, c->value ) ;
                res.lvalue     = 0 ;
                res.asignation = 0 ;
                res.constant   = 1 ;
                res.call       = 0 ;
                res.value      = c->value ;
                res.fvalue     = *( float * ) & c->value;
                res.type       = c->type ;
                return res ;
            }
            break;

        default:
            compile_error( MSG_UNKNOWN_IDENTIFIER ) ;
            break;
    }

    /* Llamada a un procedimiento o función del sistema */

    id = token.code ;

    token_next() ;

    if ( token.type == IDENTIFIER && token.code == identifier_leftp ) /* "(" */
    {
        SYSPROC ** sysproc_list = sysproc_getall( id ) ;

        if ( sysproc_list )
        {
            sysproc = compile_bestproc( sysproc_list );
            free( sysproc_list );

            token_next() ;
            if ( token.type != IDENTIFIER || token.code != identifier_rightp ) compile_error( MSG_EXPECTED, ")" ) ; /* ")" */

            res.fvalue     = 0.0 ;
            res.lvalue     = 0 ;
            res.asignation = 0 ;
            res.constant   = 0 ;
            res.call       = 1 ;
            res.value      = 0 ;
            res.type       = typedef_new( sysproc->type ) ;
            return res ;
        }

        /* Llama a un procedimiento del usuario */

        cproc = procdef_search( id ) ;
        if ( !cproc )
        {
            if ( autodeclare )
                cproc = procdef_new( procdef_getid(), id ) ;
            else
            {
                token_back() ;
                compile_error( MSG_UNDEFINED_PROC ) ;
            }
        }

        token_next() ;

        if ( token.type != IDENTIFIER || token.code != identifier_rightp ) /* ")" */
        {
            token_back() ;
            param_count = compile_paramlist( cproc->paramtype, 0 ) ;
            token_next() ;
        }
        else
        {
            param_count = 0 ;
        }

        if ( token.type != IDENTIFIER || token.code != identifier_rightp ) compile_error( MSG_EXPECTED, ")" ) ; /* ")" */

        if ( cproc->params == -1 )
        {
            cproc->params = param_count ;
        }
        else if ( cproc->params != param_count )
        {
            compile_error( MSG_INCORRECT_PARAMC, identifier_name( cproc->identifier ), cproc->params ) ;
        }

        codeblock_add( code, MN_CALL, id ) ;
        res.fvalue     = 0.0 ;
        res.lvalue     = 0 ;
        res.asignation = 0 ;
        res.constant   = 0 ;
        res.call       = 1 ;
        res.value      = 0 ;
        res.type       = typedef_new( cproc->type ) ;
        return res ;
    }

    token_back() ;

    /* Valor asignable */

    return compile_sublvalue( 0, 0, NULL ) ;

}

expresion_result compile_factor()
{
    expresion_result res, part ;

    token_next() ;

    res.value       = 0 ;
    res.fvalue      = 0 ;
    res.lvalue      = 0 ;
    res.call        = 0 ;
    res.constant    = 0 ;
    res.asignation  = 0 ;

    /* "+2" (Positivo) */
    if ( token.type == IDENTIFIER && token.code == identifier_plus ) token_next() ; /* "+" */

    if ( token.type == IDENTIFIER )
    {
        if ( token.code == identifier_minus ) /* "-" */ /* "-2" (Negativo) */
        {
            part = compile_factor() ;
            if ( part.lvalue ) codeblock_add( code, mntype( part.type, 0 ) | MN_PTR, 0 ) ;
            codeblock_add( code, ( mntype( part.type, 0 ) == MN_FLOAT ? MN_FLOAT : 0 ) | MN_NEG, 0 ) ;
            res.type = part.type ;
            if ( typedef_is_numeric( part.type ) )
            {
                res.constant = part.constant ;
                res.value    = -part.value ;
                res.fvalue   = -part.fvalue ;
                return res ;
            }
            compile_error( MSG_NUMBER_REQUIRED ) ;
        }
        else if ( token.code == identifier_not ) /* "NOT" or "!" */
        {
            part = compile_factor() ;
            if ( part.lvalue ) codeblock_add( code, mntype( part.type, 0 ) | MN_PTR, 0 ) ;
            if ( typedef_is_pointer( part.type ) )
            {
                /* codeblock_add (code, mntype(part.type, 0) | MN_POINTER2BOL, 0) ; */
                part.type = typedef_new( TYPE_DWORD ) ;
            }
            codeblock_add( code, ( mntype( part.type, 0 ) == MN_FLOAT ? MN_FLOAT : 0 ) | MN_NOT, 0 ) ;
            if ( typedef_is_numeric( part.type ) || typedef_is_pointer( part.type ) )
            {
                res.constant = part.constant ;
                res.value    = !part.value ;
                res.fvalue   = ( float )!part.fvalue ;
                res.type     = part.type ;
                return res ;
            }
            compile_error( MSG_NUMBER_REQUIRED ) ;
            return res ;
        }
        else if ( token.code == identifier_bnot ) /* "BNOT" or "~" */
        {
            part = compile_factor() ;
            if ( part.lvalue ) codeblock_add( code, mntype( part.type, 0 ) | MN_PTR, 0 ) ;
            if ( typedef_is_pointer( part.type ) )
            {
                /* codeblock_add (code, mntype(part.type, 0) | MN_POINTER2BOL, 0) ; */
                part.type = typedef_new( TYPE_DWORD ) ;
            }

            if ( mntype( part.type, 0 ) == MN_FLOAT ) compile_error( MSG_INTEGER_REQUIRED );

            codeblock_add( code, mntype( part.type, 0 ) | MN_BNOT, 0 ) ;
            if ( typedef_is_numeric( part.type ) )
            {
                res.constant = part.constant ;
                res.value    = ~part.value ;
                res.type     = part.type /*typedef_new( TYPE_INT )*/ ;
                return res ;
            }
            compile_error( MSG_NUMBER_REQUIRED ) ;
            return res ;
        }
        else if ( token.type == IDENTIFIER && token.code == identifier_plusplus ) /* "++" */
        {
            part = compile_factor() ;
            if ( !part.lvalue ) compile_error( MSG_VARIABLE_REQUIRED ) ;
            if ( typedef_is_string( part.type ) ) compile_error( MSG_INCOMP_TYPE ) ;
            if ( typedef_is_pointer( part.type ) )
                codeblock_add( code,  MN_INC, typedef_size( typedef_reduce( part.type ) ) ) ;
            else
                codeblock_add( code, mntype( part.type, 0 ) | MN_INC, 1 ) ;
            res.asignation = 1 ;
            res.lvalue = 1 ;
            res.type = part.type ;
            return res ;
        }
        else if ( token.type == IDENTIFIER && token.code == identifier_minusminus ) /* "--" */
        {
            part = compile_factor() ;
            if ( !part.lvalue ) compile_error( MSG_VARIABLE_REQUIRED ) ;
            if ( typedef_is_string( part.type ) ) compile_error( MSG_INCOMP_TYPE ) ;
            if ( typedef_is_pointer( part.type ) )
                codeblock_add( code,  MN_DEC, typedef_size( typedef_reduce( part.type ) ) ) ;
            else
                codeblock_add( code, mntype( part.type, 0 ) | MN_DEC, 1 ) ;
            res.asignation = 1 ;
            res.lvalue = 1 ;
            res.type = part.type ;
            return res ;
        }
    }
    token_back() ;

    part = compile_value() ;

    /* Sufijos (operadores ., [], etc) */

    for ( ;; )
    {
        token_next() ;

        if ( token.type == IDENTIFIER )
        {
            if ( token.code == identifier_point ) /* "." */ /* Operador "." */
            {
                if ( typedef_is_array( part.type ) )
                {
                    while ( typedef_is_array ( part.type = typedef_reduce( part.type ) ) ) ;
                }

                if ( typedef_is_pointer( part.type ) )
                {
                    part.type = typedef_reduce( part.type ) ;
                    if ( !typedef_is_struct( part.type ) ) compile_error( MSG_STRUCT_REQUIRED ) ;
                    codeblock_add( code, MN_PTR, 0 ) ;
                }

                if ( typedef_is_struct( part.type ) )
                {
                    VARSPACE * v = typedef_members( part.type ) ;
                    if ( !v->vars ) compile_error( MSG_STRUCT_REQUIRED ) ;

                    part = compile_sublvalue( v, v->vars[0].offset, NULL ) ;
                }
                else
                {
                    VARSPACE * v = typedef_members( part.type ) ;
                    if ( typedef_base( part.type ) != TYPE_DWORD && typedef_base( part.type ) != TYPE_INT ) compile_error( MSG_INTEGER_REQUIRED ) ;
                    if ( part.lvalue ) codeblock_add( code, mntype( part.type, 0 ) | MN_PTR, 0 ) ;

                    part = compile_sublvalue( &local, 0, v ) ; /* referenciada REMOTA por proceso (Splinter) */
                }
                continue ;
            }
            else if ( token.code == identifier_plusplus ) /* Operador ++ posterior */
            {
                if ( !part.lvalue ) compile_error( MSG_VARIABLE_REQUIRED ) ;
                if ( typedef_is_string( part.type ) ) compile_error( MSG_INCOMP_TYPE ) ;
                if ( typedef_is_pointer( part.type ) )
                    codeblock_add( code,  MN_POSTINC, typedef_size( typedef_reduce( part.type ) ) ) ;
                else
                    codeblock_add( code, mntype( part.type, 0 ) | MN_POSTINC, 1 ) ;
                part.asignation = 1 ;
                part.lvalue = 0 ;
                continue ;
            }
            else if ( token.code == identifier_minusminus ) /* Operador -- posterior */
            {
                if ( !part.lvalue ) compile_error( MSG_VARIABLE_REQUIRED ) ;
                if ( typedef_is_string( part.type ) ) compile_error( MSG_INCOMP_TYPE ) ;
                if ( typedef_is_pointer( part.type ) )
                    codeblock_add( code,  MN_POSTDEC, typedef_size( typedef_reduce( part.type ) ) ) ;
                else
                    codeblock_add( code, mntype( part.type, 0 ) | MN_POSTDEC, 1 ) ;
                part.asignation = 1 ;
                part.lvalue = 0 ;
                continue ;
            }
        }

        /* Indexado vía [...] */

        if ( token.type == IDENTIFIER && token.code == identifier_leftb ) /* "[" */
        {
            if ( typedef_is_pointer( part.type ) ) /* De punteros */
            {
                if ( part.lvalue ) codeblock_add( code, mntype( part.type, 0 ) | MN_PTR, 0 ) ;
                part.type = typedef_reduce( part.type ) ;

                res = compile_subexpresion() ;
                if ( !typedef_is_integer( res.type ) ) compile_error( MSG_INTEGER_REQUIRED ) ;
                if ( res.lvalue ) codeblock_add( code, mntype( res.type, 0 ) | MN_PTR, 0 ) ;

                codeblock_add( code, MN_ARRAY, typedef_size( part.type ) ) ;
                part.lvalue = 1 ;

                token_next() ;

                if ( token.type != IDENTIFIER || token.code != identifier_rightb ) compile_error( MSG_EXPECTED, "]" ) ; /* "]" */
                continue ;
            }

            if ( typedef_is_string( part.type ) ) /* De cadenas */
            {
                if ( part.lvalue ) codeblock_add( code, MN_STRING | MN_PTR, 0 ) ;

                res = compile_subexpresion() ;
                if ( !typedef_is_integer( res.type ) ) compile_error( MSG_INTEGER_REQUIRED ) ;
                if ( res.lvalue ) codeblock_add( code, mntype( res.type, 0 ) | MN_PTR, 0 ) ;

                codeblock_add( code, MN_STRI2CHR, 0 ) ;

                token_next() ;
                if ( token.type != IDENTIFIER || token.code != identifier_rightb ) compile_error( MSG_EXPECTED, "]" ) ; /* "]" */
/*                part.type   = typedef_new( TYPE_STRING ) ; */
                part.type   = typedef_new( TYPE_CHAR ) ;
                part.lvalue = 0 ;
            }
            continue ;
        }

        break ;
    }

    token_back() ;

    return part ;
}

/* Paso 7 */
expresion_result compile_operand()
{
    expresion_result left = compile_factor(), right, res ;
    int op, t ;

    for ( ;; )
    {
        token_next() ;
        if ( token.type == IDENTIFIER && token.code == identifier_multiply ) /* "*" */
        {
            if ( left.lvalue ) codeblock_add( code, mntype( left.type, 0 ) | MN_PTR, 0 ) ;

            right = compile_factor();
            if ( right.lvalue ) codeblock_add( code, mntype( right.type, 0 ) | MN_PTR, 0 ) ;

            t = check_numeric_types( &left, &right ) ;
            codeblock_add( code, MN_MUL | t, 0 ) ;

            res.constant   = ( right.constant && left.constant ) ;

            if ( t == MN_FLOAT )
            {
                res.value  = 0 ;
                res.fvalue = left.fvalue * right.fvalue ;
                res.type   = typedef_new( TYPE_FLOAT ) ;
            }
            else
            {
                res.fvalue = 0.0 ;
                res.type   = typedef_new( TYPE_INT ) ;
                res.value  = left.value * right.value ;
            }

            res.lvalue     = 0 ;
            res.asignation = 0 ;
            res.call       = 0 ;

            left = res ;
            continue ;
        }
        else if ( token.type == IDENTIFIER && ( token.code == identifier_divide || token.code == identifier_mod ) ) /* "/" or "%" */
        {
            op = token.code == identifier_mod ? MN_MOD : MN_DIV ;
            if ( op == MN_MOD && typedef_base( left.type ) == TYPE_FLOAT ) compile_error( MSG_INTEGER_REQUIRED ) ;

            if ( left.lvalue ) codeblock_add( code, mntype( left.type, 0 ) | MN_PTR, 0 ) ;

            right = compile_factor();
            if ( op == MN_MOD && typedef_base( right.type ) == TYPE_FLOAT ) compile_error( MSG_INTEGER_REQUIRED ) ;
            if ( right.lvalue ) codeblock_add( code, mntype( right.type, 0 ) | MN_PTR, 0 ) ;

            t = check_numeric_types( &left, &right ) ;
            codeblock_add( code, op | t, 0 ) ;
            res.constant   = ( right.constant && left.constant ) ;
            res.value      = 0 ;
            res.fvalue     = 0.0 ;
            res.lvalue     = 0 ;
            res.asignation = 0 ;
            res.call       = 0 ;
            res.type       = typedef_new( t == MN_FLOAT ? TYPE_FLOAT : TYPE_INT ) ;

            if ( res.constant )
            {
                if ( t == MN_FLOAT )
                {
                    if ( op == MN_MOD ) compile_error( MSG_NUMBER_REQUIRED ) ;
                    res.fvalue = left.fvalue / right.fvalue ;
                    res.type = typedef_new( TYPE_FLOAT ) ;
                    res.value = 0 ;
                }
                else
                {
                    if ( right.value == 0 ) compile_error( MSG_DIVIDE_BY_ZERO ) ;
                    res.value = op == MN_MOD ? left.value % right.value : left.value / right.value ;
                    res.type = typedef_new( TYPE_INT ) ;
                    res.fvalue = 0.0 ;
                }
            }

            left = res ;
            continue ;
        }

        token_back() ;
        break ;
    }

    return left ;
}

/* Paso 6 */
expresion_result compile_operation()
{
    expresion_result left = compile_operand(), right, res ;
    int op, t ;

    for ( ;; )
    {
        token_next() ;

        /* Suma(o resta) de un entero a un puntero */

        if (
            typedef_is_pointer( left.type ) && token.type == IDENTIFIER &&
            ( token.code == identifier_plus || token.code == identifier_minus ) ) /* "+" o "-" */
        {
            TYPEDEF ptr_t = typedef_reduce( left.type ) ;

            op = token.code == identifier_plus ? MN_ADD : MN_SUB ;
            if ( left.lvalue ) codeblock_add( code, mntype( left.type, 0 ) | MN_PTR, 0 ) ;

            right = compile_operand() ;
            if ( right.lvalue ) codeblock_add( code, mntype( right.type, 0 ) | MN_PTR, 0 ) ;

            if ( !typedef_is_integer( right.type ) && !typedef_is_pointer( right.type )) compile_error( MSG_INCOMP_TYPES ) ;

            if ( typedef_size( ptr_t ) > 1 )
                codeblock_add( code, MN_ARRAY, ( op == MN_ADD ? 1 : -1 ) * typedef_size( ptr_t ) ) ;
            else
                codeblock_add( code, op, 0 ) ;

            res.value      = 0 ;
            res.fvalue     = 0.0 ;
            res.constant   = 0 ;
            res.lvalue     = 0 ;
            res.asignation = 0 ;
            res.call       = 0 ;
            res.type       = left.type ;

            left = res ;
            continue ;
        }

        /* Suma de cadenas */

        if ( typedef_is_array( left.type ) && left.lvalue &&
             token.type == IDENTIFIER && token.code == identifier_plus && left.type.chunk[1].type == TYPE_CHAR ) /* "+" */
        {
            codeblock_add( code, MN_A2STR, 0 ) ;
            left.lvalue = 0 ;
            left.type = typedef_new( TYPE_STRING ) ;
        }

        /* Suma/resta de valores numéricos */

        if ( token.type == IDENTIFIER && ( token.code == identifier_plus || token.code == identifier_minus ) ) /* "+" or "-" */
        {
            op = token.code == identifier_plus ? MN_ADD : MN_SUB ;
            if ( left.lvalue ) codeblock_add( code, mntype( left.type, 0 ) | MN_PTR, 0 ) ;
            right = compile_operand() ;

            /* Concatenación de cadenas */

            if (( typedef_is_string( left.type ) || typedef_is_string( right.type ) ) && op == MN_ADD )
            {
                if ( typedef_is_array( right.type ) && right.lvalue && right.type.chunk[1].type == TYPE_CHAR )
                {
                    codeblock_add( code, MN_A2STR, 0 ) ;
                    right.type = typedef_new( TYPE_STRING ) ;
                    right.lvalue = 0 ;
                }
                if ( right.lvalue ) codeblock_add( code, mntype( right.type, 0 ) | MN_PTR, 0 ) ;

                if ( typedef_is_integer( right.type ) )
                    codeblock_add( code, MN_INT2STR | mntype( right.type, 0 ), 0 ) ;
                else if ( typedef_is_float( right.type ) )
                    codeblock_add( code, MN_FLOAT2STR, 0 ) ;
                else if ( typedef_is_pointer( right.type ) )
                    codeblock_add( code, MN_POINTER2STR, 0 ) ;
                else if ( typedef_base( right.type ) == TYPE_CHAR )
                    codeblock_add( code, MN_CHR2STR, 0 );
                else if ( !typedef_is_string( right.type ) )
                    compile_error( MSG_INCOMP_TYPES ) ;

                if ( typedef_is_integer( left.type ) )
                    codeblock_add( code, MN_INT2STR | mntype( left.type, 0 ), 1 ) ;
                else if ( typedef_is_float( left.type ) )
                    codeblock_add( code, MN_FLOAT2STR, 1 ) ;
                else if ( typedef_is_pointer( left.type ) )
                    codeblock_add( code, MN_POINTER2STR, 1 ) ;
                else if ( typedef_base( left.type ) == TYPE_CHAR )
                    codeblock_add( code, MN_CHR2STR, 1 );
                else if ( !typedef_is_string( left.type ) )
                    compile_error( MSG_INCOMP_TYPES ) ;

                codeblock_add( code, MN_STRING | MN_ADD, 0 ) ;
                res.fvalue     = 0.0 ;
                res.value      = 0 ;
                res.constant   = 0 ;
                res.lvalue     = 0 ;
                res.asignation = 0 ;
                res.call       = 0 ;
                res.type       = typedef_new( TYPE_STRING ) ;

                left = res ;
                continue ;
            }

            if ( right.lvalue ) codeblock_add( code, mntype( right.type, 0 ) | MN_PTR, 0 ) ;

            t = check_numeric_types( &left, &right ) ;
            if ( t != MN_FLOAT ) t = MN_DWORD ;

            codeblock_add( code, op | t, 0 ) ;
            res.constant   = ( right.constant && left.constant ) ;
            res.lvalue     = 0 ;
            res.asignation = 0 ;
            res.call       = 0 ;
            if ( t == MN_FLOAT )
            {
                res.value  = 0.0 ;
                res.type   = typedef_new( TYPE_FLOAT ) ;
                res.fvalue = op == MN_ADD ? left.fvalue + right.fvalue : left.fvalue - right.fvalue ;
            }
            else
            {
                res.fvalue = 0.0 ;
                res.type   = left.type /*typedef_new( TYPE_INT )*/ ;
                res.value  = op == MN_ADD ? left.value + right.value : left.value - right.value ;
            }

            left = res ;
            continue ;
        }
        token_back() ;
        break ;
    }

    return left ;
}

/* Paso 5 */
expresion_result compile_rotation()
{
    expresion_result left = compile_operation(), right, res ;
    int op ;
    BASETYPE t ;

    for ( ;; )
    {
        token_next() ;
        if ( token.type == IDENTIFIER && ( token.code == identifier_ror || token.code == identifier_rol ) ) /* ">>" or "<<" */
        {
            op = token.code == identifier_ror ? MN_ROR : MN_ROL ;
            if ( left.lvalue ) codeblock_add( code, mntype( left.type, 0 ) | MN_PTR, 0 ) ;

            right = compile_operation() ;
            if ( right.lvalue ) codeblock_add( code, mntype( right.type, 0 ) | MN_PTR, 0 ) ;

            t = check_numeric_types( &left, &right ) ;
            codeblock_add( code, op | mntype( left.type, 0 ), 0 ) ;

            res.constant   = ( right.constant && left.constant ) ;

            if ( t == MN_FLOAT ) compile_error( MSG_INTEGER_REQUIRED ) ;

            res.type       = left.type /*typedef_new( TYPE_DWORD ) */;
            res.value      = ( op == MN_ROR ? ( left.value >> right.value ) : ( left.value << right.value ) ) ;
            res.lvalue     = 0 ;
            res.asignation = 0 ;
            res.call       = 0 ;

            left = res ;
            continue ;
        }

        token_back() ;
        break;
    }

    return left ;
}

/* Paso 4 */
expresion_result compile_comparison_1()
{
    expresion_result left = compile_rotation(), right, res ;
    int op, t ;

    for ( ;; )
    {
        token_next() ;
        if (token.type == IDENTIFIER && (
            token.code == identifier_gt ||      /* ">" */
            token.code == identifier_lt ||      /* "<" */
            token.code == identifier_gte ||     /* ">=" or "=>" */
            token.code == identifier_lte ) )    /* "<=" or "=<" */
        {
            int is_unsigned = 0;

            op = token.code  ;
            if ( left.lvalue && ( left.type.chunk[0].type != TYPE_ARRAY || left.type.chunk[1].type != TYPE_CHAR ) )
                codeblock_add( code, mntype( left.type, 0 ) | MN_PTR, 0 ) ;

            right = compile_rotation() ;
            if ( right.lvalue && ( right.type.chunk[0].type != TYPE_ARRAY || right.type.chunk[1].type != TYPE_CHAR ) )
                codeblock_add( code, mntype( right.type, 0 ) | MN_PTR, 0 ) ;

            t = check_numeric_or_string_types( &left, &right ) ;
            if ( t != MN_FLOAT && t != MN_STRING ) t = MN_DWORD ;

            if (
                ( typedef_is_unsigned( left.type ) || typedef_is_unsigned( right.type ) ) &&
                ( typedef_is_integer( left.type ) && typedef_is_integer( right.type ) )
               )
                is_unsigned = MN_UNSIGNED;

            res.value = 0 ;
            if ( op == identifier_gt )   /* ">" */
            {
                codeblock_add( code, t | MN_GT | is_unsigned, 0 ) ;
                if ( left.constant && right.constant )
                {
                    if ( t == MN_DWORD )
                        res.value = left.value > right.value;
                    else if ( t == MN_FLOAT )
                        res.value = ( left.fvalue > right.fvalue );
                    else
                        res.value = strcmp( string_get( left.value ), string_get( right.value ) ) > 0;
                }
            }
            else if ( op == identifier_lt ) /* "<" */
            {
                codeblock_add( code, t | MN_LT | is_unsigned, 0 ) ;
                if ( left.constant && right.constant )
                {
                    if ( t == MN_DWORD )
                        res.value = left.value < right.value;
                    else if ( t == MN_FLOAT )
                        res.value = ( left.fvalue < right.fvalue );
                    else
                        res.value = strcmp( string_get( left.value ), string_get( right.value ) ) < 0;
                }
            }
            else if ( op == identifier_gte )  /* ">=" or "=>" */
            {
                codeblock_add( code, t | MN_GTE | is_unsigned, 0 ) ;
                if ( left.constant && right.constant )
                {
                    if ( t == MN_DWORD )
                        res.value = left.value >= right.value;
                    else if ( t == MN_FLOAT )
                        res.value = ( left.fvalue >= right.fvalue );
                    else
                        res.value = strcmp( string_get( left.value ), string_get( right.value ) ) >= 0;
                }
            }
            else if ( op == identifier_lte )  /* "<=" or "=<" */
            {
                codeblock_add( code, t | MN_LTE | is_unsigned, 0 ) ;
                if ( left.constant && right.constant )
                {
                    if ( t == MN_DWORD )
                        res.value = left.value <= right.value;
                    else if ( t == MN_FLOAT )
                        res.value = ( left.fvalue <= right.fvalue );
                    else
                        res.value = strcmp( string_get( left.value ), string_get( right.value ) ) <= 0;
                }
            }
            res.lvalue     = 0 ;
            res.asignation = 0 ;
            res.call       = 0 ;
            res.constant   = ( right.constant && left.constant ) ;
            res.type       = typedef_new( TYPE_INT ) ;
            left = res;
            continue;
        }
        token_back() ;
        break;
    }

    return left ;
}

expresion_result compile_comparison_2()
{
    expresion_result left = compile_comparison_1(), right, res ;
    int op, t ;

    for ( ;; )
    {
        token_next() ;
        if ( token.type == IDENTIFIER && ( token.code == identifier_eq ||  /* "==" */
                token.code == identifier_ne ) ) /* "!=" or "<>" */
        {
            int is_unsigned = 0;

            op = token.code ;
            if ( left.lvalue && ( left.type.chunk[0].type != TYPE_ARRAY || left.type.chunk[1].type != TYPE_CHAR ) )
                codeblock_add( code, mntype( left.type, 0 ) | MN_PTR, 0 ) ;

            right = compile_comparison_1();
            if ( right.lvalue && ( right.type.chunk[0].type != TYPE_ARRAY || right.type.chunk[1].type != TYPE_CHAR ) )
                codeblock_add( code, mntype( right.type, 0 ) | MN_PTR, 0 ) ;

            t = check_numeric_or_string_types( &left, &right ) ;
            if ( t != MN_FLOAT && t != MN_STRING ) t = MN_DWORD ;

            if ( typedef_is_unsigned( left.type ) && typedef_is_unsigned( right.type ) ) is_unsigned = MN_UNSIGNED;

            res.value = 0 ;

            if ( op == identifier_eq )       /* "==" */
            {
                codeblock_add( code, t | MN_EQ, 0 ) ;
                if ( left.constant && right.constant )
                {
                    if ( t == MN_DWORD )
                        res.value = left.value == right.value;
                    else if ( t == MN_FLOAT )
                        res.value = ( left.fvalue == right.fvalue );
                    else
                        res.value = strcmp( string_get( left.value ), string_get( right.value ) ) == 0;
                }
            }
            else if ( op == identifier_ne )   /* "!=" or "<>" */
            {
                codeblock_add( code, t | MN_NE, 0 ) ;
                if ( left.constant && right.constant )
                {
                    if ( t == MN_DWORD )
                        res.value = left.value != right.value;
                    else if ( t == MN_FLOAT )
                        res.value = ( left.fvalue != right.fvalue );
                    else
                        res.value = strcmp( string_get( left.value ), string_get( right.value ) ) != 0;
                }
            }
            res.lvalue     = 0 ;
            res.asignation = 0 ;
            res.call       = 0 ;
            res.constant   = ( right.constant && left.constant ) ;
            res.type       = typedef_new( TYPE_INT ) ;

            left = res ;
            continue;
        }
        token_back() ;
        break;
    }

    return left ;
}

expresion_result compile_bitwise_and()
{
    expresion_result left = compile_comparison_2(), right, res ;

    for ( ;; )
    {
        token_next() ;
        if ( token.type == IDENTIFIER && ( token.code == identifier_band || token.code == identifier_bandoffset ) ) /* "BAND" or "&" */
        {
            if ( left.lvalue ) codeblock_add( code, mntype( left.type, 0 ) | MN_PTR, 0 ) ;
            right = compile_comparison_2() ;
            if ( right.lvalue ) codeblock_add( code, mntype( right.type, 0 ) | MN_PTR, 0 ) ;
            check_integer_types( &left, &right ) ;

            codeblock_add( code, MN_BAND, 0 ) ;

            res.lvalue     = 0 ;
            res.asignation = 0 ;
            res.call       = 0 ;
            res.constant   = ( right.constant && left.constant ) ;
            res.value      = ( left.value & right.value ) ;
            res.type       = left.type /* typedef_new( TYPE_INT )*/ ;

            left = res ;
            continue ;
        }

        token_back() ;
        break;
    }

    return left ;
}

expresion_result compile_bitwise_xor()
{
    expresion_result left = compile_bitwise_and(), right, res ;

    for ( ;; )
    {
        token_next() ;
        if ( token.type == IDENTIFIER && token.code == identifier_bxor ) /* "BXOR" or "^" */
        {
            if ( left.lvalue ) codeblock_add( code, mntype( left.type, 0 ) | MN_PTR, 0 ) ;
            right = compile_bitwise_and() ;
            if ( right.lvalue ) codeblock_add( code, mntype( right.type, 0 ) | MN_PTR, 0 ) ;
            check_integer_types( &left, &right ) ;

            codeblock_add( code, MN_BXOR, 0 ) ;

            res.lvalue     = 0 ;
            res.asignation = 0 ;
            res.call       = 0 ;
            res.constant   = ( right.constant && left.constant ) ;
            res.value      = ( left.value ^ right.value ) ;
            res.type       = left.type /* typedef_new( TYPE_INT )*/ ;

            left = res ;
            continue ;
        }

        token_back() ;
        break;
    }

    return left ;
}

/* Paso 3 */
expresion_result compile_bitwise_or()
{
    expresion_result left = compile_bitwise_xor(), right, res ;

    for ( ;; )
    {
        token_next() ;
        if ( token.type == IDENTIFIER && token.code == identifier_bor ) /* "BOR" or "|" */
        {
            if ( left.lvalue ) codeblock_add( code, mntype( left.type, 0 ) | MN_PTR, 0 ) ;
            right = compile_bitwise_xor() ;
            if ( right.lvalue ) codeblock_add( code, mntype( right.type, 0 ) | MN_PTR, 0 ) ;
            check_integer_types( &left, &right ) ;

            codeblock_add( code, MN_BOR, 0 ) ;

            res.lvalue     = 0 ;
            res.asignation = 0 ;
            res.call       = 0 ;
            res.constant   = ( right.constant && left.constant ) ;
            res.value      = ( left.value | right.value ) ;
            res.type       = left.type /*typedef_new( TYPE_INT )*/ ;

            left = res ;
            continue ;
        }

        token_back() ;
        break;
    }
    return left ;
}


/* Paso 2 */
expresion_result compile_logical_and()
{
    expresion_result left = compile_bitwise_or(), right, res ;
    int et1;

    for ( ;; )
    {
        token_next() ;
        if ( token.type == IDENTIFIER && token.code == identifier_and ) /* "AND" or "&&" */
        {
            et1 = codeblock_label_add( code, -1 );

            if ( left.lvalue ) codeblock_add( code, MN_PTR | mntype( left.type, 0 ), 0 ) ;
            check_integer_type( &left ) ;
            codeblock_add( code, MN_JTFALSE, et1 ) ;
            right = compile_bitwise_or() ;
            if ( right.lvalue ) codeblock_add( code, MN_PTR | mntype( right.type, 0 ), 0 ) ;
            check_integer_type( &right ) ;

            codeblock_add( code, MN_AND, 0 ) ;

            codeblock_label_set( code, et1, code->current ) ;

            res.lvalue     = 0 ;
            res.asignation = 0 ;
            res.call       = 0 ;
            res.constant   = ( right.constant && left.constant ) ;
            res.value      = ( left.value && right.value ) ;
            res.type       = typedef_new( TYPE_INT ) ;

            left = res ;
            continue ;
        }

        token_back() ;
        break;
    }

    return left ;
}

/* Paso 2 */
expresion_result compile_logical_xor()
{
    expresion_result left = compile_logical_and(), right, res ;

    for ( ;; )
    {
        token_next() ;
        if ( token.type == IDENTIFIER && token.code == identifier_xor ) /* "XOR" or "^^" */
        {
            if ( left.lvalue ) codeblock_add( code, mntype( left.type, 0 ) | MN_PTR, 0 ) ;
            right = compile_logical_and() ;
            if ( right.lvalue ) codeblock_add( code, mntype( right.type, 0 ) | MN_PTR, 0 ) ;
            check_integer_types( &left, &right ) ;

            codeblock_add( code, MN_XOR, 0 ) ;

            res.lvalue     = 0 ;
            res.asignation = 0 ;
            res.call       = 0 ;
            res.constant   = ( right.constant && left.constant ) ;
            res.value      = (( left.value != 0 ) ^( right.value != 0 ) ) ;
            res.type       = typedef_new( TYPE_INT ) ;

            left = res ;
            continue ;
        }

        token_back() ;
        break;
    }

    return left ;
}

/* Paso 2 */
expresion_result compile_logical_or()
{
    expresion_result left = compile_logical_xor(), right, res ;
    int et1;
    int et2;

    for ( ;; )
    {
        token_next() ;
        if ( token.type == IDENTIFIER && token.code == identifier_or ) /* "OR" or "||" */
        {
            et1 = codeblock_label_add( code, -1 );
            et2 = codeblock_label_add( code, -1 );

            if ( left.lvalue ) codeblock_add( code,  mntype( left.type, 0 ) | MN_PTR, 0 ) ;
            check_integer_type( &left ) ;
            codeblock_add( code, MN_JTFALSE, et1 ) ;

            codeblock_add( code, MN_POP, 0 );
            codeblock_add( code, MN_PUSH, 1 );
            codeblock_add( code, MN_JUMP, et2 );

            codeblock_label_set( code, et1, code->current ) ;

            right = compile_logical_xor() ;
            if ( right.lvalue ) codeblock_add( code, mntype( right.type, 0 ) | MN_PTR, 0 ) ;
            check_integer_type( &right ) ;

            codeblock_add( code, MN_OR, 0 ) ;

            codeblock_label_set( code, et2, code->current ) ;

            res.lvalue     = 0 ;
            res.asignation = 0 ;
            res.call       = 0 ;
            res.constant   = ( right.constant && left.constant ) ;
            res.value      = ( left.value || right.value ) ;
            res.type       = typedef_new( TYPE_INT ) ;

            left = res ;
            continue;
        }

        token_back() ;
        break ;
    }
    return left ;
}

/* Paso 1 */
expresion_result compile_ternarycond()
{
    CODEBLOCK_POS pos ;

    if ( code ) pos = codeblock_pos( code );

    expresion_result base = compile_logical_or(), right, res ;
    int et1, et2 ;

    token_next() ;
    /* Operador EXPR ? TRUE : FALSE */
    if ( token.type == IDENTIFIER && token.code == identifier_question )  /* "?" */
    {
        if ( base.lvalue ) codeblock_add( code, mntype( base.type, 0 ) | MN_PTR, 0 ) ;

        if ( base.constant )
        {
            codeblock_setpos( code, pos );

            if (
                ( typedef_is_integer( base.type ) && base.value ) ||
                ( typedef_is_float( base.type ) && base.fvalue )
            )
            {
                right = compile_expresion( 0, 0, 0, TYPE_UNDEFINED );
                if ( code ) pos = codeblock_pos( code );
                token_next();
                if ( token.type != IDENTIFIER || token.code != identifier_colon ) compile_error( MSG_EXPECTED, ":" ); /* ":" */
                res = compile_expresion( 0, 0, 0, right.type.chunk[0].type );
                codeblock_setpos( code, pos );

            }
            else
            {
                if ( code ) pos = codeblock_pos( code );
                right = compile_expresion( 0, 0, 0, TYPE_UNDEFINED );
                codeblock_setpos( code, pos );
                token_next();
                if ( token.type != IDENTIFIER || token.code != identifier_colon ) compile_error( MSG_EXPECTED, ":" ); /* ":" */
                res = compile_expresion( 0, 0, 0, right.type.chunk[0].type );
            }
        }
        else
        {
            et1 = codeblock_label_add( code, -1 );
            et2 = codeblock_label_add( code, -1 );
            codeblock_add( code, MN_JFALSE, et1 );

            right = compile_expresion( 0, 0, 0, TYPE_UNDEFINED );
            codeblock_add( code, MN_JUMP, et2 );

            codeblock_label_set( code, et1, code->current );
            token_next();

            if ( token.type != IDENTIFIER || token.code != identifier_colon ) compile_error( MSG_EXPECTED, ":" ); /* ":" */

            res = compile_expresion( 0, 0, 0, right.type.chunk[0].type );
            codeblock_label_set( code, et2, code->current );
        }

        if ( base.constant && res.constant && right.constant )
        {
            if ( typedef_is_integer( base.type ) )
            {
                return base.value ? right : res;
            }
            else if ( typedef_is_float( base.type ) )
            {
                return base.fvalue ? right : res;
            }
        }

        res.constant = 0;
        res.lvalue = ( right.lvalue && res.lvalue );

        return res;
    }

    token_back() ;
    return base;
}


/* Paso 1 */
expresion_result compile_subexpresion()
{
    expresion_result base = compile_ternarycond(), right, res ;
    int op, type ;

    token_next() ;
    if ( token.type == IDENTIFIER )
    {
        /* Asignaciones */
        if ( token.code == identifier_equal ) /* "=" */
        {
            if ( typedef_is_array( base.type ) && base.lvalue && base.type.chunk[1].type == TYPE_CHAR ) /* Asignaciones a cadenas de ancho fijo */
            {
                right = compile_expresion( 0, 0, 0, TYPE_UNDEFINED ) ;
                if ( typedef_is_integer( right.type ) )
                {
                    compile_warning( 1, "implicit conversion (INTEGER to CHAR[])" );
                    codeblock_add( code, MN_INT2STR | mntype( right.type, 0 ), 0 ) ;
                }
                else if ( typedef_is_float( right.type ) )
                {
                    compile_warning( 1, "implicit conversion (FLOAT to CHAR[])" );
                    codeblock_add( code, MN_FLOAT2STR, 0 ) ;
                }
                else if ( !typedef_is_string( right.type ) )
                {
                    compile_error( MSG_INCOMP_TYPE ) ;
                }
                codeblock_add( code, MN_STR2A, base.type.chunk[0].count - 1 ) ;
                right.asignation = 1 ;

                return right ;
            }

            if ( typedef_is_pointer( base.type ) ) /* Asignaciones a punteros */
            {
                TYPEDEF pointer_type ;

                pointer_type = typedef_reduce( base.type ) ;
                if ( !base.lvalue ) compile_error( MSG_VARIABLE_REQUIRED ) ;

                right = compile_expresion( 0, 0, 0, TYPE_UNDEFINED ) ;

                if (( typedef_base( right.type ) == TYPE_DWORD || typedef_base( right.type ) == TYPE_INT ) && right.constant && right.value == 0 )
                {
                    right.type = base.type ;
                }

                if ( typedef_base( right.type ) == TYPE_FLOAT )
                {
                    compile_warning( 1, "implicit conversion (FLOAT to POINTER)" );
                    codeblock_add( code, MN_FLOAT2INT, 0 );
                    right.type = base.type;
                }

                /* Un puntero "void" puede asignarse a otro cualquiera */
                right.type = typedef_pointer( pointer_type ) ;
                if ( typedef_base( typedef_reduce( right.type ) ) != typedef_base( pointer_type ) ) compile_error( MSG_TYPES_NOT_THE_SAME ) ;

                codeblock_add( code, MN_DWORD | MN_LET, 0 ) ;

                res.lvalue     = 1 ;
                res.asignation = 1 ;
                res.call       = 0 ;
                res.constant   = 0 ;
                res.value      = 0 ;
                res.type       = base.type ;

                return res ;
            }

            if ( typedef_base( base.type ) == TYPE_CHAR ) /* Asignaciones a chars */
            {
                if ( !base.lvalue ) compile_error( MSG_VARIABLE_REQUIRED ) ;

                right = compile_expresion( 0, 0, 0, TYPE_UNDEFINED ) ;

                if ( typedef_is_string( right.type ) )
                {
                    compile_warning( 1, "implicit conversion (STRING to CHAR)" );
                    codeblock_add( code, MN_STR2CHR, 0 ) ;
                }
                else if ( typedef_base( right.type ) == TYPE_FLOAT )
                {
                    compile_warning( 1, "implicit conversion (FLOAT to CHAR)" );
                    codeblock_add( code, MN_FLOAT2INT, 0 ) ;
                }
                else if ( !typedef_is_numeric( right.type ) )
                {
                    compile_error( MSG_INCOMP_TYPE ) ;
                }

                codeblock_add( code, MN_LET | MN_BYTE, 0 ) ;

                res.lvalue     = 1 ;
                res.asignation = 1 ;
                res.call       = 0 ;
                res.constant   = 0 ;
                res.value      = 0 ;
                res.type       = typedef_new( TYPE_CHAR ) ;

                return res ;
            }

            if ( typedef_is_string( base.type ) ) /* Asignaciones a cadenas */
            {
                if ( !base.lvalue ) compile_error( MSG_VARIABLE_REQUIRED ) ;

                right = compile_expresion( 0, 0, 0, TYPE_UNDEFINED ) ;

                if ( typedef_base( right.type ) == TYPE_CHAR )
                {
                    compile_warning( 1, "implicit conversion (CHAR to STRING)" );
                    codeblock_add( code, MN_CHR2STR, 0 ) ;
                }
                else if ( typedef_is_integer( right.type ) )
                {
                    compile_warning( 1, "implicit conversion (INTEGER to STRING)" );
                    codeblock_add( code, MN_INT2STR | mntype( right.type, 0 ), 0 ) ;
                }
                else if ( typedef_is_float( right.type ) )
                {
                    compile_warning( 1, "implicit conversion (FLOAT to STRING)" );
                    codeblock_add( code, MN_FLOAT2STR, 0 ) ;
                }
                else if ( typedef_is_pointer( right.type ) )
                {
                    compile_warning( 1, "implicit conversion (POINTER to STRING)" );
                    codeblock_add( code, MN_POINTER2STR, 0 ) ;
                }
                else if ( !typedef_is_string( right.type ) )
                {
                    compile_error( MSG_INCOMP_TYPE ) ;
                }
                codeblock_add( code, MN_STRING | MN_LET, 0 ) ;

                res.lvalue     = 1 ;
                res.asignation = 1 ;
                res.call       = 0 ;
                res.constant   = 0 ;
                res.value      = 0 ;
                res.type       = typedef_new( TYPE_STRING ) ;

                return res ;
            }
        }

        /* Puntero += entero */

        if ( typedef_is_pointer( base.type ) && ( token.code == identifier_plusequal || token.code == identifier_minusequal ) ) /* "+=" or "-=" */
        {
            TYPEDEF pointer_type ;

            op = ( token.code == identifier_plusequal ? MN_VARADD : MN_VARSUB ) ;
            pointer_type = typedef_reduce( base.type ) ;
            if ( !base.lvalue ) compile_error( MSG_VARIABLE_REQUIRED ) ;

            right = compile_expresion( 0, 0, 0, TYPE_UNDEFINED ) ;

            if ( !typedef_is_integer( right.type ) ) compile_error( MSG_INCOMP_TYPES ) ;

            if ( typedef_size( pointer_type ) > 1 )
                codeblock_add( code, MN_ARRAY, ( op == MN_VARADD ? 1 : -1 ) * typedef_size( pointer_type ) ) ;
            else
                codeblock_add( code, op, 0 ) ;

            res.lvalue     = 1 ;
            res.asignation = 1 ;
            res.call       = 0 ;
            res.constant   = 0 ;
            res.value      = 0 ;
            res.type       = typedef_new( TYPE_STRING ) ;

            return res ;
        }

        /* Increment and assign */

        if ( token.code == identifier_plusequal ) /* "+=" */
        {
            if ( typedef_is_array( base.type ) && base.lvalue && base.type.chunk[1].type == TYPE_CHAR ) /* Cadena += cadena fija */
            {
                right = compile_expresion( 0, 0, 0, TYPE_UNDEFINED ) ;
                if ( typedef_is_integer( right.type ) )
                {
                    compile_warning( 1, "implicit conversion (INTEGER to CHAR[])" );
                    codeblock_add( code, MN_INT2STR | mntype( right.type, 0 ), 0 ) ;
                }
                else if ( typedef_is_float( right.type ) )
                {
                    compile_warning( 1, "implicit conversion (FLOAT to CHAR[])" );
                    codeblock_add( code, MN_FLOAT2STR, 0 ) ;
                }
                else if ( typedef_base( right.type ) == TYPE_CHAR )
                {
                    compile_warning( 1, "implicit conversion (CHAR to CHAR[])" );
                    codeblock_add( code, MN_CHR2STR, 0 );
                }
                else if ( !typedef_is_string( right.type ) )
                {
                    compile_error( MSG_INCOMP_TYPE ) ;
                }
                codeblock_add( code, MN_STRACAT, base.type.chunk[0].count ) ;
                right.asignation = 1 ;

                return right ;
            }

            if ( typedef_is_string( base.type ) )
            {
                if ( !base.lvalue ) compile_error( MSG_VARIABLE_REQUIRED ) ;
                right = compile_expresion( 0, 0, 0, TYPE_UNDEFINED ) ;
                if ( typedef_is_integer( right.type ) )
                {
                    compile_warning( 1, "implicit conversion (INTEGER to STRING)" );
                    codeblock_add( code, MN_INT2STR | mntype( right.type, 0 ), 0 ) ;
                }
                else if ( typedef_is_float( right.type ) )
                {
                    compile_warning( 1, "implicit conversion (FLOAT to STRING)" );
                    codeblock_add( code, MN_FLOAT2STR, 0 ) ;
                }
                else if ( typedef_base( right.type ) == TYPE_CHAR )
                {
                    compile_warning( 1, "implicit conversion (CHAR to STRING)" );
                    codeblock_add( code, MN_CHR2STR, 0 );
                }
                else if ( !typedef_is_string( right.type ) )
                {
                    compile_error( MSG_INCOMP_TYPE ) ;
                }
                codeblock_add( code, MN_STRING | MN_VARADD, 0 ) ;

                res.lvalue     = 1 ;
                res.asignation = 1 ;
                res.call       = 0 ;
                res.constant   = 0 ;
                res.value      = 0 ;
                res.type       = typedef_new( TYPE_STRING ) ;

                return res ;
            }
        }

        /* Otra posible combinación(for not string/char[]/pointers) */

        if (    token.code == identifier_plusequal      /* "+=" */
            ||  token.code == identifier_minusequal     /* "-=" */
            ||  token.code == identifier_multequal      /* "*=" */
            ||  token.code == identifier_divequal       /* "/=" */
            ||  token.code == identifier_modequal       /* "%=" */
            ||  token.code == identifier_andequal       /* "&=" */
            ||  token.code == identifier_xorequal       /* "^=" */
            ||  token.code == identifier_orequal        /* "|=" */
            ||  token.code == identifier_rolequal       /* "<<=" */
            ||  token.code == identifier_rorequal       /* ">>=" */
            ||  token.code == identifier_equal )        /* "=" */
        {
            SYSPROC * proc_copy = sysproc_get( identifier_search_or_add( "#COPY#" ) );
            SYSPROC * proc_memcopy = sysproc_get( identifier_search_or_add( "#MEMCOPY#" ) );
            SYSPROC * proc_copy_string_array = sysproc_get( identifier_search_or_add( "#COPYSTRA#" ) );
            int size, nvar;

            op = token.code ;

            /* Assignation to struct: struct copy */
            if ( typedef_is_struct( base.type ) )
            {
                if ( token.code != identifier_equal ) compile_error( MSG_EXPECTED, "=" ) ; /* "=" */

                right = compile_expresion( 0, 0, 0, TYPE_UNDEFINED );
                while ( typedef_is_array( right.type ) ) right.type = typedef_reduce( right.type );

                if ( typedef_base( right.type ) != TYPE_POINTER ) compile_error( MSG_STRUCT_REQUIRED );

                if ( !typedef_is_struct( typedef_reduce( right.type ) ) )
                {
                    compile_error( MSG_STRUCT_REQUIRED );
                }
                else if ( right.type.varspace != base.type.varspace )
                {
                    compile_error( MSG_TYPES_NOT_THE_SAME );
                }
                else
                {
                    /*
                     *  Struct copy operator
                     */

                    while ( typedef_is_pointer( base.type ) )
                    {
                        codeblock_add( code, MN_PTR, 0 );
                        res.type = typedef_reduce( base.type );
                    }

                    if ( typedef_base( base.type ) != TYPE_STRUCT )
                        compile_error( MSG_STRUCT_REQUIRED );
                    else
                    {
                        size = right.type.varspace->count * sizeof( DCB_TYPEDEF );

                        if ( right.type.varspace->stringvar_count > 0 )
                        {
                            /* True struct copy version */

                            segment_alloc( globaldata, size );
                            codeblock_add( code, MN_GLOBAL, globaldata->current ) ;
                            for ( nvar = 0 ; nvar < right.type.varspace->count ; nvar++ )
                            {
                                DCB_TYPEDEF type;
                                dcb_settype( &type, &right.type.varspace->vars[nvar].type );
                                memcpy(( uint8_t* )globaldata->bytes + globaldata->current, &type, sizeof( DCB_TYPEDEF ) );
                                globaldata->current += sizeof( DCB_TYPEDEF );
                            }
                            codeblock_add( code, MN_PUSH, right.type.varspace->count );
                            codeblock_add( code, MN_PUSH, right.count ? right.count : 1 );
                            codeblock_add( code, MN_SYSCALL, proc_copy->code );
                        }
                        else
                        {
                            /* Optimized fast memcopy version */
                            codeblock_add( code, MN_PUSH, right.type.varspace->size*( right.count ? right.count : 1 ) );
                            codeblock_add( code, MN_SYSCALL, proc_memcopy->code );
                        }
                    }
                    base.type = right.type;
                    base.constant = 0;
                    base.lvalue = 0;
                    base.call = 1;
                }
                return base;
            }

            if ( op != identifier_equal && typedef_is_array( base.type ) ) compile_error( MSG_EXPECTED, "[" ) ;

            if ( !base.lvalue ) compile_error( MSG_VARIABLE_REQUIRED ) ;
            right = compile_expresion( 0, 0, 0, typedef_base( base.type ) ) ;

            /* Array copy */
            if ( op == identifier_equal && typedef_is_array( base.type ) && typedef_is_array( right.type ) )
            {
                int size;

                if ( !typedef_is_equal( base.type, right.type ) ) compile_error( MSG_TYPES_NOT_THE_SAME );

                size = typedef_size( base.type );

                while ( typedef_is_array( base.type ) )
                {
                    base.type = typedef_reduce( base.type ) ;
                    right.type = typedef_reduce( right.type ) ;
                }

                /* Optimized fast memcopy version */
                if ( typedef_is_string( base.type ) )
                {
                    codeblock_add( code, MN_PUSH, size / sizeof( int ) );
                    codeblock_add( code, MN_SYSCALL, proc_copy_string_array->code );
                }
                else
                {
                    codeblock_add( code, MN_PUSH, size );
                    codeblock_add( code, MN_SYSCALL, proc_memcopy->code );
                }

                base.type = right.type;
                base.constant = 0;
                base.lvalue = 0;
                base.call = 1;

                return base;
            }

            if ( right.lvalue ) codeblock_add( code, mntype( right.type, 0 ) | MN_PTR, 0 ) ;

            type = check_numeric_types( &base, &right ) ;

            if ( op == identifier_plusequal )               /* "+=" */
                codeblock_add( code, type | MN_VARADD, 0 ) ;
            else if ( op == identifier_minusequal )         /* "-=" */
                codeblock_add( code, type | MN_VARSUB, 0 ) ;
            else if ( op == identifier_multequal )          /* "*=" */
                codeblock_add( code, type | MN_VARMUL, 0 ) ;
            else if ( op == identifier_divequal )           /* "/=" */
                codeblock_add( code, type | MN_VARDIV, 0 ) ;
            else if ( op == identifier_modequal )           /* "%=" */
                codeblock_add( code, type | MN_VARMOD, 0 ) ;
            else if ( op == identifier_orequal )            /* "|=" */
                codeblock_add( code, type | MN_VAROR, 0 ) ;
            else if ( op == identifier_andequal )           /* "&=" */
                codeblock_add( code, type | MN_VARAND, 0 ) ;
            else if ( op == identifier_xorequal )           /* "^=" */
                codeblock_add( code, type | MN_VARXOR, 0 ) ;
            else if ( op == identifier_rorequal )           /* ">>=" */
                codeblock_add( code, type | MN_VARROR, 0 ) ;
            else if ( op == identifier_rolequal )           /* "<<=" */
                codeblock_add( code, type | MN_VARROL, 0 ) ;
            else if ( op == identifier_equal )              /* "=" */
                codeblock_add( code, type | MN_LET, 0 ) ;

            res.lvalue     = 1 ;
            res.asignation = 1 ;
            res.call       = 0 ;
            res.constant   = 0 ;
            res.value      = 0 ;
            res.type       = right.type ;

            return res ;
        }
        else
        {
            token_back() ;
        }
    }
    else
    {
        token_back() ;
    }

    return base ;
}

expresion_result compile_expresion( int need_constant, int need_lvalue, int discart_code, BASETYPE t )
{
    expresion_result res ;

    CODEBLOCK_POS pos ;

    if ( code ) pos = codeblock_pos( code );

    res = compile_subexpresion() ;

    /* Interpreta una estructura tal cual como un puntero a la misma */

    if ( res.lvalue && typedef_base( res.type ) == TYPE_STRUCT && !need_lvalue )
    {
        res.type = typedef_pointer( res.type ) ;
        res.lvalue = 0 ;
        res.constant = 0 ;
    }

    /* Interpretar arrays de byte como cadenas */

    if ( typedef_base( res.type ) == TYPE_ARRAY && res.type.chunk[1].type == TYPE_CHAR && res.lvalue && !need_lvalue )
    {
        codeblock_add( code, MN_A2STR, 0 ) ;
        res.type = typedef_new( TYPE_STRING ) ; /* Array 2 String */
        res.lvalue = 0 ;
    }

    /* Quita los lvalue */

    if ( !need_lvalue && res.lvalue && !typedef_is_array( res.type ) )
    {
        res.lvalue = 0 ;
        codeblock_add( code, mntype( res.type, 0 ) | MN_PTR, 0 ) ;
    }

    /* Conversiones de tipo */

    if ( t != TYPE_UNDEFINED ) res = convert_result_type( res, t ) ;

    /* Optimización de datos constantes */

    if ( res.constant )
    {
        if ( code ) codeblock_setpos( code, pos );
        if ( typedef_base( res.type ) == TYPE_FLOAT )
            codeblock_add( code, MN_PUSH, *( int * )&res.fvalue ) ;
        else if ( typedef_base( res.type ) == TYPE_STRING )
            codeblock_add( code, MN_PUSH | MN_STRING, res.value ) ;
        else
            codeblock_add( code, MN_PUSH, res.value ) ;
    }

    if ( need_lvalue && !res.lvalue ) compile_error( MSG_VARIABLE_REQUIRED ) ;

    if ( need_constant && !res.constant ) compile_error( MSG_CONSTANT_EXP ) ;

    if ( discart_code && code ) codeblock_setpos( code, pos );

    return res ;
}

/*
 *  FUNCTION : convert_result_type
 *
 *  Given an expresion result in the current context, convert it
 *  if possible to the basic type given(and emit the necessary code)
 *
 *  PARAMS:
 *      res             Result of expression at current context
 *      t               Basic type required
 *
 *  RETURN VALUE:
 *      The converted type result
 */

expresion_result convert_result_type( expresion_result res, BASETYPE t )
{
    /* Conversiones de tipo */

    if ( t < 9 && typedef_is_integer( res.type ) )
    {
        res.type = typedef_new( t );
    }
    else if ( typedef_base( res.type ) == TYPE_POINTER && t == TYPE_STRING )
    {
        codeblock_add( code, MN_POINTER2STR, 0 ) ;
        res.type = typedef_new( t ) ; /* Pointer 2 String */
    }
    else if ( typedef_is_integer( res.type ) /*&& res.constant && res.value == 0*/ && t == TYPE_POINTER )
    {
        res.type = typedef_new( t ) ; /* pointer */
    }
    else if ( typedef_base( res.type ) == TYPE_POINTER && t < 8 )
    {
        res.type = typedef_new( t ) ; /* Pointer 2 Int */
    }
    else if ( typedef_base( res.type ) == TYPE_FLOAT && t < 8 )
    {
        codeblock_add( code, MN_FLOAT2INT, 0 ) ;

        res.type = typedef_new( t ) ;
        res.value = ( int )res.fvalue ;
    }
    else if ( t == TYPE_FLOAT && typedef_is_integer( res.type ) )
    {
        codeblock_add( code, MN_INT2FLOAT, 0 ) ;
        res.type = typedef_new( TYPE_FLOAT ) ;
        res.fvalue = ( float )res.value ;
    }
    else if (( t == TYPE_BYTE || t == TYPE_WORD || t == TYPE_DWORD ) && typedef_is_integer( res.type ) )
    {
        res.type = typedef_new( t ) ;
    }
    else if ( t == TYPE_STRING && typedef_is_integer( res.type ) )
    {
        codeblock_add( code, MN_INT2STR | mntype( res.type, 0 ), 0 ) ;
        if ( res.constant )
        {
            char buffer[32] ;
            switch ( res.type.chunk[0].type )
            {
                case TYPE_INT:
                    sprintf( buffer, "%d", res.value ) ;
                    break;

                case TYPE_WORD:
                    sprintf( buffer, "%d", ( uint16_t )res.value ) ;
                    break;

                case TYPE_BYTE:
                    sprintf( buffer, "%d", ( uint8_t )res.value ) ;
                    break;

                case TYPE_SHORT:
                    sprintf( buffer, "%d", ( int16_t )res.value ) ;
                    break;

                case TYPE_SBYTE:
                    sprintf( buffer, "%d", ( int8_t )res.value ) ;
                    break;

                case TYPE_DWORD:
                    sprintf( buffer, "%u", ( int32_t )res.value ) ;
                    break;

                case TYPE_UNDEFINED :
                case TYPE_CHAR      :
                case TYPE_FLOAT     :
                case TYPE_STRING    :
                case TYPE_ARRAY     :
                case TYPE_STRUCT    :
                case TYPE_POINTER   :
                    break;
            }
            res.value = string_new( buffer ) ;
        }
        res.type = typedef_new( t ) ;
    }
    else if ( t == TYPE_STRING && typedef_base( res.type ) == TYPE_CHAR )
    {
        codeblock_add( code, MN_CHR2STR, 0 ) ;
        if ( res.constant )
        {
            char buffer[2] ;
            buffer[0] = res.value;
            buffer[1] = 0;
            res.value = string_new( buffer ) ;
        }
        res.type = typedef_new( t ) ;
    }
    else if ( t != TYPE_UNDEFINED && typedef_base( res.type ) != t )
    {
        switch ( t )
        {
            case TYPE_CHAR:

                /* Allow string-to-char conversions */

                if ( typedef_is_string( res.type ) )
                {
                    codeblock_add( code, MN_STR2CHR, 0 ) ;
                    if ( res.constant == 1 ) res.value = ( unsigned char ) * ( string_get( res.value ) );
                }
                else
                    compile_error( MSG_INTEGER_REQUIRED ) ;
                break ;

            case TYPE_DWORD:
            case TYPE_INT:
            case TYPE_WORD:
            case TYPE_SHORT:
            case TYPE_BYTE:
            case TYPE_SBYTE:
                if ( typedef_is_array( res.type ) && res.lvalue &&
                        res.type.chunk[1].type == TYPE_CHAR )
                {
                    codeblock_add( code, MN_A2STR, 0 ) ;
                    codeblock_add( code, MN_STR2INT, 0 ) ;
                    res.lvalue = 0 ;
                    res.constant = 0;
                }
                else if ( typedef_is_string( res.type ) )
                {
                    codeblock_add( code, MN_STR2INT, 0 ) ;
                    if ( res.constant == 1 ) res.value = atoi( string_get( res.value ) );
                }
                else if ( typedef_base( res.type ) == TYPE_CHAR )
                {
                    res.type = typedef_new( t );
                }
                else
                    compile_error( MSG_INTEGER_REQUIRED ) ;
                break ;

            case TYPE_FLOAT:
                if ( typedef_is_string( res.type ) )
                {
                    codeblock_add( code, MN_STR2FLOAT, 0 ) ;
                    if ( res.constant == 1 ) res.fvalue = ( float )atof( string_get( res.value ) );
                }
                else
                    compile_error( MSG_NUMBER_REQUIRED ) ;
                break ;

            case TYPE_STRING:
                if ( typedef_is_array( res.type ) && res.lvalue &&
                        res.type.chunk[1].type == TYPE_CHAR )
                {
                    codeblock_add( code, MN_A2STR, 0 ) ;
                    res.lvalue = 0 ;
                }
                else if ( typedef_is_integer( res.type ) )
                {
                    codeblock_add( code, MN_INT2STR | mntype( res.type, 0 ), 0 ) ;
                    if ( res.constant )
                    {
                        char buffer[32] ;
                        sprintf( buffer, "%d", res.value ) ;
                        res.value = string_new( buffer ) ;
                    }
                }
                else if ( typedef_is_float( res.type ) )
                {
                    codeblock_add( code, MN_FLOAT2STR, 0 ) ;
                    if ( res.constant )
                    {
                        char buffer[32] ;
                        sprintf( buffer, "%g", res.fvalue ) ;
                        res.value = string_new( buffer ) ;
                    }
                }
                else
                    compile_error( MSG_STRING_EXP ) ;
                break ;

            default:
                compile_error( MSG_INCOMP_TYPE ) ;
        }
        res.type = typedef_new( t ) ;
    }

    return res ;
}

int compile_sentence_end()
{
    token_next() ;

    if ( token.type == NOTOKEN ) return 1;

    if ( token.type == IDENTIFIER && token.code == identifier_semicolon ) return 0 ; /* ";" */

    /*
    if (token.type == IDENTIFIER && token.code == identifier_end)
    {
        token_back() ;
        return 1 ;
    }
    */

    compile_error( MSG_EXPECTED, ";" ) ;
    return 0;
}

extern int dcb_options ;

void basetype_describe( char * buffer, BASETYPE t )
{
    switch ( t )
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
            sprintf( buffer, "STRUCT" ) ;

        case TYPE_ARRAY:
            sprintf( buffer, "ARRAY" ) ;
            return ;

        case TYPE_POINTER:
            sprintf( buffer, "POINTER" ) ;
            return ;

        case TYPE_UNDEFINED:
        default:
            sprintf( buffer, "<UNDEFINED>" ) ;
            return ;
    }
}

void compile_block( PROCDEF * p )
{
    int loop, last_loop, et1, et2 ;
    expresion_result res, from, to ;
    int codelabel;

    proc = p ;
    code = &p->code ;

    for ( ;; )
    {
        token_next() ;
        if ( token.type == NOTOKEN ) break ;

        if ( token.type == IDENTIFIER )
        {
            if ( token.code == identifier_end    || /* "END" */
                 token.code == identifier_until  ||  /* "UNTIL" */
                 token.code == identifier_else   ||  /* "ELSE" */
                 token.code == identifier_elseif )   /* "ELSEIF" */
                break ;

            if ( token.code == identifier_semicolon ) /* ";" */
                continue ;

            if ( token.code == identifier_colon )   /* ":" */
                continue ;

            if ( token.code == identifier_continue )  /* "CONTINUE" */
            {
                if ( dcb_options & DCB_DEBUG ) codeblock_add( code, MN_SENTENCE, line_count + ( current_file << 20 ) ) ;

                if ( !code->loop_active ) compile_error( MSG_NO_LOOP ) ;

                codeblock_add( code, MN_REPEAT, code->loop_active ) ;
                compile_sentence_end() ;
                continue ;
            }

            if ( token.code == identifier_break ) /* "BREAK" */
            {
                if ( dcb_options & DCB_DEBUG ) codeblock_add( code, MN_SENTENCE, line_count + ( current_file << 20 ) ) ;

                if ( !code->loop_active ) compile_error( MSG_NO_LOOP ) ;

                codeblock_add( code, MN_BREAK, code->loop_active ) ;
                compile_sentence_end() ;
                continue ;
            }

            if ( token.code == identifier_frame || token.code == identifier_yield ) /* "FRAME or YIELD " */
            {
                if ( proc->type != TYPE_INT && proc->type != TYPE_DWORD )
                    if ( !( proc->flags & PROC_FUNCTION ) ) compile_error( MSG_FRAME_REQUIRES_INT );

                if ( dcb_options & DCB_DEBUG ) codeblock_add( code, MN_SENTENCE, line_count + ( current_file << 20 ) ) ;

                token_next() ;
                if ( token.type != IDENTIFIER || token.code != identifier_semicolon ) /* ";" */
                {
                    token_back() ;
                    compile_expresion( 0, 0, 0, TYPE_DWORD ) ;
                    codeblock_add( code, MN_FRAME, 0 ) ;
                    compile_sentence_end() ;
                }
                else
                {
                    codeblock_add( code, MN_PUSH, 100 ) ;
                    codeblock_add( code, MN_FRAME, 0 ) ;
                }
                proc->flags |= PROC_USES_FRAME;
                continue ;
            }

            if ( token.code == identifier_debug ) /* "DEBUG" */
            {
                if ( dcb_options & DCB_DEBUG ) codeblock_add( code, MN_SENTENCE, line_count + ( current_file << 20 ) ) ;

                codeblock_add( code, MN_DEBUG, 0 ) ;
                compile_sentence_end() ;
                continue ;
            }

            if ( token.code == identifier_return )  /* "RETURN" */
            {
                if ( dcb_options & DCB_DEBUG ) codeblock_add( code, MN_SENTENCE, line_count + ( current_file << 20 ) ) ;

                token_next() ;
                if ( token.type != IDENTIFIER || token.code != identifier_semicolon ) /* ";" */
                {
                    token_back() ;
                    compile_expresion( 0, 0, 0, p->type ) ;
                    codeblock_add( code, MN_RETURN, 0 ) ;
                    compile_sentence_end() ;
                }
                else
                {
                    codeblock_add( code, MN_END, 0 ) ;
                }
                continue ;
            }

            if ( token.code == identifier_clone ) /* "CLONE" */
            {
                if ( dcb_options & DCB_DEBUG ) codeblock_add( code, MN_SENTENCE, line_count + ( current_file << 20 ) ) ;

                et1 = codeblock_label_add( code, -1 ) ;
                codeblock_add( code, MN_CLONE, et1 ) ;
                compile_block( p ) ;
                codeblock_label_set( code, et1, code->current ) ;
                proc->flags |= PROC_USES_FRAME;
                continue ;
            }

            if ( token.code == identifier_if )  /* "IF" */
            {
                /* Label at the end of a IF/ELSEIF/ELSEIF/ELSE chain */
                int end_of_chain = -1;

                if ( dcb_options & DCB_DEBUG ) codeblock_add( code, MN_SENTENCE, line_count + ( current_file << 20 ) ) ;

                for ( ;; )
                {
                    token_next() ;
                    if ( token.type != IDENTIFIER || token.code != identifier_leftp ) /* "(" */
                    {
                        token_back() ;
                        compile_expresion( 0, 0, 0, TYPE_DWORD );
                        token_next() ;
                        if ( token.type != IDENTIFIER || ( token.code != identifier_semicolon && token.code != identifier_colon ) ) /* ";" or ":" */
                            compile_error( MSG_EXPECTED, ";" MSG_OR ":" ) ;
                    }
                    else
                    {
                        compile_expresion( 0, 0, 0, TYPE_DWORD );
                        token_next() ;
                        if ( token.type != IDENTIFIER || token.code != identifier_rightp ) /* ")" */
                            compile_error( MSG_EXPECTED, ")" ) ;
                    }

                    et1 = codeblock_label_add( code, -1 ) ;
                    codeblock_add( code, MN_JFALSE, et1 ) ;
                    compile_block( p ) ;
                    if ( token.type == IDENTIFIER && token.code == identifier_else )  /* "ELSE" */
                    {
                        et2 = codeblock_label_add( code, -1 ) ;
                        codeblock_add( code, MN_JUMP, et2 ) ;
                        codeblock_label_set( code, et1, code->current ) ;
                        compile_block( p ) ;
                        codeblock_label_set( code, et2, code->current ) ;
                        break;
                    }
                    else if ( token.type == IDENTIFIER && token.code == identifier_elseif ) /* "ELSEIF" */
                    {
                        if ( dcb_options & DCB_DEBUG ) codeblock_add( code, MN_SENTENCE, line_count + ( current_file << 20 ) ) ;
                        if ( end_of_chain == -1 ) end_of_chain = codeblock_label_add( code, -1 ) ;
                        codeblock_add( code, MN_JUMP, end_of_chain );
                        codeblock_label_set( code, et1, code->current ) ;
                        continue;
                    }
                    else
                    {
                        codeblock_label_set( code, et1, code->current ) ;
                        break;
                    }
                }
                if ( end_of_chain != -1 ) codeblock_label_set( code, end_of_chain, code->current ) ;
                continue ;
            }

            if ( token.code == identifier_for ) /* "FOR" */
            {
                if ( dcb_options & DCB_DEBUG ) codeblock_add( code, MN_SENTENCE, line_count + ( current_file << 20 ) ) ;

                loop = codeblock_loop_add( code ) ;
                et1 = codeblock_label_add( code, -1 ) ;
                et2 = codeblock_label_add( code, -1 ) ;

                token_next() ;
                if ( token.type != IDENTIFIER || token.code != identifier_leftp ) /* "(" */
                    compile_error( MSG_EXPECTED, "(" ) ;

                /* Inicializadores */
                token_next() ;
                if ( token.type != IDENTIFIER || token.code != identifier_semicolon ) /* ";" */
                {
                    token_back() ;
                    do
                    {
                        compile_expresion( 0, 0, 0, TYPE_DWORD ) ;
                        codeblock_add( code, MN_POP, 0 ) ;
                        token_next() ;
                    }
                    while ( token.type == IDENTIFIER && token.code == identifier_comma ) ; /* "," */
                }

                if ( token.type != IDENTIFIER || token.code != identifier_semicolon ) /* ";" */
                    compile_error( MSG_EXPECTED, ";" ) ;

                codeblock_label_set( code, et1, code->current ) ;

                /* Condiciones */
                token_next() ;
                if ( token.type != IDENTIFIER || token.code != identifier_semicolon ) /* ";" */
                {
                    if ( dcb_options & DCB_DEBUG ) codeblock_add( code, MN_SENTENCE, line_count + ( current_file << 20 ) ) ;
                    token_back() ;
                    do
                    {
                        compile_expresion( 0, 0, 0, TYPE_DWORD ) ;
                        codeblock_add( code, MN_BRFALSE, loop ) ;
                        token_next() ;
                    }
                    while ( token.type == IDENTIFIER && token.code == identifier_comma ) ; /* "," */
                }

                if ( token.type != IDENTIFIER || token.code != identifier_semicolon ) /* ";" */
                    compile_error( MSG_EXPECTED, ";" ) ;

                codeblock_add( code, MN_JUMP, et2 ) ;

                /* Incrementos */
                codeblock_loop_start( code, loop, code->current ) ;

                token_next() ;
                if ( token.type != IDENTIFIER || token.code != identifier_rightp )  /* ")" */
                {
                    if ( dcb_options & DCB_DEBUG ) codeblock_add( code, MN_SENTENCE, line_count + ( current_file << 20 ) ) ;
                    token_back() ;
                    do
                    {
                        compile_expresion( 0, 0, 0, TYPE_DWORD ) ;
                        codeblock_add( code, MN_POP, 0 ) ;
                        token_next() ;
                    }
                    while ( token.type == IDENTIFIER && token.code == identifier_comma ) ;  /* "," */
                }

                if ( token.type != IDENTIFIER || token.code != identifier_rightp )  /* ")" */
                    compile_error( MSG_EXPECTED, ")" ) ;

                codeblock_add( code, MN_JUMP, et1 ) ;

                /* Bloque */
                codeblock_label_set( code, et2, code->current ) ;

                last_loop = code->loop_active ;
                code->loop_active = loop ;
                compile_block( p ) ;
                code->loop_active = last_loop ;

                if ( dcb_options & DCB_DEBUG ) codeblock_add( code, MN_SENTENCE, line_count + ( current_file << 20 ) ) ;

                codeblock_add( code, MN_REPEAT, loop ) ;
                codeblock_loop_end( code, loop, code->current ) ;
                continue ;
            }

            if ( token.code == identifier_switch )  /* "SWITCH" */
            {
                BASETYPE switch_type = TYPE_UNDEFINED;
                expresion_result switch_exp ;

                if ( dcb_options & DCB_DEBUG ) codeblock_add( code, MN_SENTENCE, line_count + ( current_file << 20 ) ) ;

                token_next() ;
                if ( token.type != IDENTIFIER || token.code != identifier_leftp ) /* "(" */
                    compile_error( MSG_EXPECTED, "(" ) ;

                switch_exp = compile_expresion( 0, 0, 0, TYPE_UNDEFINED ) ;
                switch_type = typedef_base( switch_exp.type );

                if ( switch_type == TYPE_ARRAY && switch_exp.type.chunk[0].type == TYPE_CHAR )
                {
                    codeblock_add( code, MN_A2STR, 0 ) ;
                    switch_type = TYPE_STRING;
                }
                else if ( typedef_is_integer( switch_exp.type ) )
                {
                    switch_exp = convert_result_type( switch_exp, TYPE_INT );
                    switch_type = TYPE_INT;
                }
                token_next() ;

                if ( token.type != IDENTIFIER || token.code != identifier_rightp )  /* ")" */
                    compile_error( MSG_EXPECTED, ")" );

                token_next() ;
                if ( token.type != IDENTIFIER || ( token.code != identifier_semicolon && token.code != identifier_colon ) ) /* ";" or ":" */
                    token_back() ;

                if ( switch_type == TYPE_STRING )
                    codeblock_add( code, MN_SWITCH | MN_STRING, 0 ) ;
                else
                    codeblock_add( code, MN_SWITCH, 0 ) ;

                et1 = codeblock_label_add( code, -1 ) ;
                for ( ;; )
                {
                    token_next() ;
                    if ( token.type == IDENTIFIER && token.code == identifier_case )  /* "CASE" */
                    {
                        if ( dcb_options & DCB_DEBUG ) codeblock_add( code, MN_SENTENCE, line_count + ( current_file << 20 ) ) ;

                        for ( ;; )
                        {
                            token_next() ;
                            if ( token.type == IDENTIFIER && token.code == identifier_colon ) /* ":" */
                                break ;
                            if ( token.type == IDENTIFIER && token.code == identifier_comma ) /* "," */
                                continue ;
                            token_back() ;

                            compile_expresion( 0, 0, 0, switch_type ) ;
                            token_next() ;
                            if ( token.type == IDENTIFIER && token.code == identifier_twopoints ) /* ".." */
                            {
                                compile_expresion( 0, 0, 0, switch_type ) ;
                                if ( switch_type == TYPE_STRING )
                                {
                                    codeblock_add( code, MN_CASE_R | MN_STRING, 0 ) ;
                                }
                                else
                                {
                                    codeblock_add( code, MN_CASE_R, 0 ) ;
                                }
                                token_next() ;
                            }
                            else
                            {
                                if ( switch_type == TYPE_STRING )
                                {
                                    codeblock_add( code, MN_CASE | MN_STRING, 0 ) ;
                                }
                                else
                                {
                                    codeblock_add( code, MN_CASE, 0 ) ;
                                }
                            }

                            if ( token.type == IDENTIFIER && token.code == identifier_colon ) /* ":" */
                                break ;

                            if ( token.type != IDENTIFIER || token.code != identifier_comma ) /* "," */
                                compile_error( MSG_EXPECTED, "," ) ;
                        }
                        et2 = codeblock_label_add( code, -1 ) ;
                        codeblock_add( code, MN_JNOCASE, et2 ) ;
                        compile_block( p ) ;
                        codeblock_add( code, MN_JUMP, et1 ) ;
                        codeblock_label_set( code, et2, code->current ) ;
                    }
                    else if ( token.type == IDENTIFIER && token.code == identifier_default ) /* "DEFAULT" */
                    {
                        if ( dcb_options & DCB_DEBUG ) codeblock_add( code, MN_SENTENCE, line_count + ( current_file << 20 ) ) ;

                        token_next() ;

                        if ( token.type != IDENTIFIER || token.code != identifier_colon ) /* ":" */
                            compile_error( MSG_EXPECTED, ":" ) ;

                        compile_block( p ) ;
                    }
                    else if ( token.type == IDENTIFIER && token.code == identifier_semicolon )  /* ";" */
                        continue ;
                    else if ( token.type == IDENTIFIER && token.code == identifier_end )  /* "END" */
                        break ;
                    else
                        compile_error( MSG_EXPECTED, "CASE" );
                }
                codeblock_label_set( code, et1, code->current ) ;
                continue ;
            }

            if ( token.code == identifier_loop )  /* "LOOP" */
            {
                loop = codeblock_loop_add( code ) ;
                codeblock_loop_start( code, loop, code->current ) ;

                if ( dcb_options & DCB_DEBUG ) codeblock_add( code, MN_SENTENCE, line_count + ( current_file << 20 ) ) ;

                last_loop = code->loop_active ;
                code->loop_active = loop ;
                compile_block( p ) ;
                code->loop_active = last_loop ;

                if ( dcb_options & DCB_DEBUG ) codeblock_add( code, MN_SENTENCE, line_count + ( current_file << 20 ) ) ;

                codeblock_add( code, MN_REPEAT, loop ) ;
                codeblock_loop_end( code, loop, code->current ) ;
                continue ;
            }

            /* FROM ... TO */

            if ( token.code == identifier_from )  /* "FROM" */
            {
                int inc = 1 ;
                CODEBLOCK_POS var_pos;
                CODEBLOCK_POS var_end;
                int is_unsigned = 0;
                int is_float = 0;
                BASETYPE res_type = TYPE_UNDEFINED;

                et1 = codeblock_label_add( code, -1 ) ;

                /* Compile the variable access */

                loop = codeblock_loop_add( code ) ;

                if ( dcb_options & DCB_DEBUG ) codeblock_add( code, MN_SENTENCE, line_count + ( current_file << 20 ) ) ;

                var_pos = codeblock_pos( code );

                res = compile_value() ;
                res_type = typedef_base( res.type );

                if ( typedef_is_unsigned( res.type ) ) is_unsigned = MN_UNSIGNED;
                if ( typedef_is_float( res.type ) ) is_float = MN_FLOAT;

                var_end = codeblock_pos( code );
                if ( !res.lvalue ) compile_error( MSG_VARIABLE_REQUIRED ) ;
                if ( !typedef_is_numeric( res.type ) ) compile_error( MSG_NUMBER_REQUIRED );

                /* Compile the assignation of first value */

                token_next() ;

                if ( token.type != IDENTIFIER || token.code != identifier_equal ) /* "=" */
                    compile_error( MSG_EXPECTED, "=" ) ;

                from = compile_expresion( 0, 0, 0, res_type );
                codeblock_add( code, MN_LETNP | mntype( res.type, 0 ), 0 );

                /* Compile the loop termination check */

                codeblock_label_set( code, et1, code->current ) ;

                codeblock_add_block( code, var_pos, var_end );
                codeblock_add( code, MN_PTR | mntype( res.type, 0 ), 0 );
                token_next() ;

                if ( token.type != IDENTIFIER || token.code != identifier_to )  /* "TO" */
                    compile_error( MSG_EXPECTED, "TO" ) ;

                to = compile_expresion( 0, 0, 0, res_type ) ;

                token_next() ;
                if ( token.type == IDENTIFIER && token.code == identifier_step )  /* "STEP" */
                {
                    CODEBLOCK_POS p = codeblock_pos( code );
                    expresion_result r = compile_expresion( 1, 0, 0, res_type ) ;
                    if ( !r.constant ) compile_error( MSG_CONSTANT_EXP );
                    if ( !typedef_is_numeric( r.type ) ) compile_error( MSG_NUMBER_REQUIRED );

                    if ( res_type == TYPE_FLOAT )
                    {
                        if ( r.fvalue == 0.0 ) compile_error( MSG_INVALID_STEP ) ;
                        inc = *( int * ) & r.fvalue;
                    }
                    else
                    {
                        if ( r.value == 0 ) compile_error( MSG_INVALID_STEP ) ;
                        inc = r.value;
                    }

                    codeblock_setpos( code, p );
                    if (( res_type == TYPE_FLOAT && r.fvalue > 0 ) || ( res_type != TYPE_FLOAT && r.value > 0 ) )
                        codeblock_add( code, MN_LTE | is_float | is_unsigned, 0 ) ;
                    else
                        codeblock_add( code, MN_GTE | is_float | is_unsigned, 0 ) ;
                }
                else
                {
                    if ( from.constant && to.constant )
                    {
                        int dec = 0;
                        if ( res_type == TYPE_FLOAT && from.fvalue > to.fvalue )
                        {
                            float v = -1;
                            inc = *(( int * ) & v );
                            dec = 1;
                        }
                        else if ( from.value > to.value )
                        {
                            inc = -1;
                            dec = 1;
                        }
                        codeblock_add( code, ( dec ? MN_GTE : MN_LTE ) | is_float | is_unsigned, 0 ) ;
                    }
                    else
                        codeblock_add( code, MN_LTE | is_float | is_unsigned, 0 ) ;
                    token_back() ;
                }

                token_next() ;
                if ( token.type != IDENTIFIER || token.code != identifier_semicolon ) /* ";" */
                    compile_error( MSG_EXPECTED, ";" ) ;

                if ( dcb_options & DCB_DEBUG ) codeblock_add( code, MN_SENTENCE, line_count + ( current_file << 20 ) ) ;

                codeblock_add( code, MN_BRFALSE, loop ) ;

                /* Compile the loop block contents */

                last_loop = code->loop_active ;
                code->loop_active = loop ;
                compile_block( p ) ;
                code->loop_active = last_loop ;

                /* Compile the increment and looping code */

                codeblock_loop_start( code, loop, code->current ) ;

                codeblock_add_block( code, var_pos, var_end );
                if ( inc == 1 )
                    codeblock_add( code, MN_INC | mntype( res.type, 0 ), 1 ) ;
                else if ( inc == -1 )
                    codeblock_add( code, MN_DEC | mntype( res.type, 0 ), 1 ) ;
                else
                {
                    codeblock_add( code, MN_PUSH, inc ) ;
                    codeblock_add( code, MN_VARADD | mntype( res.type, 0 ), 0 ) ;
                }
                codeblock_add( code, MN_POP, 0 ) ;
                codeblock_add( code, MN_JUMP, et1 ) ;

                codeblock_loop_end( code, loop, code->current ) ;
                continue ;
            }

            /* REPEAT ... UNTIL */

            if ( token.code == identifier_repeat ) /* "REPEAT" */
            {
                et1 = codeblock_label_add( code, -1 ) ;
                et2 = codeblock_label_add( code, -1 ) ;

                loop = codeblock_loop_add( code ) ;

                codeblock_add( code, MN_JUMP, et1 ) ;

                codeblock_loop_start( code, loop, code->current ) ;

                codeblock_add( code, MN_JUMP, et2 ) ;
                codeblock_label_set( code, et1, code->current ) ;

                if ( dcb_options & DCB_DEBUG ) codeblock_add( code, MN_SENTENCE, line_count + ( current_file << 20 ) ) ;

                last_loop = code->loop_active ;
                code->loop_active = loop ;
                compile_block( p ) ;
                code->loop_active = last_loop ;

                codeblock_label_set( code, et2, code->current ) ;

                if ( dcb_options & DCB_DEBUG ) codeblock_add( code, MN_SENTENCE, line_count + ( current_file << 20 ) ) ;

                if ( token.type != IDENTIFIER || token.code != identifier_until ) /* "UNTIL" */
                    compile_error( MSG_EXPECTED, "UNTIL" ) ;

                token_next() ;
                if ( token.type == IDENTIFIER && token.code == identifier_leftp ) /* "(" */
                {
                    compile_expresion( 0, 0, 0, TYPE_DWORD ) ;
                    token_next() ;
                    if ( token.type != IDENTIFIER || token.code != identifier_rightp ) /* ")" */
                        compile_error( MSG_EXPECTED, ")" ) ;
                }
                else
                {
                    token_back() ;
                    compile_expresion( 0, 0, 0, TYPE_DWORD ) ;
                    token_next() ;
                    if ( token.type != IDENTIFIER || token.code != identifier_semicolon ) /* ";" */
                        compile_error( MSG_EXPECTED, ";" ) ;
                }
                codeblock_add( code, MN_JFALSE, et1 ) ;
                codeblock_loop_end( code, loop, code->current ) ;
                continue ;
            }

            /* WHILE ... END */

            if ( token.code == identifier_while ) /* "WHILE" */
            {
                token_next() ;
                if ( token.type != IDENTIFIER || token.code != identifier_leftp ) /* "(" */
                {
                    token_back() ;
                    loop = codeblock_loop_add( code ) ;
                    codeblock_loop_start( code, loop, code->current ) ;

                    if ( dcb_options & DCB_DEBUG ) codeblock_add( code, MN_SENTENCE, line_count + ( current_file << 20 ) ) ;

                    compile_expresion( 0, 0, 0, TYPE_DWORD ) ;
                    token_next() ;
                    if ( token.type != IDENTIFIER || ( token.code != identifier_semicolon && token.code != identifier_colon ) ) /* ";" or ":" */
                        compile_error( MSG_EXPECTED, ";" MSG_OR ":" ) ;
                }
                else
                {
                    loop = codeblock_loop_add( code ) ;
                    codeblock_loop_start( code, loop, code->current ) ;

                    if ( dcb_options & DCB_DEBUG ) codeblock_add( code, MN_SENTENCE, line_count + ( current_file << 20 ) ) ;

                    compile_expresion( 0, 0, 0, TYPE_DWORD ) ;
                    token_next() ;
                    if ( token.type != IDENTIFIER || token.code != identifier_rightp ) /* ")" */
                        compile_error( MSG_EXPECTED, ")" ) ;
                }

                codeblock_add( code, MN_BRFALSE, loop ) ;

                last_loop = code->loop_active ;
                code->loop_active = loop ;
                compile_block( p ) ;
                code->loop_active = last_loop ;

                if ( dcb_options & DCB_DEBUG ) codeblock_add( code, MN_SENTENCE, line_count + ( current_file << 20 ) ) ;

                codeblock_add( code, MN_REPEAT, loop ) ;
                codeblock_loop_end( code, loop, code->current ) ;
                continue ;
            }

            if ( token.code == identifier_jmp ) /* JMP */
            {
                if ( dcb_options & DCB_DEBUG ) codeblock_add( code, MN_SENTENCE, line_count + ( current_file << 20 ) ) ;

                token_next();

                if ( token.type != IDENTIFIER ) compile_error( MSG_INVALID_PARAM );

                if (( codelabel = codeblock_label_get_id_by_name( code, token.code ) ) == -1 )
                    codelabel = codeblock_label_add( code, token.code );

                codeblock_add( code, MN_JUMP, codelabel );

                token_next() ;
                if ( token.type != IDENTIFIER || token.code != identifier_semicolon ) compile_error( MSG_EXPECTED, ";" ) ;
                continue;
            }

            if ( token.code == identifier_call ) /* CALL */
            {
                if ( dcb_options & DCB_DEBUG ) codeblock_add( code, MN_SENTENCE, line_count + ( current_file << 20 ) ) ;

                token_next();

                if ( token.type != IDENTIFIER ) compile_error( MSG_INVALID_PARAM );

                if (( codelabel = codeblock_label_get_id_by_name( code, token.code ) ) == -1 )
                    codelabel = codeblock_label_add( code, token.code );

                codeblock_add( code, MN_NCALL, codelabel );

                token_next() ;
                if ( token.type != IDENTIFIER || token.code != identifier_semicolon ) compile_error( MSG_EXPECTED, ";" ) ;
                continue;
            }

            if ( token.code == identifier_on ) /* ON EXIT/ON ERROR */
            {
                int on_req = 0;

                if ( dcb_options & DCB_DEBUG ) codeblock_add( code, MN_SENTENCE, line_count + ( current_file << 20 ) ) ;

                token_next();

                if ( token.type != IDENTIFIER || ( token.code != identifier_exit && token.code != identifier_error ) ) compile_error( MSG_ON_PARAM_ERR );

                on_req = token.code;

                token_next();
                if ( token.type != IDENTIFIER || token.code != identifier_jmp ) compile_error( MSG_GOTO_EXP );

                token_next();
                if ( token.type == NUMBER && token.code == 0 )
                    codelabel = -1;
                else if (( codelabel = codeblock_label_get_id_by_name( code, token.code ) ) == -1 )
                    codelabel = codeblock_label_add( code, token.code );

                codeblock_add( code, on_req == identifier_exit ? MN_EXITHNDLR : MN_ERRHNDLR, codelabel );

                token_next() ;
                if ( token.type != IDENTIFIER || token.code != identifier_semicolon ) compile_error( MSG_EXPECTED, ";" ) ;
                continue;
            }
        }

        if ( token.type == LABEL ||     /* Label */
                ( token.type == IDENTIFIER && ( token.code == identifier_onexit || token.code == identifier_onerror ) ) /* ONEXIT or ONERROR */
           )
        {
            if ( token.code == identifier_onexit )  /* "ONEXIT" */
            {
                /* Finalizo el bloque actual y todo el codigo a continuacion es onexit */
                codeblock_add( code, MN_END, 0 ) ;
                p->exitcode = code->current;
            }

            if ( token.code == identifier_onerror )  /* "ONERROR" */
            {
                /* Finalizo el bloque actual y todo el codigo a continuacion es onerror */
                codeblock_add( code, MN_END, 0 ) ;
                p->errorcode = code->current;
            }

            if (( codelabel = codeblock_label_get_id_by_name( code, token.code ) ) != -1 )
            {
                if ( codeblock_label_get( code, codelabel ) != -1 )
                    compile_error( "Label already defined" );
            }
            else
            {
                codelabel = codeblock_label_add( code, token.code );
            }

            codeblock_label_set( code, codelabel, code->current ) ;
            continue;
        }

        if ( token.type != IDENTIFIER ) /* || token.code < reserved_words) */ compile_error( MSG_INVALID_SENTENCE ) ;

        if ( dcb_options & DCB_DEBUG ) codeblock_add( code, MN_SENTENCE, line_count + ( current_file << 20 ) ) ;

        token_back() ;

        /* Asignation */

        res = compile_subexpresion() ;
        if ( !res.asignation && !res.call )
            compile_error( MSG_INVALID_SENTENCE ) ;
        if ( typedef_is_string( res.type ) && !res.lvalue )
            codeblock_add( code, MN_POP | MN_STRING, 0 ) ;
        else
            codeblock_add( code, MN_POP, 0 ) ;

        if ( compile_sentence_end() ) break ;
    }
}

