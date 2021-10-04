/*
 *  Copyright � 2006-2012 SplinterGU (Fenix/Bennugd)
 *  Copyright � 2002-2006 Fenix Team (Fenix)
 *  Copyright � 1999-2002 Jos� Luis Cebri�n Pag�e (Fenix)
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

/****************************************************************************/
/* FILE        : strings.c                                                  */
/* DESCRIPTION : Strings management. Includes any function related to       */
/*               variable-length strings. Those strings are allocated       */
/*               in dynamic memory with reference counting.                 */
/****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef TARGET_BEOS
#include <posix/assert.h>
#else
#include <assert.h>
#endif

#include "files.h"
#include "xctype.h"

/****************************************************************************/

#define BLOCK_INCR  1024

#define bit_set(m,b)    (((uint32_t *)(m))[(b)>>5] |=   1<<((b)&0x1F))
#define bit_clr(m,b)    (((uint32_t *)(m))[(b)>>5] &= ~(1<<((b)&0x1F)))
#define bit_tst(m,b)    (((uint32_t *)(m))[(b)>>5] &   (1<<((b)&0x1F)))

/****************************************************************************/
/* STATIC VARIABLES :                                                       */
/****************************************************************************/

/* Fixed string memory. The DCB fixed strings are stored here */
static char     * string_mem = NULL ;

static int      string_reserved = 0;        /* Last fixed string */

static char     ** string_ptr = NULL ;      /* Pointers to each string's text. Every string is allocated using strdup() or malloc().
                                               A pointer of a unused slot is 0.
                                               Exception: "fixed" strings are stored in a separate memory block and should not be freed */
static uint32_t * string_uct = NULL ;       /* Usage count for each string. An unused slot has a count of 0 */

static uint32_t * string_bmp = NULL ;       /* Bitmap for speed up string creation, and reused freed slots */

static int      string_allocated = 0 ;      /* How many string slots are available in the ptr, uct and dontfree arrays */

static int      string_bmp_start = 0 ;      /* Offset of assignable string for reused (32bits each one) */

static int      string_last_id = 1 ;        /* How many strings slots are used. This is only the bigger id in use + 1.
                                               There may be unused slots in this many positions */

/* --------------------------------------------------------------------------- */

void _string_ptoa( char *t, void * ptr )
{
    unsigned char c ;
    int p = ( int ) ptr;

    c = ((( p ) & 0xf0000000 ) >> 28 );
    *t++ = ( c > 9 ? '7' : '0' ) + c; /* '7' + 10 = 'A' */

    c = ((( p ) & 0x0f000000 ) >> 24 );
    *t++ = ( c > 9 ? '7' : '0' ) + c; /* '7' + 10 = 'A' */

    c = ((( p ) & 0x00f00000 ) >> 20 );
    *t++ = ( c > 9 ? '7' : '0' ) + c; /* '7' + 10 = 'A' */

    c = ((( p ) & 0x000f0000 ) >> 16 );
    *t++ = ( c > 9 ? '7' : '0' ) + c; /* '7' + 10 = 'A' */

    c = ((( p ) & 0x0000f000 ) >> 12 );
    *t++ = ( c > 9 ? '7' : '0' ) + c; /* '7' + 10 = 'A' */

    c = ((( p ) & 0x00000f00 ) >>  8 );
    *t++ = ( c > 9 ? '7' : '0' ) + c; /* '7' + 10 = 'A' */

    c = ((( p ) & 0x000000f0 ) >>  4 );
    *t++ = ( c > 9 ? '7' : '0' ) + c; /* '7' + 10 = 'A' */

    c = ( p ) & 0x0000000f;
    *t++ = ( c > 9 ? '7' : '0' ) + c; /* '7' + 10 = 'A' */

    *t = '\0';
}

/* --------------------------------------------------------------------------- */

