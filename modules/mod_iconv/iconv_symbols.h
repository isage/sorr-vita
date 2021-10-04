/*
 *  Copyright © 2011 Joseba García Etxebarria <joseba.gar@gmail.com>
 *
 *  mod_iconv is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  mod_iconv is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 */

#ifndef __MODICONV_SYMBOLS_H
#define __MODICONV_SYMBOLS_H

#include <bgddl.h>

#ifdef __BGDC__
DLSYSFUNCS __bgdexport( mod_iconv, functions_exports )[] =
{
	{ "ICONV"            , "SSS"  , TYPE_STRING, 0 },
	{ 0                  , 0      , 0          , 0 }
};
#else
extern DLSYSFUNCS __bgdexport( mod_iconv, functions_exports )[];
#endif

#endif
