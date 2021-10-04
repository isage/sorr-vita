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

/* ----------------------------------------------------------------- */

#include "bgdrtm.h"
#include "bgddl.h"

#include "libgrbase.h"

/* ----------------------------------------------------------------- */
/*
 *  FUNCTION : modblendop_create_blendop
 *
 *  Create a new blendop table and initialize it with blend_init
 *  A blendop table is a group of two tables that return intermediate
 *  colors to a composite (+) operation that does no saturation:
 *
 *      Src_param   =   Src_color
 *      Dst_param   =   Dst_color
 *      Dst_color   =   Src_param + Dst_param       // No clamp!
 *
 *  The other blend_x funcions change the Src_param and Dst_param formulas.
 *  The Dst_color formula is immutable and embedded in the blitter
 *
 *  PARAMS :
 *      None
 *
 *  RETURN VALUE :
 *      Pointer to the new blendop table or NULL if not enough memory
 */

static int modblendop_create_blendop( INSTANCE * my, int * params )
{
    return ( int ) blend_create();
}

/* ----------------------------------------------------------------- */
/*
 *  FUNCTION : modblendop_apply
 *
 *  Apply a blend operation to the pixels of a graphic, as if it was
 *  rendered into a black (color 0) background
 *
 *  PARAMS :
 *      map             Pointer to the graphic
 *      blend           Pointer to the blend table
 *
 *  RETURN VALUE :
 *      0               Error
 *      1               OK
 */

static int modblendop_apply( INSTANCE * my, int * params )
{
    GRAPH * graph = bitmap_get( params[0], params[1] );
    if ( !graph ) return 0;
    blend_apply( graph, ( int16_t * )params[2] );
    return 1;

}

/* ----------------------------------------------------------------- */
/*
 *  FUNCTION : modblendop_assign
 *
 *  Assign a blend operation to a graphic. The graphic will be
 *  drawn using this blend operation thereafter.
 *
 *  PARAMS :
 *      map             Pointer to the graphic
 *      blend           Pointer to the blend table
 *
 *  RETURN VALUE :
 *      None
 */

static int modblendop_assign( INSTANCE * my, int * params )
{
    GRAPH * graph = bitmap_get( params[0], params[1] );
    if ( !graph ) return 0;
    blend_assign( graph, ( int16_t * )params[2] );
    return 1;
}

/* ----------------------------------------------------------------- */
/*
 *  FUNCTION : modblendop_free
 *
 *  Free the memory used by a blendop table
 *
 *  PARAMS :
 *      blend           Pointer to the blend table
 *
 *  RETURN VALUE :
 *      None
 */

static int modblendop_free( INSTANCE * my, int * params )
{
    blend_free(( int16_t * )params[0] );
    return 1;
}

/* ----------------------------------------------------------------- */
/*
 *  FUNCTION : modblendop_identity
 *
 *  Restore default blendop table
 *
 *  PARAMS :
 *      blend           Pointer to the blend table
 *
 *  RETURN VALUE :
 *      None
 */

static int modblendop_identity( INSTANCE * my, int * params )
{
    blend_init(( int16_t * )params[0] );
    return 1;
}

/* ----------------------------------------------------------------- */
/*
 *  FUNCTION : modblendop_grayscale
 *
 *  Initialize a blend table as a grayscale operation (changes the
 *  color value of the source object to grayscale). Three methods
 *  of operation are supported:
 *
 *      1.  Luminance:  result = 0.3R + 0.59G + 0.11B
 *      2.  Desaturate: result = (MAX(R,G,B)+MIN(R,G,B))/2
 *      3.  Maximum:    result = MAX(R,G,B)
 *
 *
 *      Src_param   =   Grayscale_operation(previous Src_param)
 *
 *  PARAMS :
 *      blend           Pointer to the blend table
 *      method          Method of operation
 *
 *  RETURN VALUE :
 *      0               Error
 *      1               Ok
 */

static int modblendop_grayscale( INSTANCE * my, int * params )
{
    blend_grayscale(( int16_t * )params[0], params[1] );
    return 1;
}

