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

#include <SDL.h>

/* --------------------------------------------------------------------------- */

#include "bgddl.h"

#include "bgdrtm.h"

#include "files.h"
#include "xstrings.h"

/* --------------------------------------------------------------------------- */

#ifdef TARGET_CAANOO
#include "caanoo/te9_tf9_hybrid_driver.c"

#ifndef ABS
#define ABS(x) (((x) < 0) ? -(x):(x))
#endif

#endif

/* --------------------------------------------------------------------------- */

#define MAX_JOYS    32

static int _max_joys = 0;
static SDL_Joystick * _joysticks[MAX_JOYS];
static int _selected_joystick = -1;

/* --------------------------------------------------------------------------- */
/* libjoy_num ()                                                               */
/* Returns the number of joysticks present in the system                       */
/* --------------------------------------------------------------------------- */

int libjoy_num( void )
{
    return _max_joys ;
}

/* --------------------------------------------------------------------------- */
/* libjoy_name (int JOY)                                                       */
/* Returns the name for a given joystick present in the system                 */
/* --------------------------------------------------------------------------- */

int libjoy_name( int joy )
{
    int result;
    result = string_new( SDL_JoystickNameForIndex( joy ) );
    string_use( result );
    return result;
}

/* --------------------------------------------------------------------------- */
/* libjoy_select (int JOY)                                                     */
/* Returns the selected joystick number                                        */
/* --------------------------------------------------------------------------- */

int libjoy_select( int joy )
{
    return ( _selected_joystick = joy );
}

/* --------------------------------------------------------------------------- */
/* libjoy_buttons ()                                                           */
/* Returns the selected joystick total buttons                                 */
/* --------------------------------------------------------------------------- */

int libjoy_buttons( void )
{
    if ( _selected_joystick >= 0 && _selected_joystick < _max_joys )
    {
#ifdef TARGET_CAANOO
        if ( _selected_joystick == 0 ) return 21;
#endif
        return SDL_JoystickNumButtons( _joysticks[ _selected_joystick ] ) ;
    }
    return 0 ;
}

/* --------------------------------------------------------------------------- */
/* libjoy_axes ()                                                              */
/* Returns the selected joystick total axes                                    */
/* --------------------------------------------------------------------------- */

int libjoy_axes( void )
{
    if ( _selected_joystick >= 0 && _selected_joystick < _max_joys )
    {
        return SDL_JoystickNumAxes( _joysticks[ _selected_joystick ] ) ;
    }
    return 0 ;
}

/* --------------------------------------------------------------------------- */
/* libjoy_get_button ( int button )                                            */
/* Returns the selected joystick state for the given button                    */
/* --------------------------------------------------------------------------- */