void _string_ntoa( char *p, unsigned long n )
{
    char * i = p ;

    p += 10;
    if (( long ) n < 0 )
    {
        * i++ = '-';
        p++ ;
        n = ( unsigned long )( -( long )n ) ;
    }

    * p = '\0';
    do
    {
        * --p = '0' + ( n % 10 );
    }
    while ( n /= 10 ) ;

    if ( p > i ) while (( *i++ = *p++ ) ) ;
}

/* --------------------------------------------------------------------------- */

void _string_utoa( char *p, unsigned long n )
{
    char * i = p ;

    p += 10;

    * p = '\0';
    do
    {
        * --p = '0' + ( n % 10 );
    }
    while ( n /= 10 ) ;

    if ( p > i ) while (( *i++ = *p++ ) ) ;
}

/* --------------------------------------------------------------------------- */

/****************************************************************************/
/* FUNCTION : string_alloc                                                  */
/****************************************************************************/
/* int bytes: how many new strings we could need                            */
/****************************************************************************/
/* Increase the size of the internal string arrays. This limits how many    */
/* strings you can have in memory at the same time, and this should be      */
/* called when every identifier slot available is already used.             */
/****************************************************************************/

static void string_alloc( int count )
{
    int lim = ( string_allocated >> 5 ) ;

    count = (( count >> 5 ) + 1 ) << 5 ;

    string_allocated += count ;

    string_ptr = ( char ** ) realloc( string_ptr, string_allocated * sizeof( char * ) ) ;
    string_uct = ( uint32_t * ) realloc( string_uct, string_allocated * sizeof( uint32_t ) ) ;
    string_bmp = ( uint32_t * ) realloc( string_bmp, ( string_allocated >> 5 ) * sizeof( uint32_t ) );

    if ( !string_ptr || !string_uct || !string_bmp )
    {
        fprintf( stderr, "ERROR: Runtime error - string_alloc: out of memory\n" ) ;
        exit( 0 );
    }

    memset( &string_bmp[ lim ], '\0', ( count >> 5 ) * sizeof ( uint32_t ) );
}

/****************************************************************************/
/* FUNCTION : string_init                                                   */
/****************************************************************************/
/* Allocate memory for the dynamic arrays. You should call this function    */
/* before anything else in this file. There is enough space for about       */
/* BLOCK_INCR short strings, that should be enough for simple programs.     */
/* More space is allocated as needed.                                       */
/****************************************************************************/

void string_init()
{
    string_alloc( BLOCK_INCR );

    /* Create an empty string with ID 0 */

    string_last_id = 0;
    string_reserved = 0;
    string_bmp_start = 0;
}

/****************************************************************************/
/* FUNCTION : string_dump                                                   */
/****************************************************************************/
/* Shows all the strings in memory in the console, including the reference  */
/* count (usage count) of each string.                                      */
/****************************************************************************/

void string_dump( void ( *wlog )( const char *fmt, ... ) )
{
    int i ;
    int used = 0;

    if ( wlog )
        wlog( "[STRING] ---- Dumping MaxID=%d strings ----\n", string_allocated ) ;
    else
        printf( "[STRING] ---- Dumping MaxID=%d strings ----\n", string_allocated ) ;

    for ( i = 0; i < string_allocated; i++ )
    {
        if ( string_ptr[i] )
        {
            if ( !string_uct[i] )
            {
                if ( i >= string_reserved )
                {
                    free( string_ptr[i] ) ;
                    string_ptr[i] = NULL ;
                    bit_clr( string_bmp, i );
                }
                continue ;
            }
            used++;
            if ( wlog )
                wlog( "[STRING] %4d [%4d]%s: {%s}\n", i, string_uct[i], ( i >= string_reserved ) ? "" : " STATIC", string_ptr[i] ) ;
            else
                printf( "[STRING] %4d [%4d]%s: {%s}\n", i, string_uct[i], ( i >= string_reserved ) ? "" : " STATIC", string_ptr[i] ) ;
        }
        else
        {
            continue ;
        }
    }
    if ( wlog )
        wlog( "[STRING] ---- Dumping Used=%d End ----\n", used ) ;
    else
        printf( "[STRING] ---- Dumping Used=%d End ----\n", used ) ;
}

