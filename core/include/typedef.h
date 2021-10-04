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

#ifndef __TYPEDEF_H
#define __TYPEDEF_H

#include <typedef_st.h>

/* Tipos de dato */

extern TYPEDEF      typedef_new      (BASETYPE type) ;
extern TYPEDEF      typedef_enlarge  (TYPEDEF base) ;
extern TYPEDEF      typedef_reduce   (TYPEDEF base) ;
extern int          typedef_size     (TYPEDEF t) ;
extern int          typedef_subsize  (TYPEDEF t, int c) ;
extern void         typedef_describe (char * buffer, TYPEDEF t) ;
extern TYPEDEF      typedef_pointer  (TYPEDEF to) ;
extern int			typedef_tcount   (TYPEDEF t) ;
extern int			typedef_is_equal (TYPEDEF a, TYPEDEF b);

extern TYPEDEF    * typedef_by_name  (int code) ;
extern void         typedef_name     (TYPEDEF t, int code) ;

extern int			mntype			 (TYPEDEF type, int accept_structs);

#endif
