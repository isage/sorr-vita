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

/*
 * FILE        : sysprocs.h
 * DESCRIPTION : Defines Internals functions SPECS and handlers
 **
 */

/*
 *  In interpreter: SYSMACRO returns a function pointer
 *  In compiler:    Already defined to return 0
 */

#ifndef SYSMACRO
#define SYSMACRO(a) a
#endif

#define MAX_SYSPROCS 3072

/* WARNING!!!!
   This table must be sorted by element Code !!!! Don't unsort it !!!
   2006/11/09    Splinter (jj_arg@yahoo.com)
*/

#ifdef SYSPROCS_ONLY_DECLARE
extern SYSPROC sysprocs[MAX_SYSPROCS] ;
#else
SYSPROC sysprocs[MAX_SYSPROCS] =
{
    /* Internal use */
    { 0x01, "#COPY#"        , "PV++I" , TYPE_POINTER    , 5, SYSMACRO(bgd_copy_struct)                  },
    { 0x02, "#MEMCOPY#"     , "PPI"   , TYPE_INT        , 3, SYSMACRO(bgd_internal_memcopy)             },
    { 0x03, "#COPYSTRA#"    , "PPI"   , TYPE_INT        , 3, SYSMACRO(bgd_internal_copy_string_array)   },

    { 0   , 0               , ""      , TYPE_UNDEFINED  , 0, 0                                          }
} ;
#endif
