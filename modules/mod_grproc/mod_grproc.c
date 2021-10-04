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

#include "bgdrtm.h"
#include "bgddl.h"
#include "dlvaracc.h"
#include "fmath.h"

#include "libgrbase.h"
#include "libvideo.h"
#include "librender.h"
#include "libblit.h"
#include "resolution.h"

#include "libscroll.h"

/* --------------------------------------------------------------------------- */

enum {
    COLLISION_NORMAL = 0,
    COLLISION_BOX,
    COLLISION_CIRCLE
};

/* --------------------------------------------------------------------------- */

/* Locals */

enum {
    GRPROC_TYPE_SCAN = 0,
    GRPROC_ID_SCAN,
    GRPROC_CONTEXT,
    PROCESS_ID,
    PROCESS_TYPE,
    STATUS,
    CTYPE,
    CNUMBER,
    COORDX,
    COORDY,
    ANGLE,
    GRAPHSIZE,
    GRAPHSIZEX,
    GRAPHSIZEY,
    FLAGS,
    REGIONID,
    RESOLUTION,
    XGRAPH
};

/* Globals */

enum {
    MOUSEX = 0,
    MOUSEY,
    SCROLLS
};

/* ----------------------------------------------------------------- */

char * __bgdexport( mod_grproc, locals_def ) =
    "STRUCT _mod_grproc_reserved\n"
    "int type_scan;\n"
    "int id_scan;\n"
    "int context;\n"
    "END\n";

/* ----------------------------------------------------------------- */

DLVARFIXUP __bgdexport( mod_grproc, locals_fixup )[]  =
{
    /* Nombre de variable local, offset al dato, tamaño del elemento, cantidad de elementos */
    { "_mod_grproc_reserved.type_scan"  , NULL, -1, -1 },
    { "_mod_grproc_reserved.id_scan"    , NULL, -1, -1 },
    { "_mod_grproc_reserved.context"    , NULL, -1, -1 },

    { "id"                              , NULL, -1, -1 },
    { "reserved.process_type"           , NULL, -1, -1 },
    { "reserved.status"                 , NULL, -1, -1 },

    { "ctype"                           , NULL, -1, -1 },
    { "cnumber"                         , NULL, -1, -1 },

    { "x"                               , NULL, -1, -1 },
    { "y"                               , NULL, -1, -1 },

    { "angle"                           , NULL, -1, -1 },
    { "size"                            , NULL, -1, -1 },
    { "size_x"                          , NULL, -1, -1 },
    { "size_y"                          , NULL, -1, -1 },
    { "flags"                           , NULL, -1, -1 },

    { "region"                          , NULL, -1, -1 },
    { "resolution"                      , NULL, -1, -1 },
    { "xgraph"                          , NULL, -1, -1 },

    { NULL                              , NULL, -1, -1 }
};

/* --------------------------------------------------------------------------- */

DLVARFIXUP __bgdexport( mod_grproc, globals_fixup )[] =
{
    /* Nombre de variable global, puntero al dato, tamaño del elemento, cantidad de elementos */

    { "mouse.x"                         , NULL, -1, -1 },
    { "mouse.y"                         , NULL, -1, -1 },
    { "scroll"                          , NULL, -1, -1 },

    { NULL                              , NULL, -1, -1 }
};

/* --------------------------------------------------------------------------- */
/* Funciones del sistema                                                       */
/* --------------------------------------------------------------------------- */

static int grproc_advance( INSTANCE * my, int * params )
{
    int angle = LOCINT32( mod_grproc, my, ANGLE ) ;
    LOCINT32( mod_grproc, my, COORDX ) += fixtoi( fmul( fcos( angle ), itofix( params[0] ) ) ) ;
    LOCINT32( mod_grproc, my, COORDY ) -= fixtoi( fmul( fsin( angle ), itofix( params[0] ) ) ) ;
    return 1 ;
}

/* --------------------------------------------------------------------------- */

static int grproc_xadvance( INSTANCE * my, int * params )
{
    int angle = params[0] ;
    LOCINT32( mod_grproc, my, COORDX ) += fixtoi( fmul( fcos( angle ), itofix( params[1] ) ) ) ;
    LOCINT32( mod_grproc, my, COORDY ) -= fixtoi( fmul( fsin( angle ), itofix( params[1] ) ) ) ;
    return 1 ;
}

/* --------------------------------------------------------------------------- */