int libjoy_get_button( int button )
{
    if ( _selected_joystick >= 0 && _selected_joystick < _max_joys )
    {
#ifdef TARGET_CAANOO
        if ( _selected_joystick == 0 )
        {
            int vax;

            switch ( button )
            {
                case    1: /* UPLF                  */  return ( SDL_JoystickGetAxis( _joysticks[ 0 ], 1 ) < -16384 && SDL_JoystickGetAxis( _joysticks[ 0 ], 0 ) < -16384 );
                case    3: /* DWLF                  */  return ( SDL_JoystickGetAxis( _joysticks[ 0 ], 1 ) >  16384 && SDL_JoystickGetAxis( _joysticks[ 0 ], 0 ) < -16384 );
                case    5: /* DWRT                  */  return ( SDL_JoystickGetAxis( _joysticks[ 0 ], 1 ) >  16384 && SDL_JoystickGetAxis( _joysticks[ 0 ], 0 ) >  16384 );
                case    7: /* UPRT                  */  return ( SDL_JoystickGetAxis( _joysticks[ 0 ], 1 ) < -16384 && SDL_JoystickGetAxis( _joysticks[ 0 ], 0 ) >  16384 );
                case    0: /* UP                    */  vax = SDL_JoystickGetAxis( _joysticks[ 0 ], 0 ) ; return ( SDL_JoystickGetAxis( _joysticks[ 0 ], 1 ) < -16384 && ABS( vax ) < 16384 );
                case    4: /* DW                    */  vax = SDL_JoystickGetAxis( _joysticks[ 0 ], 0 ) ; return ( SDL_JoystickGetAxis( _joysticks[ 0 ], 1 ) >  16384 && ABS( vax ) < 16384 );
                case    2: /* LF                    */  vax = SDL_JoystickGetAxis( _joysticks[ 0 ], 1 ) ; return ( SDL_JoystickGetAxis( _joysticks[ 0 ], 0 ) < -16384 && ABS( vax ) < 16384 );
                case    6: /* RT                    */  vax = SDL_JoystickGetAxis( _joysticks[ 0 ], 1 ) ; return ( SDL_JoystickGetAxis( _joysticks[ 0 ], 0 ) >  16384 && ABS( vax ) < 16384 );

                case    8:  /* MENU->HOME           */  return ( SDL_JoystickGetButton( _joysticks[ 0 ], 6 ) );
                case    9:  /* SELECT->HELP-II      */  return ( SDL_JoystickGetButton( _joysticks[ 0 ], 9 ) );
                case    10: /* L                    */  return ( SDL_JoystickGetButton( _joysticks[ 0 ], 4 ) );
                case    11: /* R                    */  return ( SDL_JoystickGetButton( _joysticks[ 0 ], 5 ) );
                case    12: /* A                    */  return ( SDL_JoystickGetButton( _joysticks[ 0 ], 0 ) );
                case    13: /* B                    */  return ( SDL_JoystickGetButton( _joysticks[ 0 ], 2 ) );
                case    14: /* X                    */  return ( SDL_JoystickGetButton( _joysticks[ 0 ], 1 ) );
                case    15: /* Y                    */  return ( SDL_JoystickGetButton( _joysticks[ 0 ], 3 ) );
                case    16: /* VOLUP                */  return ( 0 );
                case    17: /* VOLDOWN              */  return ( 0 );
                case    18: /* CLICK                */  return ( SDL_JoystickGetButton( _joysticks[ 0 ], 10 ) );
                case    19: /* POWER-LOCK  (CAANOO) */  return ( SDL_JoystickGetButton( _joysticks[ 0 ], 7 ) ); /* Only Caanoo */
                case    20: /* HELP-I      (CAANOO) */  return ( SDL_JoystickGetButton( _joysticks[ 0 ], 8 ) ); /* Only Caanoo */
                default:                                return ( 0 );
            }
        }
#endif
        return SDL_JoystickGetButton( _joysticks[ _selected_joystick ], button ) ;
    }
    return 0 ;
}

/* --------------------------------------------------------------------------- */
/* libjoy_get_position ( int axis )                                            */
/* Returns the selected joystick state for the given axis                      */
/* --------------------------------------------------------------------------- */

int libjoy_get_position( int axis )
{
    if ( _selected_joystick >= 0 && _selected_joystick < _max_joys )
    {
        return SDL_JoystickGetAxis( _joysticks[ _selected_joystick ], axis ) ;
    }
    return 0 ;
}

/* --------------------------------------------------------------------------- */
/* libjoy_hats ()                                                              */
/* Returns the total number of POV hats of the current selected joystick       */
/* --------------------------------------------------------------------------- */

int libjoy_hats( void )
{
    if ( _selected_joystick >= 0 && _selected_joystick < _max_joys )
    {
        return SDL_JoystickNumHats( _joysticks[ _selected_joystick ] ) ;
    }
    return 0 ;
}

/* --------------------------------------------------------------------------- */
/* libjoy_balls ()                                                             */
/* Returns the total number of balls of the current selected joystick          */
/* --------------------------------------------------------------------------- */

int libjoy_balls( void )
{
    if ( _selected_joystick >= 0 && _selected_joystick < _max_joys )
    {
        return SDL_JoystickNumBalls( _joysticks[ _selected_joystick ] ) ;
    }
    return 0 ;
}

/* --------------------------------------------------------------------------- */
/* libjoy_get_hat (int HAT)                                                    */
/* Returns the state of the specfied hat on the current selected joystick      */
/* --------------------------------------------------------------------------- */

int libjoy_get_hat( int hat )
{
    if ( _selected_joystick >= 0 && _selected_joystick < _max_joys )
    {
        if ( hat >= 0 && hat <= SDL_JoystickNumHats( _joysticks[ _selected_joystick ] ) )
        {
            return SDL_JoystickGetHat( _joysticks[ _selected_joystick ], hat ) ;
        }
    }
    return 0 ;
}

