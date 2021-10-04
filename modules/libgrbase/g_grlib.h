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

#ifndef __GRLIB_H
#define __GRLIB_H

#define MAXLIBS 1024

typedef struct _grlib
{
    GRAPH ** maps ;
    int map_reserved ;
    char name[ 64 ];
}
GRLIB ;

extern GRLIB * syslib ;

extern GRAPH * bitmap_get( int libid, int mapcode ) ;
extern GRLIB * grlib_get( int libid ) ;
extern void grlib_init() ;
extern int grlib_new() ;
extern void grlib_destroy( int libid ) ;
extern int grlib_add_map( int libid, GRAPH * map ) ;
extern int grlib_unload_map( int libid, int mapcode ) ;

#endif
