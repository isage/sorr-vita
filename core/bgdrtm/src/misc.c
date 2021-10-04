/*
 *  Copyright © 2006-2012 SplinterGU (Fenix/Bennugd)
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

/* --------------------------------------------------------------------------- */
/*
 * INCLUDES
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include "bgdrtm.h"
#include "offsets.h"
#include "dcb.h"
#include "sysprocs_p.h"
#include "xstrings.h"

#include "fmath.h"

#if defined(TARGET_GP2X_WIZ) || defined(TARGET_CAANOO)
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <fcntl.h>

    #include <sys/mman.h>
#endif

/* --------------------------------------------------------------------------- */

char * appname          = NULL;
char * appexename       = NULL;
char * appexepath       = NULL;
char * appexefullpath   = NULL;

/* --------------------------------------------------------------------------- */

int debug = 0;  /* 1 if running in debug mode      */

/* --------------------------------------------------------------------------- */

/* os versions */
#ifdef _WIN32
#define _OS_ID          OS_WIN32
#endif

#ifdef TARGET_LINUX
#define _OS_ID          OS_LINUX
#endif

#ifdef TARGET_BEOS
#define _OS_ID          OS_BEOS
#endif

#ifdef TARGET_MAC
#define _OS_ID          OS_MACOS
#endif

#ifdef TARGET_GP32
#define _OS_ID          OS_GP32
#endif

#ifdef TARGET_DC
#define _OS_ID          OS_DC
#endif

#ifdef TARGET_BSD
#define _OS_ID          OS_BSD
#endif

#ifdef TARGET_GP2X
#ifdef _OS_ID
#undef _OS_ID
#endif
#define _OS_ID          OS_GP2X
#endif

#ifdef TARGET_GP2X_WIZ
#ifdef _OS_ID
#undef _OS_ID
#endif
#define _OS_ID          OS_GP2X_WIZ
#endif

#ifdef TARGET_CAANOO
#ifdef _OS_ID
#undef _OS_ID
#endif
#define _OS_ID          OS_CAANOO
#endif

#ifdef TARGET_DINGUX_A320
#ifdef _OS_ID
#undef _OS_ID
#endif
#define _OS_ID          OS_DINGUX_A320
#endif

#ifdef TARGET_WII
#ifdef _OS_ID
#undef _OS_ID
#endif
#define _OS_ID          OS_WII
#endif

#ifdef TARGET_PSP
#ifdef _OS_ID
#undef _OS_ID
#endif
#define _OS_ID          OS_PSP
#endif

#ifdef TARGET_VITA
#ifdef _OS_ID
#undef _OS_ID
#endif
#define _OS_ID          OS_VITA
#endif

#ifdef TARGET_IOS
#ifdef _OS_ID
#undef _OS_ID
#endif
#define _OS_ID          OS_IOS
#endif

#ifdef TARGET_ANDROID
#ifdef _OS_ID
#undef _OS_ID
#endif
#define _OS_ID          OS_ANDROID
#endif

/* --------------------------------------------------------------------------- */

#if defined(TARGET_GP2X_WIZ) || defined(TARGET_CAANOO)

#define TIMER_BASE3 0x1980
#define TIMER_REG(x) __bgdrtm_memregl[(TIMER_BASE3 + x) >> 2]

volatile unsigned long *__bgdrtm_memregl = NULL;
int __bgdrtm_memdev = -1;

#ifdef TARGET_CAANOO
static unsigned long caanoo_firmware_version = 0;
#endif

void bgdrtm_ptimer_init(void)
{
#if defined(TARGET_GP2X_WIZ)
    TIMER_REG(0x44) = 0x922;
#else
    if ( caanoo_firmware_version < 1000006 ) /* firmware version < 1.0.6 */
    {
        TIMER_REG(0x44) = 0x922;
    }
    else
    {
        TIMER_REG(0x44) = 0x0FF2;
    }
#endif
    TIMER_REG(0x40) = 0x0c;
    TIMER_REG(0x08) = 0x6b;
}

