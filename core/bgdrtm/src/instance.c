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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef TARGET_BEOS
#include <posix/assert.h>
#else
#include <assert.h>
#endif

#include "offsets.h"
#include "bgdrtm.h"
#include "sysprocs_p.h"
#include "instance.h"
#include "xstrings.h"

#undef STACK_SIZE
#define STACK_SIZE 4096

/* ---------------------------------------------------------------------- */

#define INSTANCE_MIN_PRIORITY       -32768
#define INSTANCE_MAX_PRIORITY       32767
#define INSTANCE_NORMALIZE_PRIORITY 32768

/* ---------------------------------------------------------------------- */
/* Módulo de gestión de instancias, con las funciones de incialización y  */
/* destrucción, duplicado, etc.                                           */
/* ---------------------------------------------------------------------- */

#define HASH(id)            (unsigned int)((id)&0x0000ffff)
#define HASH_PRIORITY(id)   (unsigned int)(((id) + INSTANCE_NORMALIZE_PRIORITY) & 0x0000ffff)
#define HASH_INSTANCE(id)   (unsigned int)(((( uint32_t )(id)) >> 2 ) & 0x0000ffff)
#define HASH_SIZE           65536

INSTANCE ** hashed_by_id = NULL;
INSTANCE ** hashed_by_instance = NULL;
INSTANCE ** hashed_by_type = NULL;
INSTANCE ** hashed_by_priority = NULL;

INSTANCE * first_instance = NULL ;

/* Priority lists */

static INSTANCE * iterator_by_priority  = NULL ;
static int        iterator_pos          = HASH_SIZE;

static int instance_maxid =  FIRST_INSTANCE_ID ;

static int instance_min_actual_prio = INSTANCE_MAX_PRIORITY ;
static int instance_max_actual_prio = INSTANCE_MIN_PRIORITY ;

/* ---------------------------------------------------------------------- */
/* By id                                                                  */
/* ---------------------------------------------------------------------- */

void instance_add_to_list_by_id( INSTANCE * r, uint32_t id )
{
    if ( !hashed_by_id ) hashed_by_id = calloc( HASH_SIZE, sizeof( INSTANCE * ) );
    hashed_by_id[HASH( id )] = r;
}

/* ---------------------------------------------------------------------- */

void instance_remove_from_list_by_id( INSTANCE * r, uint32_t id )
{
    hashed_by_id[HASH( id )] = NULL;
}

/* ---------------------------------------------------------------------- */
/* By instance                                                            */
/* ---------------------------------------------------------------------- */

void instance_add_to_list_by_instance( INSTANCE * r )
{
    unsigned int hash = HASH_INSTANCE( r );

    if ( !hashed_by_instance ) hashed_by_instance = calloc( HASH_SIZE, sizeof( INSTANCE * ) );

    r->prev_by_instance = NULL ;
    r->next_by_instance = hashed_by_instance[hash];
    if ( r->next_by_instance ) r->next_by_instance->prev_by_instance = r ;
    hashed_by_instance[hash] = r;
}

/* ---------------------------------------------------------------------- */

void instance_remove_from_list_by_instance( INSTANCE * r )
{
    unsigned int hash = HASH_INSTANCE( r );

    if ( r->prev_by_instance ) r->prev_by_instance->next_by_instance = r->next_by_instance ;
    if ( r->next_by_instance ) r->next_by_instance->prev_by_instance = r->prev_by_instance ;

    if ( hashed_by_instance[hash] == r ) hashed_by_instance[hash] = r->next_by_instance ;
}

/* ---------------------------------------------------------------------- */
/* By type                                                                */
/* ---------------------------------------------------------------------- */

void instance_add_to_list_by_type( INSTANCE * r, uint32_t type )
{
    unsigned int hash = HASH( type );

    if ( !hashed_by_type ) hashed_by_type = calloc( HASH_SIZE, sizeof( INSTANCE * ) );

    r->prev_by_type = NULL ;
    r->next_by_type = hashed_by_type[hash];
    if ( r->next_by_type ) r->next_by_type->prev_by_type = r ;
    hashed_by_type[hash] = r;
}

/* ---------------------------------------------------------------------- */

