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
#include <string.h>

/* --------------------------------------------------------------------------- */

#ifdef __GNUC__
#define _inline inline
#endif

/* --------------------------------------------------------------------------- */

#define __LIB_RENDER
#include "librender.h"
#include "resolution.h"

#include "sysprocs_st.h"

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : instance_graph
 *
 *  Returns the instance graphic or NULL if there is none
 *
 *  PARAMS :
 *      i           Pointer to the instance
 *
 *  RETURN VALUE :
 *      Pointer to the graphic or NULL if none
 */

GRAPH * instance_graph( INSTANCE * i )
{
    int * xgraph, c, a ;

    if (( xgraph = ( int * ) LOCDWORD( librender, i, XGRAPH ) ) ) // Get offset of XGRAPH table
    {
        c = *xgraph++;  // Get number of graphs ids in XGRAPH table
        if ( c )
        {
            // Normalize ANGLE
            a = LOCINT32( librender, i, ANGLE ) % 360000 ;
            if ( a < 0 ) a += 360000 ;

            // Get graph id in XGRAPH table to draw
            c = xgraph[a * c / 360000] ;

            // If graph id value is negative, then graphic must be mirrored
            if ( c < 0 )
            {
                c = -c;
                LOCDWORD( librender, i, XGRAPH_FLAGS ) = B_HMIRROR;
            }
            else
            {
                LOCDWORD( librender, i, XGRAPH_FLAGS ) = 0;
            }

            // Get GRAPH * to draw
            return bitmap_get( LOCDWORD( librender, i, FILEID ), c ) ;
        }
    }

    // Get GRAPH * to draw
    if (( c = LOCDWORD( librender, i, GRAPHID ) ) )
    {
        return bitmap_get( LOCDWORD( librender, i, FILEID ), c ) ;
    }

    return 0 ; // No graph to draw
}

void instance_get_bbox( INSTANCE * i, GRAPH * gr, REGION * dest )
{
    REGION *region;
    int x, y, r ;
    int scalex, scaley ;

    r = LOCINT32( librender, i, REGIONID ) ;
    if ( r > 0 && r < 32 )
        region = &regions[ r ] ;
    else
        region = &regions[ 0 ];

    x = LOCINT32( librender, i, COORDX ) ;
    y = LOCINT32( librender, i, COORDY ) ;

    RESOLXY( librender, i, x, y );

    scalex = LOCINT32( librender, i, GRAPHSIZEX );
    scaley = LOCINT32( librender, i, GRAPHSIZEY );

    if ( scalex == 100 && scaley == 100 ) scalex = scaley = LOCINT32( librender, i, GRAPHSIZE );

    gr_get_bbox( dest,
            region,
            x,
            y,
            LOCDWORD( librender, i, FLAGS ) ^ LOCDWORD( librender, i, XGRAPH_FLAGS ),
            LOCDWORD( librender, i, XGRAPH ) ? 0 : LOCINT32( librender, i, ANGLE ),
            scalex,
            scaley,
            gr
               ) ;
}

/* ---------------------------------------------------------------------- */

void draw_instance_at( INSTANCE * i, REGION * region, int x, int y, GRAPH * dest )
{
    GRAPH * map ;
    int16_t * blend_table = NULL ;
    int flags ;
    int scalex, scaley ;
    int alpha ;
    int blendop;
    PALETTE * palette = NULL;
    int paletteid;

    map = instance_graph( i ) ;
    if ( !map ) return ;

    flags = ( LOCDWORD( librender, i, FLAGS ) ^ LOCDWORD( librender, i, XGRAPH_FLAGS ) );

    if (( alpha = LOCDWORD( librender, i, ALPHA ) ) != 255 )
    {
        if ( alpha <= 0 ) return ;
        else if ( alpha < 255 )
            flags |= B_ALPHA | ( alpha << B_ALPHA_SHIFT );
    }

    scalex = LOCINT32( librender, i, GRAPHSIZEX );
    scaley = LOCINT32( librender, i, GRAPHSIZEY );
    if ( scalex == 100 && scaley == 100 ) scalex = scaley = LOCINT32( librender, i, GRAPHSIZE );

    if (( blendop = LOCDWORD( librender, i, BLENDOP ) ) )
    {
        blend_table = map->blend_table;
        map->blend_table = ( int16_t * ) blendop;
    }

    if (( paletteid = LOCDWORD( librender, i, PALETTEID ) ) )
    {
        palette = map->format->palette ;
        map->format->palette = ( PALETTE * ) paletteid;
    }

    /* XGRAPH does not rotate destination graphic.
       WARNING: don't remove "scalex != 100 || scaley != 100 ||" from begin the next condition */
    if ( scalex != 100 || scaley != 100 || ( LOCINT32( librender, i, ANGLE ) && !LOCDWORD( librender, i, XGRAPH ) ) )
    {
        gr_rotated_blit( dest, region, x, y, flags, LOCDWORD( librender, i, XGRAPH ) ? 0 : LOCINT32( librender, i, ANGLE ), scalex, scaley, map ) ;
    }
    else
    {
        gr_blit( dest, region, x, y, flags, map ) ;
    }

    if ( paletteid ) map->format->palette = palette;
    if ( blendop ) map->blend_table = blend_table;

}