/* --------------------------------------------------------------------------- */
/* libjoy_get_ball (int BALL, int* dx, int* dy)                                */
/* Returns the state of the specfied ball on the current selected joystick     */
/* --------------------------------------------------------------------------- */

int libjoy_get_ball( int ball, int * dx, int * dy )
{
    if ( _selected_joystick >= 0 && _selected_joystick < _max_joys )
    {
        if ( ball >= 0 && ball <= SDL_JoystickNumBalls( _joysticks[ball] ) )
        {
            return SDL_JoystickGetBall( _joysticks[ _selected_joystick ], ball, dx, dy ) ;
        }
    }
    return -1 ;
}

/* --------------------------------------------------------------------------- */

int libjoy_get_accel( int * x, int * y, int * z )
{
#ifdef TARGET_CAANOO
    if ( _selected_joystick == 0 )
    {
        KIONIX_ACCEL_read_LPF_g( x, y, z );
    }
    return 0;
#else
    return -1;
#endif
}

/* --------------------------------------------------------------------------- */
/* --------------------------------------------------------------------------- */
/* --------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------- */
/* libjoy_buttons_specific (int JOY)                                           */
/* Returns the selected joystick total buttons                                 */
/* --------------------------------------------------------------------------- */

int libjoy_buttons_specific( int joy )
{
    if ( joy >= 0 && joy < _max_joys )
    {
#ifdef TARGET_CAANOO
        if ( joy == 0 ) return 21;
#endif
        return SDL_JoystickNumButtons( _joysticks[ joy ] ) ;
    }
    return 0 ;
}

/* --------------------------------------------------------------------------- */
/* libjoy_axes_specific (int JOY)                                              */
/* Returns the selected joystick total axes                                    */
/* --------------------------------------------------------------------------- */

int libjoy_axes_specific( int joy )
{
    if ( joy >= 0 && joy < _max_joys )
    {
        return SDL_JoystickNumAxes( _joysticks[ joy ] ) ;
    }
    return 0 ;
}

/* --------------------------------------------------------------------------- */
/* libjoy_get_button_specific (int JOY, int button)                            */
/* Returns the selected joystick state for the given button                    */
/* --------------------------------------------------------------------------- */

int libjoy_get_button_specific( int joy, int button )
{
    if ( joy >= 0 && joy < _max_joys )
    {
#ifdef TARGET_CAANOO
        if ( button >= 0 && ( ( joy == 0 && button <= 21 ) || ( joy != 0 && SDL_JoystickNumButtons( _joysticks[ joy ] ) ) ) )
#else
        if ( button >= 0 && button <= SDL_JoystickNumButtons( _joysticks[ joy ] ) )
#endif
        {
#ifdef TARGET_CAANOO
            if ( joy == 0 )
            {
                int vax;

                switch ( button )
                {
                    case    1: /* UPLF                  */  return ( SDL_JoystickGetAxis( _joysticks[ 0 ], 1 ) < -16384 && SDL_JoystickGetAxis( _joysticks[ 0 ], 0 ) < -16384 );
                    case    3: /* DWLF                  */  return ( SDL_JoystickGetAxis( _joysticks[ 0 ], 1 ) >  16384 && SDL_JoystickGetAxis( _joysticks[ 0 ], 0 ) < -16384 );
                    case    5: /* DWRT                  */  return ( SDL_JoystickGetAxis( _joysticks[ 0 ], 1 ) >  16384 && SDL_JoystickGetAxis( _joysticks[ 0 ], 0 ) >  16384 );
                    case    7: /* UPRT                  */  return ( SDL_JoystickGetAxis( _joysticks[ 0 ], 1 ) < -16384 && SDL_JoystickGetAxis( _joysticks[ 0 ], 0 ) >  16384 );
                    case    0: /* UP                    */  vax = SDL_JoystickGetAxis( _joysticks[ 0 ], 0 ) ; return ( SDL_JoystickGetAxis( _joysticks[ 0 ], 1 ) < -16384 && ABS( vax ) < 16384 );
                    case    4: /* DW                    */  vax = SDL_JoystickGetAxis( _joysticks[ 0 ], 0 ) ; return ( SDL_JoystickGetAxis( _joysticks[ 0 ], 1 ) >  16384 && ABS( vax ) < 16384 );
                    case    2: /* LF                    */  vax = SDL_JoystickGetAxis( _joysticks[ 0 ], 1 ) ; return ( SDL_JoystickGetAxis( _joysticks[ 0 ], 0 ) < -16384 && ABS( vax ) < 16384 );
                    case    6: /* RT                    */  vax = SDL_JoystickGetAxis( _joysticks[ 0 ], 1 ) ; return ( SDL_JoystickGetAxis( _joysticks[ 0 ], 0 ) >  16384 && ABS( vax ) < 16384 );

                    case    8:  /* MENU->HOME           */  return ( SDL_JoystickGetButton( _joysticks[ 0 ], 6 ) );
                    case    9:  /* SELECT->HELP-II      */  return ( SDL_JoystickGetButton( _joysticks[ 0 ], 9 ) );
                    case    10: /* L                    */  return ( SDL_JoystickGetButton( _joysticks[ 0 ], 4 ) );
                    case    11: /* R                    */  return ( SDL_JoystickGetButton( _joysticks[ 0 ], 5 ) );
                    case    12: /* A                    */  return ( SDL_JoystickGetButton( _joysticks[ 0 ], 0 ) );
                    case    13: /* B                    */  return ( SDL_JoystickGetButton( _joysticks[ 0 ], 2 ) );
                    case    14: /* X                    */  return ( SDL_JoystickGetButton( _joysticks[ 0 ], 1 ) );
                    case    15: /* Y                    */  return ( SDL_JoystickGetButton( _joysticks[ 0 ], 3 ) );
                    case    16: /* VOLUP                */  return ( 0 );
                    case    17: /* VOLDOWN              */  return ( 0 );
                    case    18: /* CLICK                */  return ( SDL_JoystickGetButton( _joysticks[ 0 ], 10 ) );
                    case    19: /* POWER-LOCK  (CAANOO) */  return ( SDL_JoystickGetButton( _joysticks[ 0 ], 7 ) ); /* Only Caanoo */
                    case    20: /* HELP-I      (CAANOO) */  return ( SDL_JoystickGetButton( _joysticks[ 0 ], 8 ) ); /* Only Caanoo */
                    default:                                return ( 0 );
                }
            }
#endif
            return SDL_JoystickGetButton( _joysticks[ joy ], button ) ;
        }
    }
    return 0 ;
}

