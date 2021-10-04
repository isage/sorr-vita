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

#ifndef __BITMAP_H
#define __BITMAP_H

#include "g_pal.h"

/* --------------------------------------------------------------------------- */

#define GI_NOCOLORKEY       0x00000001  /* info_flags bits */
#define GI_CLEAN            0x40000000  /* the graphic is clean (no data) */
#define GI_EXTERNAL_DATA    0x80000000  /* data area is external, it don't belong to bitmap */

#define GI_ANALIZE_MASK     GI_NOCOLORKEY

#define CPOINT_UNDEFINED    32767       /* It's enough if X is set to this value */

/* --------------------------------------------------------------------------- */

typedef struct _cpoint
{
    short int x;
    short int y;
}
CPOINT;

typedef struct _pixel_format
{
    PALETTE * palette;  /* Palette for 8 bits graphics */

    uint8_t  depth;       /* bits per pixel */
    uint8_t  depthb;      /* bytes per pixel */

    uint8_t  Rloss;
    uint8_t  Gloss;
    uint8_t  Bloss;
    uint8_t  Aloss;

    uint8_t  Rshift;
    uint8_t  Gshift;
    uint8_t  Bshift;
    uint8_t  Ashift;

    uint32_t Rmask;
    uint32_t Gmask;
    uint32_t Bmask;
    uint32_t Amask;
}
PIXEL_FORMAT;

typedef struct _bitmap
{
    int code;               /* Identifier of the graphic (in the graphic library) */
    char name[ 64 ];        /* Name of the graphic */

    uint32_t width;           /* Width of a bitmap frame in pixels */
    uint32_t height;          /* Height of a bitmap frame in pixels */

    uint32_t pitch;           /* Bytes of distance between bitmap lines */
    uint32_t widthb;          /* Width of a bitmap frame in bytes */

    PIXEL_FORMAT * format;  /* Pixel format */

    /* Runtime use */
    int modified;           /* 0 - bitmap not modified
                               1 - bitmap is modified
                               2 - bitmap is modified and needs analysis
                             */
    int info_flags;         /* Analysis result (see bitmap_analize) */

    void * data;            /* Pointer to the bitmap data at current frame */

    uint32_t ncpoints;        /* Number of control points */
    CPOINT * cpoints;       /* Pointer to the control points ([0] = center) */

    /* Blend Ops */
    int16_t * blend_table;   /* Pointer to 16 bits blend table if any */

    /* Linked list of all bitmaps in memory */
/*
    struct _bitmap * next;
    struct _bitmap * prev;
*/
}
GRAPH;

/* --------------------------------------------------------------------------- */

extern GRAPH * bitmap_new( int code, int w, int h, int depth );
extern GRAPH * bitmap_new_ex( int code, int w, int h, int depth, void * data, int pitch );
extern GRAPH * bitmap_clone( GRAPH * t );
extern GRAPH * bitmap_new_syslib( int w, int h, int depth );
extern void bitmap_destroy( GRAPH * map );
extern void bitmap_destroy_fake( GRAPH * map );
extern void bitmap_add_cpoint( GRAPH *map, int x, int y );
extern void bitmap_set_cpoint( GRAPH * map, uint32_t point, int x, int y );
extern void bitmap_analize( GRAPH * bitmap );

extern PIXEL_FORMAT * bitmap_create_format( int bpp );

extern int bitmap_next_code();

/* --------------------------------------------------------------------------- */

#endif
