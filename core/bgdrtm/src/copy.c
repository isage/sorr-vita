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

/*
 * FILE        : i_copy.c
 * DESCRIPTION : Struct copy functions based on varspace type info
 *
 * HISTORY:      0.85 - First version
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <math.h>
#include <time.h>

#include "xstrings.h"
#include "bgdrtm.h"

static int copytype( void * dst, void * src, DCB_TYPEDEF * var );

/*
 *  FUNCTION : copyvars
 *
 *  Copy data using a varspace's type information. It updates the
 *  destination pointer while walking the variables.
 *
 *  PARAMS :
 *  dst    Pointer to the destination memory
 *  src    Pointer to the data
 *  var    Pointer to the type array
 *  nvars   Number of variables (length of var array)
 *
 *  RETURN VALUE :
 *      Number of bytes actually copied
 *
 */

int copyvars( void * dst, void * src, DCB_VAR * var, int nvars )
{
    int result = 0;
    int partial;

    for ( ; nvars > 0; nvars--, var++ )
    {
        partial = copytype( dst, src, &var->Type );
        src = (( uint8_t* )src ) + partial;
        dst = (( uint8_t* )dst ) + partial;
        result += partial;
    }
    return result;
}


/*
 *  FUNCTION : copytypes
 *
 *  Copy data from memory using type information stored in memory
 *
 *  PARAMS :
 *  dst    Pointer to the destination memory
 *  src    Pointer to the source
 *  var    Pointer to the type array
 *  nvars   Number of variables (length of var array)
 *
 *  RETURN VALUE :
 *      Number of bytes actually coppied
 *
 */

int copytypes( void * dst, void * src, DCB_TYPEDEF * var, int nvars, int reps )
{
    int result = 0;
    int partial;
    DCB_TYPEDEF * _var = var;
    int _nvars = nvars ;

    for ( ; reps > 0; reps-- )
    {
        var = _var;
        nvars = _nvars;
        for ( ; nvars > 0; nvars--, var++ )
        {
            partial = copytype( dst, src, var );
            result += partial;
            src = (( uint8_t* )src ) + partial;
            dst = (( uint8_t* )dst ) + partial;
        }
    }
    return result;
}

/*
 *  FUNCTION : copytype
 *
 *  Copy one variable using the given type information.
 *
 *  PARAMS :
 *  dst    Pointer to the file object
 *  src    Pointer to the data
 *  var    Pointer to the variable type
 *
 *  RETURN VALUE :
 *      Number of bytes actually written
 *
 */

static int copytype( void * dst, void * src, DCB_TYPEDEF * var )
{
    int count  = 1;
    int result = 0;
    int n      = 0;

    for ( ;; )
    {
        switch ( var->BaseType[n] )
        {
            case TYPE_FLOAT:
            case TYPE_INT:
            case TYPE_DWORD:
            case TYPE_POINTER:
                memcpy( dst, src, 4 * count );
                return 4 * count;

            case TYPE_WORD:
            case TYPE_SHORT:
                memcpy( dst, src, 2 * count );
                return 2 * count;

            case TYPE_BYTE:
            case TYPE_SBYTE:
            case TYPE_CHAR:
                memcpy( dst, src, count );
                return count;

            case TYPE_STRING:
                while ( count-- )
                {
                    string_discard( *( int * )dst );
                    string_use( *( int * )src );
                    *(( int * )dst ) = *(( int * )src );
                    dst = (( int * )dst ) + 1;
                    src = (( int * )src ) + 1;
                    result += 4;
                }
                return result;

            case TYPE_ARRAY:
                count *= var->Count[n];
                n++;
                continue;

            case TYPE_STRUCT:
                for ( ; count ; count-- )
                {
                    int partial = copyvars( dst, src, dcb.varspace_vars[var->Members], dcb.varspace[var->Members].NVars );
                    src = (( uint8_t* )src ) + partial;
                    dst = (( uint8_t* )dst ) + partial;
                    result += partial;
                }
                break;

            default:
                fprintf( stderr, "ERROR: Runtime error - Could not copy datatype\n" ) ;
                exit( 1 );
                break;
        }
        break;
    }
    return result;
}

/**
 *  POINTER #COPY# (POINTER DEST, POINTER SRC, POINTER VARSPACE, INT VARS)
 *
 *  Copy struct data from src to dst, using the information varspace given
 **/

int bgd_copy_struct( INSTANCE * my, int * params )
{
    return ( int ) copytypes(( void * )params[0], ( void * )params[1], ( DCB_TYPEDEF * )params[2], params[3], params[4] );
}

int bgd_internal_memcopy( INSTANCE * my, int * params )
{
    memmove(( void * )params[0], ( void * )params[1], params[2] ) ;
    return 1 ;
}

int bgd_internal_copy_string_array( INSTANCE * my, int * params )
{
    int n = params[ 2 ];
    while( n-- )
    {
        ((int *)( params[ 0 ] )) [ n ] = ((int *)( params[ 1 ] )) [ n ];
        string_use( ((int *)( params[ 0 ] )) [ n ] );
    }

    return 1 ;
}
