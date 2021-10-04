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

#include <stdio.h>
#include <stdlib.h>

#include <math.h>

#include "bgddl.h"
#include "dlvaracc.h"
#include "fmath.h"

#include "libgrbase.h"
#include "librender.h"
#include "libvideo.h"
#include "libblit.h"

#include "instance.h"

#include "resolution.h"

#include "mod_m7.h"

/* --------------------------------------------------------------------------- */

extern uint8_t trans_table[256][256] ;
extern int trans_table_updated ;

/* --------------------------------------------------------------------------- */

/* Funciones generales de rotación en 2D
 *
 *  x1  =  x * cos(angulo)  -  y * sin(angulo)
 *  y1  =  x * sin(angulo)  +  y * cos(angulo)
 *
 * */

#define ROTATEDX(x,y,sina,cosa) (fmul(x,cosa) - fmul(y,sina))
#define ROTATEDY(x,y,sina,cosa) (fmul(x,sina) + fmul(y,cosa))

/* Esta estructura guarda información que se recalcula a cada frame.
 * Quizá no sería necesario recalcularlo todo. El incremento podría
 * quedar constante mientras no variara la altura, p.ej. */

typedef struct _lineinfo
{
    fixed   left_bmp_x ;
    fixed   left_bmp_y ;
    fixed   right_bmp_x ;
    fixed   right_bmp_y ;

    fixed   hinc, vinc ;
}
LINEINFO ;

static LINEINFO lines[1280] ;

/* --------------------------------------------------------------------------- */

#ifndef __GNUC__
#define __PACKED
#define inline __inline
#else
#define __PACKED __attribute__ ((packed))
#endif

#ifdef _MSC_VER
#pragma pack(push, 1)
#endif

typedef struct _mode7
{
    int id ;
    REGION * region ;
    GRAPH * indoor ;
    GRAPH * outdoor ;
    GRAPH * dest ;
}
MODE7 ;

typedef struct _mode7_info
{
    uint32_t  camera_id ;
    uint32_t  height ;
    uint32_t  distance ;
    uint32_t  horizon ;
    uint32_t  focus ;
    uint32_t  z ;
    uint32_t  color ;
    uint32_t  flags ;
}
__PACKED
MODE7_INFO ;

#ifdef _MSC_VER
#pragma pack(pop)
#endif

/* --------------------------------------------------------------------------- */

static MODE7 mode7_inf[10] = { { 0 } } ;

/* --------------------------------------------------------------------------- */
/* Globals */
enum {
    M7STRUCTS = 0
};

/* Locals */
enum {
    ANGLE = 0,
    CNUMBER,
    COORDX,
    COORDY,
    CTYPE,
    DISTANCE_1,
    GRAPHSIZE,
    HEIGHT,
    STATUS,
    RESOLUTION
};

/* --------------------------------------------------------------------------- */
/* Definicion de constantes (usada en tiempo de compilacion)         */

DLCONSTANT __bgdexport( mod_m7, constants_def )[] =
{
    { "C_M7", TYPE_INT, C_M7 },
    { NULL  , 0       , 0 }
} ;

/* --------------------------------------------------------------------------- */
/* Definicion de variables globales (usada en tiempo de compilacion) */
char * __bgdexport( mod_m7, globals_def ) =
    "STRUCT m7[9]\n"
    "camera;\n"
    "height = 32;\n"
    "distance = 64;\n"
    "horizon = 0;\n"
    "focus = 256;\n"
    "z = 256;\n"
    "color = 0;\n"
    "flags = 0;\n"
    "END\n" ;

/* --------------------------------------------------------------------------- */
/* Definicion de variables locales (usada en tiempo de compilacion)  */
char * __bgdexport( mod_m7, locals_def ) =
    "ctype;\n"
    "cnumber;\n"
    "height;\n"
    "STRUCT _m7_reserved\n"
    "  distance1;\n"
    "END;\n";

/* --------------------------------------------------------------------------- */
/* Son las variables que se desea acceder.                           */
/* El interprete completa esta estructura, si la variable existe.    */
/* (usada en tiempo de ejecucion)                                    */
DLVARFIXUP __bgdexport( mod_m7, globals_fixup )[] =
{
    /* Nombre de variable global, puntero al dato, tamaño del elemento, cantidad de elementos */
    { "m7", NULL, -1, -1 },
    { NULL, NULL, -1, -1 }
};