static int grproc_get_angle( INSTANCE * my, int * params )
{
    INSTANCE * b = instance_get( params[0] ) ;

    if ( my && b )
    {
        double dx = ( double )(( int )( LOCINT32( mod_grproc, b, COORDX ) - LOCINT32( mod_grproc, my, COORDX ) ) ) ;
        double dy = ( double )(( int )( LOCINT32( mod_grproc, b, COORDY ) - LOCINT32( mod_grproc, my, COORDY ) ) ) ;
        int angle ;

        if ( dx == 0 ) return ( dy > 0 ) ? 270000L : 90000L ;

        angle = ( int )( atan( dy / dx ) * 180000.0 / M_PI ) ;

        return ( dx > 0 ) ? -angle : -angle + 180000L ;
    }
    return -1 ;
}

/* --------------------------------------------------------------------------- */

#define RESOLXY_RES(x,y,res) \
    {                                           \
        if ( res > 0 )                          \
        {                                       \
            if ( x < 0 )                        \
                ( x ) = ( ( x ) - ( res - 1 )) / res ; \
            else                                \
                ( x ) /= res ;                  \
                                                \
            if ( y < 0 )                        \
                ( y ) = ( ( y ) - ( res - 1 )) / res ; \
            else                                \
                ( y ) /= res ;                  \
        }                                       \
        else if ( res < 0 )                     \
        {                                       \
            ( x ) *= -res ;                     \
            ( y ) *= -res ;                     \
        }                                       \
    }

/* --------------------------------------------------------------------------- */

static int inline get_distance( int x1, int y1, int r1, int x2, int y2, int r2 )
{
    RESOLXY_RES( x1, y1, r1 );
    RESOLXY_RES( x2, y2, r2 );

    double dx = ( x2 - x1 ) * ( x2 - x1 ) ;
    double dy = ( y2 - y1 ) * ( y2 - y1 ) ;

    if ( r1 > 0 ) return ( int )sqrt( dx + dy ) * r1 ;
    else if ( r1 < 0 ) return ( int )sqrt( dx + dy ) / -r1 ;

    return ( int )sqrt( dx + dy ) ;
}

/* --------------------------------------------------------------------------- */

static int inline get_distance_xy( INSTANCE * a, int x2, int y2, int r2 )
{
    if ( a )
    {
        int x1, y1;
        int res = LOCINT32( mod_grproc, a, RESOLUTION ) ;

        x1 = LOCINT32( mod_grproc, a, COORDX ) ;
        y1 = LOCINT32( mod_grproc, a, COORDY ) ;

        RESOLXY( mod_grproc, a, x1, y1 );

        RESOLXY_RES( x2, y2, r2 );

        double dx = ( x2 - x1 ) * ( x2 - x1 ) ;
        double dy = ( y2 - y1 ) * ( y2 - y1 ) ;

        if ( res > 0 ) return ( int )sqrt( dx + dy ) * res ;
        else if ( res < 0 ) return ( int )sqrt( dx + dy ) / -res ;

        return ( int )sqrt( dx + dy ) ;
    }

    return -1 ;
}

/* --------------------------------------------------------------------------- */

static int get_distance_proc( INSTANCE * a, INSTANCE * b )
{
    if ( a && b ) return ( get_distance_xy( a, LOCINT32( mod_grproc, b, COORDX ), LOCINT32( mod_grproc, b, COORDY ), LOCINT32( mod_grproc, b, RESOLUTION ) ) ) ;
    return -1 ;
}

/* --------------------------------------------------------------------------- */

static int grproc_get_dist( INSTANCE * a, int * params )
{
    return ( get_distance_proc( a, instance_get( params[0] ) ) );
}

/* --------------------------------------------------------------------------- */

