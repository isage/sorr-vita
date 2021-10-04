/*
 *  Copyright � 2006-2012 SplinterGU (Fenix/Bennugd)
 *  Copyright � 2002-2006 Fenix Team (Fenix)
 *  Copyright � 1999-2002 Jos� Luis Cebri�n Pag�e (Fenix)
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
 * INCLUDES
 */

#ifdef _WIN32
#define  _WIN32_WINNT 0x0500
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "bgdi.h"
#include "bgdrtm.h"
#include "xstrings.h"
#include "dirs.h"

#if defined(TARGET_IOS)
#include <SDL.h>
#elif defined(TARGET_WII)
#include <SDL.h>
#include <fat.h>
#elif defined(TARGET_PSP)
#include "psp.h"
#endif

/* ---------------------------------------------------------------------- */

static char * dcb_exts[] = { ".dcb", ".dat", ".bin", NULL };

static int standalone  = 0;  /* 1 only if this is an standalone interpreter   */
static int embedded    = 0;  /* 1 only if this is a stub with an embedded DCB */

/* ---------------------------------------------------------------------- */

/*
 *  FUNCTION : main
 *
 *  Main function
 *
 *  PARAMS:
 *      INT n: ERROR LEVEL to return to OS
 *
 *  RETURN VALUE:
 *      No value
 *
 */


int _newlib_heap_size_user   = 168 * 1024 * 1024;

