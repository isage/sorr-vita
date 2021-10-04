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

#ifndef __FILES_ST_H
#define __FILES_ST_H

#include <stdio.h>

/* Funciones de acceso a ficheros */
/* ------------------------------ */

/* Ahora mismo son casi wrappers de stdio.h, pero en el futuro
 * el tipo "file" puede ser una estructura y las funciones,
 * ofrecer soporte transparente para ficheros PAK, etc. */

#define F_XFILE  1
#define F_FILE   2
#define F_GZFILE 3
#define F_RWOPS  4

#ifndef NO_ZLIB
#include <zlib.h>
#endif

#ifdef WITH_SDLRWOPS
#include <SDL_rwops.h>
#endif

#ifdef _WIN32
#define __MAX_PATH          32768
#else
#define __MAX_PATH          4096
#endif

/*
#define __MAX_PATH          260
#define	__MAX_DRIVE	        (3)
#define	__MAX_DIR	        256
#define	__MAX_FNAME	        256
#define	__MAX_EXT	        256
*/

#ifdef WIN32
#define PATH_SEP "\\"
#define PATH_ISEP "/"
#define PATH_CHAR_SEP '\\'
#define PATH_CHAR_ISEP '/'
#define PATH_BACKSLASH
#else
#define PATH_SEP "/"
#define PATH_ISEP "\\"
#define PATH_CHAR_SEP '/'
#define PATH_CHAR_ISEP '\\'
#define PATH_SLASH
#endif


typedef struct
{
    int     type ;

    FILE *  fp ;
#ifndef NO_ZLIB
    gzFile  gz ;
#endif
#ifdef WITH_SDLRWOPS
    SDL_RWops * rwops ;
#endif
    int     n ;
    int     error ;
	char	name[__MAX_PATH];
	long    pos ;
	int     eof ;
}
file ;

#endif
