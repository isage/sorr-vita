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

#include <stdint.h>

#include "files.h"
#include "varspace_file.h"
#include "xstrings.h"

/* ----------------------------------------------------------------- */
/*
 *  FUNCTION : loadvars
 *
 *  Load data from memory to a given file at the current file offset,
 *  using a varspace's type information
 *
 *  PARAMS :
 *  fp    Pointer to the file object
 *  data   Pointer to the data
 *  var    Pointer to the type array
 *  nvars   Number of variables (length of var array)
 *
 *  RETURN VALUE :
 *      Number of bytes actually read
 *
 */

int loadvars( file * fp, void * data, DCB_VAR * var, int nvars, int dcbformat )
{
    int result = 0;
    int partial;

    for ( ; nvars > 0; nvars--, var++ )
    {
        partial = loadtype( fp, data, &var->Type, dcbformat );
        data = ( uint8_t* )data + partial;
        result += partial;
    }
    return result;
}


/*
 *  FUNCTION : loadtypes
 *
 *  Load data from memory to a given file at the current file offset,
 *  using type information stored in memory
 *
 *  PARAMS :
 *  fp    Pointer to the file object
 *  data   Pointer to the data
 *  var    Pointer to the type array
 *  nvars   Number of variables (length of var array)
 *
 *  RETURN VALUE :
 *      Number of bytes actually read
 *
 */

int loadtypes( file * fp, void * data, DCB_TYPEDEF * var, int nvars, int dcbformat )
{
    int result = 0;
    int partial;

    for ( ; nvars > 0; nvars--, var++ )
    {
        partial = loadtype( fp, data, var, dcbformat );
        data = (( uint8_t* )data ) + partial;
        result += partial;
    }
    return result;
}

/*
 *  FUNCTION : savevars
 *
 *  Save data from memory to a given file at the current file offset,
 *  using a varspace's type information
 *
 *  PARAMS :
 *  fp    Pointer to the file object
 *  data   Pointer to the data
 *  var    Pointer to the type array
 *  nvars   Number of variables (length of var array)
 *
 *  RETURN VALUE :
 *      Number of bytes actually written
 *
 */

int savevars( file * fp, void * data, DCB_VAR * var, int nvars, int dcbformat )
{
    int result = 0;
    int partial;

    for ( ; nvars > 0; nvars--, var++ )
    {
        partial = savetype( fp, data, &var->Type, dcbformat );
        data = (( uint8_t* ) data ) + partial;
        result += partial;
    }
    return result;
}


/*
 *  FUNCTION : savetypes
 *
 *  Save data from memory to a given file at the current file offset,
 *  using type information stored in memory
 *
 *  PARAMS :
 *  fp    Pointer to the file object
 *  data   Pointer to the data
 *  var    Pointer to the type array
 *  nvars   Number of variables (length of var array)
 *
 *  RETURN VALUE :
 *      Number of bytes actually written
 *
 */

int savetypes( file * fp, void * data, DCB_TYPEDEF * var, int nvars, int dcbformat )
{
    int result = 0;
    int partial;

    for ( ; nvars > 0; nvars--, var++ )
    {
        partial = savetype( fp, data, var, dcbformat );
        result += partial;
        data = (( uint8_t* )data ) + partial;
    }
    return result;
}

/*
 *  FUNCTION : savetype
 *
 *  Save one variable from memory to a given file at the current file offset,
 *  using the given type information. This is a convenience function
 *  used by both savevars and savetypes.
 *
 *  PARAMS :
 *  fp    Pointer to the file object
 *  data   Pointer to the data
 *  var    Pointer to the variable type
 *
 *  RETURN VALUE :
 *      Number of bytes actually written
 *
 */

