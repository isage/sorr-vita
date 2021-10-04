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

#include <stdlib.h>

#include "bgddl.h"
#include "fmath.h"

/* ---------------------------------------------------------------------- */

static int rand_seed( INSTANCE * my, int * params )
{
    srand( params[0] ) ;
    return 1 ;
}

/* ---------------------------------------------------------------------- */

static int rand_std( INSTANCE * my, int * params )
{
    int num1 = MIN( params[0], params[1] ) ;
    int num2 = MAX( params[0], params[1] ) ;
    int var = num2 - num1 + 1;

    if ( var > RAND_MAX )
        return num1 + rand() * ((( double ) var ) / RAND_MAX );
    else
        return num1 + rand() % var;
}

/* ---------------------------------------------------------------------- */
/* Declaracion de funciones                                               */

DLSYSFUNCS  __bgdexport( mod_rand, functions_exports )[] =
{
    { "RAND_SEED"   , "I"   , TYPE_INT  , rand_seed     },
    { "RAND"        , "II"  , TYPE_INT  , rand_std      },
    { 0             , 0     , 0         , 0             }
};

/* ---------------------------------------------------------------------- */
