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

#include "mod_map.h"

/* --------------------------------------------------------------------------- */

PALETTE * gr_read_pal( file * fp )
{
    PALETTE * pal;
    int i;
    uint8_t colors[256*3] ;

    if ( !file_read( fp, colors, sizeof( colors ) ) ) return NULL ;

    for ( i = 0; i < 768; i++ ) colors[i] <<= 2;

#ifdef COLORSPACE_BGR
    for ( i = 0; i < 768; i+=3 ){
    	uint8_t temp_color =colors[i];
    	colors[i]=colors[i+2];
    	colors[i+2]=temp_color;
    }
#endif

    pal = pal_new_rgb(( uint8_t * )colors );
    pal_refresh( pal );

    if ( !sys_pixel_format->palette )
    {
        sys_pixel_format->palette = pal_new( pal );
/*        pal_use( sys_pixel_format->palette ); */
        palette_changed = 1 ;
    }

    return pal ;
}

/* --------------------------------------------------------------------------- */

PALETTE * gr_read_pal_with_gamma( file * fp )
{
    PALETTE * pal = gr_read_pal( fp );

    /* Ignora definiciones de gama */
    if ( pal ) file_seek( fp, 576, SEEK_CUR ) ;

    return pal ;
}

/* --------------------------------------------------------------------------- */

/*
 *  FUNCTION : gr_save_pal
 *
 *  Saves palette to the given file
 *
 *  PARAMS :
 *      filename        Name of file to create
 *      pal             Palette
 *
 *  RETURN VALUE :
 *      1 if succeded, 0 if error
 */

int gr_save_pal( const char * filename, PALETTE * pal )
{
    if ( !filename ) return 0;
    
    uint8_t colors[256*3] ;

    file * fp ;
    char header[8] = PAL_MAGIC;
    int i;

    if ( !pal ) return 0;

    fp = file_open( filename, "wb" );
    if ( !fp ) return 0;

    memmove( colors, pal->rgb, sizeof( colors ) );
    for ( i = 0; i < sizeof( colors ); i++ ) colors[i] >>= 2;

    header[7] = 0x00; /* Version */
    file_write( fp, header, 8 );
    file_write( fp, colors, 768 );

    memset( colors, 0, 576 );
    file_write( fp, colors, 576 );
    file_close( fp );

    return 1;
}

/* --------------------------------------------------------------------------- */

/*
 *  FUNCTION : gr_save_system_pal
 *
 *  Saves the current palette to the given file
 *
 *  PARAMS :
 *      filename        Name of file to create
 *
 *  RETURN VALUE :
 *      1 if succeded, 0 if error
 */

int gr_save_system_pal( const char * filename )
{
    file * fp = file_open( filename, "wb" );
    char header[8] = PAL_MAGIC;
    int i;

    if ( !fp ) return 0;

    uint8_t colors[256*3] ;

    memmove( colors, sys_pixel_format->palette ? ( uint8_t * ) sys_pixel_format->palette->rgb : default_palette, sizeof( colors ) );
    for ( i = 0; i < sizeof( colors ); i++ ) colors[i] >>= 2;

    header[7] = 0x00; /* Version */
    file_write( fp, header, 8 );
    file_write( fp, colors, 768 );

    memset( colors, 0, 576 );
    file_write( fp, colors, 576 );
    file_close( fp );

    return 1;
}

/* --------------------------------------------------------------------------- */

int gr_load_pal( const char * filename )
{
    if ( !filename ) return 0;

    file * fp = file_open( filename, "rb" ) ;
    char header[8] ;
    PALETTE * r = NULL ;
    PALETTE * old_sys_pal = sys_pixel_format->palette ;

    if ( !fp ) return 0 ;

    file_read( fp, header, 8 ) ;
    if ( !strcmp( header, MAP_MAGIC ) )
    {
        file_seek( fp, 48, SEEK_SET ) ;
        r = gr_read_pal_with_gamma( fp ) ;
    }
    else if (
        !strcmp( header, FPG_MAGIC ) ||
        !strcmp( header, FNT_MAGIC ) ||
        !strcmp( header, PAL_MAGIC ) )
    {
        r = gr_read_pal_with_gamma( fp ) ;
    }
    else if ( memcmp( header, "\x89PNG", 4 ) == 0 )
    {
        GRAPH * graph ;
        file_close( fp );
        fp = NULL;
        graph = gr_read_png( filename );
        if ( graph )
        {
            r = pal_new( graph->format->palette );
            bitmap_destroy( graph );
        }
    }

    if ( r )
    {
//        pal_use( r );
        if ( old_sys_pal )
        {
            pal_destroy( old_sys_pal );

            sys_pixel_format->palette = pal_new( r );
/*            pal_use( sys_pixel_format->palette ); */
            palette_changed = 1 ;
        }
    }

    if ( fp ) file_close( fp ) ;
    return ( int ) r;
}

/* --------------------------------------------------------------------------- */
