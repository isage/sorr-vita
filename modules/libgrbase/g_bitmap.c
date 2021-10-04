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
#include "bitwise_map.h"

/* --------------------------------------------------------------------------- */

uint32_t * map_code_bmp = NULL ;
int map_code_allocated = 0 ;
int map_code_last = 0;

/* --------------------------------------------------------------------------- */

PIXEL_FORMAT * bitmap_create_format( int bpp )
{
    PIXEL_FORMAT *format;

    /* Allocate an empty pixel format structure */
    format = malloc( sizeof( *format ) );
    if ( !format ) return( NULL );

    /* Set up the format */

    format->palette = NULL;
    format->depth = bpp;
    format->depthb = ( bpp + 7 ) / 8;

    if ( bpp == 32 )
    {
        format->Aloss = 0;
        format->Rloss = 0;
        format->Gloss = 0;
        format->Bloss = 0;

#ifdef COLORSPACE_BGR
        format->Ashift = 0x18;
        format->Rshift = 0x00;
        format->Gshift = 0x08;
        format->Bshift = 0x10;

        format->Amask = 0xFF000000;
        format->Rmask = 0x000000FF;
        format->Gmask = 0x0000FF00;
        format->Bmask = 0x00FF0000;
#else
        format->Ashift = 24;
        format->Rshift = 16;
        format->Gshift = 8;
        format->Bshift = 0;

        format->Amask = 0xFF000000;
        format->Rmask = 0x00FF0000;
        format->Gmask = 0x0000FF00;
        format->Bmask = 0x000000FF;
#endif
    }
    else if ( bpp > 8 )
    {
        /* R-G-B */
        if ( bpp > 24 ) bpp = 24;

        format->Rloss = 8 - ( bpp / 3 );
        format->Gloss = 8 - ( bpp / 3 ) - ( bpp % 3 );
        format->Bloss = 8 - ( bpp / 3 );

#ifdef COLORSPACE_BGR
        format->Rshift = 0;
        format->Gshift = ( bpp / 3 );
        format->Bshift = (( bpp / 3 ) + ( bpp % 3 ) ) + ( bpp / 3 );
#else
        format->Rshift = (( bpp / 3 ) + ( bpp % 3 ) ) + ( bpp / 3 );
        format->Gshift = ( bpp / 3 );
        format->Bshift = 0;
#endif

        format->Rmask = (( 0xFF >> format->Rloss ) << format->Rshift );
        format->Gmask = (( 0xFF >> format->Gloss ) << format->Gshift );
        format->Bmask = (( 0xFF >> format->Bloss ) << format->Bshift );
    }
    else
    {
        format->Rloss = 8;
        format->Gloss = 8;
        format->Bloss = 8;
        format->Aloss = 8;

        format->Rshift = 0;
        format->Gshift = 0;
        format->Bshift = 0;
        format->Ashift = 0;

        format->Rmask = 0;
        format->Gmask = 0;
        format->Bmask = 0;
        format->Amask = 0;
    }

    return( format );
}

/* --------------------------------------------------------------------------- */

GRAPH * bitmap_new_ex( int code, int w, int h, int depth, void * data, int pitch )
{
    GRAPH * gr ;
    int wb ;

    if ( w < 1 || h < 1 ) return NULL;

    /* Create and fill the struct */

    gr = ( GRAPH * ) malloc( sizeof( GRAPH ) ) ;
    if ( !gr ) return NULL; // sin memoria

    /* Calculate the row size (dword-aligned) */

    wb = w * depth / 8;
    if (( wb * 8 / depth ) < w ) wb++;

    gr->data = data ;

    gr->width = w ;
    gr->height = h ;

    gr->format = bitmap_create_format( depth ) ;

    gr->pitch = pitch ;
    gr->widthb = wb ;

    gr->code = code ;
    gr->name[ 0 ] = '\0';

    gr->ncpoints = 0;
    gr->cpoints = NULL ;

    gr->format->palette = NULL ;

    gr->blend_table = NULL ;

    gr->modified = 0;
    gr->info_flags = GI_EXTERNAL_DATA ;

    return gr ;
}

/* --------------------------------------------------------------------------- */

