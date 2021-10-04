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

#ifndef __LIBJOY_H
#define __LIBJOY_H

extern int libjoy_num( void );
extern int libjoy_name( int joy );
extern int libjoy_select( int joy );
extern int libjoy_buttons( void );
extern int libjoy_axes( void );
extern int libjoy_get_button( int button );
extern int libjoy_get_position( int axis );
extern int libjoy_hats( void );
extern int libjoy_balls( void );
extern int libjoy_get_hat( int hat );
extern int libjoy_get_ball( int ball, int * dx, int * dy );
extern int libjoy_buttons_specific( int joy );
extern int libjoy_axes_specific( int joy );
extern int libjoy_get_button_specific( int joy, int button );
extern int libjoy_get_position_specific( int joy, int axis );
extern int libjoy_hats_specific( int joy );
extern int libjoy_balls_specific( int joy );
extern int libjoy_get_hat_specific( int joy, int hat );
extern int libjoy_get_ball_specific( int joy, int ball, int * dx, int * dy );
extern int libjoy_get_accel( int * x, int * y, int * z );
extern int libjoy_get_accel_specific( int joy, int * x, int * y, int * z );

#endif
