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

/* --------------------------------------------------------------------------- */

#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <math.h>

#include "libblit.h"

/* --------------------------------------------------------------------------- */

/* Define some constants and structs used by the blitter */

typedef struct
{
    int x, y;
}
_POINT;

typedef struct
{
    float x, y;
}
_POINTF;

typedef struct
{
    int x;
    int y;
    float s;
    float t;
}
VERTEX;

/* --------------------------------------------------------------------------- */

typedef _POINTF VECTOR;

typedef void ( DRAW_SPAN )( GRAPH *, GRAPH *, int, int, int, int, int, int, int );
typedef void ( DRAW_HSPAN )( void *, void *, int, int, int, int, int );
typedef int ( BLEND_FUNC )( int, int );

/* --------------------------------------------------------------------------- */

/* Conversion tables used by transparency /blending
 *
 * In 16 bits, this two lookup tables will be used as:
 *
 *      Dest_color = ghost1[screen_color] + ghost2[graphic_color]
 *
 * In transparency mode, both tables are the assigned to the ghostcolor global table
 * (a table that reduces all color components to half)
 */

uint16_t  * ghost1;
uint16_t  * ghost2;
uint8_t   * ghost8;

uint32_t * pcolorequiv = NULL ;

uint32_t _factor ;
uint32_t _factor2 ;

/* --------------------------------------------------------------------------- */

/* Calculates additive blend value */

static BLEND_FUNC * blend_func;

/* --------------------------------------------------------------------------- */

static int substractive_blend8( int A, int B )
{
    int32_t r, g, b, r2, g2, b2;

    gr_get_rgb( B, &r, &g, &b );
    gr_get_rgb( A, &r2, &g2, &b2 );

    r += ( r2 - 256 ); if ( r < 0 ) r = 0;
    g += ( g2 - 256 ); if ( g < 0 ) g = 0;
    b += ( b2 - 256 ); if ( b < 0 ) b = 0;

    return gr_find_nearest_color( r, g, b );
}

static int substractive_blend16( int A, int B )
{
    int32_t r, g, b;

    r = ( B & 0x0000F800 ) + (( A & 0x0000F800 ) - 0x00010000 );
    g = ( B & 0x000007e0 ) + (( A & 0x000007e0 ) - 0x00000800 );
    b = ( B & 0x0000001f ) + (( A & 0x0000001f ) - 0x00000020 );

    if ( r & 0xFFFF07FF ) r = 0 ;
    if ( g & 0xFFFFF81F ) g = 0 ;
    if ( b & 0xFFFFFFE0 ) b = 0 ;

    return ( r | g | b );
}

static int substractive_blend32( int A, int B )
{
    uint32_t r, g, b/*, a*/;

    /* NOTE: ~ and << are expensive */

    r = ( B & 0x00ff0000 ) + ( A & 0x00ff0000 ) - 0x01000000 ;
    g = ( B & 0x0000ff00 ) + ( A & 0x0000ff00 ) - 0x00010000 ;
    b = ( B & 0x000000ff ) + ( A & 0x000000ff ) - 0x00000100 ;
//    a = ( B & 0xff000000 ) + ( A & 0xff000000 ) - 0x00000001 ;

    if ( r & 0xFF00FFFF ) r = 0 ;
    if ( g & 0xFFFF00FF ) g = 0 ;
    if ( b & 0xFFFFFF00 ) b = 0 ;
//    if ( a & 0x00FFFFFF ) a = 0 ;

    return ( r | g | b /*| a*/ );
}

static int additive_blend8( int A, int B )
{
    int32_t r, g, b, r2, g2, b2;

    gr_get_rgb( B, &r, &g, &b );
    gr_get_rgb( A, &r2, &g2, &b2 );

    r += r2; if ( r > 255 ) r = 255;
    g += g2; if ( g > 255 ) g = 255;
    b += b2; if ( b > 255 ) b = 255;

    return gr_find_nearest_color( r, g, b );
}

static int additive_blend16( int A, int B )
{
    int32_t r, g, b;

    r = ( B & 0x0000F800 ) + ( A & 0x0000F800 ) ;
    g = ( B & 0x000007e0 ) + ( A & 0x000007e0 ) ;
    b = ( B & 0x0000001f ) + ( A & 0x0000001f ) ;

    if ( r & 0xFFFF07FF ) r = 0x0000F800 ;
    if ( g & 0xFFFFF81F ) g = 0x000007e0 ;
    if ( b & 0xFFFFFFE0 ) b = 0x0000001f ;

    return ( r | g | b );
}

static int additive_blend32( int A, int B )
{
    uint32_t r, g, b/*, a*/;

    r = ( B & 0x00ff0000 ) + ( A & 0x00ff0000 ) ;
    g = ( B & 0x0000ff00 ) + ( A & 0x0000ff00 ) ;
    b = ( B & 0x000000ff ) + ( A & 0x000000ff ) ;
//    a = ( B & 0xff000000 ) + ( A & 0xff000000 ) ;

    if ( r & 0xFF00FFFF ) r = 0x00ff0000 ;
    if ( g & 0xFFFF00FF ) g = 0x0000ff00 ;
    if ( b & 0xFFFFFF00 ) b = 0x000000ff ;
//    if ( a & 0x00FFFFFF ) b = 0xFF000000 ;

    return ( r | g | b /*| a*/ );
}

