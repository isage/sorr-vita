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
 * FILE        : dirs.c
 * DESCRIPTION : directory functions
 *
 * HISTORY:
 *
 */

#include "bgdrtm.h"
#include "dirs.h"
#include "xstrings.h"

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* ------------------------------------------------------------------------------------ */
/*
 *  FUNCTION : dir_path_convert
 *
 *  Convert a path to the valid OS format
 *
 *  PARAMS :
 *      char * path:    path to convert
 *
 *  RETURN VALUE :
 *      char *:         converted path
 *
 */

char * dir_path_convert( const char * dir )
{
    char *c, *p ;

    p = strdup( dir ) ;
    if ( !p ) return NULL;

    c = p ;
    /* Convert characters */
    while ( *p )
    {
        if ( *p == PATH_CHAR_ISEP ) *p = PATH_CHAR_SEP ;
        p++ ;
    }
    return c;
}


/* ------------------------------------------------------------------------------------ */
/*
 *  FUNCTION : dir_current
 *
 *  Retrieve current directory
 *
 *  PARAMS :
 *
 *  RETURN VALUE :
 *      STRING ID pointing to a system string with the current dir
 *
 */

static const char * cur_path = "ux0:/data/sorr/";

char * dir_current( void )
{
    printf("dir current\n");
    return cur_path;
//    return ( getcwd( NULL, 0 ) ) ;
}

/* ------------------------------------------------------------------------------------ */
/*
 *  FUNCTION : dir_change
 *
 *  Retrieve current directory
 *
 *  PARAMS :
 *      char * dir:     the new current directory
 *
 *  RETURN VALUE :
 *      0           - FAILURE
 *      NON_ZERO    - SUCCESS
 *
 */

int dir_change( const char * dir )
{
    printf("dir change: %s\n", dir);
    return -1;
/*    char *c = dir_path_convert( dir ) ;
    if ( !c ) return 0;
    int r = chdir( c ) ;
    free( c ) ;
    return r ;*/
}

/* ------------------------------------------------------------------------------------ */
/*
 *  FUNCTION : dir_create
 *
 *  Retrieve current directory
 *
 *  PARAMS :
 *      char * dir:     the directory to create
 *
 *  RETURN VALUE :
 *      0           - FAILURE
 *      NON_ZERO    - SUCCESS
 *
 */

int dir_create( const char * dir )
{
    char *c = dir_path_convert( dir ) ;
    if ( !c ) return 0;
    char *path[256];
    sprintf(path, "ux0:/data/sorr/%s", c);
#ifdef WIN32
    int r = mkdir( c ) ;
#else
    int r = mkdir( path, 0777 ) ;
#endif
    free( c ) ;
    return r ;
}

/* ------------------------------------------------------------------------------------ */
/*
 *  FUNCTION : dir_delete
 *
 *  Retrieve current directory
 *
 *  PARAMS :
 *      char * dir:     the directory to delete
 *
 *  RETURN VALUE :
 *      0           - FAILURE
 *      NON_ZERO    - SUCCESS
 *
 */

int dir_delete( const char * dir )
{
    char *c = dir_path_convert( dir ) ;
    if ( !c ) return 0;
    char *path[256];
    sprintf(path, "ux0:/data/sorr/%s", c);
#ifdef TARGET_WII
    int r = remove( c );
#else
    int r = rmdir( path ) ;
#endif
    free( c ) ;
    return r ;
}

/* ------------------------------------------------------------------------------------ */
/*
 *  FUNCTION : dir_deletefile
 *
 *  Remove a given file
 *
 *  PARAMS :
 *      char * filename: the file to delete
 *
 *  RETURN VALUE :
 *      0           - FAILURE
 *      NON_ZERO    - SUCCESS
 *
 */

int dir_deletefile( const char * filename )
{
    char *c = dir_path_convert( filename ) ;
    if ( !c ) return 0;
    char *path[256];
    sprintf(path, "ux0:/data/sorr/%s", c);
    int r = unlink( path ) ;
    free( c ) ;
    return ( r == -1 ) ? 0 : 1 ;
}

/* ------------------------------------------------------------------------------------ */