/* --------------------------------------------------------------------------- */
/* Son las variables que se desea acceder.                           */
/* El interprete completa esta estructura, si la variable existe.    */
/* (usada en tiempo de ejecucion)                                    */
DLVARFIXUP __bgdexport( mod_m7, locals_fixup )[]  =
{
    /* Nombre de variable local, offset al dato, tamaño del elemento, cantidad de elementos */
    { "angle"                   , NULL, -1, -1 },
    { "cnumber"                 , NULL, -1, -1 },
    { "x"                       , NULL, -1, -1 },
    { "y"                       , NULL, -1, -1 },
    { "ctype"                   , NULL, -1, -1 },
    { "_m7_reserved.distance1"  , NULL, -1, -1 },
    { "size"                    , NULL, -1, -1 },
    { "height"                  , NULL, -1, -1 },
    { "reserved.status"         , NULL, -1, -1 },
    { "resolution"              , NULL, -1, -1 },
    { NULL                      , NULL, -1, -1 }
};

/* --------------------------------------------------------------------------- */
/* Internals Draw Object                                             */

static int compare_by_distance( const void * ptr1, const void * ptr2 )
{
    const INSTANCE * i1 = *( const INSTANCE ** )ptr1 ;
    const INSTANCE * i2 = *( const INSTANCE ** )ptr2 ;

    return LOCINT32( mod_m7, i2, DISTANCE_1 ) - LOCINT32( mod_m7, i1, DISTANCE_1 );
}

/* --------------------------------------------------------------------------- */

#define M7_8bpp() \
                baseline = ptr ; \
 \
                for ( x = 0 ; x < width ; x++ ) \
                { \
                    sx = fixtoi( bmp_x ) ; \
                    sy = fixtoi( bmp_y ) ; \
 \
                    if ( indoor && sx >= 0 && sx < indoor->width && sy >= 0 && sy < indoor->height ) \
                    { \
                        c = (( uint8_t* )indoor->data )[indoor->pitch*sy + sx] ; \
                        if ( c > 0 ) *ptr   = c ; \
                        ptr++ ; \
                    } \
                    else \
                    { \
                        if ( outdoor ) c = (( uint8_t* )outdoor->data )[outdoor->pitch*( sy & outdoor_vmask ) + ( sx & outdoor_hmask )] ; \
                        else           c = dat->color ; \
                        if ( c > 0 ) *ptr   = c ; \
                        ptr++ ; \
                    } \
 \
                    bmp_x += hinc ; \
                    bmp_y += vinc ; \
                } \
 \
                ptr = baseline + (( jump > 0 ) ? dest->pitch : -( int )dest->pitch ) ;


#define M7_16bpp() \
                baseline = ( uint8_t * ) ptr16 ; \
 \
                for ( x = 0 ; x < width ; x++ ) \
                { \
                    sx = fixtoi( bmp_x ) ; \
                    sy = fixtoi( bmp_y ) ; \
 \
                    if ( indoor && sx >= 0 && sx < indoor->width && sy >= 0 && sy < indoor->height ) \
                    { \
                        c16 = *( uint16_t* )&((( uint8_t* )indoor->data )[indoor->pitch*sy + (sx<<1)]) ; \
                        if ( c16 > 0 ) *ptr16 = c16 ; \
                        ptr16++ ; \
                    } \
                    else \
                    { \
                        if ( outdoor ) c16 = *( uint16_t* )&((( uint8_t* )outdoor->data )[outdoor->pitch*( sy & outdoor_vmask ) + (( sx & outdoor_hmask ) << 1)]) ; \
                        else           c16 = dat->color ; \
                        if ( c16 > 0 ) *ptr16 = c16 ; \
                        ptr16++ ; \
                    } \
 \
                    bmp_x += hinc ; \
                    bmp_y += vinc ; \
                } \
 \
                ptr16 = ( uint16_t * ) ( baseline + (( jump > 0 ) ? dest->pitch : -( int )dest->pitch ) );


#define M7_32bpp() \
                baseline = ( uint8_t * ) ptr32 ; \
 \
                for ( x = 0 ; x < width ; x++ ) \
                { \
                    sx = fixtoi( bmp_x ) ; \
                    sy = fixtoi( bmp_y ) ; \
 \
                    if ( indoor && sx >= 0 && sx < indoor->width && sy >= 0 && sy < indoor->height ) \
                    { \
                        c32 = *( uint32_t* )&((( uint8_t* )indoor->data )[indoor->pitch*sy + (sx<<2)]) ; \
                        if ( c32 > 0 ) *ptr32 = c32 ; \
                        ptr32++ ; \
                    } \
                    else \
                    { \
                        if ( outdoor ) c32 = *( uint32_t* )&((( uint8_t* )outdoor->data )[outdoor->pitch*( sy & outdoor_vmask ) + (( sx & outdoor_hmask ) << 2)]) ; \
                        else           c32 = dat->color ; \
                        if ( c32 > 0 ) *ptr32 = c32 ; \
                        ptr32++ ; \
                    } \
 \
                    bmp_x += hinc ; \
                    bmp_y += vinc ; \
                } \
 \
                ptr32 = ( uint32_t * ) ( baseline + (( jump > 0 ) ? dest->pitch : -( int )dest->pitch ) );


