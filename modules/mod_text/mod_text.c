/*
 *  Copyright © 2006-2012 SplinterGU( Fenix/Bennugd )
 *  Copyright © 2002-2006 Fenix Team( Fenix )
 *  Copyright © 1999-2002 José Luis Cebrián Pagüe( Fenix )
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

#include "xstrings.h"

#include "libgrbase.h"
#include "libfont.h"
#include "libtext.h"

/* --------------------------------------------------------------------------- */

DLCONSTANT __bgdexport( mod_text, constants_def )[] =
{
    { "ALL_TEXT"            , TYPE_INT  , 0                     },

    /* Alineacion de textos */
    { "ALIGN_TOP_LEFT"      , TYPE_INT  , ALIGN_TOP_LEFT        },
    { "ALIGN_TOP"           , TYPE_INT  , ALIGN_TOP             },
    { "ALIGN_TOP_RIGHT"     , TYPE_INT  , ALIGN_TOP_RIGHT       },
    { "ALIGN_CENTER_LEFT"   , TYPE_INT  , ALIGN_CENTER_LEFT     },
    { "ALIGN_CENTER"        , TYPE_INT  , ALIGN_CENTER          },
    { "ALIGN_CENTER_RIGHT"  , TYPE_INT  , ALIGN_CENTER_RIGHT    },
    { "ALIGN_BOTTOM_LEFT"   , TYPE_INT  , ALIGN_BOTTOM_LEFT     },
    { "ALIGN_BOTTOM"        , TYPE_INT  , ALIGN_BOTTOM          },
    { "ALIGN_BOTTOM_RIGHT"  , TYPE_INT  , ALIGN_BOTTOM_RIGHT    },

    { NULL                  , 0         , 0                     }
} ;

/* --------------------------------------------------------------------------- */
/* Funciones de visualización de textos */

static int modtext_write( INSTANCE * my, int * params )
{
    const char * text = string_get( params[4] );
    int r = text ? gr_text_new( params[0], params[1], params[2], params[3], text ) : 0 ;
    string_discard( params[4] );
    return r ;
}

/* --------------------------------------------------------------------------- */

static int modtext_write2( INSTANCE * my, int * params )
{
    const char * text = string_get( params[5] );
    int r = text ? gr_text_new2( params[0], params[1], params[2], params[3], params[4], text ) : 0 ;
    string_discard( params[5] );
    return r ;
}

/* --------------------------------------------------------------------------- */