GRAPH * bitmap_new( int code, int w, int h, int depth )
{
    GRAPH * gr ;
    int bytesPerRow, wb ;

    if ( w < 1 || h < 1 ) return NULL;

    /* Create and fill the struct */

    gr = ( GRAPH * ) malloc( sizeof( GRAPH ) ) ;
    if ( !gr ) return NULL; // sin memoria

    /* Calculate the row size (dword-aligned) */

    wb = w * depth / 8;
    if (( wb * 8 / depth ) < w ) wb++;

    bytesPerRow = wb;
    if ( bytesPerRow & 0x03 ) bytesPerRow = ( bytesPerRow & ~3 ) + 4;

    gr->data = ( char * ) malloc( h * bytesPerRow ) ;
    if ( !gr->data )   // Sin memoria
    {
        free( gr );
        return NULL;
    }

    gr->width = w ;
    gr->height = h ;

    gr->format = bitmap_create_format( depth ) ;

    gr->pitch = bytesPerRow ;
    gr->widthb = wb ;

    gr->code = code ;
    gr->name[ 0 ] = '\0';

    gr->ncpoints = 0;
    gr->cpoints = NULL ;

    gr->format->palette = NULL ;

    gr->blend_table = NULL ;

    gr->modified = 0;
    gr->info_flags = 0;

    return gr ;
}

/* --------------------------------------------------------------------------- */

GRAPH * bitmap_clone( GRAPH * map )
{
    GRAPH * gr ;
    uint32_t y;

    gr = bitmap_new( 0, map->width, map->height, map->format->depth ) ;
    if ( gr == NULL ) return NULL;

    for ( y = 0 ; y < map->height ; y++ )
        memcpy(( uint8_t* ) gr->data + gr->pitch * y, ( uint8_t* ) map->data + gr->pitch * y, gr->widthb );

    if ( map->cpoints )
    {
        gr->cpoints = malloc( sizeof( CPOINT ) * map->ncpoints ) ;
        memcpy( gr->cpoints, map->cpoints, sizeof( CPOINT ) * map->ncpoints ) ;
        gr->ncpoints = map->ncpoints ;
    }

    gr->blend_table = map->blend_table;
    gr->info_flags = map->info_flags & ~GI_EXTERNAL_DATA ;
    gr->modified = map->modified ;
    gr->format->palette = map->format->palette ;
    pal_use( map->format->palette );

    memcpy( gr->name, map->name, sizeof( map->name ) ) ;

    return gr ;
}

/* --------------------------------------------------------------------------- */

