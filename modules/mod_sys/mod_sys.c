/*
 *  Copyright � 2006-2012 SplinterGU (Fenix/Bennugd)
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "bgddl.h"
#include "files.h"
#include "xstrings.h"

#include <unistd.h>

#ifndef WIN32
#include <sys/wait.h>
#else
#include <process.h>
#endif

#if defined(TARGET_WII)
#include <ogc/wiilaunch.h>
#elif defined(TARGET_IOS)
#import <UIKit/UIKit.h>
#elif defined(TARGET_ANDROID)
#include <jni.h>
extern JNIEnv *Android_JNI_GetEnv();
extern jclass Android_JNI_GetActivityClass(void);
#endif

/* ---------------------------------------------------------------------- */

#ifndef _P_WAIT
#define _P_WAIT     0
#endif

#ifndef _P_NOWAIT
#define _P_NOWAIT   1
#endif

/*
#define _P_OVERLAY  2
#define _OLD_P_OVERLAY  _P_OVERLAY
#define _P_NOWAITO  3
#define _P_DETACH   4
*/

DLCONSTANT __bgdexport( mod_sys, constants_def )[] =
{
    { "_P_WAIT"     , TYPE_DWORD,  _P_WAIT   },
    { "_P_NOWAIT"   , TYPE_DWORD,  _P_NOWAIT },
    { NULL          , 0         ,  0         }
} ;

/* ---------------------------------------------------------------------- */

static int modsys_exec( INSTANCE * my, int * params )
{
#if defined(TARGET_PSP) || defined(TARGET_VITA)
    return 0;
#elif defined(TARGET_WII)
    WII_OpenURL(string_get(params[1]));
    string_discard(params[1]);
    return 0;
#elif defined(TARGET_IOS)
    NSString *urlString = [NSString stringWithFormat:@"%s" , string_get(params[1])];
    string_discard(params[1]);

    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:urlString]];
    return 0;
#elif defined(TARGET_ANDROID)
    jmethodID mid;
    jclass mActivityClass;

    JNIEnv *mEnv   = Android_JNI_GetEnv();
    mActivityClass = Android_JNI_GetActivityClass();
    mid = (*mEnv)->GetStaticMethodID(mEnv, mActivityClass, "openURL", "(Ljava/lang/String;)V");
    if (mid) {
        jstring Url = (*mEnv)->NewStringUTF(mEnv, string_get(params[1]));
        (*mEnv)->CallStaticVoidMethod(mEnv, mActivityClass, mid, Url);
        (*mEnv)->DeleteLocalRef(mEnv, Url);
    }
    string_discard(params[1]);
    return 0;
#else
    int mode = params[0];
    char * filename = ( char * ) string_get( params[1] );
    int argc = params[2];
    char ** argv;
    int n = 0;
#ifndef WIN32
    pid_t child;
#endif
    int status = -1;

    // fill argv
    argv = ( char ** ) calloc( argc + 2, sizeof( char * ) );
    argv[0] = filename;
    for ( n = 0; n < argc; n++ )
        argv[n + 1] = ( char * ) string_get((( int * )( params[3] ) )[n] );

    // Execute program
#ifdef WIN32
    status = spawnvp( mode, filename, ( const char ** )argv );
#else
    if (( child = fork() ) == -1 )
    {
        //Error
        status = -1 ;
    }
    else if ( child == 0 )
    {
        execvp( filename, ( const char ** )argv );
        exit(-1);
    }
    else
    {
        /* father */
        switch ( mode )
        {
            case _P_WAIT:
                if ( waitpid( child, &status, WUNTRACED ) != child )
                    status = -1;
                else
                    status = (int)(char)WEXITSTATUS(status);
                break;

            case _P_NOWAIT:
                status = child;
                break;
        }
    }
#endif

    // Free resources
    string_discard( params[1] );
    if ( argv ) free( argv );

    return ( status ) ;
#endif
}

/* ---------------------------------------------------------------------- */

static int modsys_getenv( INSTANCE * my, int * params )
{
    char *e ;
    int str ;

    if (( e = getenv( string_get( params[0] ) ) ) )
    {
        str = string_new( e ) ;
    }
    else
    {
        str = string_new( "" ) ;
    }

    string_discard( params[0] ) ;
    string_use( str ) ;
    return str ;
}

/* ----------------------------------------------------------------- */
/* Declaracion de funciones                                          */

DLSYSFUNCS __bgdexport( mod_sys, functions_exports )[] =
{
    { "GETENV"  , "S"    , TYPE_STRING, modsys_getenv },
    { "EXEC"    , "ISIP" , TYPE_INT   , modsys_exec   },
    { 0         , 0      , 0          , 0             }
};

/* ----------------------------------------------------------------- */
