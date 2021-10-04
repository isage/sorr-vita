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

#include <loadlib.h> /* Must be fist include */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "bgdrtm.h"
#include "dcb.h"
#include "xstrings.h"

#include <bgddl.h>

/* ---------------------------------------------------------------------- */

/* Debe existir un header bgdrtm.h */
extern int bgd_copy_struct( INSTANCE * my, int * params ) ;
extern int bgd_internal_memcopy( INSTANCE * my, int * params ) ;
extern int bgd_internal_copy_string_array( INSTANCE * my, int * params ) ;

#include "sysprocs.h"
#include "sysprocs_p.h"

/* ---------------------------------------------------------------------- */

HOOK * handler_hook_list = NULL ;
int handler_hook_allocated = 0 ;
int handler_hook_count = 0 ;

INSTANCE_HOOK * instance_create_hook_list = NULL ;
int instance_create_hook_allocated = 0 ;
int instance_create_hook_count = 0 ;

INSTANCE_HOOK * instance_destroy_hook_list = NULL ;
int instance_destroy_hook_allocated = 0 ;
int instance_destroy_hook_count = 0 ;

INSTANCE_HOOK * instance_pre_execute_hook_list = NULL ;
int instance_pre_execute_hook_allocated = 0 ;
int instance_pre_execute_hook_count = 0 ;

INSTANCE_HOOK * instance_pos_execute_hook_list = NULL ;
int instance_pos_execute_hook_allocated = 0 ;
int instance_pos_execute_hook_count = 0 ;

INSTANCE_HOOK * process_exec_hook_list = NULL ;
int process_exec_hook_allocated = 0 ;
int process_exec_hook_count = 0 ;

FN_HOOK * module_initialize_list = NULL ;
int module_initialize_allocated = 0 ;
int module_initialize_count = 0 ;

FN_HOOK * module_finalize_list = NULL ;
int module_finalize_allocated = 0 ;
int module_finalize_count = 0 ;

/* ---------------------------------------------------------------------- */

#define hook_add(new_hook, hook_list, hook_allocated, hook_count) \
    { \
        if ((hook_count) >= (hook_allocated)) { \
            (hook_allocated) = (hook_count) + 5; \
            (hook_list) = realloc (hook_list, (hook_allocated) * sizeof((hook_list)[0])); \
        } \
        \
        if (!(hook_list)) { \
            fprintf (stderr, "No memory for alloc hook\n"); \
            exit(-1); \
        } \
        (hook_list)[hook_count] = (new_hook); \
        (hook_count)++; \
    }

/* ---------------------------------------------------------------------- */

static SYSPROC ** sysproc_tab = NULL ;

/* ---------------------------------------------------------------------- */

static int tsize( DCB_TYPEDEF orig );
static DCB_TYPEDEF treduce( DCB_TYPEDEF orig );

/* ---------------------------------------------------------------------- */

/* Tipos de token */
static struct
{
    enum { NOTOKEN, IDENTIFIER, NUMBER } type ;
    char name[128] ;
    int  code ;
} token ;

static const char * token_ptr = NULL;

/* ---------------------------------------------------------------------- */

/* Very simple tokenizer */

static void get_token()
{
    char * ptr ;
    unsigned int n ;

    while ( isspace( *token_ptr ) ) token_ptr++ ;

    if ( !*token_ptr )
    {
        token.type = NOTOKEN ;
        return ;
    }

    /* Numbers */

    if ( ISNUM( *token_ptr ) )
    {
        int num = 0 ;

        /* Calculate the number value */

        while ( ISNUM( *token_ptr ) ) num = num * 10 + ( *token_ptr++ - '0' ) ;
        token.type = NUMBER ;
        token.code = num ;
        snprintf( token.name, sizeof( token.name ), "%d", token.code ) ;
        return ;
    }

    ptr = token.name ;
    *ptr++ = TOUPPER( *token_ptr ) ;
    if ( ISWORDCHAR( *token_ptr++ ) )
        while ( ISWORDCHAR( *token_ptr ) ) *ptr++ = TOUPPER( *token_ptr++ ) ;
    *ptr = 0 ;

    for ( n = 0; n < dcb.data.NID; n++ )
    {
        if ( strcmp( (const char *)dcb.id[n].Name, token.name ) == 0 )
        {
            token.type = IDENTIFIER ;
            token.code = dcb.id[n].Code ;
            strcpy( token.name, (char *)dcb.id[n].Name ) ;
            return ;
        }
    }

    token.type = NOTOKEN ;
}

