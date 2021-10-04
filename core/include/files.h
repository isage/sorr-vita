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

#ifndef __FILES_H
#define __FILES_H

#include <stdint.h>

#include "arrange.h"

#include "files_st.h"

/* Funciones de acceso a ficheros */
/* ------------------------------ */

/* Ahora mismo son casi wrappers de stdio.h, pero en el futuro
 * el tipo "file" puede ser una estructura y las funciones,
 * ofrecer soporte transparente para ficheros PAK, etc. */

extern file * file_open        (const char * filename, char * mode) ;

extern int    file_read        (file * fp, void * buffer, int len) ;

extern int    file_readSint8   (file * fp, int8_t  * buffer) ;
extern int    file_readUint8   (file * fp, uint8_t  * buffer) ;
extern int    file_readSint16  (file * fp, int16_t * buffer) ;
extern int    file_readUint16  (file * fp, uint16_t * buffer) ;
extern int    file_readSint32  (file * fp, int32_t * buffer) ;
extern int    file_readUint32  (file * fp, uint32_t * buffer) ;

extern int    file_readSint8A  (file * fp, int8_t  * buffer, int n) ;
extern int    file_readUint8A  (file * fp, uint8_t  * buffer, int n) ;
extern int    file_readSint16A (file * fp, int16_t * buffer, int n) ;
extern int    file_readUint16A (file * fp, uint16_t * buffer, int n) ;
extern int    file_readSint32A (file * fp, int32_t * buffer, int n) ;
extern int    file_readUint32A (file * fp, uint32_t * buffer, int n) ;

extern int    file_write       (file * fp, void * buffer, int len) ;

extern int    file_writeSint8  (file * fp, int8_t  * buffer) ;
extern int    file_writeUint8  (file * fp, uint8_t  * buffer) ;
extern int    file_writeSint16 (file * fp, int16_t * buffer) ;
extern int    file_writeUint16 (file * fp, uint16_t * buffer) ;
extern int    file_writeSint32 (file * fp, int32_t * buffer) ;
extern int    file_writeUint32 (file * fp, uint32_t * buffer) ;

extern int    file_writeSint8A (file * fp, int8_t  * buffer, int n) ;
extern int    file_writeUint8A (file * fp, uint8_t  * buffer, int n) ;
extern int    file_writeSint16A(file * fp, int16_t * buffer, int n) ;
extern int    file_writeUint16A(file * fp, uint16_t * buffer, int n) ;
extern int    file_writeSint32A(file * fp, int32_t * buffer, int n) ;
extern int    file_writeUint32A(file * fp, uint32_t * buffer, int n) ;

extern int    file_qgets       (file * fp, char * buffer, int len) ;
extern int    file_qputs       (file * fp, char * buffer) ;
extern int    file_gets        (file * fp, char * buffer, int len) ;
extern int    file_puts        (file * fp, char * buffer) ;
extern int    file_size        (file * fp) ;
extern int    file_pos         (file * fp) ;
extern int    file_flush       (file * fp) ;
extern int    file_seek        (file * fp, int pos, int where) ;
extern void   file_rewind      (file * fp) ;
extern void   file_addp        (const char * path) ;
extern void   file_close       (file * fp) ;
extern int    file_remove      (const char * filename) ;
extern int    file_move        (const char * source_file, const char * target_file) ;
extern int    file_exists      (const char * filename) ;
extern void   file_add_xfile   (file * fp, char * stubname, long offset, char * name, int size) ;
extern int    file_eof         (file * fp) ;
extern FILE * file_fp          (file * fp) ;

extern void   xfile_init       (int maxfiles);

extern int    opened_files;

extern char * getfullpath( char *rel_path );
extern char * whereis( char *filename );

#endif
