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

/* --------------------------------------------------------------------------- */

#ifndef __COMMONS_DEFS_H
#define __COMMONS_DEFS_H

/* Instances Status, used in local STATUS var */

#define STATUS_DEAD             0
#define STATUS_KILLED           1
#define STATUS_RUNNING          2
#define STATUS_SLEEPING         3
#define STATUS_FROZEN           4
#define STATUS_WAITING_MASK     0x8000

/* New OS_* must be updated in bgdc/src/main_core.c and bgdrtm/src/misc.c */

#define OS_WIN32                0
#define OS_LINUX                1
#define OS_BEOS                 2
#define OS_MACOS                3
#define OS_GP32                 4
#define OS_DC                   5
#define OS_BSD                  6
#define OS_GP2X                 7
#define OS_GP2X_WIZ             8
#define OS_CAANOO               9
#define OS_DINGUX_A320          10
#define OS_WII                  1000
#define OS_PSP                  1001
#define OS_IOS                  1002
#define OS_ANDROID              1003
#define OS_VITA                 1004

#endif

/* --------------------------------------------------------------------------- */
