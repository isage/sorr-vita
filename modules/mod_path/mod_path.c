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

#include <stdio.h>
#include <stdlib.h>

#include "bgddl.h"
#include "bgdrtm.h"

#include "libgrbase.h"

/* --------------------------------------------------------------------------- */

typedef struct _node
{
    unsigned int x, y ;
    double f, g, h ;
    struct _node * parent ;
    struct _node * next ;
    struct _node * inext ;
}
node ;

/* --------------------------------------------------------------------------- */

static int  * path_result = NULL ;
static int  * path_result_pointer = NULL ;

static node * pf_all = NULL;

static node * pf_open = NULL ;
static node * pf_closed = NULL ;
static node * found = NULL ;

static int destination_x, destination_y ;
static int startup_x, startup_y ;

static GRAPH * map ;

static int block_if = 1 ;

/* --------------------------------------------------------------------------- */

#define PF_NODIAG       1
#define PF_REVERSE      2

/* --------------------------------------------------------------------------- */

DLCONSTANT __bgdexport( mod_path, constants_def )[] =
{
    { "PF_NODIAG"   , TYPE_INT, PF_NODIAG   }, /* Prohibit the pathfinding from using diagonal paths. */
    { "PF_REVERSE"  , TYPE_INT, PF_REVERSE  }, /* Return the path found in reverse order.             */

    { NULL          , 0       , 0           }
} ;

/* --------------------------------------------------------------------------- */

static double heuristic( int x, int y )
{
    int dx, dy ;
    uint8_t block = (( uint8_t* )map->data )[map->pitch*y+x] ;

    if ( x == destination_x && y == destination_y ) return 0 ;
    if ( block >= block_if ) return 1073741824.0 ;
    if ( x < 0 || y < 0 || x >= ( int )map->width || y >= ( int )map->height ) return 1073741824.0 ;

    dx = abs( destination_x - x ) ;
    dy = abs( destination_y - y ) ;
    return ( double )block + ( double )dx*dx + ( double )dy*dy ;
}

/* --------------------------------------------------------------------------- */

/* Uso: pf_open = add(pf_open, this) ; */
/* La lista permanecerá ordenada */
static node * node_add( node * list, node * this )
{
    node * curr = list ;
    if ( !curr )
    {
        this->next = NULL ;
        return this ;
    }
    if ( curr->f > this->f )
    {
        this->next = curr ;
        return this ;
    }

    for ( ;; )
    {
        if ( !curr->next )
        {
            curr->next = this ;
            this->next = NULL ;
            return list ;
        }
        if ( curr->next->f > this->f )
        {
            this->next = curr->next ;
            curr->next = this ;
            return list ;
        }
        curr = curr->next ;
    }
}

/* --------------------------------------------------------------------------- */

/* Uso: pf_open = remove(pf_open, this) ; */
static node * node_remove( node * list, node * this )
{
    node * curr = list ;
    if ( curr == this ) return this->next ;
    while ( curr )
    {
        if ( curr->next == this )
        {
            curr->next = this->next ;
            return list ;
        }
        curr = curr->next ;
    }
    return list ;
}

/* --------------------------------------------------------------------------- */

static node * node_find( node * list, int x, int y )
{
    while ( list )
    {
        if ( list->x == ( unsigned )x && list->y == ( unsigned )y ) return list ;
        list = list->next ;
    }
    return NULL ;
}

/* --------------------------------------------------------------------------- */
/*
static void node_reset( node * list )
{
    node * next ;
    while ( list )
    {
        next = list->next ;
        free( list ) ;
        list = next ;
    }
}
*/
/* --------------------------------------------------------------------------- */

static node * node_new( node * parent, int x, int y, int cost_inc )
{
    node * curr ;

    curr = ( node * ) malloc( sizeof( node ) ) ;
    if ( !curr ) return NULL ;

    curr->x = x ;
    curr->y = y ;
    curr->g = ( parent ? parent->g : 0 ) + cost_inc ;
    curr->h = heuristic( x, y ) ;
    curr->f = curr->g + curr->h ;

    curr->parent = parent ;
    curr->next   = NULL ;
    return curr ;
}

/* --------------------------------------------------------------------------- */

static void node_push_succesor( node * parent, int ix, int iy, int cost )
{
    node * curr, * f_op, * f_cl ;

    curr = node_new( parent, parent->x + ix, parent->y + iy, cost ) ;
    if ( curr->h > 131072 )
    {
        free( curr ); return ;
    }

    f_cl = node_find( pf_closed, curr->x, curr->y ) ;
    if ( f_cl )
    {
        free( curr ); return ;
    }

    f_op = node_find( pf_open, curr->x, curr->y ) ;
    if ( f_op && f_op->f <= curr->f )
    {
        free( curr ); return ;
    }

    /* Add to general list (used for free resources)*/
    curr->inext = pf_all; pf_all = curr;

    if ( f_op ) { pf_open = node_remove( pf_open, f_op ); } /* this node is removed but childs that referent this node as parent will be wrong */
/* this can't be possible, previous "if ( f_cl )" abort this code ->   if ( f_cl ) { pf_closed = node_remove( pf_closed, f_cl ); }*/ /* this node is removed but childs that referent this node as parent will be wrong */

    pf_open = node_add( pf_open, curr ) ;
}

/* --------------------------------------------------------------------------- */

