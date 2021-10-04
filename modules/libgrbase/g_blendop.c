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
#include "libgrbase.h"

/* --------------------------------------------------------------------------- */

/* Fast macros for color component extraction from a 16 bits value */

#define GETR(color) (((color & sys_pixel_format->Rmask) >> sys_pixel_format->Rshift) << sys_pixel_format->Rloss)
#define GETG(color) (((color & sys_pixel_format->Gmask) >> sys_pixel_format->Gshift) << sys_pixel_format->Gloss)
#define GETB(color) (((color & sys_pixel_format->Bmask) >> sys_pixel_format->Bshift) << sys_pixel_format->Bloss)

/* Fast macros for color composition */

#define MAKERGB_SATURATE(r,g,b)                                             \
    (                                                                           \
            ((int)(r) > 255 ? sys_pixel_format->Rmask : (((int)(r) >> sys_pixel_format->Rloss) << sys_pixel_format->Rshift)) |    \
            ((int)(g) > 255 ? sys_pixel_format->Gmask : (((int)(g) >> sys_pixel_format->Gloss) << sys_pixel_format->Gshift)) |    \
            ((int)(b) > 255 ? sys_pixel_format->Bmask : (((int)(b) >> sys_pixel_format->Bloss) << sys_pixel_format->Bshift))      \
    )

#define MAKERGB(r,g,b)                                                  \
    (                                                                       \
            (((int)(r) >> sys_pixel_format->Rloss) << sys_pixel_format->Rshift) |   \
            (((int)(g) >> sys_pixel_format->Gloss) << sys_pixel_format->Gshift) |   \
            (((int)(b) >> sys_pixel_format->Bloss) << sys_pixel_format->Bshift)     \
    )

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : blend_init
 *
 *  Initialize a blend table as a copy operation
 *
 *      Src_param   =   Src_color
 *      Dst_param   =   0
 *
 *  PARAMS :
 *      blend           Pointer to the blend table
 *      amount         Color component factor (1.0f leaves source unchanged)
 *
 *  RETURN VALUE :
 *      None
 */

void blend_init( int16_t * blend )
{
    int i;
    int16_t * blend2;

    if ( !blend ) return ;

    blend2 = blend + 65536;

    for ( i = 0; i < 65536; i++ )
    {
        *blend++ = i;
        *blend2++ = 0;
    }
}

/* --------------------------------------------------------------------------- */
/* ************************* */
/* Blendops public functions */
/* ************************* */
/*
 *  FUNCTION : blend_create
 *
 *  Create a new blendop table and initialize it with blend_init
 *  A blendop table is a group of two tables that return intermediate
 *  colors to a composite (+) operation that does no saturation:
 *
 *      Src_param   =   Src_color
 *      Dst_param   =   Dst_color
 *      Dst_color   =   Src_param + Dst_param       // No clamp!
 *
 *  The other blend_x funcions change the Src_param and Dst_param formulas.
 *  The Dst_color formula is immutable and embedded in the blitter
 *
 *  PARAMS :
 *      None
 *
 *  RETURN VALUE :
 *      Pointer to the new blendop table or NULL if not enough memory
 */

