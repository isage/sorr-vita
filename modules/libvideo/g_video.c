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

#include <stdlib.h>
#include <string.h>

#include "bgdrtm.h"

#include "bgddl.h"
#include "dlvaracc.h"

#include "libvideo.h"

#ifdef _WIN32
#include <initguid.h>
#include "ddraw.h"
#endif

/* --------------------------------------------------------------------------- */

GRAPH * icon = NULL ;

SDL_Surface * screen = NULL ;
SDL_Window * window = NULL;
SDL_Renderer * renderer = NULL;
SDL_Texture * texture = NULL;

char * apptitle = NULL ;

int scr_width = 320 ;
int scr_height = 240 ;
int renderer_width = 320 ;
int renderer_height = 240 ;

int scr_initialized = 0 ;

int enable_16bits = 0 ;
int enable_32bits = 0 ;
int full_screen = 0 ;
int double_buffer = 0 ;
int hardware_scr = 0 ;
int grab_input = 0 ;
int frameless = 0 ;
int waitvsync = 0 ;

int scale_resolution = 0;
int scale_resolution_aspectratio = 0;
int scale_resolution_orientation = 0;

/* --------------------------------------------------------------------------- */

DLCONSTANT  __bgdexport( libvideo, constants_def )[] =
{
    { "M320X200"            , TYPE_DWORD    , 3200200               },
    { "M320X240"            , TYPE_DWORD    , 3200240               },
    { "M320X400"            , TYPE_DWORD    , 3200400               },
    { "M360X240"            , TYPE_DWORD    , 3600240               },
    { "M376X282"            , TYPE_DWORD    , 3760282               },
    { "M400X300"            , TYPE_DWORD    , 4000300               },
    { "M512X384"            , TYPE_DWORD    , 5120384               },
    { "M640X400"            , TYPE_DWORD    , 6400400               },
    { "M640X480"            , TYPE_DWORD    , 6400480               },
    { "M800X600"            , TYPE_DWORD    , 8000600               },
    { "M1024X768"           , TYPE_DWORD    , 10240768              },
    { "M1280X1024"          , TYPE_DWORD    , 12801024              },

    { "MODE_WINDOW"         , TYPE_DWORD    , MODE_WINDOW           },
    { "MODE_2XSCALE"        , TYPE_DWORD    , MODE_2XSCALE          },
    { "MODE_FULLSCREEN"     , TYPE_DWORD    , MODE_FULLSCREEN       },
    { "MODE_DOUBLEBUFFER"   , TYPE_DWORD    , MODE_DOUBLEBUFFER     },
    { "MODE_HARDWARE"       , TYPE_DWORD    , MODE_HARDWARE         },

    { "MODE_WAITVSYNC"      , TYPE_DWORD    , MODE_WAITVSYNC        },
    { "WAITVSYNC"           , TYPE_DWORD    , MODE_WAITVSYNC        },

    { "DOUBLE_BUFFER"       , TYPE_DWORD    , MODE_DOUBLEBUFFER     },  /* Obsolete */
    { "HW_SURFACE"          , TYPE_DWORD    , MODE_HARDWARE         },  /* Obsolete */

    { "MODE_8BITS"          , TYPE_DWORD    , 8                     },
    { "MODE_16BITS"         , TYPE_DWORD    , 16                    },
    { "MODE_32BITS"         , TYPE_DWORD    , 32                    },

    { "MODE_8BPP"           , TYPE_DWORD    , 8                     },
    { "MODE_16BPP"          , TYPE_DWORD    , 16                    },
    { "MODE_32BPP"          , TYPE_DWORD    , 32                    },

    { "MODE_MODAL"          , TYPE_DWORD    , MODE_MODAL            },  /* GRAB INPU */
    { "MODE_FRAMELESS"      , TYPE_DWORD    , MODE_FRAMELESS        },  /* FRAMELESS window */

    { "SCALE_NONE"          , TYPE_DWORD    , SCALE_NONE            },

    { "SRO_NORMAL"          , TYPE_DWORD    , SRO_NORMAL            },
    { "SRO_LEFT"            , TYPE_DWORD    , SRO_LEFT              },
    { "SRO_DOWN"            , TYPE_DWORD    , SRO_DOWN              },
    { "SRO_RIGHT"           , TYPE_DWORD    , SRO_RIGHT             },

    { "SRA_STRETCH"         , TYPE_DWORD    , SRA_STRETCH           },
    { "SRA_PRESERVE"        , TYPE_DWORD    , SRA_PRESERVE          },

    { NULL                  , 0             , 0                     }
} ;

