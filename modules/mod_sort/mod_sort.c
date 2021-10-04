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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bgddl.h"

#include "arrange.h"
#include "xstrings.h"
#include "dcb.h"

static int keyoffset = 0;

/*
 *  Comparison functions used by sort_variables
 */

static int compare_int( const int32_t * a, const int32_t * b )
{
    return *( int32_t * )(( uint8_t * )a + keyoffset ) - *( int32_t * )(( uint8_t * )b + keyoffset );
}

static int compare_dword( const uint32_t * a, const uint32_t * b )
{
    return *( uint32_t * )(( uint8_t * )a + keyoffset ) - *( uint32_t * )(( uint8_t * )b + keyoffset );
}

static int compare_word( const uint16_t * a, const uint16_t * b )
{
    return *( uint16_t * )(( uint8_t * )a + keyoffset ) - *( uint16_t * )(( uint8_t * )b + keyoffset );
}

static int compare_short( const int16_t * a, const int16_t * b )
{
    return *( int16_t * )(( uint8_t * )a + keyoffset ) - *( int16_t * )(( uint8_t * )b + keyoffset );
}

static int compare_byte( const uint8_t * a, const uint8_t * b )
{
    return *( uint8_t * )(( uint8_t * )a + keyoffset ) - *( uint8_t * )(( uint8_t * )b + keyoffset );
}

static int compare_sbyte( const int8_t * a, const int8_t * b )
{
    return *( int8_t * )(( uint8_t * )a + keyoffset ) - *( int8_t * )(( uint8_t * )b + keyoffset );
}

static int compare_float( const float * a, const float * b )
{
    return *( float * )(( uint8_t * )a + keyoffset ) - *( float * )(( uint8_t * )b + keyoffset );
}

static int compare_string( const int * a, const int * b )
{
    return strcmp( string_get( *( int * )(( uint8_t * )a + keyoffset ) ), string_get( *( int * )(( uint8_t * )b + keyoffset ) ) );
}

/*
 *  FUNCTION : dcb_typedef_reduce
 *
 *  Utility function to convert a DCB TYPEDEF (reduced type definition
 *  used in the DCB file instead of the full TYPEDEF of the compiler)
 *  to the definition of a pointer to itself.
 *
 *  Beware: it could cause a type to grow too much. In this case this
 *  function will fail with a printf and with no operation performed.
 *
 *  PARAMS:
 *      type            Pointer to the type to be converted
 *
 *  RETURN VALUE:
 *      None
 *
 */

static void dcb_typedef_enlarge( DCB_TYPEDEF * type )
{
    int i;

    for ( i = 0 ; i < MAX_TYPECHUNKS ; i++ )
    {
        if ( i != TYPE_POINTER && i != TYPE_ARRAY )
            break;
    }
    if ( i >= MAX_TYPECHUNKS - 1 )
    {
        /* Datatype too much complex */
        return;
    }
    for ( i = MAX_TYPECHUNKS - 1 ; i > 0 ; i-- )
    {
        type->BaseType[i] = type->BaseType[i-1];
        type->Count[i]    = type->Count[i-1];
    }
    type->BaseType[0] = TYPE_POINTER;
}

/*
 *  FUNCTION : dcb_typedef_reduce
 *
 *  Utility function to convert a DCB TYPEDEF (reduced type definition
 *  used in the DCB file instead of the full TYPEDEF of the compiler)
 *  to its pointed type.
 *
 *  PARAMS:
 *      type            Pointer to the type to be converted
 *
 *  RETURN VALUE:
 *      None
 *
 */

static void dcb_typedef_reduce( DCB_TYPEDEF * type )
{
    int i;

    for ( i = 0 ; i < MAX_TYPECHUNKS - 1 ; i++ )
    {
        type->BaseType[i] = type->BaseType[i+1];
        type->Count[i]    = type->Count[i+1];
    }
}

/*
 *  FUNCTION : dcb_typedef_size
 *
 *  Utility function to calculate the size of a variable using its
 *  DCB TYPEDEF (reduced type definition used in the DCB file)
 *
 *  PARAMS:
 *      type            Pointer to the type
 *
 *  RETURN VALUE:
 *      Size in bytes of the variable
 *
 */