static int grproc_get_real_point( INSTANCE * my, int * params )
{
    GRAPH * b ;
    int x, y, r, centerx, centery, px = 0, py = 0, rx = 0, ry = 0 ;
    int _angle = 0, angle = 0;

    b = instance_graph( my ) ;
    if ( !b )  return 0 ;

    /* Point 0 is the graphic center, but it may be not defined */
    if ( params[0] == 0 && ( b->ncpoints == 0 || b->cpoints[0].x == CPOINT_UNDEFINED ) )
    {
        if ( b->ncpoints == 0 )
            bitmap_add_cpoint( b, b->width / 2, b->height / 2 );
        else
        {
            b->cpoints[0].x = b->width / 2;
            b->cpoints[0].y = b->height / 2;
        }
    }
    else
        if (( uint32_t )params[0] >= b->ncpoints || params[0] < 0)
            return 0 ;

    if ( b->cpoints[params[0]].x == CPOINT_UNDEFINED && b->cpoints[params[0]].y == CPOINT_UNDEFINED )
        return 0;

    r = LOCINT32( mod_grproc, my, REGIONID ) ;
    if ( r < 0 || r > 31 ) r = 0 ;

    if ( b->cpoints[0].x != CPOINT_UNDEFINED && b->cpoints[0].y != CPOINT_UNDEFINED )
    {
        centerx = b->cpoints[0].x ;
        centery = b->cpoints[0].y ;
    }
    else
    {
        centerx = b->width / 2 ;
        centery = b->height / 2 ;
    }


    if ( LOCINT32( mod_grproc, my, ANGLE ) != 0 && !LOCDWORD( mod_grproc, my, XGRAPH ) )
    {
        _angle = angle = LOCINT32( mod_grproc, my, ANGLE ) ;
    }

/* if ( b->cpoints[params[0]].x >= 0 )
    {
        if ( LOCDWORD( mod_grproc, my, FLAGS ) & B_HMIRROR )
            px = centerx - b->cpoints[params[0]].x - 1 ;
        else
            px = b->cpoints[params[0]].x - centerx ;

        if ( LOCDWORD( mod_grproc, my, FLAGS ) & B_VMIRROR )
            py = centery - b->cpoints[params[0]].y - 1 ;
        else
            py = b->cpoints[params[0]].y - centery ;
    }
    else
        px = py = 0 ; */

    if ( params[0] > 0 )
    {
        px = b->cpoints[params[0]].x - centerx ;
        py = b->cpoints[params[0]].y - centery ;

        if ( LOCDWORD( mod_grproc, my, FLAGS ) & B_HMIRROR )
        {
            if (_angle)
            {
                py = (centery - b->cpoints[params[0]].y) ;
                angle = 90000 + ( 90000 - angle );
            }
            else
                px = (centerx - b->cpoints[params[0]].x) ;
        }

        if ( LOCDWORD( mod_grproc, my, FLAGS ) & B_VMIRROR )
        {
            if (_angle)
            {
                px = (centerx - b->cpoints[params[0]].x) ;
                angle = 360000 + ( 180000 - angle );
            }
            else
                py = (centery - b->cpoints[params[0]].y) ;
        }
    }

    if ( LOCINT32( mod_grproc, my, GRAPHSIZEX ) == 100 && LOCINT32( mod_grproc, my, GRAPHSIZEY ) == 100 )
    {
        if ((( int )LOCINT32( mod_grproc, my, GRAPHSIZE ) ) > 0 )
        {
            // Corrected a bug from the casting that rounded to 0
            px = ( int )( px * ( LOCINT32( mod_grproc, my, GRAPHSIZE ) / 100.0F ) ) ;
            py = ( int )( py * ( LOCINT32( mod_grproc, my, GRAPHSIZE ) / 100.0F ) ) ;
        }
    }
    else
    {
        // Adding size_x/size_y control
        if ( LOCINT32( mod_grproc, my, GRAPHSIZEX ) > 0 )
            px = ( int )( px * ( LOCINT32( mod_grproc, my, GRAPHSIZEX ) / 100.0F ) ) ;

        if ( LOCINT32( mod_grproc, my, GRAPHSIZEY ) > 0 )
            py = ( int )( py * ( LOCINT32( mod_grproc, my, GRAPHSIZEY ) / 100.0F ) ) ;
    }

    if ( angle )
    {
        float cos_angle = ( float ) cos( angle * M_PI / -180000.0 );
        float sin_angle = ( float ) sin( angle * M_PI / -180000.0 );

        rx = ( int )(( double )px * cos_angle - ( double )py * sin_angle ) ;
        ry = ( int )(( double )px * sin_angle + ( double )py * cos_angle ) ;

        px = rx ;
        py = ry ;
    }

    x = LOCINT32( mod_grproc, my, COORDX ) ;
    y = LOCINT32( mod_grproc, my, COORDY ) ;

    RESOLXY( mod_grproc, my, x, y );

    rx = x + px ;
    ry = y + py ;

    if ( LOCINT32( mod_grproc, my, RESOLUTION ) > 0 )
    {
        rx *= LOCINT32( mod_grproc, my, RESOLUTION );
        ry *= LOCINT32( mod_grproc, my, RESOLUTION );
    }
    else if ( LOCINT32( mod_grproc, my, RESOLUTION ) < 0 )
    {
        rx /= -LOCINT32( mod_grproc, my, RESOLUTION );
        ry /= -LOCINT32( mod_grproc, my, RESOLUTION );
    }

    *( int * )params[1] = rx ;
    *( int * )params[2] = ry ;

    return 1 ;
}