/* --------------------------------------------------------------------------- */

enum {
    GRAPH_MODE = 0,
    SCALE_MODE,
    FULL_SCREEN,
    SCALE_RESOLUTION,
    SCALE_RESOLUTION_ASPECTRATIO,
    SCALE_RESOLUTION_ORIENTATION
};

/* --------------------------------------------------------------------------- */
/* Definicion de variables globales (usada en tiempo de compilacion) */

char * __bgdexport( libvideo, globals_def ) =
    "graph_mode = 0;\n"
    "scale_mode = 0;\n"
    "full_screen = 0;\n"
    "scale_resolution = 0;\n"
    "scale_resolution_aspectratio = 0;\n"
    "scale_resolution_orientation = 0;\n"
    ;

/* --------------------------------------------------------------------------- */
/* Son las variables que se desea acceder.                           */
/* El interprete completa esta estructura, si la variable existe.    */
/* (usada en tiempo de ejecucion)                                    */

DLVARFIXUP __bgdexport( libvideo, globals_fixup )[] =
{
    /* Nombre de variable global, puntero al dato, tamaño del elemento, cantidad de elementos */
    { "graph_mode" , NULL, -1, -1 },
    { "scale_mode" , NULL, -1, -1 },
    { "full_screen" , NULL, -1, -1 },
    { "scale_resolution", NULL, -1, -1 },

    /* new vars for use with scale_resolution */
    { "scale_resolution_aspectratio", NULL, -1, -1 },
    { "scale_resolution_orientation", NULL, -1, -1 },

    { NULL , NULL, -1, -1 }
};

/* --------------------------------------------------------------------------- */

#ifdef _WIN32
/* Based on allegro */

LPDIRECTDRAW2 directdraw = NULL;
DDCAPS ddcaps;

HRESULT WINAPI( *_DirectDrawCreate )( GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, IUnknown FAR *pUnkOuter );

/* --------------------------------------------------------------------------- */

int init_dx( void )
{
    HINSTANCE handle;
    LPDIRECTDRAW directdraw1;
    HRESULT hr;
    LPVOID temp;

    handle = LoadLibrary( "DDRAW.DLL" );
    if ( handle == NULL ) return -1;

    _DirectDrawCreate = GetProcAddress( handle, "DirectDrawCreate" );

    hr = _DirectDrawCreate( NULL, &directdraw1, NULL );
    if ( FAILED( hr ) ) return -1;

    hr = IDirectDraw_QueryInterface( directdraw1, &IID_IDirectDraw2, &directdraw );
    if ( FAILED( hr ) ) return -1;

    IDirectDraw_Release( directdraw1 );

    hr = IDirectDraw2_SetCooperativeLevel( directdraw, NULL, DDSCL_NORMAL );
    if ( FAILED( hr ) ) return -1;

    /* get capabilities */
    ddcaps.dwSize = sizeof( ddcaps );
    hr = IDirectDraw2_GetCaps( directdraw, &ddcaps, NULL );
    if ( FAILED( hr ) ) return -1;

    return 0;
}
#endif

/* --------------------------------------------------------------------------- */

void gr_wait_vsync()
{
#ifdef _WIN32
    if ( directdraw ) IDirectDraw2_WaitForVerticalBlank( directdraw, DDWAITVB_BLOCKBEGIN, NULL );
#endif
}