/****************************************************************************/
/* FUNCTION : string_get                                                    */
/****************************************************************************/
/* int code: identifier of the string you want                              */
/****************************************************************************/
/* Returns the contens of an string. Beware: this pointer with only be      */
/* valid while no other string function is called.                          */
/****************************************************************************/

const char * string_get( int code )
{
    assert( code < string_allocated && code >= 0 ) ;
    return string_ptr[code] ;
}

/****************************************************************************/
/* FUNCTION : string_load                                                   */
/****************************************************************************/
/* file * fp: the DCB file (must be opened)                                 */
/*                                                                          */
/* This function uses the global "dcb" struct. It should be already filled. */
/****************************************************************************/
/* Loads the string portion of a DCB file. This includes an area with all   */
/* the text (that will be stored in the string_mem pointer) and an array of */
/* the offsets of every string. This function fills the internal arrayswith */
/* all this data and allocates memory if needed.                            */
/****************************************************************************/

void string_load( void * fp, int ostroffs, int ostrdata, int nstrings, int totalsize )
{
    uint32_t * string_offset;
    int n;

    string_mem = malloc( totalsize );
    assert( string_mem );

    string_offset = ( uint32_t * ) malloc( sizeof( uint32_t ) * nstrings ) ;
    assert( string_offset );

    file_seek(( file * )fp, ostroffs, SEEK_SET ) ;
    file_readUint32A(( file * )fp, string_offset, nstrings ) ;

    if ( string_last_id + nstrings > string_allocated )
        string_alloc((( string_last_id + nstrings - string_allocated ) / BLOCK_INCR + 1 ) * BLOCK_INCR ) ;

    file_seek(( file * )fp, ostrdata, SEEK_SET ) ;
    file_read(( file * )fp, string_mem, totalsize ) ;

    for ( n = 0 ; n < nstrings ; n++ )
    {
        string_ptr[string_last_id + n] = string_mem + string_offset[n] ;
        string_uct[string_last_id + n] = 0 ;
        bit_set( string_bmp, string_last_id + n );
    }

    string_last_id += nstrings ;

    string_last_id = ( string_last_id + 32 ) & ~0x1F;

    string_reserved = string_last_id ;
    string_bmp_start = string_last_id >> 5;

    free( string_offset ) ;
}

/****************************************************************************/
/* FUNCTION : string_use                                                    */
/****************************************************************************/
/* int code: identifier of the string you are using                         */
/****************************************************************************/
/* Increase the usage counter of an string. Use this when you store the     */
/* identifier of the string somewhere.                                      */
/****************************************************************************/

void string_use( int code )
{
    string_uct[code]++ ;
}

/****************************************************************************/
/* FUNCTION : string_discard                                                */
/****************************************************************************/
/* int code: identifier of the string you don't need anymore                */
/****************************************************************************/
/* Decrease the usage counter of an string. Use this when you retrieve the  */
/* identifier of the string and discard it, or some memory (like private    */
/* variables) containing the string identifier is destroyed. If the usage   */
/* count is decreased to zero, the string will be discarted, and the        */
/* identifier may be used in the future by other string.                    */
/****************************************************************************/

void string_discard( int code )
{
    if ( code < 0 || code > string_allocated || !string_ptr[code] ) return;

    if ( !string_uct[code] ) return ;

    string_uct[code]-- ;

    if ( !string_uct[code] )
    {
        if ( code >= string_reserved )
        {
            free( string_ptr[code] ) ;
            string_ptr[code] = NULL ;
            bit_clr( string_bmp, code );
        }
    }
}

/****************************************************************************/
/* FUNCTION : string_getid                                                  */
/****************************************************************************/
/* Searchs for an available ID and returns it. If none available, more space*/
/* is allocated for the new string. This is used for new strings only.      */
/****************************************************************************/

