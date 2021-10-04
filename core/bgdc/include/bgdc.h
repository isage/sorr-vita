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
 * FILE        : bgdc.h
 * DESCRIPTION : Base includes in compiler and some vars and const defines
 *
 * HISTORY:
 *
 */

#ifndef ENGLISH
#define ENGLISH
#endif

/*
 *  VERSION
 */

#define BGDC_VERSION     "BGDC " VERSION " (" __DATE__ " " __TIME__ ")"

/*
 * INSTEAD OF JUST EXITING, GIVE THE USER SOME GRACE TIME TO READ THE ERRORS
 */
#ifdef TARGET_WII
#define exit( retval ) printf("\nQUITTING in 5 seconds.\n"); sleep(5); exit(retval);
#endif

#ifdef TARGET_MAC
#define _printf(...) printf(__VA_ARGS__)
#endif

/*
 *  HEADER FILES
 */

#include <messages.h>
#include <commons_defs.h>

#include "files.h"
#include "xctype.h"

/* ---------------------------------------------------------------------- */
/* Módulos generales de mantenimiento de datos                            */
/* ---------------------------------------------------------------------- */

#include "typedef.h"
#include "constants.h"
#include "identifiers.h"
#include "xstrings.h"

/* ---------------------------------------------------------------------- */
/* Trucos de portabilidad                                                 */
/* ---------------------------------------------------------------------- */

#include "arrange.h"

/* ---------------------------------------------------------------------- */
/* Compilador                                                             */
/* ---------------------------------------------------------------------- */

#include "segment.h"
#include "varspace.h"
#include "token.h"
#include "codeblock.h"
#include "procdef.h"
#include "compiler.h"

extern char * main_path ;

extern char * appexename ;
extern char * appexepath ;
extern char * appexefullpath ;

extern int autoinclude ;    /* Incluye ficheros en el DCB automáticamente */
extern int imports[] ;      /* Códigos de cadena con nombres de imports */
extern int nimports ;       /* Número de imports */
extern int libmode ;

extern char langinfo[64] ;  /* language setting */

extern int no_include_this_file ;

extern int debug;

/* Funciones para guardar y cargar un fichero DCB */

#include "dcb.h"

extern void dcb_add_file (const char * filename) ;
extern int  dcb_save (const char * filename, int options, const char * stubname) ;
extern void dcb_settype (DCB_TYPEDEF * d, TYPEDEF * t) ;

/* Funciones del sistema (no definidas) */

typedef struct _sysproc
{
    int    code ;
    char * name ;
    char * paramtypes ;
    BASETYPE type ;
    int    params ;
    int    id ;

    /* For sysproc_list */
    struct _sysproc * next;
}
SYSPROC ;

extern int         sysproc_add    (char * name, char * paramtypes, int type, void * func);
extern SYSPROC *   sysproc_get    (int id) ;
extern SYSPROC * * sysproc_getall (int id) ;
extern char    *   sysproc_name   (int code) ;
extern void        sysproc_init() ;

extern void compile_warning( int, const char *fmt, ... );
extern void compile_error( const char *fmt, ... );

/* Constantes */

extern void core_init() ;

#include "offsets.h"
#include "pslang.h"