__DIR_ST * dir_open( const char * path )
{
    printf("dir open\n");
    __DIR_ST * hDir = malloc( sizeof( __DIR_ST ) );
    if ( !hDir ) return NULL;

    hDir->path = strdup( path );
    if ( !hDir->path )
    {
        free ( hDir );
        return NULL;
    }

#ifdef _WIN32
    hDir->handle = FindFirstFile( hDir->path, &hDir->data );
    hDir->eod = ( hDir->handle == INVALID_HANDLE_VALUE );

    if ( !hDir->handle )
    {
        free( hDir->path );
        free( hDir );
        return NULL;
    }
#else
    const char * ptr = hDir->path;
    char * fptr;
    int r;

    hDir->pattern = malloc( strlen( path ) * 4 );
    if ( !hDir->pattern )
    {
        free ( hDir->path );
        free ( hDir );
        return NULL;
    }

    /* Clean the path creating a case-insensitive match pattern */

    fptr = hDir->pattern;
    while ( *ptr )
    {
        if ( *ptr == '\\' )
        {
            *fptr++ = '/';
        }
        else if ( *ptr >= 'a' && *ptr <= 'z' )
        {
            *fptr++ = '[';
            *fptr++ = *ptr;
            *fptr++ = toupper( *ptr );
            *fptr++ = ']';
        }
        else if ( *ptr >= 'A' && *ptr <= 'Z' )
        {
            *fptr++ = '[';
            *fptr++ = tolower( *ptr );
            *fptr++ = *ptr;
            *fptr++ = ']';
        }
        else
            *fptr++ = *ptr;
        ptr++;
    }
    *fptr = 0;

    /* Convert '*.*' to '*' */
    if ( fptr > hDir->pattern + 2 && fptr[ -1 ] == '*' && fptr[ -2 ] == '.' && fptr[ -3 ] == '*' ) fptr[ -2 ] = 0;

#if defined(TARGET_MAC) || defined(TARGET_BEOS) || defined(TARGET_WII) || defined(TARGET_PSP) || defined(TARGET_ANDROID) || defined(TARGET_VITA)
//    r = glob( hDir->pattern, GLOB_ERR | GLOB_NOSORT, NULL, &hDir->globd );
#else
    r = glob( hDir->pattern, GLOB_ERR | GLOB_PERIOD | GLOB_NOSORT, NULL, &hDir->globd );
#endif

    if (1)// r )
    {
        free( hDir->pattern );
        free( hDir->path );
        free( hDir );
        return NULL;
    }

    hDir->currFile = 0;
#endif

    return hDir;
}

/* ------------------------------------------------------------------------------------ */

void dir_close ( __DIR_ST * hDir )
{
    printf("dir close\n");

    free ( hDir->path );

#ifdef _WIN32
    FindClose( hDir->handle );
#else
//    globfree( &hDir->globd );
    free( hDir->pattern );
#endif

    free ( hDir );
}

/* ------------------------------------------------------------------------------------ */

