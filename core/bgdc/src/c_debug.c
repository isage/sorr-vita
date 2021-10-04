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
#ifdef TARGET_BEOS
#include <posix/assert.h>
#else
#include <assert.h>
#endif

#include "bgdc.h"

/* ---------------------------------------------------------------------- */
/* Este módulo contiene funciones que muestran el equivalente en texto de */
/* una instrucción o mnemónico, o de un bloque de código completo         */
/* ---------------------------------------------------------------------- */

void codeblock_dump (CODEBLOCK * c)
{
	int i, n, showdir ;
	for (i = 0 ; i < c->current ; i += MN_PARAMS(c->data[i])+1)
	{
		showdir = 0 ;
		for (n = 0 ; n < c->label_count ; n++)
		{
			if (c->labels[n] == i)
			{
				if (!showdir) printf ("\n") ;
				printf ("Label %d:\n", n) ;
				showdir = 1 ;
			}
		}
		for (n = 1 ; n < c->loop_count ; n++)
		{
			if (c->loops[n*2] == i)
			{
				if (!showdir) printf ("\n") ;
				printf("Start %d:\n", n) ;
				showdir = 1 ;
			}
			if (c->loops[n*2+1] == i)
			{
				if (!showdir) printf ("\n") ;
				printf("End %d:\n", n) ;
				showdir = 1 ;
			}
		}
		if (showdir) printf ("\n%d:\n", i) ;

		printf ("\t") ;

		mnemonic_dump (c->data[i], c->data[i+1] );

	}
	printf ("%d:\n", i) ;
}

/* ---------------------------------------------------------------------- */