#define M7_t8bpp() \
                baseline = ptr ; \
 \
                for ( x = 0 ; x < width ; x++ ) \
                { \
                    sx = fixtoi( bmp_x ) ; \
                    sy = fixtoi( bmp_y ) ; \
 \
                    if ( indoor && sx >= 0 && sx < indoor->width && sy >= 0 && sy < indoor->height ) \
                    { \
                        *ptr = trans_table[(( uint8_t* )indoor->data )[indoor->pitch*sy + sx]][*ptr] ; \
                        ptr++ ; \
                    } \
                    else \
                    { \
                        if ( outdoor ) *ptr = trans_table[(( uint8_t* )outdoor->data )[outdoor->pitch*( sy & outdoor_vmask ) + ( sx & outdoor_hmask )]][*ptr] ; \
                        else           *ptr = trans_table[dat->color][*ptr] ; \
                        ptr++ ; \
                    } \
 \
                    bmp_x += hinc ; \
                    bmp_y += vinc ; \
                } \
 \
                ptr = baseline + (( jump > 0 ) ? dest->pitch : -( int )dest->pitch ) ;


#define M7_t16bpp() \
                baseline = ( uint8_t * ) ptr16 ; \
 \
                for ( x = 0 ; x < width ; x++ ) \
                { \
                    sx = fixtoi( bmp_x ) ; \
                    sy = fixtoi( bmp_y ) ; \
 \
                    if ( indoor && sx >= 0 && sx < indoor->width && sy >= 0 && sy < indoor->height ) \
                    { \
                        *ptr16 = colorghost[*ptr16]+colorghost[*( uint16_t* )&((( uint8_t* )indoor->data )[indoor->pitch*sy + (sx<<1)])]; \
                        ptr16++ ; \
                    } \
                    else \
                    { \
                        if ( outdoor ) *ptr16 = colorghost[*ptr16]+colorghost[*( uint16_t* )&((( uint8_t* )outdoor->data )[outdoor->pitch*( sy & outdoor_vmask ) + ((sx& outdoor_hmask)<<1)])]; \
                        else           *ptr16 = colorghost[*ptr16]+colorghost[dat->color]; \
                        ptr16++ ; \
                    } \
 \
                    bmp_x += hinc ; \
                    bmp_y += vinc ; \
                } \
 \
                ptr16 = ( uint16_t * ) ( baseline + (( jump > 0 ) ? dest->pitch : -( int )dest->pitch ) ); \


