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

#include "libfont.h"
#include "mod_map.h"

/* --------------------------------------------------------------------------- */

typedef struct _chardata
{
    int width;
    int height;
    int xadvance;
    int yadvance;
    int xoffset;
    int yoffset;
    int fileoffset ;
}
_chardata ;

static int gr_font_loadfrom( file * fp );

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : gr_font_load
 *
 *  Load a font from a given file, in FNT (DIV) format
 *
 *  PARAMS :
 *  filename  Name of the file
 *
 *  RETURN VALUE :
 *      ID of the new font, or -1 if error
 *
 */

int gr_font_load( char * filename )
{
    file * fp ;
    int result;

    if ( !filename ) return -1;

    fp = file_open( filename, "rb" ) ;
    if ( !fp ) return -1 ;

    result = gr_font_loadfrom( fp );

    file_close( fp );

    return result;
}

/* --------------------------------------------------------------------------- */

static int gr_font_loadfrom( file * fp )
{
    char header[8];
    int bpp;
    int types, i, id;
    uint32_t y;
    FONT * f;
    PALETTE * pal = NULL;

    struct
    {
        int width ;
        int height ;
        int yoffset ;
        int fileoffset ;
    }
    oldchardata[256];

    _chardata chardata[256] ;

    if ( font_count == MAX_FONTS ) return -1 ;

    /* Read the file header */

    if ( !file_read( fp, header, 8 ) ) return -1;

    if (
        memcmp( header, FNT_MAGIC, 7 ) != 0 &&
        memcmp( header, FNX_MAGIC, 7 ) != 0 )
    {
        return -1;
    }

    bpp = header[7];
    if ( bpp == 0 ) bpp = 8;

    /* Read or ignore the palette */

    if ( bpp == 8 && !( pal = gr_read_pal_with_gamma( fp ) ) ) return -1 ;

    /* Read the character data (detect old format) */

    if ( header[2] == 'x' )
    {
        if ( !file_readSint32( fp, &types ) )
        {
            pal_destroy( pal );
            return -1 ;
        }
        if ( !file_read( fp, chardata, sizeof( chardata ) ) )
        {
            pal_destroy( pal );
            return -1 ;
        }
        for ( i = 0 ; i < 256 ; i++ )
        {
            ARRANGE_DWORD( &chardata[i].width );
            ARRANGE_DWORD( &chardata[i].height );
            ARRANGE_DWORD( &chardata[i].xadvance );
            ARRANGE_DWORD( &chardata[i].yadvance );
            ARRANGE_DWORD( &chardata[i].xoffset );
            ARRANGE_DWORD( &chardata[i].yoffset );
            ARRANGE_DWORD( &chardata[i].fileoffset );
        }
    }
    else
    {
        if ( !file_readSint32( fp, &types ) )
        {
            pal_destroy( pal );
            return -1 ;
        }
        if ( !file_read( fp, oldchardata, sizeof( oldchardata ) ) )
        {
            pal_destroy( pal );
            return -1 ;
        }
        for ( i = 0 ; i < 256 ; i++ )
        {
            ARRANGE_DWORD( &oldchardata[i].width );
            ARRANGE_DWORD( &oldchardata[i].height );
            ARRANGE_DWORD( &oldchardata[i].yoffset );
            ARRANGE_DWORD( &oldchardata[i].fileoffset );

            chardata[i].width      = oldchardata[i].width;
            chardata[i].height     = oldchardata[i].height;
            chardata[i].xoffset    = 0;
            chardata[i].yoffset    = oldchardata[i].yoffset;
            chardata[i].xadvance   = oldchardata[i].width;
            chardata[i].yadvance   = oldchardata[i].height + oldchardata[i].yoffset;
            chardata[i].fileoffset = oldchardata[i].fileoffset;
        }
    }

    /* Create the font */

    if ( header[2] == 'x' )
        id = gr_font_new( types, header[7] ) ;
    else
        id = gr_font_new( CHARSET_CP850, 8 ) ;

    if ( id == -1 )
    {
        pal_destroy( pal );
        return -1 ;
    }

    f = fonts[id];
    if ( !f )
    {
        gr_font_destroy( id );
        pal_destroy( pal );
        return -1 ;
    }

    /* Load the character bitmaps */

    for ( i = 0 ; i < 256 ; i++ )
    {
        GRAPH * gr;
        uint8_t * ptr;

        f->glyph[i].xadvance = chardata[i].xadvance ;
        f->glyph[i].yadvance = chardata[i].yadvance ;

        if ( chardata[i].fileoffset == 0 || chardata[i].width == 0 || chardata[i].height == 0 ) continue ;

        f->glyph[i].xoffset = chardata[i].xoffset ;
        f->glyph[i].yoffset = chardata[i].yoffset ;

        file_seek( fp, chardata[i].fileoffset, SEEK_SET ) ;
        f->glyph[i].bitmap = gr = bitmap_new( i, chardata[i].width, chardata[i].height, f->bpp ) ;
        if ( !gr )
        {
            gr_font_destroy( id );
            pal_destroy( pal );
            return -1 ;
        }
        bitmap_add_cpoint( gr, 0, 0 ) ;
        gr->format->palette = pal;
        pal_use( pal );

        for ( y = 0, ptr = gr->data; y < gr->height; y++, ptr += gr->pitch )
        {
            if ( !file_read( fp, ptr, gr->widthb ) ) break ;

            if ( gr->format->depth == 16 )
            {
                ARRANGE_WORDS( ptr, ( int )gr->width );
            }
            else if ( gr->format->depth == 32 )
            {
                ARRANGE_DWORDS( ptr, ( int )gr->width );
            }
        }

        f->glyph[i].yoffset = chardata[i].yoffset ;
    }
    if ( f->glyph[32].xadvance == 0 ) f->glyph[32].xadvance = f->glyph['j'].xadvance ;

    pal_destroy( pal ); // Elimino la instancia inicial

    return id ;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : gr_font_save
 *
 *  Write a font to disk, in FNT/FNX format
 *
 *  PARAMS :
 *  fontid   ID of the font to save
 *  filename  Name of the file to create
 *
 *  RETURN VALUE :
 *      1 if succeded or 0 otherwise
 *
 */

int gr_font_save( int fontid, const char * filename )
{
    if ( !filename ) return 0;

    file * file;
    int n;
    uint32_t y;
    long offset;
    uint8_t * block = NULL ;
    uint8_t * lineptr;

    FONT * font;
    uint8_t header[8];
    _chardata chardata[256] ;
    int palette_saved = 0;

    if ( fontid < 0 || fontid > MAX_FONTS || !fonts[fontid] ) return 0;

    font = fonts[fontid];

    /* Open the file */

    file = file_open( filename, "wb0" );
    if ( !file ) return 0;

    /* Write the header */

    strcpy( ( char * ) header, FNX_MAGIC );
    header[7] = font->bpp;
    file_write( file, &header, 8 );

    /* Write the character information */

    memset( chardata, 0, sizeof( chardata ) );
    offset = 8 + 4 + (( font->bpp == 8 ) ? 576 + 768 : 0 ) + sizeof( chardata );

    for ( n = 0 ; n < 256 ; n++ )
    {
        chardata[n].xadvance   = font->glyph[n].xadvance;
        chardata[n].yadvance   = font->glyph[n].yadvance;

        if ( font->glyph[n].bitmap )
        {
            /* Write the palette */
            if ( !palette_saved && font->bpp == 8 )
            {
                uint8_t   colors[256][3];
                uint8_t * block = calloc( 576, 1 ) ;
                rgb_component * gpal = NULL;
                int k;

                if ( font->glyph[n].bitmap->format->palette )
                    gpal = font->glyph[n].bitmap->format->palette->rgb;
                else if ( sys_pixel_format->palette )
                    gpal = sys_pixel_format->palette->rgb;
                else
                    gpal = ( rgb_component * ) default_palette;

                /* Generate palette info */
                for ( k = 0 ; k < 256 ; k++ )
                {
                    colors[k][0] = gpal[k].r >> 2 ;
                    colors[k][1] = gpal[k].g >> 2 ;
                    colors[k][2] = gpal[k].b >> 2 ;
                }

                file_write( file, &colors, sizeof(colors) ) ;
                file_write( file, block, 576 ) ;
                free( block ) ;
                palette_saved = 1;
            }

            chardata[n].width      = font->glyph[n].bitmap->width;
            chardata[n].height     = font->glyph[n].bitmap->height;
            chardata[n].xadvance   = font->glyph[n].xadvance;
            chardata[n].yadvance   = font->glyph[n].yadvance;
            chardata[n].xoffset    = font->glyph[n].xoffset;
            chardata[n].yoffset    = font->glyph[n].yoffset;
            chardata[n].fileoffset = offset;

            offset += font->glyph[n].bitmap->widthb * chardata[n].height;
        }

        ARRANGE_DWORD( &chardata[n].xadvance );
        ARRANGE_DWORD( &chardata[n].yadvance );
        ARRANGE_DWORD( &chardata[n].width );
        ARRANGE_DWORD( &chardata[n].width );
        ARRANGE_DWORD( &chardata[n].xoffset );
        ARRANGE_DWORD( &chardata[n].yoffset );
        ARRANGE_DWORD( &chardata[n].fileoffset );
    }

    file_writeSint32( file, &font->charset );

    file_write( file, &chardata, sizeof( chardata ) );

    /* Write the character bitmaps */

    for ( n = 0 ; n < 256 ; n++ )
    {
        if ( font->glyph[n].bitmap )
        {
            GRAPH * gr = font->glyph[n].bitmap;

            if ( gr->format->depth != font->bpp )
            {
                file_close( file );
                return 0;
            }

            if ( gr->format->depth > 8 )
            {
                if (( block = malloc( gr->widthb ) ) == NULL )
                {
                    file_close( file );
                    return 0;
                }
            }

            lineptr = gr->data;

            for ( y = 0 ; y < gr->height ; y++, lineptr += gr->pitch )
            {
                if ( gr->format->depth > 8 )
                {
                    memcpy( block, lineptr, gr->widthb );
                    if ( gr->format->depth == 16 )
                    {
                        ARRANGE_WORDS( block, ( int )gr->width );
/*                        gr_convert16_ScreenTo565(( uint16_t * )block, gr->width ); */
                        file_write( file, block, gr->widthb );
                    }
                    else if ( gr->format->depth == 32 )
                    {
                        file_writeUint32A( file, ( uint32_t * ) block, gr->width );
                    }
                }
                else
                {
                    file_write( file, lineptr, gr->widthb );
                }
            }

            if ( gr->format->depth > 8 ) free( block );
        }
    }

    file_close( file );

    return 1;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : gr_load_bdf
 *
 *  Load a BDF font from disk. This is a very simple loader that ignores
 *  anything that is not relevant to screen display or non-horizontal
 *  writing fonts.
 *
 *  PARAMS :
 *  filename  Name of the BDF file
 *
 *  RETURN VALUE :
 *      ID of the font if succeded or -1 otherwise
 *
 */

int gr_load_bdf( const char * filename )
{
    if ( !filename ) return -1;

    file * fp;
    char line[2048];
    uint8_t * ptr, * optr;
    FONT * font;
    int id, x, y, i;
    int error = 0;

    uint8_t nibbleh[256];
    uint8_t nibblel[256];

    int default_xadvance = 0;
    int default_yadvance = 0;
    int in_char = 0;
    int encoding = -1;
    int width = 0;
    int height = 0;
    int xoffset = 0;
    int yoffset = 0;
    int xadvance = 0;
    int yadvance = 0;
    int minyoffset = 0;
    int len;

    /* Arrays used to convert hex ASCII to binary */

    memset( nibbleh, 0, 256 );
    memset( nibblel, 0, 256 );

    for ( i = '0' ; i <= '9' ; i++ )
    {
        nibbleh[i] = (( i - '0' ) << 4 );
        nibblel[i] = i - '0';
    }
    for ( i = 10 ; i <= 15 ; i++ )
    {
        nibbleh['A' + i - 10] = ( i << 4 );
        nibbleh['a' + i - 10] = ( i << 4 );
        nibblel['A' + i - 10] = i;
        nibblel['a' + i - 10] = i;
    }

    /* Open the file and create the font */

    fp = file_open( filename, "r" );
    if ( !fp ) return -1;

    id = gr_font_new( CHARSET_ISO8859, 1 );
    if ( id < 0 ) return -1;
    font = fonts[id];
    font->maxwidth = 0;
    font->maxheight = 0;

    /* Process the file, a line each time */

    for ( line[2047] = 0 ; ; )
    {
        if ( !( len = file_gets( fp, line, 2047 ) ) ) break;
        if ( line[len-1] == '\n' ) line[len-1] = '\0';

        /* Handle global-level commands */

        if ( strncmp( line, "DWIDTH ", 7 ) == 0 && !in_char )
        {
            default_xadvance = atoi( line + 7 );
            ptr = ( uint8_t * ) strchr( line + 7, ' ' );
            if ( ptr ) default_yadvance = atoi( ( char * ) ptr + 1 );
        }
        else if ( strncmp( line, "STARTCHAR", 9 ) == 0 )
        {
            in_char = 1;
            encoding = -1;
            height = 0;
            xadvance = default_xadvance;
            yadvance = default_yadvance;
        }
        else if ( strncmp( line, "ENDCHAR", 7 ) == 0 )
        {
            in_char = 0;
        }

        /* Handle character-level commands */

        else if ( strncmp( line, "DWIDTH ", 7 ) == 0 && in_char )
        {
            xadvance = atoi( line + 7 );
            ptr = ( uint8_t * ) strchr( line + 7, ' ' );
            if ( ptr ) yadvance = atoi( ( char * ) ptr + 1 );
        }
        else if ( strncmp( line, "ENCODING ", 9 ) == 0 && in_char )
        {
            encoding = atoi( line + 9 );
            if ( encoding == -1 )
            {
                ptr = ( uint8_t * ) strchr( line + 7, ' ' );
                if ( ptr ) encoding = atoi( ( char * ) ptr + 1 );
            }
        }
        else if ( strncmp( line, "BBX ", 4 ) == 0 && in_char )
        {
            width = atoi( ( char * ) line + 4 );
            if ( width & 7 ) width = ( width & ~7 ) + 8;
            if (( ptr = ( uint8_t * ) strchr( ( char * ) line + 4, ' ' ) ) == NULL ) continue;
            height = atoi( ( char * ) ptr + 1 );
            if (( ptr = ( uint8_t * ) strchr( ( char * ) ptr + 1, ' ' ) ) == NULL ) continue;
            xoffset = atoi( ( char * ) ptr + 1 );
            if (( ptr = ( uint8_t * ) strchr( ( char * ) ptr + 1, ' ' ) ) == NULL ) continue;
            yoffset = atoi( ( char * ) ptr + 1 );
        }
        else if ( strncmp( line, "BITMAP", 6 ) == 0 )
        {
            /* Read bitmap data */
            if ( encoding >= 0 && encoding < 256 && height > 0 )
            {
                font->glyph[encoding].xadvance = xadvance;
                font->glyph[encoding].yadvance = yadvance;
                font->glyph[encoding].xoffset  = xoffset;
                font->glyph[encoding].yoffset  = -yoffset - height;

                if ( minyoffset > -yoffset - height ) minyoffset = -yoffset - height;

                error = 1;
                font->glyph[encoding].bitmap = bitmap_new( encoding, width, height, 1 );
                if ( font->glyph[encoding].bitmap == 0 ) break;
                bitmap_add_cpoint( font->glyph[encoding].bitmap, 0, 0 ) ;

                if ( font->maxwidth < width ) font->maxwidth = width;
                if ( font->maxheight < height ) font->maxheight = height;

                for ( y = 0 ; y < height ; y++ )
                {
                    if ( !( len = file_gets( fp, line, 2047 ) ) ) break;
                    if ( line[len-1] == '\n' ) line[len-1] = '\0';
                    ptr  = ( uint8_t * ) line;
                    optr = ( uint8_t * ) font->glyph[encoding].bitmap->data + font->glyph[encoding].bitmap->pitch * y;

                    for ( x = 0 ; x < width ; x += 8 )
                    {
                        if ( !ptr[0] || !ptr[1] ) break;
                        *optr++ = nibbleh[ptr[0]] | nibblel[ptr[1]];
                        ptr += 2;
                    }
                }
                if ( y != height ) break;
                error = 0;
            }
        }
    }

    file_close( fp );

    if ( error )
    {
        gr_font_destroy( id );
        return -1;
    }

    /* Adjust yoffsets to positive */

    for ( i = 0 ; i < 256 ; i++ ) font->glyph[i].yoffset -= minyoffset;

    if ( font->glyph[32].xadvance == 0 ) font->glyph[32].xadvance = font->glyph['j'].xadvance;

//    fonts[font_count] = font ;
    return id /* font_count++ */ ;
}

/* --------------------------------------------------------------------------- */
