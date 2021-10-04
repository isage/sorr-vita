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

#ifndef __MODMAP_SYMBOLS_H
#define __MODMAP_SYMBOLS_H

#include <bgddl.h>

#ifdef __BGDC__
#define G_WIDE          0
#define G_WIDTH         0
#define G_HEIGHT        1
#define G_CENTER_X      2
#define G_X_CENTER      2
#define G_CENTER_Y      3
#define G_Y_CENTER      3
#define G_PITCH         4
#define G_DEPTH         5
#define B_CLEAR         0x00000001
#define CHARSET_ISO8859 0
#define CHARSET_CP850   1
#define NFB_FIXEDWIDTH  1

DLCONSTANT __bgdexport( mod_map, constants_def )[] =
{
    { "G_WIDE"          , TYPE_INT, G_WIDE              },    /* Obsolete */
    { "G_WIDTH"         , TYPE_INT, G_WIDTH             },
    { "G_HEIGHT"        , TYPE_INT, G_HEIGHT            },
    { "G_CENTER_X"      , TYPE_INT, G_CENTER_X          },
    { "G_X_CENTER"      , TYPE_INT, G_X_CENTER          },
    { "G_CENTER_Y"      , TYPE_INT, G_CENTER_Y          },
    { "G_Y_CENTER"      , TYPE_INT, G_Y_CENTER          },
    { "G_PITCH"         , TYPE_INT, G_PITCH             },
    { "G_DEPTH"         , TYPE_INT, G_DEPTH             },
    { "B_CLEAR"         , TYPE_INT, B_CLEAR             },
    { "CHARSET_ISO8859" , TYPE_INT, CHARSET_ISO8859     },
    { "CHARSET_CP850"   , TYPE_INT, CHARSET_CP850       },
    { "NFB_VARIABLEWIDTH", TYPE_INT, 0                  },
    { "NFB_FIXEDWIDTH"  , TYPE_INT, NFB_FIXEDWIDTH      },

    { NULL              , 0       , 0                   }
} ;