/* --------------------------------------------------------------------------- */

void gr_set_caption( char * title )
{
    SDL_SetWindowTitle(window, apptitle = title);
}

/* --------------------------------------------------------------------------- */

int gr_set_icon( GRAPH * map )
{
    if (( icon = map ))
    {
        SDL_Surface *ico = NULL;
        if ( icon->format->depth == 8 )
        {
            SDL_Color palette[256];
            if ( sys_pixel_format && sys_pixel_format->palette )
            {
                int n ;
                for ( n = 0 ; n < 256 ; n++ )
                {
                    palette[ n ].r = sys_pixel_format->palette->rgb[ n ].r;
                    palette[ n ].g = sys_pixel_format->palette->rgb[ n ].g;
                    palette[ n ].b = sys_pixel_format->palette->rgb[ n ].b;
                }
            }

            ico = SDL_CreateRGBSurfaceFrom( icon->data, 32, 32, 8, 32, 0x00, 0x00, 0x00, 0x00 ) ;

            SDL_SetPaletteColors(ico->format->palette, palette, 0, 256);
        }
        else
        {
            ico = SDL_CreateRGBSurfaceFrom( icon->data, 32, 32, icon->format->depth, icon->pitch, icon->format->Rmask, icon->format->Gmask, icon->format->Bmask, icon->format->Amask ) ;
        }

        SDL_SetWindowIcon(window, ico);
        SDL_FreeSurface( ico ) ;
    }

    return 1 ;
}

/* --------------------------------------------------------------------------- */