static int dcb_typedef_size( const DCB_TYPEDEF * type )
{
    int i = 0;
    int count = 1;

    while ( type->BaseType[i] == TYPE_ARRAY )
    {
        count *= type->Count[i];
        i++;
    }
    switch ( type->BaseType[i] )
    {
        case TYPE_DWORD:
        case TYPE_INT:
        case TYPE_FLOAT:
        case TYPE_STRING:
        case TYPE_POINTER:
            return 4 * count;

        case TYPE_SHORT:
        case TYPE_WORD:
            return 2 * count;

        case TYPE_CHAR:
        case TYPE_SBYTE:
        case TYPE_BYTE:
            return count;

        case TYPE_STRUCT:
        {
            unsigned int maxoffset = 0;
            unsigned int n;
            DCB_TYPEDEF * maxvar = NULL;

            for ( n = 0 ; n < dcb.varspace[type->Members].NVars ; n++ )
            {
                if ( dcb.varspace_vars[type->Members][n].Offset > maxoffset )
                {
                    maxoffset = dcb.varspace_vars[type->Members][n].Offset;
                    maxvar    = &dcb.varspace_vars[type->Members][n].Type;
                }
            }
            if ( maxvar == NULL )
            {
                /* Void struct */
                return 0;
            }

            return count * maxoffset + dcb_typedef_size( maxvar );
        }

        default:
            /* Unknow datatype */
            return 0;
    }
}

/*
 *  FUNCTION : sort_variables
 *
 *  Sorting function used by all variants
 *
 *  PARAMS:
 *      data            Pointer to the start of the array
 *      key_offset      Offset of the key variable inside each element
 *      key_type        Basic type (like TYPE_INT) of the key variable
 *      element_size    Size of a single element
 *      elements        Number of elements to be sorted
 *
 *  RETURN VALUE:
 *      1 if succesful, 0 if error
 *
 */

static int sort_variables( void * data, int key_offset, int key_type, int element_size, int elements )
{
    int ( *compare )( const void *, const void * ) = NULL;

    keyoffset = key_offset;

    switch ( key_type )
    {
        case TYPE_INT:
            compare = ( int ( * )( const void*, const void* ) ) compare_int;
            break;

        case TYPE_WORD:
            compare = ( int ( * )( const void*, const void* ) ) compare_word;
            break;

        case TYPE_DWORD:
            compare = ( int ( * )( const void*, const void* ) ) compare_dword;
            break;

        case TYPE_SHORT:
            compare = ( int ( * )( const void*, const void* ) ) compare_short;
            break;

        case TYPE_BYTE:
            compare = ( int ( * )( const void*, const void* ) ) compare_byte;
            break;

        case TYPE_SBYTE:
            compare = ( int ( * )( const void*, const void* ) ) compare_sbyte;
            break;

        case TYPE_CHAR:
            compare = ( int ( * )( const void*, const void* ) ) compare_byte;
            break;

        case TYPE_STRING:
            compare = ( int ( * )( const void*, const void* ) ) compare_string;
            break;

        case TYPE_FLOAT:
            compare = ( int ( * )( const void*, const void* ) ) compare_float;
            break;

        default:
            /* key error, invalid datatype */
            return 0;
    }

    qsort( data, elements, element_size, compare );
    return 1;
}

/**
 *  SORT (variable)
 *  Sorts an array of structs or simple values. If a struct, uses the first
 *  variable as a key for sorting order.
 **/

static int modsort_sort( INSTANCE * my, int * params )
{
    /* Get the description of the data to be sorted */

    void *          data = ( void * )params[0];
    DCB_TYPEDEF *   type = ( DCB_TYPEDEF * )params[1];
    DCB_TYPEDEF     copy = *type;
    int             vars = params[2];
    int             element_size;

    /* Is it valid? */

    if ( type->BaseType[0] != TYPE_ARRAY )
    {
        /* Only struct or value arrays are allowed */
        return 0;
    }
    if ( vars > 1 )
    {
        /* Sort of a 1 element struct */
        return 0;
    }
    if ( type->Count[0] < 2 )
    {
        /* Sort of a 1 element array */
        return 0;
    }

    /* Get the key type */

    dcb_typedef_reduce( &copy );
    element_size = dcb_typedef_size( &copy );

    /* If an struct or array, get its first element */

    for ( ;; )
    {
        while ( copy.BaseType[0] == TYPE_ARRAY )
            dcb_typedef_reduce( &copy );
        if ( copy.BaseType[0] == TYPE_STRUCT )
            copy = dcb.varspace_vars[copy.Members][0].Type;
        else
            break;
    }

    /* Do the sorting */

    return sort_variables( data, 0, copy.BaseType[0], element_size, type->Count[0] );
}

