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

#ifndef __FLIC_H
#define __FLIC_H

#include "libgrbase.h"
#include "files.h"
#include "xstrings.h"

#include <SDL_types.h>

#ifndef __GNUC__
#define __PACKED
#define inline __inline
#else
#define __PACKED __attribute__ ((packed))
#endif

#ifdef _MSC_VER
#pragma pack(push, 1)
#endif

/* Reproducción de FLI */
/* ------------------- */

typedef struct
{
    uint32_t  size;          /* Size of FLIC including this header */
    uint16_t  type;          /* File type 0xAF11, 0xAF12, 0xAF30, 0xAF44, ... */
    uint16_t  frames;        /* Number of frames in first segment */
    uint16_t  width;         /* FLIC width in pixels */
    uint16_t  height;        /* FLIC height in pixels */
    uint16_t  depth;         /* Bits per pixel (usually 8) */
    uint16_t  flags;         /* Set to zero or to three */
    uint32_t  speed;         /* Delay between frames */
    uint16_t  reserved1;     /* Set to zero */
    uint32_t  created;       /* Date of FLIC creation (FLC only) */
    uint32_t  creator;       /* Serial number or compiler id (FLC only) */
    uint32_t  updated;       /* Date of FLIC update (FLC only) */
    uint32_t  updater;       /* Serial number (FLC only), see creator */
    uint16_t  aspect_dx;     /* Width of square rectangle (FLC only) */
    uint16_t  aspect_dy;     /* Height of square rectangle (FLC only) */
    uint16_t  ext_flags;     /* EGI: flags for specific EGI extensions */
    uint16_t  keyframes;     /* EGI: key-image frequency */
    uint16_t  totalframes;   /* EGI: total number of frames (segments) */
    uint32_t  req_memory;    /* EGI: maximum chunk size (uncompressed) */
    uint16_t  max_regions;   /* EGI: max. number of regions in a CHK_REGION chunk */
    uint16_t  transp_num;    /* EGI: number of transparent levels */
    uint8_t   reserved2[24]; /* Set to zero */
    uint32_t  oframe1;       /* Offset to frame 1 (FLC only) */
    uint32_t  oframe2;       /* Offset to frame 2 (FLC only) */
    uint8_t   reserved3[40]; /* Set to zero */
}
__PACKED
FLIC_HEADER;

typedef union
{
    struct
    {
        uint32_t size;           /* Size of the chunk */
        uint16_t type;           /* Chunk type */
    }
    header ;

    struct
    {
        uint32_t size;           /* Size of the chunk */
        uint16_t type;           /* Chunk type: 12 */
        uint16_t first_line;     /* First line */
        uint16_t line_count;     /* Number of lines in the chunk */
        uint8_t  data[0];
    }
    delta_fli ;

    struct
    {
        uint32_t size;           /* Size of the chunk  */
        uint16_t type;           /* Chunk type: 15, 12, etc. */
        uint8_t  data[0];
    }
    raw ;
}
FLIC_CHUNK ;

typedef struct
{
    uint32_t size;  /* Size of the frame, including subchunks */
    uint16_t type;  /* Chunk type: 0xF1FA */
    uint16_t chunks; /* Number of subchunks */
    uint8_t  expand[8];
}
FLIC_FRAME ;

typedef struct
{
    FLIC_HEADER header ;
    FLIC_CHUNK  * chunk ;
    FLIC_FRAME  * frame ;
    uint32_t      frame_reserved ;

    GRAPH       * bitmap ;

    file        * fp ;

    int         objid;

    int         current_frame ;
    int         speed_ms ;
    int         last_frame_ms ;
    int         finished ;

    int         x;
    int         y;
    int         z;
    int         angle;
    int         size;
    int         flags;

    int         saved_x;
    int         saved_y;
    int         saved_z;
    int         saved_angle;
    int         saved_size;
    int         saved_flags;

}
FLIC ;

/* Tipos de chunk */

#define CHUNK_COLOR_256  4
#define CHUNK_COLOR_64   11
#define CHUNK_DELTA_FLI  12
#define CHUNK_DELTA_FLC  7
#define CHUNK_BLACK      13
#define CHUNK_BYTE_RUN  15
#define CHUNK_FLI_COPY  16
#define CHUNK_STAMP     18
#define CHUNK_PREFIX  0xF100
#define CHUNK_FRAME   0xF1FA

#ifdef _MSC_VER
#pragma pack(pop)
#endif

#endif