static void node_push_succesors( node * parent, int options )
{
    node * prior = parent->parent ;
    if ( !prior ) prior = parent ;

    node_push_succesor( parent, 1, 0, prior->x < parent->x ? 9 : 10 ) ;
    node_push_succesor( parent, 0, 1, prior->x > parent->x ? 9 : 10 ) ;
    node_push_succesor( parent, -1, 0, prior->y < parent->y ? 9 : 10 ) ;
    node_push_succesor( parent, 0, -1, prior->y > parent->y ? 9 : 10 ) ;

    if ( !( options & PF_NODIAG ) )
    {
        node_push_succesor( parent, 1, 1, 12 ) ;
        node_push_succesor( parent, -1, -1, 12 ) ;
        node_push_succesor( parent, -1, 1, 12 ) ;
        node_push_succesor( parent, 1, -1, 12 ) ;
    }
}

/* --------------------------------------------------------------------------- */

static int path_find( GRAPH * bitmap, int sx, int sy, int dx, int dy, int options )
{
    node * curr, * inext ;

    startup_x = sx ;
    startup_y = sy ;
    map = bitmap ;
    destination_x = dx ;
    destination_y = dy ;
/*
    node_reset( pf_open ) ;
    node_reset( pf_closed ) ;
*/
    /* Release all resources */

    curr = pf_all;
    while ( curr )
    {
        inext = curr->inext ;
        free( curr ) ;
        curr = inext ;
    }
    pf_all = NULL;

    pf_open = NULL;
    pf_closed = NULL;

    if ( path_result ) { free ( path_result ); path_result = NULL; }
    path_result_pointer = NULL;

    curr = node_new( NULL, startup_x, startup_y, 0 ) ;

    /* Add to general list (used for free resources)*/
    curr->inext = pf_all; pf_all = curr;

    curr->f = curr->h = 1 ;
    pf_open = node_add( pf_open, curr ) ;

    while ( pf_open )
    {
        curr = pf_open ;
        pf_open = node_remove( pf_open, curr ) ;

        if ( curr->x == ( unsigned )destination_x && curr->y == ( unsigned )destination_y )
        {
            int count = 1 ;

            found = curr ;
            while ( curr->parent )
            {
                count++ ;
                curr = curr->parent ;
            }

            path_result = malloc( sizeof( int ) * 2 * ( count + 4 ) ) ;
            if ( !( options & PF_REVERSE ) )
            {
                path_result_pointer = path_result + count * 2 + 1;
                *path_result_pointer-- = -1 ;
                *path_result_pointer-- = -1 ;
                while ( found )
                {
                    *path_result_pointer-- = found->y ;
                    *path_result_pointer-- = found->x ;
                    found = found->parent ;
                }

                if ( path_result_pointer != path_result - 1 )
                {
                    path_result_pointer = NULL;
                    return 0;
                }
            }
            else
            {
                path_result_pointer = path_result ;
                while ( found )
                {
                    *path_result_pointer++ = found->x ;
                    *path_result_pointer++ = found->y ;
                    found = found->parent ;
                }
                *path_result_pointer++ = -1 ;
                *path_result_pointer++ = -1 ;
            }

            path_result_pointer = path_result ;
            return 1 ;
        }

        node_push_succesors( curr, options ) ;

        pf_closed = node_add( pf_closed, curr ) ;
    }

    return 0 ;
}

/* --------------------------------------------------------------------------- */

static int path_get( int * x, int * y )
{
    if ( path_result_pointer )
    {
        ( *x ) = *path_result_pointer++ ;
        ( *y ) = *path_result_pointer++ ;
        if ( *path_result_pointer == -1 ) path_result_pointer = NULL ;
        return 1 ;
    }
    return 0 ;
}

/* --------------------------------------------------------------------------- */

static int path_set_wall( int n )
{
    if ( n >= 1 ) block_if = n ;
    return block_if ;
}

/* --------------------------------------------------------------------------- */
/* Funciones de búsqueda de caminos */

static int modpathfind_path_find( INSTANCE * my, int * params )
{
    GRAPH * gpath = bitmap_get( params[0], params[1] ) ;
    if ( !gpath || !gpath->format || gpath->format->depth != 8 ) return 0;
    return path_find( gpath, params[2], params[3], params[4], params[5], params[6] ) ;
}

/* --------------------------------------------------------------------------- */

static int modpathfind_path_getxy( INSTANCE * my, int * params )
{
    return path_get(( int * )params[0], ( int * )params[1] ) ;
}

/* --------------------------------------------------------------------------- */

static int modpathfind_path_wall( INSTANCE * my, int * params )
{
    return path_set_wall( params[0] ) ;
}

/* --------------------------------------------------------------------------- */

DLSYSFUNCS __bgdexport( mod_path, functions_exports )[] =
{
    /* Búsqueda de caminos*/
    { "PATH_FIND"   , "IIIIIII", TYPE_INT   , modpathfind_path_find     },
    { "PATH_GETXY"  , "PP"     , TYPE_INT   , modpathfind_path_getxy    },
    { "PATH_WALL"   , "I"      , TYPE_INT   , modpathfind_path_wall     },

    { 0             , 0        , 0          , 0                         }
};

/* --------------------------------------------------------------------------- */

char * __bgdexport( mod_path, modules_dependency )[] =
{
    "libgrbase",
    NULL
};

/* --------------------------------------------------------------------------- */