/**
 *  KSORT (variable, key)
 *  Sorts an array of structs, using the given variable as a key
 **/

static int modsort_ksort( INSTANCE * my, int * params )
{
    /* Get the description of the data to be sorted */

    void *          data = ( void * )params[0];
    DCB_TYPEDEF *   type = ( DCB_TYPEDEF * )params[1];
    DCB_TYPEDEF     copy = *type;
    int             vars = params[2];
    int             element_size;

    void *          key_data = ( void * )params[3];
    DCB_TYPEDEF *   key_type = ( DCB_TYPEDEF * )params[4];

    /* Is it valid? */

    if ( type->BaseType[0] != TYPE_ARRAY )
    {
        /* Only struct or value arrays are allowed */
        return 0;
    }
    if ( vars > 1 )
    {
        /* Sort of a 1 element struct */
        return 0;
    }
    if ( type->Count[0] < 2 )
    {
        /* Sort of a 1 element array */
        return 0;
    }

    /* Check the key given */

    dcb_typedef_reduce( &copy );
    element_size = dcb_typedef_size( &copy );

    if (( uint8_t * )key_data > ( uint8_t* )data + element_size || key_data < data )
    {
        /* Sort using an external key to first element */
        return 0;
    }

    copy = *key_type;

    /* If an struct or array, get its first element */

    for ( ;; )
    {
        while ( copy.BaseType[0] == TYPE_ARRAY )
            dcb_typedef_reduce( &copy );
        if ( copy.BaseType[0] == TYPE_STRUCT )
            copy = dcb.varspace_vars[copy.Members][0].Type;
        else
            break;
    }

    /* Do the sorting */

    return sort_variables( data, ( uint8_t* )key_data - ( uint8_t* )data, copy.BaseType[0], element_size, type->Count[0] );
}

/**
 *  SORT (variable, count)
 *  Sorts an array of structs or simple values. If a struct, uses the first
 *  variable as a key for sorting order. It accepts arrays of one element
 *  or a pointer to an array, unlike the simple SORT version.
 **/

static int modsort_sort_n( INSTANCE * my, int * params )
{
    /* Get the description of the data to be sorted */

    void *          data = ( void * )params[0];
    DCB_TYPEDEF *   type = ( DCB_TYPEDEF * )params[1];
    DCB_TYPEDEF     copy = *type;
    int             vars = params[2];
    int             element_size;

    /* Is it valid? */

    if ( type->BaseType[0] == TYPE_POINTER )
    {
        dcb_typedef_reduce( &copy );
        data = *( void ** )data;
    }
    if ( copy.BaseType[0] != TYPE_ARRAY )
    {
        dcb_typedef_enlarge( &copy );
        copy.BaseType[0] = TYPE_ARRAY;
        copy.Count[0]    = 1;
    }
    if ( vars > 1 )
    {
        /* Sort of a 1 element struct */
        return 0;
    }

    /* Get the key type */

    dcb_typedef_reduce( &copy );
    element_size = dcb_typedef_size( &copy );

    /* If an struct or array, get its first element */

    for ( ;; )
    {
        while ( copy.BaseType[0] == TYPE_ARRAY )
            dcb_typedef_reduce( &copy );
        if ( copy.BaseType[0] == TYPE_STRUCT )
            copy = dcb.varspace_vars[copy.Members][0].Type;
        else
            break;
    }

    /* Do the sorting */

    return sort_variables( data, 0, copy.BaseType[0], element_size, params[3] );
}

/**
 *  KSORT (variable, key, elements)
 *  Sorts an array of structs, using the given variable as a key and
 *  limiting the sorting to the first n elements. Accepts pointers or
 *  single elements, unlike the previous version of KSORT above.
 **/