#define M7_t32bpp() \
                baseline = ( uint8_t * ) ptr32 ; \
 \
                for ( x = 0 ; x < width ; x++ ) \
                { \
                    sx = fixtoi( bmp_x ) ; \
                    sy = fixtoi( bmp_y ) ; \
 \
                    if ( indoor && sx >= 0 && sx < indoor->width && sy >= 0 && sy < indoor->height ) \
                    { \
                        c32 = *( uint32_t* )&((( uint8_t* )indoor->data )[indoor->pitch*sy + (sx<<2)]); \
                        _f = c32 & 0xff000000; \
                        if ( _f != 0xff000000 ) \
                        { \
                            _f = ( _f >> 24 ) * 128 / 255 ; \
                            _f2 = 255 - _f ; \
 \
                            r = ((( c32 & 0x00ff0000 ) * _f ) + (( *ptr32 & 0x00ff0000 ) * _f2 ) ) >> 8 ; \
                            g = ((( c32 & 0x0000ff00 ) * _f ) + (( *ptr32 & 0x0000ff00 ) * _f2 ) ) >> 8 ; \
                            b = ((( c32 & 0x000000ff ) * _f ) + (( *ptr32 & 0x000000ff ) * _f2 ) ) >> 8 ; \
                        } \
                        else \
                        { \
                            r = ((( c32 & 0x00ff0000 ) * 128 ) + (( *ptr32 & 0x00ff0000 ) * 128 ) ) >> 8 ; \
                            g = ((( c32 & 0x0000ff00 ) * 128 ) + (( *ptr32 & 0x0000ff00 ) * 128 ) ) >> 8 ; \
                            b = ((( c32 & 0x000000ff ) * 128 ) + (( *ptr32 & 0x000000ff ) * 128 ) ) >> 8 ; \
                        } \
 \
                        if ( r > 0x00ff0000 ) r = 0x00ff0000 ; else r &= 0x00ff0000 ; \
                        if ( g > 0x0000ff00 ) g = 0x0000ff00 ; else g &= 0x0000ff00 ; \
                        if ( b > 0x000000ff ) b = 0x000000ff ; else b &= 0x000000ff ; \
 \
                        *ptr32 = ( MAX( c32 & 0xff000000, *ptr32 & 0xff000000 ) ) | r | g | b ; \
                        ptr32++ ; \
                    } \
                    else \
                    { \
                        if ( outdoor ) \
                        { \
                            c32 = *( uint32_t* )&((( uint8_t* )outdoor->data )[outdoor->pitch*( sy & outdoor_vmask ) + (( sx & outdoor_hmask ) << 2)]) ; \
                            _f = c32 & 0xff000000; \
                            if ( _f != 0xff000000 ) \
                            { \
                                _f = ( _f >> 24 ) * 128 / 255 ; \
                                _f2 = 255 - _f ; \
 \
                                r = ((( c32 & 0x00ff0000 ) * _f ) + (( *ptr32 & 0x00ff0000 ) * _f2 ) ) >> 8 ; \
                                g = ((( c32 & 0x0000ff00 ) * _f ) + (( *ptr32 & 0x0000ff00 ) * _f2 ) ) >> 8 ; \
                                b = ((( c32 & 0x000000ff ) * _f ) + (( *ptr32 & 0x000000ff ) * _f2 ) ) >> 8 ; \
                            } \
                            else \
                            { \
                                r = ((( c32 & 0x00ff0000 ) * 128 ) + (( *ptr32 & 0x00ff0000 ) * 128 ) ) >> 8 ; \
                                g = ((( c32 & 0x0000ff00 ) * 128 ) + (( *ptr32 & 0x0000ff00 ) * 128 ) ) >> 8 ; \
                                b = ((( c32 & 0x000000ff ) * 128 ) + (( *ptr32 & 0x000000ff ) * 128 ) ) >> 8 ; \
                            } \
 \
                            if ( r > 0x00ff0000 ) r = 0x00ff0000 ; else r &= 0x00ff0000 ; \
                            if ( g > 0x0000ff00 ) g = 0x0000ff00 ; else g &= 0x0000ff00 ; \
                            if ( b > 0x000000ff ) b = 0x000000ff ; else b &= 0x000000ff ; \
 \
                            *ptr32 = ( MAX( c32 & 0xff000000, *ptr32 & 0xff000000 ) ) | r | g | b ; \
                        } \
                        else \
                        { \
                            c32 = dat->color ; \
                            _f = c32 & 0xff000000; \
                            if ( _f != 0xff000000 ) \
                            { \
                                _f = ( _f >> 24 ) * 128 / 255 ; \
                                _f2 = 255 - _f ; \
 \
                                r = ((( c32 & 0x00ff0000 ) * _f ) + (( *ptr32 & 0x00ff0000 ) * _f2 ) ) >> 8 ; \
                                g = ((( c32 & 0x0000ff00 ) * _f ) + (( *ptr32 & 0x0000ff00 ) * _f2 ) ) >> 8 ; \
                                b = ((( c32 & 0x000000ff ) * _f ) + (( *ptr32 & 0x000000ff ) * _f2 ) ) >> 8 ; \
                            } \
                            else \
                            { \
                                r = ((( c32 & 0x00ff0000 ) * 128 ) + (( *ptr32 & 0x00ff0000 ) * 128 ) ) >> 8 ; \
                                g = ((( c32 & 0x0000ff00 ) * 128 ) + (( *ptr32 & 0x0000ff00 ) * 128 ) ) >> 8 ; \
                                b = ((( c32 & 0x000000ff ) * 128 ) + (( *ptr32 & 0x000000ff ) * 128 ) ) >> 8 ; \
                            } \
 \
                            if ( r > 0x00ff0000 ) r = 0x00ff0000 ; else r &= 0x00ff0000 ; \
                            if ( g > 0x0000ff00 ) g = 0x0000ff00 ; else g &= 0x0000ff00 ; \
                            if ( b > 0x000000ff ) b = 0x000000ff ; else b &= 0x000000ff ; \
 \
                            *ptr32 = ( MAX( c32 & 0xff000000, *ptr32 & 0xff000000 ) ) | r | g | b ; \
                        } \
                        ptr32++ ; \
                    } \
 \
                    bmp_x += hinc ; \
                    bmp_y += vinc ; \
                } \
 \
                ptr32 = ( uint32_t * ) ( baseline + (( jump > 0 ) ? dest->pitch : -( int )dest->pitch ) ); \