/* --------------------------------------------------------------------------- */
/* Rutinas gráficas de alto nivel */

void draw_instance( INSTANCE * i, REGION * clip )
{
    GRAPH * map ;
    int16_t * blend_table = NULL ;
    int flags;
    int scalex, scaley;
    REGION region;
    int alpha ;
    int blendop = 0 ;
    PALETTE * palette = NULL ;
    int paletteid;

    /* Difference with draw_instance_at from here */
    int x, y, r ;
    /* Difference with draw_instance_at to here */

//    map = instance_graph( i ) ;
    map = ( GRAPH * ) LOCDWORD( librender, i, GRAPHPTR );
    if ( !map ) return ;

    flags = ( LOCDWORD( librender, i, FLAGS ) ^ LOCDWORD( librender, i, XGRAPH_FLAGS ) );

    if (( alpha = LOCDWORD( librender, i, ALPHA ) ) != 255 )
    {
        if ( alpha <= 0 ) return ;
        else if ( alpha < 255 )
            flags |= B_ALPHA | ( alpha << B_ALPHA_SHIFT );
    }

    scalex = LOCINT32( librender, i, GRAPHSIZEX );
    scaley = LOCINT32( librender, i, GRAPHSIZEY );
    if ( scalex == 100 && scaley == 100 ) scalex = scaley = LOCINT32( librender, i, GRAPHSIZE );

    if (( blendop = LOCDWORD( librender, i, BLENDOP ) ) )
    {
        blend_table = map->blend_table;
        map->blend_table = ( int16_t * ) blendop;
    }

    if (( paletteid = LOCDWORD( librender, i, PALETTEID ) ) )
    {
        palette = map->format->palette ;
        map->format->palette = ( PALETTE * ) paletteid;
    }

    /* Difference with draw_instance_at from here */

    x = LOCINT32( librender, i, COORDX ) ;
    y = LOCINT32( librender, i, COORDY ) ;

    RESOLXY( librender, i, x, y );

    r = LOCINT32( librender, i, REGIONID ) ;
    if ( r > 0 && r < 32 )
        region = regions[ r ] ;
    else
        region = regions[ 0 ];

    if ( clip ) region_union( &region, clip );
    /* Difference with draw_instance_at to here */

    /* XGRAPH does not rotate destination graphic.
       WARNING: don't remove "scalex != 100 || scaley != 100 ||" from begin the next condition */
    if ( scalex != 100 || scaley != 100 || ( LOCINT32( librender, i, ANGLE ) && !LOCDWORD( librender, i, XGRAPH ) ) )
        gr_rotated_blit( 0, &region, x, y, flags, LOCDWORD( librender, i, XGRAPH ) ? 0 : LOCINT32( librender, i, ANGLE ), scalex, scaley, map ) ;
    else
        gr_blit( 0, &region, x, y, flags, map ) ;

    if ( paletteid ) map->format->palette = palette;
    if ( blendop ) map->blend_table = blend_table;

    if ( map->modified ) map->modified = 0;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : draw_instance_info
 *
 *  Compares the internal position variables of the instance with its
 *  currents values, and returns 1 if there is any difference. Used
 *  to detect changes in a visible process's aspect or position.
 *
 *  PARAMS :
 *      i           Pointer to the instance
 *
 *  RETURN VALUE :
 *      1 if there is any change, 0 otherwise
 */

int draw_instance_info( INSTANCE * i, REGION * region, int * z, int * drawme )
{
    GRAPH * graph;

    * drawme = 0;

    LOCDWORD( librender, i, GRAPHPTR ) = ( int )( graph = instance_graph( i ) );
    if ( !graph )
    {
        /*
                region->x  = -2; region->y  = -2;
                region->x2 = -2; region->y2 = -2;
        */
        return 0;
    }

    int changed;
    int status;
    int coordz, coordx, coordy;

    status = ( LOCDWORD( librender, i, STATUS ) & ~STATUS_WAITING_MASK ) ;

    coordz = LOCINT32( librender, i, COORDZ );

    /* Si tiene grafico o xgraph o (ctype == 0 y esta corriendo o congelado) */

    if ( LOCDWORD( librender, i, CTYPE ) == C_SCREEN && ( status == STATUS_RUNNING || status == STATUS_FROZEN ) )
        * drawme = 1;


    coordx = LOCINT32( librender, i, COORDX );
    coordy = LOCINT32( librender, i, COORDY );

    RESOLXY( librender, i, coordx, coordy );

    changed =
        graph->modified                                                                           ||
        LOCINT32( librender, i, SAVED_COORDX )        != coordx                                   ||
        LOCINT32( librender, i, SAVED_COORDY )        != coordy                                   ||
        LOCINT32( librender, i, SAVED_COORDZ )        != coordz                                   ||
        LOCDWORD( librender, i, SAVED_GRAPHID )       != LOCDWORD( librender, i, GRAPHID )        ||
        LOCINT32( librender, i, SAVED_ANGLE )         != LOCINT32( librender, i, ANGLE )          ||
        LOCDWORD( librender, i, SAVED_ALPHA )         != LOCDWORD( librender, i, ALPHA )          ||
        LOCDWORD( librender, i, SAVED_BLENDOP )       != LOCDWORD( librender, i, BLENDOP )        ||
        LOCINT32( librender, i, SAVED_GRAPHSIZE )     != LOCINT32( librender, i, GRAPHSIZE )      ||
        LOCINT32( librender, i, SAVED_GRAPHSIZEX )    != LOCINT32( librender, i, GRAPHSIZEX )     ||
        LOCINT32( librender, i, SAVED_GRAPHSIZEY )    != LOCINT32( librender, i, GRAPHSIZEY )     ||
        LOCDWORD( librender, i, SAVED_FLAGS )         != LOCDWORD( librender, i, FLAGS )          ||
        LOCDWORD( librender, i, SAVED_FILEID )        != LOCDWORD( librender, i, FILEID )         ||
        LOCDWORD( librender, i, SAVED_XGRAPH )        != LOCDWORD( librender, i, XGRAPH )         ||
        (
            graph->ncpoints &&
            (
                LOCDWORD( librender, i, SAVED_CENTERX )       != graph->cpoints[0].x              ||
                LOCDWORD( librender, i, SAVED_CENTERY )       != graph->cpoints[0].y
            )
        )
        ;

    if ( changed )
    {
        /* Update key */

        * z = coordz;

        LOCINT32( librender, i, SAVED_COORDX )       = coordx;
        LOCINT32( librender, i, SAVED_COORDY )       = coordy;
        LOCINT32( librender, i, SAVED_COORDZ )       = coordz;
        LOCDWORD( librender, i, SAVED_GRAPHID )      = LOCDWORD( librender, i, GRAPHID );
        LOCINT32( librender, i, SAVED_ANGLE )        = LOCINT32( librender, i, ANGLE );
        LOCDWORD( librender, i, SAVED_ALPHA )        = LOCDWORD( librender, i, ALPHA );
        LOCDWORD( librender, i, SAVED_BLENDOP )      = LOCDWORD( librender, i, BLENDOP );
        LOCINT32( librender, i, SAVED_GRAPHSIZE )    = LOCINT32( librender, i, GRAPHSIZE );
        LOCINT32( librender, i, SAVED_GRAPHSIZEX )   = LOCINT32( librender, i, GRAPHSIZEX );
        LOCINT32( librender, i, SAVED_GRAPHSIZEY )   = LOCINT32( librender, i, GRAPHSIZEY );
        LOCDWORD( librender, i, SAVED_FLAGS )        = LOCDWORD( librender, i, FLAGS );
        LOCDWORD( librender, i, SAVED_FILEID )       = LOCDWORD( librender, i, FILEID );
        LOCDWORD( librender, i, SAVED_XGRAPH )       = LOCDWORD( librender, i, XGRAPH );
        if ( graph->ncpoints )
        {
            LOCDWORD( librender, i, SAVED_CENTERX )      = graph->cpoints[0].x;
            LOCDWORD( librender, i, SAVED_CENTERY )      = graph->cpoints[0].y;
        }
        else
        {
            LOCDWORD( librender, i, SAVED_CENTERX )      = CPOINT_UNDEFINED;
            LOCDWORD( librender, i, SAVED_CENTERY )      = CPOINT_UNDEFINED;
        }

        instance_get_bbox( i, graph, region );
        return 1;
    }

    return changed;
}


/* ----------------------------------------------------------------------
 Dlls Hooks
   ---------------------------------------------------------------------- */

/*
 *  FUNCTION : instance_create_hook
 *
 *  PARAMS :
 *      r           Pointer to the instance
 *
 *  RETURN VALUE :
 *      None
 */

void __bgdexport( librender, instance_create_hook )( INSTANCE * r )
{
    /* COORZ is 0 when a new instance is created */
    LOCDWORD( librender, r, OBJECTID ) = gr_new_object( /* LOCINT32( librender, r, COORDZ ) */ 0, draw_instance_info, draw_instance, r );
}

/*
 *  FUNCTION : instance_destroy_hook
 *
 *  PARAMS :
 *      r           Pointer to the instance
 *
 *  RETURN VALUE :
 *      None
 */

void __bgdexport( librender, instance_destroy_hook )( INSTANCE * r )
{
    if ( LOCDWORD( librender, r, OBJECTID ) ) gr_destroy_object( LOCDWORD( librender, r, OBJECTID ) );
}