/* ----------------------------------------------------------------- */
/*
 *  FUNCTION : modblendop_translucency
 *
 *  Modify a blend table as a translucency combination operation
 *
 *      Src_param   =   (previous Src_param) * ammount
 *      Dst_param   =   Dst_color * (1.0 - ammount)
 *
 *  PARAMS :
 *      blend           Pointer to the blend table
 *      ammount         Opacity factor (1.0f = opaque, 0.0f = transparent)
 *
 *  RETURN VALUE :
 *      None
 */

static int modblendop_translucency( INSTANCE * my, int * params )
{
    blend_translucency(( int16_t * )params[0], *( float * )( &params[1] ) );
    return 1;
}

/* ----------------------------------------------------------------- */
/*
 *  FUNCTION : modblendop_intensity
 *
 *  Modify a blend table as an intensity operation (changes the
 *  color value of the source object as the factor ammount but
 *  does no transparency or other operation with the background)
 *
 *      Src_param   =   (previous Src_param) * ammount
 *
 *  PARAMS :
 *      blend           Pointer to the blend table
 *      ammount         Color component factor (1.0f leaves source unchanged)
 *
 *  RETURN VALUE :
 *      None
 */

static int modblendop_intensity( INSTANCE * my, int * params )
{
    blend_intensity(( int16_t * )params[0], *( float * )( &params[1] ) );
    return 1;
}

/* ----------------------------------------------------------------- */
/*
 *  FUNCTION : modblendop_swap
 *
 *  Swaps a blendop table. That is, change the blend operation so
 *  the Dst_color and Src_color are exchanged in the formulas.
 *
 *  PARAMS :
 *      blend           Pointer to the blend table
 *
 *  RETURN VALUE :
 *      None
 */

static int modblendop_swap( INSTANCE * my, int * params )
{
    blend_swap(( int16_t * )params[0] );
    return 1;
}

/* ----------------------------------------------------------------- */
/*
 *  FUNCTION : modblendop_tint
 *
 *  Modify a blend table as a tint operation (changes the
 *  color value of the source object as a combination of a given
 *  color and the source color with the factor given)
 *
 *      Src_param = Const_color * ammount
 *                  + (previous Src_Param) * (1.0f-ammount)
 *
 *  PARAMS :
 *      blend           Pointer to the blend table
 *      ammount         Color component factor (1.0f = Full Const color)
 *      cr              Constant color, red component (0-255)
 *      cg              Constant color, green component (0-255)
 *      cb              Constant color, blue component (0-255)
 *
 *  RETURN VALUE :
 *      None
 */

static int modblendop_tint( INSTANCE * my, int * params )
{
    blend_tint(( int16_t * )params[0], *( float * )( &params[1] ), ( uint8_t ) params[2], ( uint8_t ) params[3], ( uint8_t ) params[4] );
    return 1;
}

/* ---------------------------------------------------------------------- */

DLSYSFUNCS __bgdexport( mod_blendop, functions_exports)[] =
{
    /* Blendops */
    { "BLENDOP_NEW"          , ""      , TYPE_INT   , modblendop_create_blendop },
    { "BLENDOP_IDENTITY"     , "I"     , TYPE_INT   , modblendop_identity       },
    { "BLENDOP_TINT"         , "IFIII" , TYPE_INT   , modblendop_tint           },
    { "BLENDOP_TRANSLUCENCY" , "IF"    , TYPE_INT   , modblendop_translucency   },
    { "BLENDOP_INTENSITY"    , "IF"    , TYPE_INT   , modblendop_intensity      },
    { "BLENDOP_SWAP"         , "I"     , TYPE_INT   , modblendop_swap           },
    { "BLENDOP_ASSIGN"       , "III"   , TYPE_INT   , modblendop_assign         },
    { "BLENDOP_APPLY"        , "III"   , TYPE_INT   , modblendop_apply          },
    { "BLENDOP_FREE"         , "I"     , TYPE_INT   , modblendop_free           },
    { "BLENDOP_GRAYSCALE"    , "II"    , TYPE_INT   , modblendop_grayscale      },

    { 0                      , 0       , 0          , 0                                 }
};

/* --------------------------------------------------------------------------- */

char * __bgdexport( mod_blendop, modules_dependency )[] =
{
    "libgrbase",
    NULL
};

/* --------------------------------------------------------------------------- */