/* --------------------------------------------------------------------------- */
/* Rutinas de utilidad local */

static void draw_at( GRAPH * dest, int x, int y, REGION * r, INSTANCE * i )
{
    GRAPH * map ;
    int scalex, scaley;

    scalex = LOCINT32( mod_grproc, i, GRAPHSIZEX );
    scaley = LOCINT32( mod_grproc, i, GRAPHSIZEY );
    if ( scalex == 100 && scaley == 100 ) scalex = scaley = LOCINT32( mod_grproc, i, GRAPHSIZE );

    map = instance_graph( i ) ;
    if ( !map ) return ;

    // PATCH - XGRAPH DOES NOT ROTATE DESTINATION GRAPHIC
    if ( LOCINT32( mod_grproc, i, ANGLE ) || scaley != 100 || scalex != 100 )
    {
        if ( LOCDWORD( mod_grproc, i, XGRAPH ) && scalex == 100 && scaley == 100 )
        {
            gr_blit( dest, r, x, y, LOCDWORD( mod_grproc, i, FLAGS ) & ( B_HMIRROR | B_VMIRROR ), map ) ;
        }
        else
        {
            if ( LOCDWORD( mod_grproc, i, XGRAPH ) )
            {
                gr_rotated_blit( dest, r, x, y, LOCDWORD( mod_grproc, i, FLAGS ) & ( B_HMIRROR | B_VMIRROR ), 0, scalex, scaley, map ) ;
            }
            else
            {
                gr_rotated_blit( dest, r, x, y, LOCDWORD( mod_grproc, i, FLAGS ) & ( B_HMIRROR | B_VMIRROR ), LOCINT32( mod_grproc, i, ANGLE ), scalex, scaley, map ) ;
            }
        }
    }
    else
    {
        gr_blit( dest, r, x, y, LOCDWORD( mod_grproc, i, FLAGS ) & ( B_HMIRROR | B_VMIRROR ), map ) ;
    }
}

/* --------------------------------------------------------------------------- */

static int get_bbox( REGION * bbox, INSTANCE * proc )
{
    GRAPH * b ;
    int     x, y ;
    int     scalex, scaley ;

    b = instance_graph( proc ) ;
    if ( !b ) return 0 ;

    scalex = LOCINT32( mod_grproc, proc, GRAPHSIZEX );
    scaley = LOCINT32( mod_grproc, proc, GRAPHSIZEY );
    if ( scalex == 100 && scaley == 100 ) scalex = scaley = LOCINT32( mod_grproc, proc, GRAPHSIZE );

    x = LOCINT32( mod_grproc, proc, COORDX ) ;
    y = LOCINT32( mod_grproc, proc, COORDY ) ;

    RESOLXY( mod_grproc, proc, x, y );

    gr_get_bbox( bbox, 0, x, y, LOCDWORD( mod_grproc, proc, FLAGS ) & ( B_HMIRROR | B_VMIRROR ), LOCINT32( mod_grproc, proc, ANGLE ), scalex, scaley, b ) ;

    return 1 ;
}

/* --------------------------------------------------------------------------- */
/* Colisiones */

