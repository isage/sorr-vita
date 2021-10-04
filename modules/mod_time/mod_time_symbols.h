/*
 *  Copyright © 2006-2012 SplinterGU (Fenix/Bennugd)
 *  Copyright © 2002-2006 Fenix Team (Fenix)
 *  Copyright © 1999-2002 José Luis Cebrián Pagüe (Fenix)
 *
 *  This file is part of Bennu - Game Development
 *
 *  Bennu is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Bennu is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 */

#ifndef __MODTIME_H
#define __MODTIME_H

#include <bgddl.h>

#ifdef __BGDC__
DLSYSFUNCS __bgdexport( mod_time, functions_exports )[] =
{
    /* Fecha/Hora */
    { "GET_TIMER"   , ""    , TYPE_INT      , 0 },
    { "TIME"        , ""    , TYPE_INT      , 0 },
    { "FTIME"       , "SI"  , TYPE_STRING   , 0 },
    { 0             , 0     , 0             , 0 }
};
#else
extern DLSYSFUNCS __bgdexport( mod_time, functions_exports )[];
extern void __bgdexport( mod_time, module_initialize )();
extern void __bgdexport( mod_time, module_finalize )();
#endif

#endif
