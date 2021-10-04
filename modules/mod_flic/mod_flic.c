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

#include <string.h>

#include <math.h>
#include <stdlib.h>

#include "bgddl.h"
#include "dlvaracc.h"

#include "libgrbase.h"
#include "libblit.h"
#include "librender.h"

#include "mod_flic.h"

/* --------------------------------------------------------------------- */
/* Librería para reproducir ficheros FLI directamente desde el disco     */
/* --------------------------------------------------------------------- */

static FLIC * current_fli = 0 ;

/* ----------------------------------------------------------------- */

static int info_fli( FLIC * flic, REGION * clip, int * z, int * drawme ) ;
static void draw_fli( FLIC * flic, REGION * clip ) ;
static void flic_destroy( FLIC * flic ) ;
static FLIC * flic_open( const char * filename ) ;
static FLIC * flic_do_delta( FLIC * flic ) ;
static FLIC * flic_do_delta_flc( FLIC * flic ) ;
static FLIC * flic_do_color( FLIC * flic ) ;
static FLIC * flic_do_brun( FLIC * flic ) ;
static FLIC * flic_do_chunk( FLIC * flic ) ;
static FLIC * flic_do_frame( FLIC * flic ) ;
static void flic_reset( FLIC * flic ) ;
static int modflic_start( INSTANCE * my, int * params ) ;
static int modflic_reset( INSTANCE * my, int * params ) ;
static int modflic_end( INSTANCE * my, int * params ) ;
static int modflic_frame( INSTANCE * my, int * params ) ;

/* ----------------------------------------------------------------- */

static int info_fli( FLIC * flic, REGION * clip, int * z, int * drawme )
{
    int changed ;
    int ms ;

    * drawme = 1;
    * z = flic->z;

    changed =
        flic->saved_x       != flic->x      ||
        flic->saved_y       != flic->y      ||
        flic->saved_z       != flic->z      ||
        flic->saved_angle   != flic->angle  ||
        flic->saved_size    != flic->size   ||
        flic->saved_flags   != flic->flags ;

    if ( changed )
    {
        flic->saved_x       = flic->x    ;
        flic->saved_y       = flic->y    ;
        flic->saved_z       = flic->z    ;
        flic->saved_angle   = flic->angle;
        flic->saved_size    = flic->size ;
        flic->saved_flags   = flic->flags;
    }

    ms = SDL_GetTicks() ;
    if ( flic->last_frame_ms + flic->speed_ms < ms && !flic->finished )
        changed |= ( flic_do_frame( flic ) != NULL ) ;

    if ( !changed ) return 0;

    if ( flic->angle || flic->size != 100 )
    {
        gr_get_bbox( clip,
                0,
                flic->x + flic->bitmap->width / 2,
                flic->y + flic->bitmap->height  / 2,
                flic->flags,
                flic->angle,
                flic->size,
                flic->size,
                flic->bitmap
                   ) ;
    }
    else
    {
        clip->x  = flic->x ;
        clip->x2 = flic->x + flic->bitmap->width ;
        clip->y  = flic->y ;
        clip->y2 = flic->y + flic->bitmap->height ;
    }

    return 1;
}

static void draw_fli( FLIC * flic, REGION * clip )
{
    if ( flic->angle || flic->size != 100 )
        gr_rotated_blit( 0,
                clip,
                flic->x + flic->bitmap->width / 2,
                flic->y + flic->bitmap->height / 2,
                flic->flags,
                flic->angle,
                flic->size,
                flic->size,
                flic->bitmap ) ;
    else
        gr_blit( 0,
                clip,
                flic->x + flic->bitmap->width / 2,
                flic->y + flic->bitmap->height / 2,
                flic->flags,
                flic->bitmap ) ;
}

/* --------------------------------------------------------------------- */

static void flic_destroy( FLIC * flic )
{
    if ( !flic ) return ;

    file_close( flic->fp ) ;
    if ( flic->objid ) gr_destroy_object( flic->objid );
    if ( flic->bitmap ) bitmap_destroy( flic->bitmap ) ;
    if ( flic->frame ) free( flic->frame ) ;
    free( flic ) ;
}