/* ---------------------------------------------------------------------- */

static DCB_TYPEDEF treduce( DCB_TYPEDEF orig )
{
    int n ;
    for ( n = 0; n < MAX_TYPECHUNKS - 1; n++ )
    {
        orig.BaseType[n] = orig.BaseType[n+1] ;
        orig.Count[n] = orig.Count[n+1] ;
    }
    return orig ;
}

/* ---------------------------------------------------------------------- */

static int tsize( DCB_TYPEDEF orig )
{
    unsigned int n, total ;

    switch ( orig.BaseType[0] )
    {
        case TYPE_ARRAY:
            return orig.Count[0] * tsize( treduce( orig ) ) ;

        case TYPE_POINTER:
        case TYPE_STRING:
        case TYPE_DWORD:
        case TYPE_FLOAT:
        case TYPE_INT:
            return 4 ;

        case TYPE_WORD:
        case TYPE_SHORT:
            return 2 ;

        case TYPE_BYTE:
        case TYPE_SBYTE:
        case TYPE_CHAR:
            return 1 ;

        case TYPE_STRUCT:
            total = 0 ;
            for ( n = 0; n < dcb.varspace[orig.Members].NVars; n++ )
                total += tsize( dcb.varspace_vars[orig.Members][n].Type ) ;
            return total ;

        default:
            return 0 ;
    }
}

/* ---------------------------------------------------------------------- */

static void get_var_info( DLVARFIXUP * varfixup, DCB_VAR * basevar, int nvars, char * basedata )
{
    unsigned int n ;
    DCB_VAR rvar ;
    void * rdata = NULL ;

    token_ptr = varfixup->var ;

    /* for Error */
    varfixup->data_offset = NULL;
    varfixup->size = -1;
    varfixup->elements = -1;

    get_token() ;
    if ( token.type != IDENTIFIER ) return ; /* Not a valid expression */

    /* Busca variable */
    for ( n = 0; n < nvars; n++ )
    {
        if ( basevar[n].ID == token.code )
        {
            rvar  = basevar[n] ;
            rdata = ( uint8_t* )basedata + basevar[n].Offset ;

            get_token() ;
            break;
        }
    }

    if ( n == nvars ) return ; /* Error, no es variable */

    for ( ;; )
    {
        if ( token.name[0] == '.' )
        {
            DCB_VARSPACE * v ;
            DCB_VAR * var ;

            if ( rvar.Type.BaseType[0] != TYPE_STRUCT ) return ; /* not an struct */

            get_token() ;
            if ( token.type != IDENTIFIER ) return ; /* not a member */

            v = &dcb.varspace[rvar.Type.Members] ;
            var = dcb.varspace_vars[rvar.Type.Members] ;
            for ( n = 0; n < v->NVars; n++ ) if ( var[n].ID == token.code ) break ;

            if ( n == v->NVars ) return ; /* not a member */

            rvar = var[n] ;
            rdata = ((uint8_t *)rdata) + var[n].Offset ;

            get_token() ;
            continue ;
        }

        if ( token.name[0] == '[' )
        {
            int index;

            if ( rvar.Type.BaseType[0] != TYPE_ARRAY ) return ; /* not an array */

            get_token() ;
            if ( token.type != NUMBER ) return ; /* not an integer */
            index = token.code;

            if ( index < 0 ) return ; /* Index less than zero */
            if ( index >= rvar.Type.Count[0] ) return ; /* Index out of bounds */

            rvar.Type = treduce( rvar.Type ) ;
            rdata = ((uint8_t *) rdata ) + index * tsize( rvar.Type ) ;

            get_token() ;
            if ( token.name[0] == ']' ) get_token() ; /* Skip ] */

            continue ;
        }

        varfixup->data_offset = ( void * ) rdata;
        varfixup->elements = rvar.Type.BaseType[0] == TYPE_ARRAY ? rvar.Type.Count[0] : 1;
        varfixup->size = tsize( rvar.Type ) / varfixup->elements;
/*
        printf ("varfixup: %p var: %s offset: %p elements: %d size: %d\n", varfixup, varfixup->var, varfixup->data_offset, varfixup->elements, varfixup->size);
*/
        break ;
    }
}

/* ---------------------------------------------------------------------- */

int compare_priority( const HOOK * a1, const HOOK * a2 )
{
    return ( a2->prio - a1->prio ) ;
}

