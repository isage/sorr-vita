/*
 *  Copyright © 2006-2012 SplinterGU (Fenix/Bennugd)
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

#ifndef __B_CRYPT_H
#define __B_CRYPT_H

/* ------------------------------------------------------------------------- */

#ifdef USE_LIBDES
#include <des.h>
#define DES_key_schedule    des_key_schedule
#define DES_key_sched       key_sched
#define DES_ecb_encrypt     des_ecb_encrypt
#define DES_cblock          des_cblock
#else
#include <openssl/des.h>
#endif

/* ------------------------------------------------------------------------- */

enum {
    CRYPT_NONE = 0,
    CRYPT_DES,
    CRYPT_3DES
};

enum {
    KEY0 = 0,
    KEY1,
    KEY2,
    MAX_KEYS
};

/* ------------------------------------------------------------------------- */

typedef struct
{
    int method;
    DES_key_schedule ks[MAX_KEYS];
} crypt_handle;

/* ------------------------------------------------------------------------- */

extern crypt_handle * crypt_create( int method, char * key );
extern void crypt_destroy( crypt_handle * ch );
extern int crypt_data( crypt_handle * ch, char * in, char * out, int size, int enc );

/* ------------------------------------------------------------------------- */

#endif