static int check_collision_with_mouse( INSTANCE * proc1, int colltype )
{
    REGION bbox1, bbox2 ;
    int x, y, mx, my ;
    static GRAPH * bmp = NULL;

    switch ( colltype )
    {
        case    COLLISION_BOX:
        case    COLLISION_CIRCLE:
                if ( !get_bbox( &bbox2, proc1 ) ) return 0 ;

        case    COLLISION_NORMAL:
                break;

        default:
                return 0;
    }

    mx = GLOINT32( mod_grproc, MOUSEX ) ;
    my = GLOINT32( mod_grproc, MOUSEY ) ;

    /* Checks the process's bounding box to optimize checking
       (only for screen-type objects) */

    if ( LOCDWORD( mod_grproc, proc1, CTYPE ) == C_SCREEN )
    {
        switch ( colltype )
        {
            case    COLLISION_NORMAL:
                    if ( !get_bbox( &bbox2, proc1 ) ) return 0 ;
                    if ( bbox2.x > mx || bbox2.x2 < mx || bbox2.y > my || bbox2.y2 < my ) return 0 ;
                    break;

            case    COLLISION_BOX:
                    if ( bbox2.x <= mx && bbox2.x2 >= mx && bbox2.y <= my && bbox2.y2 >= my ) return 1;
                    return 0;
                    break;

            case    COLLISION_CIRCLE:
                {
                    int cx1, cy1, dx1, dy1;

                    cx1 = bbox2.x + ( dx1 = ( bbox2.x2 - bbox2.x + 1 ) ) / 2 ;
                    cy1 = bbox2.y + ( dy1 = ( bbox2.y2 - bbox2.y + 1 ) ) / 2 ;

                    if ( get_distance( cx1, cy1, 0, mx, my, 0 ) < ( dx1 + dy1 ) / 4 ) return 1;
                    return 0;
                    break;
                }
        }
    }

    /* Creates a temporary bitmap (only once) */

    if ( colltype == COLLISION_NORMAL )
    {
        /* maybe must force this to 32 bits */
        if ( bmp && bmp->format->depth != sys_pixel_format->depth ) { bitmap_destroy( bmp ); bmp = NULL; }
        if ( !bmp ) bmp = bitmap_new( 0, 2, 1, sys_pixel_format->depth ) ;
        if ( !bmp ) return 0 ;

        memset( bmp->data, 0, bmp->pitch * bmp->height ) ;
    }

    /* Retrieves process information */

    bbox1.x = 0 ; bbox1.x2 = 1 ;
    bbox1.y = 0 ; bbox1.y2 = 0 ;

    x = LOCINT32( mod_grproc, proc1, COORDX ) ;
    y = LOCINT32( mod_grproc, proc1, COORDY ) ;

    RESOLXY( mod_grproc, proc1, x, y );

    /* Scroll-type process: check for each region */

    if ( LOCDWORD( mod_grproc, proc1, CTYPE ) == C_SCROLL )
    {
        SCROLL_EXTRA_DATA * data;
        scrolldata  * scroll;
        int i;

        if ( GLOEXISTS( mod_grproc, SCROLLS ) )
        {
            int cnumber = LOCDWORD( mod_grproc, proc1, CNUMBER );
            if ( !cnumber ) cnumber = 0xffffffff ;

            for ( i = 0 ; i < 10 ; i++ )
            {
                data = &(( SCROLL_EXTRA_DATA * ) & GLODWORD( mod_grproc, SCROLLS ) )[i] ;
                scroll = ( scrolldata * ) data->reserved[0];

                if ( scroll && scroll->active && ( cnumber & ( 1 << i ) ) )
                {
                    REGION * r = scroll->region;

                    switch ( colltype )
                    {
                        case    COLLISION_NORMAL:
                                if ( r->x > mx || r->x2 < mx || r->y > my || r->y2 < my ) continue;

                                draw_at( bmp, x + r->x - mx - scroll->posx0, y + r->y - my - scroll->posy0, &bbox1, proc1 );
                                switch ( sys_pixel_format->depth )
                                {
                                    case    8:
                                        if ( *( uint8_t * )bmp->data ) return 1;
                                        break;

                                    case    16:
                                        if ( *( uint16_t * )bmp->data ) return 1;
                                        break;

                                    case    32:
                                        if ( *( uint32_t * )bmp->data ) return 1;
                                        break;
                                }
                                break;

                        case    COLLISION_BOX:
                                if ( bbox2.x <= scroll->posx0 + r->x + mx && bbox2.x2 >= scroll->posx0 + r->x + mx &&
                                     bbox2.y <= scroll->posy0 + r->y + my && bbox2.y2 >= scroll->posy0 + r->y + my ) return 1;
                                break;

                        case    COLLISION_CIRCLE:
                            {
                                int cx1, cy1, dx1, dy1;

                                cx1 = bbox2.x + ( dx1 = ( bbox2.x2 - bbox2.x + 1 ) ) / 2 ;
                                cy1 = bbox2.y + ( dy1 = ( bbox2.y2 - bbox2.y + 1 ) ) / 2 ;

                                if ( get_distance( cx1, cy1, 0, r->x + mx + scroll->posx0, r->y + my + scroll->posy0, 0 ) < ( dx1 + dy1 ) / 4 ) return 1;
                                break;
                            }
                    }
                }
            }
        }

        return 0;
    }

    switch ( colltype )
    {
        case    COLLISION_NORMAL:
                /* Collision check (blits into temporary space and checks the resulting pixel) */
                draw_at( bmp, x - mx, y - my, &bbox1, proc1 ) ;

                switch ( sys_pixel_format->depth )
                {
                    case    8:
                        if ( *( uint8_t * )bmp->data ) return 1;
                        break;

                    case    16:
                        if ( *( uint16_t * )bmp->data ) return 1;
                        break;

                    case    32:
                        if ( *( uint32_t * )bmp->data ) return 1;
                        break;
                }
                break;

        case    COLLISION_BOX:
                if ( bbox2.x <= mx && bbox2.x2 >= mx &&
                     bbox2.y <= my && bbox2.y2 >= my ) return 1;
                break;

        case    COLLISION_CIRCLE:
            {
                int cx1, cy1, dx1, dy1;

                cx1 = bbox2.x + ( dx1 = ( bbox2.x2 - bbox2.x + 1 ) ) / 2 ;
                cy1 = bbox2.y + ( dy1 = ( bbox2.y2 - bbox2.y + 1 ) ) / 2 ;

                if ( get_distance( cx1, cy1, 0, mx, my, 0 ) < ( dx1 + dy1 ) / 4 ) return 1;
                break;
            }
    }

    return 0 ;
}

