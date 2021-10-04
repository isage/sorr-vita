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

#include "bgddl.h"
#include "dlvaracc.h"

/* --------------------------------------------------------------------------- */

/* Conversion tables - used by 16 bits conversions - 256K */
static uint16_t * convert565ToScreen = NULL ;
static uint16_t * convertScreenTo565 = NULL ;
static int conversion_tables_ok = 0 ;

/* Alpha multiplication tables - multiply a color by its alpha.
 * There will be less than 256 levels
 */
static uint16_t * alpha16[ 256 ];
static uint8_t * alpha8[ 256 ];
static int alpha16_tables_ok = 0 ;
static int alpha8_tables_ok = 0 ;

/* --------------------------------------------------------------------------- */
/* used for variable access                                                    */
/* --------------------------------------------------------------------------- */

enum {
    ALPHA_STEPS = 0
};

char * __bgdexport( libgrbase, globals_def ) =
    "alpha_steps = 16;\n"
    ;

/* --------------------------------------------------------------------------- */
/* Son las variables que se desea acceder.                                     */
/* El interprete completa esta estructura, si la variable existe.              */
/* (usada en tiempo de ejecucion)                                              */

DLVARFIXUP __bgdexport( libgrbase, globals_fixup )[] =
{
    /* Nombre de variable global, puntero al dato, tamaño del elemento, cantidad de elementos */
    { "alpha_steps" , NULL, -1, -1 },
    { NULL , NULL, -1, -1 }
};

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : init_alpha16_tables
 *
 *  Init the 16 bit alpha tables (create as many tables as the parameter)
 *  Each alpha table has a 128K memory footprint. Having many alpha tables
 *  provides more transparency values.
 *
 *  PARAMS :
 *  count   Number of tables
 *
 *  RETURN VALUE :
 *      None
 *
 */