int savetype( file * fp, void * data, DCB_TYPEDEF * var, int dcbformat )
{
    int n = 0;
    int count = 1;
    int result = 0;
    const char * str;
    int32_t len;
    int partial;

    for ( ;; )
    {
        switch ( var->BaseType[n] )
        {
            case TYPE_FLOAT:
            case TYPE_INT:
            case TYPE_DWORD:
            case TYPE_POINTER:
                result += file_writeUint32A( fp, data, count ) * sizeof( uint32_t );
                break;

            case TYPE_SHORT:
            case TYPE_WORD:
                result += file_writeUint16A( fp, data, count ) * sizeof( uint16_t );
                break;

            case TYPE_BYTE:
            case TYPE_SBYTE:
            case TYPE_CHAR:
                result += file_write( fp, data, count );
                break;

            case TYPE_STRING:
                if ( dcbformat )
                {
                    result += file_writeUint32A( fp, data, count ) * sizeof( uint32_t );
                }
                else
                {
                    for ( ; count ; count-- )
                    {
                        str = string_get( *( uint32_t * )data );
                        len = strlen( str );
                        file_writeUint32( fp, (uint32_t *)&len );
                        file_write( fp, ( void* )str, len );
                        data = ( uint8_t* )data + sizeof( uint32_t );
                        result += sizeof( uint32_t );
                    }
                }
                break;

            case TYPE_ARRAY:
                count *= var->Count[n];
                n++;
                continue;

            case TYPE_STRUCT:
                for ( ; count ; count-- )
                {
                    partial = savevars( fp, data, dcb.varspace_vars[var->Members], dcb.varspace[var->Members].NVars, dcbformat );
                    data = (( uint8_t* )data ) + partial;
                    result += partial;
                }
                break;

            default:
                /* Can't be possible save this struct */
                return -1;
                break;
        }
        break;
    }
    return result;
}

/*
 *  FUNCTION : loadtype
 *
 *  Load one variable from a given file at the current file offset,
 *  using the given type information. This is a convenience function
 *  used by both loadvars and loadtypes.
 *
 *  PARAMS :
 *  fp    Pointer to the file object
 *  data   Pointer to the data
 *  var    Pointer to the variable type
 *
 *  RETURN VALUE :
 *      Number of bytes actually written
 *
 */

int loadtype( file * fp, void * data, DCB_TYPEDEF * var, int dcbformat )
{
    int n = 0;
    int count = 1;
    int result = 0;
    char * str;
    int len;
    int partial;

    for ( ;; )
    {
        switch ( var->BaseType[n] )
        {
                /* Not sure about float types */
            case TYPE_FLOAT:
            case TYPE_INT:
            case TYPE_DWORD:
            case TYPE_POINTER:
                result += file_readUint32A( fp, data, count ) * sizeof( uint32_t );
                break;

            case TYPE_SHORT:
            case TYPE_WORD:
                result += file_readUint16A( fp, data, count ) * sizeof( uint16_t );
                break;

            case TYPE_SBYTE:
            case TYPE_BYTE:
            case TYPE_CHAR:
                result += file_read( fp, data, count );
                break;

            case TYPE_STRING:
                if ( dcbformat )
                {
                    result += file_readUint32A( fp, data, count ) * sizeof( uint32_t );
                }
                else
                {
                    for ( ; count ; count-- )
                    {
                        string_discard( *( uint32_t* )data );
                        file_readUint32( fp, (uint32_t *)&len );
                        str = malloc( len + 1 );
                        if ( !str )
                        {
                            fprintf( stderr, "loadtype: out of memory\n" ) ;
                            return -1;
                        }

                        if ( len > 0 ) file_read( fp, str, len );
                        str[len] = 0;
                        *( uint32_t* )data = string_new( str );
                        string_use( *( uint32_t* )data );
                        free( str );
                        data = ( uint8_t* )data + sizeof( uint32_t );
                        result += sizeof( uint32_t );
                    }
                }
                break;

            case TYPE_ARRAY:
                count *= var->Count[n];
                n++;
                continue;

            case TYPE_STRUCT:
                for ( ; count ; count-- )
                {
                    partial = loadvars( fp, data, dcb.varspace_vars[var->Members], dcb.varspace[var->Members].NVars, dcbformat );
                    result += partial;
                    data = ( uint8_t* )data + partial;
                }
                break;

            default:
                /* Can't be possible load this struct */
                return -1;
                break;
        }
        break;
    }
    return result;
}
