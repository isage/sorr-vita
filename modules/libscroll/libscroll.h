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

#ifndef __SCROLL_H
#define __SCROLL_H

/* --------------------------------------------------------------------------- */

#define C_SCROLL    1

/* --------------------------------------------------------------------------- */

#ifndef __GNUC__
#define __PACKED
#define inline __inline
#else
#define __PACKED __attribute__ ((packed))
#endif

#ifdef _MSC_VER
#pragma pack(push, 1)
#endif

typedef struct _scrolldata
{
    int fileid ;
    int graphid ;
    int backid ;
    REGION * region ;
    int flags ;

    int x0, y0 ;
    int posx0, posy0 ;
    int x1, y1 ;
    int z ;

    INSTANCE * camera ;

    int ratio ;
    int speed ;
    REGION * region1 ;
    REGION * region2 ;

    int destfile;
    int destid;

    int active ;

    struct _scrolldata * follows ;
}
__PACKED
scrolldata ;

typedef struct _scroll_Extra_data
{
    int32_t x0 ;
    int32_t y0 ;
    int32_t x1 ;
    int32_t y1 ;
    int32_t z ;
    int32_t camera ;
    int32_t ratio ;
    int32_t speed ;
    int32_t region1 ;
    int32_t region2 ;
    int32_t flags1 ;
    int32_t flags2 ;
    int32_t follows ;
    int32_t reserved[7] ;       /* First reserved dword point to internal scrolldata struct */
}
__PACKED
SCROLL_EXTRA_DATA ;

#ifdef _MSC_VER
#pragma pack(pop)
#endif

/* --------------------------------------------------------------------------- */

extern scrolldata  scrolls[10] ;

/* --------------------------------------------------------------------------- */

extern void scroll_start( int n, int fileid, int graphid, int backid, int region, int flags, int destfile, int destid ) ;
extern void scroll_stop( int n ) ;
extern void scroll_update( int n );
extern void scroll_draw( int n, REGION * clipping ) ;
extern void scroll_region( int n, REGION * r ) ;

/* --------------------------------------------------------------------------- */

#endif