/* --------------------------------------------------------------------------- */

static int check_collision_circle( INSTANCE * proc1, GRAPH * bmp1, REGION * bbox1, INSTANCE * proc2 )
{
    REGION bbox2 ;
    GRAPH * bmp2 ;
    int cx1, cy1, cx2, cy2, dx1, dy1, dx2, dy2;

    bmp2 = instance_graph( proc2 ) ; if ( !bmp2 ) return 0 ;
    instance_get_bbox( proc2, bmp2, &bbox2 );

    cx1 = bbox1->x + ( dx1 = ( bbox1->x2 - bbox1->x + 1 ) ) / 2 ;
    cy1 = bbox1->y + ( dy1 = ( bbox1->y2 - bbox1->y + 1 ) ) / 2 ;

    cx2 = bbox2.x + ( dx2 = ( bbox2.x2 - bbox2.x + 1 ) ) / 2 ;
    cy2 = bbox2.y + ( dy2 = ( bbox2.y2 - bbox2.y + 1 ) ) / 2 ;

    if ( get_distance( cx1, cy1, 0, cx2, cy2, 0 ) < ( ( dx1 + dy1 ) / 2 + ( dx2 + dy2 ) / 2 ) / 2 ) return 1;

    return 0;
}

/* --------------------------------------------------------------------------- */

static int check_collision_box( INSTANCE * proc1, GRAPH * bmp1, REGION * bbox1, INSTANCE * proc2 )
{
    REGION bbox2 ;
    GRAPH * bmp2 ;

    bmp2 = instance_graph( proc2 ) ; if ( !bmp2 ) return 0 ;
    instance_get_bbox( proc2, bmp2, &bbox2 );

    region_union( &bbox2, bbox1 ) ;
    if ( region_is_empty( &bbox2 ) ) return 0 ;

    return 1;
}

/* --------------------------------------------------------------------------- */

