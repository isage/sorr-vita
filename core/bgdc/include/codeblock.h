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

#ifndef __CODEBLOCK_H
#define __CODEBLOCK_H

/* Bloques de código compilado. Contienen estructuras que sólo se emplean
 * durante el compilado: la función program_postprocess() independiza los
 * datos (CODEBLOCK.data[]) de dichas estructuras, que el intérprete
 * no necesita */

typedef struct _codeblock
{
	int * data ;
	int reserved ;
	int current ;
	int previous ;
	int previous2 ;

	int * loops ;
	int loop_count ;
	int loop_reserved ;
	int loop_active ;

	int * labels ;
	int label_count ;
	int label_reserved ;
	struct _labelsextra{
	    int name;
	    int file;
	    int line;
	} * labelsextra;

}
CODEBLOCK ;

typedef struct _codeblock_pos
{
	int current;
	int previous;
	int previous2;
}
CODEBLOCK_POS ;

extern void codeblock_init(CODEBLOCK * c) ;
extern void codeblock_add (CODEBLOCK * c, int code, int param) ;
extern void codeblock_add_block (CODEBLOCK * c, CODEBLOCK_POS from, CODEBLOCK_POS to);
extern void codeblock_loop_start (CODEBLOCK * c, int loop, int begin) ;
extern void codeblock_loop_end (CODEBLOCK * c, int loop, int end) ;
extern int  codeblock_loop_add (CODEBLOCK * c) ;
extern int  codeblock_label_add (CODEBLOCK * c, int identifier) ;
extern void codeblock_label_set (CODEBLOCK * c, int label, int offset) ;
extern int  codeblock_label_get (CODEBLOCK * c, int label);
extern int codeblock_label_get_id_by_name (CODEBLOCK * c, int name);
extern void codeblock_postprocess (CODEBLOCK * c) ;
extern void codeblock_dump (CODEBLOCK * c) ;
extern void mnemonic_dump (int i, int param) ;
extern void program_postprocess () ;

extern CODEBLOCK_POS codeblock_pos(CODEBLOCK * c);
extern void          codeblock_setpos(CODEBLOCK * c, CODEBLOCK_POS p);

extern void codeblock_alloc (CODEBLOCK * c, int count);


#endif

