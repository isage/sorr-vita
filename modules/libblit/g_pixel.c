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

#include "libblit.h"

/* --------------------------------------------------------------------------- */

int pixel_color8 = 15 ;
uint16_t pixel_color16 = 0xFFFF ;
uint32_t pixel_color32 = 0xFFFFFFFF ;

uint16_t pixel_color16_alpha = 0xFFFF ;

int pixel_alpha = 255;
uint8_t * pixel_alpha8 = NULL ;
uint16_t * pixel_alpha16 = NULL ;

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : gr_get_pixel
 *
 *  Read a pixel from a bitmap
 *
 *  PARAMS :
 *      dest            Destination bitmap
 *      x, y            Pixel coordinates
 *
 *  RETURN VALUE :
 *      1, 8 or 16-bit integer with the pixel value
 *
 */

int gr_get_pixel( GRAPH * dest, int x, int y )
{
    if ( x < 0 || y < 0 || x >= ( int ) dest->width || y >= ( int ) dest->height ) return -1 ;

    switch ( dest->format->depth )
    {
        case 1:
            return (( *( uint8_t * )( dest->data + dest->pitch * y + ( x >> 3 ) ) ) & ( 0x80 >> ( x & 7 ) ) ) ? 1 : 0;

        case 8:
            return *(( uint8_t * ) dest->data + dest->pitch * y + x ) ;

        case 16:
            return *( uint16_t * )(( uint8_t * ) dest->data + dest->pitch * y + ( x << 1 ) ) ;

        case 32:
            return *( uint32_t * )(( uint8_t * ) dest->data + dest->pitch * y + ( x << 2 ) ) ;
    }
    return -1;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : gr_put_pixel
 *
 *  Paint a pixel with no clipping whatsoever, except by
 *  the bitmap's dimensions
 *
 *  PARAMS :
 *      dest            Destination bitmap
 *      x, y            Pixel coordinates
 *      color           1, 8 or 16-bit pixel value
 *
 *  RETURN VALUE :
 *      None
 *
 */

void gr_put_pixel( GRAPH * dest, int x, int y, int color )
{
    if ( x < 0 || y < 0 || x >= ( int ) dest->width || y >= ( int ) dest->height ) return ;

    switch ( dest->format->depth )
    {
        case 1:
            if ( color )
                *(( uint8_t * ) dest->data + dest->pitch * y + ( x >> 3 ) ) |= ( 0x80 >> ( x & 7 ) ) ;
            else
                *(( uint8_t * ) dest->data + dest->pitch * y + ( x >> 3 ) ) &= ~( 0x80 >> ( x & 7 ) ) ;
            break;

        case 8:
            _Pixel8(( uint8_t * ) dest->data + dest->pitch * y + x , color );
            break;

        case 16:
            _Pixel16(( uint8_t * ) dest->data + dest->pitch * y + ( x << 1 ), color, gr_alpha16( pixel_alpha )[ color ] ) ;
            break;

        case 32:
            {
                uint32_t * ptr = ( uint32_t * ) (( uint8_t * ) dest->data + dest->pitch * y + ( x << 2 )) ;

                if ( pixel_alpha == 255 && ( color & 0xff000000 ) == 0xff000000 )
                {
                    *ptr = color ;
                }
                else
                {
                    unsigned int _f = color & 0xff000000, _f2 ;
                    unsigned int r, g, b ;

                    _f = ( _f >> 24 ) * pixel_alpha / 255 ;
                    _f2 = 255 - _f ;

                    if ( _f != 0x000000ff )
                    {
                        r = ( ( color & 0x00ff0000 ) * _f + (( *ptr & 0x00ff0000 ) * _f2 ) ) >> 8 ;
                        g = ( ( color & 0x0000ff00 ) * _f + (( *ptr & 0x0000ff00 ) * _f2 ) ) >> 8 ;
                        b = ( ( color & 0x000000ff ) * _f + (( *ptr & 0x000000ff ) * _f2 ) ) >> 8 ;

                        if ( r > 0x00ff0000 ) r = 0x00ff0000 ; else r &= 0x00ff0000 ;
                        if ( g > 0x0000ff00 ) g = 0x0000ff00 ; else g &= 0x0000ff00 ;
                        if ( b > 0x000000ff ) b = 0x000000ff ; else b &= 0x000000ff ;

                        *ptr = ( _f << 24 ) | r | g | b ;
                    }
                    else
                    {
                        r = ( ( color & 0x00ff0000 ) * pixel_alpha + (( *ptr & 0x00ff0000 ) * _f2 ) ) >> 8 ;
                        g = ( ( color & 0x0000ff00 ) * pixel_alpha + (( *ptr & 0x0000ff00 ) * _f2 ) ) >> 8 ;
                        b = ( ( color & 0x000000ff ) * pixel_alpha + (( *ptr & 0x000000ff ) * _f2 ) ) >> 8 ;

                        if ( r > 0x00ff0000 ) r = 0x00ff0000 ; else r &= 0x00ff0000 ;
                        if ( g > 0x0000ff00 ) g = 0x0000ff00 ; else g &= 0x0000ff00 ;
                        if ( b > 0x000000ff ) b = 0x000000ff ; else b &= 0x000000ff ;

                        *ptr = 0xff000000 | r | g | b ;
                    }
                }
            }
            break;
    }

    if ( color )
    {
        dest->modified = 2 ;
        dest->info_flags &= ~GI_CLEAN;
    }
    else
    {
        dest->modified = 1 ; /* Don't needs analysis */
        dest->info_flags &= ~GI_NOCOLORKEY;
    }
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : gr_put_pixelc
 *
 *  Paint a pixel using a clipping region
 *
 *  PARAMS :
 *      dest            Destination bitmap
 *      x, y            Pixel coordinates
 *      color           1, 8 or 16-bit pixel value
 *
 *  RETURN VALUE :
 *      None
 *
 */

void gr_put_pixelc( GRAPH * dest, REGION * clip, int x, int y, int color )
{
    if ( clip && x >= clip->x && x <= clip->x2 && y >= clip->y && y <= clip->y2 )
        gr_put_pixel( dest, x, y, color );
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : gr_setcolor
 *
 *  Sets the drawing color
 *
 *  PARAMS :
 *      alpha           New color (16 bits if 16 bits enabled)
 *
 *  RETURN VALUE :
 *      None
 *
 */

void gr_setcolor( int c )
{
/*
    int r, g, b;

    if ( c )
    {
        if ( sys_pixel_format->depth != 8 )
        {
            gr_get_rgb( c, &r, &g, &b );
            pixel_color8 = gr_find_nearest_color( r, g, b );
        }
        else
        {

        }
    }
    else
        pixel_color8 = 0;
*/
    /* Fix this */
    pixel_color8 = c;
    pixel_color16 = c ;
    pixel_color32 = c ;

    if ( sys_pixel_format->depth == 16 && pixel_alpha != 255 ) pixel_color16_alpha = gr_alpha16( pixel_alpha )[ pixel_color16 ];
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : gr_setalpha
 *
 *  Sets the drawing alpha value for primitives
 *
 *  PARAMS :
 *      alpha           New alpha (0-255)
 *
 *  RETURN VALUE :
 *      None
 *
 */

void gr_setalpha( int value )
{
    value &= 0xFF;
    pixel_alpha = value;

    if ( sys_pixel_format->depth == 16 )
    {
        pixel_alpha8 = gr_alpha8( value );
        pixel_alpha16 = gr_alpha16( 255 - value );
        pixel_color16_alpha = gr_alpha16( value )[ pixel_color16 ];
    }
    else
    {
        pixel_alpha8 = gr_alpha8( value );
    }
}

/* --------------------------------------------------------------------------- */