/* ---------------------------------------------------------------------- */

int sysproc_add( char * name, char * paramtypes, int type, void * func )
{
    static SYSPROC * sysproc_new = 0 ;
    static int sysproc_count = 0 ;

    if ( !sysproc_new )
    {
        sysproc_new = sysprocs ;
        while ( sysproc_new->name )
        {
            sysproc_new++ ;
            sysproc_count++ ;
        }
    }

    if ( sysproc_count >= MAX_SYSPROCS )
    {
        fprintf( stderr, "ERROR: Runtime error - too many system functions\n" ) ;
        return -1;
    }

    sysproc_new->code = -1 ; /* Se llena en el fixup */
    sysproc_new->name = name ;
    sysproc_new->paramtypes = paramtypes ;
    sysproc_new->params = strlen( paramtypes ) ;
    sysproc_new->type = type ;
    sysproc_new->func = ( SYSFUNC * ) func ;
    sysproc_new->id = getid( name ) ;

    sysproc_new++ ;
    sysproc_count++ ;

    sysproc_new->func = NULL ;

    return 0 /*sysproc_new->code*/ ;
}

/* ---------------------------------------------------------------------- */

SYSPROC * sysproc_get( int code )
{
    return sysproc_tab[code] ;
}

/* ---------------------------------------------------------------------- */

void sysproc_add_tab( DLSYSFUNCS * functions_exports )
{
    if ( functions_exports )
    {
        while ( functions_exports->name )
        {
            sysproc_add( functions_exports->name, functions_exports->paramtypes, functions_exports->type, functions_exports->func );
            functions_exports++;
        }
    }
}

/* ---------------------------------------------------------------------- */

static char * dlsearchpath[] =
{
    "modules",
    "mod",
    "mods",
    "extensions",
    "plugins",
    "data",
    NULL
};

/* ---------------------------------------------------------------------- */

