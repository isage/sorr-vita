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
#include "libdraw.h"

/* --------------------------------------------------------------------------- */

/* Dibujo de primitivas */
#define DRAWOBJ_LINE        1
#define DRAWOBJ_RECT        2
#define DRAWOBJ_BOX         3
#define DRAWOBJ_CIRCLE      4
#define DRAWOBJ_FCIRCLE     5
#define DRAWOBJ_CURVE       6

typedef struct _drawing_object
{
    int type;
    int x1;
    int y1;
    int x2;
    int y2;
    int x3;
    int y3;
    int x4;
    int y4;
    int level;

    /* Private */
    int color8;
    int color16;
    int color32;
    int id;

    struct _drawing_object * prev;
    struct _drawing_object * next;
}
DRAWING_OBJECT;

/* --------------------------------------------------------------------------- */

static DRAWING_OBJECT * drawing_objects = NULL;

static GRAPH * drawing_graph = NULL;
static int drawing_z = -512 ;

/* --------------------------------------------------------------------------- */

/*
 *  FUNCTION : _moddraw_object_info
 *
 *  Internal function used to return information about a primitive object
 *
 *  PARAMS :
 *      dr          Drawing object
 *      bbox        Pointer to a REGION to be filled with the bounding box
 *
 *  RETURN VALUE :
 *      1 if the primitive changed since last frame
 *
 */