static FLIC * flic_open( const char * filename )
{
    FLIC * flic ;

    flic = ( FLIC * ) malloc( sizeof( FLIC ) ) ;
    if ( !flic ) return 0 ;

    flic->objid = 0;

    flic->fp = file_open( filename, "rb" ) ;
    if ( !flic->fp )
    {
        free( flic ) ;
        return 0 ;
    }
    flic->frame_reserved = 8192 ;
    flic->frame = ( FLIC_FRAME * ) malloc( 8192 ) ;
    if ( !flic->frame )
    {
        flic_destroy( flic ) ;
        return 0 ;
    }

    if ( !file_read( flic->fp, &flic->header, sizeof( FLIC_HEADER ) ) )
    {
        flic_destroy( flic ) ;
        return 0 ;
    }

    if ( flic->header.type != 0xAF11 && flic->header.type != 0xAF12 )
    {
        /* Tipo de FLIC no reconocido */
        flic_destroy( flic ) ;
        return 0 ;
    }

    flic->bitmap = bitmap_new( 0, flic->header.width, flic->header.height, 8 ) ;
    if ( !flic->bitmap )
    {
        /* Tamaño incorrecto */
        flic_destroy( flic ) ;
        return 0 ;
    }

    if ( !flic->header.oframe1 ) flic->header.oframe1 = file_pos( flic->fp ) ;

    flic->current_frame = 0 ;
    flic->finished      = 0 ;
    flic->last_frame_ms = SDL_GetTicks() ;

    if ( flic->header.type == 0xAF11 )
        flic->speed_ms = ( int )(( 1000.0F / 70.0F ) * flic->header.speed ) ;
    else
        flic->speed_ms = flic->header.speed ;

    flic->objid = gr_new_object( 0, info_fli, draw_fli, flic );

    return flic ;
}

static FLIC * flic_do_delta( FLIC * flic )
{
    GRAPH       * bitmap = flic->bitmap ;
    int         first_line, line_count ;
    uint8_t       * ptr, * optr, * loptr, packet_count ;
    int         size;

    first_line = flic->chunk->delta_fli.first_line ;
    line_count = flic->chunk->delta_fli.line_count ;
    ptr        = flic->chunk->delta_fli.data ;

    loptr = ( uint8_t * )bitmap->data + bitmap->pitch * first_line ;

    while ( line_count-- > 0 )
    {
        optr = loptr ;
        loptr += bitmap->pitch ;
        packet_count = *ptr++ ;

        while ( packet_count-- )
        {
            optr += *ptr++ ;
            size = *( Sint8 * )ptr++ ;
            if ( size > 0 )
            {
                memcpy( optr, ptr, size ) ;
                ptr += size ;
            }
            else if ( size < 0 )
            {
                size = -size ;
                memset( optr, *ptr++, size ) ;
            }
            optr += size ;
        }
    }

    return flic ;
}

static FLIC * flic_do_delta_flc( FLIC * flic )
{
    GRAPH   * bitmap = flic->bitmap ;
    int     line_count ;
    uint16_t  * ptr, opcode ;
    uint8_t   * optr, * loptr ;
    Sint8   data_count ;

    ptr = ( uint16_t * )flic->chunk->raw.data ;

    line_count = *ptr++ ;

    optr = bitmap->data ;

    while ( line_count > 0 )
    {
        opcode = *ptr++ ;

        switch ( opcode & 0xC000 )
        {
            case 0x0000:
                loptr = optr ;
                while ( opcode-- > 0 )
                {
                    optr += *( uint8_t * )ptr ;
                    data_count = *(( Sint8 * )ptr + 1 ) ;
                    ptr++ ;

                    if ( data_count > 0 )
                    {
                        memcpy( optr, ptr, ( int )data_count * 2 ) ;
                        ptr += data_count ;
                        optr += data_count * 2 ;
                    }
                    else if ( data_count < 0 )
                    {
                        data_count = -data_count ;
                        while ( data_count-- )
                        {
                            *optr++ = ( *ptr & 0xFF ) ;
                            *optr++ = ( *ptr >> 8 ) ;
                        }
                        ptr++ ;
                    }
                }

                optr = loptr ;
                optr += bitmap->width ;
                line_count-- ;
                break ;

            case 0x4000:
                return 0 ;

            case 0x8000:
                optr[bitmap->width-1] = ( opcode & 0xFF ) ;
                break ;

            case 0xC000:
                optr += bitmap->pitch * -( Sint16 )opcode;
                break ;
        }
    }

    return flic ;
}

