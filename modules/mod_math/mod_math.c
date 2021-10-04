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

#define _GNU_SOURCE

#include "fmath.h"

#include <math.h>

#include <stdio.h>

#include "bgddl.h"

/* --------------------------------------------------------------------------- */

DLCONSTANT __bgdexport( mod_math, constants_def )[] =
{
    { "PI"  , TYPE_INT  , 180000    },
    { NULL  , 0         , 0         }
} ;

/* --------------------------------------------------------------------------- */
/* Funciones matemáticas */
/* --------------------------------------------------------------------------- */

static int math_abs( INSTANCE * my, int * params )
{
    float num = *( float * ) &params[0] ;
    float res = ( num < 0 ) ? -num : num ;
    return *(( int * )&res ) ;
}

/* --------------------------------------------------------------------------- */

static int math_pow( INSTANCE * my, int * params )
{
    float res = ( float )pow( *( float * ) &params[0], *( float * ) &params[1] ) ;
    return *(( int * )&res ) ;
}

/* --------------------------------------------------------------------------- */

static int math_sqrt( INSTANCE * my, int * params )
{
    float res = ( float )sqrt( *( float * ) &params[0] ) ;
    return *(( int * )&res ) ;
}

/* --------------------------------------------------------------------------- */

static int math_cos( INSTANCE * my, int * params )
{
    float param = *( float * ) &params[0] ;
    float res = ( float )cos(( double )( param * M_PI / 180000.0 ) ) ;
    return *(( int * )&res ) ;
}

/* --------------------------------------------------------------------------- */

static int math_sin( INSTANCE * my, int * params )
{
    float param = *( float * ) &params[0] ;
    float res = ( float )sin(( double )( param * M_PI / 180000.0 ) ) ;
    return *(( int * )&res ) ;
}

/* --------------------------------------------------------------------------- */

static int math_tan( INSTANCE * my, int * params )
{
    float param = *( float * ) &params[0] ;
    float res = ( float )tan(( double )( param * M_PI / 180000.0 ) ) ;
    return *(( int * )&res ) ;
}

/* --------------------------------------------------------------------------- */

static int math_acos( INSTANCE * my, int * params )
{
    float param = *( float * ) &params[0] ;
    float res = ( float )( acos(( double )param ) * 180000.0 / M_PI ) ;
    return *(( int * )&res ) ;
}

/* --------------------------------------------------------------------------- */

static int math_asin( INSTANCE * my, int * params )
{
    float param = *( float * ) &params[0] ;
    float res = ( float )( asin(( double )param ) * 180000.0 / M_PI ) ;
    return *(( int * )&res ) ;
}

/* --------------------------------------------------------------------------- */

static int math_atan( INSTANCE * my, int * params )
{
    float param = *( float * ) &params[0] ;
    float res = ( float )( atan(( double )param ) * 180000.0 / M_PI ) ;
    return *(( int * )&res ) ;
}

/* --------------------------------------------------------------------------- */

static int math_atan2( INSTANCE * my, int * params )
{
    float param1 = *( float * ) &params[0],
          param2 = *( float * ) &params[1];
    float res = ( float )( atan2(( double )param1, ( double )param2 ) * 180000.0 / M_PI ) ;
    return *(( int * )&res ) ;
}

/* --------------------------------------------------------------------------- */

static int math_isinf( INSTANCE * my, int * params )
{
    double param = ( double ) *( float * ) &params[0] ;
    return isinf( param );
}

/* --------------------------------------------------------------------------- */

static int math_isnan( INSTANCE * my, int * params )
{
    double param = ( double ) *( float * ) & params[0] ;
    return isnan( param );
}

/* --------------------------------------------------------------------------- */

static int math_finite( INSTANCE * my, int * params )
{
    double param = ( double ) *( float * ) &params[0] ;
#ifdef TARGET_IOS
	return ( !isnan(param) && !isinf(param) );
#else
    return finite ( param );
#endif
}

/* --------------------------------------------------------------------------- */

static int math_fget_angle( INSTANCE * my, int * params )
{
    double dx = params[2] - params[0] ;
    double dy = params[3] - params[1] ;
    int angle ;

    if ( dx == 0 ) return ( dy > 0 ) ? 270000L : 90000L ;

    angle = ( int )( atan( dy / dx ) * 180000.0 / M_PI ) ;

    return ( dx > 0 ) ? -angle : -angle + 180000L ;
}

/* --------------------------------------------------------------------------- */

static int math_fget_dist( INSTANCE * my, int * params )
{
    double dx = params[2] - params[0] ;
    double dy = params[3] - params[1] ;

    return ( int )sqrt( dx*dx + dy*dy ) ;
}

/* --------------------------------------------------------------------------- */

static int math_near_angle( INSTANCE * my, int * params )
{
    int angle = params[0] ;
    int dest  = params[1] ;
    int incr  = params[2] ;

    if ( angle < dest && dest - angle > 180000L )
        angle += 360000L ;

    if ( angle > dest && angle - dest > 180000L )
        angle -= 360000L ;

    if ( angle < dest )
    {
        angle += incr ;
        if ( angle > dest ) angle = dest ;
    }
    else
    {
        angle -= incr ;
        if ( angle < dest ) angle = dest ;
    }

    if ( angle < 0 ) return angle + 360000L ;
    if ( angle >= 360000L ) return angle - 360000L ;
    return angle ;
}

/* --------------------------------------------------------------------------- */

static int math_get_distx( INSTANCE * my, int * params )
{
    double angle = params[0] * M_PI / 180000.0 ;
    return ( int )( params[1] * cos( angle ) ) ;
}

/* --------------------------------------------------------------------------- */

static int math_get_disty( INSTANCE * my, int * params )
{
    double angle = params[0] * M_PI / 180000.0 ;
    return ( int )( params[1] * -sin( angle ) ) ;
}

/* ----------------------------------------------------------------- */
/* Declaracion de funciones                                                    */

DLSYSFUNCS __bgdexport( mod_math, functions_exports )[] =
{
    { "ABS"         , "F"       , TYPE_FLOAT    , math_abs          },
    { "POW"         , "FF"      , TYPE_FLOAT    , math_pow          },
    { "SQRT"        , "F"       , TYPE_FLOAT    , math_sqrt         },

    { "COS"         , "F"       , TYPE_FLOAT    , math_cos          },
    { "SIN"         , "F"       , TYPE_FLOAT    , math_sin          },
    { "TAN"         , "F"       , TYPE_FLOAT    , math_tan          },
    { "ACOS"        , "F"       , TYPE_FLOAT    , math_acos         },
    { "ASIN"        , "F"       , TYPE_FLOAT    , math_asin         },
    { "ATAN"        , "F"       , TYPE_FLOAT    , math_atan         },
    { "ATAN2"       , "FF"      , TYPE_FLOAT    , math_atan2        },

    { "ISINF"       , "F"       , TYPE_INT      , math_isinf        },
    { "ISNAN"       , "F"       , TYPE_INT      , math_isnan        },
    { "FINITE"      , "F"       , TYPE_INT      , math_finite       },

    { "FGET_ANGLE"  , "IIII"    , TYPE_INT      , math_fget_angle   },
    { "FGET_DIST"   , "IIII"    , TYPE_INT      , math_fget_dist    },
    { "NEAR_ANGLE"  , "III"     , TYPE_INT      , math_near_angle   },
    { "GET_DISTX"   , "II"      , TYPE_INT      , math_get_distx    },
    { "GET_DISTY"   , "II"      , TYPE_INT      , math_get_disty    },

    { 0             , 0         , 0             , 0                 }
};

/* --------------------------------------------------------------------------- */