static int string_getid()
{
    int n, nb, lim, ini ;

    /* Si tengo suficientes alocados, retorno el siguiente segun string_last_id */
    if ( string_last_id < string_allocated )
    {
        if ( !bit_tst( string_bmp, string_last_id ) )
        {
            bit_set( string_bmp, string_last_id );
            return string_last_id++ ;
        }
    }

    /* Ya no tengo mas espacio, entonces busco alguno libre entre ~+32 desde el ultimo fijo y ~-32 del ultimo asignado */

    ini = ( string_last_id < string_allocated ) ? ( string_last_id >> 5 ) : string_reserved ;
    lim = ( string_allocated >> 5 ) ;

    while ( 1 )
    {
        for ( n = ini; n < lim ; n++ )
        {
            if ( string_bmp[n] != ( uint32_t ) 0xFFFFFFFF ) /* Aca hay 1 libre, busco cual es */
            {
                for ( nb = 0; nb < 32; nb++ )
                {
                    if ( !bit_tst( string_bmp + n, nb ) )
                    {
                        string_last_id = ( n << 5 ) + nb ;
                        bit_set( string_bmp, string_last_id );
                        return string_last_id++ ;
                    }
                }
            }
        }
        if ( ini == string_reserved ) break;
        lim = ini;
        ini = string_reserved;
    }

    string_last_id = string_allocated ;

    /* Incremento espacio, no habia libres */
    string_alloc( BLOCK_INCR ) ;

    assert( !bit_tst( string_bmp, string_last_id ) );

    /* Devuelvo string_last_id e incremento en 1, ya que ahora tengo BLOCK_INCR mas que antes */
    bit_set( string_bmp, string_last_id );
    return string_last_id++ ;
}

/****************************************************************************/
/* FUNCTION : string_new                                                    */
/****************************************************************************/
/* Create a new string. It returns its ID. Note that it uses strdup()       */
/* TODO: do something if no memory available                                */
/****************************************************************************/

int string_new( const char * ptr )
{
    char * str = strdup( ptr ) ;
    int    id ;

    assert( str ) ;

    id = string_getid() ;

    string_ptr[id] = str ;
    string_uct[id] = 0 ;

    return id ;
}

/*
 *  FUNCTION : string_newa
 *
 *  Create a new string from a text buffer section
 *
 *  PARAMS:
 *              ptr         Pointer to the text buffer at start position
 *              count       Number of characters
 *
 *  RETURN VALUE:
 *      ID of the new string
 */

int string_newa( const char * ptr, unsigned count )
{
    char * str = malloc( count + 1 );
    int    id ;

    assert( str ) ;
    id = string_getid() ;

    strncpy( str, ptr, count );
    str[count] = '\0';
    string_ptr[id] = str ;
    string_uct[id] = 0 ;

    return id ;
}

/****************************************************************************/
/* FUNCTION : string_concat                                                 */
/****************************************************************************/
/* Add some text to an string and return the resulting string. This does not*/
/* modify the original string, but creates a new one.                       */
/****************************************************************************/

int string_concat( int code1, char * str2 )
{
    char * str1 ;
    int len1, len2;

    assert( code1 < string_allocated && code1 >= 0 ) ;

    str1 = string_ptr[code1] ;
    assert( str1 ) ;

    len1 = strlen( str1 ) ;
    len2 = strlen( str2 ) + 1 ;

    str1 = ( char * ) realloc( str1, len1 + len2 ) ;
    assert( str1 ) ;

    memmove( str1 + len1, str2, len2 ) ;

    string_ptr[code1] = str1 ;

    return code1 ;
}

/****************************************************************************/
/* FUNCTION : string_add                                                    */
/****************************************************************************/
/* Add an string to another one and return the resulting string. This does  */
/* not modify the original string, but creates a new one.                   */
/****************************************************************************/

int string_add( int code1, int code2 )
{
    const char * str1 = string_get( code1 ) ;
    const char * str2 = string_get( code2 ) ;
    char * str3 ;
    int id ;
    int len1, len2;

    assert( str1 ) ;
    assert( str2 ) ;

    len1 = strlen( str1 ) ;
    len2 = strlen( str2 ) + 1;

    str3 = ( char * ) malloc( len1 + len2 ) ;
    assert( str3 ) ;

    memmove( str3, str1, len1 ) ;
    memmove( str3 + len1, str2, len2 ) ;

    id = string_getid() ;

    string_ptr[id] = str3 ;
    string_uct[id] = 0 ;

    return id ;
}