/* --------------------------------------------------------------------------- */
/* libjoy_get_position_specific (int JOY, int axis)                            */
/* Returns the selected joystick state for the given axis                      */
/* --------------------------------------------------------------------------- */

int libjoy_get_position_specific( int joy, int axis )
{
    if ( joy >= 0 && joy < _max_joys )
    {
        if ( axis >= 0 && axis <= SDL_JoystickNumAxes( _joysticks[ joy ] ) )
        {
            return SDL_JoystickGetAxis( _joysticks[ joy ], axis ) ;
        }
    }
    return 0 ;
}

/* --------------------------------------------------------------------------- */
/* Added by Sandman */
/* --------------------------------------------------------------------------- */
/* --------------------------------------------------------------------------- */
/* libjoy_hats_specific (int JOY)                                              */
/* Returns the total number of POV hats of the specified joystick              */
/* --------------------------------------------------------------------------- */

int libjoy_hats_specific( int joy )
{
    if ( joy >= 0 && joy < _max_joys )
    {
        return SDL_JoystickNumHats( _joysticks[ joy ] ) ;
    }
    return 0 ;
}

/* --------------------------------------------------------------------------- */
/* libjoy_balls_specific (int JOY)                                             */
/* Returns the total number of balls of the specified joystick                 */
/* --------------------------------------------------------------------------- */

int libjoy_balls_specific( int joy )
{
    if ( joy >= 0 && joy < _max_joys )
    {
        return SDL_JoystickNumBalls( _joysticks[ joy ] ) ;
    }
    return 0 ;
}

/* --------------------------------------------------------------------------- */
/* libjoy_get_hat_specific (int JOY, int HAT)                                  */
/* Returns the state of the specfied hat on the specified joystick             */
/* --------------------------------------------------------------------------- */

int libjoy_get_hat_specific( int joy, int hat )
{
    if ( joy >= 0 && joy < _max_joys )
    {
        if ( hat >= 0 && hat <= SDL_JoystickNumHats( _joysticks[ joy ] ) )
        {
            return SDL_JoystickGetHat( _joysticks[ joy ], hat ) ;
        }
    }
    return 0 ;
}