static FLIC * flic_do_color( FLIC * flic )
{
    uint8_t * ptr ;
    int packet_count ;
    int copy_count ;
    rgb_component * palette ;
    rgb_component * color;
    PALETTE * pal ;

    if ( sys_pixel_format->depth == 8 )
    {
        if ( ! sys_pixel_format->palette )
        {
            sys_pixel_format->palette = pal_new( NULL ) ;
            palette_changed = 1 ;
        }
        pal = sys_pixel_format->palette ;
    }
    else
    {
        if ( !flic->bitmap->format->palette )
        {
            flic->bitmap->format->palette = pal_new( NULL ) ;
            palette_changed = 1 ;
        }
        pal = flic->bitmap->format->palette ;
    }

    palette = pal->rgb;

    ptr = flic->chunk->raw.data ;

    packet_count = *( uint16_t * )ptr ;
    ptr += 2 ;

    while ( packet_count-- > 0 )
    {
        color = &palette[*ptr++] ;
        copy_count = *ptr++ ;
        if ( copy_count < 1 ) copy_count = 256 ;

        if ( flic->chunk->header.type == CHUNK_COLOR_64 )
        {
            while ( copy_count-- )
            {
                color->r = ( *ptr++ << 2 ) ;
                color->g = ( *ptr++ << 2 ) ;
                color->b = ( *ptr++ << 2 ) ;
                color++ ;
            }
        }
        else
        {
            while ( copy_count-- )
            {
                color->r = *ptr++ ;
                color->g = *ptr++ ;
                color->b = *ptr++ ;
                color++ ;
            }
        }
    }

    pal_refresh( pal );

    return flic ;
}

static FLIC * flic_do_brun( FLIC * flic )
{
    GRAPH       * bitmap = flic->bitmap ;
    int         line_count ;
    uint8_t       * ptr, * optr, * loptr ;
    uint16_t     remaining_width ;
    int         size;

    ptr        = flic->chunk->raw.data ;
    loptr      = bitmap->data ;
    line_count = bitmap->height ;

    while ( line_count-- )
    {
        optr = loptr;
        loptr += bitmap->pitch ;
        ptr++ ;
        remaining_width = bitmap->width ;

        while ( remaining_width > 0 )
        {
            size = *( Sint8 * )ptr++;
            if ( size < 0 )
            {
                size = -size;
                memcpy( optr, ptr, size ) ;
                ptr += size ;
            }
            else if ( size > 0 )
            {
                memset( optr, *ptr++, size ) ;
            }
            optr += size ;
            remaining_width -= size ;
        }
    }

    return flic ;
}

static FLIC * flic_do_chunk( FLIC * flic )
{
    uint32_t y;
    uint8_t * data ;
    uint8_t * rawdata ;

    /* Procesa el contenido del chunk actual */

    switch ( flic->chunk->header.type )
    {
        case CHUNK_BLACK:
            data = ( uint8_t * )flic->bitmap->data ;
            for ( y = flic->bitmap->height ; y-- ; )
            {
                memset( data, 0, flic->bitmap->pitch ) ;
                data += flic->bitmap->pitch ;
            }
            break ;

        case CHUNK_FLI_COPY:
            data = ( uint8_t * )flic->bitmap->data ;
            rawdata = ( uint8_t * )flic->chunk->raw.data ;
            for ( y = flic->bitmap->height ; y-- ; )
            {
                memcpy( data, rawdata, flic->bitmap->width ) ;
                data += flic->bitmap->pitch ;
                rawdata += flic->bitmap->width ;
            }
            break ;

        case CHUNK_DELTA_FLI:
            if ( !flic_do_delta( flic ) ) return 0 ;
            break ;

        case CHUNK_DELTA_FLC:
            if ( !flic_do_delta_flc( flic ) ) return 0 ;
            break ;

        case CHUNK_BYTE_RUN:
            if ( !flic_do_brun( flic ) ) return 0 ;
            break ;

        case CHUNK_COLOR_256:
        case CHUNK_COLOR_64:
            if ( !flic_do_color( flic ) ) return 0 ;
            break ;

        case CHUNK_STAMP:
            break ;

        default:
            /* Tipo de chunk desconocido */
            return 0 ;
    }

    return flic ;
}

