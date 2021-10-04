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

#ifndef __FRAME_H
#define __FRAME_H

/* --------------------------------------------------------------------------- */

extern uint32_t frame_count ;
extern int last_frame_ticks ;
extern int next_frame_ticks ;
extern float frame_ms ;
extern int max_jump ;
extern int current_jump ;
extern int jump ;
extern int FPS_count ;
extern int FPS_init ;

/* --------------------------------------------------------------------------- */

extern void gr_set_fps( int fps, int jump );
extern void gr_wait_frame();
extern void gr_draw_frame();

extern void gr_refresh_palette();

/* --------------------------------------------------------------------------- */

#endif
