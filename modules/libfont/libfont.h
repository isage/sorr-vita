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

#ifndef __FONT_H
#define __FONT_H

#include "libgrbase.h"

/* -------------------------------------------------------------------------- */

#define MAX_FONTS       256

#define NFB_FIXEDWIDTH  1

/* -------------------------------------------------------------------------- */

enum
{
    CHARSET_ISO8859 = 0,
    CHARSET_CP850   = 1
};

/* -------------------------------------------------------------------------- */

typedef struct _font
{
    int         charset;
    uint32_t    bpp;

    struct _glyph
    {
        GRAPH * bitmap;
        int     xoffset;
        int     yoffset;
        int     xadvance;
        int     yadvance;
    }
    glyph[256] ;

    int         maxheight;
    int         maxwidth;
}
FONT ;

/* -------------------------------------------------------------------------- */

extern unsigned char default_font[256*8];

/* -------------------------------------------------------------------------- */

extern FONT * fonts[MAX_FONTS] ;
extern int font_count ;

/* -------------------------------------------------------------------------- */

extern void gr_font_destroy( int fontid ) ;
extern FONT * gr_font_get( int id );
extern int gr_font_new( int charset, uint32_t bpp ) ;
extern int gr_font_newfrombitmap( GRAPH * map, int charset, int width, int height, int first, int last, int options ) ;
extern int gr_font_systemfont( char * chardata ) ;

/* -------------------------------------------------------------------------- */

#endif