int16_t * blend_create( void )
{
    int16_t * blend = malloc( 65536 * 2 * sizeof( int16_t ) );
    if ( !blend ) return NULL;

    blend_init( blend );

    return blend;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : blend_apply
 *
 *  Apply a blend operation to the pixels of a graphic, as if it was
 *  rendered into a black (color 0) background
 *
 *  PARAMS :
 *      graph           Pointer to the graphic
 *      blend           Pointer to the blend table
 *
 *  RETURN VALUE :
 *      0               Error
 *      1               OK
 */

void blend_apply( GRAPH * graph, int16_t * blend )
{
    uint16_t * ptr;
    uint32_t x, y;
    uint8_t * ptr8;
    int16_t * blend2;

    if ( !graph || !blend || graph->format->depth != 16 ) return ;

    blend2 = blend + 65536;

    ptr = ( uint16_t * ) (ptr8 = ( uint8_t * ) graph->data);
    y = graph->height;
    while ( y-- )
    {
        x = graph->width;
        while ( x-- )
        {
            if ( *ptr ) * ptr = blend[ *ptr ] + blend2[ *ptr ];
            ptr++;
        }
        ptr = ( uint16_t * ) (ptr8 += graph->pitch);
    }

    graph->modified = 2;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : blend_assign
 *
 *  Assign a blend operation to a graphic. The graphic will be
 *  drawn using this blend operation thereafter.
 *
 *  PARAMS :
 *      graph           Pointer to the graphic
 *      blend           Pointer to the blend table
 *
 *  RETURN VALUE :
 *      None
 */

void blend_assign( GRAPH * graph, int16_t * blend )
{
    if ( !graph ) return ;
    graph->blend_table = blend;
    graph->modified = 1; /* Don't needs analysis */
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : blend_free
 *
 *  Free the memory used by a blendop table
 *
 *  PARAMS :
 *      blend           Pointer to the blend table
 *
 *  RETURN VALUE :
 *      None
 */

void blend_free( int16_t * blend )
{
    if ( !blend ) return ;
    free( blend );
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : blend_grayscale
 *
 *  Initialize a blend table as a grayscale operation (changes the
 *  color value of the source object to grayscale). Three methods
 *  of operation are supported:
 *
 *      1.  Luminance:  result = 0.3R + 0.59G + 0.11B
 *      2.  Desaturate: result = (MAX(R,G,B)+MIN(R,G,B))/2
 *      3.  Maximum:    result = MAX(R,G,B)
 *
 *
 *      Src_param   =   Grayscale_operation(previous Src_param)
 *
 *  PARAMS :
 *      blend           Pointer to the blend table
 *      method          Method of operation
 *
 *  RETURN VALUE :
 *      0               Error
 *      1               Ok
 */

void blend_grayscale( int16_t * blend, int method )
{
    int i, r, g, b, max, min;
    int16_t * blend2;

    if ( !blend ) return ;

    switch ( method )
    {
        case 1:
            blend2 = blend + 65536;

            for ( i = 0; i < 65536; i++ )
            {
                r = ( int )( GETR( i ) * 0.3 );
                g = ( int )( GETG( i ) * 0.59 );
                b = ( int )( GETB( i ) * 0.11 );
                r = r + g + b;
                *blend++ = MAKERGB( r, r, r );
                *blend2++ = 0;
            }
            break;

        case 2:
            blend2 = blend + 65536;

            for ( i = 0; i < 65536; i++ )
            {
                r = GETR( i );
                g = GETG( i );
                b = GETB( i );

            max = ( r > g ) ? ( r > b ) ? r : g : ( g > b ) ? g : b;
            min = ( r < g ) ? ( r < b ) ? r : g : ( g < b ) ? g : b;

                r = ( max + min ) / 2;
                *blend++ = MAKERGB( r, r, r );
                *blend2++ = 0;
            }
            break;

        case 3:
            blend2 = blend + 65536;

            for ( i = 0; i < 65536; i++ )
            {
                r = GETR( i );
                g = GETG( i );
                b = GETB( i );

            max = ( r > g ) ? ( r > b ) ? r : g : ( g > b ) ? g : b;

                *blend++ = MAKERGB( max, max, max );
                *blend2++ = 0;
            }
            break;
    }
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : blend_translucency
 *
 *  Modify a blend table as a translucency combination operation
 *
 *      Src_param   =   (previous Src_param) * amount
 *      Dst_param   =   Dst_color * (1.0 - amount)
 *
 *  PARAMS :
 *      blend           Pointer to the blend table
 *      amount         Opacity factor (1.0f = opaque, 0.0f = transparent)
 *
 *  RETURN VALUE :
 *      None
 */

void blend_translucency( int16_t * blend, float amount )
{
    int i, r, g, b;
    float amount2;
    int16_t * blend2;

    if ( !blend ) return ;

    blend2 = blend + 65536;

    if ( amount > 1.0f ) amount = 1.0f;
    if ( amount < 0.0f ) amount = 0.0f;

    amount = 1.0f - amount;
    amount2 = 1.0f - amount;

    for ( i = 0; i < 65536; i++ )
    {
        r = ( int )( GETR( *blend ) * amount );
        g = ( int )( GETG( *blend ) * amount );
        b = ( int )( GETB( *blend ) * amount );
        *blend++ = MAKERGB( r, g, b );

        r = ( int )( GETR( i ) * amount2 );
        g = ( int )( GETG( i ) * amount2 );
        b = ( int )( GETB( i ) * amount2 );
        *blend2++ = MAKERGB( r, g, b );
    }
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : blend_intensity
 *
 *  Modify a blend table as an intensity operation (changes the
 *  color value of the source object as the factor amount but
 *  does no transparency or other operation with the background)
 *
 *      Src_param   =   (previous Src_param) * amount
 *
 *  PARAMS :
 *      blend           Pointer to the blend table
 *      amount         Color component factor (1.0f leaves source unchanged)
 *
 *  RETURN VALUE :
 *      None
 */

void blend_intensity( int16_t * blend, float amount )
{
    int i, r, g, b;

    if ( !blend ) return ;

    if ( amount < 0.0f ) amount = 0.0f;

    for ( i = 65536; i--; )
    {
        r = ( int )( GETR( *blend ) * amount );
        g = ( int )( GETG( *blend ) * amount );
        b = ( int )( GETB( *blend ) * amount );
        *blend++ = MAKERGB_SATURATE( r, g, b );
    }
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : blend_swap
 *
 *  Swaps a blendop table. That is, change the blend operation so
 *  the Dst_color and Src_color are exchanged in the formulas.
 *
 *  PARAMS :
 *      blend           Pointer to the blend table
 *
 *  RETURN VALUE :
 *      None
 */

void blend_swap( int16_t * blend )
{
    int i, j;
    int16_t * blend2;

    if ( !blend ) return ;

    blend2 = blend + 65536;

    for ( i = 65536; i--; )
    {
        j = *blend;
        *blend++ = *blend2;
        *blend2++ = j;
    }
}


/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : blend_tint
 *
 *  Modify a blend table as a tint operation (changes the
 *  color value of the source object as a combination of a given
 *  color and the source color with the factor given)
 *
 *      Src_param = Const_color * amount
 *                  + (previous Src_Param) * (1.0f-amount)
 *
 *  PARAMS :
 *      blend           Pointer to the blend table
 *      amount         Color component factor (1.0f = Full Const color)
 *      cr              Constant color, red component (0-255)
 *      cg              Constant color, green component (0-255)
 *      cb              Constant color, blue component (0-255)
 *
 *  RETURN VALUE :
 *      None
 */

void blend_tint( int16_t * blend, float amount, uint8_t cr, uint8_t cg, uint8_t cb )
{
    int i, r, g, b;

    if ( !blend ) return ;

    if ( amount > 1.0f ) amount = 1.0f;
    if ( amount < 0.0f ) amount = 0.0f;

    for ( i = 65536; i--; )
    {
        r = ( int )( amount * cr + ( 1.0f - amount ) * GETR( *blend ) );
        g = ( int )( amount * cg + ( 1.0f - amount ) * GETG( *blend ) );
        b = ( int )( amount * cb + ( 1.0f - amount ) * GETB( *blend ) );
        *blend = MAKERGB_SATURATE( r, g, b );
        blend++;
    }
}

/* --------------------------------------------------------------------------- */
