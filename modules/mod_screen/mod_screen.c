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

#include "bgdcore.h"
#include "bgdrtm.h"
#include "bgddl.h"
#include "dlvaracc.h"

#include "libgrbase.h"
#include "libvideo.h"
#include "librender.h"
#include "libblit.h"

#include "libscroll.h"

/* --------------------------------------------------------------------------- */

enum {
    CTYPE = 0,
    CNUMBER
};

enum {
    SCROLLS = 0
};

/* --------------------------------------------------------------------------- */

DLVARFIXUP __bgdexport( mod_screen, locals_fixup )[] =
{
    { "ctype"       , NULL, -1, -1 },
    { "cnumber"     , NULL, -1, -1 },

    { NULL          , NULL, -1, -1 }
};

/* --------------------------------------------------------------------------- */

DLVARFIXUP __bgdexport( mod_screen, globals_fixup )[] =
{
    { "scroll"      , NULL, -1, -1 },

    { NULL          , NULL, -1, -1 }
};

/* --------------------------------------------------------------------------- */

static int modscreen_define_region( INSTANCE * my, int * params )
{
    REGION * orig = region_get( params[0] );

    gr_mark_rect( MIN( orig->x, params[1] ), MIN( orig->y, params[2] ), MAX( orig->x2, params[1] + params[3] ), MAX( orig->y2, params[2] + params[4] ) );
    region_define( params[0], params[1], params[2], params[3], params[4] ) ;

    return params[0] ;
}

/* --------------------------------------------------------------------------- */

static int modscreen_out_region( INSTANCE * my, int * params )
{
    INSTANCE * proc = instance_get( params[0] ) ;
    int region = params[1] ;
    REGION bbox ;
    GRAPH * gr ;

    if ( region < 0 || region > 31 || !proc ) return 0 ;

    gr = instance_graph( proc ) ;
    if ( !gr ) return 0 ;

    instance_get_bbox( proc, gr, &bbox );

    if ( LOCDWORD( mod_screen, proc, CTYPE ) == C_SCROLL )
    {
        SCROLL_EXTRA_DATA * data;
        scrolldata  * scroll;
        int i;

        if ( GLOEXISTS( mod_screen, SCROLLS ) )
        {
            int cnumber = LOCDWORD( mod_screen, proc, CNUMBER );
            if ( !cnumber ) cnumber = 0xFFFFFFFF ;

            for ( i = 0 ; i < 10 ; i++ )
            {
                data = &(( SCROLL_EXTRA_DATA * ) & GLODWORD( mod_screen, SCROLLS ) )[i] ;
                scroll = ( scrolldata  * ) data->reserved[0];

                if ( scroll && scroll->active && ( cnumber & ( 1 << i ) ) )
                {
                    bbox.x  -= scroll->posx0 ;
                    bbox.y  -= scroll->posy0 ;
                    bbox.x2 -= scroll->posx0 ;
                    bbox.y2 -= scroll->posy0 ;
                    break;
                }
            }
        }
    }

    return region_is_out( &regions[region], &bbox ) ;
}

/* --------------------------------------------------------------------------- */
/* Funciones gráficas */

static int modscreen_put( INSTANCE * my, int * params )
{
    GRAPH * map = bitmap_get( params[0], params[1] ) ;

    if ( !map ) return 0 ;

    gr_blit( background, 0, params[2], params[3], 0, map ) ;

    return 1 ;
}

/* --------------------------------------------------------------------------- */

static int modscreen_xput( INSTANCE * my, int * params )
{
    int r ;
    GRAPH * map = bitmap_get( params[0], params[1] ) ;
    if ( !map ) return 0 ;

    r = params[7] ;
    if ( r < 0 || r > 31 ) return 0 ;

    if ( params[4] == 0 && params[5] == 100 )
        gr_blit( background, &regions[r], params[2], params[3], params[6], map ) ;
    else
        gr_rotated_blit( background, &regions[r], params[2], params[3], params[6], params[4], params[5], params[5], map ) ;

    return 1 ;
}

/* --------------------------------------------------------------------------- */

static int modscreen_put_screen( INSTANCE * my, int * params )
{
    int     x, y ;
    GRAPH * map = bitmap_get( params[0], params[1] ) ;

    if ( !map ) return 0;

    x = background->width / 2;
    y = background->height / 2;

    if ( map->ncpoints > 0 )
    {
        x = x -  map->width  / 2 +  map->cpoints[0].x ;
        y = y -  map->height / 2 +  map->cpoints[0].y ;
    }

    gr_clear( background ) ;
    gr_blit( background, 0, x, y, B_NOCOLORKEY, map ) ;

    return 1 ;
}

/* --------------------------------------------------------------------------- */

static int modscreen_clear_screen( INSTANCE * my, int * params )
{
    gr_clear( background ) ;
    return 1 ;
}

/* --------------------------------------------------------------------------- */

static int modscreen_get_screen( INSTANCE * my, int * params )
{
    GRAPH * map = bitmap_clone( bitmap_get( 0, -1 ) );

    map->info_flags = GI_NOCOLORKEY ;
    map->code = bitmap_next_code();

    bitmap_set_cpoint( map, 0, CPOINT_UNDEFINED, CPOINT_UNDEFINED );

    grlib_add_map( 0, map );

    return map->code ;
}

/* --------------------------------------------------------------------------- */

DLSYSFUNCS  __bgdexport( mod_screen, functions_exports )[] =
{
    /* Regiones */
    { "REGION_DEFINE"        , "IIIII"      , TYPE_INT      , modscreen_define_region   },
    { "REGION_OUT"           , "II"         , TYPE_INT      , modscreen_out_region      },

    /* Fondo de pantalla */
    { "PUT"                  , "IIII"       , TYPE_INT      , modscreen_put             },
    { "XPUT"                 , "IIIIIIII"   , TYPE_INT      , modscreen_xput            },
    { "SCREEN_PUT"           , "II"         , TYPE_INT      , modscreen_put_screen      },
    { "SCREEN_CLEAR"         , ""           , TYPE_INT      , modscreen_clear_screen    },

    /* Video */
    { "SCREEN_GET"           , ""           , TYPE_INT      , modscreen_get_screen      },

    /* ------------ Compatibility ------------ */

    /* Regiones */
    { "DEFINE_REGION"        , "IIIII"      , TYPE_INT      , modscreen_define_region   },
    { "OUT_REGION"           , "II"         , TYPE_INT      , modscreen_out_region      },

    /* Fondo de pantalla */
    { "PUT_SCREEN"           , "II"         , TYPE_INT      , modscreen_put_screen      },
    { "CLEAR_SCREEN"         , ""           , TYPE_INT      , modscreen_clear_screen    },

    /* Video */
    { "GET_SCREEN"           , ""           , TYPE_INT      , modscreen_get_screen      },

    { 0                     , 0             , 0             , 0                         }
};

/* --------------------------------------------------------------------------- */

char * __bgdexport( mod_screen, modules_dependency )[] =
{
    "libgrbase",
    "libvideo",
    "libblit",
    "librender",
    NULL
};

/* --------------------------------------------------------------------------- */
