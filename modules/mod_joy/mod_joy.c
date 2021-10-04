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
/* Thanks Sandman for suggest on openjoys at initialization time               */
/* --------------------------------------------------------------------------- */
/* Credits SplinterGU/Sandman 2007-2009                                        */
/* --------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* --------------------------------------------------------------------------- */

#include "bgddl.h"
#include "bgdrtm.h"

#include "libjoy.h"

/* --------------------------------------------------------------------------- */
/**
 * JOY_NUMBER()
 * JOY_NUMJOYSTICKS()
 * NUMBER_JOY()
 *
 * Returns the number of joysticks present in the system
 *
 **/

static int modjoy_num( INSTANCE * my, int * params )
{
    return ( libjoy_num() );
}

/* --------------------------------------------------------------------------- */
/**
 * JOY_NAME (int JOY)
 *
 * Returns the name for a given joystick present in the system
 *
 **/

static int modjoy_name( INSTANCE * my, int * params )
{
    return ( libjoy_name( params[0] ) );
}

/* --------------------------------------------------------------------------- */
/**
 * JOY_SELECT (int JOY)
 * SELECT_JOY(int JOY)
 *
 * Returns the selected joystick number
 *
 **/

static int modjoy_select( INSTANCE * my, int * params )
{
    return ( libjoy_select( params[0] ) );
}

/* --------------------------------------------------------------------------- */
/**
 * JOY_BUTTONS ()
 * JOY_NUMBUTTONS ()
 *
 * Returns the selected joystick total buttons
 *
 **/

static int modjoy_buttons( INSTANCE * my, int * params )
{
    return ( libjoy_buttons() ) ;
}

/* --------------------------------------------------------------------------- */
/**
 * JOY_AXES ()
 * JOY_NUMAXES()
 *
 * Returns the selected joystick total axes
 *
 **/

static int modjoy_axes( INSTANCE * my, int * params )
{
    return ( libjoy_axes() ) ;
}

/* --------------------------------------------------------------------------- */
/**
 * JOY_GETBUTTON (int button)
 * GET_JOY_BUTTON (int button)
 *
 * Returns the selected joystick state for the given button
 *
 **/

static int modjoy_get_button( INSTANCE * my, int * params )
{
    return ( libjoy_get_button( params[0] ) ) ;
}

/* --------------------------------------------------------------------------- */
/**
 * JOY_GETAXIS (int axis)
 * JOY_GETPOSITION (int axis)
 * GET_JOY_POSITION (int axis)
 *
 * Returns the selected joystick state for the given axis
 *
 **/

static int modjoy_get_position( INSTANCE * my, int * params )
{
    return ( libjoy_get_position( params[0] ) ) ;
}

/* --------------------------------------------------------------------------- */
/**
 * JOY_NUMHATS ()
 *
 * Returns the total number of POV hats of the current selected joystick
 *
 **/

static int modjoy_hats( INSTANCE * my, int * params )
{
    return ( libjoy_hats() ) ;
}

/* --------------------------------------------------------------------------- */
/**
 * JOY_NUMBALLS ()
 *
 * Returns the total number of balls of the current selected joystick
 *
 **/

static int modjoy_balls( INSTANCE * my, int * params )
{
    return ( libjoy_balls() ) ;
}

/* --------------------------------------------------------------------------- */
/**
 * JOY_GETHAT (int HAT)
 *
 * Returns the state of the specfied hat on the current selected joystick
 *
 **/

static int modjoy_get_hat( INSTANCE * my, int * params )
{
    return ( libjoy_get_hat( params[0] ) ) ;
}

/* --------------------------------------------------------------------------- */
/**
 * JOY_GETBALL (int BALL, int* dx, int* dy)
 *
 * Returns the state of the specfied ball on the current selected joystick
 *
 **/

static int modjoy_get_ball( INSTANCE * my, int * params )
{
    return ( libjoy_get_ball( params[0], ( int* )params[1], ( int* )params[2] ) ) ;
}

