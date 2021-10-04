/*
 *  BennuGD - Videogame compiler/interpreter
 *  Project documentation : http://www.bennugd.org
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 *  Copyright © 1999 José Luis Cebrián Pagüe
 *  Copyright © 2002 Fenix Team
 *  Copyright © 2009 Joseba García Etxebarria
 *
 */

/*
 * FILE        : libimage.c
 * DESCRIPTION : sdlf_Image BennuGD support DLL
 */

#ifdef WIN32
#include <windows.h>
#include <winbase.h>
#endif

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#ifndef TARGET_WII
#include <memory.h>
#endif
#include <SDL_image.h>
/* BennuGD stuff */
#include <files.h>
#include <bgddl.h>
#include <xstrings.h>
#include <libgrbase.h>
#include <g_video.h>

#ifndef __MONOLITHIC__
#include "mod_image_symbols.h"
#endif


static int sdlf_seek (SDL_RWops * context, int offset, int whence);
static int sdlf_read (SDL_RWops * context, void * ptr, int size, int count);
static int sdlf_write (SDL_RWops * context, const void * ptr, int size, int count);
static int sdlf_close (SDL_RWops * context);

enum
{
	TYPE_BMP = 0x001,
	TYPE_GIF = 0x002,
	TYPE_JPG = 0x004,
	TYPE_LBM = 0x008,
	TYPE_PCX = 0x010,
	TYPE_PNG = 0x020,
	TYPE_PNM = 0x040,
	TYPE_TIF = 0x080,
	TYPE_XPM = 0x100,
	TYPE_XCF = 0x200
};

/*
 *  FUNCTION : file_RWops
 *
 *  Given a Fenix file object, create a SDL_RWops for SDL
 *
 *  PARAMS :
 *		file			Pointer to the file object
 *
 *  RETURN VALUE :
 *      Pointer to the SDL_RWops struct
 *
 */

SDL_RWops * file_RWops (file * fp)
{
	static SDL_RWops * sdlf;

	sdlf = SDL_AllocRW();
	if (sdlf == NULL) return NULL;

	sdlf->seek = sdlf_seek;
	sdlf->read = sdlf_read;
	sdlf->write = sdlf_write;
	sdlf->close = sdlf_close;
	sdlf->hidden.unknown.data1 = fp;

	return sdlf;
}

/* Stub functions for RWops */

static int sdlf_seek (SDL_RWops * context, int offset, int whence)
{
	file * fp = (file *) context->hidden.unknown.data1;
	file_seek (fp, offset, whence);
	return file_pos (fp);
}

static int sdlf_read (SDL_RWops * context, void * ptr, int size, int count)
{
	file * fp = (file *) context->hidden.unknown.data1;
	if (count == 0) return -1;
	return file_read (fp, ptr, size * count) / size;
}

static int sdlf_write (SDL_RWops * context, const void * ptr, int size, int count)
{
	file * fp = (file *) context->hidden.unknown.data1;
	if (count == 0) return -1;
	return file_write (fp, ptr, size * count) / size;
}

static int sdlf_close (SDL_RWops * context)
{
	/* We don't close the file here with file_close, because the SDL_RWops
	 * is created from an existing file * pointer that could be closed
	 * elsewhere! */
	return 1;
}

/*
 *  FUNCTION : gr_load_image
 *
 *  Load an libimage using SDL_image functions
 *
 *  PARAMS :
 *		filename		File name
 *
 *  RETURN VALUE :
 *      ID of the new libimage or 0 if error
 *
 */

int gr_load_image (const char * filename)
{
    file            * fp;
    GRAPH           * gr = NULL, * gr1 = NULL;
    SDL_RWops       * rwops;
    SDL_Surface     * s = NULL , * aux = NULL;

    /* Some libimage loaders may not be compatible with the file_* functions
    * (for example, back-seeks in compressed gzip files are not supported) */
    s = IMG_Load (filename);

    /* Try to open compressed/packed files */
    if (s == NULL)
    {
        fp = file_open (filename, "rb");
        if (fp == NULL) {
            printf("Image: Couldn't load %s\n", filename);
            return 0;
        }
        rwops = file_RWops (fp);
        if (rwops != NULL) s = IMG_Load_RW (rwops, 1);
        SDL_FreeRW(rwops);
        file_close(fp);
    }

    /* Convert the SDL_Surface to a Bennu MAP */
    if (s != NULL)
    {
        //printf("Image loaded with %dbpp\n", s->format->BitsPerPixel);
        // Create the map from the surface raw bitmap data
        // Convert it if needed.

        if(s->format->BitsPerPixel != screen->format->BitsPerPixel)
        {
            //printf("Need to convert the surface to %dbpp.\n", screen->format->BitsPerPixel);
            // Duplicate the s surface to aux, free s surface
            aux = SDL_ConvertSurface(s, screen->format, 0);
            SDL_FreeSurface(s);
            if(aux == NULL)
            {
                printf("Image error: Couldn't convert the libimage");
                return 0;
            }
            s = aux;
        }

        gr1 = bitmap_new_ex(0, s->w, s->h, s->format->BitsPerPixel, s->pixels, s->pitch);

        // Make the data permanent
        gr = bitmap_clone(gr1);

        if ( gr->format->depth == 32 )
        {
            int h, w;
            char * src = gr->data;
            int * ptr = src;

            for ( h = gr->height; h--; )
            {
                for ( w = gr->width; w--; )
                {
                    if ( *ptr & 0x00FFFFFF ) *ptr |= 0x0FF000000 ;
                    ptr++;
                }
                ptr = ( src += gr->pitch );
            }
        }

        // Free the temporary graph & assign graph code
        bitmap_destroy(gr1);
        gr1 = NULL;

        gr->code = bitmap_next_code();
        grlib_add_map(0, gr);

        // Free surface and unlock it (if needed)
        SDL_FreeSurface(s);
    }
    else
    {
        printf("Image: Couldn't load %s\n", filename);
    }

    return ( gr != NULL ) ? gr->code : 0;
}

