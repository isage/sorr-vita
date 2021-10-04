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
#include <string.h>
#include <stdlib.h>

#include "bgdc.h"

/*
 *  FUNCTION : codeblock_postprocess
 *
 *  After every codeblock has been completed (all the source code is
 *  compiled), update a codeblock with all things that couldn't be
 *  done before. This function does the following:
 *
 *	- Updates all user procedure/function calls with the process
 *    number instead of its identifier id. Emits a compiler error
 *    if a process what user but never defined.
 *
 *  - Updates all jumps so that they go to absolute opcode offsets
 *    inside the procedure, instead of refering to the label table
 *
 *  This function will be called once for each existing code block.
 *
 *  PARAMS :
 *      code			Pointer to the codeblock to modify
 *
 *  RETURN VALUE :
 *      None
 */

void codeblock_postprocess (CODEBLOCK * code)
{
	int * ptr = code->data ;
	PROCDEF * proc, * my = procdef_search_by_codeblock( code ) ;

	while (ptr < code->data + code->current)
	{
		if ( !libmode && ( *ptr == MN_CALL || *ptr == MN_PROC || *ptr == MN_TYPE ))
		{
			proc = procdef_search(ptr[1]) ;
			if (!proc || !proc->defined)
			{
				token.type = IDENTIFIER ;
				token.code = ptr[1] ;
				/* Patch so linecount gets right */
				line_count = identifier_line(token.code) ;
				current_file = identifier_file(token.code) ;
				compile_error (MSG_UNDEFINED_PROC) ;
			}
	        ptr[1] = proc->typeid ;
		}
		if ( !my->imported )
		{
    		if (*ptr == MN_JUMP      || *ptr == MN_NCALL   ||
    		    *ptr == MN_JFALSE    || *ptr == MN_JTFALSE ||
    		    *ptr == MN_JTRUE     || *ptr == MN_JTTRUE  ||
    		    *ptr == MN_JNOCASE   || *ptr == MN_CLONE   ||
    		    *ptr == MN_EXITHNDLR || *ptr == MN_ERRHNDLR
    		    )
    		{
    			ptr++ ;
    			if (*ptr == -1)
    			{
//    			    *ptr = 0;
    			    ptr++;
    			    continue;
    			}
                if (code->labels[*ptr] == -1)
    			{
    				token.type = LABEL ;
    				token.code = code->labelsextra[*ptr].name ;
    				/* Patch so linecount gets right */
    				line_count = code->labelsextra[*ptr].line ;
    				current_file = code->labelsextra[*ptr].file ;
    				compile_error ("Undefined label") ;
    			}
    			*ptr = code->labels[*ptr] ;
    			ptr++ ;
    			continue ;
    		}
    		if (*ptr == MN_REFALSE)
    		{
    			*ptr++ = MN_JFALSE ;
    			*ptr = code->loops[*ptr*2] ;
    			ptr++ ;
    			continue ;
    		}
    		if (*ptr == MN_REPEAT || *ptr == MN_RETRUE)
    		{
    			*ptr = (*ptr == MN_REPEAT ? MN_JUMP : MN_JTRUE) ;
    			ptr++ ;
    			*ptr = code->loops[*ptr*2] ;
    			ptr++ ;
    			continue ;
    		}
    		if (*ptr == MN_BREAK || *ptr == MN_BRFALSE)
    		{
    			*ptr = (*ptr == MN_BREAK ? MN_JUMP : MN_JFALSE) ;
    			ptr++ ;
    			*ptr = code->loops[*ptr*2 + 1] ;
    			ptr++ ;
    			continue ;
    		}
    	}
		ptr+=MN_PARAMS(*ptr)+1 ;
	}
}

/*
 *  FUNCTION : codeblock_init
 *
 *  Initializes a new code block and allocates initial data
 *  for all its internal structs.
 *
 *  PARAMS :
 *      c			Pointer to the codeblock to initialize
 *
 *  RETURN VALUE :
 *      None
 */