static int check_collision( INSTANCE * proc1, GRAPH * bmp1, REGION * bbox3, INSTANCE * proc2 )
{
    REGION bbox1, bbox2 ;
    int x, y, w, h ;
    GRAPH * bmp2 ;

    bbox1 = *bbox3;

    bmp2 = instance_graph( proc2 ) ; if ( !bmp2 ) return 0 ;
    instance_get_bbox( proc2, bmp2, &bbox2 );

    region_union( &bbox1, &bbox2 ) ;
    if ( region_is_empty( &bbox1 ) ) return 0 ;

    // Solo si las regiones de ambos bbox se superponen

    w = bbox1.x2 - bbox1.x + 1 ;
    h = bbox1.y2 - bbox1.y + 1 ;
    bbox2.x = bbox2.y = 0 ;
    bbox2.x2 = w - 1 ;
    bbox2.y2 = h - 1 ;

    bmp1 = bitmap_new( 0, w, h, sys_pixel_format->depth ) ;
    if ( !bmp1 ) return 0;

    bmp2 = bitmap_new( 0, w, h, sys_pixel_format->depth ) ;
    if ( !bmp2 )
    {
        bitmap_destroy( bmp1 ) ;
        return 0;
    }

    memset( bmp1->data, 0, bmp1->pitch * h ) ;
    memset( bmp2->data, 0, bmp2->pitch * h ) ;

    x = LOCINT32( mod_grproc, proc1, COORDX ) ;
    y = LOCINT32( mod_grproc, proc1, COORDY ) ;
    RESOLXY( mod_grproc, proc1, x, y );

    x -= bbox1.x ;
    y -= bbox1.y ;
    draw_at( bmp1, x, y, &bbox2, proc1 ) ;

    x = LOCINT32( mod_grproc, proc2, COORDX ) ;
    y = LOCINT32( mod_grproc, proc2, COORDY ) ;
    RESOLXY( mod_grproc, proc2, x, y );

    x -= bbox1.x ;
    y -= bbox1.y ;
    draw_at( bmp2, x, y, &bbox2, proc2 ) ;

    if ( sys_pixel_format->depth == 32 )
    {
        uint32_t * ptr1 = ( uint32_t * ) bmp1->data ;
        uint32_t * ptr2 = ( uint32_t * ) bmp2->data ;

        uint8_t * _ptr1 = ( uint8_t * ) ptr1 ;
        uint8_t * _ptr2 = ( uint8_t * ) ptr2 ;

        for ( y = 0 ; y < h ; y++ )
        {
            for ( x = 0 ; x < w ; x++, ptr1++, ptr2++ )
            {
                if ( *ptr1 && *ptr2 )
                {
                    bitmap_destroy( bmp1 ) ;
                    bitmap_destroy( bmp2 ) ;
                    return 1;
                }
            }
            ptr1 = ( uint32_t * )( _ptr1 += bmp1->pitch );
            ptr2 = ( uint32_t * )( _ptr2 += bmp2->pitch );
        }
    }
    else
    {
        if ( sys_pixel_format->depth == 16 )
        {
            uint16_t * ptr1 = ( uint16_t * ) bmp1->data ;
            uint16_t * ptr2 = ( uint16_t * ) bmp2->data ;

            uint8_t * _ptr1 = ( uint8_t * ) ptr1 ;
            uint8_t * _ptr2 = ( uint8_t * ) ptr2 ;

            for ( y = 0 ; y < h ; y++ )
            {
                for ( x = 0 ; x < w ; x++, ptr1++, ptr2++ )
                {
                    if ( *ptr1 && *ptr2 )
                    {
                        bitmap_destroy( bmp1 ) ;
                        bitmap_destroy( bmp2 ) ;
                        return 1;
                    }
                }
                ptr1 = ( uint16_t * )( _ptr1 += bmp1->pitch );
                ptr2 = ( uint16_t * )( _ptr2 += bmp2->pitch );
            }
        }
        else
        {
            uint8_t * ptr1 = ( uint8_t * )bmp1->data ;
            uint8_t * ptr2 = ( uint8_t * )bmp2->data ;

            uint8_t * _ptr1 = ptr1 ;
            uint8_t * _ptr2 = ptr2 ;

            for ( y = 0 ; y < h ; y++ )
            {
                for ( x = 0 ; x < w ; x++, ptr1++, ptr2++ )
                {
                    if ( *ptr1 && *ptr2 )
                    {
                        bitmap_destroy( bmp1 ) ;
                        bitmap_destroy( bmp2 ) ;
                        return 1;
                    }
                }
                ptr1 = _ptr1 += bmp1->pitch;
                ptr2 = _ptr2 += bmp2->pitch;
            }
        }
    }

    bitmap_destroy( bmp1 ) ;
    bitmap_destroy( bmp2 ) ;
    return 0;
}

/* --------------------------------------------------------------------------- */

