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

#ifndef __RESOLUTION_H
#define __RESOLUTION_H

/* -------------------------------------------------------------------- */
/* Librería gráfica                                                     */
/* -------------------------------------------------------------------- */

#define RESOLXY(m,r,x,y) \
    {                                           \
        int res = LOCINT32(m, r, RESOLUTION ) ; \
        if ( res > 0 )                          \
        {                                       \
            if ( x < 0 )                        \
                ( x ) = ( ( x ) - ( res - 1 )) / res ; \
            else                                \
                ( x ) /= res ;                  \
                                                \
            if ( y < 0 )                        \
                ( y ) = ( ( y ) - ( res - 1 )) / res ; \
            else                                \
                ( y ) /= res ;                  \
        }                                       \
        else if ( res < 0 )                     \
        {                                       \
            ( x ) *= -res ;                     \
            ( y ) *= -res ;                     \
        }                                       \
    }

#define RESOLXYZ(m,r,x,y,z) \
    {                                           \
        int res = LOCINT32(m, r, RESOLUTION ) ; \
        if ( res > 0 )                          \
        {                                       \
            if ( x < 0 )                        \
                ( x ) = ( ( x ) - ( res - 1 )) / res ; \
            else                                \
                ( x ) /= res ;                  \
                                                \
            if ( y < 0 )                        \
                ( y ) = ( ( y ) - ( res - 1 )) / res ; \
            else                                \
                ( y ) /= res ;                  \
                                                \
            if ( z < 0 )                        \
                ( z ) = ( ( z ) - ( res - 1 )) / res ; \
            else                                \
                ( z ) /= res ;                  \
        }                                       \
        else if ( res < 0 )                     \
        {                                       \
            ( x ) *= -res ;                     \
            ( y ) *= -res ;                     \
            ( z ) *= -res ;                     \
        }                                       \
    }

#endif