/* Routine to sort vertexes in y, x order */
static int compare_vertex_y( const VERTEX * a, const VERTEX * b )
{
    return ( a->y == b->y ) ? a->x - b->x : a->y - b->y;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : gr_draw_span_XXX
 *
 *  Draw a textures span line into a bitmap. Those functions
 *  represent the inner loop of the blitter.
 *
 *  This file includes unoptimized C versions of those functions
 *
 *  There is one version of this function for each bit depth
 *  and blend effect configuration
 *
 *  PARAMS :
 *      dest            Destination bitmap or NULL for screen
 *      orig            Pointer to the graphic object to draw
 *      x, y            Pixel coordinates of the destination leftmost point
 *      pixels          Number of pixels to draw
 *      s, t            Texture coordinates of the leftmost point
 *      s2, t2          Texture coordinates of the rightmost point
 *
 *  RETURN VALUE :
 *      None
 *
 */
/* --------------------------------------------------------------------------- */

/*
static void draw_span_1to1(GRAPH * dest, GRAPH * orig, int x, int y, int pixels, int s, int t, int incs, int inct)
{
    uint8_t *ptr = (uint8_t *)dest->data + dest->pitch * y + x/8 ;
    int cs = s, ct = t, i;

    for (i = 0 ; i < pixels ; i++)
    {
        uint8_t * tex = (uint8_t *)orig->data + orig->pitch *(ct >> 16)/8 + (cs >> 16)/8;
        uint8_t   mask = (0x80 >> ((cs >> 16)/8 & 7));

        *ptr |= (*tex & mask);

        if (ct%8==7) ptr++;

        cs += incs, ct += inct;
    }
}

static void draw_span_1to1_nocolorkey(GRAPH * dest, GRAPH * orig, int x, int y, int pixels, int s, int t, int incs, int inct)
{
    uint8_t *ptr = (uint8_t *)dest->data + dest->pitch * y + x/8 ;
    int cs = s, ct = t, i;

    for (i = 0 ; i < pixels ; i++)
    {
        uint8_t * tex = (uint8_t *)orig->data + orig->pitch *(ct >> 16)/8 + (cs >> 16)/8;
        uint8_t   mask = (0x80 >> ((cs >> 16)/8 & 7));

        *ptr &= ~mask;
        *ptr |= (*tex & mask);

        if (ct%8==7) ptr++;

        cs += incs, ct += inct;
    }
}
*/

/* --------------------------------------------------------------------------- */
/* 1 to 8                                                                      */
/* --------------------------------------------------------------------------- */

static void draw_span_1to8( GRAPH * dest, GRAPH * orig, int x, int y, int pixels, int s, int t, int incs, int inct )
{
    uint8_t *ptr = ( uint8_t * ) dest->data + dest->pitch * y + x ;
    int cs = s, ct = t;

    while ( pixels-- )
    {
        uint8_t * tex = ( uint8_t * ) orig->data + orig->pitch * ( ct >> 16 ) + ( cs >> 16 ) / 8;
        uint8_t   mask = ( 0x80 >> (( cs >> 16 ) & 7 ) );
        if ( *tex & mask ) *ptr++ = pixel_color8;
        else ptr++;
        cs += incs, ct += inct;
    }
}

/* --------------------------------------------------------------------------- */
/* 8 to 8                                                                      */
/* --------------------------------------------------------------------------- */

static void draw_span_8to8_nocolorkey( GRAPH * dest, GRAPH * orig, int x, int y, int pixels, int s, int t, int incs, int inct )
{
    uint8_t *ptr = ( uint8_t * ) dest->data + dest->pitch * y + x;
    int cs = s, ct = t;

    while ( pixels-- )
    {
        uint8_t * tex = ( uint8_t * ) orig->data + orig->pitch * ( ct >> 16 ) + ( cs >> 16 );
        *ptr++ = *tex;
        cs += incs, ct += inct;
    }
}

static void draw_span_8to8_translucent( GRAPH * dest, GRAPH * orig, int x, int y, int pixels, int s, int t, int incs, int inct )
{
    uint8_t *ptr = ( uint8_t * ) dest->data + dest->pitch * y + x;
    int cs = s, ct = t;

    while ( pixels-- )
    {
        uint8_t * tex = ( uint8_t * ) orig->data + orig->pitch * ( ct >> 16 ) + ( cs >> 16 );
        if ( *tex ) *ptr = ghost8[( *tex << 8 ) + *ptr];
        ptr++;
        cs += incs, ct += inct;
    }
}

static void draw_span_8to8( GRAPH * dest, GRAPH * orig, int x, int y, int pixels, int s, int t, int incs, int inct )
{
    uint8_t *ptr = ( uint8_t * ) dest->data + dest->pitch * y + x;
    int cs = s, ct = t;

    while ( pixels-- )
    {
        uint8_t * tex = ( uint8_t * ) orig->data + orig->pitch * ( ct >> 16 ) + ( cs >> 16 );
        if ( *tex ) *ptr = *tex;
        ptr++;
        cs += incs, ct += inct;
    }
}

static void draw_span_8to8_ablend( GRAPH * dest, GRAPH * orig, int x, int y, int pixels, int s, int t, int incs, int inct )
{
    uint8_t *ptr = ( uint8_t * ) dest->data + dest->pitch * y + x;
    int cs = s, ct = t;

    while ( pixels-- )
    {
        uint8_t * tex = ( uint8_t * ) orig->data + orig->pitch * ( ct >> 16 ) + ( cs >> 16 );

        if ( *tex ) *ptr = ( uint8_t ) blend_func( *tex, *ptr );
        ptr++;
        cs += incs, ct += inct;
    }
}

static void draw_span_8to8_tablend( GRAPH * dest, GRAPH * orig, int x, int y, int pixels, int s, int t, int incs, int inct )
{
    uint8_t *ptr = ( uint8_t * ) dest->data + dest->pitch * y + x;
    int cs = s, ct = t;

    while ( pixels-- )
    {
        uint8_t * tex = ( uint8_t * ) orig->data + orig->pitch * ( ct >> 16 ) + ( cs >> 16 );
        if ( *tex ) *ptr = ( uint8_t ) ghost8[(blend_func( *tex, *ptr ) << 8 )+ *ptr ];
        ptr++;
        cs += incs, ct += inct;
    }
}

/* --------------------------------------------------------------------------- */
/* 1 to 16                                                                     */
/* --------------------------------------------------------------------------- */

static void draw_span_1to16( GRAPH * dest, GRAPH * orig, int x, int y, int pixels, int s, int t, int incs, int inct )
{
    uint16_t *ptr = ( uint16_t * ) dest->data + ( dest->pitch * y >> 1 ) + x ;
    int cs = s, ct = t;

    while ( pixels-- )
    {
        uint8_t * tex = ( uint8_t * ) orig->data + orig->pitch * ( ct >> 16 ) + ( cs >> 16 ) / 8;
        uint8_t   mask = ( 0x80 >> (( cs >> 16 ) & 7 ) );
        if ( *tex & mask ) *ptr++ = pixel_color16;
        else ptr++;
        cs += incs, ct += inct;
    }
}

/* --------------------------------------------------------------------------- */
/* 8 to 16                                                                     */
/* --------------------------------------------------------------------------- */

static void draw_span_8to16( GRAPH * dest, GRAPH * orig, int x, int y, int pixels, int s, int t, int incs, int inct )
{
    uint16_t *ptr = ( uint16_t * ) dest->data + ( dest->pitch * y >> 1 ) + x;
    int cs = s, ct = t;

    while ( pixels-- )
    {
        uint8_t * tex = ( uint8_t * ) orig->data + orig->pitch * ( ct >> 16 ) + ( cs >> 16 );
        if ( *tex ) *ptr = pcolorequiv[*tex];
        ptr++;
        cs += incs, ct += inct;
    }
}

static void draw_span_8to16_ablend( GRAPH * dest, GRAPH * orig, int x, int y, int pixels, int s, int t, int incs, int inct )
{
    uint16_t *ptr = ( uint16_t * ) dest->data + ( dest->pitch * y >> 1 ) + x;
    int cs = s, ct = t;

    while ( pixels-- )
    {
        uint8_t * tex = ( uint8_t * ) orig->data + orig->pitch * ( ct >> 16 ) + ( cs >> 16 );
        if ( *tex ) *ptr = blend_func( pcolorequiv[*tex], *ptr );
        ptr++;
        cs += incs, ct += inct;
    }
}

static void draw_span_8to16_tablend( GRAPH * dest, GRAPH * orig, int x, int y, int pixels, int s, int t, int incs, int inct )
{
    uint16_t *ptr = ( uint16_t * ) dest->data + ( dest->pitch * y >> 1 ) + x;
    int cs = s, ct = t;

    while ( pixels-- )
    {
        uint8_t * tex = ( uint8_t * ) orig->data + orig->pitch * ( ct >> 16 ) + ( cs >> 16 );
        if ( *tex ) *ptr = ( uint16_t )( ghost1[blend_func( pcolorequiv[*tex], *ptr )] + ghost2[*ptr] );
        ptr++;
        cs += incs, ct += inct;
    }
}

static void draw_span_8to16_translucent( GRAPH * dest, GRAPH * orig, int x, int y, int pixels, int s, int t, int incs, int inct )
{
    uint16_t *ptr = ( uint16_t * ) dest->data + ( dest->pitch * y >> 1 ) + x;
    int cs = s, ct = t;

    while ( pixels-- )
    {
        uint8_t * tex = ( uint8_t * ) orig->data + orig->pitch * ( ct >> 16 ) + ( cs >> 16 );
        if ( *tex ) *ptr = ghost1[pcolorequiv[*tex]] + ghost2[*ptr];
        ptr++;
        cs += incs, ct += inct;
    }
}

static void draw_span_8to16_nocolorkey( GRAPH * dest, GRAPH * orig, int x, int y, int pixels, int s, int t, int incs, int inct )
{
    uint16_t *ptr = ( uint16_t * ) dest->data + ( dest->pitch * y >> 1 ) + x;
    int cs = s, ct = t;

    while ( pixels-- )
    {
        uint8_t * tex = ( uint8_t * ) orig->data + orig->pitch * ( ct >> 16 ) + ( cs >> 16 );
        *ptr++ = pcolorequiv[*tex];
        cs += incs, ct += inct;
    }
}

/* --------------------------------------------------------------------------- */
/* 8 to 32                                                                     */
/* --------------------------------------------------------------------------- */

static void draw_span_8to32( GRAPH * dest, GRAPH * orig, int x, int y, int pixels, int s, int t, int incs, int inct )
{
    uint32_t *ptr = ( uint32_t * ) dest->data + ( dest->pitch * y >> 2 ) + x;
    int cs = s, ct = t;

    while ( pixels-- )
    {
        uint8_t * tex = ( uint8_t * ) orig->data + orig->pitch * ( ct >> 16 ) + ( cs >> 16 );
        if ( *tex ) *ptr = ( *ptr & 0xff000000 ) | pcolorequiv[*tex];
        ptr++;
        cs += incs, ct += inct;
    }
}

static void draw_span_8to32_ablend( GRAPH * dest, GRAPH * orig, int x, int y, int pixels, int s, int t, int incs, int inct )
{
    uint32_t *ptr = ( uint32_t * ) dest->data + ( dest->pitch * y >> 2 ) + x;
    int cs = s, ct = t;

    while ( pixels-- )
    {
        uint8_t * tex = ( uint8_t * ) orig->data + orig->pitch * ( ct >> 16 ) + ( cs >> 16 );
        if ( *tex ) *ptr = ( *ptr & 0xff000000 ) | blend_func( pcolorequiv[*tex], *ptr );
        ptr++;
        cs += incs, ct += inct;
    }
}

static void draw_span_8to32_tablend( GRAPH * dest, GRAPH * orig, int x, int y, int pixels, int s, int t, int incs, int inct )
{
    uint32_t *ptr = ( uint32_t * ) dest->data + ( dest->pitch * y >> 2 ) + x;
    int cs = s, ct = t;
    uint32_t r, g, b, c;

    while ( pixels-- )
    {
        uint8_t * tex = ( uint8_t * ) orig->data + orig->pitch * ( ct >> 16 ) + ( cs >> 16 );
        if ( *tex )
        {
            c = blend_func( pcolorequiv[*tex], *ptr ) ;

            r = ((( c & 0x00ff0000 ) * _factor ) + (( *ptr & 0x00ff0000 ) * _factor2 ) ) >> 8 ;
            g = ((( c & 0x0000ff00 ) * _factor ) + (( *ptr & 0x0000ff00 ) * _factor2 ) ) >> 8 ;
            b = ((( c & 0x000000ff ) * _factor ) + (( *ptr & 0x000000ff ) * _factor2 ) ) >> 8 ;

            if ( r > 0x00ff0000 ) r = 0x00ff0000 ; else r &= 0x00ff0000 ;
            if ( g > 0x0000ff00 ) g = 0x0000ff00 ; else g &= 0x0000ff00 ;
            if ( b > 0x000000ff ) b = 0x000000ff ; else b &= 0x000000ff ;

            *ptr = ( *ptr & 0xff000000 ) | r | g | b ;
        }

        ptr++;
        cs += incs, ct += inct;
    }
}

static void draw_span_8to32_translucent( GRAPH * dest, GRAPH * orig, int x, int y, int pixels, int s, int t, int incs, int inct )
{
    uint32_t *ptr = ( uint32_t * ) dest->data + ( dest->pitch * y >> 2 ) + x;
    int cs = s, ct = t;
    uint32_t r, g, b, c;

    while ( pixels-- )
    {
        uint8_t * tex = ( uint8_t * ) orig->data + orig->pitch * ( ct >> 16 ) + ( cs >> 16 );
        if ( *tex )
        {
            c = pcolorequiv[*tex];

            r = ((( c & 0x00ff0000 ) * _factor ) + (( *ptr & 0x00ff0000 ) * _factor2 ) ) >> 8 ;
            g = ((( c & 0x0000ff00 ) * _factor ) + (( *ptr & 0x0000ff00 ) * _factor2 ) ) >> 8 ;
            b = ((( c & 0x000000ff ) * _factor ) + (( *ptr & 0x000000ff ) * _factor2 ) ) >> 8 ;

            if ( r > 0x00ff0000 ) r = 0x00ff0000 ; else r &= 0x00ff0000 ;
            if ( g > 0x0000ff00 ) g = 0x0000ff00 ; else g &= 0x0000ff00 ;
            if ( b > 0x000000ff ) b = 0x000000ff ; else b &= 0x000000ff ;

            *ptr = ( *ptr & 0xff000000 ) | r | g | b ;
        }

        ptr++;
        cs += incs, ct += inct;
    }
}

static void draw_span_8to32_nocolorkey( GRAPH * dest, GRAPH * orig, int x, int y, int pixels, int s, int t, int incs, int inct )
{
    uint32_t *ptr = ( uint32_t * ) dest->data + ( dest->pitch * y >> 2 ) + x;
    int cs = s, ct = t;

    while ( pixels-- )
    {
        uint8_t * tex = ( uint8_t * ) orig->data + orig->pitch * ( ct >> 16 ) + ( cs >> 16 );
        *ptr = ( *ptr & 0xff000000 ) | pcolorequiv[*tex];
        ptr++;
        cs += incs, ct += inct;
    }
}

/* --------------------------------------------------------------------------- */
/* 16 to 16                                                                    */
/* --------------------------------------------------------------------------- */

static void draw_span_16to16( GRAPH * dest, GRAPH * orig, int x, int y, int pixels, int s, int t, int incs, int inct )
{
    uint16_t *ptr = ( uint16_t * ) dest->data + ( dest->pitch * y >> 1 ) + x;
    int cs = s, ct = t;
    uint16_t * tex ;

    while ( pixels-- )
    {
        tex = ( uint16_t * ) orig->data + ( orig->pitch * ( ct >> 16 ) >> 1 ) + ( cs >> 16 );
        if ( *tex ) *ptr = *tex;
        ptr++;
        cs += incs, ct += inct;
    }
}

static void draw_span_16to16_ablend( GRAPH * dest, GRAPH * orig, int x, int y, int pixels, int s, int t, int incs, int inct )
{
    uint16_t *ptr = ( uint16_t * ) dest->data + ( dest->pitch * y >> 1 ) + x;
    int cs = s, ct = t;
    uint16_t * tex ;

    while ( pixels-- )
    {
        tex = ( uint16_t * ) orig->data + ( orig->pitch * ( ct >> 16 ) >> 1 ) + ( cs >> 16 );
        if ( *tex ) *ptr = blend_func( *tex, *ptr );
        ptr++;
        cs += incs, ct += inct;
    }
}

static void draw_span_16to16_tablend( GRAPH * dest, GRAPH * orig, int x, int y, int pixels, int s, int t, int incs, int inct )
{
    uint16_t *ptr = ( uint16_t * ) dest->data + ( dest->pitch * y >> 1 ) + x;
    int cs = s, ct = t;
    uint16_t * tex ;

    while ( pixels-- )
    {
        tex = ( uint16_t * ) orig->data + ( orig->pitch * ( ct >> 16 ) >> 1 ) + ( cs >> 16 );
        if ( *tex ) *ptr = ( uint16_t )( ghost1[blend_func( *tex,*ptr )] + ghost2[*ptr] );
        ptr++;
        cs += incs, ct += inct;
    }
}

static void draw_span_16to16_translucent( GRAPH * dest, GRAPH * orig, int x, int y, int pixels, int s, int t, int incs, int inct )
{
    uint16_t *ptr = ( uint16_t * ) dest->data + ( dest->pitch * y >> 1 ) + x;
    int cs = s, ct = t;
    uint16_t * tex ;

    while ( pixels-- )
    {
        tex = ( uint16_t * ) orig->data + ( orig->pitch * ( ct >> 16 ) >> 1 ) + ( cs >> 16 );
        if ( *tex ) *ptr = ghost1[*tex] + ghost2[*ptr];
        ptr++;
        cs += incs, ct += inct;
    }
}

static void draw_span_16to16_nocolorkey( GRAPH * dest, GRAPH * orig, int x, int y, int pixels, int s, int t, int incs, int inct )
{
    uint16_t *ptr = ( uint16_t * ) dest->data + ( dest->pitch * y >> 1 ) + x;
    int cs = s, ct = t;
    uint16_t * tex ;

    while ( pixels-- )
    {
        tex = ( uint16_t * ) orig->data + ( orig->pitch * ( ct >> 16 ) >> 1 ) + ( cs >> 16 );
        *ptr++ = *tex;
        cs += incs, ct += inct;
    }
}

/* --------------------------------------------------------------------------- */
/* 16 to 32                                                                    */
/* --------------------------------------------------------------------------- */

static void draw_span_16to32( GRAPH * dest, GRAPH * orig, int x, int y, int pixels, int s, int t, int incs, int inct )
{
    uint32_t *ptr = ( uint32_t * ) dest->data + ( dest->pitch * y >> 2 ) + x;
    int cs = s, ct = t;
    uint16_t * tex ;

    while ( pixels-- )
    {
        tex = ( uint16_t * ) orig->data + ( orig->pitch * ( ct >> 16 ) >> 1 ) + ( cs >> 16 );
        if ( *tex ) *ptr = ( *ptr & 0xff000000 ) | (( * tex & 0xf800 ) << 8 ) | (( * tex & 0x07e0 ) << 5 ) | (( * tex & 0x001f ) << 3 ) ;
        ptr++;
        cs += incs, ct += inct;
    }
}

static void draw_span_16to32_ablend( GRAPH * dest, GRAPH * orig, int x, int y, int pixels, int s, int t, int incs, int inct )
{
    uint32_t *ptr = ( uint32_t * ) dest->data + ( dest->pitch * y >> 2 ) + x;
    int cs = s, ct = t;
    uint16_t * tex ;

    while ( pixels-- )
    {
        tex = ( uint16_t * ) orig->data + ( orig->pitch * ( ct >> 16 ) >> 1 ) + ( cs >> 16 );
        if ( *tex ) *ptr = ( *ptr & 0xff000000 ) | blend_func((( * tex & 0xf800 ) << 8 ) | (( * tex & 0x07e0 ) << 5 ) | (( * tex & 0x001f ) << 3 ), *ptr );
        ptr++;
        cs += incs, ct += inct;
    }
}

static void draw_span_16to32_tablend( GRAPH * dest, GRAPH * orig, int x, int y, int pixels, int s, int t, int incs, int inct )
{
    uint32_t *ptr = ( uint32_t * ) dest->data + ( dest->pitch * y >> 2 ) + x;
    int cs = s, ct = t;
    uint16_t * tex ;
    uint32_t r, g, b, c;

    while ( pixels-- )
    {
        tex = ( uint16_t * ) orig->data + ( orig->pitch * ( ct >> 16 ) >> 1 ) + ( cs >> 16 );
        if ( *tex )
        {
            c = blend_func((( * tex & 0xf800 ) << 8 ) | (( * tex & 0x07e0 ) << 5 ) | (( * tex & 0x001f ) << 3 ), *ptr ) ;

            r = ((( c & 0x00ff0000 ) * _factor ) + (( *ptr & 0x00ff0000 ) * _factor2 ) ) >> 8 ;
            g = ((( c & 0x0000ff00 ) * _factor ) + (( *ptr & 0x0000ff00 ) * _factor2 ) ) >> 8 ;
            b = ((( c & 0x000000ff ) * _factor ) + (( *ptr & 0x000000ff ) * _factor2 ) ) >> 8 ;

            if ( r > 0x00ff0000 ) r = 0x00ff0000 ; else r &= 0x00ff0000 ;
            if ( g > 0x0000ff00 ) g = 0x0000ff00 ; else g &= 0x0000ff00 ;
            if ( b > 0x000000ff ) b = 0x000000ff ; else b &= 0x000000ff ;

            *ptr = ( *ptr & 0xff000000 ) | r | g | b ;
        }

        ptr++;
        cs += incs, ct += inct;
    }
}

static void draw_span_16to32_translucent( GRAPH * dest, GRAPH * orig, int x, int y, int pixels, int s, int t, int incs, int inct )
{
    uint32_t *ptr = ( uint32_t * ) dest->data + ( dest->pitch * y >> 2 ) + x;
    int cs = s, ct = t;
    uint16_t * tex ;
    int r, g, b, c;

    while ( pixels-- )
    {
        tex = ( uint16_t * ) orig->data + ( orig->pitch * ( ct >> 16 ) >> 1 ) + ( cs >> 16 );
        if ( *tex )
        {
            c = (( * tex & 0xf800 ) << 8 ) | (( * tex & 0x07e0 ) << 5 ) | (( * tex & 0x001f ) << 3 );

            r = ((( c & 0x00ff0000 ) * _factor ) + (( *ptr & 0x00ff0000 ) * _factor2 ) ) >> 8 ;
            g = ((( c & 0x0000ff00 ) * _factor ) + (( *ptr & 0x0000ff00 ) * _factor2 ) ) >> 8 ;
            b = ((( c & 0x000000ff ) * _factor ) + (( *ptr & 0x000000ff ) * _factor2 ) ) >> 8 ;

            if ( r > 0x00ff0000 ) r = 0x00ff0000 ; else r &= 0x00ff0000 ;
            if ( g > 0x0000ff00 ) g = 0x0000ff00 ; else g &= 0x0000ff00 ;
            if ( b > 0x000000ff ) b = 0x000000ff ; else b &= 0x000000ff ;

            *ptr = ( *ptr & 0xff000000 ) | r | g | b ;
        }

        ptr++;
        cs += incs, ct += inct;
    }
}

static void draw_span_16to32_nocolorkey( GRAPH * dest, GRAPH * orig, int x, int y, int pixels, int s, int t, int incs, int inct )
{
    uint32_t *ptr = ( uint32_t * ) dest->data + ( dest->pitch * y >> 2 ) + x;
    int cs = s, ct = t;
    uint16_t * tex ;

    while ( pixels-- )
    {
        tex = ( uint16_t * ) orig->data + ( orig->pitch * ( ct >> 16 ) >> 1 ) + ( cs >> 16 );
        *ptr = ( *ptr & 0xff000000 ) | (( * tex & 0xf800 ) << 8 ) | (( * tex & 0x07e0 ) << 5 ) | (( * tex & 0x001f ) << 3 );
        ptr++;
        cs += incs, ct += inct;
    }
}

/* --------------------------------------------------------------------------- */
/* 1 to 32                                                                     */
/* --------------------------------------------------------------------------- */

static void draw_span_1to32( GRAPH * dest, GRAPH * orig, int x, int y, int pixels, int s, int t, int incs, int inct )
{
    uint32_t *ptr = ( uint32_t * ) dest->data + ( dest->pitch * y >> 2 ) + x ;
    int cs = s, ct = t;

    while ( pixels-- )
    {
        uint8_t * tex = ( uint8_t * ) orig->data + orig->pitch * ( ct >> 16 ) + ( cs >> 16 ) / 8;
        uint8_t   mask = ( 0x80 >> (( cs >> 16 ) & 7 ) );
        if ( *tex & mask ) *ptr = ( *ptr & 0xff000000 ) | pixel_color32;
        ptr++;
        cs += incs, ct += inct;
    }
}

/* --------------------------------------------------------------------------- */
/* 32 to 32                                                                    */
/* --------------------------------------------------------------------------- */

static void draw_span_32to32( GRAPH * dest, GRAPH * orig, int x, int y, int pixels, int s, int t, int incs, int inct )
{
    uint32_t *ptr = ( uint32_t * ) dest->data + ( dest->pitch * y >> 2 ) + x;
    int cs = s, ct = t;
    uint32_t * tex ;
    int r, g, b, c;

    while ( pixels-- )
    {
        tex = ( uint32_t * ) orig->data + ( orig->pitch * ( ct >> 16 ) >> 2 ) + ( cs >> 16 );
        if ( *tex )
        {
            if ( *tex ^ 0xff000000 )
            {
                c = *tex;
                _factor = ( c & 0xff000000 ) >> 24;
                _factor2 = 255 - _factor;

                r = ((( c & 0x00ff0000 ) * _factor ) + (( *ptr & 0x00ff0000 ) * _factor2 ) ) >> 8 ;
                g = ((( c & 0x0000ff00 ) * _factor ) + (( *ptr & 0x0000ff00 ) * _factor2 ) ) >> 8 ;
                b = ((( c & 0x000000ff ) * _factor ) + (( *ptr & 0x000000ff ) * _factor2 ) ) >> 8 ;

                if ( r > 0x00ff0000 ) r = 0x00ff0000 ; else r &= 0x00ff0000 ;
                if ( g > 0x0000ff00 ) g = 0x0000ff00 ; else g &= 0x0000ff00 ;
                if ( b > 0x000000ff ) b = 0x000000ff ; else b &= 0x000000ff ;

                *ptr = ( MAX( c & 0xff000000, *ptr & 0xff000000 ) ) | r | g | b ;
            }
            else if ( *tex )
                *ptr = *tex;
        }
        ptr++;
        cs += incs, ct += inct;
    }
}

static void draw_span_32to32_ablend( GRAPH * dest, GRAPH * orig, int x, int y, int pixels, int s, int t, int incs, int inct )
{
    uint32_t *ptr = ( uint32_t * ) dest->data + ( dest->pitch * y >> 2 ) + x;
    int cs = s, ct = t;
    uint32_t * tex ;
    uint32_t r, g, b, c;

    while ( pixels-- )
    {
        tex = ( uint32_t * ) orig->data + ( orig->pitch * ( ct >> 16 ) >> 2 ) + ( cs >> 16 );
        if ( *tex )
        {
            if ( *tex ^ 0xff000000 )
            {
                c = blend_func( *tex, *ptr );
                _factor = ( *tex & 0xff000000 ) >> 24;
                _factor2 = 255 - _factor;

                r = ((( c & 0x00ff0000 ) * _factor ) + (( *ptr & 0x00ff0000 ) * _factor2 ) ) >> 8 ;
                g = ((( c & 0x0000ff00 ) * _factor ) + (( *ptr & 0x0000ff00 ) * _factor2 ) ) >> 8 ;
                b = ((( c & 0x000000ff ) * _factor ) + (( *ptr & 0x000000ff ) * _factor2 ) ) >> 8 ;

                if ( r > 0x00ff0000 ) r = 0x00ff0000 ; else r &= 0x00ff0000 ;
                if ( g > 0x0000ff00 ) g = 0x0000ff00 ; else g &= 0x0000ff00 ;
                if ( b > 0x000000ff ) b = 0x000000ff ; else b &= 0x000000ff ;

                *ptr = ( MAX( c & 0xff000000, *ptr & 0xff000000 ) ) | r | g | b ;
            }
            else if ( *tex )
                *ptr = ( MAX( *tex & 0xff000000, *ptr & 0xff000000 ) ) | blend_func( *tex, *ptr );
        }
        ptr++;
        cs += incs, ct += inct;
    }
}

static void draw_span_32to32_tablend( GRAPH * dest, GRAPH * orig, int x, int y, int pixels, int s, int t, int incs, int inct )
{
    uint32_t *ptr = ( uint32_t * ) dest->data + ( dest->pitch * y >> 2 ) + x;
    int cs = s, ct = t;
    uint32_t * tex ;
    uint32_t r, g, b;
    unsigned int c, _f, _f2;

    while ( pixels-- )
    {
        tex = ( uint32_t * ) orig->data + ( orig->pitch * ( ct >> 16 ) >> 2 ) + ( cs >> 16 );

        if ( *tex )
        {
            c = blend_func( *tex, *ptr ) ;
            _f = (( unsigned int ) * tex ) & 0xff000000;
            if ( _f != 0xff000000 )
            {
                _f = ( _f >> 24 ) * _factor / 255 ;
                _f2 = 255 - _f ;

                r = ((( c & 0x00ff0000 ) * _f ) + (( *ptr & 0x00ff0000 ) * _f2 ) ) >> 8 ;
                g = ((( c & 0x0000ff00 ) * _f ) + (( *ptr & 0x0000ff00 ) * _f2 ) ) >> 8 ;
                b = ((( c & 0x000000ff ) * _f ) + (( *ptr & 0x000000ff ) * _f2 ) ) >> 8 ;
            }
            else
            {
                r = ((( c & 0x00ff0000 ) * _factor ) + (( *ptr & 0x00ff0000 ) * _factor2 ) ) >> 8 ;
                g = ((( c & 0x0000ff00 ) * _factor ) + (( *ptr & 0x0000ff00 ) * _factor2 ) ) >> 8 ;
                b = ((( c & 0x000000ff ) * _factor ) + (( *ptr & 0x000000ff ) * _factor2 ) ) >> 8 ;
            }

            if ( r > 0x00ff0000 ) r = 0x00ff0000 ; else r &= 0x00ff0000 ;
            if ( g > 0x0000ff00 ) g = 0x0000ff00 ; else g &= 0x0000ff00 ;
            if ( b > 0x000000ff ) b = 0x000000ff ; else b &= 0x000000ff ;

            *ptr = ( MAX( c & 0xff000000, *ptr & 0xff000000 ) ) | r | g | b ;
        }
        ptr++;
        cs += incs, ct += inct;
    }
}

static void draw_span_32to32_translucent( GRAPH * dest, GRAPH * orig, int x, int y, int pixels, int s, int t, int incs, int inct )
{
    uint32_t *ptr = ( uint32_t * ) dest->data + ( dest->pitch * y >> 2 ) + x;
    int cs = s, ct = t;
    uint32_t * tex ;
    int r, g, b;
    unsigned int c, _f, _f2;

    while ( pixels-- )
    {
        tex = ( uint32_t * ) orig->data + ( orig->pitch * ( ct >> 16 ) >> 2 ) + ( cs >> 16 );

        if ( *tex )
        {
            c = *tex ;
            _f = c & 0xff000000;
            if ( _f != 0xff000000 )
            {
                _f = ( _f >> 24 ) * _factor / 255 ;
                _f2 = 255 - _f ;

                r = ((( c & 0x00ff0000 ) * _f ) + (( *ptr & 0x00ff0000 ) * _f2 ) ) >> 8 ;
                g = ((( c & 0x0000ff00 ) * _f ) + (( *ptr & 0x0000ff00 ) * _f2 ) ) >> 8 ;
                b = ((( c & 0x000000ff ) * _f ) + (( *ptr & 0x000000ff ) * _f2 ) ) >> 8 ;
            }
            else
            {
                r = ((( c & 0x00ff0000 ) * _factor ) + (( *ptr & 0x00ff0000 ) * _factor2 ) ) >> 8 ;
                g = ((( c & 0x0000ff00 ) * _factor ) + (( *ptr & 0x0000ff00 ) * _factor2 ) ) >> 8 ;
                b = ((( c & 0x000000ff ) * _factor ) + (( *ptr & 0x000000ff ) * _factor2 ) ) >> 8 ;
            }

            if ( r > 0x00ff0000 ) r = 0x00ff0000 ; else r &= 0x00ff0000 ;
            if ( g > 0x0000ff00 ) g = 0x0000ff00 ; else g &= 0x0000ff00 ;
            if ( b > 0x000000ff ) b = 0x000000ff ; else b &= 0x000000ff ;

            *ptr = ( MAX( c & 0xff000000, *ptr & 0xff000000 ) ) | r | g | b ;
        }
        ptr++;
        cs += incs, ct += inct;
    }
}

static void draw_span_32to32_nocolorkey( GRAPH * dest, GRAPH * orig, int x, int y, int pixels, int s, int t, int incs, int inct )
{
    uint32_t *ptr = ( uint32_t * ) dest->data + ( dest->pitch * y >> 2 ) + x;
    int cs = s, ct = t;
    uint32_t * tex ;

    while ( pixels-- )
    {
        tex = ( uint32_t * ) orig->data + ( orig->pitch * ( ct >> 16 ) >> 2 ) + ( cs >> 16 );
        *ptr++ = *tex;
        cs += incs, ct += inct;
    }
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : gr_draw_hspan_XXX
 *
 *  Draw a textures span line into a bitmap. Those functions
 *  represent the inner loop of the blitter, but in an
 *  unscaled, non-rotated case (for gr_blit). Texture/screen
 *  coordinates are already calculated in origin/dest pointers.
 *
 *  This file includes unoptimized C versions of those functions
 *
 *  There is one version of this function for each bit depth
 *  and blend effect configuration
 *
 *  PARAMS :
 *      dest            Destination pointer
 *      tex             Origin pointer
 *      pixels          Number of pixels to draw
 *      incs            Texture increment: must be 1 or -1
 *
 *  RETURN VALUE :
 *      None
 *
 */

/* Parameter for 1to8 and 16to8 */
static int posx;

/* --------------------------------------------------------------------------- */
/* 1 to 8                                                                      */
/* --------------------------------------------------------------------------- */

static void draw_hspan_1to8( uint8_t *scr, uint8_t * tex, int pixels, int incs, int l, int scr_inc, int tex_inc )
{
    int mask, omask = ( 0x80 >> ( posx & 7 ) );
    int i;
    uint8_t * _scr = ( uint8_t * ) scr, * _tex = ( uint8_t * ) tex;

    while ( l-- )
    {
        mask = omask;
        for ( i = pixels; i--; )
        {
            if ( *tex & mask ) *scr++ = pixel_color8;
            else scr++;
            if ( incs < 0 )
            {
                if ( mask == 0x80 ) mask = 0x01, tex--;
                else mask <<= 1;
            }
            else
            {
                if ( mask == 0x01 ) mask = 0x80, tex++;
                else mask >>= 1;
            }
        }
        scr = _scr += scr_inc; tex = _tex += tex_inc;
    }
}

/* --------------------------------------------------------------------------- */
/* 8 to 8                                                                      */
/* --------------------------------------------------------------------------- */

static void draw_hspan_8to8_nocolorkey( uint8_t *scr, uint8_t * tex, int pixels, int incs, int l, int scr_inc, int tex_inc )
{
    int i;
    uint8_t * _scr = ( uint8_t * ) scr, * _tex = ( uint8_t * ) tex;

    while ( l-- )
    {
        for ( i = pixels; i--; )
        {
            *scr++ = *tex;
            tex += incs;
        }
        scr = _scr += scr_inc; tex = _tex += tex_inc;
    }
}

static void draw_hspan_8to8_translucent( uint8_t *scr, uint8_t * tex, int pixels, int incs, int l, int scr_inc, int tex_inc )
{
    int i;
    uint8_t * _scr = ( uint8_t * ) scr, * _tex = ( uint8_t * ) tex;

    while ( l-- )
    {
        for ( i = pixels; i--; )
        {
            if ( *tex ) *scr = ghost8[( *tex << 8 ) + *scr];
            scr++;
            tex += incs;
        }
        scr = _scr += scr_inc; tex = _tex += tex_inc;
    }
}

static void draw_hspan_8to8_tablend( uint8_t *scr, uint8_t * tex, int pixels, int incs, int l, int scr_inc, int tex_inc )
{
    int i;
    uint8_t * _scr = ( uint8_t * ) scr, * _tex = ( uint8_t * ) tex;

    while ( l-- )
    {
        for ( i = pixels; i--; )
        {
            if ( *tex ) *scr = ( uint8_t ) ghost8[(blend_func( *tex,*scr ) << 8 )+ *scr ];
            scr++;
            tex += incs;
        }
        scr = _scr += scr_inc; tex = _tex += tex_inc;
    }
}

static void draw_hspan_8to8_ablend( uint8_t *scr, uint8_t * tex, int pixels, int incs, int l, int scr_inc, int tex_inc )
{
    int i;
    uint8_t * _scr = ( uint8_t * ) scr, * _tex = ( uint8_t * ) tex;

    while ( l-- )
    {
        for ( i = pixels; i--; )
        {
            if ( *tex ) *scr = ( uint8_t ) blend_func( *tex, *scr );
            scr++;
            tex += incs;
        }
        scr = _scr += scr_inc; tex = _tex += tex_inc;
    }
}

static void draw_hspan_8to8( uint8_t *scr, uint8_t * tex, int pixels, int incs, int l, int scr_inc, int tex_inc )
{
    int i;
    uint8_t * _scr = ( uint8_t * ) scr, * _tex = ( uint8_t * ) tex;

    while ( l-- )
    {
        for ( i = pixels; i--; )
        {
            if ( *tex ) *scr = *tex;
            scr++;
            tex += incs;
        }
        scr = _scr += scr_inc; tex = _tex += tex_inc;
    }
}

/* --------------------------------------------------------------------------- */
/* 1 to 16                                                                     */
/* --------------------------------------------------------------------------- */

static void draw_hspan_1to16( uint16_t *scr, uint8_t * tex, int pixels, int incs, int l, int scr_inc, int tex_inc )
{
    int mask, omask = ( 0x80 >> ( posx & 7 ) );
    int i;
    uint8_t * _scr = ( uint8_t * ) scr, * _tex = ( uint8_t * ) tex;

    while ( l-- )
    {
        mask = omask;
        for ( i = pixels; i--; )
        {
            if ( *tex & mask ) *scr++ = pixel_color16;
            else scr++;
            if ( incs < 0 )
            {
                if ( mask == 0x80 ) mask = 0x01, tex--;
                else mask <<= 1;
            }
            else
            {
                if ( mask == 0x01 ) mask = 0x80, tex++;
                else mask >>= 1;
            }
        }
        scr = ( uint16_t * )( _scr += scr_inc ); tex = _tex += tex_inc;
    }
}

/* --------------------------------------------------------------------------- */
/* 8 to 16                                                                    */
/* --------------------------------------------------------------------------- */

static void draw_hspan_8to16( uint16_t *scr, uint8_t * tex, int pixels, int incs, int l, int scr_inc, int tex_inc )
{
    int i;
    uint8_t * _scr = ( uint8_t * ) scr, * _tex = ( uint8_t * ) tex;

    while ( l-- )
    {
        for ( i = pixels; i--; )
        {
            if ( *tex ) *scr = pcolorequiv[*tex];
            scr++;
            tex += incs;
        }
        scr = ( uint16_t * )( _scr += scr_inc ); tex = _tex += tex_inc;
    }
}

static void draw_hspan_8to16_ablend( uint16_t *scr, uint8_t * tex, int pixels, int incs, int l, int scr_inc, int tex_inc )
{
    int i;
    uint8_t * _scr = ( uint8_t * ) scr, * _tex = ( uint8_t * ) tex;

    while ( l-- )
    {
        for ( i = pixels; i--; )
        {
            if ( *tex ) *scr = blend_func( pcolorequiv[*tex], *scr );
            scr++;
            tex += incs;
        }
        scr = ( uint16_t * )( _scr += scr_inc ); tex = _tex += tex_inc;
    }
}

static void draw_hspan_8to16_tablend( uint16_t *scr, uint8_t * tex, int pixels, int incs, int l, int scr_inc, int tex_inc )
{
    int i;
    uint8_t * _scr = ( uint8_t * ) scr, * _tex = ( uint8_t * ) tex;

    while ( l-- )
    {
        for ( i = pixels; i--; )
        {
            if ( *tex ) *scr = ( uint16_t )( ghost1[blend_func( pcolorequiv[*tex], *scr )] + ghost2[*scr] );
            scr++;
            tex += incs;
        }
        scr = ( uint16_t * )( _scr += scr_inc ); tex = _tex += tex_inc;
    }
}

static void draw_hspan_8to16_translucent( uint16_t *scr, uint8_t * tex, int pixels, int incs, int l, int scr_inc, int tex_inc )
{
    int i;
    uint8_t * _scr = ( uint8_t * ) scr, * _tex = ( uint8_t * ) tex;

    while ( l-- )
    {
        for ( i = pixels; i--; )
        {
            if ( *tex ) *scr = ghost1[pcolorequiv[*tex]] + ghost2[*scr];
            scr++;
            tex += incs;
        }
        scr = ( uint16_t * )( _scr += scr_inc ); tex = _tex += tex_inc;
    }
}

static void draw_hspan_8to16_nocolorkey( uint16_t *scr, uint8_t * tex, int pixels, int incs, int l, int scr_inc, int tex_inc )
{
    int i;
    uint8_t * _scr = ( uint8_t * ) scr, * _tex = ( uint8_t * ) tex;

    while ( l-- )
    {
        for ( i = pixels; i--; )
        {
            *scr++ = pcolorequiv[*tex];
            tex += incs;
        }
        scr = ( uint16_t * )( _scr += scr_inc ); tex = _tex += tex_inc;
    }
}

/* --------------------------------------------------------------------------- */
/* 16 to 16                                                                    */
/* --------------------------------------------------------------------------- */

static void draw_hspan_16to16( uint16_t *scr, uint16_t * tex, int pixels, int incs, int l, int scr_inc, int tex_inc )
{
    int i;
    uint8_t * _scr = ( uint8_t * ) scr, * _tex = ( uint8_t * ) tex;

    while ( l-- )
    {
        for ( i = pixels; i--; )
        {
            if ( *tex ) *scr = *tex;
            scr++ ;
            tex += incs;
        }
        scr = ( uint16_t * )( _scr += scr_inc ); tex = ( uint16_t * )( _tex += tex_inc );
    }
}

static void draw_hspan_16to16_ablend( uint16_t *scr, uint16_t * tex, int pixels, int incs, int l, int scr_inc, int tex_inc )
{
    int i;
    uint8_t * _scr = ( uint8_t * ) scr, * _tex = ( uint8_t * ) tex;

    while ( l-- )
    {
        for ( i = pixels; i--; )
        {
            if ( *tex ) *scr = blend_func( *tex, *scr );
            scr++ ;
            tex += incs;
        }
        scr = ( uint16_t * )( _scr += scr_inc ); tex = ( uint16_t * )( _tex += tex_inc );
    }
}

static void draw_hspan_16to16_tablend( uint16_t *scr, uint16_t * tex, int pixels, int incs, int l, int scr_inc, int tex_inc )
{
    int i;
    uint8_t * _scr = ( uint8_t * ) scr, * _tex = ( uint8_t * ) tex;

    while ( l-- )
    {
        for ( i = pixels; i--; )
        {
            if ( *tex ) *scr = ( uint16_t )( ghost1[blend_func( *tex,*scr )] + ghost2[*scr] );
            scr++;
            tex += incs;
        }
        scr = ( uint16_t * )( _scr += scr_inc ); tex = ( uint16_t * )( _tex += tex_inc );
    }
}

static void draw_hspan_16to16_translucent( uint16_t *scr, uint16_t * tex, int pixels, int incs, int l, int scr_inc, int tex_inc )
{
    int i;
    uint8_t * _scr = ( uint8_t * ) scr, * _tex = ( uint8_t * ) tex;

    while ( l-- )
    {
        for ( i = pixels; i--; )
        {
            if ( *tex ) *scr = ghost1[*tex] + ghost2[*scr];
            scr++;
            tex += incs;
        }
        scr = ( uint16_t * )( _scr += scr_inc ); tex = ( uint16_t * )( _tex += tex_inc );
    }
}

static void draw_hspan_16to16_nocolorkey( uint16_t *scr, uint16_t * tex, int pixels, int incs, int l, int scr_inc, int tex_inc )
{
    int i;
    uint8_t * _scr = ( uint8_t * ) scr, * _tex = ( uint8_t * ) tex;

    while ( l-- )
    {
        for ( i = pixels; i--; )
        {
            *scr++ = *tex;
            tex += incs;
        }
        scr = ( uint16_t * )( _scr += scr_inc ); tex = ( uint16_t * )( _tex += tex_inc );
    }
}

/* --------------------------------------------------------------------------- */
/* 1 to 32                                                                     */
/* --------------------------------------------------------------------------- */

static void draw_hspan_1to32( uint32_t *scr, uint8_t * tex, int pixels, int incs, int l, int scr_inc, int tex_inc )
{
    int mask, omask = ( 0x80 >> ( posx & 7 ) );
    int i;
    uint8_t * _scr = ( uint8_t * ) scr, * _tex = ( uint8_t * ) tex;

    while ( l-- )
    {
        mask = omask;
        for ( i = pixels; i--; )
        {
            if ( *tex & mask ) *scr = ( *scr & 0xff000000 ) | pixel_color32;
            scr++;
            if ( incs < 0 )
            {
                if ( mask == 0x80 ) mask = 0x01, tex--;
                else mask <<= 1;
            }
            else
            {
                if ( mask == 0x01 ) mask = 0x80, tex++;
                else mask >>= 1;
            }
        }
        scr = ( uint32_t * )( _scr += scr_inc ); tex = _tex += tex_inc;
    }
}

/* --------------------------------------------------------------------------- */
/* 8 to 32                                                                    */
/* --------------------------------------------------------------------------- */

static void draw_hspan_8to32( uint32_t *scr, uint8_t * tex, int pixels, int incs, int l, int scr_inc, int tex_inc )
{
    int i;
    uint8_t * _scr = ( uint8_t * ) scr, * _tex = ( uint8_t * ) tex;

    while ( l-- )
    {
        for ( i = pixels; i--; )
        {
            if ( *tex ) *scr = ( *scr & 0xff000000 ) | pcolorequiv[*tex];
            scr++;
            tex += incs;
        }
        scr = ( uint32_t * )( _scr += scr_inc ); tex = _tex += tex_inc;
    }
}

static void draw_hspan_8to32_ablend( uint32_t *scr, uint8_t * tex, int pixels, int incs, int l, int scr_inc, int tex_inc )
{
    int i;
    uint8_t * _scr = ( uint8_t * ) scr, * _tex = ( uint8_t * ) tex;

    while ( l-- )
    {
        for ( i = pixels; i--; )
        {
            if ( *tex ) *scr = ( *scr & 0xff000000 ) | blend_func( pcolorequiv[*tex], *scr );
            scr++;
            tex += incs;
        }
        scr = ( uint32_t * )( _scr += scr_inc ); tex = _tex += tex_inc;
    }
}

static void draw_hspan_8to32_tablend( uint32_t *scr, uint8_t * tex, int pixels, int incs, int l, int scr_inc, int tex_inc )
{
    int i;
    uint8_t * _scr = ( uint8_t * ) scr, * _tex = ( uint8_t * ) tex;
    uint32_t r, g, b, c;

    while ( l-- )
    {
        for ( i = pixels; i--; )
        {
            if ( *tex )
            {
                c = blend_func( pcolorequiv[*tex], *scr ) ;

                r = ((( c & 0x00ff0000 ) * _factor ) + (( *scr & 0x00ff0000 ) * _factor2 ) ) >> 8 ;
                g = ((( c & 0x0000ff00 ) * _factor ) + (( *scr & 0x0000ff00 ) * _factor2 ) ) >> 8 ;
                b = ((( c & 0x000000ff ) * _factor ) + (( *scr & 0x000000ff ) * _factor2 ) ) >> 8 ;

                if ( r > 0x00ff0000 ) r = 0x00ff0000 ; else r &= 0x00ff0000 ;
                if ( g > 0x0000ff00 ) g = 0x0000ff00 ; else g &= 0x0000ff00 ;
                if ( b > 0x000000ff ) b = 0x000000ff ; else b &= 0x000000ff ;

                *scr = ( *scr & 0xff000000 ) | r | g | b ;
            }
            scr++;
            tex += incs;
        }
        scr = ( uint32_t * )( _scr += scr_inc ); tex = _tex += tex_inc;
    }
}

static void draw_hspan_8to32_translucent( uint32_t *scr, uint8_t * tex, int pixels, int incs, int l, int scr_inc, int tex_inc )
{
    int i;
    uint8_t * _scr = ( uint8_t * ) scr, * _tex = ( uint8_t * ) tex;
    uint32_t r, g, b, c;

    while ( l-- )
    {
        for ( i = pixels; i--; )
        {
            if ( *tex )
            {
                c = ( pcolorequiv[*tex] );

                r = ((( c & 0x00ff0000 ) * _factor ) + (( *scr & 0x00ff0000 ) * _factor2 ) ) >> 8 ;
                g = ((( c & 0x0000ff00 ) * _factor ) + (( *scr & 0x0000ff00 ) * _factor2 ) ) >> 8 ;
                b = ((( c & 0x000000ff ) * _factor ) + (( *scr & 0x000000ff ) * _factor2 ) ) >> 8 ;

                if ( r > 0x00ff0000 ) r = 0x00ff0000 ; else r &= 0x00ff0000 ;
                if ( g > 0x0000ff00 ) g = 0x0000ff00 ; else g &= 0x0000ff00 ;
                if ( b > 0x000000ff ) b = 0x000000ff ; else b &= 0x000000ff ;

                *scr = ( *scr & 0xff000000 ) | r | g | b ;
            }
            scr++;
            tex += incs;
        }
        scr = ( uint32_t * )( _scr += scr_inc ); tex = _tex += tex_inc;
    }
}

static void draw_hspan_8to32_nocolorkey( uint32_t *scr, uint8_t * tex, int pixels, int incs, int l, int scr_inc, int tex_inc )
{
    int i;
    uint8_t * _scr = ( uint8_t * ) scr, * _tex = ( uint8_t * ) tex;

    while ( l-- )
    {
        for ( i = pixels; i--; )
        {
            *scr = ( *scr & 0xff000000 ) | pcolorequiv[*tex];
            scr++;
            tex += incs;
        }
        scr = ( uint32_t * )( _scr += scr_inc ); tex = _tex += tex_inc;
    }
}

/* --------------------------------------------------------------------------- */
/* 16 to 32                                                                    */
/* --------------------------------------------------------------------------- */

static void draw_hspan_16to32( uint32_t *scr, uint16_t * tex, int pixels, int incs, int l, int scr_inc, int tex_inc )
{
    int i;
    uint8_t * _scr = ( uint8_t * ) scr, * _tex = ( uint8_t * ) tex;

    while ( l-- )
    {
        for ( i = pixels; i--; )
        {
            if ( *tex ) *scr = ( *scr & 0xff000000 ) | (( * tex & 0xf800 ) << 8 ) | (( * tex & 0x07e0 ) << 5 ) | (( * tex & 0x001f ) << 3 ) ;
            scr++ ;
            tex += incs;
        }
        scr = ( uint32_t * )( _scr += scr_inc ); tex = ( uint16_t * )( _tex += tex_inc );
    }
}

static void draw_hspan_16to32_ablend( uint32_t *scr, uint16_t * tex, int pixels, int incs, int l, int scr_inc, int tex_inc )
{
    int i;
    uint8_t * _scr = ( uint8_t * ) scr, * _tex = ( uint8_t * ) tex;

    while ( l-- )
    {
        for ( i = pixels; i--; )
        {
            if ( *tex ) *scr = ( *scr & 0xff000000 ) | blend_func((( * tex & 0xf800 ) << 8 ) | (( * tex & 0x07e0 ) << 5 ) | (( * tex & 0x001f ) << 3 ), *scr );
            scr++ ;
            tex += incs;
        }
        scr = ( uint32_t * )( _scr += scr_inc ); tex = ( uint16_t * )( _tex += tex_inc );
    }
}

static void draw_hspan_16to32_tablend( uint32_t *scr, uint16_t * tex, int pixels, int incs, int l, int scr_inc, int tex_inc )
{
    int i;
    uint8_t * _scr = ( uint8_t * ) scr, * _tex = ( uint8_t * ) tex;
    uint32_t r, g, b, c;

    while ( l-- )
    {
        for ( i = pixels; i--; )
        {
            if ( *tex )
            {
                c = blend_func((( * tex & 0xf800 ) << 8 ) | (( * tex & 0x07e0 ) << 5 ) | (( * tex & 0x001f ) << 3 ), *scr ) ;

                r = ((( c & 0x00ff0000 ) * _factor ) + (( *scr & 0x00ff0000 ) * _factor2 ) ) >> 8 ;
                g = ((( c & 0x0000ff00 ) * _factor ) + (( *scr & 0x0000ff00 ) * _factor2 ) ) >> 8 ;
                b = ((( c & 0x000000ff ) * _factor ) + (( *scr & 0x000000ff ) * _factor2 ) ) >> 8 ;

                if ( r > 0x00ff0000 ) r = 0x00ff0000 ; else r &= 0x00ff0000 ;
                if ( g > 0x0000ff00 ) g = 0x0000ff00 ; else g &= 0x0000ff00 ;
                if ( b > 0x000000ff ) b = 0x000000ff ; else b &= 0x000000ff ;

                *scr = ( *scr & 0xff000000 ) | r | g | b ;
            }
            scr++;
            tex += incs;
        }
        scr = ( uint32_t * )( _scr += scr_inc ); tex = ( uint16_t * )( _tex += tex_inc );
    }
}

static void draw_hspan_16to32_translucent( uint32_t *scr, uint16_t * tex, int pixels, int incs, int l, int scr_inc, int tex_inc )
{
    int i;
    uint8_t * _scr = ( uint8_t * ) scr, * _tex = ( uint8_t * ) tex;
    uint32_t r, g, b, c;

    while ( l-- )
    {
        for ( i = pixels; i--; )
        {
            if ( *tex )
            {
                c = (( * tex & 0xf800 ) << 8 ) | (( * tex & 0x07e0 ) << 5 ) | (( * tex & 0x001f ) << 3 );

                r = ((( c & 0x00ff0000 ) * _factor ) + (( *scr & 0x00ff0000 ) * _factor2 ) ) >> 8 ;
                g = ((( c & 0x0000ff00 ) * _factor ) + (( *scr & 0x0000ff00 ) * _factor2 ) ) >> 8 ;
                b = ((( c & 0x000000ff ) * _factor ) + (( *scr & 0x000000ff ) * _factor2 ) ) >> 8 ;

                if ( r > 0x00ff0000 ) r = 0x00ff0000 ; else r &= 0x00ff0000 ;
                if ( g > 0x0000ff00 ) g = 0x0000ff00 ; else g &= 0x0000ff00 ;
                if ( b > 0x000000ff ) b = 0x000000ff ; else b &= 0x000000ff ;

                *scr = ( *scr & 0xff000000 ) | r | g | b ;
            }
            scr++;
            tex += incs;
        }
        scr = ( uint32_t * )( _scr += scr_inc ); tex = ( uint16_t * )( _tex += tex_inc );
    }
}

static void draw_hspan_16to32_nocolorkey( uint32_t *scr, uint16_t * tex, int pixels, int incs, int l, int scr_inc, int tex_inc )
{
    int i;
    uint8_t * _scr = ( uint8_t * ) scr, * _tex = ( uint8_t * ) tex;

    while ( l-- )
    {
        for ( i = pixels; i--; )
        {
            *scr = ( *scr & 0xff000000 ) | (( * tex & 0xf800 ) << 8 ) | (( * tex & 0x07e0 ) << 5 ) | (( * tex & 0x001f ) << 3 );
            scr++;
            tex += incs;
        }
        scr = ( uint32_t * )( _scr += scr_inc ); tex = ( uint16_t * )( _tex += tex_inc );
    }
}

/* --------------------------------------------------------------------------- */
/* 32 to 32                                                                    */
/* --------------------------------------------------------------------------- */

static void draw_hspan_32to32( uint32_t *scr, uint32_t * tex, int pixels, int incs, int l, int scr_inc, int tex_inc )
{
    int i;
    uint8_t * _scr = ( uint8_t * ) scr, * _tex = ( uint8_t * ) tex;
    uint32_t r, g, b, c;

    while ( l-- )
    {
        for ( i = pixels; i--; )
        {
            if ( *tex )
            {
                if ( *tex ^ 0xff000000 )
                {
                    c = *tex;
                    _factor = ( c & 0xff000000 ) >> 24;
                    _factor2 = 255 - _factor;

                    r = ((( c & 0x00ff0000 ) * _factor ) + (( *scr & 0x00ff0000 ) * _factor2 ) ) >> 8 ;
                    g = ((( c & 0x0000ff00 ) * _factor ) + (( *scr & 0x0000ff00 ) * _factor2 ) ) >> 8 ;
                    b = ((( c & 0x000000ff ) * _factor ) + (( *scr & 0x000000ff ) * _factor2 ) ) >> 8 ;

                    if ( r > 0x00ff0000 ) r = 0x00ff0000 ; else r &= 0x00ff0000 ;
                    if ( g > 0x0000ff00 ) g = 0x0000ff00 ; else g &= 0x0000ff00 ;
                    if ( b > 0x000000ff ) b = 0x000000ff ; else b &= 0x000000ff ;

                    *scr = ( MAX( c & 0xff000000, *scr & 0xff000000 ) ) | r | g | b ;
                }
                else if ( *tex )
                    *scr = *tex;
            }
            scr++ ;
            tex += incs;
        }
        scr = ( uint32_t * )( _scr += scr_inc ); tex = ( uint32_t * )( _tex += tex_inc );
    }
}

static void draw_hspan_32to32_ablend( uint32_t *scr, uint32_t * tex, int pixels, int incs, int l, int scr_inc, int tex_inc )
{
    int i;
    uint8_t * _scr = ( uint8_t * ) scr, * _tex = ( uint8_t * ) tex;
    uint32_t r, g, b, c;

    while ( l-- )
    {
        for ( i = pixels; i--; )
        {
            if ( *tex )
            {
                if ( *tex ^ 0xff000000 )
                {
                    c = blend_func( *tex, *scr );
                    _factor = ( *tex & 0xff000000 ) >> 24;
                    _factor2 = 255 - _factor;

                    r = ((( c & 0x00ff0000 ) * _factor ) + (( *scr & 0x00ff0000 ) * _factor2 ) ) >> 8 ;
                    g = ((( c & 0x0000ff00 ) * _factor ) + (( *scr & 0x0000ff00 ) * _factor2 ) ) >> 8 ;
                    b = ((( c & 0x000000ff ) * _factor ) + (( *scr & 0x000000ff ) * _factor2 ) ) >> 8 ;

                    if ( r > 0x00ff0000 ) r = 0x00ff0000 ; else r &= 0x00ff0000 ;
                    if ( g > 0x0000ff00 ) g = 0x0000ff00 ; else g &= 0x0000ff00 ;
                    if ( b > 0x000000ff ) b = 0x000000ff ; else b &= 0x000000ff ;

                    *scr = ( MAX( c & 0xff000000, *scr & 0xff000000 ) ) | r | g | b ;
                }
                else if ( *tex )
                    *scr = ( MAX( *tex & 0xff000000, *scr & 0xff000000 ) ) | blend_func( *tex, *scr );
            }
            scr++ ;
            tex += incs;
        }
        scr = ( uint32_t * )( _scr += scr_inc ); tex = ( uint32_t * )( _tex += tex_inc );
    }
}

static void draw_hspan_32to32_tablend( uint32_t *scr, uint32_t * tex, int pixels, int incs, int l, int scr_inc, int tex_inc )
{
    int i;
    uint8_t * _scr = ( uint8_t * ) scr, * _tex = ( uint8_t * ) tex;
    uint32_t r, g, b;
    unsigned int c, _f, _f2;

    while ( l-- )
    {
        for ( i = pixels; i--; )
        {
            if ( *tex )
            {
                c = blend_func( *tex, *scr ) ;
                _f = (( unsigned int ) * tex ) & 0xff000000;
                if ( _f != 0xff000000 )
                {
                    _f = ( _f >> 24 ) * _factor / 255 ;
                    _f2 = 255 - _f ;

                    r = ((( c & 0x00ff0000 ) * _f ) + (( *scr & 0x00ff0000 ) * _f2 ) ) >> 8 ;
                    g = ((( c & 0x0000ff00 ) * _f ) + (( *scr & 0x0000ff00 ) * _f2 ) ) >> 8 ;
                    b = ((( c & 0x000000ff ) * _f ) + (( *scr & 0x000000ff ) * _f2 ) ) >> 8 ;
                }
                else
                {
                    r = ((( c & 0x00ff0000 ) * _factor ) + (( *scr & 0x00ff0000 ) * _factor2 ) ) >> 8 ;
                    g = ((( c & 0x0000ff00 ) * _factor ) + (( *scr & 0x0000ff00 ) * _factor2 ) ) >> 8 ;
                    b = ((( c & 0x000000ff ) * _factor ) + (( *scr & 0x000000ff ) * _factor2 ) ) >> 8 ;
                }
                if ( r > 0x00ff0000 ) r = 0x00ff0000 ; else r &= 0x00ff0000 ;
                if ( g > 0x0000ff00 ) g = 0x0000ff00 ; else g &= 0x0000ff00 ;
                if ( b > 0x000000ff ) b = 0x000000ff ; else b &= 0x000000ff ;

                *scr = ( MAX( c & 0xff000000, *scr & 0xff000000 ) ) | r | g | b ;
            }
            scr++;
            tex += incs;
        }
        scr = ( uint32_t * )( _scr += scr_inc ); tex = ( uint32_t * )( _tex += tex_inc );
    }
}

static void draw_hspan_32to32_translucent( uint32_t *scr, uint32_t * tex, int pixels, int incs, int l, int scr_inc, int tex_inc )
{
    int i;
    uint8_t * _scr = ( uint8_t * ) scr, * _tex = ( uint8_t * ) tex;
    int r, g, b;
    unsigned int c, _f, _f2;

    while ( l-- )
    {
        for ( i = pixels; i--; )
        {
            if ( *tex )
            {
                c = *tex ;
                _f = c & 0xff000000;
                if ( _f != 0xff000000 )
                {
                    _f = ( _f >> 24 ) * _factor / 255 ;
                    _f2 = 255 - _f ;

                    r = ((( c & 0x00ff0000 ) * _f ) + (( *scr & 0x00ff0000 ) * _f2 ) ) >> 8 ;
                    g = ((( c & 0x0000ff00 ) * _f ) + (( *scr & 0x0000ff00 ) * _f2 ) ) >> 8 ;
                    b = ((( c & 0x000000ff ) * _f ) + (( *scr & 0x000000ff ) * _f2 ) ) >> 8 ;
                }
                else
                {
                    r = ((( c & 0x00ff0000 ) * _factor ) + (( *scr & 0x00ff0000 ) * _factor2 ) ) >> 8 ;
                    g = ((( c & 0x0000ff00 ) * _factor ) + (( *scr & 0x0000ff00 ) * _factor2 ) ) >> 8 ;
                    b = ((( c & 0x000000ff ) * _factor ) + (( *scr & 0x000000ff ) * _factor2 ) ) >> 8 ;
                }

                if ( r > 0x00ff0000 ) r = 0x00ff0000 ; else r &= 0x00ff0000 ;
                if ( g > 0x0000ff00 ) g = 0x0000ff00 ; else g &= 0x0000ff00 ;
                if ( b > 0x000000ff ) b = 0x000000ff ; else b &= 0x000000ff ;

                *scr = ( MAX( c & 0xff000000, *scr & 0xff000000 ) ) | r | g | b ;
            }
            scr++;
            tex += incs;
        }
        scr = ( uint32_t * )( _scr += scr_inc ); tex = ( uint32_t * )( _tex += tex_inc );
    }
}

static void draw_hspan_32to32_nocolorkey( uint32_t *scr, uint32_t * tex, int pixels, int incs, int l, int scr_inc, int tex_inc )
{
    int i;
    uint8_t * _scr = ( uint8_t * ) scr, * _tex = ( uint8_t * ) tex;

    while ( l-- )
    {
        for ( i = pixels; i--; )
        {
            *scr++ = *tex;
            tex += incs;
        }
        scr = ( uint32_t * )( _scr += scr_inc ); tex = ( uint32_t * )( _tex += tex_inc );
    }
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : gr_calculate_corners
 *
 *  Calculate the screen coordinates of the corners of a graphic
 *  when rotated and scaled in an specific angle
 *
 *  PARAMS :
 *      graph           Pointer to the graphic object
 *      x, y            Pixel coordinates of the center on screen
 *      angle           Angle of rotation in miliangles
 *      scalex, scaley  Scaling ratio in percentaje (100 for original size)
 *      corners         Pointer to the output array of 4 points
 *
 *  RETURN VALUE :
 *      The screen coordinates of the points will be written
 *      into the "corners" array
 *
 */

static void gr_calculate_corners( GRAPH * dest, int screen_x, int screen_y, int flags, int angle, int scalex, int scaley, _POINTF * corners )
{
    float center_x, center_y;
    float top_y, bot_y;
    float lef_x, rig_x;
    float scalexf, scaleyf;
    int _angle = angle;

    /* Calculate the graphic center */

    if ( dest->ncpoints && dest->cpoints[0].x != CPOINT_UNDEFINED )
    {
        center_x = dest->cpoints[0].x /*- 0.5*/ ;
        center_y = dest->cpoints[0].y /*- 0.5*/ ;
    }
    else
    {
        center_x = ( dest->width  / 2.0 );
        center_y = ( dest->height / 2.0 );
    }

    if ( flags & B_HMIRROR )
    {
        if (_angle)
        {
            center_y = dest->height - center_y - 1 ;
            angle = 180000 - angle;
        }
        else
            center_x = dest->width - center_x - 1 ;
    }

    if ( flags & B_VMIRROR )
    {
        if (_angle)
        {
            center_x = dest->width - center_x - 1 ;
            angle = 180000 - angle;
        }
        else
            center_y = dest->height - center_y - 1 ;
    }

    if ( scalex == 100 && scaley == 100 && angle == 0 )
    {
        lef_x = - center_x ;
        rig_x = + ( dest->width  - center_x ) ;
        top_y = - center_y ;
        bot_y = + ( dest->height - center_y ) ;

        corners[0].x = ( screen_x  + lef_x ) * 1000.0 ;
        corners[0].y = ( screen_y  + top_y ) * 1000.0 ;

        corners[1].x = ( screen_x  + rig_x ) * 1000.0 ;
        corners[1].y = ( screen_y  + top_y ) * 1000.0 ;

        corners[2].x = ( screen_x  + lef_x ) * 1000.0 ;
        corners[2].y = ( screen_y  + bot_y ) * 1000.0 ;

        corners[3].x = ( screen_x  + rig_x ) * 1000.0 ;
        corners[3].y = ( screen_y  + bot_y ) * 1000.0 ;

        return;
    }

    /* Adjust size to prevent integer conversion errors */

    if ( scalex < 0 ) scalex = 0;
    if ( scaley < 0 ) scaley = 0;
    /*
        if ( scalex > 100 && scalex < 250 ) scalex = (( scalex - 4 ) & ~3 ) + 6;
        if ( scaley > 100 && scaley < 250 ) scaley = (( scaley - 4 ) & ~3 ) + 6;
    */
    /* Scale the coordinates */

    scalexf = scalex / 100.0 ;
    scaleyf = scaley / 100.0 ;

    /* Calculate the non-rotated non-translated coordinates */

    lef_x = - center_x * scalexf;
    rig_x = ( ( float ) dest->width  - center_x ) * scalexf;
    top_y = - center_y * scaleyf;
    bot_y = ( ( float ) dest->height - center_y ) * scaleyf;

    /* Rotate the coordinates */

    float cos_angle = ( float ) cos( ( float ) angle * M_PI / -180000.0 );
    float sin_angle = ( float ) sin( ( float ) angle * M_PI / -180000.0 );

    /* Top-left, top-right, bottom-left, bottom-right */

    corners[0].x = ( screen_x + ( cos_angle * lef_x - sin_angle * top_y ) ) * 1000.0 ;
    corners[0].y = ( screen_y + ( sin_angle * lef_x + cos_angle * top_y ) ) * 1000.0 ;

    corners[1].x = ( screen_x + ( cos_angle * rig_x - sin_angle * top_y ) ) * 1000.0 ;
    corners[1].y = ( screen_y + ( sin_angle * rig_x + cos_angle * top_y ) ) * 1000.0 ;

    corners[2].x = ( screen_x + ( cos_angle * lef_x - sin_angle * bot_y ) ) * 1000.0 ;
    corners[2].y = ( screen_y + ( sin_angle * lef_x + cos_angle * bot_y ) ) * 1000.0 ;

    corners[3].x = ( screen_x + ( cos_angle * rig_x - sin_angle * bot_y ) ) * 1000.0 ;
    corners[3].y = ( screen_y + ( sin_angle * rig_x + cos_angle * bot_y ) ) * 1000.0 ;

}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : gr_get_bbox
 *
 *  Calculate the bounding box of a graphic, when drawing it with
 *  the gr_rotated_blit function
 *
 *  PARAMS :
 *      dest            Destination bitmap or NULL for screen
 *      clip            Clipping region or NULL for the whole screen
 *      scrx, scry      Pixel coordinates of the center on screen
 *      angle           Angle of rotation in miliangles
 *      scalex, scaley  Scaling ratio in percentaje (100 for original size)
 *      gr              Pointer to the graphic object to draw
 *
 *  RETURN VALUE :
 *      None
 *
 */

void gr_get_bbox( REGION * dest, REGION * clip, int x, int y, int flags, int angle, int scalex, int scaley, GRAPH * gr )
{
    _POINTF corners[4];
    _POINT  min, max;
    int     i;

    /* Calculate the coordinates of each corner in the graphic */

    gr_calculate_corners( gr, x, y, flags, angle, scalex, scaley, corners );

    /* Calculate the bounding box */

    min.x = max.x = corners[0].x;
    min.y = max.y = corners[0].y;

    for ( i = 1 ; i < 4 ; i++ )
    {
        if ( min.x > corners[i].x ) min.x = corners[i].x;
        if ( max.x < corners[i].x ) max.x = corners[i].x;
        if ( min.y > corners[i].y ) min.y = corners[i].y;
        if ( max.y < corners[i].y ) max.y = corners[i].y;
    }

    dest->x  = min.x / 1000 ;
    dest->y  = min.y / 1000 ;
    dest->x2 = max.x / 1000 ;
    dest->y2 = max.y / 1000 ;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : gr_rotated_blit
 *
 *  Draw a rotated and/or scaled bitmap
 *
 *  PARAMS :
 *      dest            Destination bitmap or NULL for screen
 *      clip            Clipping region or NULL for the whole screen
 *      scrx, scry      Pixel coordinates of the center on screen
 *      angle           Angle of rotation in miliangles
 *      scalex, scaley  Scaling ratio in percentaje (100 for original size)
 *      gr              Pointer to the graphic object to draw
 *
 *  RETURN VALUE :
 *      None
 *
 */

void gr_rotated_blit( GRAPH * dest, REGION * clip, int scrx, int scry, int flags, int angle, int scalex, int scaley, GRAPH * gr )
{
    _POINTF corners[4];
    _POINT  min, max;
    VERTEX  vertex[4];
    int     i;

    float   half_texel_size_x = 0;
    float   half_texel_size_y = 0;

    /* Data for the left line */
    int     left_steps;
    int     left_pos;
    VECTOR  left_texture_increment;
    VERTEX* left_start;
    VERTEX* left_end;

    /* Data for the right line */
    int     right_steps;
    int     right_pos;
    VECTOR  right_texture_increment;
    VERTEX* right_start;
    VERTEX* right_end;

    /* Pointer to the line drawing function */
    DRAW_SPAN * draw_span = NULL;

    if ( !dest ) dest = scrbitmap;

    if ( !dest->data || !gr->data ) return;
    if ( scalex <= 0 || scaley <= 0 ) return;

    /* Analize the bitmap if needed (find if no color key used */

    if ( gr->modified > 1 ) bitmap_analize( gr ) ;

    if ( gr->info_flags & GI_NOCOLORKEY ) flags |= B_NOCOLORKEY ;

    /* Setup the 16 bits translucency tables if necessay */

    if ( gr->blend_table )
    {
        if ( dest->format->depth == 32 ) return ;
        ghost1 = ( uint16_t * ) gr->blend_table ;
        ghost2 = ( uint16_t * )( gr->blend_table + 65536 ) ;
        flags |= B_TRANSLUCENT ;
    }
    else if ( flags & B_ALPHA )
    {
        if ( dest->format->depth == 32 )
        {
            if ( flags & B_TRANSLUCENT )
            {
                _factor = ((( flags & B_ALPHA_MASK ) >> B_ALPHA_SHIFT ) ) >> 1;
            }
            else
            {
                _factor = ((( flags & B_ALPHA_MASK ) >> B_ALPHA_SHIFT ) ) ;
            }

            _factor2 = 255 - _factor ;
        }
        else if ( dest->format->depth == 16 )
        {
            if ( flags & B_TRANSLUCENT )
            {
                ghost1 = gr_alpha16((( flags & B_ALPHA_MASK ) >> B_ALPHA_SHIFT ) >> 1 );
                ghost2 = gr_alpha16( 255 - ((( flags & B_ALPHA_MASK ) >> B_ALPHA_SHIFT ) >> 1 ) );
            }
            else
            {
                ghost1 = gr_alpha16(( flags & B_ALPHA_MASK ) >> B_ALPHA_SHIFT );
                ghost2 = gr_alpha16( 255 - (( flags & B_ALPHA_MASK ) >> B_ALPHA_SHIFT ) );
            }
        }
        else if ( dest->format->depth == 8 )
        {
            if ( flags & B_TRANSLUCENT )
            {
                ghost8 = gr_alpha8((( flags & B_ALPHA_MASK ) >> B_ALPHA_SHIFT ) >> 1 );
            }
            else
            {
                ghost8 = gr_alpha8(( flags & B_ALPHA_MASK ) >> B_ALPHA_SHIFT );
            }
        }

        flags |= B_TRANSLUCENT ;
    }
    else if ( flags & B_TRANSLUCENT )
    {
        _factor = 128 ;
        _factor2 = 128 ;
        ghost1 = ghost2 = colorghost ;
        ghost8 = ( uint8_t * ) trans_table ;
    }

    if ((flags & B_TRANSLUCENT) && !trans_table_updated) gr_make_trans_table() ;

    blend_func = ( BLEND_FUNC * ) NULL;

    /* Choose a line drawing function */

    if ( dest->format->depth == 8 ) /* 8 bits target */
    {
        if ( gr->format->depth == 8 )
        {
            if ( flags & B_TRANSLUCENT )
            {
                if ( flags & B_ABLEND )
                {
                    blend_func = additive_blend8;
                    draw_span = ( DRAW_SPAN * )draw_span_8to8_tablend;
                }
                else if ( flags & B_SBLEND )
                {
                    blend_func = substractive_blend8;
                    draw_span = ( DRAW_SPAN * )draw_span_8to8_tablend;
                }
                else
                {
                    draw_span = ( DRAW_SPAN * )draw_span_8to8_translucent;
                }
            }
            else if ( flags & B_ABLEND )
            {
                blend_func = additive_blend8;
                draw_span = ( DRAW_SPAN * )draw_span_8to8_ablend;
            }
            else if ( flags & B_SBLEND )
            {
                blend_func = substractive_blend8;
                draw_span = ( DRAW_SPAN * )draw_span_8to8_ablend;
            }
            else if ( flags & B_NOCOLORKEY )
            {
                draw_span = ( DRAW_SPAN * )draw_span_8to8_nocolorkey;
            }
            else
            {
                draw_span = ( DRAW_SPAN * )draw_span_8to8;
            }
        }
        else if ( gr->format->depth == 1 )
        {
            draw_span = ( DRAW_SPAN * )draw_span_1to8;
        }
        else
            return;
    }
    else if ( dest->format->depth == 16 ) /* 16 bits target */
    {
        if ( gr->format->depth == 8 )
        {
            pcolorequiv = gr->format->palette ? gr->format->palette->colorequiv : sys_pixel_format->palette ? sys_pixel_format->palette->colorequiv : default_colorequiv ;

            if ( flags & B_TRANSLUCENT )
            {
                if ( flags & B_ABLEND )
                {
                    blend_func = additive_blend16;
                    draw_span = ( DRAW_SPAN * )draw_span_8to16_tablend;
                }
                else if ( flags & B_SBLEND )
                {
                    blend_func = substractive_blend16;
                    draw_span = ( DRAW_SPAN * )draw_span_8to16_tablend;
                }
                else
                {
                    draw_span = ( DRAW_SPAN * )draw_span_8to16_translucent;
                }
            }
            else if ( flags & B_ABLEND )
            {
                blend_func = additive_blend16;
                draw_span = ( DRAW_SPAN * )draw_span_8to16_ablend;
            }
            else if ( flags & B_SBLEND )
            {
                blend_func = substractive_blend16;
                draw_span = ( DRAW_SPAN * )draw_span_8to16_ablend;
            }
            else if ( flags & B_NOCOLORKEY )
            {
                draw_span = ( DRAW_SPAN * )draw_span_8to16_nocolorkey;
            }
            else
            {
                draw_span = ( DRAW_SPAN * )draw_span_8to16;
            }
        }
        else if ( gr->format->depth == 16 )
        {
            if ( flags & B_TRANSLUCENT )
            {
                if ( flags & B_ABLEND )
                {
                    blend_func = additive_blend16;
                    draw_span = ( DRAW_SPAN * )draw_span_16to16_tablend;
                }
                else if ( flags & B_SBLEND )
                {
                    blend_func = substractive_blend16;
                    draw_span = ( DRAW_SPAN * )draw_span_16to16_tablend;
                }
                else
                {
                    draw_span = ( DRAW_SPAN * )draw_span_16to16_translucent;
                }
            }
            else if ( flags & B_ABLEND )
            {
                blend_func = additive_blend16;
                draw_span = ( DRAW_SPAN * )draw_span_16to16_ablend;
            }
            else if ( flags & B_SBLEND )
            {
                blend_func = substractive_blend16;
                draw_span = ( DRAW_SPAN * )draw_span_16to16_ablend;
            }
            else if ( flags & B_NOCOLORKEY )
            {
                draw_span = ( DRAW_SPAN * )draw_span_16to16_nocolorkey;
            }
            else
            {
                draw_span = ( DRAW_SPAN * )draw_span_16to16;
            }
        }
        else if ( gr->format->depth == 1 )
        {
            draw_span = ( DRAW_SPAN * )draw_span_1to16;
        }
        else
        {
            return ; //Profundidad de color no soportada
        }
    }
    else if ( dest->format->depth == 32 ) /* 32 bits target */
    {
        if ( gr->format->depth == 8 )
        {
            pcolorequiv = gr->format->palette ? gr->format->palette->colorequiv : sys_pixel_format->palette ? sys_pixel_format->palette->colorequiv : default_colorequiv ;

            if ( flags & B_TRANSLUCENT )
            {
                if ( flags & B_ABLEND )
                {
                    blend_func = additive_blend32;
                    draw_span = ( DRAW_SPAN * )draw_span_8to32_tablend;
                }
                else if ( flags & B_SBLEND )
                {
                    blend_func = substractive_blend32;
                    draw_span = ( DRAW_SPAN * )draw_span_8to32_tablend;
                }
                else
                {
                    draw_span = ( DRAW_SPAN * )draw_span_8to32_translucent;
                }
            }
            else if ( flags & B_ABLEND )
            {
                blend_func = additive_blend32;
                draw_span = ( DRAW_SPAN * )draw_span_8to32_ablend;
            }
            else if ( flags & B_SBLEND )
            {
                blend_func = substractive_blend32;
                draw_span = ( DRAW_SPAN * )draw_span_8to32_ablend;
            }
            else if ( flags & B_NOCOLORKEY )
            {
                draw_span = ( DRAW_SPAN * )draw_span_8to32_nocolorkey;
            }
            else
            {
                draw_span = ( DRAW_SPAN * )draw_span_8to32;
            }
        }
        else if ( gr->format->depth == 16 )
        {
            if ( flags & B_TRANSLUCENT )
            {
                if ( flags & B_ABLEND )
                {
                    blend_func = additive_blend32;
                    draw_span = ( DRAW_SPAN * )draw_span_16to32_tablend;
                }
                else if ( flags & B_SBLEND )
                {
                    blend_func = substractive_blend32;
                    draw_span = ( DRAW_SPAN * )draw_span_16to32_tablend;
                }
                else
                {
                    draw_span = ( DRAW_SPAN * )draw_span_16to32_translucent;
                }
            }
            else if ( flags & B_ABLEND )
            {
                blend_func = additive_blend32;
                draw_span = ( DRAW_SPAN * )draw_span_16to32_ablend;
            }
            else if ( flags & B_SBLEND )
            {
                blend_func = substractive_blend32;
                draw_span = ( DRAW_SPAN * )draw_span_16to32_ablend;
            }
            else if ( flags & B_NOCOLORKEY )
            {
                draw_span = ( DRAW_SPAN * )draw_span_16to32_nocolorkey;
            }
            else
            {
                draw_span = ( DRAW_SPAN * )draw_span_16to32;
            }
        }
        else if ( gr->format->depth == 32 )
        {
            if ( flags & B_TRANSLUCENT )
            {
                if ( flags & B_ABLEND )
                {
                    blend_func = additive_blend32;
                    draw_span = ( DRAW_SPAN * )draw_span_32to32_tablend;
                }
                else if ( flags & B_SBLEND )
                {
                    blend_func = substractive_blend32;
                    draw_span = ( DRAW_SPAN * )draw_span_32to32_tablend;
                }
                else
                {
                    draw_span = ( DRAW_SPAN * )draw_span_32to32_translucent;
                }
            }
            else if ( flags & B_ABLEND )
            {
                blend_func = additive_blend32;
                draw_span = ( DRAW_SPAN * )draw_span_32to32_ablend;
            }
            else if ( flags & B_SBLEND )
            {
                blend_func = substractive_blend32;
                draw_span = ( DRAW_SPAN * )draw_span_32to32_ablend;
            }
            else if ( flags & B_NOCOLORKEY )
            {
                draw_span = ( DRAW_SPAN * )draw_span_32to32_nocolorkey;
            }
            else
            {
                draw_span = ( DRAW_SPAN * )draw_span_32to32;
            }
        }
        else if ( gr->format->depth == 1 )
        {
            draw_span = ( DRAW_SPAN * )draw_span_1to32;
        }
        else
        {
            return ; //Profundidad de color no soportada
        }
    }
    else
    {
        return ; //Profundidad de color no soportada
    }

    /* Calculate the coordinates of each corner in the graphic */

    gr_calculate_corners( gr, scrx, scry, flags, angle, scalex, scaley, corners );

    /* Calculate the clipping coordinates */

    if ( clip )
    {
        min.x = MAX( clip->x, 0 );
        min.y = MAX( clip->y, 0 );
        max.x = MIN( clip->x2, dest->width - 1 );
        max.y = MIN( clip->y2, dest->height - 1 );
    }
    else
    {
        min.x = 0;
        min.y = 0;
        max.x = dest->width - 1;
        max.y = dest->height - 1;
    }

    /* The texture coordinates of each corner point are displaced
       to the center of the texel to sidestep precision errors */

//    half_texel_size_x = 50.0 / scalex;
//    half_texel_size_y = 50.0 / scaley;


    half_texel_size_x = 0.5 / scalex;
    half_texel_size_y = 0.5 / scaley;

    /* Fill the vertex array with the four obtained points */

    for ( i = 0; i < 4; i++ )
    {
        vertex[i].x = corners[i].x ;
        vertex[i].y = corners[i].y ;
    }

    if ((( flags & B_VMIRROR ) && angle ) || (( flags & B_HMIRROR ) && !angle ) )
    {
        vertex[1].s = vertex[3].s = half_texel_size_y ;
        vertex[0].s = vertex[2].s = gr->width - half_texel_size_x /*- 1*/;
    }
    else
    {
        vertex[0].s = vertex[2].s = half_texel_size_y;
        vertex[1].s = vertex[3].s = gr->width - half_texel_size_x;
    }

    if ((( flags & B_HMIRROR ) && angle ) || (( flags & B_VMIRROR ) && !angle ) )
    {
        vertex[2].t = vertex[3].t = half_texel_size_x;
        vertex[0].t = vertex[1].t = gr->height - half_texel_size_y /*- 1*/;
    }
    else
    {
        vertex[0].t = vertex[1].t = half_texel_size_x;
        vertex[2].t = vertex[3].t = gr->height - half_texel_size_y ;
    }

    /* Sort the vertex list in y coordinate order */

    qsort( vertex, 4, sizeof( VERTEX ), compare_vertex_y );

    /* Create the first two left and right line segments */

    left_start = &vertex[0];
    left_end   = &vertex[1];

    right_start = &vertex[2];
    right_end   = &vertex[3];

    if ( right_start->x < right_end->x )
    {
        left_end   = &vertex[2];
        right_start = &vertex[1];
    }

    if ( right_start->x > right_end->x )
    {
        right_end   = right_start;
        right_start = &vertex[0];
    }

    left_steps  = ( left_end->y  - left_start->y ) / 1000 ;
    left_texture_increment.x  = ( left_end->s  - left_start->s ) / ( float )( left_steps ) ;
    left_texture_increment.y  = ( left_end->t  - left_start->t ) / ( float )( left_steps ) ;

    right_steps = ( right_end->y - right_start->y ) / 1000;
    right_texture_increment.x = ( right_end->s - right_start->s ) / ( float )( right_steps ) ;
    right_texture_increment.y = ( right_end->t - right_start->t ) / ( float )( right_steps ) ;

    left_pos  = -1 ;
    right_pos = -1 ;

    /* Draw the graphic a line each time, navigating through the
     * left and right line segments until reaching the graphic bottom */

    int  x, x2;
    float s, t, s2, t2;
    int yf = vertex[3].y / 1000;

    for ( i = vertex[0].y / 1000; i < yf; i++ )
    {
        left_pos++;
        right_pos++;

        /* Calculate the left point coordinates in screen and texture */
        x  = left_start->x / 1000 ;
        s  = left_start->s;
        t  = left_start->t;
        if ( left_pos > 0 && left_steps > 0 )
        {
            x += (( left_end->x - left_start->x ) / 1000 ) * left_pos / left_steps;
            s += left_texture_increment.x * left_pos ;
            t += left_texture_increment.y * left_pos ;
        }

        /* Calculate the right point coordinates in screen and texture */
        x2 = right_start->x / 1000;
        s2 = right_start->s;
        t2 = right_start->t;
        if ( right_pos > 0 && right_steps > 0 )
        {
            x2 += (( right_end->x - right_start->x ) / 1000 ) * right_pos / right_steps;
            s2 += right_texture_increment.x * right_pos ;
            t2 += right_texture_increment.y * right_pos ;
        }

        /* We don't calculate y /y2 coordinate positions, because they
           should be equal to i. However, precision errors may prevent it. */

        /* Clip the resulting coordinates */
        if ( i >= min.y && i <= max.y )
        {
            if ( x < min.x && x2 > x )
            {
                s += ( min.x - x ) * (( s2 - s ) / ( x2 - x ) );
                t += ( min.x - x ) * (( t2 - t ) / ( x2 - x ) );
                x  = min.x;
            }

            if ( x2 > max.x && x2 > x )
            {
                s2 -= ( x2 - max.x ) * (( s2 - s ) / ( x2 - x ) );
                t2 -= ( x2 - max.x ) * (( t2 - t ) / ( x2 - x ) );
                x2  = max.x;
            }

            /* Draw the resulting line */
            if ( x2 > x )
            {
                draw_span
                (
                    dest,
                    gr,
                    x,
                    i,
                    x2 - x + 1,
                    ( int )( s * 65536 ),
                    ( int )( t * 65536 ),
                    ( int )(( s2 - s ) / ( x2 - x ) * 65536 ),
                    ( int )(( t2 - t ) / ( x2 - x ) * 65536 )
                );
            }
        }

        /* If there is no more steps in the left vector,
         * advance to the next segment */

        if ( left_pos == left_steps )
        {
            left_start = left_end;
            left_end   = &vertex[3];
            left_pos   = -1 ;
            left_steps = ( left_end->y - left_start->y ) / 1000;
            if ( left_steps > 0 )
            {
                left_texture_increment.x = ( left_end->s - left_start->s ) / ( float )( left_steps ) ;
                left_texture_increment.y = ( left_end->t - left_start->t ) / ( float )( left_steps ) ;
            }
        }

        /* Same for the right vector */

        if ( right_pos == right_steps )
        {
            right_start = right_end;
            right_end   = &vertex[3];
            right_pos   = -1 ;
            right_steps = ( right_end->y - right_start->y ) / 1000;
            if ( right_steps > 0 )
            {
                right_texture_increment.x = ( right_end->s - right_start->s ) / ( float )( right_steps ) ;
                right_texture_increment.y = ( right_end->t - right_start->t ) / ( float )( right_steps ) ;
            }
        }
    }

    dest->info_flags &= ~GI_CLEAN;
    dest->modified = 2 ;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : gr_blit
 *
 *  Draw a bitmap (with no rotation or scaling, but with flags & clipping)
 *
 *  PARAMS :
 *      dest            Destination bitmap or NULL for screen
 *      clip            Clipping region or NULL for the whole screen
 *      scrx, scry      Pixel coordinates of the center on screen
 *      gr              Pointer to the graphic object to draw
 *
 *  RETURN VALUE :
 *      None
 *
 */

void gr_blit( GRAPH * dest, REGION * clip, int scrx, int scry, int flags, GRAPH * gr )
{
    _POINT  min, max;
    _POINT  center;
    int     x, y, s, t, p, l;
    void *  tex;
    void *  scr;
    int     tex_inc;
    int     scr_inc;
    int     direction;

    DRAW_HSPAN  * draw_hspan = ( DRAW_HSPAN * )NULL;

    if ( !dest ) dest = scrbitmap ;
    if ( !dest->data || !gr->data ) return;

    /* Calculate the clipping coordinates */

    if ( clip )
    {
        min.x = MAX( clip->x, 0 );
        min.y = MAX( clip->y, 0 );
        max.x = MIN( clip->x2, ( int ) dest->width - 1 );
        max.y = MIN( clip->y2, ( int ) dest->height - 1 );
    }
    else
    {
        min.x = 0;
        min.y = 0;
        max.x = dest->width - 1;
        max.y = dest->height - 1;
    }

    /* Analize the bitmap if needed (find if no color key used */

    if ( gr->modified > 1 ) bitmap_analize( gr ) ;

    if ( gr->info_flags & GI_NOCOLORKEY ) flags |= B_NOCOLORKEY ;

    /* Setup the 16 bits translucency tables if necessay */

    if ( gr->blend_table )
    {
        if ( dest->format->depth == 32 ) return ;
        ghost1 = ( uint16_t * ) gr->blend_table ;
        ghost2 = ( uint16_t * )( gr->blend_table + 65536 );
        flags |= B_TRANSLUCENT ;
    }
    else if ( flags & B_ALPHA )
    {
        if ( dest->format->depth == 32 )
        {
            if ( flags & B_TRANSLUCENT )
            {
                _factor = ((( flags & B_ALPHA_MASK ) >> B_ALPHA_SHIFT ) ) >> 1;
            }
            else
            {
                _factor = ((( flags & B_ALPHA_MASK ) >> B_ALPHA_SHIFT ) ) ;
            }

            _factor2 = 255 - _factor ;
        }
        else if ( dest->format->depth == 16 )
        {
            if ( flags & B_TRANSLUCENT )
            {
                ghost1 = gr_alpha16((( flags & B_ALPHA_MASK ) >> B_ALPHA_SHIFT ) >> 1 );
                ghost2 = gr_alpha16( 255 - ((( flags & B_ALPHA_MASK ) >> B_ALPHA_SHIFT ) >> 1 ) );
            }
            else
            {
                ghost1 = gr_alpha16(( flags & B_ALPHA_MASK ) >> B_ALPHA_SHIFT );
                ghost2 = gr_alpha16( 255 - (( flags & B_ALPHA_MASK ) >> B_ALPHA_SHIFT ) );
            }
        }
        else if ( dest->format->depth == 8 )
        {
            if ( flags & B_TRANSLUCENT )
            {
                ghost8 = gr_alpha8((( flags & B_ALPHA_MASK ) >> B_ALPHA_SHIFT ) >> 1 );
            }
            else
            {
                ghost8 = gr_alpha8(( flags & B_ALPHA_MASK ) >> B_ALPHA_SHIFT );
            }
        }

        flags |= B_TRANSLUCENT ;
    }
    else if ( flags & B_TRANSLUCENT )
    {
        _factor = 128 ;
        _factor2 = 128 ;
        ghost1 = ghost2 = colorghost ;
        ghost8 = ( uint8_t * ) trans_table ;
    }

    if ((flags & B_TRANSLUCENT) && !trans_table_updated) gr_make_trans_table() ;

    blend_func = ( BLEND_FUNC * ) NULL;

    /* Choose a line drawing function */

    if ( dest->format->depth == 8 ) /* 8 bits target */
    {
        if ( gr->format->depth == 8 )
        {
            if ( flags & B_TRANSLUCENT )
            {
                if ( flags & B_ABLEND )
                {
                    blend_func = additive_blend8;
                    draw_hspan = ( DRAW_HSPAN * )draw_hspan_8to8_tablend;
                }
                else if ( flags & B_SBLEND )
                {
                    blend_func = substractive_blend8;
                    draw_hspan = ( DRAW_HSPAN * )draw_hspan_8to8_tablend;
                }
                else
                {
                    draw_hspan = ( DRAW_HSPAN * )draw_hspan_8to8_translucent;
                }
            }
            else if ( flags & B_ABLEND )
            {
                blend_func = additive_blend8;
                draw_hspan = ( DRAW_HSPAN * )draw_hspan_8to8_ablend;
            }
            else if ( flags & B_SBLEND )
            {
                blend_func = substractive_blend8;
                draw_hspan = ( DRAW_HSPAN * )draw_hspan_8to8_ablend;
            }
            else if ( flags & B_NOCOLORKEY )
            {
                draw_hspan = ( DRAW_HSPAN * )draw_hspan_8to8_nocolorkey;
            }
            else
            {
                draw_hspan = ( DRAW_HSPAN * )draw_hspan_8to8;
            }
        }
        else if ( gr->format->depth == 1 )
        {
            draw_hspan = ( DRAW_HSPAN * )draw_hspan_1to8;
        }
        else
            return;
    }
    else if ( dest->format->depth == 16 ) /* 16 bits target */
    {
        if ( gr->format->depth == 8 )
        {
            pcolorequiv = gr->format->palette ? gr->format->palette->colorequiv : sys_pixel_format->palette ? sys_pixel_format->palette->colorequiv : default_colorequiv ;

            if ( flags & B_TRANSLUCENT )
            {
                if ( flags & B_ABLEND )
                {
                    blend_func = additive_blend16;
                    draw_hspan = ( DRAW_HSPAN * )draw_hspan_8to16_tablend;
                }
                else if ( flags & B_SBLEND )
                {
                    blend_func = substractive_blend16;
                    draw_hspan = ( DRAW_HSPAN * )draw_hspan_8to16_tablend;
                }
                else
                {
                    draw_hspan = ( DRAW_HSPAN * )draw_hspan_8to16_translucent;
                }
            }
            else if ( flags & B_ABLEND )
            {
                blend_func = additive_blend16;
                draw_hspan = ( DRAW_HSPAN * )draw_hspan_8to16_ablend;
            }
            else if ( flags & B_SBLEND )
            {
                blend_func = substractive_blend16;
                draw_hspan = ( DRAW_HSPAN * )draw_hspan_8to16_ablend;
            }
            else if ( flags & B_NOCOLORKEY )
            {
                draw_hspan = ( DRAW_HSPAN * )draw_hspan_8to16_nocolorkey;
            }
            else
            {
                draw_hspan = ( DRAW_HSPAN * )draw_hspan_8to16;
            }
        }
        else if ( gr->format->depth == 16 )
        {
            if ( flags & B_TRANSLUCENT )
            {
                if ( flags & B_ABLEND )
                {
                    blend_func = additive_blend16;
                    draw_hspan = ( DRAW_HSPAN * )draw_hspan_16to16_tablend;
                }
                else if ( flags & B_SBLEND )
                {
                    blend_func = substractive_blend16;
                    draw_hspan = ( DRAW_HSPAN * )draw_hspan_16to16_tablend;
                }
                else
                {
                    draw_hspan = ( DRAW_HSPAN * )draw_hspan_16to16_translucent;
                }
            }
            else if ( flags & B_ABLEND )
            {
                blend_func = additive_blend16;
                draw_hspan = ( DRAW_HSPAN * )draw_hspan_16to16_ablend;
            }
            else if ( flags & B_SBLEND )
            {
                blend_func = substractive_blend16;
                draw_hspan = ( DRAW_HSPAN * )draw_hspan_16to16_ablend;
            }
            else if ( flags & B_NOCOLORKEY )
            {
                draw_hspan = ( DRAW_HSPAN * )draw_hspan_16to16_nocolorkey;
            }
            else
            {
                draw_hspan = ( DRAW_HSPAN * )draw_hspan_16to16;
            }
        }
        else if ( gr->format->depth == 1 )
        {
            draw_hspan = ( DRAW_HSPAN * )draw_hspan_1to16;
        }
        else
        {
            return ; //Profundidad de color no soportada
        }
    }
    else if ( dest->format->depth == 32 ) /* 32 bits target */
    {
        if ( gr->format->depth == 8 )
        {
            pcolorequiv = gr->format->palette ? gr->format->palette->colorequiv : sys_pixel_format->palette ? sys_pixel_format->palette->colorequiv : default_colorequiv ;

            if ( flags & B_TRANSLUCENT )
            {
                if ( flags & B_ABLEND )
                {
                    blend_func = additive_blend32;
                    draw_hspan = ( DRAW_HSPAN * )draw_hspan_8to32_tablend;
                }
                else if ( flags & B_SBLEND )
                {
                    blend_func = substractive_blend32;
                    draw_hspan = ( DRAW_HSPAN * )draw_hspan_8to32_tablend;
                }
                else
                {
                    draw_hspan = ( DRAW_HSPAN * )draw_hspan_8to32_translucent;
                }
            }
            else if ( flags & B_ABLEND )
            {
                blend_func = additive_blend32;
                draw_hspan = ( DRAW_HSPAN * )draw_hspan_8to32_ablend;
            }
            else if ( flags & B_SBLEND )
            {
                blend_func = substractive_blend32;
                draw_hspan = ( DRAW_HSPAN * )draw_hspan_8to32_ablend;
            }
            else if ( flags & B_NOCOLORKEY )
            {
                draw_hspan = ( DRAW_HSPAN * )draw_hspan_8to32_nocolorkey;
            }
            else
            {
                draw_hspan = ( DRAW_HSPAN * )draw_hspan_8to32;
            }
        }
        else if ( gr->format->depth == 16 )
        {
            if ( flags & B_TRANSLUCENT )
            {
                if ( flags & B_ABLEND )
                {
                    blend_func = additive_blend32;
                    draw_hspan = ( DRAW_HSPAN * )draw_hspan_16to32_tablend;
                }
                else if ( flags & B_SBLEND )
                {
                    blend_func = substractive_blend32;
                    draw_hspan = ( DRAW_HSPAN * )draw_hspan_16to32_tablend;
                }
                else
                {
                    draw_hspan = ( DRAW_HSPAN * )draw_hspan_16to32_translucent;
                }
            }
            else if ( flags & B_ABLEND )
            {
                blend_func = additive_blend32;
                draw_hspan = ( DRAW_HSPAN * )draw_hspan_16to32_ablend;
            }
            else if ( flags & B_SBLEND )
            {
                blend_func = substractive_blend32;
                draw_hspan = ( DRAW_HSPAN * )draw_hspan_16to32_ablend;
            }
            else if ( flags & B_NOCOLORKEY )
            {
                draw_hspan = ( DRAW_HSPAN * )draw_hspan_16to32_nocolorkey;
            }
            else
            {
                draw_hspan = ( DRAW_HSPAN * )draw_hspan_16to32;
            }
        }
        else if ( gr->format->depth == 32 )
        {
            if ( flags & B_TRANSLUCENT )
            {
                if ( flags & B_ABLEND )
                {
                    blend_func = additive_blend32;
                    draw_hspan = ( DRAW_HSPAN * )draw_hspan_32to32_tablend;
                }
                else if ( flags & B_SBLEND )
                {
                    blend_func = substractive_blend32;
                    draw_hspan = ( DRAW_HSPAN * )draw_hspan_32to32_tablend;
                }
                else
                {
                    draw_hspan = ( DRAW_HSPAN * )draw_hspan_32to32_translucent;
                }
            }
            else if ( flags & B_ABLEND )
            {
                blend_func = additive_blend32;
                draw_hspan = ( DRAW_HSPAN * )draw_hspan_32to32_ablend;
            }
            else if ( flags & B_SBLEND )
            {
                blend_func = substractive_blend32;
                draw_hspan = ( DRAW_HSPAN * )draw_hspan_32to32_ablend;
            }
            else if ( flags & B_NOCOLORKEY )
            {
                draw_hspan = ( DRAW_HSPAN * )draw_hspan_32to32_nocolorkey;
            }
            else
            {
                draw_hspan = ( DRAW_HSPAN * )draw_hspan_32to32;
            }
        }
        else if ( gr->format->depth == 1 )
        {
            draw_hspan = ( DRAW_HSPAN * )draw_hspan_1to32;
        }
        else
        {
            return ; //Profundidad de color no soportada
        }
    }
    else
    {
        return ; //Profundidad de color no soportada
    }

    /* Calculate the graphic center */

    if ( gr->ncpoints && gr->cpoints[0].x != CPOINT_UNDEFINED )
    {
        center.x = gr->cpoints[0].x ;
        center.y = gr->cpoints[0].y ;
    }
    else
    {
        center.x = gr->width / 2.0;
        center.y = gr->height / 2.0;
    }

    if ( flags & B_HMIRROR ) center.x = gr->width  - 1 - center.x;
    if ( flags & B_VMIRROR ) center.y = gr->height - 1 - center.y;

    /* Calculate the initial texture and screen coordinates */

    x = scrx - center.x;
    y = scry - center.y;
    p = gr->width;
    l = gr->height;
    s = t = 0;

    /* Clip the coordinates */

    if ( y < min.y )
    {
        l -= min.y - y;
        t += min.y - y;
        y  = min.y;
    }

    if ( y + l - 1 > max.y ) l -= y + l - 1 - max.y ;

    if ( x < min.x )
    {
        p -= min.x - x;
        s += min.x - x;
        x  = min.x;
    }

    if ( x + p - 1 > max.x ) p -= x + p - 1 - max.x;

    if ( p < 1 || l < 1 ) return;

    /* Mirror the texture coordinates if needed */

    if ( flags & B_HMIRROR ) s = gr->width  - 1 - s;
    if ( flags & B_VMIRROR ) t = gr->height - 1 - t;

    /* Calculate the initial pointers and advances */

    posx    = s;
    if ( dest->format->depth == 1 )
        scr = ( uint8_t * ) dest->data + dest->pitch * y + ( x / 8 ) ;
    else
        scr = ( uint8_t * ) dest->data + dest->pitch * y + x * dest->format->depthb ;

    if ( gr->format->depth == 1 )
        tex = ( uint8_t * ) gr->data + gr->pitch * t + s / 8;
    else
        tex = ( uint8_t * ) gr->data + gr->pitch * t + s * gr->format->depthb;

    scr_inc   = dest->pitch ;
    tex_inc   = gr->pitch ;
    direction = 1;

    if ( flags & B_VMIRROR ) tex_inc = -tex_inc;
    if ( flags & B_HMIRROR ) direction = -1;

    if ( p > 0 ) draw_hspan( scr, tex, p, direction, l, scr_inc, tex_inc );

    dest->info_flags &= ~GI_CLEAN;
    dest->modified = 2 ;
}

/* --------------------------------------------------------------------------- */