/****************************************************************************/
/* FUNCTION : string_ptoa                                                   */
/****************************************************************************/
/* Convert a pointer to a new created string and return its ID.             */
/****************************************************************************/

int string_ptoa( void * n )
{
    char * str ;
    int id ;

    str = ( char * ) malloc( 10 ) ;
    assert( str )  ;

    _string_ptoa( str, n ) ;

    id = string_getid() ;
    string_ptr[id] = str ;
    string_uct[id] = 0 ;

    return id ;
}

/****************************************************************************/
/* FUNCTION : string_ftoa                                                   */
/****************************************************************************/
/* Convert a float to a new created string and return its ID.               */
/****************************************************************************/

int string_ftoa( float n )
{
    char * str = ( char * ) malloc( 32 ), * ptr = str;
    int id ;

    assert( str )  ;

    ptr += sprintf( str, "%f", n ) - 1;

    while ( ptr >= str )
    {
        if ( *ptr != '0' ) break ;
        *ptr-- = 0 ;
    }
    if ( ptr >= str && *ptr == '.' ) *ptr = 0 ;
    if ( *str == 0 )
    {
        *str = '0';
        *( str + 1 ) = '\0';
    }

    id = string_getid() ;
    string_ptr[id] = str ;
    string_uct[id] = 0 ;

    return id ;
}

/****************************************************************************/
/* FUNCTION : string_itoa                                                   */
/****************************************************************************/
/* Convert an integer to a new created string and return its ID.            */
/****************************************************************************/

int string_itoa( int n )
{
    char * str ;
    int id ;

    str = ( char * ) malloc( 16 ) ;
    assert( str )  ;

    _string_ntoa( str, n ) ;

    id = string_getid() ;
    string_ptr[id] = str ;
    string_uct[id] = 0 ;

    return id ;
}

/****************************************************************************/
/* FUNCTION : string_uitoa                                                  */
/****************************************************************************/
/* Convert an unsigned integer to a new created string and return its ID.   */
/****************************************************************************/

int string_uitoa( unsigned int n )
{
    char * str ;
    int id ;

    str = ( char * ) malloc( 16 ) ;
    assert( str )  ;

    _string_utoa( str, n ) ;

    id = string_getid() ;
    string_ptr[id] = str ;
    string_uct[id] = 0 ;

    return id ;
}

/****************************************************************************/
/* FUNCTION : string_comp                                                   */
/****************************************************************************/
/* Compare two strings using strcmp and return the result                   */
/****************************************************************************/

int string_comp( int code1, int code2 )
{
    const char * str1 = string_get( code1 ) ;
    const char * str2 = string_get( code2 ) ;

//    SDL_Log("Told to compare \"%s\" <-> \"%s\"\n", str1, str2);

    return strcmp( str1, str2 ) ;
}

/****************************************************************************/
/* FUNCTION : string_char                                                   */
/****************************************************************************/
/* Extract a character from a string. The parameter nchar can be:           */
/* - 0 or positive: return this character from the left (0 = leftmost)      */
/* - negative: return this character from the right (-1 = rightmost)        */
/* The result is not the ASCII value, but the identifier of a new string    */
/* that is create in the process and contains only the extracted character  */
/****************************************************************************/

int string_char( int n, int nchar )
{
    const char * str = string_get( n ) ;

    assert( str ) ;

    if ( nchar < 0 )
    {
        nchar = strlen( str ) + nchar ;
        if ( nchar < 0 ) return 0 ;
    }

    return str[nchar] ;
}

/****************************************************************************/
/* FUNCTION : string_substr                                                 */
/****************************************************************************/
/* Extract a substring from a string. The parameters can be:                */
/* - 0 or positive: count this character from the left (0 = leftmost)       */
/* - negative: count this character from the right (-1 = rightmost)         */
/*                                                                          */
/* NO MORE: If first > last, the two values are swapped before returning the result  */
/****************************************************************************/