void sysproc_init()
{
    SYSPROC       * proc = sysprocs ;
    void          * library = NULL ;
    const char    * filename ;
    unsigned int    n ;

    DLVARFIXUP    * globals_fixup = NULL ;
    DLVARFIXUP    * locals_fixup = NULL ;
    DLSYSFUNCS    * functions_exports = NULL ;
    FN_HOOK         module_initialize ;
    FN_HOOK         module_finalize ;
    INSTANCE_HOOK   instance_create_hook ;
    INSTANCE_HOOK   instance_destroy_hook ;
    INSTANCE_HOOK   instance_pre_execute_hook ;
    INSTANCE_HOOK   instance_pos_execute_hook ;
    INSTANCE_HOOK   process_exec_hook ;
    HOOK          * handler_hooks = NULL ;

    int             maxcode = 0 ;

    char soname[ __MAX_PATH ], fullsoname[ __MAX_PATH ], **spath ;

#if defined( __MONOLITHIC__ )
#define DLLEXT      ".fakelib"
#define SIZEDLLEXT  8
#elif defined( WIN32 )
#define DLLEXT      ".dll"
#define SIZEDLLEXT  4
#elif defined(TARGET_MAC)
#define DLLEXT      ".dylib"
#define SIZEDLLEXT  6
#else
#define DLLEXT      ".so"
#define SIZEDLLEXT  3
#endif

    for ( n = 0; n < dcb.data.NImports; n++ )
    {
        filename = string_get( dcb.imports[n] ) ;

        snprintf( soname, __MAX_PATH, "%s" DLLEXT, filename );

        filename = soname ;

        /* Load library */

        if ( debug ) printf ("Loading... %s\n", filename );

        fullsoname[0] = '\0';

        library = NULL;

        spath = dlsearchpath;
        while( !library && spath && *spath )
        {
            sprintf( fullsoname, "%s%s/%s", appexepath, *spath, filename );
            library  = dlibopen( fullsoname ) ;
            spath++;
        }

        if ( !library ) library  = dlibopen( filename ) ;

        if ( !library )
        {
            SDL_Log( "FATAL ERROR: Couldn't open library \"%s\", quitting. \"%s\"\n", filename, dliberror() ) ;
            exit( 0 );
        }

        globals_fixup     = ( DLVARFIXUP * ) _dlibaddr( library, "globals_fixup" ) ;
        locals_fixup      = ( DLVARFIXUP * ) _dlibaddr( library, "locals_fixup" ) ;
        functions_exports = ( DLSYSFUNCS * ) _dlibaddr( library, "functions_exports" ) ;

        module_initialize = ( FN_HOOK ) _dlibaddr( library, "module_initialize" ) ;
        module_finalize   = ( FN_HOOK ) _dlibaddr( library, "module_finalize" ) ;

        instance_create_hook       = ( INSTANCE_HOOK ) _dlibaddr( library, "instance_create_hook" ) ;
        instance_destroy_hook      = ( INSTANCE_HOOK ) _dlibaddr( library, "instance_destroy_hook" ) ;
        instance_pre_execute_hook  = ( INSTANCE_HOOK ) _dlibaddr( library, "instance_pre_execute_hook" ) ;
        instance_pos_execute_hook  = ( INSTANCE_HOOK ) _dlibaddr( library, "instance_pos_execute_hook" ) ;
        process_exec_hook          = ( INSTANCE_HOOK ) _dlibaddr( library, "process_exec_hook" ) ;

        handler_hooks = ( HOOK * ) _dlibaddr( library, "handler_hooks" ) ;

        /* Fixups */

        if ( globals_fixup )
        {
            while ( globals_fixup->var )
            {
                get_var_info( globals_fixup, dcb.glovar, dcb.data.NGloVars, globaldata );
                globals_fixup++;
            }
        }

        if ( locals_fixup )
        {
            while ( locals_fixup->var )
            {
                get_var_info( locals_fixup, dcb.locvar, dcb.data.NLocVars, NULL );
                locals_fixup++;
            }
        }

        sysproc_add_tab( functions_exports ) ;

        if ( module_initialize )
            hook_add( module_initialize, module_initialize_list, module_initialize_allocated, module_initialize_count ) ;

        if ( module_finalize )
            hook_add( module_finalize, module_finalize_list, module_finalize_allocated, module_finalize_count ) ;

        if ( instance_create_hook )
            hook_add( instance_create_hook, instance_create_hook_list, instance_create_hook_allocated, instance_create_hook_count ) ;

        if ( instance_destroy_hook )
            hook_add( instance_destroy_hook, instance_destroy_hook_list, instance_destroy_hook_allocated, instance_destroy_hook_count ) ;

        if ( instance_pre_execute_hook )
            hook_add( instance_pre_execute_hook, instance_pre_execute_hook_list, instance_pre_execute_hook_allocated, instance_pre_execute_hook_count ) ;

        if ( instance_pos_execute_hook )
            hook_add( instance_pos_execute_hook, instance_pos_execute_hook_list, instance_pos_execute_hook_allocated, instance_pos_execute_hook_count ) ;

        if ( process_exec_hook )
            hook_add( process_exec_hook, process_exec_hook_list, process_exec_hook_allocated, process_exec_hook_count ) ;

        while ( handler_hooks && handler_hooks->hook )
        {
            hook_add( *handler_hooks, handler_hook_list, handler_hook_allocated, handler_hook_count ) ;
            handler_hooks++;
        }
    }

    if ( debug ) printf ("\n");

    /* System Procs FixUp */

    sysprocs_fixup();

    proc = sysprocs ;
    while ( proc->func )
    {
        if ( maxcode < proc->code ) maxcode = proc->code ;
        proc++ ;
    }

    sysproc_tab = calloc( maxcode + 1 , sizeof( SYSPROC * ) );

    proc = sysprocs ;
    while ( proc->func )
    {
        if ( proc->code > -1 ) sysproc_tab[proc->code] = proc ;
        proc++ ;
    }

    /* Sort handler_hooks */
    if ( handler_hook_list )
        qsort( handler_hook_list, handler_hook_count, sizeof( handler_hook_list[0] ), ( int ( * )( const void *, const void * ) ) compare_priority ) ;

    /* Initialize all modules */
    if ( module_initialize_count )
        for ( n = 0; n < module_initialize_count; n++ )
            module_initialize_list[n]();
}

/* ---------------------------------------------------------------------- */

extern DCB_SYSPROC_CODE2 * sysproc_code_ref ;

/* ---------------------------------------------------------------------- */

char * sysproc_name( int code )
{
    DCB_SYSPROC_CODE2 * s = NULL ;
    int n;

    s = sysproc_code_ref ;
    for ( n = 0; n < dcb.data.NSysProcsCodes; n++, s++ )
        if ( s->Code == code ) return getid_name( s->Id ) ;

    return NULL ;
}

/* ---------------------------------------------------------------------- */