void instance_remove_from_list_by_type( INSTANCE * r, uint32_t type )
{
    unsigned int hash = HASH( type );

    if ( r->prev_by_type ) r->prev_by_type->next_by_type = r->next_by_type ;
    if ( r->next_by_type ) r->next_by_type->prev_by_type = r->prev_by_type ;

    if ( hashed_by_type[hash] == r ) hashed_by_type[hash] = r->next_by_type ;
}

/* ---------------------------------------------------------------------- */
/* By priority                                                            */
/* ---------------------------------------------------------------------- */

void instance_add_to_list_by_priority( INSTANCE * r, int32_t priority )
{
    unsigned int hash ;

    if ( priority < INSTANCE_MIN_PRIORITY ) priority = LOCINT32( r, PRIORITY ) = INSTANCE_MIN_PRIORITY;
    if ( priority > INSTANCE_MAX_PRIORITY ) priority = LOCINT32( r, PRIORITY ) = INSTANCE_MAX_PRIORITY;

    hash = HASH_PRIORITY( priority );

    if ( !hashed_by_priority ) hashed_by_priority = calloc( HASH_SIZE, sizeof( INSTANCE * ) );

    r->prev_by_priority = NULL ;
    r->next_by_priority = hashed_by_priority[hash];
    if ( r->next_by_priority ) r->next_by_priority->prev_by_priority = r ;
    hashed_by_priority[hash] = r;
    r->last_priority = priority ;

    if ( priority < instance_min_actual_prio ) instance_min_actual_prio = priority ;
    if ( priority > instance_max_actual_prio ) instance_max_actual_prio = priority ;
}

/* ---------------------------------------------------------------------- */

void instance_remove_from_list_by_priority( INSTANCE * r )
{
    unsigned int hash = HASH_PRIORITY( r->last_priority );

    /* Update iterator_by_priority if necessary */

    if ( iterator_by_priority == r ) instance_next_by_priority() ;

    if ( r->prev_by_priority ) r->prev_by_priority->next_by_priority = r->next_by_priority ;
    if ( r->next_by_priority ) r->next_by_priority->prev_by_priority = r->prev_by_priority ;

    if ( hashed_by_priority[hash] == r ) hashed_by_priority[hash] = r->next_by_priority ;

    if ( !hashed_by_priority[hash] )
    {
        if ( r->last_priority == instance_min_actual_prio )
        {
            while ( instance_min_actual_prio < instance_max_actual_prio && !hashed_by_priority[ instance_min_actual_prio + INSTANCE_NORMALIZE_PRIORITY ] )
                    instance_min_actual_prio++;
        }

        if ( r->last_priority == instance_max_actual_prio )
        {
            while ( instance_max_actual_prio > instance_min_actual_prio && !hashed_by_priority[ instance_max_actual_prio + INSTANCE_NORMALIZE_PRIORITY ] )
                    instance_max_actual_prio--;
        }
    }
}

/* ---------------------------------------------------------------------- */

/*
 *  FUNCTION : instance_dirty
 *
 *  Adds an instance to the dirty instances list. This is a list of all
 *  instances which priority changed since the last execution.
 *
 *  PARAMS :
 *      i               Pointer to the instance
 *
 *  RETURN VALUE :
 *      None
 */

void instance_dirty( INSTANCE * i )
{
    instance_remove_from_list_by_priority( i );
    instance_add_to_list_by_priority( i, LOCINT32( i, PRIORITY ) );
}

/* ---------------------------------------------------------------------- */

/*
 *  FUNCTION : instance_get
 *
 *  Returns a instance, given its ID.
  *
 *  PARAMS :
 *      id              Integer ID of the instance
 *
 *  RETURN VALUE :
 *      Pointer to the found instance or NULL if not found
 */

INSTANCE * instance_get( int id )
{
    if ( !hashed_by_id || !id ) return NULL;
    return ( hashed_by_id[HASH( id )] );
}

/* ---------------------------------------------------------------------- */

/*
 *  FUNCTION : instance_getid
 *
 *  Allocate and return a free instance identifier code.
 *
 *  PARAMS :
 *      None
 *
 *  RETURN VALUE :
 *      A new, unused instance identifier
 *      -1 if error
 */