static FLIC * flic_do_frame( FLIC * flic )
{
    int chunkno = 0 ;

    /* Cuenta el frame y vuelve al inicio si es necesario */

    flic->current_frame++ ;

    if ( flic->current_frame >= flic->header.frames )
    {
        file_seek( flic->fp, flic->header.oframe1, SEEK_SET ) ;
        flic->current_frame = 1 ;
        flic->finished = 1 ;
        return flic ;
    }

    do
    {
        /* Recupera información del siguiente chunk del fichero */

        if ( !file_read( flic->fp, flic->frame, sizeof( FLIC_FRAME ) ) ) return 0 ;

        /* Tipo de frame incorrecto */
        if ( flic->frame->type != CHUNK_FRAME && flic->frame->type != CHUNK_PREFIX ) return 0 ;

        /* Fichero corrupto */
        if ( flic->frame->size < sizeof( FLIC_FRAME ) ) return 0;

        /* Reserva la memoria necesaria y carga el chunk */

        if ( flic->frame_reserved < flic->frame->size )
        {
            flic->frame_reserved = flic->frame->size ;
            flic->frame = ( FLIC_FRAME * ) realloc( flic->frame, flic->frame_reserved ) ;

            /* Error: sin memoria */
            if ( !flic->frame ) return 0 ;
        }

        /* If it's a prefix frame, skip it. */

        if ( flic->frame->size > sizeof( FLIC_FRAME ) )
            if ( !file_read( flic->fp, &flic->frame[1], flic->frame->size - sizeof( FLIC_FRAME ) ) ) return 0 ;

    }
    while ( flic->frame->type != CHUNK_FRAME ) ;

    /* Procesa cada sub-chunk */

    flic->chunk = ( FLIC_CHUNK * ) & flic->frame[1] ;

    for ( chunkno = 0 ; chunkno < flic->frame->chunks ; chunkno++ )
    {
        if ( !flic_do_chunk( flic ) ) return 0 ;
        flic->chunk = ( FLIC_CHUNK * )((( uint8_t * )flic->chunk ) + flic->chunk->header.size ) ;
    }

    flic->last_frame_ms += flic->speed_ms ;
    return flic ;
}

static void flic_reset( FLIC * flic )
{
    flic->current_frame = 0 ;
    flic->finished = 0 ;
    file_seek( flic->fp, flic->header.oframe1, SEEK_SET ) ;
}

/* ----------------------------------------------------------------- */
/* Reproducción de FLI */

/*
 *  FUNCTION : modflic_start
 *
 *  Load & start playing a FLI/FLC animation
 *
 *  PARAMS:
 *
 *      file : filename/path for the FLI/FLC file
 *      x,y  : screen position
 *
 */

static int modflic_start( INSTANCE * my, int * params )
{
    const char * str = string_get( params[0] ) ;
    if ( !str ) return 0 ;

    if ( current_fli ) flic_destroy( current_fli ) ;

    current_fli = flic_open( str ) ;

    string_discard( params[0] ) ;

    if ( current_fli )
    {
        current_fli->x = params[1] ;
        current_fli->y = params[2] ;
        current_fli->z = 0;
        current_fli->angle = 0;
        current_fli->size = 100;
        current_fli->flags = 0;

        return current_fli->header.frames ;
    }

    return 0 ;
}

/*
 *  FUNCTION : modflic_reset
 *
 *  Reset current FLI/FLC animation to frame 0
 *
 *  PARAMS:
 *
 *      No params
 *
 */

static int modflic_reset( INSTANCE * my, int * params )
{
    if ( current_fli ) flic_reset( current_fli ) ;
    return 1 ;
}

/*
 *  FUNCTION : modflic_end
 *
 *  Stop current FLI/FLC animation
 *
 *  PARAMS:
 *
 *      No params
 *
 */

static int modflic_end( INSTANCE * my, int * params )
{
    if ( current_fli )
    {
        flic_destroy( current_fli ) ;
        current_fli = 0 ;
    }
    return 1 ;
}

/*
 *  FUNCTION : modflic_frame
 *
 *  Check status for the current FLI/FLC animation
 *
 *  RETURN VALUE:
 *
 *      Current frame or 0 if animation has ended
 *
 *  PARAMS:
 *
 *      No params
 *
 */

static int modflic_frame( INSTANCE * my, int * params )
{
    if ( current_fli ) return current_fli->finished ? 0 : current_fli->current_frame ;
    return 0 ;
}

/* --------------------------------------------------------------------------- */

static int modflic_startx1( INSTANCE * my, int * params )
{
    FLIC * flic ;
    const char * str = string_get( params[0] ) ;
    if ( !str ) return 0 ;

    flic = flic_open( str ) ;

    string_discard( params[0] ) ;

    if ( flic )
    {
        flic->x = params[1] ;
        flic->y = params[2] ;
        flic->z = 0 ;
        flic->angle = 0;
        flic->size = 100;
        flic->flags = 0;

        return ( int ) flic ;
    }

    return 0 ;
}

static int modflic_startx2( INSTANCE * my, int * params )
{
    FLIC * flic ;
    const char * str = string_get( params[0] ) ;
    if ( !str ) return 0 ;

    flic = flic_open( str ) ;

    string_discard( params[0] ) ;

    if ( flic )
    {
        flic->x = params[1] ;
        flic->y = params[2] ;
        flic->z = params[3] ;
        flic->angle = params[4];
        flic->size = params[5];
        flic->flags = params[6];

        return ( int ) flic ;
    }

    return 0 ;
}

