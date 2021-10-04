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

#ifndef __INSTANCE_ST_H
#define __INSTANCE_ST_H

#define FIRST_INSTANCE_ID   0x00010000
#define LAST_INSTANCE_ID    0x0001ffff

#define STACK_RETURN_VALUE  0x8000
#define STACK_SIZE_MASK     0x7FFF
#define STACK_SIZE          2048

/* Instancias. Una instancia se crea a partir de un proceso, pero en
 * realidad es independiente del proceso original */

typedef struct _instance
{
    void * locdata ;
    void * pridata ;
    void * pubdata ;

    int * code ;
    int * codeptr ;

    int exitcode ;
    int errorcode ;

    int call_level ;

    struct _procdef  * proc ;

    int private_size ;
    int public_size ;

    int first_run ;

    /* General list - unsorted */

    struct _instance * next ;
    struct _instance * prev ;

    /* Linked list by priority */

    struct _instance * next_by_priority ;
    struct _instance * prev_by_priority ;
    int last_priority ;

    /* Linked list by process_type */

    struct _instance * next_by_type ;
    struct _instance * prev_by_type ;

    /* Linked list by INSTANCE * */

    struct _instance * next_by_instance ;
    struct _instance * prev_by_instance ;

    /* Function support */

    struct _instance * called_by ;

    /* The first integer at the stack is the stack size,
       with optional NO_RETURN_VALUE mask. The stack contents follows */

    int * stack ;
    int * stack_ptr ;

    /* For debugging */

    int switchval;
    int switchval_string;
    int cased;

    int breakpoint;

}
INSTANCE ;

/* Macros para acceder a datos locales o privados de una instancia */

#define LOCDWORD(a,b)   ( *(uint32_t *) ((uint8_t *)(a->locdata)+b) )
#define LOCWORD(a,b)    ( *(uint16_t *) ((uint8_t *)(a->locdata)+b) )
#define LOCBYTE(a,b)    ( *(uint8_t  *) ((uint8_t *)(a->locdata)+b) )

#define LOCINT32(a,b)   ( *(int32_t  *) ((uint8_t *)(a->locdata)+b) )
#define LOCINT16(a,b)   ( *(int16_t  *) ((uint8_t *)(a->locdata)+b) )
#define LOCINT8(a,b)    ( *(int8_t   *) ((uint8_t *)(a->locdata)+b) )

#define LOCUINT32(a,b)  ( *(uint32_t *) ((uint8_t *)(a->locdata)+b) )
#define LOCUINT16(a,b)  ( *(uint16_t *) ((uint8_t *)(a->locdata)+b) )
#define LOCUINT8(a,b)   ( *(uint8_t  *) ((uint8_t *)(a->locdata)+b) )


#define PRIDWORD(a,b)   ( *(uint32_t *) ((uint8_t *)(a->pridata)+b) )
#define PRIWORD(a,b)    ( *(uint16_t *) ((uint8_t *)(a->pridata)+b) )
#define PRIBYTE(a,b)    ( *(uint8_t  *) ((uint8_t *)(a->pridata)+b) )

#define PRIINT32(a,b)   ( *(int32_t  *) ((uint8_t *)(a->pridata)+b) )
#define PRIINT16(a,b)   ( *(int16_t  *) ((uint8_t *)(a->pridata)+b) )
#define PRIINT8(a,b)    ( *(int8_t   *) ((uint8_t *)(a->pridata)+b) )

#define PRIUINT32(a,b)  ( *(uint32_t *) ((uint8_t *)(a->pridata)+b) )
#define PRIUINT16(a,b)  ( *(uint16_t *) ((uint8_t *)(a->pridata)+b) )
#define PRIUINT8(a,b)   ( *(uint8_t  *) ((uint8_t *)(a->pridata)+b) )


#define PUBDWORD(a,b)   ( *(uint32_t *) ((uint8_t *)(a->pubdata)+b) )
#define PUBWORD(a,b)    ( *(uint16_t *) ((uint8_t *)(a->pubdata)+b) )
#define PUBBYTE(a,b)    ( *(uint8_t  *) ((uint8_t *)(a->pubdata)+b) )

#define PUBINT32(a,b)   ( *(int32_t  *) ((uint8_t *)(a->pubdata)+b) )
#define PUBINT16(a,b)   ( *(int16_t  *) ((uint8_t *)(a->pubdata)+b) )
#define PUBINT8(a,b)    ( *(int8_t   *) ((uint8_t *)(a->pubdata)+b) )

#define PUBUINT32(a,b)  ( *(uint32_t *) ((uint8_t *)(a->pubdata)+b) )
#define PUBUINT16(a,b)  ( *(uint16_t *) ((uint8_t *)(a->pubdata)+b) )
#define PUBUINT8(a,b)   ( *(uint8_t  *) ((uint8_t *)(a->pubdata)+b) )


#define GLODWORD(b)     ( *(uint32_t *) ((uint8_t *)(globaldata)+b) )
#define GLOWORD(b)      ( *(uint16_t *) ((uint8_t *)(globaldata)+b) )
#define GLOBYTE(b)      ( *(uint8_t  *) ((uint8_t *)(globaldata)+b) )

#define GLOINT32(b)     ( *(int32_t  *) ((uint8_t *)(globaldata)+b) )
#define GLOINT16(b)     ( *(int16_t  *) ((uint8_t *)(globaldata)+b) )
#define GLOINT8(b)      ( *(int8_t   *) ((uint8_t *)(globaldata)+b) )

#define GLOUINT32(b)    ( *(uint32_t *) ((uint8_t *)(globaldata)+b) )
#define GLOUINT16(b)    ( *(uint16_t *) ((uint8_t *)(globaldata)+b) )
#define GLOUINT8(b)     ( *(uint8_t  *) ((uint8_t *)(globaldata)+b) )

#endif
