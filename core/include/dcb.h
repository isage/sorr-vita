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
 * FILE        : dcb.h
 * DESCRIPTION : Data compiled block information
 *
 */

#ifndef __DCB_H
#define __DCB_H

#include <stdint.h>

#include "typedef.h"

#define DCB_DEBUG 1

#ifdef _MSC_VER
#pragma pack(push, 1)
#endif

/* Opción del GNU C para que la estructura ocupe el mínimo de memoria */

#ifdef __GNUC__
#define __PACKED __attribute__ ((packed))
#else
#define __PACKED
#endif

/* Estructura del fichero .dcb */

/* Please update the version's high-number between versions */
#define DCB_VERSION 0x0710

#define DCL_MAGIC       "dcl\x0d\x0a\x1f\x00\x00"
#define DCB_MAGIC       "dcb\x0d\x0a\x1f\x00\x00"

#define DCB_STUB_MAGIC  "DCB Stub\x1a\x0d\x0a"

typedef struct
{
    uint8_t     Name[60] ;
    uint32_t    Code ;
}
__PACKED
DCB_ID ;

#define DCB_FILE_COMPRESSED 1

typedef struct
{
    union {
        uint8_t     * Name ;
        uint32_t    SName ;
    };
    uint8_t     Flags ;
    uint32_t    SFile ;
    uint32_t    OFile ;
}
__PACKED
DCB_FILE ;

typedef struct
{
    uint32_t    NFile ;
    uint32_t    NLine ;
    uint32_t    NCol ;
    uint32_t    OCode ;
}
__PACKED
DCB_SENTENCE ;

#define NO_MEMBERS       0xFFFFFFFF

typedef struct
{
    uint8_t     BaseType [MAX_TYPECHUNKS] ;
    uint32_t    Count    [MAX_TYPECHUNKS] ;
    uint32_t    Members ;
}
__PACKED
DCB_TYPEDEF ;

typedef struct
{
    DCB_TYPEDEF Type ;      /* 40 bytes */
    uint32_t    ID ;
    uint32_t    Offset ;
    uint32_t    Varspace ;
}
__PACKED
DCB_VAR ;

typedef struct
{
    uint32_t    NVars ;
    uint32_t    OVars ;
}
__PACKED
DCB_VARSPACE ;

typedef struct
{
    uint32_t    Id ; /* Identifier of Name, Segment Key 1 */
    BASETYPE    Type ; /* Segment Key 2 */
    uint32_t    Params ; /* Segment Key 3 */
    uint32_t    Code ;
}
__PACKED
DCB_SYSPROC_CODE ;

/* Internal Use, see also DCB_SYSPROC_CODE */
typedef struct
{
    uint32_t    Id ;
    BASETYPE    Type ;
    uint32_t    Params ;
    uint32_t    Code ;
    uint8_t     * ParamTypes;
}
__PACKED
DCB_SYSPROC_CODE2 ;

typedef struct          /* Cabecera de cada proceso     */
{
    uint32_t    ID ;

    uint32_t    Flags ;

    uint32_t    NParams ;
    uint32_t    NPriVars ;
    uint32_t    NPriStrings ;
    uint32_t    NPubVars ;
    uint32_t    NPubStrings ;
    uint32_t    NSentences ;

    uint32_t    SPrivate ;
    uint32_t    SPublic ;
    uint32_t    SCode ;

    uint32_t    OPrivate ;
    uint32_t    OPriVars ;
    uint32_t    OPriStrings ;

    uint32_t    OPublic ;
    uint32_t    OPubVars ;
    uint32_t    OPubStrings ;

    uint32_t    OCode ;
    uint32_t    OSentences ;

    uint32_t    OExitCode ;
    uint32_t    OErrorCode ;
}
__PACKED
DCB_PROC_DATA ;

typedef struct          /* Cabecera de cada proceso     */
{
    DCB_PROC_DATA   data; /* Estructura que se carga desde el fichero */

    DCB_SENTENCE    * sentence ;

    DCB_VAR         * privar ;
    DCB_VAR         * pubvar ;
}
__PACKED
DCB_PROC ;

/*
typedef struct
{
    uint32_t    ProcId ;
    uint32_t    Identifier ;
    uint32_t    Code ;
}
__PACKED
DCB_LABEL ;
*/

typedef struct          /* Cabecera general del fichero */
{
    uint8_t     Header[8] ; /* "DCB"            */
    uint32_t    Version ;   /* 0x0100 para versión 1.0  */

    uint32_t    NProcs ;
    uint32_t    NFiles ;
    uint32_t    NID ;
    uint32_t    NStrings ;
    uint32_t    NLocStrings ;
    uint32_t    NLocVars ;
    uint32_t    NGloVars ;
    uint32_t    NVarSpaces ;

    uint32_t    SGlobal ;
    uint32_t    SLocal ;
    uint32_t    SText ;

    uint32_t    NImports ;
    uint32_t    NSourceFiles ;
    uint32_t    NSysProcsCodes ; /* Para el fixup de los SYSPROCS */
    uint32_t    __reserved1[2] ;

    uint32_t    OProcsTab ;
    uint32_t    OID ;
    uint32_t    OStrings ;
    uint32_t    OText ;
    uint32_t    OGlobal ;
    uint32_t    OGloVars ;
    uint32_t    OLocal ;
    uint32_t    OLocVars ;
    uint32_t    OLocStrings ;
    uint32_t    OVarSpaces ;
    uint32_t    OFilesTab ;
    uint32_t    OImports ;
    uint32_t    OSourceFiles ;
    uint32_t    OSysProcsCodes ; /* Para el fixup de los SYSPROCS */
    uint32_t    __reserved2[2] ;
}
__PACKED
DCB_HEADER_DATA ;

typedef struct          /* Cabecera general del fichero */
{
    DCB_HEADER_DATA data;   /* Estructura que se carga desde el fichero */

    DCB_ID          * id ;
    DCB_VAR         * glovar ;
    DCB_VAR         * locvar ;
    DCB_PROC        * proc ;
    DCB_FILE        * file ;
    DCB_VARSPACE    * varspace ;
    DCB_VAR         ** varspace_vars ;
    uint32_t        * imports ;
    uint8_t         *** sourcelines ;
    uint32_t        * sourcecount ;
    uint8_t         ** sourcefiles ;
}
__PACKED
DCB_HEADER ;

typedef struct
{
    char    magic[12];
    int     dcb_offset;
}
__PACKED
dcb_signature;

#ifdef _MSC_VER
#pragma pack(pop)
#endif

extern DCB_HEADER dcb ;

extern void sysprocs_fixup( void );
extern int getid( char * name );

#endif