/* --------------------------------------------------------------------------- */
/* libjoy_get_ball_specific (int JOY, int BALL, int* dx, int* dy)              */
/* Returns the state of the specfied ball on the specified joystick            */
/* --------------------------------------------------------------------------- */

int libjoy_get_ball_specific( int joy, int ball, int * dx, int * dy )
{
    if ( joy >= 0 && joy < _max_joys )
    {
        if ( ball >= 0 && ball <= SDL_JoystickNumBalls( _joysticks[ joy ] ) )
        {
            return SDL_JoystickGetBall( _joysticks[ joy ], ball, dx, dy ) ;
        }
    }
    return -1 ;
}

/* --------------------------------------------------------------------------- */

int libjoy_get_accel_specific( int joy, int * x, int * y, int * z )
{
#ifdef TARGET_CAANOO
    if ( joy == 0 )
    {
        KIONIX_ACCEL_read_LPF_g( x, y, z );
        return 0;
    }
#endif
    return -1;
}

/* --------------------------------------------------------------------------- */
/* Funciones de inicializacion del modulo/plugin                               */

DLCONSTANT __bgdexport( libjoy, constants_def )[] =
{
    { "JOY_HAT_CENTERED"    , TYPE_DWORD, SDL_HAT_CENTERED  },
    { "JOY_HAT_UP"          , TYPE_DWORD, SDL_HAT_UP        },
    { "JOY_HAT_RIGHT"       , TYPE_DWORD, SDL_HAT_RIGHT     },
    { "JOY_HAT_DOWN"        , TYPE_DWORD, SDL_HAT_DOWN      },
    { "JOY_HAT_LEFT"        , TYPE_DWORD, SDL_HAT_LEFT      },
    { "JOY_HAT_RIGHTUP"     , TYPE_DWORD, SDL_HAT_RIGHTUP   },
    { "JOY_HAT_RIGHTDOWN"   , TYPE_DWORD, SDL_HAT_RIGHTDOWN },
    { "JOY_HAT_LEFTUP"      , TYPE_DWORD, SDL_HAT_LEFTUP    },
    { "JOY_HAT_LEFTDOWN"    , TYPE_DWORD, SDL_HAT_LEFTDOWN  },
    { NULL                  , 0         , 0                 }
} ;

/* --------------------------------------------------------------------------- */

void  __bgdexport( libjoy, module_initialize )()
{
    int i;

    if ( !SDL_WasInit( SDL_INIT_JOYSTICK ) )
    {
        SDL_InitSubSystem( SDL_INIT_JOYSTICK );
        SDL_JoystickEventState( SDL_ENABLE ) ;
    }

    /* Open all joysticks */
    if (( _max_joys = SDL_NumJoysticks() ) > MAX_JOYS )
    {
        printf( "[JOY] Warning: maximum number of joysticks exceeded (%i>%i)", _max_joys, MAX_JOYS );
        _max_joys = MAX_JOYS;
    }

    for ( i = 0; i < _max_joys; i++ )
    {
        _joysticks[i] = SDL_JoystickOpen( i ) ;
        if ( !_joysticks[ i ] ) printf( "[JOY] Failed to open joystick '%i'", i );
    }

    SDL_JoystickUpdate() ;

#ifdef TARGET_CAANOO
    KIONIX_ACCEL_init();

    if ( KIONIX_ACCEL_get_device_type() != DEVICE_TYPE_KIONIX_KXTF9 ) KIONIX_ACCEL_deinit();

    KXTF9_set_G_range(2);
    KXTF9_set_resolution(12);
    KXTF9_set_lpf_odr(400);

    KIONIX_ACCEL_enable_outputs();
#endif
}

/* ----------------------------------------------------------------- */

void  __bgdexport( libjoy, module_finalize )()
{
    int i;

#ifdef TARGET_CAANOO
    KIONIX_ACCEL_deinit();
#endif

    for ( i = 0; i < _max_joys; i++ )
        if ( _joysticks[ i ] ) SDL_JoystickClose( _joysticks[ i ] ) ;

    if ( SDL_WasInit( SDL_INIT_JOYSTICK ) ) SDL_QuitSubSystem( SDL_INIT_JOYSTICK );

}

/* ----------------------------------------------------------------- */

char * __bgdexport( libjoy, modules_dependency )[] =
{
    "libsdlhandler",
    NULL
};

/* ----------------------------------------------------------------- */
