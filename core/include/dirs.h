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
 * FILE        : dirs.h
 * DESCRIPTION : Base include for directory functions
 *
 * HISTORY:
 *
 */

#ifndef __DIRS_H
#define __DIRS_H

#ifdef WIN32
#include <windows.h>
#include <winbase.h>
#include <windef.h>
#include <direct.h>
#else
#include <glob.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#include <errno.h>
#include <time.h>

#include "files_st.h"

/* ------------------------------------------------------------------------------------ */

enum
{
    DIR_FI_ATTR_NORMAL         = 0x00000000,
    DIR_FI_ATTR_DIRECTORY      = 0x00000001,
    DIR_FI_ATTR_HIDDEN         = 0x00000002,
    DIR_FI_ATTR_READONLY       = 0x00000004
};

/* ------------------------------------------------------------------------------------ */

typedef struct __DIR_FILEINFO_ST
{
    char fullpath[__MAX_PATH];
    char filename[__MAX_PATH];
    long attributes;
    long size;
    struct tm crtime; /* creation time (unix not available) */
    struct tm mtime; /* last modification time */
    struct tm atime; /* last access time */
    struct tm ctime; /* last status change time (windows not available) */
} __DIR_FILEINFO_ST;

typedef struct __DIR_ST
{
    char * path;
#ifdef _WIN32
    WIN32_FIND_DATA data;
    HANDLE handle;
    int eod;
#else
    glob_t globd;
    int currFile;
    char *pattern;
#endif
    __DIR_FILEINFO_ST info;
} __DIR_ST;

/* ------------------------------------------------------------------------------------ */

extern char *               dir_path_convert(const char *path) ;

extern char *	            dir_current(void) ;
extern int		            dir_change(const char *dir) ;
extern int		            dir_create(const char *dir) ;
extern int		            dir_delete(const char *dir) ;
extern int		            dir_deletefile(const char *filename) ;

extern __DIR_ST *           dir_open( const char * path );
extern void                 dir_close ( __DIR_ST * hDir );
extern __DIR_FILEINFO_ST *  dir_read( __DIR_ST * hDir );

#endif
