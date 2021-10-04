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

#ifndef __INSTANCE_H
#define __INSTANCE_H

#include <stdint.h>
#include <instance_st.h>
#include "i_procdef_st.h"

/* Instancias. Una instancia se crea a partir de un proceso, pero en
 * realidad es independiente del proceso original */

#ifdef __BGDRTM__
extern void         * globaldata ;
extern void         * localdata ;
#endif

extern int          local_size ;

extern INSTANCE     * first_instance ;
extern INSTANCE     * last_instance ;

extern int          instance_getid() ;
extern INSTANCE     * instance_get( int id ) ;
extern INSTANCE     * instance_get_by_type( uint32_t type, INSTANCE ** context ) ;
extern INSTANCE     * instance_getfather( INSTANCE * i ) ;
extern INSTANCE     * instance_getson( INSTANCE * i ) ;
extern INSTANCE     * instance_getbigbro( INSTANCE * i ) ;
extern INSTANCE     * instance_getsmallbro( INSTANCE * i ) ;
extern INSTANCE     * instance_new( PROCDEF * proc, INSTANCE * father ) ;
extern INSTANCE     * instance_duplicate( INSTANCE * i ) ;
extern void         instance_destroy( INSTANCE * r ) ;
extern void         instance_dump( INSTANCE * father, int indent ) ;
extern void         instance_dump_all() ;
extern void         instance_posupdate( INSTANCE * i ) ;
extern int          instance_poschanged( INSTANCE * i ) ;
extern int          instance_exists( INSTANCE * i ) ;

extern INSTANCE     * instance_next_by_priority();
extern void         instance_dirty( INSTANCE * i ) ;

/* Las siguientes funciones son el punto de entrada del intérprete */

extern int          instance_go( INSTANCE * r ) ;
extern int          instance_go_all() ;

#endif