static void draw_mode7( int n, REGION * clip )
{
    fixed   bmp_x, bmp_y ;
    fixed   base_x,   base_y,   base_z ;
    fixed   camera_x, camera_y, camera_z ;
    fixed   point_x,  point_y,  point_z ;
    fixed   cosa,  sina  ;
    fixed   angle ;
    fixed   hinc, vinc ;

    if ( n < 0 || n > 9 ) return ;

    MODE7 * mode7 = &mode7_inf[n];

    if ( !mode7->id ) return ;

    int horizon_y   = -1;
    int jump ;

    int outdoor_hmask = 0 ;
    int outdoor_vmask = 0 ;

    uint32_t  sx, sy ;
    int x, y, z, height, width ;
    uint8_t  * ptr  , * baseline  , c ;
    uint16_t * ptr16,               c16 ;
    uint32_t * ptr32,               c32 ;

    GRAPH * indoor  = mode7->indoor ;
    GRAPH * outdoor = mode7->outdoor ;
    GRAPH * dest ;
    GRAPH * pgr ;

    MODE7_INFO * dat  = &(( MODE7_INFO * ) & GLODWORD( mod_m7, M7STRUCTS ) )[n] ;

    INSTANCE   * camera ;

    static INSTANCE ** proclist = 0  ;
    static int proclist_reserved = 0 ;
    int proclist_count, nproc ;

    INSTANCE * i ;

    dest = mode7->dest ? mode7->dest : scrbitmap ;

    /* only if scr_depth == outdoor depth == indoor_depth */
    if ( ( outdoor && outdoor->format->depth != dest->format->depth ) ||
         (  indoor &&  indoor->format->depth != dest->format->depth ) ) return;

    /* Averigua la posición inicial de dibujo */

    camera = instance_get( dat->camera_id ) ;
    if ( !camera ) return ;

    angle = LOCINT32( mod_m7, camera, ANGLE ) ;

    cosa = fcos( -angle ) ;
    sina = fsin( -angle ) ;

    /* Averigua la posición de inicio */

    camera_x = itofix( LOCINT32( mod_m7, camera, COORDX ) ) ;
    camera_y = itofix( LOCINT32( mod_m7, camera, COORDY ) ) ;
    camera_z = itofix( dat->height ) ;

    RESOLXY( mod_m7, camera, camera_x, camera_y );

    camera_x -= cosa * dat->distance ;
    camera_y -= sina * dat->distance ;

    if ( dat->flags & B_VMIRROR ) camera_z = -camera_z ;

    /* Bucle para sacar las sub-posiciones de cada línea */

    width  = mode7->region->x2 - mode7->region->x + 1 ;
    height = mode7->region->y2 - mode7->region->y + 1 ;

    if ( !height || !width ) return ;

//    horizon_y = 0 ;

    for ( y = height ; y >= 0 ; y-- )
    {
        /* Representa en las 3D el punto (0, y) de pantalla */

        base_x = itofix( dat->focus /*FOCAL_DIST*/ ) ;
        base_y = -itofix( dat->focus / 2 ) ;
        base_z = itofix( dat->focus / 2 ) - itofix( y * dat->focus / height ) ;

        /* Rota dicho punto según el ángulo del proceso */

        point_x = ROTATEDX( base_x, base_y, sina, cosa ) + camera_x ;
        point_y = ROTATEDY( base_x, base_y, sina, cosa ) + camera_y ;
        point_z = base_z + camera_z ;

        /* Aplica la fórmula (ver mode7.txt) */

        if ( point_z == camera_z )
        {
            horizon_y = y ;
            continue ;
        }

        //if (point_z >= camera_z) break ;

        lines[y].left_bmp_x = fdiv( fmul(( point_x - camera_x ), -camera_z ), ( point_z - camera_z ) ) + camera_x ;
        lines[y].left_bmp_y = fdiv( fmul(( point_y - camera_y ), -camera_z ), ( point_z - camera_z ) ) + camera_y ;

        /* Lo mismo para el punto (width,y) */

        base_x = itofix( dat->focus /*FOCAL_DIST*/ ) ;
        base_y = itofix( dat->focus / 2 ) ;
        base_z = itofix( dat->focus / 2 ) - itofix( y * dat->focus / height ) ;

        point_x = ROTATEDX( base_x, base_y, sina, cosa ) + camera_x ;
        point_y = ROTATEDY( base_x, base_y, sina, cosa ) + camera_y ;
        point_z = base_z + camera_z ;

        //if (point_z >= camera_z) break ;

        lines[y].right_bmp_x = fdiv( fmul(( point_x - camera_x ), -camera_z ), ( point_z - camera_z ) ) + camera_x ;
        lines[y].right_bmp_y = fdiv( fmul(( point_y - camera_y ), -camera_z ), ( point_z - camera_z ) ) + camera_y ;

        /* Averigua el incremento necesario para cada paso de la línea */

        lines[y].hinc = ( lines[y].right_bmp_x - lines[y].left_bmp_x ) / width ;
        lines[y].vinc = ( lines[y].right_bmp_y - lines[y].left_bmp_y ) / width ;
    }

    /* Bucle principal */

    if ( horizon_y == -1 ) return ;

    y = horizon_y ;
    ptr32 = ( uint32_t * ) ( ptr16 = ( uint16_t * ) ( ptr = ( uint8_t * )dest->data + dest->pitch * y + mode7->region->x ) ) ;

    if ( outdoor )
    {
        outdoor_hmask = outdoor_vmask = 0xFFFFFFFF ;
        while ( ~( outdoor_hmask << 1 ) < ( int )outdoor->width - 1 ) outdoor_hmask <<= 1 ;
        while ( ~( outdoor_vmask << 1 ) < ( int )outdoor->height - 1 ) outdoor_vmask <<= 1 ;
        outdoor_hmask = ~outdoor_hmask ;
        outdoor_vmask = ~outdoor_vmask ;
    }

    jump = ( camera_z < 0 ) ? -1 : 1 ;
    ptr32 = ( uint32_t * ) ( ptr16 = ( uint16_t * ) ( ptr += (( jump > 0 ) ? dest->pitch : -( int )dest->pitch ) ) ) ;
    y   += jump ;

    if ( !( dat->flags & B_TRANSLUCENT ) )
    {
        if ( dat->flags & B_HMIRROR )
        {
            switch( dest->format->depth )
            {
                case    8:
                        for ( ; y < height && y >= 0 ; y += jump )
                        {
                            bmp_x = lines[y].right_bmp_x ;
                            bmp_y = lines[y].right_bmp_y ;
                            hinc  = -lines[y].hinc ;
                            vinc  = -lines[y].vinc ;

                            M7_8bpp();
                        }
                        break;

                case    16:
                        for ( ; y < height && y >= 0 ; y += jump )
                        {
                            bmp_x = lines[y].right_bmp_x ;
                            bmp_y = lines[y].right_bmp_y ;
                            hinc  = -lines[y].hinc ;
                            vinc  = -lines[y].vinc ;

                            M7_16bpp();
                        }
                        break;

                case    32:
                        for ( ; y < height && y >= 0 ; y += jump )
                        {
                            bmp_x = lines[y].right_bmp_x ;
                            bmp_y = lines[y].right_bmp_y ;
                            hinc  = -lines[y].hinc ;
                            vinc  = -lines[y].vinc ;

                            M7_32bpp();
                        }
                        break;
            }
        }
        else
        {
            switch( dest->format->depth )
            {
                case    8:
                        for ( ; y < height && y >= 0 ; y += jump )
                        {
                            bmp_x = lines[y].left_bmp_x ;
                            bmp_y = lines[y].left_bmp_y ;
                            hinc  = lines[y].hinc ;
                            vinc  = lines[y].vinc ;

                            M7_8bpp();
                        }
                        break;

                case    16:
                        for ( ; y < height && y >= 0 ; y += jump )
                        {
                            bmp_x = lines[y].left_bmp_x ;
                            bmp_y = lines[y].left_bmp_y ;
                            hinc  = lines[y].hinc ;
                            vinc  = lines[y].vinc ;

                            M7_16bpp();
                        }
                        break;

                case    32:
                        for ( ; y < height && y >= 0 ; y += jump )
                        {
                            bmp_x = lines[y].left_bmp_x ;
                            bmp_y = lines[y].left_bmp_y ;
                            hinc  = lines[y].hinc ;
                            vinc  = lines[y].vinc ;

                            M7_32bpp();
                        }
                        break;
            }
        }
    }
    else /* translucent */
    {
        if ( dat->flags & B_HMIRROR )
        {
            switch( dest->format->depth )
            {
                case    8:
                        for ( ; y < height && y >= 0 ; y += jump )
                        {
                            bmp_x = lines[y].right_bmp_x ;
                            bmp_y = lines[y].right_bmp_y ;
                            hinc  = -lines[y].hinc ;
                            vinc  = -lines[y].vinc ;

                            M7_t8bpp();
                        }
                        break;

                case    16:
                        for ( ; y < height && y >= 0 ; y += jump )
                        {
                            bmp_x = lines[y].right_bmp_x ;
                            bmp_y = lines[y].right_bmp_y ;
                            hinc  = -lines[y].hinc ;
                            vinc  = -lines[y].vinc ;

                            M7_t16bpp();
                        }
                        break;

                case    32:
                    {
                        unsigned int _f, _f2;
                        int r, g, b;

                        for ( ; y < height && y >= 0 ; y += jump )
                        {
                            bmp_x = lines[y].right_bmp_x ;
                            bmp_y = lines[y].right_bmp_y ;
                            hinc  = -lines[y].hinc ;
                            vinc  = -lines[y].vinc ;

                            M7_t32bpp();
                        }
                        break;
                    }
            }
        }
        else
        {
            switch( dest->format->depth )
            {
                case    8:
                        for ( ; y < height && y >= 0 ; y += jump )
                        {
                            bmp_x = lines[y].left_bmp_x ;
                            bmp_y = lines[y].left_bmp_y ;
                            hinc  = lines[y].hinc ;
                            vinc  = lines[y].vinc ;

                            M7_t8bpp();
                        }
                        break;

                case    16:
                        for ( ; y < height && y >= 0 ; y += jump )
                        {
                            bmp_x = lines[y].left_bmp_x ;
                            bmp_y = lines[y].left_bmp_y ;
                            hinc  = lines[y].hinc ;
                            vinc  = lines[y].vinc ;

                            M7_t16bpp();
                        }
                        break;

                case    32:
                    {
                        unsigned int _f, _f2;
                        int r, g, b;

                        for ( ; y < height && y >= 0 ; y += jump )
                        {
                            bmp_x = lines[y].left_bmp_x ;
                            bmp_y = lines[y].left_bmp_y ;
                            hinc  = lines[y].hinc ;
                            vinc  = lines[y].vinc ;

                            M7_t32bpp();
                        }
                        break;
                    }
            }
        }
    }

    /* Crea una lista ordenada de instancias a dibujar */

    i = first_instance ;
    proclist_count = 0 ;
    while ( i )
    {
        if (
            (
                ( LOCDWORD( mod_m7, i, STATUS ) & ~STATUS_WAITING_MASK ) == STATUS_RUNNING ||
                ( LOCDWORD( mod_m7, i, STATUS ) & ~STATUS_WAITING_MASK ) == STATUS_FROZEN
            )
            && LOCDWORD( mod_m7, i, CTYPE ) == C_M7 )
        {
            if ( LOCDWORD( mod_m7, i, CNUMBER ) && !( LOCDWORD( mod_m7, i, CNUMBER ) & ( 1 << n ) ) )
            {
                i = i->next ;
                continue ;
            }

            if ( proclist_count == proclist_reserved )
            {
                proclist_reserved += 16 ;
                proclist = ( INSTANCE ** ) realloc( proclist, sizeof( INSTANCE * ) * proclist_reserved ) ;
            }

            /* Averigua la distancia a la cámara */

            x = LOCINT32( mod_m7, i, COORDX ) ;
            y = LOCINT32( mod_m7, i, COORDY ) ;

            RESOLXY( mod_m7, i, x, y );

            x -= fixtoi( camera_x ) ;
            y -= fixtoi( camera_y ) ;

            LOCINT32( mod_m7, i, DISTANCE_1 ) = ftofix( sqrt(( double )(x * x) + ( double )(y * y) ) ) ;

            proclist[proclist_count++] = i ;
        }
        i = i->next ;
    }

    if ( proclist_count )
    {
        /* Ordena la listilla */

        qsort( proclist, proclist_count, sizeof( INSTANCE * ), compare_by_distance ) ;

        /* Visualiza los procesos */

        for ( nproc = 0 ; nproc < proclist_count ; nproc++ )
        {
            pgr = instance_graph( proclist[nproc] ) ;
            if ( !pgr ) continue ;

            if ( LOCINT32( mod_m7, proclist[nproc], DISTANCE_1 ) <= 0 ) continue ;

            base_x = itofix( LOCINT32( mod_m7, proclist[nproc], COORDX ) ) ;
            base_y = itofix( LOCINT32( mod_m7, proclist[nproc], COORDY ) ) ;
            base_z = itofix( LOCINT32( mod_m7, proclist[nproc], HEIGHT ) ) ;

            RESOLXYZ( mod_m7, proclist[nproc], base_x, base_y, base_z );

            base_x = /*itofix( */base_x /*)*/ - camera_x ;
            base_y = /*itofix( */base_y /*)*/ - camera_y ;
            base_z = /*itofix( */base_z /*)*/ + /*dat->*/height - camera_z  ;

            x = ROTATEDX( base_x, base_y, cosa, sina ) ;
            y = ROTATEDY( base_x, base_y, cosa, sina ) ;
            z = base_z ;

            if ( y <= 0 ) continue ;

            bmp_x = ( long )( -(( float )dat->focus /*FOCAL_DIST*/ * fixtof( x ) / fixtof( y ) ) * ( float )width / ( float ) dat->focus ) ;
            bmp_y = ( long )( -(( float )dat->focus /*FOCAL_DIST*/ * fixtof( z ) / fixtof( y ) ) * ( float )height / ( float ) dat->focus ) ;

            x = LOCINT32( mod_m7, proclist[nproc], GRAPHSIZE ) ;
            LOCINT32( mod_m7, proclist[nproc], GRAPHSIZE ) = dat->focus * 8 / fixtof( LOCDWORD( mod_m7, proclist[nproc], DISTANCE_1 ) ) ;
            draw_instance_at( proclist[nproc], mode7->region, mode7->region->x + width / 2  + bmp_x, mode7->region->y + height / 2 + bmp_y, mode7->dest ) ;
            LOCINT32( mod_m7, proclist[nproc], GRAPHSIZE ) = x ;
        }
    }
}

