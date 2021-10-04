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

#include "libgrbase.h"

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : gr_clear
 *
 *  Clear a bitmap (paint all pixels as 0 [transparent])
 *
 *  PARAMS :
 *      dest            Bitmap to clear
 *
 *  RETURN VALUE :
 *      None
 *
 */

void gr_clear( GRAPH * dest )
{
    memset( dest->data, 0, dest->pitch * dest->height ) ;

    dest->modified = 1 ; /* Don't needs analysis */
    dest->info_flags &= ~GI_NOCOLORKEY;
    dest->info_flags |= GI_CLEAN;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : gr_clear_as
 *
 *  Clear a bitmap (paint all pixels as the given color)
 *
 *  PARAMS :
 *      dest            Bitmap to clear
 *      color           8 or 16-bit color value
 *
 *  RETURN VALUE :
 *      None
 *
 */

void gr_clear_as( GRAPH * dest, int color )
{
    uint32_t y;

    if ( !color )
    {
        gr_clear( dest );
        return;
    }

    switch ( dest->format->depth )
    {
        case 8:
        {
            memset( dest->data, color, dest->pitch * dest->height ) ;
            break;
        }

        case 16:
        {
            uint8_t * data = dest->data ;
            int16_t * ptr ;
            int n ;
            y = dest->height;
            while ( y-- )
            {
                ptr = ( int16_t * ) data;
                n = dest->width;
                while ( n-- ) * ptr++ = color ;
                data += dest->pitch ;
            }
            break;
        }

        case 32:
        {
            uint8_t * data = dest->data ;
            uint32_t * ptr ;
            int n ;
            y = dest->height;
            while ( y-- )
            {
                ptr = ( uint32_t * ) data;
                n = dest->width;
                while ( n-- ) * ptr++ = color ;
                data += dest->pitch ;
            }
            break;
        }

        case 1:
        {
            int c = color ? 0xFF : 0 ;
            memset( dest->data, c, dest->pitch * dest->height ) ;
            break;
        }
    }

    dest->modified = 1 ; /* Don't needs analysis */
    if ( dest->format->depth != 32 || ( color & 0xff000000 ) == 0xff000000 )
        dest->info_flags |= GI_NOCOLORKEY;
    else
        dest->info_flags &= ~GI_NOCOLORKEY;
    dest->info_flags &= ~GI_CLEAN;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : gr_clear_region
 *
 *  Clear a region bitmap (paint all pixels as 0 [transparent])
 *
 *  PARAMS :
 *      dest            Bitmap to clear
 *      region          Region to clear or NULL for the whole screen
 *
 *  RETURN VALUE :
 *      None
 *
 */

void gr_clear_region( GRAPH * dest, REGION * region )
{
    REGION base_region ;
    int y, l ;

    if ( !dest ) dest = scrbitmap ;
    if ( !region )
    {
        region = &base_region ;
        region->x = 0 ;
        region->y = 0 ;
        region->x2 = dest->width - 1 ;
        region->y2 = dest->height - 1 ;
    }
    else
    {
        base_region = *region ;
        region = &base_region ;
        region->x = MAX( MIN( region->x, region->x2 ), 0 ) ;
        region->y = MAX( MIN( region->y, region->y2 ), 0 ) ;
        region->x2 = MIN( MAX( region->x, region->x2 ), dest->width - 1 ) ;
        region->y2 = MIN( MAX( region->y, region->y2 ), dest->height - 1 ) ;
    }

    switch ( dest->format->depth )
    {
        case 8:
        case 16:
        case 32:
        {
            uint8_t * data = (( uint8_t * ) dest->data ) + dest->pitch * region->y + region->x * dest->format->depthb;
            l = ( region->x2 - region->x + 1 ) * dest->format->depthb;
            for ( y = region->y; y <= region->y2; y++ )
            {
                memset( data, 0, l );
                data += dest->pitch ;
            }
            break ;
        }

        case 1:
        {
            uint8_t * data = (( uint8_t * ) dest->data ) + region->x / 8 ;
            l = ( region->x2 - region->x - 1 ) / 8 + 1 ;
            for ( y = region->y; y <= region->y2; y++ )
            {
                /* Esta debe ser cambiada, por bits */
                memset( data, 0, l ) ;
                data += dest->pitch ;
            }
            break ;
        }

        default:
            return;
    }

    dest->modified = 1 ; /* Don't needs analysis */
    dest->info_flags &= ~GI_NOCOLORKEY;
}

/* --------------------------------------------------------------------------- */
