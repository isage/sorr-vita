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

#include <string.h>
#include <stdlib.h>

#define __LIB_RENDER
#include "librender.h"

/* -------------------------------------------------------------------------- */

SDL_Color vpalette[ 256 ] ;

int fade_inc = 0 ;
int fade_on = 0 ;
int fade_set = 0 ;
int fade_step = 64 ;

SDL_Color fade_from ;
SDL_Color fade_to ;
SDL_Color fade_pos = { 100, 100, 100 };

/* -------------------------------------------------------------------------- */

static void activate_vpalette()
{
    int n ;

    if ( !screen ) return ;

    if (( sys_pixel_format ) && ( sys_pixel_format->palette ) )
    {
        if ( sys_pixel_format->depth > 8 )
        {
            for ( n = 0 ; n < 256 ; n++ )
                sys_pixel_format->palette->colorequiv[ n ] = SDL_MapRGB( screen->format, sys_pixel_format->palette->rgb[ n ].r, sys_pixel_format->palette->rgb[ n ].g, sys_pixel_format->palette->rgb[ n ].b ) ;
        }
        else
        {
            for ( n = 0 ; n < 256 ; n++ )
            {
                if ( fade_pos.r <= 100 )
                    vpalette[ n ].r = sys_pixel_format->palette->rgb[ n ].r * fade_pos.r / 100;
                else
                    vpalette[ n ].r = sys_pixel_format->palette->rgb[ n ].r + ( 255 - sys_pixel_format->palette->rgb[ n ].r ) * ( fade_pos.r - 100 ) / 100;

                if ( fade_pos.g <= 100 )
                    vpalette[ n ].g = sys_pixel_format->palette->rgb[ n ].g * fade_pos.g / 100;
                else
                    vpalette[ n ].g = sys_pixel_format->palette->rgb[ n ].g + ( 255 - sys_pixel_format->palette->rgb[ n ].g ) * ( fade_pos.g - 100 ) / 100;

                if ( fade_pos.b <= 100 )
                    vpalette[ n ].b = sys_pixel_format->palette->rgb[ n ].b * fade_pos.b / 100;
                else
                    vpalette[ n ].b = sys_pixel_format->palette->rgb[ n ].b + ( 255 - sys_pixel_format->palette->rgb[ n ].b ) * ( fade_pos.b - 100 ) / 100;
            }

            SDL_SetPaletteColors(screen->format->palette, vpalette, 0, 256);
        }
    }
}

/* -------------------------------------------------------------------------- */

void gr_fade_init( int r, int g, int b, int speed )
{
    if ( fade_pos.r == r && fade_pos.g == g && fade_pos.b == b )
    {
        GLODWORD( librender, FADING ) = 0 ;
        fade_on = 0 ;
        return ;
    }

    fade_inc = speed;
    fade_step = 0;
    fade_on = 1 ;
    fade_from = fade_pos;
    fade_to.r = ( r > 200 ) ? 200 : r ;
    fade_to.g = ( g > 200 ) ? 200 : g ;
    fade_to.b = ( b > 200 ) ? 200 : b ;

    GLODWORD( librender, FADING ) = 1 ;
}

/* -------------------------------------------------------------------------- */

void gr_fade_step()
{
    if ( fade_on )
    {
        fade_set = 1 ;
        GLODWORD( librender, FADING ) = 1 ;

        fade_step += fade_inc ;
        if ( fade_step < 0 )
        {
            GLODWORD( librender, FADING ) = 0 ;
            fade_step = 0 ;
            fade_on = 0 ;
        }

        if ( fade_step >= 64 )
        {
            GLODWORD( librender, FADING ) = 0 ;
            fade_step = 64 ;
            fade_on = 0 ;
        }

        fade_pos.r = ( fade_to.r * fade_step + fade_from.r * ( 64 - fade_step ) ) / 64;
        fade_pos.g = ( fade_to.g * fade_step + fade_from.g * ( 64 - fade_step ) ) / 64;
        fade_pos.b = ( fade_to.b * fade_step + fade_from.b * ( 64 - fade_step ) ) / 64;

        if (
            ( fade_step + fade_inc < 0 || fade_step + fade_inc > 64 ) &&
            ( fade_pos.r == 100 && fade_pos.g == 100 && fade_pos.b == 100 ) )
        {
            GLODWORD( librender, FADING ) = 0 ;
            fade_step = 100 ;
            fade_on = 0;
        }
    }

    if ( fade_set )
    {
        if ( !fade_on && fade_to.r == 100 && fade_to.g == 100 && fade_to.b == 100 ) fade_set = 0;

        activate_vpalette() ;

        if ( scrbitmap->format->depth > 8 )
        {
            gr_fade16( scrbitmap, fade_pos.r, fade_pos.g, fade_pos.b );
        }
    }
}

/* -------------------------------------------------------------------------- */