/* --------------------------------------------------------------------------- */

static int info_mode7( int n, REGION * clip, int * z, int * drawme )
{
    MODE7_INFO * dat   = &(( MODE7_INFO * ) & GLODWORD( mod_m7, M7STRUCTS ) )[n];

    * z = dat->z;
    * drawme = 1;
    * clip = * mode7_inf[n].region;

    // Force clean map (need optimization)
    if ( mode7_inf[n].dest /*&& !( mode7_inf[n].dest->info_flags & GI_CLEAN )*/ ) gr_clear_region( mode7_inf[n].dest, mode7_inf[n].region );

    return 1;
}

/* --------------------------------------------------------------------------- */
/* Mode 7                                                            */

static int __m7_start( int n, int fileid, int inid, int outid, int region, int horizon, int destfile, int destid )
{
    MODE7_INFO * dat ;

    if ( n < 0 || n > 9 ) return 0;

    dat = &(( MODE7_INFO * ) & GLODWORD( mod_m7, M7STRUCTS ) )[n];

    dat->height     = 32;
    dat->distance   = 64;
    dat->horizon    = horizon;
    dat->focus      = 256;
    dat->z          = 256;
    dat->color      = 0;
    dat->flags      = 0;

    mode7_inf[n].dest    = destid ? bitmap_get( destfile, destid ) : NULL ;
    mode7_inf[n].indoor  = inid ? bitmap_get( fileid, inid ) : NULL ;
    mode7_inf[n].outdoor = outid ? bitmap_get( fileid, outid ) : NULL ;
    mode7_inf[n].region  = region_get( region ) ;

    if ( mode7_inf[n].id ) gr_destroy_object( mode7_inf[n].id );
    mode7_inf[n].id = gr_new_object( dat->z, info_mode7, draw_mode7, n );

    return 1;
}