unsigned long bgdrtm_ptimer_get_ticks_us(void)
{
    TIMER_REG(0x08) = 0x4b;  /* run timer, latch value */
    return TIMER_REG(0);
}

void bgdrtm_ptimer_cleanup(void)
{
    TIMER_REG(0x40) = 0x0c;
    TIMER_REG(0x08) = 0x23;
    TIMER_REG(0x00) = 0;
    TIMER_REG(0x40) = 0;
    TIMER_REG(0x44) = 0;
}

#endif

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : strncmpi
 *
 *  Compare two strings (case-insensitive version)
 *
 *  PARAMS:
 *      str1    first string
 *      str2    second string
 *
 *  RETURN VALUE:
 *      Result of the comparison
 */

int strncmpi( char * str1, char * str2, int sz )
{
    while (( *str1 || *str2 ) && sz )
    {
#ifdef _WIN32
        if ( toupper( *str1 ) != toupper( *str2 ) ) return toupper( *str1 ) - toupper( *str2 );
#else
        if ( *str1 != *str2 ) return *str1 - *str2;
#endif
        str1++; str2++; sz--;
    }

    return 0 ;
}

/* --------------------------------------------------------------------------- */

void bgdrtm_entry( int argc, char * argv[] )
{
    int i;
    int * args = (int *)&GLODWORD( ARGV_TABLE );
    char * os_id;

    GLODWORD( ARGC ) = argc ;

    for ( i = 0 ; i < argc && i < 32; i++ )
    {
        args[i] = string_new( argv[i] ) ;
        string_use( args[i] ) ;
    }

    if ( ( os_id = getenv( "OS_ID" ) ) )
        GLODWORD( OS_ID ) = atol( os_id ) ;
    else
        GLODWORD( OS_ID ) = _OS_ID ;

#if defined(TARGET_GP2X_WIZ) || defined(TARGET_CAANOO)

#ifdef TARGET_CAANOO
    {
        FILE * fp = fopen( "/usr/gp2x/version", "r" );
        if ( fp )
        {
            char *p1, *p2;
            char b[32] = "";

            fgets( b, sizeof(b), fp );
            fclose( fp );

            if ( ( p2 = strchr( b, '.' ) ) )
            {
                *p2++ = '\0';
                caanoo_firmware_version = atol( b ) * 1000000L;
                if ( ( p1 = strchr( p2, '.' ) ) )
                {
                    *p1++ = '\0';
                    caanoo_firmware_version += atol( p2 ) * 1000L;
                    caanoo_firmware_version += atol( p1 );
                }
                else
                {
                    caanoo_firmware_version += atol( p2 ) * 1000L;
                }
            }
        }
    }
#endif

    __bgdrtm_memdev = open( "/dev/mem", O_RDWR );
    __bgdrtm_memregl = mmap( 0, 0x20000, PROT_READ|PROT_WRITE, MAP_SHARED, __bgdrtm_memdev, 0xc0000000 );

    bgdrtm_ptimer_init();
#endif

    init_cos_tables();
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : bgdrtm_exit
 *
 *  Exits from the program cleanly ending operations
 *
 *  PARAMS:
 *      INT n: ERROR LEVEL to return to OS
 *
 *  RETURN VALUE:
 *      No value
 *
 */

void bgdrtm_exit( int exit_value )
{
    int n;
    /* Finalize all modules */
    if ( module_finalize_count )
        for ( n = 0; n < module_finalize_count; n++ )
            module_finalize_list[n]();

#if defined(TARGET_GP2X_WIZ) || defined(TARGET_CAANOO)
    bgdrtm_ptimer_cleanup();

    __bgdrtm_memregl = munmap( 0, 0x20000 ); __bgdrtm_memregl = NULL;
    close( __bgdrtm_memdev ); __bgdrtm_memdev = -1;
#endif

    exit( exit_value ) ;
}

/* --------------------------------------------------------------------------- */
