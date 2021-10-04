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
#include <string.h>

#include "bgdrtm.h"

#include "bgddl.h"
#include "libblit.h"
#include "libdraw.h"
#include "fmath.h"

/* --------------------------------------------------------------------------- */

uint32_t drawing_stipple = 0xFFFFFFFF;

/* --------------------------------------------------------------------------- */

#ifdef __GNUC__
#define _inline
#endif

_inline void _HLine8_nostipple( uint8_t * ptr, uint32_t length )
{
    if ( pixel_alpha == 255 )
    {
        memset( ptr, pixel_color8, length + 1 ) ;
    }
    else
    {
        register int n;
        for ( n = length; n >= 0; n--, ptr++ )
            *ptr = pixel_alpha8[( pixel_color8 << 8 ) + *ptr ];
    }
}

/* --------------------------------------------------------------------------- */

_inline void _HLine8_stipple( uint8_t * ptr, uint32_t length )
{
    register int n;

    if ( pixel_alpha == 255 )
    {
        for ( n = length; n >= 0; n--, ptr++ )
        {
            if ( drawing_stipple & 1 ) * ptr = pixel_color8;
            drawing_stipple = (( drawing_stipple << 1 ) | (( drawing_stipple & 0x80000000 ) ? 1 : 0 ) );
        }
    }
    else
    {
        for ( n = length; n >= 0; n--, ptr++ )
        {
            if ( drawing_stipple & 1 ) * ptr = pixel_alpha8[( pixel_color8 << 8 ) + *ptr ];
            drawing_stipple = (( drawing_stipple << 1 ) | (( drawing_stipple & 0x80000000 ) ? 1 : 0 ) );
        }
    }
}

/* --------------------------------------------------------------------------- */

_inline void _HLine16_nostipple( uint16_t * ptr, uint32_t length )
{
    register int n ;

    if ( pixel_alpha == 255 )
    {
        for ( n = length; n >= 0; n-- ) *ptr++ = pixel_color16 ;
    }
    else
    {
        for ( n = length; n >= 0; n--, ptr++ ) *ptr = pixel_alpha16[ *ptr ] + pixel_color16_alpha ;
    }
}

/* --------------------------------------------------------------------------- */

_inline void _HLine16_stipple( uint16_t * ptr, uint32_t length )
{
    register int n ;

    if ( pixel_alpha == 255 )
    {
        for ( n = length; n >= 0; n--, ptr++ )
        {
            if ( drawing_stipple & 1 ) * ptr = pixel_color16 ;
            drawing_stipple = (( drawing_stipple << 1 ) | (( drawing_stipple & 0x80000000 ) ? 1 : 0 ) );
        }
    }
    else
    {
        for ( n = length; n >= 0; n--, ptr++ )
        {
            if ( drawing_stipple & 1 ) * ptr = pixel_alpha16[ *ptr ] + pixel_color16_alpha ;
            drawing_stipple = (( drawing_stipple << 1 ) | (( drawing_stipple & 0x80000000 ) ? 1 : 0 ) );
        }
    }
}

/* --------------------------------------------------------------------------- */

_inline void _HLine32_nostipple( uint32_t * ptr, uint32_t length )
{
    register int n ;

    if ( pixel_alpha == 255 && ( pixel_color32 & 0xff000000 ) == 0xff000000 )
    {
        for ( n = length; n >= 0; n-- ) *ptr++ = pixel_color32 ;
    }
    else
    {
        unsigned int _f = pixel_color32 & 0xff000000, _f2 ;
        unsigned int _c1, _c2, _c3 ;
        unsigned int r, g, b ;

        _f = ( _f >> 24 ) * pixel_alpha / 255 ;
        _f2 = 255 - _f ;

        if ( _f != 0x000000ff )
        {
            _c1 = ( pixel_color32 & 0x00ff0000 ) * _f ;
            _c2 = ( pixel_color32 & 0x0000ff00 ) * _f ;
            _c3 = ( pixel_color32 & 0x000000ff ) * _f ;

            _f <<= 24 ;

            for ( n = length; n >= 0; n-- )
            {
                r = ( _c1 + (( *ptr & 0x00ff0000 ) * _f2 ) ) >> 8 ;
                g = ( _c2 + (( *ptr & 0x0000ff00 ) * _f2 ) ) >> 8 ;
                b = ( _c3 + (( *ptr & 0x000000ff ) * _f2 ) ) >> 8 ;

                if ( r > 0x00ff0000 ) r = 0x00ff0000 ; else r &= 0x00ff0000 ;
                if ( g > 0x0000ff00 ) g = 0x0000ff00 ; else g &= 0x0000ff00 ;
                if ( b > 0x000000ff ) b = 0x000000ff ; else b &= 0x000000ff ;

                *ptr++ = _f | r | g | b ;
            }
        }
        else
        {
            _c1 = ( pixel_color32 & 0x00ff0000 ) * pixel_alpha ;
            _c2 = ( pixel_color32 & 0x0000ff00 ) * pixel_alpha ;
            _c3 = ( pixel_color32 & 0x000000ff ) * pixel_alpha ;

            for ( n = length; n >= 0; n-- )
            {
                r = ( _c1 + (( *ptr & 0x00ff0000 ) * _f2 ) ) >> 8 ;
                g = ( _c2 + (( *ptr & 0x0000ff00 ) * _f2 ) ) >> 8 ;
                b = ( _c3 + (( *ptr & 0x000000ff ) * _f2 ) ) >> 8 ;

                if ( r > 0x00ff0000 ) r = 0x00ff0000 ; else r &= 0x00ff0000 ;
                if ( g > 0x0000ff00 ) g = 0x0000ff00 ; else g &= 0x0000ff00 ;
                if ( b > 0x000000ff ) b = 0x000000ff ; else b &= 0x000000ff ;

                *ptr++ = 0xff000000 | r | g | b ;
            }
        }
    }
}

/* --------------------------------------------------------------------------- */