/* --------------------------------------------------------------------------- */

static int modjoy_get_accel( INSTANCE * my, int * params )
{
    return ( libjoy_get_accel( ( int * ) params[0], ( int * ) params[1], ( int * ) params[2] ) );
}

/* --------------------------------------------------------------------------- */
/* --------------------------------------------------------------------------- */
/* --------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------- */
/**
 * JOY_BUTTONS (int JOY)
 * JOY_NUMBUTTONS (int JOY)
 *
 * Returns the selected joystick total buttons
 *
 **/

static int modjoy_buttons_specific( INSTANCE * my, int * params )
{
    return ( libjoy_buttons_specific( params[0] ) ) ;
}

/* --------------------------------------------------------------------------- */
/**
 * JOY_AXES (int JOY)
 * JOY_NUMAXES (int JOY)
 *
 * Returns the selected joystick total axes
 *
 **/

static int modjoy_axes_specific( INSTANCE * my, int * params )
{
    return ( libjoy_axes_specific( params[0] ) ) ;
}

/* --------------------------------------------------------------------------- */
/**
 * JOY_GETBUTTON (int JOY, int button)
 * GET_JOY_BUTTON (int JOY, int button)
 *
 * Returns the selected joystick state for the given button
 *
 **/

static int modjoy_get_button_specific( INSTANCE * my, int * params )
{
    return ( libjoy_get_button_specific( params[0], params[1] ) );
}

/* --------------------------------------------------------------------------- */
/**
 * JOY_GETAXIS (int JOY, int axis)
 * JOY_GETPOSITION (int JOY, int axis)
 * GET_JOY_POSITION (int JOY, int axis)
 *
 * Returns the selected joystick state for the given axis
 *
 **/

static int modjoy_get_position_specific( INSTANCE * my, int * params )
{
    return ( libjoy_get_position_specific( params[0], params[1] ) );
}

/* --------------------------------------------------------------------------- */
/* Added by Sandman */
/* --------------------------------------------------------------------------- */
/**
 * JOY_NUMHATS (int JOY)
 *
 * Returns the total number of POV hats of the specified joystick
 *
 **/

static int modjoy_hats_specific( INSTANCE * my, int * params )
{
    return ( libjoy_hats_specific( params[0] ) ) ;
}

/* --------------------------------------------------------------------------- */
/**
 * JOY_NUMBALLS (int JOY)
 *
 * Returns the total number of balls of the specified joystick
 *
 **/

static int modjoy_balls_specific( INSTANCE * my, int * params )
{
    return ( libjoy_balls_specific( params[0] ) ) ;
}

/* --------------------------------------------------------------------------- */
/**
 * JOY_GETHAT (int JOY, int HAT)
 *
 * Returns the state of the specfied hat on the specified joystick
 *
 **/

static int modjoy_get_hat_specific( INSTANCE * my, int * params )
{
    return ( libjoy_get_hat_specific( params[0], params[1] ) );
}

/* --------------------------------------------------------------------------- */
/**
 * JOY_GETBALL (int JOY, int BALL, int* dx, int* dy)
 *
 * Returns the state of the specfied ball on the specified joystick
 *
 **/

static int modjoy_get_ball_specific( INSTANCE * my, int * params )
{
    return ( libjoy_get_ball_specific( params[0], params[1], ( int * ) params[2], ( int * ) params[3] ) );
}

/* --------------------------------------------------------------------------- */

static int modjoy_get_accel_specific( INSTANCE * my, int * params )
{
    return ( libjoy_get_accel_specific( params[0], ( int * ) params[1], ( int * ) params[2], ( int * ) params[3] ) );
}

/* ---------------------------------------------------------------------- */