static int modsort_ksort_n( INSTANCE * my, int * params )
{
    /* Get the description of the data to be sorted */

    void *          data = ( void * )params[0];
    DCB_TYPEDEF *   type = ( DCB_TYPEDEF * )params[1];
    DCB_TYPEDEF     copy = *type;
    int             vars = params[2];
    int             element_size;

    void *          key_data = ( void * )params[3];
    DCB_TYPEDEF *   key_type = ( DCB_TYPEDEF * )params[4];

    /* Is it valid? */

    if ( type->BaseType[0] == TYPE_POINTER )
    {
        dcb_typedef_reduce( &copy );
        data = *( void ** )data;
    }
    if ( copy.BaseType[0] != TYPE_ARRAY )
    {
        /* If the type is not an array or a pointer, it is a
           pointer value already reduced by the bytecode */

        dcb_typedef_enlarge( &copy );
        copy.BaseType[0] = TYPE_ARRAY;
        copy.Count[0]    = 1;
    }
    if ( vars > 1 )
    {
        /* Sort of a 1 element struct */
        return 0;
    }

    /* Check the key given */

    dcb_typedef_reduce( &copy );
    element_size = dcb_typedef_size( &copy );

    if (( uint8_t * )key_data > ( uint8_t* )data + element_size || key_data < data )
    {
        /* Sort using an external key to first element */
        return 0;
    }

    copy = *key_type;

    /* If an struct or array, get its first element */

    for ( ;; )
    {
        while ( copy.BaseType[0] == TYPE_ARRAY )
            dcb_typedef_reduce( &copy );
        if ( copy.BaseType[0] == TYPE_STRUCT )
            copy = dcb.varspace_vars[copy.Members][0].Type;
        else
            break;
    }

    /* Do the sorting */

    return sort_variables( data, ( uint8_t* )key_data - ( uint8_t* )data, copy.BaseType[0], element_size, params[6] );
}

/*
 * Auxiliary QSort functions
 *
 */

static double GetData( uint8_t *Data, int pos, int *params )
{
    if ( params[4] == sizeof( uint8_t ) ) return Data[pos*params[1] + params[3]];
    if ( params[4] == sizeof( uint16_t ) ) return *( uint16_t * )( &Data[pos*params[1] + params[3]] );
    if ( params[4] == sizeof( int ) && params[5] == 0 ) return *( int * )( &Data[pos*params[1] + params[3]] );
    if ( params[4] == sizeof( float ) && params[5] == 1 ) return *( float * )( &Data[pos*params[1] + params[3]] );
    return 1 ;
}

static void QuickSort( uint8_t *Data, int inf, int sup, int *params )
{
    register int left, rigth;
    double middle;
    uint8_t* x = ( uint8_t* )malloc( params[1] );
    left = inf;
    rigth = sup;
    middle = GetData( Data, ( left + rigth ) >> 1, params );
    do
    {
        while ( GetData( Data, left, params ) < middle && left < sup ) left++;
        while ( middle < GetData( Data, rigth, params ) && rigth > inf ) rigth--;

        if ( left <= rigth )
        {
            memcpy( x, &Data[left*params[1]], params[1] );
            memcpy( &Data[left*params[1]], &Data[rigth*params[1]], params[1] );
            memcpy( &Data[rigth*params[1]], x, params[1] );
            left++;
            rigth--;
        }
    }
    while ( left <= rigth );

    if ( inf < rigth ) QuickSort( Data, inf, rigth, params );
    if ( left < sup ) QuickSort( Data, left, sup, params );
}

/*
 *  QSort:
 *      pointer to array,
 *      sizeof data,
 *      numbers of data,
 *      offset key,
 *      sizeof key,
 *      datatype (int=0, float=1)
 */

static int modsort_quicksort( INSTANCE *my, int *params )
{

    uint8_t *Data = ( uint8_t * )params[0];
    QuickSort( Data, 0, params[2] - 1, params );
    return 1 ;
}

/* ---------------------------------------------------------------------- */

DLSYSFUNCS  __bgdexport( mod_sort, functions_exports )[] =
{
    /* Funciones sort */
    { "QUICKSORT"   , "PIIIBB", TYPE_INT    , modsort_quicksort },
    { "KSORT"       , "V++V++", TYPE_INT    , modsort_ksort     },
    { "KSORT"       , "V++V++I", TYPE_INT    , modsort_ksort_n   },
    { "SORT"        , "V++I"  , TYPE_INT    , modsort_sort_n    },
    { "SORT"        , "V++"   , TYPE_INT    , modsort_sort      },
    { 0             , 0       , 0           , 0                 }
};