int instance_getid()
{
    int id = instance_maxid++ ;

    if ( !hashed_by_id ) hashed_by_id = calloc( HASH_SIZE, sizeof( INSTANCE * ) );

    if ( id <= LAST_INSTANCE_ID && !hashed_by_id[ HASH( id ) ] ) return id;

    if ( instance_maxid > LAST_INSTANCE_ID )
    {
        for ( instance_maxid = FIRST_INSTANCE_ID; instance_maxid <= LAST_INSTANCE_ID; instance_maxid++ )
        {
            if ( !hashed_by_id[HASH( instance_maxid )] ) return instance_maxid++;
        }
    }
    else
    {
        for ( ; instance_maxid <= LAST_INSTANCE_ID; instance_maxid++ )
        {
            if ( !hashed_by_id[HASH( instance_maxid )] ) return instance_maxid++;
        }
        for ( instance_maxid = FIRST_INSTANCE_ID; instance_maxid < id; instance_maxid++ )
        {
            if ( !hashed_by_id[HASH( instance_maxid )] ) return instance_maxid++;
        }
    }
    return -1;
}

/* ---------------------------------------------------------------------- */

/*
 *  FUNCTION : instance_duplicate
 *
 *  Create a copy of an instance, using the same local/private data
 *    - Updates the instance list, adding the new instance
 *    - Marks all local and private strings
 *    - Updates all parents local family variables
 *
 *  The new instance uses the same code pointer as the father
 *  (it begins execution at the same line, instead of the first one)
 *
 *  PARAMS :
 *      father          Pointer to the original instance
 *
 *  RETURN VALUE :
 *      Pointer to the new instance
 */

INSTANCE * instance_duplicate( INSTANCE * father )
{
    INSTANCE * r, * brother ;
    int n, pid ;
    uint32_t type ;

    if ( ( pid = instance_getid() ) == -1 ) return NULL;

    r = ( INSTANCE * ) calloc( 1, sizeof( INSTANCE ) ) ;
    assert( r ) ;

    r->pridata          = ( int * ) malloc( father->private_size + 4 ) ;
    r->pubdata          = ( int * ) malloc( father->public_size + 4 ) ;
    r->locdata          = ( int * ) malloc( local_size + 4 ) ;
    r->code             = father->code ;
    r->codeptr          = father->codeptr ;
    r->exitcode         = father->exitcode ;
    r->errorcode        = father->errorcode ;
    r->proc             = father->proc ;
    r->call_level       = father->call_level ;

    r->switchval        = 0;
    r->switchval_string = 0;
    r->cased            = 0;

    r->breakpoint       = 0 ;

    r->private_size     = father->private_size ;
    r->public_size      = father->public_size ;
    r->first_run        = 1 ;

    if ( father->private_size > 0 ) memcpy( r->pridata, father->pridata, r->private_size ) ;
    if ( father->public_size > 0 ) memcpy( r->pubdata, father->pubdata, r->public_size ) ;
    if ( local_size > 0 ) memcpy( r->locdata, father->locdata, local_size ) ;

    /* Inicializa datos de jerarquia */

    /* Crea el proceso clónico como si lo hubiera llamado el padre */

    type = LOCDWORD( father, PROCESS_TYPE ) ;
    LOCDWORD( r, PROCESS_ID )   = pid ;
    LOCDWORD( r, SON )          = 0 ;
    LOCDWORD( r, SMALLBRO )     = 0 ;

    LOCDWORD( r, FATHER )       = LOCDWORD( father, PROCESS_ID ) ;
    brother = instance_get( LOCDWORD( father, SON ) ) ;
    if ( brother )
    {
        LOCDWORD( r, BIGBRO )         = LOCDWORD( brother, PROCESS_ID ) ;
        LOCDWORD( brother, SMALLBRO ) = pid ;
    }
    else
    {
        LOCDWORD( r, BIGBRO )         = 0 ;
    }
    LOCDWORD( father, SON )      = pid ;

    /* Actualiza las cuentas de uso de las cadenas */

    for ( n = 0; n < r->proc->string_count; n++ ) string_use( PRIDWORD( r, r->proc->strings[n] ) ) ;  /* Strings privadas */
    for ( n = 0; n < r->proc->pubstring_count; n++ ) string_use( PUBDWORD( r, r->proc->pubstrings[n] ) ) ; /* Strings publicas */
    for ( n = 0; n < local_strings; n++ ) string_use( LOCDWORD( r, localstr[n] ) ) ; /* Strings locales */

    r->prev = NULL ;
    r->next = first_instance ;
    if ( first_instance ) first_instance->prev = r;
    first_instance = r ;

    instance_add_to_list_by_id( r, pid );
    instance_add_to_list_by_instance( r );
    instance_add_to_list_by_type( r, type );
    instance_add_to_list_by_priority( r, LOCINT32( r, PRIORITY ) );

    /* The called_by pointer should be set only when the caller
     * is waiting for this process to return */

    r->called_by = NULL;

    r->stack = malloc( father->stack[0] );
    memmove(r->stack, father->stack, (int)father->stack_ptr - (int)father->stack);
    r->stack_ptr = &r->stack[1];

    /* Initialize list pointers */

    LOCDWORD( r, STATUS ) = STATUS_RUNNING;

    if ( instance_create_hook_count )
        for ( n = 0; n < instance_create_hook_count; n++ )
            instance_create_hook_list[n]( r );

    return r ;
}