static int modtext_write_in_map( INSTANCE * my, int * params )
{
    const char * text = string_get( params[1] );
    GRAPH * gr ;
    if ( !text ) return 0 ;
    gr = gr_text_bitmap( params[0], text, params[2] );
    string_discard( params[1] );
    if ( !gr ) return 0 ;
    return gr->code ;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : __modtext_write_var
 *
 *  Creates a new text associated with a variable
 *
 *  FENIX LANG PARAMS:
 *      params[0] : fnt to use
 *      params[1] : X
 *      params[2] : Y
 *      params[3] : align
 *      params[4] : pointer
 *      params[5] : DCB_TYPE
 *
 *  FENIX RETURN VALUE:
 *     Text ID for the newly created text
 *
 */

static int __modtext_write_var( int withz, INSTANCE * my, int * params )
{
    DCB_TYPEDEF * var ;
    int t = 0 ;

    if ( withz ) var =( DCB_TYPEDEF * )params[6];
    else var =( DCB_TYPEDEF * )params[5];

    switch( var->BaseType[0] )
    {
        case TYPE_FLOAT:
            t = TEXT_FLOAT;
            break;

        case TYPE_DWORD:
            t = TEXT_DWORD;
            break;

        case TYPE_WORD:
            t = TEXT_WORD;
            break;

        case TYPE_BYTE:
            t = TEXT_BYTE;
            break;

        case TYPE_STRING:
            t = TEXT_STRING;
            break;

        case TYPE_ARRAY:
            if ( var->BaseType[1] == TYPE_CHAR )
            {
                t = TEXT_CHARARRAY;
                break ;
            }
            break;

        case TYPE_SBYTE:
            t = TEXT_SBYTE;
            break;

        case TYPE_CHAR:
            t = TEXT_CHAR;
            break;

        case TYPE_SHORT:
            t = TEXT_SHORT;
            break;

        case TYPE_INT:
            t = TEXT_INT;
            break;

        case TYPE_POINTER:
            t = TEXT_POINTER;
            break;

        default:
            return -1;
            break;
    }

    if ( withz ) return gr_text_new_var2( params[0], params[1], params[2], params[3], params[4], ( void * )params[5], t );
    return gr_text_new_var( params[0], params[1], params[2], params[3], ( void * )params[4], t );
}

/* --------------------------------------------------------------------------- */

static int modtext_write_var( INSTANCE * my, int * params )
{
    return __modtext_write_var( 0, my, params );
}

/* --------------------------------------------------------------------------- */

static int modtext_write_var2( INSTANCE * my, int * params )
{
    return __modtext_write_var( 1, my, params );
}

/* --------------------------------------------------------------------------- */
/*
 *  Same as modtext_write_var, but param[5] not given and always set to VAR_STRING
 */

static int modtext_write_string( INSTANCE * my, int * params )
{
    return gr_text_new_var( params[0], params[1], params[2], params[3], ( void * )params[4], TEXT_STRING );
}

/* --------------------------------------------------------------------------- */

static int modtext_write_string2( INSTANCE * my, int * params )
{
    return gr_text_new_var2( params[0], params[1], params[2], params[3], params[4], ( void * )params[5], TEXT_STRING );
}

/* --------------------------------------------------------------------------- */
/*
 *  Same as modtext_write_var, but param[5] not given and always set to VAR_INT
 */

static int modtext_write_int( INSTANCE * my, int * params )
{
    return gr_text_new_var( params[0], params[1], params[2], params[3], ( void * )params[4], TEXT_INT );
}

/* --------------------------------------------------------------------------- */

static int modtext_write_int2( INSTANCE * my, int * params )
{
    return gr_text_new_var2( params[0], params[1], params[2], params[3], params[4], ( void * )params[5], TEXT_INT );
}

/* --------------------------------------------------------------------------- */
/*
 *  Same as modtext_write_var, but param[5] not given and always set to VAR_FLOAT
 */

static int modtext_write_float( INSTANCE * my, int * params )
{
    return gr_text_new_var( params[0], params[1], params[2], params[3], ( void * )params[4], TEXT_FLOAT );
}

/* --------------------------------------------------------------------------- */

static int modtext_write_float2( INSTANCE * my, int * params )
{
    return gr_text_new_var2( params[0], params[1], params[2], params[3], params[4], ( void * )params[5], TEXT_FLOAT );
}

/* --------------------------------------------------------------------------- */

static int modtext_move_text( INSTANCE * my, int * params )
{
    gr_text_move( params[0], params[1], params[2] );
    return 1;
}

/* --------------------------------------------------------------------------- */

static int modtext_move_text2( INSTANCE * my, int * params )
{
    gr_text_move2( params[0], params[1], params[2], params[3] );
    return 1;
}

/* --------------------------------------------------------------------------- */

static int modtext_delete_text( INSTANCE * my, int * params )
{
    gr_text_destroy( params[0] );
    return 1;
}

/* --------------------------------------------------------------------------- */

static int modtext_text_height( INSTANCE * my, int * params )
{
    const char * str = string_get( params[1] );
    int result = gr_text_height( params[0], ( const unsigned char * ) str );
    string_discard( params[1] );
    return result ;
}

/* --------------------------------------------------------------------------- */

static int modtext_text_width( INSTANCE * my, int * params )
{
    const char * str = string_get( params[1] );
    int result = gr_text_width( params[0], ( const unsigned char * ) str );
    string_discard( params[1] );
    return result ;
}

/* --------------------------------------------------------------------------- */

static int modtext_set_text_color( INSTANCE * my, int * params )
{
    gr_text_setcolor( params[0] );
    return 1 ;
}

/* --------------------------------------------------------------------------- */

static int modtext_set_text_color2( INSTANCE * my, int * params )
{
    gr_text_setcolor2( params[0], params[1] );
    return 1 ;
}

/* --------------------------------------------------------------------------- */

static int modtext_get_text_color( INSTANCE * my, int * params )
{
    return( gr_text_getcolor() );
}

/* --------------------------------------------------------------------------- */

static int modtext_get_text_color2( INSTANCE * my, int * params )
{
    return( gr_text_getcolor2( params[0] ) );
}

/* --------------------------------------------------------------------------- */

DLSYSFUNCS  __bgdexport( mod_text, functions_exports )[] = {

    /* Textos */
    { "WRITE"               , "IIIIS"   , TYPE_INT  , modtext_write             },
    { "WRITE"               , "IIIIIS"  , TYPE_INT  , modtext_write2            },
    { "WRITE_INT"           , "IIIIP"   , TYPE_INT  , modtext_write_int         },
    { "WRITE_INT"           , "IIIIIP"  , TYPE_INT  , modtext_write_int2        },
    { "MOVE_TEXT"           , "III"     , TYPE_INT  , modtext_move_text         },
    { "MOVE_TEXT"           , "IIII"    , TYPE_INT  , modtext_move_text2        },
    { "DELETE_TEXT"         , "I"       , TYPE_INT  , modtext_delete_text       },
    { "WRITE_IN_MAP"        , "ISI"     , TYPE_INT  , modtext_write_in_map      },
    { "TEXT_WIDTH"          , "IS"      , TYPE_INT  , modtext_text_width        },
    { "TEXT_HEIGHT"         , "IS"      , TYPE_INT  , modtext_text_height       },
    { "GET_TEXT_COLOR"      , ""        , TYPE_INT  , modtext_get_text_color    },
    { "GET_TEXT_COLOR"      , "I"       , TYPE_INT  , modtext_get_text_color2   },
    { "SET_TEXT_COLOR"      , "I"       , TYPE_INT  , modtext_set_text_color    },
    { "SET_TEXT_COLOR"      , "II"      , TYPE_INT  , modtext_set_text_color2   },

    /* Extended Write */
    { "WRITE_VAR"           , "IIIIV++" , TYPE_INT  , modtext_write_var         },
    { "WRITE_VAR"           , "IIIIIV++", TYPE_INT  , modtext_write_var2        },
    { "WRITE_FLOAT"         , "IIIIP"   , TYPE_INT  , modtext_write_float       },
    { "WRITE_FLOAT"         , "IIIIIP"  , TYPE_INT  , modtext_write_float2      },
    { "WRITE_STRING"        , "IIIIP"   , TYPE_INT  , modtext_write_string      },
    { "WRITE_STRING"        , "IIIIIP"  , TYPE_INT  , modtext_write_string2     },

    { 0                     , 0         , 0         , 0                     }
};

/* --------------------------------------------------------------------------- */

char * __bgdexport( mod_text, modules_dependency )[] =
{
    "libgrbase",
    "libblit",
    "libtext",
    "libfont",
    NULL
};

/* --------------------------------------------------------------------------- */