int gr_set_mode( int width, int height, int depth )
{
    int n ;
    int sdl_flags = 0;
    int surface_width = 960;//width;
    int surface_height = 544;//height;
    int texture_depth = 0;
    Uint32 format = 0;
    Uint32 Rmask = 0;
    Uint32 Gmask = 0;
    Uint32 Bmask = 0;
    Uint32 Amask = 0;
    char * e;

    SDL_Log("Called set_mode with %dx%dx%d", width, height, depth);

    full_screen = ( GLODWORD( libvideo, GRAPH_MODE ) & MODE_FULLSCREEN ) ? 1 : 0 ;
    double_buffer = ( GLODWORD( libvideo, GRAPH_MODE ) & MODE_DOUBLEBUFFER ) ? 1 : 0 ;
    hardware_scr = ( GLODWORD( libvideo, GRAPH_MODE ) & MODE_HARDWARE ) ? 1 : 0 ;
    grab_input = ( GLODWORD( libvideo, GRAPH_MODE ) & MODE_MODAL ) ? 1 : 0 ;
    frameless = ( GLODWORD( libvideo, GRAPH_MODE ) & MODE_FRAMELESS ) ? 1 : 0 ;
    waitvsync = ( GLODWORD( libvideo, GRAPH_MODE ) & MODE_WAITVSYNC ) ? 1 : 0 ;
    full_screen |= GLODWORD( libvideo, FULL_SCREEN );

    scale_resolution = GLODWORD( libvideo, SCALE_RESOLUTION );

    if ( GLOEXISTS( libvideo, SCALE_RESOLUTION_ASPECTRATIO ) ) scale_resolution_aspectratio = GLODWORD( libvideo, SCALE_RESOLUTION_ASPECTRATIO );
    if ( GLOEXISTS( libvideo, SCALE_RESOLUTION_ORIENTATION ) ) scale_resolution_orientation = GLODWORD( libvideo, SCALE_RESOLUTION_ORIENTATION );

    /* Overwrite all params */

    if ( ( e = getenv( "SCALE_RESOLUTION"             ) ) ) scale_resolution = atol( e );
    if ( ( e = getenv( "SCALE_RESOLUTION_ASPECTRATIO" ) ) ) scale_resolution_aspectratio = atol( e );
    if ( ( e = getenv( "SCALE_RESOLUTION_ORIENTATION" ) ) ) scale_resolution_orientation = atol( e );

    if ( scale_resolution_orientation < 0 || scale_resolution_orientation > 4 ) scale_resolution_orientation = 0;

    if ( !depth )
    {
        enable_32bits = ( GLODWORD( libvideo, GRAPH_MODE ) & MODE_32BITS ) ? 1 : 0 ;
        if ( !enable_32bits )
            enable_16bits = ( GLODWORD( libvideo, GRAPH_MODE ) & MODE_16BITS ) ? 1 : 0 ;
        else
            enable_16bits = 0;
        depth = enable_32bits ? 32 : ( enable_16bits ? 16 : 8 );
    }
    else if ( depth == 16 )
    {
        enable_16bits = 1;
        enable_32bits = 0;
    }
    else if ( depth == 32 )
    {
        enable_16bits = 0;
        enable_32bits = 1;
    }

    /* Inicializa el modo gráfico */

    if ( scrbitmap )
    {
        bitmap_destroy( scrbitmap ) ;
        scrbitmap = NULL ;
    }

    // Use the new & fancy SDL 2 routines
    if(!window && !renderer) {
        sdl_flags = SDL_WINDOW_SHOWN;
        if (frameless) {
            sdl_flags |= SDL_WINDOW_BORDERLESS;
        } else {
        }
        if (full_screen) {
            sdl_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
        }
        if (grab_input) {
            sdl_flags |= SDL_WINDOW_INPUT_GRABBED;
        }
        window = SDL_CreateWindow(apptitle,
                                  SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                  surface_width, surface_height, sdl_flags);
        if(window) {
            sdl_flags = 0;
            if (waitvsync) {
                sdl_flags = SDL_RENDERER_PRESENTVSYNC;
            }
            renderer = SDL_CreateRenderer(window, -1, sdl_flags);
            // Store the renderer resolution
            SDL_GetRendererOutputSize(renderer, &renderer_width, &renderer_height);
        }
        if (!window || !renderer) {
            SDL_Log("Error creating window and/or renderer (%s)", SDL_GetError());
            return -1;
        }
    }

    // Clear the screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    // Enable SDL scaling, if needed
    if(surface_width != width || surface_height != height) {
        // I should add support for this from BennuGD
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");  // make the scaled rendering look smoother.
        SDL_RenderSetLogicalSize(renderer, width, height);
    }

    // This way we can force only one of the sizes (or both) to be native
    if (width == 0) {
        SDL_GetWindowSize(window, &width, NULL);
    }
    if (height == 0) {
        SDL_GetWindowSize(window, NULL, &height);
    }

    // What's the mode for 8bpp?
    // We could support BGR/YUV and other display modes here, too
    if(enable_16bits) {
        format = SDL_PIXELFORMAT_RGB565;
    } else if (enable_32bits) {
        format = SDL_PIXELFORMAT_RGBA8888;
    }
    texture = SDL_CreateTexture(renderer,
                                   format,
                                   SDL_TEXTUREACCESS_STREAMING,
                                   width, height);

    // Create a SDL_Surface for the pixel data until the complete rendering pipeline
    // is handled by SDL_Render
    SDL_PixelFormatEnumToMasks(format, &texture_depth, &Rmask, &Gmask, &Bmask, &Amask);
    screen = SDL_CreateRGBSurface(0, width, height, texture_depth, Rmask, Gmask, Bmask, Amask);
    if (depth != texture_depth) {
        SDL_Log("You asked for %dbpp but got %d, bad luck :(", depth, texture_depth);
    }

    if ( !sys_pixel_format )
    {
        sys_pixel_format = bitmap_create_format( depth );
    }
    else
    {
        PALETTE * p = sys_pixel_format->palette;

        free( sys_pixel_format );
        sys_pixel_format = bitmap_create_format( depth );

        if ( p )
        {
            sys_pixel_format->palette = p;
            pal_refresh( sys_pixel_format->palette ) ;
        }
    }

    if ( sys_pixel_format->depth == 16 )
    {
        for ( n = 0 ; n < 65536 ; n++ )
        {
            colorghost[ n ] =
                ((( n & screen->format->Rmask ) >> 1 ) & screen->format->Rmask ) +
                ((( n & screen->format->Gmask ) >> 1 ) & screen->format->Gmask ) +
                ((( n & screen->format->Bmask ) >> 1 ) & screen->format->Bmask ) ;
        }
//        bitmap_16bits_conversion();
    }

    scr_initialized = 1 ;

    SDL_ShowCursor( 0 ) ;

    pal_refresh( NULL ) ;
    palette_changed = 1 ;

//    gr_make_trans_table();

    /* Bitmaps de fondo */

    /* Only allow background with same properties that video mode */
    if (
        !background ||
        scr_width != screen->w || scr_height != screen->h ||
        sys_pixel_format->depth != background->format->depth )
    {
        if ( background ) bitmap_destroy( background );
        background = bitmap_new( 0, screen->w, screen->h, sys_pixel_format->depth ) ;
        if ( background )
        {
            gr_clear( background ) ;
            bitmap_add_cpoint( background, 0, 0 ) ;
        }
    }

    scr_width = screen->w ;
    scr_height = screen->h ;

    regions[0].x  = 0 ;
    regions[0].y  = 0 ;
    regions[0].x2 = screen->w - 1 ;
    regions[0].y2 = screen->h - 1 ;

    // Finalmente seteamos icono de aplicacion
    // Necesitamos crear una surface a partir de un MAP generico de 16x16...
    gr_set_icon( icon );

    if ( background ) background->modified = 1;

//    gr_rects_clear();

    return 0;
}