DLSYSFUNCS  __bgdexport( mod_joy, functions_exports )[] =
{
    { "JOY_AXES"            , ""      , TYPE_INT    , modjoy_axes                  },
    { "JOY_AXES"            , "I"     , TYPE_INT    , modjoy_axes_specific         },
    { "JOY_NUMAXES"         , ""      , TYPE_INT    , modjoy_axes                  },
    { "JOY_NUMAXES"         , "I"     , TYPE_INT    , modjoy_axes_specific         },
    { "JOY_GETAXIS"         , "I"     , TYPE_INT    , modjoy_get_position          },
    { "JOY_GETAXIS"         , "II"    , TYPE_INT    , modjoy_get_position_specific },

    { "JOY_BUTTONS"         , ""      , TYPE_INT    , modjoy_buttons               },
    { "JOY_BUTTONS"         , "I"     , TYPE_INT    , modjoy_buttons_specific      },

    { "JOY_NAME"            , "I"     , TYPE_STRING , modjoy_name                  },

    { "JOY_NUMBUTTONS"      , ""      , TYPE_INT    , modjoy_buttons               },
    { "JOY_NUMBUTTONS"      , "I"     , TYPE_INT    , modjoy_buttons_specific      },

    { "JOY_NUMBER"          , ""      , TYPE_INT    , modjoy_num                   },
    { "JOY_NUMJOYSTICKS"    , ""      , TYPE_INT    , modjoy_num                   },

    { "JOY_SELECT"          , "I"     , TYPE_INT    , modjoy_select                },

    { "JOY_GETBUTTON"       , "I"     , TYPE_INT    , modjoy_get_button            },
    { "JOY_GETBUTTON"       , "II"    , TYPE_INT    , modjoy_get_button_specific   },
    { "JOY_GETPOSITION"     , "I"     , TYPE_INT    , modjoy_get_position          },
    { "JOY_GETPOSITION"     , "II"    , TYPE_INT    , modjoy_get_position_specific },

    { "JOY_NUMHATS"         , ""      , TYPE_INT    , modjoy_hats                  },
    { "JOY_NUMHATS"         , "I"     , TYPE_INT    , modjoy_hats_specific         },   /* Added by Sandman */
    { "JOY_NUMBALLS"        , ""      , TYPE_INT    , modjoy_balls                 },
    { "JOY_NUMBALLS"        , "I"     , TYPE_INT    , modjoy_balls_specific        },   /* Added by Sandman */
    { "JOY_GETHAT"          , "I"     , TYPE_INT    , modjoy_get_hat               },
    { "JOY_GETHAT"          , "II"    , TYPE_INT    , modjoy_get_hat_specific      },   /* Added by Sandman */
    { "JOY_GETBALL"         , "IPP"   , TYPE_INT    , modjoy_get_ball              },
    { "JOY_GETBALL"         , "IIPP"  , TYPE_INT    , modjoy_get_ball_specific     },   /* Added by Sandman */

    { "JOY_GETACCEL"        , "PPP"   , TYPE_INT    , modjoy_get_accel             },
    { "JOY_GETACCEL"        , "IPPP"  , TYPE_INT    , modjoy_get_accel_specific    },

    /* Compatibility */

    { "NUMBER_JOY"          , ""      , TYPE_INT    , modjoy_num                   },
    { "SELECT_JOY"          , "I"     , TYPE_INT    , modjoy_select                },
    { "GET_JOY_BUTTON"      , "I"     , TYPE_INT    , modjoy_get_button            },
    { "GET_JOY_BUTTON"      , "II"    , TYPE_INT    , modjoy_get_button_specific   },
    { "GET_JOY_POSITION"    , "I"     , TYPE_INT    , modjoy_get_position          },
    { "GET_JOY_POSITION"    , "II"    , TYPE_INT    , modjoy_get_position_specific },
    { 0                     , 0       , 0           , 0                             }
};

/* ----------------------------------------------------------------- */

char * __bgdexport( mod_joy, modules_dependency )[] =
{
    "libjoy",
    NULL
};

/* ----------------------------------------------------------------- */