static int modflic_resetx( INSTANCE * my, int * params )
{
    flic_reset(( FLIC * ) params[0] ) ;
    return 1 ;
}

static int modflic_endx( INSTANCE * my, int * params )
{
    flic_destroy((( FLIC * ) params[0] ) ) ;
    return 1 ;
}

static int modflic_framex( INSTANCE * my, int * params )
{
    return (( FLIC * ) params[0] )->finished ? 0 : (( FLIC * ) params[0] )->current_frame ;
}

static int modflic_params( INSTANCE * my, int * params )
{
    FLIC * flic = ( FLIC * ) params[0] ;

    flic->x = params[1] ;
    flic->y = params[2] ;
    flic->z = params[3] ;
    flic->angle = params[4];
    flic->size = params[5];
    flic->flags = params[6];

    return 1 ;
}

static int modflic_move( INSTANCE * my, int * params )
{
    FLIC * flic = ( FLIC * ) params[0] ;

    flic->x = params[1] ;
    flic->y = params[2] ;

    return 1 ;
}

static int modflic_z( INSTANCE * my, int * params )
{
    (( FLIC * ) params[0] )->z = params[1] ;

    return 1 ;
}

static int modflic_angle( INSTANCE * my, int * params )
{
    (( FLIC * ) params[0] )->angle = params[1] ;

    return 1 ;
}

static int modflic_size( INSTANCE * my, int * params )
{
    (( FLIC * ) params[0] )->size = params[1] ;

    return 1 ;
}

static int modflic_flags( INSTANCE * my, int * params )
{
    (( FLIC * ) params[0] )->flags = params[1] ;

    return 1 ;
}

static int modflic_getinfo( INSTANCE * my, int * params )
{
    FLIC * flic = ( FLIC * ) params[0] ;

    if ((( int * ) params[1] ) ) *(( int * ) params[1] ) = flic->x ;
    if ((( int * ) params[2] ) ) *(( int * ) params[2] ) = flic->y ;
    if ((( int * ) params[3] ) ) *(( int * ) params[3] ) = flic->z ;
    if ((( int * ) params[4] ) ) *(( int * ) params[4] ) = flic->angle ;
    if ((( int * ) params[5] ) ) *(( int * ) params[5] ) = flic->size ;
    if ((( int * ) params[6] ) ) *(( int * ) params[6] ) = flic->flags ;
    if ((( int * ) params[7] ) ) *(( int * ) params[7] ) = flic->header.frames ;
    if ((( int * ) params[8] ) ) *(( int * ) params[8] ) = flic->header.width ;
    if ((( int * ) params[9] ) ) *(( int * ) params[9] ) = flic->header.height ;

    return 1 ;
}

/* ----------------------------------------------------------------- */
/* Declaracion de funciones                                          */

DLSYSFUNCS  __bgdexport( mod_flic, functions_exports )[] =
{
    /* Animaciones FLI */
    { "START_FLI"       , "SII"         , TYPE_INT , modflic_start        },
    { "END_FLI"         , ""            , TYPE_INT , modflic_end          },
    { "FRAME_FLI"       , ""            , TYPE_INT , modflic_frame        },
    { "RESET_FLI"       , ""            , TYPE_INT , modflic_reset        },

    { "FLI_START"       , "SII"         , TYPE_INT , modflic_startx1      },
    { "FLI_START"       , "SIIIIII"     , TYPE_INT , modflic_startx2      },
    { "FLI_END"         , "I"           , TYPE_INT , modflic_endx         },
    { "FLI_FRAME"       , "I"           , TYPE_INT , modflic_framex       },
    { "FLI_RESET"       , "I"           , TYPE_INT , modflic_resetx       },

    { "FLI_PARAMS"      , "IIIIIII"     , TYPE_INT , modflic_params       },
    { "FLI_MOVE"        , "III"         , TYPE_INT , modflic_move         },
    { "FLI_ANGLE"       , "II"          , TYPE_INT , modflic_angle        },
    { "FLI_SIZE"        , "II"          , TYPE_INT , modflic_size         },
    { "FLI_FLAGS"       , "II"          , TYPE_INT , modflic_flags        },
    { "FLI_Z"           , "II"          , TYPE_INT , modflic_z            },
    { "FLI_GETINFO"     , "IPPPPPPPPP"  , TYPE_INT , modflic_getinfo      },

    { NULL              , NULL          , 0        , NULL                 }
};

/* ----------------------------------------------------------------- */
