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

/****************************************************************************/
/* FILE        : xstrings.h                                                 */
/* DESCRIPTION : Strings management. Includes any function related to       */
/*               variable-length strings.                                   */
/****************************************************************************/
/* HISTORY     : 29/01/2001 (jlceb) string_internal no longer used          */
/****************************************************************************/

#ifndef __XSTRINGS_H
#define __XSTRINGS_H

extern void _string_ptoa( char *t, void * p );
extern void _string_ntoa( char *p, unsigned long n );
extern void _string_utoa( char *p, unsigned long n );

extern void         string_init() ;
extern const char * string_get( int code ) ;
extern void         string_dump( void ( *wlog )( const char *fmt, ... ) );
extern void         string_load( void *, int, int, int, int ) ;
extern int          string_new( const char * ptr ) ;
extern int          string_newa( const char * ptr, unsigned count ) ;
extern void         string_use( int code ) ;
extern void         string_discard( int code ) ;
extern int          string_add( int code1, int code2 ) ;
extern int          string_compile( const char ** source ) ;
extern int          string_itoa( int n ) ;
extern int          string_uitoa( unsigned int n ) ;
extern int          string_ftoa( float n ) ;
extern int          string_ptoa( void * n ) ;
extern int          string_comp( int code1, int code2 ) ;
extern int          string_casecmp( int code1, int code2 ) ;
extern int          string_char( int n, int nchar ) ;
extern int          string_substr( int code, int first, int len ) ;
extern int          string_find( int code1, int code2, int first ) ;
extern int          string_ucase( int code1 ) ;
extern int          string_lcase( int code1 ) ;
extern int          string_strip( int code ) ;
extern int          string_pad( int code, int length, int align ) ;
extern int          string_format( double number, int dec, char point, char thousands ) ;
extern int          string_concat( int code1, char * str2 ) ;

#endif
