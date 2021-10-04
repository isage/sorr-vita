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

#ifndef __RENDER_H
#define __RENDER_H

/* --------------------------------------------------------------------------- */

#ifdef __LIB_RENDER
#include "bgdrtm.h"

#include "dlvaracc.h"
#include "bgddl.h"

#include "libgrbase.h"
#include "libvideo.h"
#include "libblit.h"
#endif

#include "g_fade.h"
#include "g_frame.h"
#include "g_instance.h"
#include "g_object.h"
#include "g_rects.h"
#include "g_screen.h"

/* --------------------------------------------------------------------------- */

#define C_SCREEN            0

/* --------------------------------------------------------------------------- */

#ifdef __LIB_RENDER

/* Globals */

enum {
    FPS = 0,
    SPEED_GAUGE,
    FRAME_TIME,

    SCALE_MODE,
    RESTORETYPE,
    DUMPTYPE,

    FADING,
    ALPHA_STEPS
};

/* Locals */

enum {
    CTYPE = 0,
    CNUMBER,

    COORDX,
    COORDY,
    COORDZ,
    FILEID,
    GRAPHID,
    GRAPHSIZE,
    ANGLE,
    FLAGS,
    ALPHA,
    PALETTEID,
    REGIONID,
    RESOLUTION,
    GRAPHSIZEX,
    GRAPHSIZEY,
    BLENDOP,
    XGRAPH,

    OBJECTID,
    GRAPHPTR,
    XGRAPH_FLAGS,

    SAVED_COORDX,
    SAVED_COORDY,
    SAVED_COORDZ,
    SAVED_FILEID,
    SAVED_GRAPHID,
    SAVED_GRAPHSIZE,
    SAVED_ANGLE,
    SAVED_FLAGS,
    SAVED_ALPHA,
    SAVED_PALETTE,
    SAVED_GRAPHSIZEX,
    SAVED_GRAPHSIZEY,
    SAVED_BLENDOP,
    SAVED_XGRAPH,
    SAVED_CENTERX,
    SAVED_CENTERY,

    STATUS
};

/* --------------------------------------------------------------------------- */

extern DLVARFIXUP __bgdexport( librender, globals_fixup )[];
extern DLVARFIXUP __bgdexport( librender, locals_fixup )[];

/* --------------------------------------------------------------------------- */
#endif

#endif