_inline void _HLine32_stipple( uint32_t * ptr, uint32_t length )
{
    register int n ;

    if ( pixel_alpha == 255 && ( pixel_color32 & 0xff000000 ) == 0xff000000 )
    {
        for ( n = length; n >= 0; n-- )
        {
            if ( drawing_stipple & 1 ) *ptr = pixel_color32 ;
            drawing_stipple = (( drawing_stipple << 1 ) | (( drawing_stipple & 0x80000000 ) ? 1 : 0 ) );
        }
    }
    else
    {
        unsigned int _f = pixel_color32 & 0xff000000, _f2 ;
        unsigned int _c1, _c2, _c3 ;
        unsigned int r, g, b ;

        _f = ( _f >> 24 ) * pixel_alpha / 255 ;
        _f2 = 255 - _f ;

        if ( _f != 0x000000ff )
        {
            _c1 = ( pixel_color32 & 0x00ff0000 ) * _f ;
            _c2 = ( pixel_color32 & 0x0000ff00 ) * _f ;
            _c3 = ( pixel_color32 & 0x000000ff ) * _f ;

            _f <<= 24 ;

            for ( n = length; n >= 0; n-- )
            {
                if ( drawing_stipple & 1 )
                {
                    r = ( _c1 + (( *ptr & 0x00ff0000 ) * _f2 ) ) >> 8 ;
                    g = ( _c2 + (( *ptr & 0x0000ff00 ) * _f2 ) ) >> 8 ;
                    b = ( _c3 + (( *ptr & 0x000000ff ) * _f2 ) ) >> 8 ;

                    if ( r > 0x00ff0000 ) r = 0x00ff0000 ; else r &= 0x00ff0000 ;
                    if ( g > 0x0000ff00 ) g = 0x0000ff00 ; else g &= 0x0000ff00 ;
                    if ( b > 0x000000ff ) b = 0x000000ff ; else b &= 0x000000ff ;

                    *ptr++ = _f | r | g | b ;
                }
                drawing_stipple = (( drawing_stipple << 1 ) | (( drawing_stipple & 0x80000000 ) ? 1 : 0 ) );
            }
        }
        else
        {
            _c1 = ( pixel_color32 & 0x00ff0000 ) * pixel_alpha ;
            _c2 = ( pixel_color32 & 0x0000ff00 ) * pixel_alpha ;
            _c3 = ( pixel_color32 & 0x000000ff ) * pixel_alpha ;

            for ( n = length; n >= 0; n-- )
            {
                if ( drawing_stipple & 1 )
                {
                    r = ( _c1 + (( *ptr & 0x00ff0000 ) * _f2 ) ) >> 8 ;
                    g = ( _c2 + (( *ptr & 0x0000ff00 ) * _f2 ) ) >> 8 ;
                    b = ( _c3 + (( *ptr & 0x000000ff ) * _f2 ) ) >> 8 ;

                    if ( r > 0x00ff0000 ) r = 0x00ff0000 ; else r &= 0x00ff0000 ;
                    if ( g > 0x0000ff00 ) g = 0x0000ff00 ; else g &= 0x0000ff00 ;
                    if ( b > 0x000000ff ) b = 0x000000ff ; else b &= 0x000000ff ;

                    *ptr++ = 0xff000000 | r | g | b ;
                }
                drawing_stipple = (( drawing_stipple << 1 ) | (( drawing_stipple & 0x80000000 ) ? 1 : 0 ) );
            }
        }
    }
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : draw_vline
 *
 *  Draw a vertical line
 *
 *  PARAMS :
 *      dest            Destination bitmap or NULL for screen
 *      clip            Clipping region or NULL for the whole screen
 *      x, y            Coordinates of the top-left pixel
 *      h               Height in pixels
 *
 *  RETURN VALUE :
 *      None
 *
 */

void draw_vline( GRAPH * dest, REGION * clip, int x, int y, int h )
{
    REGION base_clip ;
    int old_stipple = drawing_stipple;

    if ( !dest ) dest = scrbitmap ;
    if ( !clip )
    {
        clip = &base_clip ;
        clip->x = 0 ;
        clip->y = 0 ;
        clip->x2 = dest->width - 1 ;
        clip->y2 = dest->height - 1 ;
    }
    else
    {
        base_clip = *clip ;
        clip = &base_clip ;
        clip->x = MAX( MIN( clip->x, clip->x2 ), 0 ) ;
        clip->y = MAX( MIN( clip->y, clip->y2 ), 0 ) ;
        clip->x2 = MIN( MAX( clip->x, clip->x2 ), dest->width - 1 ) ;
        clip->y2 = MIN( MAX( clip->y, clip->y2 ), dest->height - 1 ) ;
    }

    dest->modified = 2 ;
    dest->info_flags &= ~GI_CLEAN;

    if ( h < 0 ) h = -h, y -= ( h /*- 1*/ ) ;

    if ( x < clip->x || x > clip->x2 ) return ;

    if ( y < clip->y ) h += y - clip->y, y = clip->y;
    if ( y + h > clip->y2 ) h = clip->y2 - y /*+ 1*/ ;

    switch ( dest->format->depth )
    {
        case 1:
        {
            uint8_t * ptr = dest->data;
            int mask ;
            ptr += dest->pitch * y + ( x >> 3 ) ;
            mask = ( 1 << ( 7 - ( x & 7 ) ) );
            if ( drawing_stipple != 0xFFFFFFFF )
            {
                while ( h-- >= 0 )
                {
                    if ( drawing_stipple & 1 )
                    {
                        if ( !pixel_color8 )
                            * ptr &= ~mask;
                        else
                            *ptr |= mask;
                    }
                    drawing_stipple = (( drawing_stipple << 1 ) | (( drawing_stipple & 0x80000000 ) ? 1 : 0 ) );
                    ptr += dest->pitch ;
                }
            }
            else
            {
                while ( h-- >= 0 )
                {
                    if ( !pixel_color8 )
                        * ptr &= ~mask;
                    else
                        *ptr |= mask;
                    ptr += dest->pitch ;
                }
            }
        }

        case 8:
        {
            uint8_t * ptr = dest->data ;
            ptr += dest->pitch * y + x ;
            if ( drawing_stipple != 0xFFFFFFFF )
            {
                while ( h-- >= 0 )
                {
                    if ( drawing_stipple & 1 ) _Pixel8( ptr, pixel_color8 ) ;
                    drawing_stipple = (( drawing_stipple << 1 ) | (( drawing_stipple & 0x80000000 ) ? 1 : 0 ) );
                    ptr += dest->pitch ;
                }
            }
            else
            {
                while ( h-- >= 0 )
                {
                    _Pixel8( ptr, pixel_color8 ) ;
                    ptr += dest->pitch ;
                }
            }
        }
        break;

        case 16:
        {
            uint16_t * ptr = dest->data ;
            int inc = dest->pitch >> 1 ;
            ptr += inc * y + x ;
            if ( drawing_stipple != 0xFFFFFFFF )
            {
                while ( h-- >= 0 )
                {
                    if ( drawing_stipple & 1 ) _Pixel16( ptr, pixel_color16, pixel_color16_alpha ) ;
                    drawing_stipple = (( drawing_stipple << 1 ) | (( drawing_stipple & 0x80000000 ) ? 1 : 0 ) );
                    ptr += inc ;
                }
            }
            else
            {
                while ( h-- >= 0 )
                {
                    _Pixel16( ptr, pixel_color16, pixel_color16_alpha ) ;
                    ptr += inc ;
                }
            }
        }
        break;

        case 32:
        {
            uint32_t * ptr = dest->data ;
            int inc = dest->pitch >> 2 ;
            ptr += inc * y + x ;
            if ( drawing_stipple != 0xFFFFFFFF )
            {
                if ( pixel_alpha == 255 && ( pixel_color32 & 0xff000000 ) == 0xff000000 )
                {
                    while ( h-- >= 0 )
                    {
                        if ( drawing_stipple & 1 ) * ptr = pixel_color32 ;
                        drawing_stipple = (( drawing_stipple << 1 ) | (( drawing_stipple & 0x80000000 ) ? 1 : 0 ) );
                        ptr += inc ;
                    }
                }
                else
                {
                    unsigned int _f = pixel_color32 & 0xff000000, _f2 ;
                    unsigned int _c1, _c2, _c3 ;
                    unsigned int r, g, b ;

                    _f = ( _f >> 24 ) * pixel_alpha / 255 ;
                    _f2 = 255 - _f ;

                    if ( _f != 0x000000ff )
                    {
                        _c1 = ( pixel_color32 & 0x00ff0000 ) * _f ;
                        _c2 = ( pixel_color32 & 0x0000ff00 ) * _f ;
                        _c3 = ( pixel_color32 & 0x000000ff ) * _f ;

                        _f <<= 24 ;

                        while ( h-- >= 0 )
                        {
                            if ( drawing_stipple & 1 )
                            {
                                r = ( _c1 + (( *ptr & 0x00ff0000 ) * _f2 ) ) >> 8 ;
                                g = ( _c2 + (( *ptr & 0x0000ff00 ) * _f2 ) ) >> 8 ;
                                b = ( _c3 + (( *ptr & 0x000000ff ) * _f2 ) ) >> 8 ;

                                if ( r > 0x00ff0000 ) r = 0x00ff0000 ; else r &= 0x00ff0000 ;
                                if ( g > 0x0000ff00 ) g = 0x0000ff00 ; else g &= 0x0000ff00 ;
                                if ( b > 0x000000ff ) b = 0x000000ff ; else b &= 0x000000ff ;

                                *ptr = _f | r | g | b ;
                            }
                            drawing_stipple = (( drawing_stipple << 1 ) | (( drawing_stipple & 0x80000000 ) ? 1 : 0 ) );
                            ptr += inc;
                        }
                    }
                    else
                    {
                        _c1 = ( pixel_color32 & 0x00ff0000 ) * pixel_alpha ;
                        _c2 = ( pixel_color32 & 0x0000ff00 ) * pixel_alpha ;
                        _c3 = ( pixel_color32 & 0x000000ff ) * pixel_alpha ;

                        while ( h-- >= 0 )
                        {
                            if ( drawing_stipple & 1 )
                            {
                                r = ( _c1 + (( *ptr & 0x00ff0000 ) * _f2 ) ) >> 8 ;
                                g = ( _c2 + (( *ptr & 0x0000ff00 ) * _f2 ) ) >> 8 ;
                                b = ( _c3 + (( *ptr & 0x000000ff ) * _f2 ) ) >> 8 ;

                                if ( r > 0x00ff0000 ) r = 0x00ff0000 ; else r &= 0x00ff0000 ;
                                if ( g > 0x0000ff00 ) g = 0x0000ff00 ; else g &= 0x0000ff00 ;
                                if ( b > 0x000000ff ) b = 0x000000ff ; else b &= 0x000000ff ;

                                *ptr = 0xff000000 | r | g | b ;
                            }
                            drawing_stipple = (( drawing_stipple << 1 ) | (( drawing_stipple & 0x80000000 ) ? 1 : 0 ) );
                            ptr += inc;
                        }
                    }
                }
            }
            else
            {
                if ( pixel_alpha == 255 && ( pixel_color32 & 0xff000000 ) == 0xff000000 )
                {
                    while ( h-- >= 0 )
                    {
                        * ptr = pixel_color32 ;
                        ptr += inc ;
                    }
                }
                else
                {
                    unsigned int _f = pixel_color32 & 0xff000000, _f2 ;
                    unsigned int _c1, _c2, _c3 ;
                    unsigned int r, g, b ;

                    _f = ( _f >> 24 ) * pixel_alpha / 255 ;
                    _f2 = 255 - _f ;

                    if ( _f != 0x000000ff )
                    {
                        _c1 = ( pixel_color32 & 0x00ff0000 ) * _f ;
                        _c2 = ( pixel_color32 & 0x0000ff00 ) * _f ;
                        _c3 = ( pixel_color32 & 0x000000ff ) * _f ;

                        _f <<= 24 ;

                        while ( h-- >= 0 )
                        {
                            r = ( _c1 + (( *ptr & 0x00ff0000 ) * _f2 ) ) >> 8 ;
                            g = ( _c2 + (( *ptr & 0x0000ff00 ) * _f2 ) ) >> 8 ;
                            b = ( _c3 + (( *ptr & 0x000000ff ) * _f2 ) ) >> 8 ;

                            if ( r > 0x00ff0000 ) r = 0x00ff0000 ; else r &= 0x00ff0000 ;
                            if ( g > 0x0000ff00 ) g = 0x0000ff00 ; else g &= 0x0000ff00 ;
                            if ( b > 0x000000ff ) b = 0x000000ff ; else b &= 0x000000ff ;

                            *ptr = _f | r | g | b ;
                            ptr += inc;
                        }
                    }
                    else
                    {
                        _c1 = ( pixel_color32 & 0x00ff0000 ) * pixel_alpha ;
                        _c2 = ( pixel_color32 & 0x0000ff00 ) * pixel_alpha ;
                        _c3 = ( pixel_color32 & 0x000000ff ) * pixel_alpha ;

                        while ( h-- >= 0 )
                        {
                            r = ( _c1 + (( *ptr & 0x00ff0000 ) * _f2 ) ) >> 8 ;
                            g = ( _c2 + (( *ptr & 0x0000ff00 ) * _f2 ) ) >> 8 ;
                            b = ( _c3 + (( *ptr & 0x000000ff ) * _f2 ) ) >> 8 ;

                            if ( r > 0x00ff0000 ) r = 0x00ff0000 ; else r &= 0x00ff0000 ;
                            if ( g > 0x0000ff00 ) g = 0x0000ff00 ; else g &= 0x0000ff00 ;
                            if ( b > 0x000000ff ) b = 0x000000ff ; else b &= 0x000000ff ;

                            *ptr = 0xff000000 | r | g | b ;
                            ptr += inc;
                        }
                    }
                }
            }
        }
        break;
    }

    drawing_stipple = old_stipple;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : gr_gline
 *
 *  Draw an horizontal line
 *
 *  PARAMS :
 *      dest            Destination bitmap or NULL for screen
 *      clip            Clipping region or NULL for the whole screen
 *      x, y            Coordinates of the top-left pixel
 *      w               Width in pixels
 *
 *  RETURN VALUE :
 *      None
 *
 */

void draw_hline( GRAPH * dest, REGION * clip, int x, int y, int w )
{
    REGION base_clip ;
    int old_stipple = drawing_stipple;

    if ( !dest ) dest = scrbitmap ;
    if ( !clip )
    {
        clip = &base_clip ;
        clip->x = 0 ;
        clip->y = 0 ;
        clip->x2 = dest->width - 1 ;
        clip->y2 = dest->height - 1 ;
    }
    else
    {
        base_clip = *clip ;
        clip = &base_clip ;
        clip->x = MAX( MIN( clip->x, clip->x2 ), 0 ) ;
        clip->y = MAX( MIN( clip->y, clip->y2 ), 0 ) ;
        clip->x2 = MIN( MAX( clip->x, clip->x2 ), dest->width - 1 ) ;
        clip->y2 = MIN( MAX( clip->y, clip->y2 ), dest->height - 1 ) ;
    }

    dest->modified = 2 ;
    dest->info_flags &= ~GI_CLEAN;

    if ( w < 0 ) w = -w, x -= ( w /*- 1*/ ) ;

    if ( y < clip->y || y > clip->y2 ) return ;

    if ( x < clip->x ) w += x - clip->x, x = clip->x ;
    if ( x + w > clip->x2 ) w = clip->x2 - x /* + 1*/ ;

    switch ( dest->format->depth )
    {
        case 1:
        {
            uint8_t * ptr = dest->data;
            int mask ;
            ptr += dest->pitch * y + ( x >> 3 );
            mask = ( 1 << ( 7 - ( x & 7 ) ) );

            if ( drawing_stipple != 0xFFFFFFFF )
            {
                while ( w-- > 0 )
                {
                    if ( drawing_stipple & 1 )
                    {
                        if ( !pixel_color8 )
                            * ptr &= ~mask;
                        else
                            *ptr |= mask;
                    }
                    mask >>= 1;
                    if ( !mask )
                    {
                        mask = 0x80;
                        ptr++;
                    }
                    drawing_stipple = (( drawing_stipple << 1 ) | (( drawing_stipple & 0x80000000 ) ? 1 : 0 ) );
                }
            }
            else
            {
                while ( w-- > 0 )
                {
                    if ( !pixel_color8 )
                        * ptr &= ~mask;
                    else
                        *ptr |= mask;
                    mask >>= 1;
                    if ( !mask )
                    {
                        mask = 0x80;
                        ptr++;
                    }
                }
            }
        }
        break;

        case    8:
        {
            uint8_t * ptr = dest->data ;
            ptr += dest->pitch * y + x ;
            if ( drawing_stipple == 0xFFFFFFFF )
                _HLine8_nostipple( ptr, w ) ;
            else
                _HLine8_stipple( ptr, w ) ;
        }
        break;

        case 16:
        {
            uint16_t * ptr = dest->data ;
            ptr += ( dest->pitch >> 1 ) * y + x ;
            if ( drawing_stipple == 0xFFFFFFFF )
                _HLine16_nostipple( ptr, w ) ;
            else
                _HLine16_stipple( ptr, w ) ;
        }
        break;

        case 32:
        {
            uint32_t * ptr = dest->data ;
            ptr += ( dest->pitch >> 2 ) * y + x ;
            if ( drawing_stipple == 0xFFFFFFFF )
                _HLine32_nostipple( ptr, w ) ;
            else
                _HLine32_stipple( ptr, w ) ;
        }
        break;
    }

    drawing_stipple = old_stipple;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : draw_box
 *
 *  Draw a filled rectangle
 *
 *  PARAMS :
 *      dest            Destination bitmap or NULL for screen
 *      clip            Clipping region or NULL for the whole screen
 *      x, y            Coordinates of the top-left pixel
 *      w               Width in pixels
 *      h               Height in pixels
 *
 *  RETURN VALUE :
 *      None
 *
 */

void draw_box( GRAPH * dest, REGION * clip, int x, int y, int w, int h )
{
    REGION base_clip ;

    if ( !dest ) dest = scrbitmap ;
    if ( !clip )
    {
        clip = &base_clip ;
        clip->x = 0 ;
        clip->y = 0 ;
        clip->x2 = dest->width - 1 ;
        clip->y2 = dest->height - 1 ;
    }
    else
    {
        base_clip = *clip ;
        clip = &base_clip ;
        clip->x = MAX( MIN( clip->x, clip->x2 ), 0 ) ;
        clip->y = MAX( MIN( clip->y, clip->y2 ), 0 ) ;
        clip->x2 = MIN( MAX( clip->x, clip->x2 ), dest->width - 1 ) ;
        clip->y2 = MIN( MAX( clip->y, clip->y2 ), dest->height - 1 ) ;
    }

    dest->modified = 2 ;
    dest->info_flags &= ~GI_CLEAN;

    if ( w < 0 ) w = -w, x -= ( w /*- 1*/ ) ;
    if ( h < 0 ) h = -h, y -= ( h /*- 1*/ ) ;

    if ( x < clip->x ) w += x - clip->x, x = clip->x ;
    if ( y < clip->y ) h += y - clip->y, y = clip->y ;

    if ( x + w > clip->x2 ) w = clip->x2 - ( x /*+ 1*/ ) ;
    if ( y + h > clip->y2 ) h = clip->y2 - ( y /*+ 1*/ ) ;

    if ( w < 0 || h < 0 ) return ;

    switch ( dest->format->depth )
    {
        case 1:
        {
            int old_stipple = drawing_stipple;
            drawing_stipple = 0xFFFFFFFF;

            while ( h-- >= 0 ) draw_hline( dest, clip, x, y + h, w );

            drawing_stipple = old_stipple;
        }
        break;

        case 8:
        {
            uint8_t * ptr = dest->data ;
            ptr += dest->pitch * y + x ;
            while ( h-- >= 0 )
            {
                _HLine8_nostipple( ptr, w ) ;
                ptr += dest->pitch ;
            }
        }
        break;

        case 16:
        {
            uint16_t * ptr = dest->data ;
            int inc = dest->pitch >> 1 ;
            ptr += inc * y + x ;
            while ( h-- >= 0 )
            {
                _HLine16_nostipple( ptr, w ) ;
                ptr += inc ;
            }
        }
        break;

        case 32:
        {
            uint32_t * ptr = dest->data ;
            int inc = dest->pitch >> 2 ;
            ptr += inc * y + x ;
            while ( h-- >= 0 )
            {
                _HLine32_nostipple( ptr, w ) ;
                ptr += inc ;
            }
        }
        break;
    }
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : draw_rectangle
 *
 *  Draw a rectangle (non-filled)
 *
 *  PARAMS :
 *      dest            Destination bitmap or NULL for screen
 *      clip            Clipping region or NULL for the whole screen
 *      x, y            Coordinates of the top-left pixel
 *      w               Width in pixels
 *      h               Height in pixels
 *
 *  RETURN VALUE :
 *      None
 *
 */

void draw_rectangle( GRAPH * dest, REGION * clip, int x, int y, int w, int h )
{
    int stipple = drawing_stipple ;
    int iw, ih;

    iw = SGN( w );
    ih = SGN( h );

    draw_hline( dest, clip, x, y, w - iw );
    if ( h ) draw_vline( dest, clip, x + w, y, h - ih );
    if ( w && h ) draw_hline( dest, clip, x + w, y + h, -( w - iw ) );
    if ( h && w ) draw_vline( dest, clip, x, y + h, -( h - ih ) );

    drawing_stipple = stipple ;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : draw_circle
 *
 *  Draw a circle
 *
 *  PARAMS :
 *      dest            Destination bitmap or NULL for screen
 *      clip            Clipping region or NULL for the whole screen
 *      x, y            Coordinates of the center
 *      r               Radius, in pixels
 *
 *  RETURN VALUE :
 *      None
 *
 */

void draw_circle( GRAPH * dest, REGION * clip, int x, int y, int r )
{
    int cx = 0, cy = r;
    int lcy = -1;
    int df = 1 - r, de = 3, dse = -2 * r + 5;
    int old_stipple = drawing_stipple;
    REGION base_clip ;
    int color = 0;

    if ( !dest ) dest = scrbitmap ;
    if ( !clip )
    {
        clip = &base_clip ;
        clip->x = 0 ;
        clip->y = 0 ;
        clip->x2 = dest->width - 1 ;
        clip->y2 = dest->height - 1 ;
    }
    else
    {
        base_clip = *clip ;
        clip = &base_clip ;
        clip->x = MAX( MIN( clip->x, clip->x2 ), 0 ) ;
        clip->y = MAX( MIN( clip->y, clip->y2 ), 0 ) ;
        clip->x2 = MIN( MAX( clip->x, clip->x2 ), dest->width - 1 ) ;
        clip->y2 = MIN( MAX( clip->y, clip->y2 ), dest->height - 1 ) ;
    }

    dest->modified = 2 ;
    dest->info_flags &= ~GI_CLEAN;

    if ( dest->format->depth == 8 || dest->format->depth == 1 )
    {
        color = pixel_color8;
    }
    else if ( dest->format->depth == 16 )
    {
        color = pixel_color16;
    }
    else if ( dest->format->depth == 32 )
    {
        color = pixel_color32;
    }

    do
    {
        if ( drawing_stipple & 1 )
        {
            gr_put_pixelc( dest, clip, x - cx, y - cy, color ) ;
            if ( cx ) gr_put_pixelc( dest, clip, x + cx, y - cy, color ) ;

            if ( cy )
            {
                gr_put_pixelc( dest, clip, x - cx, y + cy, color ) ;
                if ( cx ) gr_put_pixelc( dest, clip, x + cx, y + cy, color ) ;
            }

            if ( cx != cy )
            {
                gr_put_pixelc( dest, clip, x - cy, y - cx, color ) ;
                if ( cy ) gr_put_pixelc( dest, clip, x + cy, y - cx, color ) ;
            }

            if ( cx && cy != cx )
            {
                gr_put_pixelc( dest, clip, x - cy, y + cx, color ) ;
                if ( cy ) gr_put_pixelc( dest, clip, x + cy, y + cx, color ) ;
            }
        }
        drawing_stipple = (( drawing_stipple << 1 ) | (( drawing_stipple & 0x80000000 ) ? 1 : 0 ) );

        lcy = cy;
        cx++ ;
        if ( df < 0 ) df += de, de += 2, dse += 2 ;
        else df += dse, de += 2, dse += 4, cy-- ;
    }
    while ( cx <= cy ) ;

    drawing_stipple = old_stipple;

}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : draw_fcircle
 *
 *  Draw a filled circle
 *
 *  PARAMS :
 *      dest            Destination bitmap or NULL for screen
 *      clip            Clipping region or NULL for the whole screen
 *      x, y            Coordinates of the center
 *      r               Radius, in pixels
 *
 *  RETURN VALUE :
 *      None
 *
 */

void draw_fcircle( GRAPH * dest, REGION * clip, int x, int y, int r )
{
    int cx = 0, cy = r;
    int df = 1 - r, de = 3, dse = -2 * r + 5;
    int old_stipple = drawing_stipple;
    drawing_stipple = 0xFFFFFFFF;

    do
    {
        if ( cx != cy )
        {
            draw_hline( dest, clip, x - cy, y - cx, cy << 1 /*+1*/ ) ;
            if ( cx ) draw_hline( dest, clip, x - cy, y + cx, cy << 1 /*+1*/ ) ;
        }
        if ( df < 0 )
        {
            df += de, de += 2, dse += 2 ;
        }
        else
        {
            df += dse, de += 2, dse += 4;
            draw_hline( dest, clip, x - cx, y - cy, cx << 1 /*+1*/ ) ;
            if ( cy ) draw_hline( dest, clip, x - cx, y + cy, cx << 1 /*+1*/ ) ;
            cy-- ;
        }
        cx++ ;
    }
    while ( cx <= cy ) ;

    drawing_stipple = old_stipple;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : draw_line
 *
 *  Draw a line
 *
 *  PARAMS :
 *      dest            Destination bitmap or NULL for screen
 *      clip            Clipping region or NULL for the whole screen
 *      x, y            Coordinates of the first point
 *      w, h            Distance to the second point
 *
 *  RETURN VALUE :
 *      None
 *
 */

void draw_line( GRAPH * dest, REGION * clip, int x, int y, int w, int h )
{
    int n, m, hinc, vinc ;
    REGION base_clip ;
    int old_stipple = drawing_stipple;
    int i1, i2, dd ;
    int w1, h1;

    if ( !w )
    {
        draw_vline( dest, clip, x, y, h ) ;
        return ;
    }

    if ( !h )
    {
        draw_hline( dest, clip, x, y, w ) ;
        return ;
    }

    if ( !dest ) dest = scrbitmap ;
    if ( !clip )
    {
        clip = &base_clip ;
        clip->x = 0 ;
        clip->y = 0 ;
        clip->x2 = dest->width - 1 ;
        clip->y2 = dest->height - 1 ;
    }
    else
    {
        base_clip = *clip ;
        clip = &base_clip ;
        clip->x = MAX( MIN( clip->x, clip->x2 ), 0 ) ;
        clip->y = MAX( MIN( clip->y, clip->y2 ), 0 ) ;
        clip->x2 = MIN( MAX( clip->x, clip->x2 ), dest->width - 1 ) ;
        clip->y2 = MIN( MAX( clip->y, clip->y2 ), dest->height - 1 ) ;
    }

    dest->modified = 2 ;
    dest->info_flags &= ~GI_CLEAN;

    w1 = ABS( w );
    h1 = ABS( h );

    /* Clipping de la línea - INCORRECTO pero funcional */

    /* TODO: SE NECESITA CORREGIR CLIPPING EN LINE */

    if ( x < clip->x )
    { /* izquierda */
        if (( x + w ) < clip->x ) return ;
        n = clip->x - x;
        m = w ? ( n * h ) / ABS( w ) : 0;
        x += n, w -= n, y += m, h -= m ;    // if (w == 0) return ;
    }
    if (( x + w ) < clip->x )
    { /* w < 0 */
        n = clip->x - ( x + w );
        m = w ? ( n * h ) / ABS( w ) : 0;
        w += n, h -= m ; // if (w == 0) return ;
    }
    if ( y < clip->y )
    { /* arriba */
        if (( y + h ) < clip->y ) return ;
        m = clip->y - y;
        n = h ? ( m * w ) / ABS( h ) : 0;
        x += n, w -= n, y += m, h -= m ; // if (h == 0) return ;
    }
    if (( y + h ) < clip->y )
    { /* h < 0 */
        m = clip->y - ( y + h );
        n = h ? ( m * w ) / ABS( h ) : 0;
        w -= n, h += m ; // if (h == 0) return ;
    }
    if ( x > clip->x2 )
    { /* derecha */
        if (( x + w ) > clip->x2 ) return ;
        n = x - clip->x2;
        m = w ? ( n * h ) / ABS( w ) : 0;
        x -= n, w += n, y += m, h -= m ; // if (w == 0) return ;
    }
    if (( x + w ) > clip->x2 )
    { /* w > 0 */
        n = ( x + w ) - clip->x2;
        m = w ? ( n * h ) / ABS( w ) : 0;
        w -= n, h -= m ; // if (w == 0) return ;
    }
    if ( y > clip->y2 )
    { /* abajo */
        if (( y + h ) > clip->y2 ) return ;
        m = y - clip->y2;
        n = h ? ( m * w ) / ABS( h ) : 0;
        x += n, w -= n, y -= m, h += m ; // if (h == 0) return ;
    }
    if (( y + h ) > clip->y2 )
    { /* h > 0 */
        m = ( y + h ) - clip->y2;
        n = h ? ( m * w ) / ABS( h ) : 0;
        w -= n, h -= m ; // if (h == 0) return ;
    }

    hinc = ( w > 0 ) ? 1 : -1 ;
    vinc = ( h > 0 ) ? dest->pitch : -( int ) dest->pitch ;

    if ( dest->format->depth > 8 ) vinc /= dest->format->depthb;

    /* Aquí va una implementación deprisa y corriendo de Bresenham */

    w = ABS( w );
    h = ABS( h );

    if ( w > h )
    {
        i1 = 2 * h1;
        dd = i1 - w1;
        i2 = dd - w1;
        dd -= w1 - w;
    }
    else
    {
        i1 = 2 * w1;
        dd = i1 - h1;
        i2 = dd - h1;
        dd -= h1 - h;
    }

    switch ( dest->format->depth )
    {
        case 1:
        {
            uint8_t * ptr = dest->data + dest->pitch * y + ( x >> 3 ) ;
            uint8_t mask, rmask ;
            mask = ( 1 << ( 7 - ( x & 7 ) ) );

            if ( hinc < 0 ) rmask = 0x01 ;
            else rmask = 0x80 ;

            if ( w > h ) while ( w-- > 0 )
                {
                    if ( drawing_stipple & 1 )
                    {
                        if ( !pixel_color8 )
                        {
                            * ptr &= ~mask;
                        }
                        else
                        {
                            *ptr |= mask;
                        }
                    }
                    drawing_stipple = (( drawing_stipple << 1 ) | (( drawing_stipple & 0x80000000 ) ? 1 : 0 ) );
                    /* Vertical */
                    if ( dd >= 0 )
                    {
                        ptr += vinc, dd += i2 ;
                    }
                    else
                    {
                        dd += i1 ;
                    }
                    /* Horizontal */
                    if ( hinc < 0 )
                    {
                        mask <<= 1;
                    }
                    else
                    {
                        mask >>= 1;
                    }
                    if ( !mask ) mask = rmask, ptr += hinc ;
                }
            else while ( h-- >= 0 )
                {
                    if ( drawing_stipple & 1 )
                    {
                        if ( !pixel_color8 )
                        {
                            * ptr &= ~mask ;
                        }
                        else
                        {
                            *ptr |= mask;
                        }
                    }
                    drawing_stipple = (( drawing_stipple << 1 ) | (( drawing_stipple & 0x80000000 ) ? 1 : 0 ) );
                    /* Vertical */
                    ptr += vinc;
                    if ( dd >= 0 )
                    {
                        dd += i2 ;
                        /* Horizontal */
                        if ( hinc < 0 )
                        {
                            mask <<= 1;
                        }
                        else
                        {
                            mask >>= 1;
                        }
                        if ( !mask ) mask = rmask, ptr += hinc ;
                    }
                    else
                        dd += i1 ;
                }
        }
        break;

        case 8:
        {
            uint8_t * ptr = ( uint8_t * ) dest->data + dest->pitch * y + x;

            if ( w > h ) while ( w-- > 0 )
                {
                    if ( drawing_stipple & 1 ) _Pixel8( ptr, pixel_color8 ) ;
                    drawing_stipple = (( drawing_stipple << 1 ) | (( drawing_stipple & 0x80000000 ) ? 1 : 0 ) );
                    ptr += hinc;
                    if ( dd >= 0 ) ptr += vinc, dd += i2 ;
                    else dd += i1 ;
                }
            else while ( h-- >= 0 )
                {
                    if ( drawing_stipple & 1 ) _Pixel8( ptr, pixel_color8 ) ;
                    drawing_stipple = (( drawing_stipple << 1 ) | (( drawing_stipple & 0x80000000 ) ? 1 : 0 ) );
                    ptr += vinc;
                    if ( dd >= 0 ) ptr += hinc, dd += i2 ;
                    else dd += i1 ;
                }
        }
        break;
        case 16:
        {
            uint16_t * ptr = ( uint16_t * ) dest->data + ( dest->pitch >> 1 ) * y + x;

            if ( w > h ) while ( w-- > 0 )
                {
                    if ( drawing_stipple & 1 ) _Pixel16( ptr, pixel_color16, pixel_color16_alpha ) ;
                    drawing_stipple = (( drawing_stipple << 1 ) | (( drawing_stipple & 0x80000000 ) ? 1 : 0 ) );
                    ptr += hinc;
                    if ( dd >= 0 ) ptr += vinc, dd += i2 ;
                    else dd += i1 ;
                }
            else while ( h-- >= 0 )
                {
                    if ( drawing_stipple & 1 ) _Pixel16( ptr, pixel_color16, pixel_color16_alpha ) ;
                    drawing_stipple = (( drawing_stipple << 1 ) | (( drawing_stipple & 0x80000000 ) ? 1 : 0 ) );
                    ptr += vinc;
                    if ( dd >= 0 ) ptr += hinc, dd += i2 ;
                    else dd += i1 ;
                }
        }
        break;
        case 32:
        {
            uint32_t * ptr = ( uint32_t * ) dest->data + ( dest->pitch >> 2 ) * y + x;

            if ( drawing_stipple == 0xffffffff )
            {
                if ( w > h )
                {
                    if ( pixel_alpha == 255 && ( pixel_color32 & 0xff000000 ) == 0xff000000 )
                    {
                        while ( w-- > 0 )
                        {
                            *ptr = pixel_color32 ;
                            ptr += hinc;
                            if ( dd >= 0 ) ptr += vinc, dd += i2 ;
                            else dd += i1 ;
                        }
                    }
                    else
                    {
                        unsigned int _f = pixel_color32 & 0xff000000, _f2 ;
                        unsigned int _c1, _c2, _c3 ;
                        unsigned int r, g, b ;

                        _f = ( _f >> 24 ) * pixel_alpha / 255 ;
                        _f2 = 255 - _f ;

                        if ( _f != 0x000000ff )
                        {
                            _c1 = ( pixel_color32 & 0x00ff0000 ) * _f ;
                            _c2 = ( pixel_color32 & 0x0000ff00 ) * _f ;
                            _c3 = ( pixel_color32 & 0x000000ff ) * _f ;

                            _f <<= 24 ;

                            while ( w-- > 0 )
                            {
                                r = ( _c1 + (( *ptr & 0x00ff0000 ) * _f2 ) ) >> 8 ;
                                g = ( _c2 + (( *ptr & 0x0000ff00 ) * _f2 ) ) >> 8 ;
                                b = ( _c3 + (( *ptr & 0x000000ff ) * _f2 ) ) >> 8 ;

                                if ( r > 0x00ff0000 ) r = 0x00ff0000 ; else r &= 0x00ff0000 ;
                                if ( g > 0x0000ff00 ) g = 0x0000ff00 ; else g &= 0x0000ff00 ;
                                if ( b > 0x000000ff ) b = 0x000000ff ; else b &= 0x000000ff ;

                                *ptr = _f | r | g | b ;
                                ptr += hinc;
                                if ( dd >= 0 ) ptr += vinc, dd += i2 ;
                                else dd += i1 ;
                            }
                        }
                        else
                        {
                            _c1 = ( pixel_color32 & 0x00ff0000 ) * pixel_alpha ;
                            _c2 = ( pixel_color32 & 0x0000ff00 ) * pixel_alpha ;
                            _c3 = ( pixel_color32 & 0x000000ff ) * pixel_alpha ;

                            while ( w-- > 0 )
                            {
                                r = ( _c1 + (( *ptr & 0x00ff0000 ) * _f2 ) ) >> 8 ;
                                g = ( _c2 + (( *ptr & 0x0000ff00 ) * _f2 ) ) >> 8 ;
                                b = ( _c3 + (( *ptr & 0x000000ff ) * _f2 ) ) >> 8 ;

                                if ( r > 0x00ff0000 ) r = 0x00ff0000 ; else r &= 0x00ff0000 ;
                                if ( g > 0x0000ff00 ) g = 0x0000ff00 ; else g &= 0x0000ff00 ;
                                if ( b > 0x000000ff ) b = 0x000000ff ; else b &= 0x000000ff ;

                                *ptr = 0xff000000 | r | g | b ;
                                ptr += hinc;
                                if ( dd >= 0 ) ptr += vinc, dd += i2 ;
                                else dd += i1 ;
                            }
                        }
                    }
                }
                else
                {
                    if ( pixel_alpha == 255 && ( pixel_color32 & 0xff000000 ) == 0xff000000 )
                    {
                        while ( h-- >= 0 )
                        {
                            *ptr = pixel_color32 ;
                            ptr += vinc;
                            if ( dd >= 0 ) ptr += hinc, dd += i2 ;
                            else dd += i1 ;
                        }
                    }
                    else
                    {
                        unsigned int _f = pixel_color32 & 0xff000000, _f2 ;
                        unsigned int _c1, _c2, _c3 ;
                        unsigned int r, g, b ;

                        _f = ( _f >> 24 ) * pixel_alpha / 255 ;
                        _f2 = 255 - _f ;

                        if ( _f != 0x000000ff )
                        {
                            _c1 = ( pixel_color32 & 0x00ff0000 ) * _f ;
                            _c2 = ( pixel_color32 & 0x0000ff00 ) * _f ;
                            _c3 = ( pixel_color32 & 0x000000ff ) * _f ;

                            _f <<= 24 ;

                            while ( h-- >= 0 )
                            {
                                r = ( _c1 + (( *ptr & 0x00ff0000 ) * _f2 ) ) >> 8 ;
                                g = ( _c2 + (( *ptr & 0x0000ff00 ) * _f2 ) ) >> 8 ;
                                b = ( _c3 + (( *ptr & 0x000000ff ) * _f2 ) ) >> 8 ;

                                if ( r > 0x00ff0000 ) r = 0x00ff0000 ; else r &= 0x00ff0000 ;
                                if ( g > 0x0000ff00 ) g = 0x0000ff00 ; else g &= 0x0000ff00 ;
                                if ( b > 0x000000ff ) b = 0x000000ff ; else b &= 0x000000ff ;

                                *ptr = _f | r | g | b ;
                                ptr += vinc;
                                if ( dd >= 0 ) ptr += hinc, dd += i2 ;
                                else dd += i1 ;
                            }
                        }
                        else
                        {
                            _c1 = ( pixel_color32 & 0x00ff0000 ) * pixel_alpha ;
                            _c2 = ( pixel_color32 & 0x0000ff00 ) * pixel_alpha ;
                            _c3 = ( pixel_color32 & 0x000000ff ) * pixel_alpha ;

                            while ( h-- >= 0 )
                            {
                                r = ( _c1 + (( *ptr & 0x00ff0000 ) * _f2 ) ) >> 8 ;
                                g = ( _c2 + (( *ptr & 0x0000ff00 ) * _f2 ) ) >> 8 ;
                                b = ( _c3 + (( *ptr & 0x000000ff ) * _f2 ) ) >> 8 ;

                                if ( r > 0x00ff0000 ) r = 0x00ff0000 ; else r &= 0x00ff0000 ;
                                if ( g > 0x0000ff00 ) g = 0x0000ff00 ; else g &= 0x0000ff00 ;
                                if ( b > 0x000000ff ) b = 0x000000ff ; else b &= 0x000000ff ;

                                *ptr = 0xff000000 | r | g | b ;
                                ptr += vinc;
                                if ( dd >= 0 ) ptr += hinc, dd += i2 ;
                                else dd += i1 ;
                            }
                        }
                    }
                }
            }
            else
            {
                if ( w > h )
                {
                    if ( pixel_alpha == 255 && ( pixel_color32 & 0xff000000 ) == 0xff000000 )
                    {
                        while ( w-- > 0 )
                        {
                            if ( drawing_stipple & 1 ) *ptr = pixel_color32 ;
                            drawing_stipple = (( drawing_stipple << 1 ) | (( drawing_stipple & 0x80000000 ) ? 1 : 0 ) );
                            ptr += hinc;
                            if ( dd >= 0 ) ptr += vinc, dd += i2 ;
                            else dd += i1 ;
                        }
                    }
                    else
                    {
                        unsigned int _f = pixel_color32 & 0xff000000, _f2 ;
                        unsigned int _c1, _c2, _c3 ;
                        unsigned int r, g, b ;

                        _f = ( _f >> 24 ) * pixel_alpha / 255 ;
                        _f2 = 255 - _f ;

                        if ( _f != 0x000000ff )
                        {
                            _c1 = ( pixel_color32 & 0x00ff0000 ) * _f ;
                            _c2 = ( pixel_color32 & 0x0000ff00 ) * _f ;
                            _c3 = ( pixel_color32 & 0x000000ff ) * _f ;

                            _f <<= 24 ;

                            while ( w-- > 0 )
                            {
                                if ( drawing_stipple & 1 )
                                {
                                    r = ( _c1 + (( *ptr & 0x00ff0000 ) * _f2 ) ) >> 8 ;
                                    g = ( _c2 + (( *ptr & 0x0000ff00 ) * _f2 ) ) >> 8 ;
                                    b = ( _c3 + (( *ptr & 0x000000ff ) * _f2 ) ) >> 8 ;

                                    if ( r > 0x00ff0000 ) r = 0x00ff0000 ; else r &= 0x00ff0000 ;
                                    if ( g > 0x0000ff00 ) g = 0x0000ff00 ; else g &= 0x0000ff00 ;
                                    if ( b > 0x000000ff ) b = 0x000000ff ; else b &= 0x000000ff ;

                                    *ptr = _f | r | g | b ;
                                }
                                drawing_stipple = (( drawing_stipple << 1 ) | (( drawing_stipple & 0x80000000 ) ? 1 : 0 ) );
                                ptr += hinc;
                                if ( dd >= 0 ) ptr += vinc, dd += i2 ;
                                else dd += i1 ;
                            }
                        }
                        else
                        {
                            _c1 = ( pixel_color32 & 0x00ff0000 ) * pixel_alpha ;
                            _c2 = ( pixel_color32 & 0x0000ff00 ) * pixel_alpha ;
                            _c3 = ( pixel_color32 & 0x000000ff ) * pixel_alpha ;

                            while ( w-- > 0 )
                            {
                                if ( drawing_stipple & 1 )
                                {
                                    r = ( _c1 + (( *ptr & 0x00ff0000 ) * _f2 ) ) >> 8 ;
                                    g = ( _c2 + (( *ptr & 0x0000ff00 ) * _f2 ) ) >> 8 ;
                                    b = ( _c3 + (( *ptr & 0x000000ff ) * _f2 ) ) >> 8 ;

                                    if ( r > 0x00ff0000 ) r = 0x00ff0000 ; else r &= 0x00ff0000 ;
                                    if ( g > 0x0000ff00 ) g = 0x0000ff00 ; else g &= 0x0000ff00 ;
                                    if ( b > 0x000000ff ) b = 0x000000ff ; else b &= 0x000000ff ;

                                    *ptr = 0xff000000 | r | g | b ;
                                }
                                drawing_stipple = (( drawing_stipple << 1 ) | (( drawing_stipple & 0x80000000 ) ? 1 : 0 ) );
                                ptr += hinc;
                                if ( dd >= 0 ) ptr += vinc, dd += i2 ;
                                else dd += i1 ;
                            }
                        }
                    }
                }
                else
                {
                    if ( pixel_alpha == 255 && ( pixel_color32 & 0xff000000 ) == 0xff000000 )
                    {
                        while ( h-- >= 0 )
                        {
                            if ( drawing_stipple & 1 ) *ptr = pixel_color32 ;
                            drawing_stipple = (( drawing_stipple << 1 ) | (( drawing_stipple & 0x80000000 ) ? 1 : 0 ) );
                            ptr += vinc;
                            if ( dd >= 0 ) ptr += hinc, dd += i2 ;
                            else dd += i1 ;
                        }
                    }
                    else
                    {
                        unsigned int _f = pixel_color32 & 0xff000000, _f2 ;
                        unsigned int _c1, _c2, _c3 ;
                        unsigned int r, g, b ;

                        _f = ( _f >> 24 ) * pixel_alpha / 255 ;
                        _f2 = 255 - _f ;

                        if ( _f != 0x000000ff )
                        {
                            _c1 = ( pixel_color32 & 0x00ff0000 ) * _f ;
                            _c2 = ( pixel_color32 & 0x0000ff00 ) * _f ;
                            _c3 = ( pixel_color32 & 0x000000ff ) * _f ;

                            _f <<= 24 ;

                            while ( h-- >= 0 )
                            {
                                if ( drawing_stipple & 1 )
                                {
                                    r = ( _c1 + (( *ptr & 0x00ff0000 ) * _f2 ) ) >> 8 ;
                                    g = ( _c2 + (( *ptr & 0x0000ff00 ) * _f2 ) ) >> 8 ;
                                    b = ( _c3 + (( *ptr & 0x000000ff ) * _f2 ) ) >> 8 ;

                                    if ( r > 0x00ff0000 ) r = 0x00ff0000 ; else r &= 0x00ff0000 ;
                                    if ( g > 0x0000ff00 ) g = 0x0000ff00 ; else g &= 0x0000ff00 ;
                                    if ( b > 0x000000ff ) b = 0x000000ff ; else b &= 0x000000ff ;

                                    *ptr = _f | r | g | b ;
                                }
                                drawing_stipple = (( drawing_stipple << 1 ) | (( drawing_stipple & 0x80000000 ) ? 1 : 0 ) );
                                ptr += vinc;
                                if ( dd >= 0 ) ptr += hinc, dd += i2 ;
                                else dd += i1 ;
                            }
                        }
                        else
                        {
                            _c1 = ( pixel_color32 & 0x00ff0000 ) * pixel_alpha ;
                            _c2 = ( pixel_color32 & 0x0000ff00 ) * pixel_alpha ;
                            _c3 = ( pixel_color32 & 0x000000ff ) * pixel_alpha ;

                            while ( h-- >= 0 )
                            {
                                if ( drawing_stipple & 1 )
                                {
                                    r = ( _c1 + (( *ptr & 0x00ff0000 ) * _f2 ) ) >> 8 ;
                                    g = ( _c2 + (( *ptr & 0x0000ff00 ) * _f2 ) ) >> 8 ;
                                    b = ( _c3 + (( *ptr & 0x000000ff ) * _f2 ) ) >> 8 ;

                                    if ( r > 0x00ff0000 ) r = 0x00ff0000 ; else r &= 0x00ff0000 ;
                                    if ( g > 0x0000ff00 ) g = 0x0000ff00 ; else g &= 0x0000ff00 ;
                                    if ( b > 0x000000ff ) b = 0x000000ff ; else b &= 0x000000ff ;

                                    *ptr = 0xff000000 | r | g | b ;
                                }
                                drawing_stipple = (( drawing_stipple << 1 ) | (( drawing_stipple & 0x80000000 ) ? 1 : 0 ) );
                                ptr += vinc;
                                if ( dd >= 0 ) ptr += hinc, dd += i2 ;
                                else dd += i1 ;
                            }
                        }
                    }
                }
            }
        }
    }

    drawing_stipple = old_stipple;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : draw_bezier
 *
 *  Draw a bezier curve
 *
 *  PARAMS :
 *      dest            Destination bitmap or NULL for screen
 *      clip            Clipping region or NULL for the whole screen
 *      params          Pointer to an integer array with the parameters:
 *
 *          x1, y1
 *          x2, y2
 *          x3, y3
 *          x4, y4      Curve points
 *          level       Curve smoothness (1 to 15, 15 is more)
 *
 *  RETURN VALUE :
 *      None
 *
 */

void draw_bezier( GRAPH * dest, REGION * clip, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int level )
{
    float x = ( float ) x1, y = ( float ) y1;
    float xp = x, yp = y;
    float delta;
    float dx, d2x, d3x;
    float dy, d2y, d3y;
    float a, b, c;
    int i;
    int n = 1;

    dest->modified = 2 ;
    dest->info_flags &= ~GI_CLEAN;

    /* Compute number of iterations */

    if ( level < 1 ) level = 1;
    if ( level >= 15 ) level = 15;
    while ( level-- > 0 ) n *= 2;
    delta = 1.0f / ( float ) n;

    /* Compute finite differences */
    /* a, b, c are the coefficient of the polynom in t defining the parametric curve */
    /* The computation is done independently for x and y */

    a = ( float )( -x1 + 3 * x2 - 3 * x3 + x4 );
    b = ( float )( 3 * x1 - 6 * x2 + 3 * x3 );
    c = ( float )( -3 * x1 + 3 * x2 );

    d3x = 6 * a * delta * delta * delta;
    d2x = d3x + 2 * b * delta * delta;
    dx = a * delta * delta * delta + b * delta * delta + c * delta;

    a = ( float )( -y1 + 3 * y2 - 3 * y3 + y4 );
    b = ( float )( 3 * y1 - 6 * y2 + 3 * y3 );
    c = ( float )( -3 * y1 + 3 * y2 );

    d3y = 6 * a * delta * delta * delta;
    d2y = d3y + 2 * b * delta * delta;
    dy = a * delta * delta * delta + b * delta * delta + c * delta;

    for ( i = 0; i < n; i++ )
    {
        x += dx;
        dx += d2x;
        d2x += d3x;
        y += dy;
        dy += d2y;
        d2y += d3y;
        if (( int16_t )( xp ) != ( int16_t )( x ) || ( int16_t )( yp ) != ( int16_t )( y ) )
        {
            draw_line( dest, clip, ( int16_t ) xp, ( int16_t ) yp, ( int16_t ) x - ( int16_t ) xp, ( int16_t ) y - ( int16_t ) yp );
        }
        xp = x;
        yp = y;
    }
}

/* --------------------------------------------------------------------------- */