static void init_alpha16_tables( int count )
{
    int i, color, inc, next = 0, factor;
    uint16_t * table16 = NULL;

    if ( count <= 0 ) count = 1;
    if ( count > 128 ) count = 128;

    if ( alpha16_tables_ok == count ) return ;

    inc = 256 / count;

    /* Destroy existing tables */

    if ( alpha16_tables_ok )
    {
        for ( table16 = NULL, i = 0 ; i < 256 ; i++ )
        {
            if ( alpha16[ i ] != table16 )
            {
                table16 = alpha16[ i ];
                free( table16 );
            }
            alpha16[ i ] = NULL;
        }
    }

    /* Make new ones */

    for ( i = 0 ; i < 256 ; i++ )
    {
        if ( i == next )
        {
            table16 = malloc( 131072 );
            factor = next + inc / 2;
            next += inc;
            if ( factor > 255 ) factor = 256;

            for ( color = 0 ; color < 65536 ; color++ )
            {
                int r = (( color & sys_pixel_format->Rmask ) >> sys_pixel_format->Rshift << sys_pixel_format->Rloss ) ;
                int g = (( color & sys_pixel_format->Gmask ) >> sys_pixel_format->Gshift << sys_pixel_format->Gloss ) ;
                int b = (( color & sys_pixel_format->Bmask ) >> sys_pixel_format->Bshift << sys_pixel_format->Bloss ) ;

                table16[ color ] =
                    (((( r * factor ) >> 8 ) >> sys_pixel_format->Rloss ) << sys_pixel_format->Rshift ) |
                    (((( g * factor ) >> 8 ) >> sys_pixel_format->Gloss ) << sys_pixel_format->Gshift ) |
                    (((( b * factor ) >> 8 ) >> sys_pixel_format->Bloss ) << sys_pixel_format->Bshift ) ;
            }
        }
        alpha16[ i ] = table16;
    }

    alpha16_tables_ok = count;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : init_alpha8_tables
 *
 *  Init the 8 bit alpha tables (create as many tables as the parameter)
 *  Each alpha table has a 64K memory footprint. Having many alpha tables
 *  provides more transparency values. Those tables should be updated
 *  when the palette changes.
 *
 *  PARAMS :
 *  count   Number of tables
 *
 *  RETURN VALUE :
 *      None
 *
 */

static void init_alpha8_tables( int count )
{
    int i, color, color2, inc, next = 0, factor;
    uint8_t * table8 = NULL;
    rgb_component * rgb ;

    if ( count <= 0 ) count = 1;
    if ( count > 128 ) count = 128;

    if ( alpha16_tables_ok == count ) return ;

    inc = 256 / count;

    /* Destroy existing tables */

    if ( alpha8_tables_ok )
    {
        for ( table8 = NULL, i = 0; i < 256; i++ )
        {
            if ( alpha8[ i ] != table8 )
            {
                table8 = alpha8[ i ];
                free( table8 );
            }
            alpha8[ i ] = NULL;
        }
    }

    /* Make new ones */

    if ( !sys_pixel_format->palette )
        rgb = ( rgb_component * ) default_palette;
    else
        rgb = sys_pixel_format->palette->rgb ;

    for ( i = 0; i < 256; i++ )
    {
        if ( i == next )
        {
            table8 = malloc( 65536 );
            factor = next + inc / 2;
            next += inc;
            if ( factor > 255 ) factor = 255;

            for ( color = 0; color < 256; color++ )
            {
                for ( color2 = 0; color2 < 256; color2++ )
                {
                    int r = ( rgb[ color ].r * factor + rgb[ color2 ].r * ( 255 - factor ) ) >> 8 ;
                    int g = ( rgb[ color ].g * factor + rgb[ color2 ].g * ( 255 - factor ) ) >> 8 ;
                    int b = ( rgb[ color ].b * factor + rgb[ color2 ].b * ( 255 - factor ) ) >> 8 ;
                    table8[( color << 8 ) + color2 ] = gr_find_nearest_color( r, g, b );
                }
                table8[ color ] = color;
            }
        }
        alpha8[ i ] = table8;
    }

    alpha8_tables_ok = count;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : init_conversion_tables
 *
 *  Static routine used to initialize the 16 bits conversion
 *  tables (this only needs to be done once)
 *
 *  PARAMS :
 *  None
 *
 *  RETURN VALUE :
 *      None
 *
 */

static void init_conversion_tables()
{
    uint8_t r, g, b ;
    int n ;

    /* Alloc space for the lookup tables */

    convert565ToScreen = ( uint16_t * ) malloc( sizeof( uint16_t ) * 65536 );
    if ( !convert565ToScreen ) return ;

    convertScreenTo565 = ( uint16_t * ) malloc( sizeof( uint16_t ) * 65536 );
    if ( !convertScreenTo565 )  // No memory
    {
        free( convert565ToScreen );
        return ;
    }

    /* Special case if screen already in 565 format */

#ifdef COLORSPACE_BGR
    if ( sys_pixel_format->Rmask == 0x001F &&
            sys_pixel_format->Gmask == 0x07E0 &&
            sys_pixel_format->Bmask == 0xF800 )
#else
    if ( sys_pixel_format->Rmask == 0xF800 &&
            sys_pixel_format->Gmask == 0x07E0 &&
            sys_pixel_format->Bmask == 0x001F )
#endif
    {
        for ( n = 0; n < 65536; n++ )
        {
            convert565ToScreen[ n ] = n;
            convertScreenTo565[ n ] = n;
        }
        return ;
    }

    /* Create a fast lookup array */

    for ( n = 0; n < 65536; n++ )
    {
        /* Calculate conversion from 565 to screen format */

#ifdef COLORSPACE_BGR
        b = (( n >> 8 ) & 0xF8 ) >> sys_pixel_format->Bloss ;
        g = (( n >> 3 ) & 0xFC ) >> sys_pixel_format->Gloss ;
        r = (( n << 3 ) & 0xF8 ) >> sys_pixel_format->Rloss ;
#else
        r = (( n >> 8 ) & 0xF8 ) >> sys_pixel_format->Rloss ;
        g = (( n >> 3 ) & 0xFC ) >> sys_pixel_format->Gloss ;
        b = (( n << 3 ) & 0xF8 ) >> sys_pixel_format->Bloss ;
#endif

        convert565ToScreen[ n ] = ( r << sys_pixel_format->Rshift ) | ( g << sys_pixel_format->Gshift ) | ( b << sys_pixel_format->Bshift ) ;

        /* Calculate conversion from 565 to screen format */

        r = ((( n & sys_pixel_format->Rmask ) >> sys_pixel_format->Rshift ) << sys_pixel_format->Rloss );
        g = ((( n & sys_pixel_format->Gmask ) >> sys_pixel_format->Gshift ) << sys_pixel_format->Gloss );
        b = ((( n & sys_pixel_format->Bmask ) >> sys_pixel_format->Bshift ) << sys_pixel_format->Bloss );

#ifdef COLORSPACE_BGR
        convertScreenTo565[ n ] = (( b & 0xF8 ) << 8 ) | (( g & 0xFC ) << 3 ) | (( r & 0xF8 ) >> 3 ) ;
#else
        convertScreenTo565[ n ] = (( r & 0xF8 ) << 8 ) | (( g & 0xFC ) << 3 ) | (( b & 0xF8 ) >> 3 ) ;
#endif
    }
    conversion_tables_ok = 1;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : gr_convert16_565ToScreen
 *
 *  Convert a sequence of 16 bits pixels from 5:6:5 format to
 *  the format used by the screen (usually 5:5:5 or 5:6:5)
 *
 *  PARAMS :
 *  ptr    Pointer to the first pixel
 *  len    Number of pixels (not bytes!)
 *
 *  RETURN VALUE :
 *      None
 *
 */

void gr_convert16_565ToScreen( uint16_t * ptr, int len )
{
    if ( !conversion_tables_ok ) init_conversion_tables();

    while ( len-- )
    {
        *ptr = convert565ToScreen[ *ptr ] ;
        ptr++;
    }
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : gr_convert16_ScreenTo565
 *
 *  Convert a sequence of 16 bits pixels in screen format
 *  (usually 5:5:5 or 5:6:5) to the 5:6:5 format used to
 *  store 16 bits pixel values to files in disk
 *
 *  PARAMS :
 *  ptr    Pointer to the first pixel
 *  len    Number of pixels (not bytes!)
 *
 *  RETURN VALUE :
 *      None
 *
 */

void gr_convert16_ScreenTo565( uint16_t * ptr, int len )
{
    if ( !conversion_tables_ok ) init_conversion_tables();

    while ( len-- )
    {
        *ptr = convertScreenTo565[ *ptr ] ;
        ptr++;
    }
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : gr_fade16
 *
 *  Fade a 16-bit graphic a given ammount. Fading values are given in percent
 *  (0% = black, 100% = original color, 200% = full color value)
 *
 *  PARAMS :
 *  graph   Pointer to the graphic object
 *  r    Percent of Red component
 *  g    Percent of Green component
 *  b    Percent of Blue component
 *
 *  RETURN VALUE :
 *      None
 *
 */

void gr_fade16( GRAPH * graph, int r, int g, int b )
{
    uint32_t Rtable[ 32 ];
    uint32_t Gtable[ 32 ];
    uint32_t Btable[ 32 ];
    uint32_t x, y;
    uint32_t Rmask;
    uint32_t Rshift;
    uint32_t Gmask;
    uint32_t Gshift;
    uint32_t Bmask;
    uint32_t Bshift;

    for ( x = 0 ; x < 32 ; x++ )
    {
        int c = 8 * x + 7;

        if ( r <= 100 )
            Rtable[ x ] = ( c * r / 100 ) >> sys_pixel_format->Rloss << sys_pixel_format->Rshift;
        else
            Rtable[ x ] = ( c + ( 255 - c ) * ( r - 100 ) / 100 ) >> sys_pixel_format->Rloss << sys_pixel_format->Rshift;

        if ( g <= 100 )
            Gtable[ x ] = ( c * g / 100 ) >> sys_pixel_format->Gloss << sys_pixel_format->Gshift;
        else
            Gtable[ x ] = ( c + ( 255 - c ) * ( g - 100 ) / 100 ) >> sys_pixel_format->Gloss << sys_pixel_format->Gshift;

        if ( b <= 100 )
            Btable[ x ] = ( c * b / 100 ) >> sys_pixel_format->Bloss << sys_pixel_format->Bshift;
        else
            Btable[ x ] = ( c + ( 255 - c ) * ( b - 100 ) / 100 ) >> sys_pixel_format->Bloss << sys_pixel_format->Bshift;
    }

    Rmask = sys_pixel_format->Rmask;
    Gmask = sys_pixel_format->Gmask;
    Bmask = sys_pixel_format->Bmask;

    Rshift = sys_pixel_format->Rshift - sys_pixel_format->Rloss + 3;
    Gshift = sys_pixel_format->Gshift - sys_pixel_format->Gloss + 3;
    Bshift = sys_pixel_format->Bshift - sys_pixel_format->Bloss + 3;

    if ( graph->format->depth == 16 )
    {
        char * p = graph->data ;
        uint16_t * ptr ;

        for ( y = 0 ; y < graph->height ; y++ )
        {
            ptr = ( uint16_t * ) p ;

            for ( x = 0 ; x < graph->width ; x++, ptr++ )
            {
                *ptr = ( Rtable[(( *ptr & Rmask ) >> Rshift )] | Gtable[(( *ptr & Gmask ) >> Gshift )] | Btable[(( *ptr & Bmask ) >> Bshift )] );
            }
            p += graph->pitch ;
        }
    }
    else if ( graph->format->depth == 32 )
    {
        char * p = graph->data ;
        uint32_t * ptr ;

        for ( y = 0 ; y < graph->height ; y++ )
        {
            ptr = ( uint32_t * ) p ;
            for ( x = 0 ; x < graph->width ; x++, ptr++ )
            {
                *ptr = ( Rtable[(( *ptr & Rmask ) >> Rshift )] | Gtable[(( *ptr & Gmask ) >> Gshift )] | Btable[(( *ptr & Bmask ) >> Bshift )] );
            }
            p += graph->pitch ;
        }
    }
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : gr_alpha16
 *
 *  Get an alpha multiplication table (a table that, given a 16 bit color,
 *  returns the color multiplied by the alpha value)
 *
 *  PARAMS :
 *  alpha   Alpha value for the requested table
 *
 *  RETURN VALUE :
 *      None
 *
 */

uint16_t * gr_alpha16( int alpha )
{
    if ( !alpha16_tables_ok ) init_alpha16_tables( GLODWORD( libgrbase, ALPHA_STEPS ) );
    return alpha16[ alpha ];
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : gr_alpha8
 *
 *  Get an alpha translation table (a table that, given two 8 bit color,
 *  returns the composite color given the alpha value)
 *
 *  PARAMS :
 *  alpha   Alpha value for the requested table
 *
 *  RETURN VALUE :
 *      None
 *
 */

uint8_t * gr_alpha8( int alpha )
{
    if ( !alpha8_tables_ok ) init_alpha8_tables( GLODWORD( libgrbase, ALPHA_STEPS ) );
    return ( uint8_t * ) alpha8[ alpha ];
}

/* --------------------------------------------------------------------------- */