static int _moddraw_object_info( DRAWING_OBJECT * dr, REGION * clip, int * z, int * drawme )
{
    REGION newclip;
    int minx, miny, maxx, maxy;

    * drawme = 1;

    switch ( dr->type )
    {
        case DRAWOBJ_CIRCLE:
        case DRAWOBJ_FCIRCLE:
            newclip.x = dr->x1 - dr->x2;
            newclip.y = dr->y1 - dr->x2;
            newclip.x2 = dr->x1 + dr->x2;
            newclip.y2 = dr->y1 + dr->x2;
            break;

        case DRAWOBJ_CURVE:
            newclip.x = dr->x1;
            newclip.y = dr->y1;
            newclip.x2 = dr->x4;
            newclip.y2 = dr->y4;
            break;

        default:
            newclip.x = dr->x1;
            newclip.y = dr->y1;
            newclip.x2 = dr->x2;
            newclip.y2 = dr->y2;
            break;
    }

    minx = newclip.x;
    miny = newclip.y;
    maxx = newclip.x2;
    maxy = newclip.y2;

    if ( minx > maxx )
    {
        minx = newclip.x2;
        maxx = newclip.x;
    }

    if ( miny > maxy )
    {
        miny = newclip.y2;
        maxy = newclip.y;
    }

    newclip.x = minx;
    newclip.y = miny;
    newclip.x2 = maxx;
    newclip.y2 = maxy;

    if (
        newclip.x != clip->x || newclip.y != clip->y ||
        newclip.x2 != clip->x2 || newclip.y2 != clip->y2 )
    {
        * clip = newclip;
        return 0;
    }

    return 1;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : _moddraw_object_draw
 *
 *  Internal function used to draw a primitive object
 *
 *  PARAMS :
 *      dr          Drawing object
 *
 *  RETURN VALUE :
 *      None
 *
 */

static void _moddraw_object_draw( DRAWING_OBJECT * dr, REGION * clip )
{
    int b8 = pixel_color8;
    int b16 = pixel_color16;
    int b32 = pixel_color32;

    pixel_color8 = dr->color8;
    pixel_color16 = dr->color16;
    pixel_color32 = dr->color32;

    if ( pixel_alpha != 255 ) gr_setalpha( pixel_alpha );

    switch ( dr->type )
    {
        case DRAWOBJ_LINE:
            draw_line( scrbitmap, clip, dr->x1, dr->y1, dr->x2 - dr->x1, dr->y2 - dr->y1 );
            break;

        case DRAWOBJ_RECT:
            draw_rectangle( scrbitmap, clip, dr->x1, dr->y1, dr->x2 - dr->x1, dr->y2 - dr->y1 ) ;
            break;

        case DRAWOBJ_BOX:
            draw_box( scrbitmap, clip, dr->x1, dr->y1, dr->x2 - dr->x1, dr->y2 - dr->y1 ) ;
            break;

        case DRAWOBJ_CIRCLE:
            draw_circle( scrbitmap, clip, dr->x1, dr->y1, dr->x2 );
            break;

        case DRAWOBJ_FCIRCLE:
            draw_fcircle( scrbitmap, clip, dr->x1, dr->y1, dr->x2 );
            break;

        case DRAWOBJ_CURVE:
            draw_bezier( scrbitmap, clip, dr->x1, dr->y1, dr->x2, dr->y2,  dr->x3,  dr->y3,  dr->x4,  dr->y4,  dr->level );
            break;
    }

    pixel_color8 = b8;
    pixel_color16 = b16;
    pixel_color32 = b32;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : _moddraw_object_new
 *
 *  Create a new on-screen drawing object
 *
 *  PARAMS :
 *      drawing         Object type and coordinates
 *      z               Z Coordinate
 *
 *  RETURN VALUE :
 *      An integer identifier, representing the new object, or -1 if error
 *
 */

static int _moddraw_object_new( DRAWING_OBJECT * dr, int z )
{
    if ( !dr ) return -1;

    /* Fill the struct and register the new object */

    if ( drawing_objects ) drawing_objects->prev = dr ;

    dr->prev = NULL;
    dr->next = drawing_objects;
    dr->color8 = pixel_color8;
    dr->color16 = pixel_color16;
    dr->color32 = pixel_color32;

    dr->id = gr_new_object( z, _moddraw_object_info, _moddraw_object_draw, dr );

    drawing_objects = dr;

    return ( int ) dr;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : _moddraw_object_destroy
 *
 *  Destroy a new on-screen drawing object
 *
 *  PARAMS :
 *      id              Object id returned by _moddraw_object_new
 *                      or 0 to destroy every object
 *
 *  RETURN VALUE :
 *      None
 *
 */

static void _moddraw_object_destroy( int id )
{
    DRAWING_OBJECT * dr = ( DRAWING_OBJECT * ) id, * next;
    int destroyall = 0;

    if ( !dr )
    {
        dr = drawing_objects;
        destroyall = 1;
    }

    while ( dr )
    {
        next = dr->next;

        if ( dr->next ) dr->next->prev = dr->prev;
        if ( dr->prev ) dr->prev->next = dr->next;

        gr_destroy_object( dr->id );

        if ( drawing_objects == dr ) drawing_objects = dr->next;

        free( dr );

        if ( !destroyall ) break;

        dr = next;
    }
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : _moddraw_object_move
 *
 *  Move an on-screen drawing object to a new coordinates,
 *  relative to the first point in the primitive
 *
 *  PARAMS :
 *      id              Object id returned by _moddraw_object_new
 *      x, y            New coordinates
 *
 *  RETURN VALUE :
 *      None
 *
 */

static void _moddraw_object_move( int id, int x, int y )
{
    DRAWING_OBJECT * dr = ( DRAWING_OBJECT * ) id;

    if ( dr )
    {
        int incx = x - dr->x1;
        int incy = y - dr->y1;

        dr->x1 += incx;
        dr->y1 += incy;

        if ( dr->type == DRAWOBJ_CIRCLE || dr->type == DRAWOBJ_FCIRCLE ) return ;

        dr->x2 += incx;
        dr->y2 += incy;
        dr->x3 += incx;
        dr->y3 += incy;
        dr->x4 += incx;
        dr->y4 += incy;
    }
}

/* --------------------------------------------------------------------------- */
/* Exportable functions                                                        */

static int moddraw_drawing_map( INSTANCE * my, int * params )
{
    drawing_graph = bitmap_get( params[ 0 ], params[ 1 ] ) ;
    return 1 ;
}

/* --------------------------------------------------------------------------- */

static int moddraw_drawing_at( INSTANCE * my, int * params )
{
    drawing_graph = NULL;
    drawing_z = params[ 0 ];
    return 1 ;
}

/* --------------------------------------------------------------------------- */

static int moddraw_drawing_stipple( INSTANCE * my, int * params )
{
    drawing_stipple = params[ 0 ];
    return 1;
}

/* --------------------------------------------------------------------------- */

static int moddraw_delete_drawing( INSTANCE * my, int * params )
{
    _moddraw_object_destroy( params[ 0 ] );
    return 1;
}

/* --------------------------------------------------------------------------- */

static int moddraw_move_drawing( INSTANCE * my, int * params )
{
    _moddraw_object_move( params[ 0 ], params[ 1 ], params[ 2 ] );
    return 1;
}

/* --------------------------------------------------------------------------- */

static int moddraw_drawing_color( INSTANCE * my, int * params )
{
    gr_setcolor( params[ 0 ] );
    return 1 ;
}

/* --------------------------------------------------------------------------- */

static int moddraw_drawing_alpha( INSTANCE * my, int * params )
{
    gr_setalpha( params[ 0 ] );
    return 1;
}

/* --------------------------------------------------------------------------- */

static int moddraw_box( INSTANCE * my, int * params )
{
    if ( !drawing_graph )
    {
        DRAWING_OBJECT * dr = malloc( sizeof( DRAWING_OBJECT ) );

        dr->type = DRAWOBJ_BOX;
        dr->x1 = params[ 0 ];
        dr->y1 = params[ 1 ];
        dr->x2 = params[ 2 ];
        dr->y2 = params[ 3 ];
        return _moddraw_object_new( dr, drawing_z );
    }

    draw_box( drawing_graph, 0, params[ 0 ], params[ 1 ], params[ 2 ] - params[ 0 ], params[ 3 ] - params[ 1 ] ) ;
    return 1 ;
}

/* --------------------------------------------------------------------------- */

static int moddraw_rect( INSTANCE * my, int * params )
{
    if ( !drawing_graph )
    {
        DRAWING_OBJECT * dr = malloc( sizeof( DRAWING_OBJECT ) );

        dr->type = DRAWOBJ_RECT;
        dr->x1 = params[ 0 ];
        dr->y1 = params[ 1 ];
        dr->x2 = params[ 2 ];
        dr->y2 = params[ 3 ];
        return _moddraw_object_new( dr, drawing_z );
    }

    draw_rectangle( drawing_graph, 0, params[ 0 ], params[ 1 ], params[ 2 ] - params[ 0 ], params[ 3 ] - params[ 1 ] ) ;
    return 1 ;
}

/* --------------------------------------------------------------------------- */

static int moddraw_line( INSTANCE * my, int * params )
{
    if ( !drawing_graph )
    {
        DRAWING_OBJECT * dr = malloc( sizeof( DRAWING_OBJECT ) );

        dr->type = DRAWOBJ_LINE;
        dr->x1 = params[ 0 ];
        dr->y1 = params[ 1 ];
        dr->x2 = params[ 2 ];
        dr->y2 = params[ 3 ];
        return _moddraw_object_new( dr, drawing_z );
    }

    draw_line( drawing_graph, 0, params[ 0 ], params[ 1 ], params[ 2 ] - params[ 0 ], params[ 3 ] - params[ 1 ] );
    return 1 ;
}

/* --------------------------------------------------------------------------- */

static int moddraw_circle( INSTANCE * my, int * params )
{
    if ( !drawing_graph )
    {
        DRAWING_OBJECT * dr = malloc( sizeof( DRAWING_OBJECT ) );

        dr->type = DRAWOBJ_CIRCLE;
        dr->x1 = params[ 0 ];
        dr->y1 = params[ 1 ];
        dr->x2 = params[ 2 ];
        return _moddraw_object_new( dr, drawing_z );
    }

    draw_circle( drawing_graph, 0, params[ 0 ], params[ 1 ], params[ 2 ] ) ;
    return 1 ;
}

/* --------------------------------------------------------------------------- */

static int moddraw_fcircle( INSTANCE * my, int * params )
{
    if ( !drawing_graph )
    {
        DRAWING_OBJECT * dr = malloc( sizeof( DRAWING_OBJECT ) );

        dr->type = DRAWOBJ_FCIRCLE;
        dr->x1 = params[ 0 ];
        dr->y1 = params[ 1 ];
        dr->x2 = params[ 2 ];
        return _moddraw_object_new( dr, drawing_z );
    }

    draw_fcircle( drawing_graph, 0, params[ 0 ], params[ 1 ], params[ 2 ] ) ;
    return 1 ;
}

/* --------------------------------------------------------------------------- */

static int moddraw_bezier( INSTANCE * my, int * params )
{
    if ( !drawing_graph )
    {
        DRAWING_OBJECT * dr = malloc( sizeof( DRAWING_OBJECT ) );

        dr->type = DRAWOBJ_CURVE;
        dr->x1 = params[ 0 ];
        dr->y1 = params[ 1 ];
        dr->x2 = params[ 2 ];
        dr->y2 = params[ 3 ];
        dr->x3 = params[ 4 ];
        dr->y3 = params[ 5 ];
        dr->x4 = params[ 6 ];
        dr->y4 = params[ 7 ];
        dr->level = params[ 8 ];
        return _moddraw_object_new( dr, drawing_z );
    }

    draw_bezier( drawing_graph, 0, params[ 0 ], params[ 1 ], params[ 2 ], params[ 3 ], params[ 4 ], params[ 5 ], params[ 6 ], params[ 7 ], params[ 8 ] );
    return 1;
}

/* --------------------------------------------------------------------------- */

static int moddraw_get_pixel( INSTANCE * my, int * params )
{
    return gr_get_pixel( background, params[ 0 ], params[ 1 ] ) ;
}

/* --------------------------------------------------------------------------- */

static int moddraw_put_pixel( INSTANCE * my, int * params )
{
    gr_put_pixel( background, params[ 0 ], params[ 1 ], params[ 2 ] ) ;
    return 1 ;
}

/* --------------------------------------------------------------------------- */

static int moddraw_map_get_pixel( INSTANCE * my, int * params )
{
    GRAPH * map = bitmap_get( params[ 0 ], params[ 1 ] ) ;
    if ( !map ) return -1;
    return gr_get_pixel( map, params[ 2 ], params[ 3 ] ) ;
}

/* --------------------------------------------------------------------------- */

static int moddraw_map_put_pixel( INSTANCE * my, int * params )
{
    GRAPH * map = bitmap_get( params[ 0 ], params[ 1 ] ) ;
    if ( !map ) return 0 ;
    gr_put_pixel( map, params[ 2 ], params[ 3 ], params[ 4 ] ) ;
    return 1 ;
}

/* --------------------------------------------------------------------------- */
/* Declaracion de funciones                                                    */

DLSYSFUNCS __bgdexport( mod_draw, functions_exports )[] =
{
    /* Funciones de primitivas */
    { "DRAWING_MAP"     , "II"          , TYPE_INT  , moddraw_drawing_map       },
    { "DRAWING_COLOR"   , "I"           , TYPE_INT  , moddraw_drawing_color     },
    { "DRAW_LINE"       , "IIII"        , TYPE_INT  , moddraw_line              },
    { "DRAW_RECT"       , "IIII"        , TYPE_INT  , moddraw_rect              },
    { "DRAW_BOX"        , "IIII"        , TYPE_INT  , moddraw_box               },
    { "DRAW_CIRCLE"     , "III"         , TYPE_INT  , moddraw_circle            },
    { "DRAW_FCIRCLE"    , "III"         , TYPE_INT  , moddraw_fcircle           },
    { "DRAW_CURVE"      , "IIIIIIIII"   , TYPE_INT  , moddraw_bezier            },
    { "DRAWING_Z"       , "I"           , TYPE_INT  , moddraw_drawing_at        },
    { "DELETE_DRAW"     , "I"           , TYPE_INT  , moddraw_delete_drawing    },
    { "MOVE_DRAW"       , "III"         , TYPE_INT  , moddraw_move_drawing      },
    { "DRAWING_ALPHA"   , "I"           , TYPE_INT  , moddraw_drawing_alpha     },
    { "DRAWING_STIPPLE" , "I"           , TYPE_INT  , moddraw_drawing_stipple   },
    { "PUT_PIXEL"       , "III"         , TYPE_INT  , moddraw_put_pixel         },
    { "GET_PIXEL"       , "II"          , TYPE_INT  , moddraw_get_pixel         },
    { "MAP_GET_PIXEL"   , "IIII"        , TYPE_INT  , moddraw_map_get_pixel     },
    { "MAP_PUT_PIXEL"   , "IIIII"       , TYPE_INT  , moddraw_map_put_pixel     },
    { NULL              , NULL          , 0         , NULL                      }
};

/* --------------------------------------------------------------------------- */

char * __bgdexport( mod_draw, modules_dependency )[] =
{
    "libgrbase",
    "librender",
    "libdraw",
    NULL
};

/* --------------------------------------------------------------------------- */