void codeblock_init(CODEBLOCK * c)
{
	c->data = (int *) calloc (64, sizeof(int)) ;
	c->reserved = 64 ;

	c->loops = (int *) calloc (16, sizeof(int)) ;
	c->loop_reserved = 8 ;
	c->loop_count = 1 ;
	c->loop_active = 0 ;

	c->labels = (int *) calloc (16, sizeof(int)) ;
	c->labelsextra = (struct _labelsextra *) calloc (16, sizeof(struct _labelsextra)) ;
	c->label_count = 0 ;
	c->label_reserved = 16 ;

	c->current = 0 ;
	c->previous = 0 ;
	c->previous2 = 0 ;

	if (!c->data || !c->loops || !c->labels || !c->labelsextra)
	{
		fprintf (stdout, "CODEBLOCK: out of memory\n") ;
		exit (1) ;
	}
}

/*
 *  FUNCTION : codeblock_alloc
 *             codeblock_loop_alloc
 *			   codeblock_label_alloc
 *
 *  Internal functions that alloc more memory for an
 *  internal data of the code block. Shouldn't be used
 *  outside this module.
 *
 *  PARAMS :
 *      c			Pointer to the codeblock
 *		count		Additional size in number of members
 *
 *  RETURN VALUE :
 *      None
 */

void codeblock_alloc (CODEBLOCK * c, int count)
{
	c->reserved += count ;
	c->data = (int *) realloc (c->data, c->reserved * sizeof(int)) ;
	if (!c->data)
	{
		fprintf (stdout, "CODEBLOCK: out of memory\n") ;
		exit (1) ;
	}
}

static void codeblock_loop_alloc (CODEBLOCK * c, int count)
{
	c->loop_reserved += count ;
	c->loops = (int *) realloc (c->loops, c->loop_reserved * sizeof(int) * 2) ;
	if (!c->loops)
	{
		fprintf (stdout, "CODEBLOCK: out of memory\n") ;
		exit (1) ;
	}
}

static void codeblock_label_alloc (CODEBLOCK * c, int count)
{
	c->label_reserved += count ;
	c->labels = (int *) realloc (c->labels, c->label_reserved * sizeof(int)) ;
	c->labelsextra = (struct _labelsextra *) realloc (c->labelsextra, c->label_reserved * sizeof(struct _labelsextra)) ;
	if (!c->labels)
	{
		fprintf (stdout, "CODEBLOCK: out of memory\n") ;
		exit (1) ;
	}
}

int  codeblock_loop_add (CODEBLOCK * c)
{
	return c->loop_count++ ;
}

void codeblock_loop_start (CODEBLOCK * c, int loop, int begin)
{
	if (c->loop_reserved <= loop)
		codeblock_loop_alloc (c, loop + 8 - c->loop_reserved) ;
	if (c->loop_count <= loop)
		c->loop_count = loop+1 ;
	c->loops[loop*2] = begin ;
}

void codeblock_loop_end (CODEBLOCK * c, int loop, int end)
{
	if (c->loop_reserved <= loop)
		codeblock_loop_alloc (c, loop + 8 - c->loop_reserved) ;
	if (c->loop_count <= loop)
		c->loop_count = loop+1 ;
	c->loops[loop*2+1] = end ;
}

int codeblock_label_add (CODEBLOCK * c, int identifier)
{
	if (c->label_count == c->label_reserved) codeblock_label_alloc (c, c->label_count + 16) ;
    c->labels[c->label_count] = -1;
    c->labelsextra[c->label_count].name = identifier;
    c->labelsextra[c->label_count].file = current_file;
    c->labelsextra[c->label_count].line = line_count;
	c->label_count++ ;
 	return c->label_count - 1 ;
}

void codeblock_label_set (CODEBLOCK * c, int label, int offset)
{
	c->labels[label] = offset ;
}

int codeblock_label_get (CODEBLOCK * c, int label)
{
	return c->labels[label] ;
}

int codeblock_label_get_id_by_name (CODEBLOCK * c, int identifier)
{
    int i;
    for (i = 0; i < c->label_count; i++) if (c->labelsextra[i].name != -1 && c->labelsextra[i].name == identifier) return i;
    return -1;
}

void codeblock_stepback (CODEBLOCK * c)
{
	int code  = c->data[c->previous];
	int param = 0;

	if (MN_PARAMS(code) > 0)
		param = c->data[c->previous+1];

	if (c->previous != c->previous2)
	{
		c->current  = c->previous;
		c->previous = c->previous2;
		codeblock_add (c, code, param);
	}
}

CODEBLOCK_POS codeblock_pos(CODEBLOCK * c)
{
	CODEBLOCK_POS p;

	p.current = c->current;
	p.previous = c->previous;
	p.previous2 = c->previous2;
	return p;
}

