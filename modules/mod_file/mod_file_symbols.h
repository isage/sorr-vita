/*
 *  Copyright © 2006-2012 SplinterGU (Fenix/Bennugd)
 *  Copyright © 2002-2006 Fenix Team (Fenix)
 *  Copyright © 1999-2002 José Luis Cebrián Pagüe (Fenix)
 *
 *  This file is part of Bennu - Game Development
 *
 *  Bennu is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Bennu is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 */

#ifndef __MODFILE_H
#define __MODFILE_H

#include <bgddl.h>

#ifdef __BGDC__
DLCONSTANT  __bgdexport( mod_file, constants_def)[] =
{
    { "O_READ"      , TYPE_INT, 0  },
    { "O_READWRITE" , TYPE_INT, 1  },
    { "O_RDWR"      , TYPE_INT, 1  },
    { "O_WRITE"     , TYPE_INT, 2  },
    { "O_ZREAD"     , TYPE_INT, 3  },
    { "O_ZWRITE"    , TYPE_INT, 4  },

    { "SEEK_SET"    , TYPE_INT, 0  },
    { "SEEK_CUR"    , TYPE_INT, 1  },
    { "SEEK_END"    , TYPE_INT, 2  },

    { NULL          , 0       , 0  }
} ;

DLSYSFUNCS  __bgdexport( mod_file, functions_exports)[] =
{
    /* Ficheros */
    { "SAVE"        , "SV++" , TYPE_INT         , 0 },
    { "LOAD"        , "SV++" , TYPE_INT         , 0 },
    { "FOPEN"       , "SI"   , TYPE_INT         , 0 },
    { "FCLOSE"      , "I"    , TYPE_INT         , 0 },
    { "FREAD"       , "IV++" , TYPE_INT         , 0 },
    { "FREAD"       , "PII"  , TYPE_INT         , 0 },
    { "FWRITE"      , "IV++" , TYPE_INT         , 0 },
    { "FWRITE"      , "PII"  , TYPE_INT         , 0 },
    { "FSEEK"       , "III"  , TYPE_INT         , 0 },
    { "FREWIND"     , "I"    , TYPE_UNDEFINED   , 0 },
    { "FTELL"       , "I"    , TYPE_INT         , 0 },
    { "FLENGTH"     , "I"    , TYPE_INT         , 0 },
    { "FPUTS"       , "IS"   , TYPE_INT         , 0 },
    { "FGETS"       , "I"    , TYPE_STRING      , 0 },
    { "FEOF"        , "I"    , TYPE_INT         , 0 },
    { "FILE"        , "S"    , TYPE_STRING      , 0 },
    { "FEXISTS"     , "S"    , TYPE_INT         , 0 } ,
    { "FILE_EXISTS" , "S"    , TYPE_INT         , 0 } ,
    { "FREMOVE"     , "S"    , TYPE_INT         , 0 } ,
    { "FMOVE"       , "SS"   , TYPE_INT         , 0 } ,
    { 0             , 0      , 0                , 0 }
};
#else
extern DLCONSTANT  __bgdexport( mod_file, constants_def)[];
extern DLSYSFUNCS  __bgdexport( mod_file, functions_exports)[];
#endif

#endif