int main( int argc, char *argv[] )
{
#ifdef TARGET_VITA
  scePowerSetArmClockFrequency(444);
  scePowerSetGpuClockFrequency(222);
  SDL_setenv("VITA_DISABLE_TOUCH_FRONT", "1");
  SDL_setenv("VITA_DISABLE_TOUCH_BACK", "1");
#endif

    char * filename = NULL, dcbname[ __MAX_PATH ], *ptr ;
    int i, j, ret = -1;
    file * fp = NULL;
    INSTANCE * mainproc_running;
    dcb_signature dcb_signature;

#ifndef TARGET_VITA
    /* get my executable name */
    ptr = argv[0] + strlen( argv[0] );
    while ( ptr > argv[0] && ptr[-1] != '\\' && ptr[-1] != '/' ) ptr-- ;
    appexename = strdup( ptr );

    /* get executable full pathname  */
    appexefullpath = getfullpath( argv[0] );
    if ( ( !strchr( argv[0], '\\' ) && !strchr( argv[0], '/' ) ) && !file_exists( appexefullpath ) )
    {
        char *p = whereis( appexename );
        if ( p )
        {
            char * tmp = calloc( 1, strlen( p ) + strlen( appexename ) + 2 );
            free( appexefullpath );
            sprintf( tmp, "%s/%s", p, appexename );
            appexefullpath = getfullpath( tmp );
            free( tmp );
        }
    }

    /* get pathname of executable */
    ptr = strstr( appexefullpath, appexename );
    appexepath = calloc( 1, ptr - appexefullpath + 1 );
    strncpy( appexepath, appexefullpath, ptr - appexefullpath );


#ifdef _WIN32
    appname = calloc( 1, strlen( appexename ) - 3 );
    strncpy( appname, appexename, strlen( appexename ) - 4 );
#else
    appname = strdup( appexename );
#endif

#endif // TARGET_VITA

#ifdef TARGET_PSP
    PSP_HEAP_SIZE_MAX ();
    pspDebugScreenInit();
    SetupCallbacks();
#endif

#ifdef TARGET_PSP
    standalone = ( strncmpi( appexename, "EBOOT", 4 ) == 0 ) ;
#else
#ifndef TARGET_VITA
    standalone = ( strncmpi( appexename, "bgdi", 4 ) == 0 ) ;
#endif
#endif

#ifdef TARGET_VITA
    appexepath = calloc( 1, 7 );
    strncpy( appexepath, "app0:/" , 7 );
    standalone = 1;
#endif

#ifdef TARGET_VITA
#else
    /* add binary path */
    file_addp( appexepath );
#endif

#ifdef TARGET_WII
    // Initialize the Wii FAT filesystem, check stuff
    if (!fatInitDefault()) {
        printf("Sorry, I cannot access the FAT filesystem on your card :(\n");
        exit(1);
    }
#endif

    if ( !standalone )
    {
        /* Hand-made interpreter: search for DCB at EOF */
        fp = file_open( argv[0], "rb0" );
        if ( fp )
        {
            file_seek( fp, -( int )sizeof( dcb_signature ), SEEK_END );
            file_read( fp, &dcb_signature, sizeof( dcb_signature ) );

            if ( strcmp( dcb_signature.magic, DCB_STUB_MAGIC ) == 0 )
            {
                ARRANGE_DWORD( &dcb_signature.dcb_offset );
                embedded = 1;
            }
        }

        filename = appname;
    }

    if ( standalone )
    {
        /* Calling BGDI.EXE so we must get all command line params */
#ifdef TARGET_PSP
    	filename="EBOOT.dcb";
    	glob_t globbuf;
    	glob("*.dcb", 0, NULL, &globbuf);
    	if (globbuf.gl_pathc>0){
    		filename=globbuf.gl_pathv[0];
    		filename[strlen(filename)-4]=0;
    	}
#endif

#ifdef TARGET_VITA
    file_addp( "ux0:/data/sorr/" );
    file_addp( "app0:/" );
    filename = "sorr.dat";
#endif



#ifndef TARGET_VITA
        for ( i = 1 ; i < argc ; i++ )
        {
            if ( argv[i][0] == '-' )
            {
                j = 1 ;
                while ( argv[i][j] )
                {
                    if ( argv[i][j] == 'd' ) debug = 1 ;
                    if ( argv[i][j] == 'i' )
                    {
                        if ( argv[i][j+1] == 0 )
                        {
                            if ( i == argc - 1 )
                            {
                                fprintf( stderr, "You must provide a directory" ) ;
                                exit( 0 );
                            }
                            file_addp( argv[i+1] );
                            i++ ;
                            break ;
                        }
                        file_addp( &argv[i][j + 1] ) ;
                        break ;
                    }
                    j++ ;
                }
            }
            else
            {
                if ( !filename )
                {
                    filename = argv[i] ;
                    if ( i < argc - 1 ) memmove( &argv[i], &argv[i+1], sizeof( char* ) * ( argc - i - 1 ) ) ;
                    argc-- ;
                    i-- ;
                }
            }
        }
#endif
        if ( !filename )
        {
            printf( BGDI_VERSION "\n"
                    "Copyright (c) 2006-2012 SplinterGU (Fenix/BennuGD)\n"
                    "Copyright (c) 2002-2006 Fenix Team (Fenix)\n"
                    "Copyright (c) 1999-2002 José Luis Cebrián Pagüe (Fenix)\n"
                    "Bennu Game Development comes with ABSOLUTELY NO WARRANTY;\n"
                    "see COPYING for details\n\n"
                    "Usage: %s [options] <data code block file>[.dcb]\n\n"
                    "   -d       Activate DEBUG mode\n"
                    "   -i dir   Adds the directory to the PATH\n"
                    "\n"
                    "This software is provided 'as-is', without any express or implied\n"
                    "warranty. In no event will the authors be held liable for any damages\n"
                    "arising from the use of this software.\n"
                    "\n"
                    "Permission is granted to anyone to use this software for any purpose,\n"
                    "including commercial applications, and to alter it and redistribute it\n"
                    "freely, subject to the following restrictions:\n"
                    "\n"
                    "   1. The origin of this software must not be misrepresented; you must not\n"
                    "   claim that you wrote the original software. If you use this software\n"
                    "   in a product, an acknowledgment in the product documentation would be\n"
                    "   appreciated but is not required.\n"
                    "\n"
                    "   2. Altered source versions must be plainly marked as such, and must not be\n"
                    "   misrepresented as being the original software.\n"
                    "\n"
                    "   3. This notice may not be removed or altered from any source\n"
                    "   distribution.\n",
                    argv[0] ) ;
            return -1 ;
        }
    }

    /* Initialization (modules needed before dcb_load) */

    string_init() ;
    init_c_type() ;

    /* Init application title for windowed modes */

    strcpy( dcbname, filename ) ;

    if ( appname && filename != appname )
    {
        free( appname );
        appname = strdup( filename ) ;
    }

    if ( !embedded )
    {
        /* First try to load directly (we expect myfile.dcb) */
        if ( !dcb_load( dcbname ) )
        {
            char ** dcbext = dcb_exts;
            int dcbloaded = 0;

            while ( dcbext && *dcbext )
            {
                strcpy( dcbname, filename ) ;
                strcat( dcbname, *dcbext ) ;
                if (( dcbloaded = dcb_load( dcbname ) ) ) break;
                dcbext++;
            }

            if ( !dcbloaded )
            {
                printf( "%s: doesn't exist or isn't version %d DCB compatible\n", filename, DCB_VERSION >> 8 ) ;
                return -1 ;
            }
        }
    }
    else
    {
        dcb_load_from( fp, dcbname, dcb_signature.dcb_offset );
    }

    /* If the dcb is not in debug mode */

    if ( dcb.data.NSourceFiles == 0 ) debug = 0;

    /* Initialization (modules needed after dcb_load) */

    sysproc_init() ;

#ifdef _WIN32
    HWND hWnd = /*GetForegroundWindow()*/ GetConsoleWindow();
    DWORD dwProcessId;
    GetWindowThreadProcessId( hWnd, &dwProcessId );
    if ( dwProcessId == GetCurrentProcessId() ) ShowWindow( hWnd, SW_HIDE );
#endif

    argv[0] = filename;
    bgdrtm_entry( argc, argv );

    if ( mainproc )
    {
        mainproc_running = instance_new( mainproc, NULL ) ;
        ret = instance_go_all() ;
    }

#ifdef TARGET_PSP
    sceKernelExitGame();
#endif

    bgdrtm_exit( ret );

    free( appexename        );
    free( appexepath        );
    free( appexefullpath    );
    free( appname           );

    return ret;
}