void codeblock_setpos(CODEBLOCK * c, CODEBLOCK_POS p)
{
	c->current = p.current;
	c->previous = p.previous;
	c->previous2 = p.previous2;
}

void codeblock_add_block (CODEBLOCK * c, CODEBLOCK_POS from, CODEBLOCK_POS to)
{
	if (c->current+(to.current - from.current)+2 >= c->reserved)
		codeblock_alloc (c, (to.current - from.current + 34) & 0x1F) ;

	memcpy (c->data + c->current, c->data + from.current, 4*(to.current - from.current));
	c->current += to.current - from.current;
	c->previous = c->current - (to.current - to.previous);
	if (to.current - from.current > 2)
		c->previous2 = c->current - (to.current - to.previous2);
	else
		c->previous2 = c->previous;
}

void codeblock_add (CODEBLOCK * c, int code, int param)
{
	if (!c) return ;

	if (MN_PARAMS(code) == 0 && param)
		fprintf (stdout, "WARNING: mnemonic 0x%02X don't get params\n", code) ;

	if (c->current > 0)
	{
		if (code == MN_ARRAY && c->data[c->previous] == MN_PUSH)
		{
			c->data[c->previous] = MN_INDEX ;
			c->data[c->previous+1] *= param ;
			codeblock_stepback(c);
			return ;
		}
		else if (code == MN_ADD && c->data[c->previous] == MN_PUSH)
		{
			c->data[c->previous] = MN_INDEX ;
			codeblock_stepback(c);
			return ;
		}
		else if (code == MN_SUB && c->data[c->previous] == MN_PUSH)
		{
			c->data[c->previous] = MN_INDEX ;
			c->data[c->previous+1] = -c->data[c->previous+1] ;
			codeblock_stepback(c);
			return ;
		}
		else if ((code & MN_MASK) == MN_INDEX)
		{
			if (c->data[c->previous] == MN_INDEX)
			{
				c->data[c->previous+1] += param ;
				return ;
			}
			if ((c->data[c->previous] & MN_MASK) == MN_GLOBAL
			 || (c->data[c->previous] & MN_MASK) == MN_LOCAL
			 || (c->data[c->previous] & MN_MASK) == MN_PUBLIC
			 || (c->data[c->previous] & MN_MASK) == MN_REMOTE_PUBLIC
			 || (c->data[c->previous] & MN_MASK) == MN_PRIVATE
			 || (c->data[c->previous] & MN_MASK) == MN_REMOTE)
			{
				c->data[c->previous+1] += param ;
				return ;
			}
		}
		else if (code == MN_POP)
		{
			switch (c->data[c->previous] & MN_MASK)
			{
				case MN_LET:
					c->data[c->previous] = MN_LETNP | (c->data[c->previous] & ~MN_MASK);
					return ;
				case MN_CALL:
					c->data[c->previous] = MN_PROC ;
					return ;
				case MN_SYSCALL:
					c->data[c->previous] = MN_SYSPROC ;
					return ;
			}
		}
		else if ((code & MN_MASK) == MN_PTR)
		{
			/* Mismo caso */

			switch (c->data[c->previous] & MN_MASK)
			{
				case MN_PRIVATE:
					c->data[c->previous] = MN_GET_PRIV   | (code & ~MN_MASK) ;
					return ;
				case MN_LOCAL:
					c->data[c->previous] = MN_GET_LOCAL  | (code & ~MN_MASK) ;
					return ;
				case MN_PUBLIC:
					c->data[c->previous] = MN_GET_PUBLIC | (code & ~MN_MASK) ;
					return ;
				case MN_REMOTE_PUBLIC:
					c->data[c->previous] = MN_GET_REMOTE_PUBLIC | (code & ~MN_MASK) ;
					return ;
				case MN_GLOBAL:
					c->data[c->previous] = MN_GET_GLOBAL | (code & ~MN_MASK) ;
					return ;
				case MN_REMOTE:
					c->data[c->previous] = MN_GET_REMOTE | (code & ~MN_MASK) ;
					return ;
			    case MN_PUSH:
					return ;
			}
		}
	}

	c->previous2 = c->previous;
	c->previous  = c->current ;
	c->data[c->current++] = code ;

	if (MN_PARAMS(code) > 0) {
		c->data[c->current++] = param ;
    }

	if (c->current+2 >= c->reserved) codeblock_alloc (c, 32) ;
}