__DIR_FILEINFO_ST * dir_read( __DIR_ST * hDir )
{
    printf("dir read\n");
    return NULL;
#if 0
    char realpath[__MAX_PATH];
    char * ptr ;

#ifdef _WIN32
    SYSTEMTIME time;

    if ( hDir->eod ) return NULL;

    /* Fill the FILEINFO struct */
    strcpy( realpath, hDir->path );
    ptr = realpath + strlen( realpath );
    while ( ptr >= realpath )
    {
        if ( *ptr == '\\' || *ptr == '/' )
        {
            ptr[ 1 ] = 0;
            break;
        }
        ptr--;
    }

    memset( &hDir->info, '\0', sizeof( hDir->info ) );

    strcat( realpath, hDir->data.cFileName );
    GetFullPathName( realpath, __MAX_PATH, hDir->info.fullpath, &ptr );
    if ( ptr ) * ptr = '\0';

    strcpy ( hDir->info.filename, hDir->data.cFileName );

    hDir->info.attributes    = (( hDir->data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) ? DIR_FI_ATTR_DIRECTORY : 0 ) |
                               (( hDir->data.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN    ) ? DIR_FI_ATTR_HIDDEN    : 0 ) |
                               (( hDir->data.dwFileAttributes & FILE_ATTRIBUTE_READONLY  ) ? DIR_FI_ATTR_READONLY  : 0 );

    hDir->info.size          = hDir->data.nFileSizeLow;

    /* Format and store the creation time */
    FileTimeToSystemTime( &hDir->data.ftCreationTime, &time );

    hDir->info.crtime.tm_sec    = time.wSecond;
    hDir->info.crtime.tm_min    = time.wMinute;
    hDir->info.crtime.tm_hour   = time.wHour;
    hDir->info.crtime.tm_mday   = time.wDay;
    hDir->info.crtime.tm_mon    = time.wMonth - 1;
    hDir->info.crtime.tm_year   = time.wYear - 1900;
    hDir->info.crtime.tm_wday   = time.wDayOfWeek;
    hDir->info.crtime.tm_yday   = time.wMonth;
    hDir->info.crtime.tm_isdst  = -1;

    /* Format and store the last write time */
    FileTimeToSystemTime( &hDir->data.ftLastWriteTime, &time );

    hDir->info.mtime.tm_sec     = time.wSecond;
    hDir->info.mtime.tm_min     = time.wMinute;
    hDir->info.mtime.tm_hour    = time.wHour;
    hDir->info.mtime.tm_mday    = time.wDay;
    hDir->info.mtime.tm_mon     = time.wMonth - 1;
    hDir->info.mtime.tm_year    = time.wYear - 1900;
    hDir->info.mtime.tm_wday    = time.wDayOfWeek;
    hDir->info.mtime.tm_yday    = time.wMonth;
    hDir->info.mtime.tm_isdst   = -1;

    /* Format and store the last access time */
    FileTimeToSystemTime( &hDir->data.ftLastAccessTime, &time );

    hDir->info.atime.tm_sec     = time.wSecond;
    hDir->info.atime.tm_min     = time.wMinute;
    hDir->info.atime.tm_hour    = time.wHour;
    hDir->info.atime.tm_mday    = time.wDay;
    hDir->info.atime.tm_mon     = time.wMonth - 1;
    hDir->info.atime.tm_year    = time.wYear - 1900;
    hDir->info.atime.tm_wday    = time.wDayOfWeek;
    hDir->info.atime.tm_yday    = time.wMonth;
    hDir->info.atime.tm_isdst   = -1;

    /* Continue last search */
    if (!FindNextFile( hDir->handle, &hDir->data )) hDir->eod = 1;
#else
    struct stat s;

    if ( hDir->currFile == hDir->globd.gl_pathc ) return NULL;

    memset( &hDir->info, '\0', sizeof( hDir->info ) );

    stat( hDir->globd.gl_pathv[ hDir->currFile ], &s );

    ptr = strrchr( hDir->globd.gl_pathv[ hDir->currFile ], '/' );
    if ( !ptr )
    {
        strcpy ( hDir->info.filename, hDir->globd.gl_pathv[ hDir->currFile ] );
//        strcpy ( hDir->info.fullpath, getcwd( realpath, sizeof( realpath ) ) );
    }
    else
    {
        strcpy ( hDir->info.filename, ptr + 1 );
        if ( hDir->globd.gl_pathv[ hDir->currFile ][0] == '/' )
        {
            strcpy ( hDir->info.fullpath, hDir->globd.gl_pathv[ hDir->currFile ] );
            hDir->info.fullpath[ ptr - hDir->globd.gl_pathv[ hDir->currFile ] + 1 ] = '\0';
        }
        else
        {
//            strcpy ( hDir->info.fullpath, getcwd( realpath, sizeof( realpath ) ) );
            strcat ( hDir->info.fullpath, "/" );
            strcat ( hDir->info.fullpath, hDir->globd.gl_pathv[ hDir->currFile ] );
            ptr = strrchr( hDir->info.fullpath, '/' );
            *(ptr + 1) = '\0';
        }
    }

    hDir->info.attributes    = (( S_ISDIR( s.st_mode )          ) ? DIR_FI_ATTR_DIRECTORY : 0 ) |
                               (( hDir->info.filename[0] == '.' ) ? DIR_FI_ATTR_HIDDEN    : 0 ) |
                               (( !( s.st_mode & 0444 )         ) ? DIR_FI_ATTR_READONLY  : 0 );

    hDir->info.size          = s.st_size;

    hDir->info.mtime    = *localtime( &s.st_mtime ) ;
    hDir->info.atime    = *localtime( &s.st_atime ) ;
    hDir->info.ctime    = *localtime( &s.st_ctime ) ;

    hDir->currFile++;

#endif

    return ( &hDir->info );
#endif
}

/* ------------------------------------------------------------------------------------ */