int string_substr( int code, int first, int len )
{
    const char * str = string_get( code ) ;
    char       * ptr ;
    int          rlen, n ;

    assert( str ) ;
    rlen = strlen( str ) ;

    if ( first < 0 )
    {
        first = rlen + first ;
        if ( first < 0 ) return string_new( "" ) ;
    }
    else
        if ( first > ( rlen - 1 ) ) return string_new( "" ) ;

    if ( len < 0 )
    {
        len = rlen + ( len + 2 ) - first - 1 ;
        if ( len < 1 ) return string_new( "" ) ;
    }

    if (( first + len ) > rlen ) len = ( rlen - first ) ;

    ptr = ( char * )malloc( len + 1 ) ;
    memcpy( ptr, str + first, len ) ;
    ptr[len] = '\0' ;

    n = string_getid() ;
    string_ptr[n] = ptr ;
    string_uct[n] = 0 ;

    return n ;
}

/*
 *  FUNCTION : string_find
 *
 *  Find a substring. Returns the position of the leftmost character (0
 *  for the leftmost position) or -1 if the string was not found.
 *
 *  PARAMS:
 *              code1                   Code of the string
 *              code2                   Code of the substring
 *              first                   Character to start the search
 *                                      (negative to search backwards)
 *
 *  RETURN VALUE:
 *      Result of the comparison
 */

int string_find( int code1, int code2, int first )
{
    char * str1 = ( char * ) string_get( code1 ) ;
    char * str2 = ( char * ) string_get( code2 ) ;
    char * p = str1, * p1, * p2 ;

    assert( str1 && str2 ) ;

    if ( first < 0 )
    {
        first += strlen( str1 ) ;
        if ( first < 0 ) return -1;
        str1 += first;
    }
    else
    {
        /* Avoid use strlen */
        while ( first-- && *str1 ) str1++;
        if ( !*str1 ) return -1;
    }

    while ( *str1 )
    {
        if ( *str1 == *str2 )
        {
            p1 = str1 + 1;
            p2 = str2 + 1;

            while ( *p1 && *p2 && *p1 == *p2 )
            {
                p1++;
                p2++;
            }
            if ( !*p2 ) return str1 - p;
        }
        str1++;
    }

    return -1 ;
}

/*
 *  FUNCTION : string_ucase
 *
 *  Convert an string to upper case. It does not alter the given string, but
 *  creates a new string in the correct case and returns its id.
 *
 *  PARAMS:
 *              code                    Internal code of original string
 *
 *  RETURN VALUE:
 *      Code of the resulting string
 */

int string_ucase( int code )
{
    const char * str = string_get( code ) ;
    char       * base, * ptr ;
    int          id ;

    assert( str ) ;

    base = ( char * )malloc( strlen( str ) + 1 ) ;
    assert( base ) ;

    for ( ptr = base; *str ; ptr++, str++ ) *ptr = TOUPPER( *str ) ;
    ptr[0] = '\0' ;

    id = string_getid() ;
    string_ptr[id] = base ;
    string_uct[id] = 0 ;

    return id ;
}

/*
 *  FUNCTION : string_lcase
 *
 *  Convert an string to lower case. It does not alter the given string, but
 *  creates a new string in the correct case and returns its id.
 *
 *  PARAMS:
 *              code                    Internal code of original string
 *
 *  RETURN VALUE:
 *      Code of the resulting string
 */

int string_lcase( int code )
{
    const char * str = string_get( code ) ;
    char       * base, * ptr ;
    int          id ;

    assert( str ) ;

    base = ( char * )malloc( strlen( str ) + 1 ) ;
    assert( base ) ;

    for ( ptr = base; *str ; ptr++, str++ ) *ptr = TOLOWER( *str ) ;
    ptr[0] = '\0' ;

    id = string_getid() ;
    string_ptr[id] = base ;
    string_uct[id] = 0 ;

    return id ;
}