static int modm7_start( INSTANCE * my, int * params )
{
    return __m7_start( params[0], params[1], params[2], params[3], params[4], params[5], 0, 0 );
}

static int modm7_start2( INSTANCE * my, int * params )
{
    return __m7_start( params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7] );
}

/* --------------------------------------------------------------------------- */

static int modm7_stop( INSTANCE * my, int * params )
{
    int n = params[0];

    if ( n >= 0 && n <= 9 )
    {
        if ( mode7_inf[n].id )
        {
            gr_destroy_object( mode7_inf[n].id );
            mode7_inf[n].id = 0 ;
        }
    }

    return 1 ;
}

/* --------------------------------------------------------------------------- */

DLSYSFUNCS  __bgdexport( mod_m7, functions_exports )[] =
{
    { "MODE7_START" , "IIIIIIII"    , TYPE_INT , modm7_start2 },
    { "MODE7_START" , "IIIIII"      , TYPE_INT , modm7_start  },
    { "MODE7_STOP"  , "I"           , TYPE_INT , modm7_stop   },

    { "START_MODE7" , "IIIIIIII"    , TYPE_INT , modm7_start2 },
    { "START_MODE7" , "IIIIII"      , TYPE_INT , modm7_start  },
    { "STOP_MODE7"  , "I"           , TYPE_INT , modm7_stop   },

    { NULL          , NULL          , 0        , NULL         }
};

/* --------------------------------------------------------------------------- */

char * __bgdexport( mod_m7, modules_dependency )[] =
{
    "libgrbase",
    "libvideo",
    "librender",
    NULL
};

/* --------------------------------------------------------------------------- */
