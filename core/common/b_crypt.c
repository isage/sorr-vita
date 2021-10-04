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

/* ------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "b_crypt.h"

/* ------------------------------------------------------------------------- */

crypt_handle * crypt_create( int method, char * key )
{
    crypt_handle * ch = calloc( sizeof( crypt_handle ), 1 );
    if ( !ch ) return NULL;

    ch->method = method;

    switch ( method )
    {
        case    CRYPT_DES  :
#ifdef USE_LIBDES
                if ( !DES_key_sched( ( DES_cblock * ) key, ch->ks[KEY0] ) ) return ch;
#else
                if ( !DES_key_sched( ( DES_cblock * ) key, &ch->ks[KEY0] ) ) return ch;
#endif
                break;

        case    CRYPT_3DES :
#ifdef USE_LIBDES
                if (  DES_key_sched( ( DES_cblock * )          key, ch->ks[KEY0] ) ) { free( ch ); return NULL; }
                if (  DES_key_sched( ( DES_cblock * ) ( 8  + key ), ch->ks[KEY1] ) ) { free( ch ); return NULL; }
                if ( !DES_key_sched( ( DES_cblock * ) ( 16 + key ), ch->ks[KEY2] ) ) return ch;
#else
                if (  DES_key_sched( ( DES_cblock * )          key, &ch->ks[KEY0] ) ) { free( ch ); return NULL; }
                if (  DES_key_sched( ( DES_cblock * ) ( 8  + key ), &ch->ks[KEY1] ) ) { free( ch ); return NULL; }
                if ( !DES_key_sched( ( DES_cblock * ) ( 16 + key ), &ch->ks[KEY2] ) ) return ch;
#endif
                break;
    }

    free( ch );
    return NULL;
}

/* ------------------------------------------------------------------------- */

void crypt_destroy( crypt_handle * ch )
{
    if ( ch ) free( ch );
}

/* ------------------------------------------------------------------------- */

int crypt_data( crypt_handle * ch, char * in, char * out, int size, int enc )
{
    DES_cblock aux;

    if ( !ch || ( size < 1 || size > 8 ) ) return -1;

    if ( enc && size < 8 ) memset( &in[size], '\0', 8 - size );

    switch( ch->method )
    {
        case    CRYPT_DES   :
                if ( enc )
#ifdef USE_LIBDES
                    DES_ecb_encrypt( ( DES_cblock * ) in, ( DES_cblock * ) out, ch->ks[KEY0], DES_ENCRYPT );
#else
                    DES_ecb_encrypt( ( DES_cblock * ) in, ( DES_cblock * ) out, &ch->ks[KEY0], DES_ENCRYPT );
#endif
                else
#ifdef USE_LIBDES
                    DES_ecb_encrypt( ( DES_cblock * ) in, ( DES_cblock * ) out, ch->ks[KEY0], DES_DECRYPT );
#else
                    DES_ecb_encrypt( ( DES_cblock * ) in, ( DES_cblock * ) out, &ch->ks[KEY0], DES_DECRYPT );
#endif
                break;

        case    CRYPT_3DES  :
                if ( enc )
                {
#ifdef USE_LIBDES
                    DES_ecb_encrypt( ( DES_cblock * )   in, ( DES_cblock * )  out, ch->ks[KEY0], DES_ENCRYPT );
                    DES_ecb_encrypt( ( DES_cblock * )  out, ( DES_cblock * ) &aux, ch->ks[KEY1], DES_DECRYPT );
                    DES_ecb_encrypt( ( DES_cblock * ) &aux, ( DES_cblock * )  out, ch->ks[KEY2], DES_ENCRYPT );
#else
                    DES_ecb_encrypt( ( DES_cblock * )   in, ( DES_cblock * )  out, &ch->ks[KEY0], DES_ENCRYPT );
                    DES_ecb_encrypt( ( DES_cblock * )  out, ( DES_cblock * ) &aux, &ch->ks[KEY1], DES_DECRYPT );
                    DES_ecb_encrypt( ( DES_cblock * ) &aux, ( DES_cblock * )  out, &ch->ks[KEY2], DES_ENCRYPT );
#endif
                }
                else
                {
#ifdef USE_LIBDES
                    DES_ecb_encrypt( ( DES_cblock * )   in, ( DES_cblock * )  out, ch->ks[KEY2], DES_DECRYPT );
                    DES_ecb_encrypt( ( DES_cblock * )  out, ( DES_cblock * ) &aux, ch->ks[KEY1], DES_ENCRYPT );
                    DES_ecb_encrypt( ( DES_cblock * ) &aux, ( DES_cblock * )  out, ch->ks[KEY0], DES_DECRYPT );
#else
                    DES_ecb_encrypt( ( DES_cblock * )   in, ( DES_cblock * )  out, &ch->ks[KEY2], DES_DECRYPT );
                    DES_ecb_encrypt( ( DES_cblock * )  out, ( DES_cblock * ) &aux, &ch->ks[KEY1], DES_ENCRYPT );
                    DES_ecb_encrypt( ( DES_cblock * ) &aux, ( DES_cblock * )  out, &ch->ks[KEY0], DES_DECRYPT );
#endif
                }
                break;
    }

    return ( enc ? 8 : size );
}

/* ------------------------------------------------------------------------- */