/* ---------------------------------------------------------------------- */

/*
 *  FUNCTION : instance_new
 *
 *  Create a new instance, using the default local/private data
 *    - Updates the instance list, adding the new instance
 *    - Marks all local and private strings
 *    - Updates all parents local family variables
 *
 *  PARAMS :
 *      proc            Pointer to the procedure definition
 *      father          Pointer to the father instance (may be NULL)
 *
 *  RETURN VALUE :
 *      Pointer to the new instance
 */

INSTANCE * instance_new( PROCDEF * proc, INSTANCE * father )
{
    INSTANCE * r, * brother;
    int n, pid;

    if ( ( pid = instance_getid() ) == -1 ) return NULL;

    r = ( INSTANCE * ) calloc( 1, sizeof( INSTANCE ) ) ;
    assert( r ) ;

    r->pridata          = ( int * ) malloc( proc->private_size + 4 ) ;
    r->pubdata          = ( int * ) malloc( proc->public_size + 4 ) ;
    r->locdata          = ( int * ) malloc( local_size + 4 ) ;
    r->code             = proc->code ;
    r->codeptr          = proc->code ;
    r->exitcode         = proc->exitcode ;
    r->errorcode        = proc->errorcode ;
    r->proc             = proc ;
    r->call_level       = 0 ;

    r->switchval        = 0;
    r->switchval_string = 0;
    r->cased            = 0;

    r->breakpoint       = 0 ;

    r->private_size     = proc->private_size ;
    r->public_size      = proc->public_size ;
    r->first_run        = 1 ;

    if ( proc->private_size > 0 ) memcpy( r->pridata, proc->pridata, proc->private_size ) ;
    if ( proc->public_size > 0 ) memcpy( r->pubdata, proc->pubdata, proc->public_size ) ;
    if ( local_size > 0 ) memcpy( r->locdata, localdata, local_size ) ;

    /* Inicializa datos de jerarquia */

    LOCDWORD( r, PROCESS_TYPE ) = proc->type ;
    LOCDWORD( r, PROCESS_ID )   = pid ;
    LOCDWORD( r, SON )          = 0 ;
    LOCDWORD( r, SMALLBRO )     = 0 ;

    if ( father )
    {
        LOCDWORD( r, FATHER )     = LOCDWORD( father, PROCESS_ID ) ;
        brother = instance_get( LOCDWORD( father, SON ) ) ;
        if ( brother )
        {
            LOCDWORD( r, BIGBRO )         = LOCDWORD( brother, PROCESS_ID ) ;
            LOCDWORD( brother, SMALLBRO ) = pid ;
        }
        else
        {
            LOCDWORD( r, BIGBRO )         = 0 ;
        }
        LOCDWORD( father, SON )    = pid ;
    }
    else
    {
        LOCDWORD( r, FATHER )     = 0 ;
        LOCDWORD( r, BIGBRO )     = 0 ;
    }

    /* Cuenta los usos de las variables tipo cadena */

    for ( n = 0; n < proc->string_count; n++ ) string_use( PRIDWORD( r, proc->strings[n] ) ) ;  /* Strings privadas */
    for ( n = 0; n < proc->pubstring_count; n++ ) string_use( PUBDWORD( r, proc->pubstrings[n] ) ) ; /* Strings publicas */
    for ( n = 0; n < local_strings; n++ ) string_use( LOCDWORD( r, localstr[n] ) ) ; /* Strings locales */

    r->prev = NULL ;
    r->next = first_instance ;
    if ( first_instance ) first_instance->prev = r;
    first_instance = r ;

    instance_add_to_list_by_id( r, pid );
    instance_add_to_list_by_instance( r );
    instance_add_to_list_by_type( r, proc->type );
    instance_add_to_list_by_priority( r, 0 );

    /* The called_by pointer should be set only when the caller
     * is waiting for this process to return */

    r->called_by = NULL;

    r->stack = malloc( STACK_SIZE );
    r->stack_ptr = &r->stack[1];
    r->stack[0] = STACK_SIZE;

    /* Initialize list pointers */

    LOCDWORD( r, STATUS ) = STATUS_RUNNING;

    if ( instance_create_hook_count )
        for ( n = 0; n < instance_create_hook_count; n++ )
            instance_create_hook_list[n]( r );

    return r ;
}

