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

struct
{
    int     code ;
    int     regsize ;
    char    name[32] ;
    char    fpname[12] ;
    int     width ;
    int     height ;
    int     flags ;
} chunk ;

/* --------------------------------------------------------------------------- */

/* Static convenience function */
static int gr_read_lib( file * fp )
{
    char header[8] ;
    short int px, py ;
    int bpp, libid, code;
    uint32_t   y ;
    unsigned c;
    GRLIB * lib ;
    GRAPH * gr ;
    PALETTE * pal = NULL ;
    int st = 0;
    int len = 0, tempcolor = 0;

    libid = grlib_new() ;
    if ( libid < 0 ) return -1 ;

    lib = grlib_get( libid );
    if ( !lib )
    {
        grlib_destroy( libid ) ;
        return -1 ;
    }

    file_read( fp, header, 8 ) ;

    if ( strcmp( header, F32_MAGIC ) == 0 ) bpp = 32 ;
    else if ( strcmp( header, F16_MAGIC ) == 0 ) bpp = 16 ;
    else if ( strcmp( header, FPG_MAGIC ) == 0 ) bpp = 8 ;
    else if ( strcmp( header, F01_MAGIC ) == 0 ) bpp = 1 ;
    else
    {
        grlib_destroy( libid ) ;
        return -1 ;
    }

    if ( bpp == 8 && !( pal = gr_read_pal_with_gamma( fp ) ) )
    {
        grlib_destroy( libid ) ;
        return -1 ;
    }

    while ( !file_eof( fp ) )
    {
        if ( !file_read( fp, &chunk, 64 ) ) break ;

        ARRANGE_DWORD( &chunk.code ) ;
        ARRANGE_DWORD( &chunk.regsize ) ;
        ARRANGE_DWORD( &chunk.width ) ;
        ARRANGE_DWORD( &chunk.height ) ;
        ARRANGE_DWORD( &chunk.flags ) ;

        /* Cabecera del gráfico */

        gr = bitmap_new( chunk.code, chunk.width, chunk.height, bpp ) ;
        if ( !gr )
        {
            grlib_destroy( libid ) ;
            if ( bpp == 8 ) pal_destroy( pal ) ; // Elimino la instancia inicial
            return -1 ;
        }
        memcpy( gr->name, chunk.name, 32 ) ;
        gr->name[31] = 0 ;
        gr->ncpoints = chunk.flags ;
        gr->modified = 2 ;
        // bitmap_analize( gr );

        /* Puntos de control */

        if ( gr->ncpoints )
        {
            gr->cpoints = ( CPOINT * ) malloc( gr->ncpoints * sizeof( CPOINT ) ) ;
            if ( !gr->cpoints )
            {
                bitmap_destroy( gr ) ;
                grlib_destroy( libid ) ;
                if ( bpp == 8 ) pal_destroy( pal ) ;
                return -1 ;
            }
            for ( c = 0 ; c < gr->ncpoints ; c++ )
            {
                file_readSint16( fp, &px ) ;
                file_readSint16( fp, &py ) ;
                if ( px == -1 && py == -1 )
                {
                    gr->cpoints[c].x = CPOINT_UNDEFINED ;
                    gr->cpoints[c].y = CPOINT_UNDEFINED ;
                }
                else
                {
                    gr->cpoints[c].x = px ;
                    gr->cpoints[c].y = py ;
                }
            }
        }
        else gr->cpoints = 0 ;

        /* Datos del gráfico */

        for ( y = 0 ; y < gr->height ; y++ )
        {
            uint8_t * line = ( uint8_t * )gr->data + gr->pitch * y;

            switch ( bpp )
            {
                case    32:
                    st = file_readUint32A( fp, ( uint32_t * ) line, gr->width );
#ifdef COLORSPACE_BGR
                    if (st){
						for (len=0; len<gr->width; len+=4){
							tempcolor=line[len+1];
							line[len+1]=line[len+3];
							line[len+3]=tempcolor;
						}
                    }
#endif
                    break;

                case    16:
                    st = file_readUint16A( fp, ( uint16_t * ) line, gr->width );
#ifdef COLORSPACE_BGR
                    if (st){
                    	uint16_t * line16 = ( uint16_t * ) line;
                    	uint8_t rgb[3];
						for (len =0;len<gr->width;len++){
							rgb[0] = (*line16 & 0x1F);
							rgb[1] =(*line16 >>5);
							rgb[2] = (*line16 >>11);
							*line16=(rgb[0]<<11)|(rgb[1]<<5)|rgb[2];
							line16 ++;
						}
                    }
#endif
                    break;

                case    8:
                case    1:
                    st = file_read( fp, line, gr->widthb );
                    break;
            }

            if ( !st )
            {
                bitmap_destroy( gr );
                grlib_destroy( libid ) ;
                if ( bpp == 8 ) pal_destroy( pal );
                return -1 ;
            }
        }

        code = grlib_add_map( libid, gr ) ;
        if ( bpp == 8 ) pal_map_assign( libid, code, pal ) ;
    }

    if ( bpp == 8 ) pal_destroy( pal ) ; // Elimino la instancia inicial

    return libid ;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : gr_load_fpg
 *
 *  Load a FPG file
 *
 *  PARAMS :
 *  libname   Name of the file
 *
 *  RETURN VALUE :
 *      Internal ID of the library or -1 if invalid file
 *
 */

int gr_load_fpg( const char * libname )
{
    int libid ;
    file * fp = file_open( libname, "rb" ) ;
    if ( !fp ) return -1 ;
    libid = gr_read_lib( fp ) ;
    file_close( fp ) ;
    return libid ;
}

/* --------------------------------------------------------------------------- */

/*
 *  FUNCTION : gr_save_fpg
 *
 *  Save a FPG file
 *
 *  PARAMS :
 *  filename  Name of the file
 *
 *  RETURN VALUE :
 *      1 if succeded
 *      >= 0 error
 *
 */

int gr_save_fpg( int libid, const char * filename )
{
    file  * fp;
    GRLIB * lib;
    uint8_t * block = NULL;
    int     n, y, c;
    int     bpp;
    int     nmaps;
    uint8_t header[8];
    rgb_component * palette = NULL;

    /* Get the library and open the file */
/*
    if ( !libid )
        lib = syslib ;
    else
*/
        lib = grlib_get( libid );

    if ( !lib ) return 0;

    /* Guess the FPG depth */

    nmaps = lib->map_reserved < 1000 ? lib->map_reserved : 1000;

    for ( bpp = n = 0 ; n < nmaps ; n++ )
    {
        if ( lib->maps[n] )
        {
            if ( bpp && lib->maps[n]->format->depth != bpp ) return 0; /* save_fpg: don't allow save maps with differents bpp */
            bpp = lib->maps[n]->format->depth;
            if ( !palette && bpp == 8 && lib->maps[n]->format->palette ) palette = lib->maps[n]->format->palette->rgb;
        }
    }

    if ( bpp == 32 ) strcpy( ( char * ) header, F32_MAGIC );
    else if ( bpp == 16 ) strcpy( ( char * ) header, F16_MAGIC );
    else if ( bpp == 8 ) strcpy( ( char * ) header, FPG_MAGIC );
    else if ( bpp == 1 ) strcpy( ( char * ) header, F01_MAGIC );
    else return 0; /* No maps for save */

    /* Create fpg */

    fp = file_open( filename, "wb" );
    if ( !fp ) return 0;

    /* Write the header */

    header[7] = 0x00; /* Version */
    file_write( fp, header, 8 );

    /* Write the color palette */

    if ( bpp == 8 )
    {
        uint8_t colors[256][3] ;
        uint8_t gamma[576];

        if ( !palette )
        {
            if ( sys_pixel_format->palette )
                palette = sys_pixel_format->palette->rgb;
            else
                palette = ( rgb_component * ) default_palette;
        }

        for ( n = 0 ; n < 256 ; n++ )
        {
            colors[n][0] = palette[n].r >> 2 ;
            colors[n][1] = palette[n].g >> 2 ;
            colors[n][2] = palette[n].b >> 2 ;
        }

        file_write( fp, &colors, 768 ) ;
        memset( gamma, '\0', sizeof( gamma ) );
        file_write( fp, gamma, sizeof( gamma ) ) ;
    }

    /* Write each map */

    for ( n = 0 ; n < nmaps ; n++ )
    {
        GRAPH * gr = lib->maps[n];

        if ( gr )
        {
            /* Write the bitmap header */

            chunk.code = n;
            chunk.regsize = 0;
            chunk.width = gr->width;
            chunk.height = gr->height;
            chunk.flags = gr->ncpoints;
            chunk.fpname[0] = 0;
            strncpy( chunk.name,  gr->name,  sizeof( chunk.name ) );

            ARRANGE_DWORD( &chunk.code );
            ARRANGE_DWORD( &chunk.regsize );
            ARRANGE_DWORD( &chunk.width );
            ARRANGE_DWORD( &chunk.height );
            ARRANGE_DWORD( &chunk.flags );

            file_write( fp, &chunk, 64 );

            /* Write the control points */

            for ( c = 0 ; c < gr->ncpoints ; c++ )
            {
                file_writeSint16( fp, &gr->cpoints[c].x );
                file_writeSint16( fp, &gr->cpoints[c].y );
            }

            /* Write the bitmap pixel data */

            if ( bpp > 8 )
            {
                if ( !( block = malloc( gr->widthb ) ) )
                {
                    file_close( fp );
                    return 0; /* No memory */
                }
            }

            for ( y = 0 ; y < gr->height ; y++ )
            {
                if ( bpp > 8 )
                {
                    memcpy( block, ( uint8_t * )gr->data + gr->pitch*y, gr->widthb );
                    if ( bpp == 16 )
                    {
/*                        gr_convert16_ScreenTo565(( uint16_t * )block, gr->width ); */
                        file_writeUint16A( fp, ( uint16_t * ) block, gr->width );
                    }
                    else
                        file_writeUint32A( fp, ( uint32_t * ) block, gr->width );
                }
                else
                {
                    file_write( fp, ( uint8_t * )gr->data + gr->pitch*y, gr->widthb );
                }
            }

            if ( bpp > 8 ) free( block );
        }
    }

    file_close( fp );
    return 1;
}

/* --------------------------------------------------------------------------- */