static int __collision( INSTANCE * my, int id, int colltype )
{
    INSTANCE * ptr, ** ctx ;
    int status, p ;
    int ( *colfunc )( INSTANCE *, GRAPH *, REGION *, INSTANCE * );
    REGION bbox1 ;
    GRAPH * bmp1 ;

    if ( id == -1 ) return ( check_collision_with_mouse( my, colltype ) ) ? 1 : 0 ;

    switch ( colltype )
    {
        case    COLLISION_NORMAL:
                colfunc = check_collision;
                break;

        case    COLLISION_BOX:
                colfunc = check_collision_box;
                break;

        case    COLLISION_CIRCLE:
                colfunc = check_collision_circle;
                break;

        default:
                return 0;
    }

    bmp1 = instance_graph( my ) ; if ( !bmp1 ) return 0 ;
    instance_get_bbox( my, bmp1, &bbox1 );

    int ctype = LOCDWORD( mod_grproc, my, CTYPE );

    /* Checks only for a single instance */
    if ( id >= FIRST_INSTANCE_ID ) return ( ( ( ptr = instance_get( id ) ) && ctype == LOCDWORD( mod_grproc, ptr, CTYPE ) ) ? colfunc( my, bmp1, &bbox1, ptr ) : 0 ) ;

    /* we must use full list of instances or get types from it */
    ptr = first_instance ;

    if ( !id )
    {
        LOCDWORD( mod_grproc, my, GRPROC_TYPE_SCAN ) = 0 ;
        if ( ( p = LOCDWORD( mod_grproc, my, GRPROC_ID_SCAN ) ) )
        {
            ptr = instance_get( p ) ;
            if ( ptr ) ptr = ptr->next ;
        }

        while ( ptr )
        {
            if ( ptr != my &&
                 ctype == LOCDWORD( mod_grproc, ptr, CTYPE ) &&
                  (
                    ( status = ( LOCDWORD( mod_grproc, ptr, STATUS ) & ~STATUS_WAITING_MASK ) ) == STATUS_RUNNING ||
                    status == STATUS_FROZEN
                  ) && colfunc( my, bmp1, &bbox1, ptr )
                )
            {
                LOCDWORD( mod_grproc, my, GRPROC_ID_SCAN ) = LOCDWORD( mod_grproc, ptr, PROCESS_ID ) ;
                return LOCDWORD( mod_grproc, ptr, PROCESS_ID ) ;
            }
            ptr = ptr->next ;
        }
        return 0 ;
    }

    LOCDWORD( mod_grproc, my, GRPROC_ID_SCAN ) = 0 ;
    /* Check if already in scan by type and we reach limit */
    ctx = ( INSTANCE ** ) LOCADDR( mod_grproc, my, GRPROC_CONTEXT );
    if ( LOCDWORD( mod_grproc, my, GRPROC_TYPE_SCAN ) != id ) /* Check if type change from last call */
    {
        *ctx = NULL;
        LOCDWORD( mod_grproc, my, GRPROC_TYPE_SCAN ) = id;
    }

    while ( ( ptr = instance_get_by_type( id, ctx ) ) )
    {
        if ( ptr != my &&
             ctype == LOCDWORD( mod_grproc, ptr, CTYPE ) &&
             (
                ( status = ( LOCDWORD( mod_grproc, ptr, STATUS ) & ~STATUS_WAITING_MASK ) ) == STATUS_RUNNING ||
                  status == STATUS_FROZEN
             ) &&
             colfunc( my, bmp1, &bbox1, ptr )
           )
        {
            return LOCDWORD( mod_grproc, ptr, PROCESS_ID ) ;
        }
    }

    return 0 ;
}

/* --------------------------------------------------------------------------- */

static int grproc_collision( INSTANCE * my, int * params )
{
    return __collision( my, params[ 0 ], COLLISION_NORMAL );
}

/* --------------------------------------------------------------------------- */

static int grproc_collision_box( INSTANCE * my, int * params )
{
    return __collision( my, params[ 0 ], COLLISION_BOX );
}

/* ----------------------------------------------------------------- */

static int grproc_collision_circle( INSTANCE * my, int * params )
{
    return __collision( my, params[ 0 ], COLLISION_CIRCLE );
}

/* ----------------------------------------------------------------- */

void __bgdexport( mod_grproc, process_exec_hook )( INSTANCE * r )
{
    LOCDWORD( mod_grproc, r, GRPROC_ID_SCAN ) = 0;
    LOCDWORD( mod_grproc, r, GRPROC_TYPE_SCAN ) = 0;
    LOCDWORD( mod_grproc, r, GRPROC_CONTEXT ) = 0;
}

/* ---------------------------------------------------------------------- */

DLSYSFUNCS  __bgdexport( mod_grproc, functions_exports )[] =
{
    { "ADVANCE"             , "I"   , TYPE_INT  , grproc_advance            },
    { "XADVANCE"            , "II"  , TYPE_INT  , grproc_xadvance           },

    { "GET_ANGLE"           , "I"   , TYPE_INT  , grproc_get_angle          },
    { "GET_DIST"            , "I"   , TYPE_INT  , grproc_get_dist           },
    { "COLLISION"           , "I"   , TYPE_INT  , grproc_collision          },
    { "COLLISION_BOX"       , "I"   , TYPE_INT  , grproc_collision_box      },
    { "COLLISION_CIRCLE"    , "I"   , TYPE_INT  , grproc_collision_circle   },

    { "GET_REAL_POINT"      , "IPP" , TYPE_INT  , grproc_get_real_point     },

    { 0                     , 0     , 0         , 0                         }
};

/* --------------------------------------------------------------------------- */

char * __bgdexport( mod_grproc, modules_dependency )[] =
{
    "libmouse",
    "libgrbase",
    "libvideo",
    "librender",
    "libblit",
    NULL
};

/* --------------------------------------------------------------------------- */