/* ---------------------------------------------------------------------- */

INSTANCE * instance_getfather( INSTANCE * i )
{
    return instance_get( LOCDWORD( i, FATHER ) ) ;
}

/* ---------------------------------------------------------------------- */

INSTANCE * instance_getson( INSTANCE * i )
{
    return instance_get( LOCDWORD( i, SON ) ) ;
}

/* ---------------------------------------------------------------------- */

INSTANCE * instance_getbigbro( INSTANCE * i )
{
    return instance_get( LOCDWORD( i, BIGBRO ) ) ;
}

/* ---------------------------------------------------------------------- */

INSTANCE * instance_getsmallbro( INSTANCE * i )
{
    return instance_get( LOCDWORD( i, SMALLBRO ) ) ;
}

/* ---------------------------------------------------------------------- */

/*
 *  FUNCTION : instance_destroy_all
 *
 *  Destroy all instances. Simply calls instance_destroy
 *  for any and every instance in existence.
 *
 *  PARAMS :
 *      except          Don't destroy this instance (used for LET_ME_ALONE)
 *
 *  RETURN VALUE :
 *      None
 */

void instance_destroy_all( INSTANCE * except )
{
    INSTANCE * i, * next ;

    i = first_instance ;
    while ( i )
    {
        next = i->next ;
        if ( i != except ) instance_destroy( i ) ;
        i = next ;
    }
}

/* ---------------------------------------------------------------------- */

/*
 *  FUNCTION : instance_destroy
 *
 *  Destroy an instance, effectively
 *    - Updates any instance list, removing the given instance
 *    - Discards all local and private strings
 *    - Updates all parents local family variables
 *    - Frees any memory involved
 *
 *  PARAMS :
 *      r           Pointer to the instance
 *
 *  RETURN VALUE :
 *      None
 */

void instance_destroy( INSTANCE * r )
{
    INSTANCE * father, * bigbro, * smallbro;
    int n ;

    LOCDWORD( r, STATUS ) = STATUS_RUNNING;

    if ( instance_destroy_hook_count )
        for ( n = 0; n < instance_destroy_hook_count; n++ )
            instance_destroy_hook_list[n]( r );

    /* Actualiza la cuenta de referencia de las variables tipo string */

    for ( n = 0 ; n < r->proc->string_count ; n++ ) string_discard( PRIDWORD( r, r->proc->strings[n] ) ) ; /* Strings privadas */
    for ( n = 0 ; n < r->proc->pubstring_count ; n++ ) string_discard( PUBDWORD( r, r->proc->pubstrings[n] ) ) ; /* Strings publicas */
    for ( n = 0 ; n < local_strings ; n++ ) string_discard( LOCDWORD( r, localstr[n] ) ) ; /* Strings locales */

    /* Actualiza árbol de jerarquias */

    bigbro = instance_get( LOCDWORD( r, BIGBRO ) ) ; /* Tengo hermano mayor? */
    if ( bigbro ) LOCDWORD( bigbro, SMALLBRO ) = LOCDWORD( r, SMALLBRO ) ; /* El hermano menor de mi hermano mayor es mi hermano menor */

    smallbro = instance_get( LOCDWORD( r, SMALLBRO ) ) ; /* Tengo hermano menor? */
    if ( smallbro ) LOCDWORD( smallbro, BIGBRO ) = LOCDWORD( r, BIGBRO ) ; /* El hermano mayor de mi hermano menor es mi hermano mayor */

    father = instance_get( LOCDWORD( r, FATHER ) ) ; /* Tengo padre? */
    if ( father && instance_get( LOCDWORD( father, SON ) ) == r ) LOCDWORD( father, SON ) = LOCDWORD( r, BIGBRO ); /* Si tengo padre y soy el hijo menor, mi hermano mayor pasa a ser el menor hijo de mi padre */

    /* Quita la instancia de la lista */

    if ( r->prev ) r->prev->next = r->next ;
    if ( r->next ) r->next->prev = r->prev ;

    if ( first_instance == r ) first_instance = r->next ;

    /* Remove the instance from all hash lists */

    instance_remove_from_list_by_id( r, LOCDWORD( r, PROCESS_ID ) );
    instance_remove_from_list_by_instance( r );
    instance_remove_from_list_by_type( r, LOCDWORD( r, PROCESS_TYPE ) );
    instance_remove_from_list_by_priority( r );

    if ( r->stack ) free( r->stack ) ;

    if ( r->locdata ) free( r->locdata ) ;
    if ( r->pubdata ) free( r->pubdata ) ;
    if ( r->pridata ) free( r->pridata ) ;
    free( r ) ;
}