/*
 *  FUNCTION : gr_image_type
 *
 *  Check the libimage type of a file using the SDL_image functions
 *
 *  PARAMS :
 *		filename		File name
 *		type			Type of the file, from the IMAGETYPES enum
 *
 *  RETURN VALUE :
 *      1 if the libimage has such a type, 0 otherwise
 */

int gr_image_type (const char * filename, int type)
{
	file        * fp = file_open (filename, "rb");
	SDL_RWops   * rwops;
	int           result = 0;

	if (fp == NULL) return 0;
	rwops = file_RWops (fp);
	if (rwops != NULL)
	{
		if (type & TYPE_BMP) result |= IMG_isBMP(rwops) ? 1:0;
		if (type & TYPE_PNM) result |= IMG_isPNM(rwops) ? 1:0;
		if (type & TYPE_XPM) result |= IMG_isXPM(rwops) ? 1:0;
		if (type & TYPE_XCF) result |= IMG_isXCF(rwops) ? 1:0;
		if (type & TYPE_PCX) result |= IMG_isPCX(rwops) ? 1:0;
		if (type & TYPE_GIF) result |= IMG_isGIF(rwops) ? 1:0;
		if (type & TYPE_JPG) result |= IMG_isJPG(rwops) ? 1:0;
		if (type & TYPE_TIF) result |= IMG_isTIF(rwops) ? 1:0;
		if (type & TYPE_PNG) result |= IMG_isPNG(rwops) ? 1:0;
		if (type & TYPE_LBM) result |= IMG_isLBM(rwops) ? 1:0;

		SDL_FreeRW(rwops);
	}
	file_close(fp);
	return result;
}

/* --------------------------- Fenix functions --------------------------- */

/*
 *  LOAD_IMAGE (STRING file)
 */
int bgd_load_image (INSTANCE * i, int * params)
{
	const char * ptr = string_get(params[0]);
	int result = gr_load_image(ptr);
	string_discard(params[0]);
	return result;
}

/*
 *  IS_PCX (STRING file)
 */
int bgd_is_PCX (INSTANCE * i, int * params)
{
	const char * ptr = string_get(params[0]);
	int result = gr_image_type(ptr, TYPE_PCX);
	string_discard(params[0]);
	return result;
}

/*
 *  IS_GIF (STRING file)
 */
int bgd_is_GIF (INSTANCE * i, int * params)
{
	const char * ptr = string_get(params[0]);
	int result = gr_image_type(ptr, TYPE_GIF);
	string_discard(params[0]);
	return result;
}

/*
 *  IS_JPG (STRING file)
 */
int bgd_is_JPG (INSTANCE * i, int * params)
{
	const char * ptr = string_get(params[0]);
	int result = gr_image_type(ptr, TYPE_JPG);
	string_discard(params[0]);
	return result;
}

/*
 *  IS_PNG (STRING file)
 */
int bgd_is_PNG (INSTANCE * i, int * params)
{
	const char * ptr = string_get(params[0]);
	int result = gr_image_type(ptr, TYPE_PNG);
	string_discard(params[0]);
	return result;
}

/*
 *  IS_XPM (STRING file)
 */
int bgd_is_XPM (INSTANCE * i, int * params)
{
	const char * ptr = string_get(params[0]);
	int result = gr_image_type(ptr, TYPE_XPM);
	string_discard(params[0]);
	return result;
}

/*
 *  IS_BMP (STRING file)
 */
int bgd_is_BMP (INSTANCE * i, int * params)
{
	const char * ptr = string_get(params[0]);
	int result = gr_image_type(ptr, TYPE_BMP);
	string_discard(params[0]);
	return result;
}

/*
 *  IS_PNM (STRING file)
 */
int bgd_is_PNM (INSTANCE * i, int * params)
{
	const char * ptr = string_get(params[0]);
	int result = gr_image_type(ptr, TYPE_PNM);
	string_discard(params[0]);
	return result;
}

/*
 *  IS_TIF (STRING file)
 */
int bgd_is_TIF (INSTANCE * i, int * params)
{
	const char * ptr = string_get(params[0]);
	int result = gr_image_type(ptr, TYPE_TIF);
	string_discard(params[0]);
	return result;
}

/*
 *  IS_LBM (STRING file)
 */
int bgd_is_LBM (INSTANCE * i, int * params)
{
	const char * ptr = string_get(params[0]);
	int result = gr_image_type(ptr, TYPE_LBM);
	string_discard(params[0]);
	return result;
}

/*
 *  IS_XCF (STRING file)
 */
int bgd_is_XCF (INSTANCE * i, int * params)
{
	const char * ptr = string_get(params[0]);
	int result = gr_image_type(ptr, TYPE_XCF);
	string_discard(params[0]);
	return result;
}