/* --------------------------------------------------------------------------- */

int gr_init( int width, int height )
{
#if defined(TARGET_ANDROID) || defined(TARGET_IOS)
    return gr_set_mode( 0, 0, 0 );
#else
    return gr_set_mode( width, height, 0 );
#endif
}

/* --------------------------------------------------------------------------- */

void __bgdexport( libvideo, module_initialize )()
{
    char * e;

    if ( !SDL_WasInit( SDL_INIT_VIDEO ) ) SDL_InitSubSystem( SDL_INIT_VIDEO );

#ifdef _WIN32
    if ( !directdraw ) init_dx();
#endif
    apptitle = appname;

    if ( ( e = getenv( "VIDEO_WIDTH"  ) ) ) scr_width = atoi(e);
    if ( ( e = getenv( "VIDEO_HEIGHT" ) ) ) scr_height = atoi(e);
    if ( ( e = getenv( "VIDEO_DEPTH"  ) ) )
        GLODWORD( libvideo, GRAPH_MODE ) = atoi(e);
    else
        GLODWORD( libvideo, GRAPH_MODE ) = MODE_16BITS;
    if ( ( e = getenv( "VIDEO_FULLSCREEN" ) ) ) GLODWORD( libvideo, GRAPH_MODE ) |= atoi(e) ? MODE_FULLSCREEN : 0;

    gr_init( scr_width, scr_height ) ;
}

/* --------------------------------------------------------------------------- */

void __bgdexport( libvideo, module_finalize )()
{
#ifdef _WIN32
    if ( directdraw )
    {
        /* set cooperative level back to normal */
        IDirectDraw2_SetCooperativeLevel( directdraw, NULL, DDSCL_NORMAL );

        /* release DirectDraw interface */
        IDirectDraw2_Release( directdraw );

        directdraw = NULL;
    }
#endif

    if ( renderer ) {
        SDL_DestroyRenderer(renderer);
    }

    if ( window ) {
        SDL_DestroyWindow(window);
    }

    if ( SDL_WasInit( SDL_INIT_VIDEO ) ) SDL_QuitSubSystem( SDL_INIT_VIDEO );
}

/* --------------------------------------------------------------------------- */

char * __bgdexport( libvideo, modules_dependency )[] =
{
    "libgrbase",
    NULL
};

/* --------------------------------------------------------------------------- */