/* ---------------------------------------------------------------------- */

/*
 *  FUNCTION : instance_exists
 *
 *  Given an instance pointer, returns TRUE if it is still valid.
 *
 *  PARAMS :
 *      i               Pointer to the instance
 *
 *  RETURN VALUE :
 *      1 if the instance pointer is in the global instance list, 0 otherwise
 */

int instance_exists( INSTANCE * r )
{
    INSTANCE * i ;

    if ( !hashed_by_instance || !r ) return 0;

    i = hashed_by_instance[HASH_INSTANCE( r )];
    while ( i )
    {
        if ( r == i ) return 1 ;
        i = i->next_by_instance ;
    }

    return 0 ;
}

/* ---------------------------------------------------------------------- */

/*
 *  FUNCTION : instance_next_by_priority
 *
 *  Gets the next instance pointer until no more instances are
 *  returned. Instances are returned sorted by priority.
 *
 *  PARAMS :
 *      None
 *
 *  RETURN VALUE :
 *      Pointer to the next priority on the list or NULL
 *      if there is no more instances. The next call in this
 *      case will return a pointer to the first instance
 *      (the one with the lower priority)
 */

INSTANCE * instance_next_by_priority()
{
    INSTANCE * r = iterator_by_priority ;

    if ( iterator_by_priority ) iterator_by_priority = iterator_by_priority->next_by_priority;

    if ( !iterator_by_priority )
    {
        if ( !hashed_by_priority ) return NULL;

        if ( iterator_pos < ( instance_min_actual_prio + INSTANCE_NORMALIZE_PRIORITY ) ||
             iterator_pos > ( instance_max_actual_prio + INSTANCE_NORMALIZE_PRIORITY )
           )
            iterator_pos = instance_max_actual_prio + INSTANCE_NORMALIZE_PRIORITY + 1;

        while ( --iterator_pos >= ( instance_min_actual_prio + INSTANCE_NORMALIZE_PRIORITY ) && !( iterator_by_priority = hashed_by_priority[iterator_pos] ) );
    }

    return ( r ) ;
}

/* ---------------------------------------------------------------------- */

/*
 *  FUNCTION : instance_get_by_type
 *
 *  Returns a instance, given its type.
 *
 *  PARAMS :
 *      type            Integer type of the first instance
 *      context         Pointer to an INSTANCE * use as context (internal use)
 *
 *  RETURN VALUE :
 *      Pointer to the found instance or NULL if not found
 */

/*
context = NULL = start scan
context = pointer = continue scan
context = -1 = end scan
*/

INSTANCE * instance_get_by_type( uint32_t type, INSTANCE ** context )
{
    INSTANCE * i;

    if ( !context || !hashed_by_type || !type /* || type >= FIRST_INSTANCE_ID */ ) return NULL;

    if ( !*context ) /* start scan */
        i = hashed_by_type[HASH( type )];
    else if ( ( i = *context ) == ( INSTANCE * ) -1 ) /* End scan */
        return ( *context = NULL );

    if ( i ) /* Valid instance, continue scan */
    {
        if ( i->next_by_type )
            *context = i->next_by_type ;
        else
            *context = ( INSTANCE * ) -1 ; /* Next call will be "end scan" */

        return i;
    }

    /* Here only if hashed_by_type[HASH( type )] is NULL */
    return ( *context = NULL ) ; /* return is null, then end scan */
}

/* ---------------------------------------------------------------------- */