/*
 *  FUNCTION : string_strip
 *
 *  Create a copy of a string, without any leading or ending blanks
 *
 *  PARAMS:
 *              code                    Internal code of original string
 *
 *  RETURN VALUE:
 *      Code of the resulting string
 */

int string_strip( int code )
{
    const char * str = string_get( code ) ;
    char * base, * ptr;
    int id = string_new( str );

    ptr = base = ( char * )string_get( id ) ;

    assert( ptr );

    while ( *str == ' ' || *str == '\n' || *str == '\r' || *str == '\t' ) str++;
    while ( *str ) *ptr++ = *str++;
    while ( ptr > base && ( ptr[-1] == ' ' || ptr[-1] == '\n' || ptr[-1] == '\r' || ptr[-1] == '\t' ) ) ptr--;
    *ptr = '\0';

    return id ;
}

/*
 *  FUNCTION : string_format
 *
 *  Format a number using the given characters
 *
 *  PARAMS:
 *              number                  Number to format
 *              spec                    Format specification
 *
 *  RETURN VALUE:
 *      Code of the resulting string
 */

int string_format( double number, int dec, char point, char thousands )
{
    char * str = malloc( 128 );
    char * s = str, * t, * p = NULL;
    int c, id, neg ;

    assert( str );

    if ( dec == -1 )
        s += sprintf( str, "%f", number );
    else
        s += sprintf( str, "%.*f", dec, number );

    neg = (*str == '-') ? 1 : 0;

    if ( dec )
    {
        p = str;
        while ( *p && *p != '.' ) p++;
        if ( *p ) *p = point;
    }
    else
    {
        p = s;
    }

    /* p = where decimal point is */
    /* s = where '\0' is */

    if ( thousands )
        t = s + (p - (str + neg) - 1 ) / 3;
    else
        t = s;

    c = 0;
    while ( s >= str )
    {
        if ( isdigit( *s ) && s < p )
        {
            if ( c == 3 )
            {
                *t-- = thousands ;
                c = 0;
                continue;
            }
            else
                c++;
        }
        *t-- = *s-- ;
    }

    id = string_getid() ;

    string_ptr[id] = str ;
    string_uct[id] = 0 ;

    return id;
}

/*
 *  FUNCTION : string_casecmp
 *
 *  Compare two strings (case-insensitive version)
 *
 *  PARAMS:
 *              code1                   Code of the first string
 *              code2                   Code of the second string
 *
 *  RETURN VALUE:
 *      Result of the comparison
 */

int string_casecmp( int code1, int code2 )
{
    unsigned char * str1 = ( unsigned char * ) string_get( code1 ) ;
    unsigned char * str2 = ( unsigned char * ) string_get( code2 ) ;

    while ( *str1 || *str2 )
    {
        if ( TOUPPER( *str1 ) != TOUPPER( *str2 ) ) return TOUPPER( *str1 ) - TOUPPER( *str2 );

        str1++;
        str2++;
    }

    return 0 ;
}

/*
 *  FUNCTION : string_pad
 *
 *  Add spaces to the left or right of a string
 *
 *  PARAMS:
 *              code                    Code of the string
 *              total                   Total length of the final string
 *              align                   0 = align to the right; 1 = align to the left
 *
 *  RETURN VALUE:
 *      Result of the comparison
 */

int string_pad( int code, int total, int align )
{
    const char * ptr = string_get( code );

    int    len;
    int    spaces = 0;
    int    id;
    char * str;

    assert( ptr );
    len = strlen( ptr );
    if ( len < total ) spaces = total - len;

    if ( !spaces ) return string_new( ptr ) ;

    str = malloc( total + 1 );
    assert( str );

    if ( !align )
    {
        memset( str, ' ', spaces );
        strcpy( str + spaces, ptr ) ;
    }
    else
    {
        strcpy( str, ptr ) ;
        memset( str + len, ' ', spaces ) ;
        str[total] = '\0';
    }

    id = string_getid() ;
    string_ptr[id] = str ;
    string_uct[id] = 0 ;

    return id ;
}