void bitmap_add_cpoint( GRAPH * map, int x, int y )
{
    map->cpoints = ( CPOINT * ) realloc( map->cpoints, ( map->ncpoints + 1 ) * sizeof( CPOINT ) ) ;
    map->cpoints[ map->ncpoints ].x = x ;
    map->cpoints[ map->ncpoints ].y = y ;
    map->ncpoints++;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : bitmap_set_cpoint
 *
 *  Set a control point in a graphic
 *
 *  PARAMS :
 *      map             Pointer to the bitmap
 *      point           Control point index
 *      x               New X coordinate or CPOINT_UNDEFINED to unset
 *      y               New Y coordinate or CPOINT_UNDEFINED to unset
 *
 *  RETURN VALUE :
 *      None
 *
 */

void bitmap_set_cpoint( GRAPH * map, uint32_t point, int x, int y )
{
    uint32_t n;

    if ( point == 0 ) map->modified = 1;

    if ( map->ncpoints <= point )
    {
        map->cpoints = ( CPOINT * ) realloc( map->cpoints, ( point + 1 ) * sizeof( CPOINT ) ) ;
        for ( n = map->ncpoints; n < point; n++ )
        {
            map->cpoints[ n ].x = CPOINT_UNDEFINED;
            map->cpoints[ n ].y = CPOINT_UNDEFINED;
        }
        map->ncpoints = point + 1 ;
    }
    map->cpoints[ point ].x = x;
    map->cpoints[ point ].y = y;
}

/* --------------------------------------------------------------------------- */

void bitmap_destroy( GRAPH * map )
{
    if ( !map ) return ;

    if ( map->cpoints ) free( map->cpoints ) ;

    if ( map->code > 999 ) bit_clr( map_code_bmp, map->code - 1000 );

    if ( map->data && !( map->info_flags & GI_EXTERNAL_DATA ) ) free( map->data ) ;

    if ( map->format )
    {
        if ( map->format->palette ) pal_destroy( map->format->palette ) ;
        free ( map->format );
    }

    free( map ) ;
}

/* --------------------------------------------------------------------------- */
/* Análisis */

void bitmap_analize( GRAPH * bitmap )
{
    uint32_t x, y;

    if ( bitmap->modified > 1 ) bitmap->modified = 1 ;

    bitmap->info_flags &= ~GI_ANALIZE_MASK ;

    /* Search for transparent pixels (value 0).
     * If none found, set the flag GI_NOCOLORKEY */

    switch ( bitmap->format->depth )
    {
        case    8:
        {
            uint8_t * ptr = ( uint8_t * ) bitmap->data ;

            for ( y = bitmap->height; y--; ptr += bitmap->pitch )
            {
                if ( memchr( ptr, 0, bitmap->width ) ) break ;
            }
        }
        break;
        case    16:
        {
            int16_t * ptr = ( int16_t * ) bitmap->data ;
            int inc = bitmap->pitch - bitmap->widthb ;

            for ( y = bitmap->height; y--; ptr = ( int16_t * )((( uint8_t * ) ptr ) + inc ) )
            {
                for ( x = bitmap->width; x--; ) if ( !*ptr++ ) break;
                if ( x >= 0 ) break;
            }
        }
        break;
        case    32:
        {
            int32_t * ptr = ( int32_t * ) bitmap->data ;
            int inc = bitmap->pitch - bitmap->widthb ;

            for ( y = bitmap->height; y--; ptr = ( int32_t * )((( uint8_t * ) ptr ) + inc ) )
            {
                for ( x = bitmap->width; x--; ) if ( !*ptr++ ) break;
                if ( x >= 0 ) break;
            }
        }
    }

    if ( y < 0 ) bitmap->info_flags |= GI_NOCOLORKEY ;
}

/* --------------------------------------------------------------------------- */
/* Returns the code of a new system library graph (1000+). Searchs
   for free slots if the program creates too many system maps */

int bitmap_next_code()
{
    int n, nb, lim, ini ;

    // Si tengo suficientes alocados, retorno el siguiente segun map_code_last
    if ( map_code_last < map_code_allocated )
    {
        if ( !bit_tst( map_code_bmp, map_code_last ) )
        {
            bit_set( map_code_bmp, map_code_last );
            return 1000 + map_code_last++ ;
        }
    }

    // Ya no tengo mas espacio, entonces busco alguno libre entre ~+32 desde el ultimo fijo y ~-32 del ultimo asignado

    ini = ( map_code_last < map_code_allocated ) ? ( map_code_last >> 5 ) : 0 ;
    lim = ( map_code_allocated >> 5 ) ;

    while ( 1 )
    {
        for ( n = ini; n < lim ; n++ )
        {
            if ( map_code_bmp[n] != ( uint32_t ) 0xFFFFFFFF ) // Aca hay 1 libre, busco cual es
            {
                for ( nb = 0; nb < 32; nb++ )
                {
                    if ( !bit_tst( map_code_bmp + n, nb ) )
                    {
                        map_code_last = ( n << 5 ) + nb ;
                        bit_set( map_code_bmp, map_code_last );
                        return 1000 + map_code_last++ ;
                    }
                }
            }
        }
        if ( !ini ) break;
        lim = ini;
        ini = 0;
    }

    map_code_last = map_code_allocated ;

    // Increment space, no more slots availables
    // 256 new maps available for alloc

    map_code_allocated += 256 ;
    map_code_bmp = ( uint32_t * ) realloc( map_code_bmp, sizeof( uint32_t ) * ( map_code_allocated >> 5 ) );

    memset( &map_code_bmp[( map_code_last >> 5 )], 0, 32 );  /* 256 >> 5 = 8 * sizeof ( uint32_t ) = 8 * 4 = 32 */

    // Devuelvo map_code_last e incremento en 1, ya que ahora tengo BLOCK_INCR mas que antes
    bit_set( map_code_bmp, map_code_last );
    return 1000 + map_code_last++ ;

}

/* --------------------------------------------------------------------------- */

GRAPH * bitmap_new_syslib( int w, int h, int depth )
{
    GRAPH * gr ;

    if ( !syslib ) return NULL;

    gr = bitmap_new( 0, w, h, depth ) ;
    if ( !gr ) return NULL;

    gr->code = bitmap_next_code() ;
    grlib_add_map( 0, gr ) ;

    return gr ;
}

/* --------------------------------------------------------------------------- */
