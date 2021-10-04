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

#define __LIB_RENDER
#include "librender.h"
#include "fmath.h"

/* --------------------------------------------------------------------------- */

//uint8_t zonearray[ 128 / 8 ];
#ifdef __USE_BIT_MASK
static uint32_t zonearray[ /* DIRTYROWS * */ DIRTYCOLS ];
#else
static uint32_t zonearray[ DIRTYROWS ][ DIRTYCOLS ];
#endif

/* --------------------------------------------------------------------------- */

void gr_rects_clear()
{
    memset( zonearray, 0, sizeof( zonearray ) );
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : gr_mark_rect
 *
 *  Updates the given rectangle as a dirty zone at the 128-bits array
 *
 *  PARAMS :
 *      x, y                Top-left coordinate
 *      width               Width in pixels
 *      height              Height in pixels
 *
 *  RETURN VALUE :
 *      None
 */

void gr_mark_rect( int x, int y, int width, int height )
{
#ifdef __USE_BIT_MASK
    int cx, cy;
    int w, h;
    int iy, lx, ly;

    w = scr_width / DIRTYCOLS;
    h = scr_height / DIRTYROWS;

    width = ABS( width ) - 1;
    height = ABS( height ) - 1;

    x = MIN( x, x + width );
    y = MIN( y, y + height );

    iy = MAX( y / h, 0 );

    lx = MIN(( x + width ) / w, DIRTYCOLS - 1 );
    ly = MIN(( y + height ) / h, DIRTYROWS - 1 );

    for ( cx = MAX( x / w, 0 ); cx <= lx; cx++ )
        for ( cy = iy; cy <= ly; cy++ )
            zonearray[ cx ] |= ( 1 << cy );
#else
    int cx, cy;
    int w, h;
    int iy, lx, ly;

    w = scr_width / DIRTYCOLS;
    h = scr_height / DIRTYROWS;

    if ( w * DIRTYCOLS != scr_width ) w++;
    if ( h * DIRTYROWS != scr_height ) h++;

    width = ABS( width ) - 1;
    height = ABS( height ) - 1;

    x = MIN( x, x + width );
    y = MIN( y, y + height );

    iy = MAX( y / h, 0 );

    lx = MIN(( x + width ) / w, DIRTYCOLS - 1 );
    ly = MIN(( y + height ) / h, DIRTYROWS - 1 );

    for ( cx = MAX( x / w, 0 ); cx <= lx; cx++ )
        for ( cy = iy; cy <= ly; cy++ )
            zonearray[ cy ][ cx ] = 1;
#endif
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : gr_mark_rects
 *
 *  Given a 128-bits array of dirty screen zones, create an array of SDL_Rect regions
 *
 *  PARAMS :
 *      zonearray       Pointer to a 128-bits array with dirty screen zones = 1
 *      rects           Pointer to a 128 REGION array
 *
 *  RETURN VALUE :
 *      Number of rects filled
 */

int gr_mark_rects( REGION * rects )
{
    int count = 0, x, y;
    int w, h, cw, ch, x2;

    w = scr_width / DIRTYCOLS;
    h = scr_height / DIRTYROWS;

    if ( w * DIRTYCOLS != scr_width ) w++;
    if ( h * DIRTYROWS != scr_height ) h++;

    for ( x = 0; x < DIRTYCOLS; x++ )
    {
#ifdef __USE_BIT_MASK
        if ( zonearray[ x ] )
        {
            for ( y = 0; y < DIRTYROWS; y++ )
            {
                if ( zonearray[ x ] & ( 1 << y ) )
                {
                    zonearray[ x ] &= ~( 1 << y );
                    for ( cw = x + 1; ( cw < DIRTYCOLS ) && ( zonearray[ cw ] & ( 1 << y ) ); cw++ )
                        zonearray[ cw ] &= ~( 1 << y );

                    for ( ch = y + 1; ch < DIRTYROWS; ch++ )
                    {
                        /* Si hay algun hueco en el ancho de las siguiente lineas, corto aca,
                           y deja esta linea para otra recta */
                        for ( x2 = x; ( x2 < cw ) && ( zonearray[ x2 ] & ( 1 << ch ) ); x2++ );

                        if ( x2 < cw ) break;

                        /* Limpio bitmap de la recta actual */
                        for ( x2 = x; x2 < cw; x2++ )
                            zonearray[ x2 ] &= ~( 1 << ch );
                    }
                    rects[ count ].x = w * x;
                    rects[ count ].y = h * y;
                    rects[ count ].x2 = w * cw - 1 /* + rects[ count ].x */;
                    rects[ count ].y2 = h * ch - 1 /* + rects[ count ].y */;
                    count++;
                }
            }
        }
#else
        for ( y = 0; y < DIRTYROWS; y++ )
        {
            if ( zonearray[ y ][ x ] )
            {
                zonearray[ y ][ x ] = 0;
                for ( cw = x + 1; ( cw < DIRTYCOLS ) && ( zonearray[ y ][ cw ] ); cw++ ) zonearray[ y ][ cw ] = 0;

                for ( ch = y + 1; ch < DIRTYROWS; ch++ )
                {
                    /* Si hay algun hueco en el ancho de las siguiente lineas, corto aca,
                       y deja esta linea para otra recta */
                    for ( x2 = x; ( x2 < cw ) && ( zonearray[ ch ][ x2 ] ); x2++ );

                    if ( x2 < cw ) break;

                    /* Limpio bitmap de la recta actual */
                    for ( x2 = x; x2 < cw; x2++ ) zonearray[ ch ][ x2 ] = 0;
                }

                rects[ count ].x = w * x;
                rects[ count ].y = h * y;
                rects[ count ].x2 = MIN( w * cw - 1, scr_width - 1 );
                rects[ count ].y2 = MIN( h * ch - 1, scr_height - 1 );

                count++;
            }
        }
#endif
    }

    return count;
}

/* --------------------------------------------------------------------------- */