DLSYSFUNCS  __bgdexport( mod_map, functions_exports )[] =
{
    /* Bitmaps */
    { "MAP_BLOCK_COPY"      , "IIIIIIIIII"  , TYPE_INT      , 0 },
    { "MAP_PUT"             , "IIIII"       , TYPE_INT      , 0 },
    { "MAP_XPUT"            , "IIIIIIII"    , TYPE_INT      , 0 },
    { "MAP_NEW"             , "III"         , TYPE_INT      , 0 },
    { "MAP_NEW"             , "IIII"        , TYPE_INT      , 0 },
    { "MAP_CLEAR"           , "III"         , TYPE_INT      , 0 },
    { "MAP_CLONE"           , "II"          , TYPE_INT      , 0 },
    { "MAP_NAME"            , "II"          , TYPE_STRING   , 0 },
    { "MAP_SET_NAME"        , "IIS"         , TYPE_INT      , 0 },
    { "MAP_EXISTS"          , "II"          , TYPE_INT      , 0 },
    { "MAP_XPUTNP"          , "IIIIIIIIII"  , TYPE_INT      , 0 },
    { "MAP_DEL"             , "II"          , TYPE_INT      , 0 },
    { "MAP_UNLOAD"          , "II"          , TYPE_INT      , 0 },
    { "MAP_LOAD"            , "S"           , TYPE_INT      , 0 },
    { "MAP_LOAD"            , "SP"          , TYPE_INT      , 0 },
    { "MAP_SAVE"            , "IIS"         , TYPE_INT      , 0 },
    { "MAP_BUFFER"          , "II"          , TYPE_POINTER  , 0 },
    { "FPG_ADD"             , "IIII"        , TYPE_INT      , 0 },
    { "FPG_NEW"             , ""            , TYPE_INT      , 0 },
    { "FPG_EXISTS"          , "I"           , TYPE_INT      , 0 },
    { "FPG_LOAD"            , "S"           , TYPE_INT      , 0 },
    { "FPG_LOAD"            , "SP"          , TYPE_INT      , 0 },
    { "FPG_SAVE"            , "IS"          , TYPE_INT      , 0 },
    { "FPG_DEL"             , "I"           , TYPE_INT      , 0 },
    { "FPG_UNLOAD"          , "I"           , TYPE_INT      , 0 },
    { "PAL_NEW"             , ""            , TYPE_INT      , 0 },
    { "PAL_DEL"             , "I"           , TYPE_INT      , 0 },
    { "PAL_UNLOAD"          , "I"           , TYPE_INT      , 0 },
    { "PAL_CLONE"           , "I"           , TYPE_INT      , 0 },
    { "PAL_REFRESH"         , ""            , TYPE_INT      , 0 },
    { "PAL_REFRESH"         , "I"           , TYPE_INT      , 0 },
    { "PAL_MAP_GETID"       , "II"          , TYPE_INT      , 0 },
    { "PAL_MAP_ASSIGN"      , "III"         , TYPE_INT      , 0 },
    { "PAL_MAP_REMOVE"      , "II"          , TYPE_INT      , 0 },
    { "PAL_GET"             , "IIP"         , TYPE_INT      , 0 },
    { "PAL_GET"             , "IIIP"        , TYPE_INT      , 0 },
    { "PAL_SYS_SET"         , "I"           , TYPE_INT      , 0 },
    { "PAL_SYS_SET"         , "P"           , TYPE_INT      , 0 },
    { "PAL_SET"             , "IIP"         , TYPE_INT      , 0 },
    { "PAL_SET"             , "IIIP"        , TYPE_INT      , 0 },
    { "PAL_SAVE"            , "S"           , TYPE_INT      , 0 },
    { "PAL_SAVE"            , "SI"          , TYPE_INT      , 0 },
    { "PAL_LOAD"            , "S"           , TYPE_INT      , 0 },
    { "PAL_LOAD"            , "SP"          , TYPE_INT      , 0 },
    { "COLORS_SET"          , "IIP"         , TYPE_INT      , 0 },
    { "COLORS_SET"          , "IIIP"        , TYPE_INT      , 0 },
    { "COLORS_GET"          , "IIP"         , TYPE_INT      , 0 },
    { "COLORS_GET"          , "IIIP"        , TYPE_INT      , 0 },
    { "PALETTE_ROLL"        , "III"         , TYPE_INT      , 0 },
    { "PALETTE_CONVERT"     , "IIP"         , TYPE_INT      , 0 },
    { "COLOR_FIND"          , "BBB"         , TYPE_INT      , 0 },
    { "RGB"                 , "BBBI"        , TYPE_INT      , 0 },
    { "RGBA"                , "BBBBI"       , TYPE_INT      , 0 },
    { "RGB_GET"             , "IPPPI"       , TYPE_INT      , 0 },
    { "RGBA_GET"            , "IPPPPI"      , TYPE_INT      , 0 },
    { "RGB"                 , "BBB"         , TYPE_INT      , 0 },
    { "RGBA"                , "BBBB"        , TYPE_INT      , 0 },
    { "RGB_GET"             , "IPPP"        , TYPE_INT      , 0 },
    { "RGBA_GET"            , "IPPPP"       , TYPE_INT      , 0 },
    { "FADE"                , "IIII"        , TYPE_INT      , 0 },
    { "FADE_ON"             , ""            , TYPE_INT      , 0 },
    { "FADE_OFF"            , ""            , TYPE_INT      , 0 },
    { "MAP_INFO_SET"        , "IIII"        , TYPE_INT      , 0 },
    { "MAP_INFO_GET"        , "III"         , TYPE_INT      , 0 },
    { "MAP_INFO"            , "III"         , TYPE_INT      , 0 },
    { "GRAPHIC_SET"         , "IIII"        , TYPE_INT      , 0 },
    { "GRAPHIC_INFO"        , "III"         , TYPE_INT      , 0 },
    { "POINT_GET"           , "IIIPP"       , TYPE_INT      , 0 },
    { "POINT_SET"           , "IIIII"       , TYPE_INT      , 0 },
    { "CENTER_SET"          , "IIII"        , TYPE_INT      , 0 },
    { "FNT_LOAD"            , "S"           , TYPE_INT      , 0 },
    { "FNT_LOAD"            , "SP"          , TYPE_INT      , 0 },
    { "FNT_UNLOAD"          , "I"           , TYPE_INT      , 0 },
    { "FNT_SAVE"            , "IS"          , TYPE_INT      , 0 },
    { "FNT_NEW"             , "I"           , TYPE_INT      , 0 },
    { "FNT_NEW"             , "II"          , TYPE_INT      , 0 },
    { "FNT_NEW"             , "IIIIIIII"    , TYPE_INT      , 0 },
    { "BDF_LOAD"            , "S"           , TYPE_INT      , 0 },
    { "BDF_LOAD"            , "SP"          , TYPE_INT      , 0 },
    { "GLYPH_GET"           , "II"          , TYPE_INT      , 0 },
    { "GLYPH_SET"           , "IIII"        , TYPE_INT      , 0 },
    { "PNG_LOAD"            , "S"           , TYPE_INT      , 0 },
    { "PNG_LOAD"            , "SP"          , TYPE_INT      , 0 },
    { "PCX_LOAD"            , "S"           , TYPE_INT      , 0 },
    { "PCX_LOAD"            , "SP"          , TYPE_INT      , 0 },
    { "PNG_SAVE"            , "IIS"         , TYPE_INT      , 0 },
    { "NEW_MAP"             , "III"         , TYPE_INT      , 0 },
    { "LOAD_MAP"            , "S"           , TYPE_INT      , 0 },
    { "LOAD_MAP"            , "SP"          , TYPE_INT      , 0 },
    { "UNLOAD_MAP"          , "II"          , TYPE_INT      , 0 },
    { "SAVE_MAP"            , "IIS"         , TYPE_INT      , 0 },
    { "NEW_PAL"             , ""            , TYPE_INT      , 0 },
    { "LOAD_PAL"            , "S"           , TYPE_INT      , 0 },
    { "LOAD_PAL"            , "SP"          , TYPE_INT      , 0 },
    { "UNLOAD_PAL"          , "I"           , TYPE_INT      , 0 },
    { "SAVE_PAL"            , "S"           , TYPE_INT      , 0 },
    { "SAVE_PAL"            , "SI"          , TYPE_INT      , 0 },
    { "SET_COLORS"          , "IIP"         , TYPE_INT      , 0 },
    { "SET_COLORS"          , "IIIP"        , TYPE_INT      , 0 },
    { "GET_COLORS"          , "IIP"         , TYPE_INT      , 0 },
    { "GET_COLORS"          , "IIIP"        , TYPE_INT      , 0 },
    { "ROLL_PALETTE"        , "III"         , TYPE_INT      , 0 },
    { "CONVERT_PALETTE"     , "IIP"         , TYPE_INT      , 0 },
    { "FIND_COLOR"          , "BBB"         , TYPE_INT      , 0 },
    { "GET_RGB"             , "IPPPI"       , TYPE_INT      , 0 },
    { "GET_RGBA"            , "IPPPPI"      , TYPE_INT      , 0 },
    { "GET_RGB"             , "IPPP"        , TYPE_INT      , 0 },
    { "GET_RGBA"            , "IPPPP"       , TYPE_INT      , 0 },
    { "NEW_FPG"             , ""            , TYPE_INT      , 0 },
    { "LOAD_FPG"            , "S"           , TYPE_INT      , 0 },
    { "LOAD_FPG"            , "SP"          , TYPE_INT      , 0 },
    { "SAVE_FPG"            , "IS"          , TYPE_INT      , 0 },
    { "UNLOAD_FPG"          , "I"           , TYPE_INT      , 0 },
    { "GET_POINT"           , "IIIPP"       , TYPE_INT      , 0 },
    { "SET_POINT"           , "IIIII"       , TYPE_INT      , 0 },
    { "SET_CENTER"          , "IIII"        , TYPE_INT      , 0 },
    { "NEW_FNT"             , "I"           , TYPE_INT      , 0 },
    { "NEW_FNT"             , "II"          , TYPE_INT      , 0 },
    { "NEW_FNT"             , "IIIIIIII"    , TYPE_INT      , 0 },
    { "LOAD_FNT"            , "S"           , TYPE_INT      , 0 },
    { "LOAD_FNT"            , "SP"          , TYPE_INT      , 0 },
    { "UNLOAD_FNT"          , "I"           , TYPE_INT      , 0 },
    { "SAVE_FNT"            , "IS"          , TYPE_INT      , 0 },
    { "LOAD_BDF"            , "S"           , TYPE_INT      , 0 },
    { "LOAD_BDF"            , "SP"          , TYPE_INT      , 0 },
    { "GET_GLYPH"           , "II"          , TYPE_INT      , 0 },
    { "SET_GLYPH"           , "IIII"        , TYPE_INT      , 0 },
    { "LOAD_PNG"            , "S"           , TYPE_INT      , 0 },
    { "LOAD_PNG"            , "SP"          , TYPE_INT      , 0 },
    { "LOAD_PCX"            , "S"           , TYPE_INT      , 0 },
    { "LOAD_PCX"            , "SP"          , TYPE_INT      , 0 },
    { "SAVE_PNG"            , "IIS"         , TYPE_INT      , 0 },
    { 0                     , 0             , 0             , 0 }
};

/* --------------------------------------------------------------------------- */

char * __bgdexport( mod_map, modules_dependency )[] =
{
    "libgrbase",
    "libvideo",
    "libblit",
    "libfont",
    NULL
};
#else
extern DLCONSTANT __bgdexport( mod_map, constants_def )[];
extern DLSYSFUNCS  __bgdexport( mod_map, functions_exports )[];
extern char * __bgdexport( mod_map, modules_dependency )[];
#endif

#endif
