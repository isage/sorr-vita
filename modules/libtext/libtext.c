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

#include "xstrings.h"

#include "libblit.h"
#include "librender.h"

#include "libtext.h"

/* --------------------------------------------------------------------------- */

int fntcolor8 = -1 ;
int fntcolor16 = 0xFFFF;
int fntcolor32 = 0xFFFFFFFF;

/* --------------------------------------------------------------------------- */

typedef struct _text
{
    int id ;
    int on ;          /* 1 - Texto ; 2 - VarSTR; 3 - VarINT; 4 - VarFLOAT; 5 - VarWORD; 6 - VarBYTE */
    int fontid ;
    int x ;
    int y ;
    int z ;
    int alignment ;
    int color8 ;
    int color16 ;
    int color32 ;
    int objectid ;
    int last_value ;
    char * text ;           /* Memoria dinámica */
    const void * var  ;  /* CHANGED TO VOID to allow diff. data types */
    int last_z ;
    int last_color8 ;
    int last_color16 ;
    int last_color32 ;
    /* Internals, for speed up */
    int _x ;
    int _y ;
    int _width;
    int _height;
} TEXT;

TEXT texts[MAX_TEXTS] ;

int text_nextid = 1 ;
int text_count  = 0 ;

/* --------------------------------------------------------------------------- */

int gr_text_height_no_margin( int fontid, const unsigned char * text );
int gr_text_widthn( int fontid, const unsigned char * text, int n );

/* --------------------------------------------------------------------------- */

enum {
    TEXTZ = 0,
    TEXT_FLAGS
};

/* --------------------------------------------------------------------------- */
/* Definicion de variables globales (usada en tiempo de compilacion) */

char * __bgdexport( libtext, globals_def ) =
    "text_z = -256;\n"
    "text_flags;\n"
    ;

/* --------------------------------------------------------------------------- */
/* Son las variables que se desea acceder.                           */
/* El interprete completa esta estructura, si la variable existe.    */
/* (usada en tiempo de ejecucion)                                    */

DLVARFIXUP __bgdexport( libtext, globals_fixup )[] =
{
    /* Nombre de variable global, puntero al dato, tamaño del elemento, cantidad de elementos */
    { "text_z"      , NULL, -1, -1 },
    { "text_flags"  , NULL, -1, -1 },
    { NULL          , NULL, -1, -1 }
};

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : get_text
 *
 *  Returns the character string of a given text
 *  (may be the representation of a integer or float value)
 *
 *  PARAMS :
 *  text   Pointer to the text object
 *
 *  RETURN VALUE :
 *      String contained within the text
 *
 */

static const char * get_text( TEXT * text )
{
    static char buffer[64];

    switch ( text->on )
    {
        case TEXT_TEXT:
            return text->text;

        case TEXT_STRING:
            return string_get( *( int* )text->var ) ;

        case TEXT_INT:
            _string_ntoa( buffer, *( int * )text->var ) ;
            return buffer ;

        case TEXT_DWORD:
            _string_utoa( buffer, *( int * )text->var ) ;
            return buffer ;

        case TEXT_FLOAT:
            {
                char * aux;
                sprintf( buffer, "%f", *( float * )text->var ) ;
                aux = buffer + 1; // We know that buffer contain at least 2 chars, skip first
                while ( *( aux + 1 ) ) aux++; // We can test for pointer + 1 because we know that buffer contain at least 2 chars
                while ( *aux == '0' && *( aux - 1 ) != '.' ) *aux-- = '\0';
                return buffer ;
            }

        case TEXT_BYTE:
            _string_utoa( buffer, *( uint8_t * )text->var ) ;
            return buffer ;

        case TEXT_SBYTE:
            _string_ntoa( buffer, *( int8_t * )text->var ) ;
            return buffer ;

        case TEXT_CHAR:
            *buffer = *( uint8_t * )text->var ;
            *( buffer + 1 ) = '\0';
            return buffer ;

        case TEXT_WORD:
            _string_utoa( buffer, *( uint16_t * )text->var ) ;
            return buffer ;

        case TEXT_SHORT:
            _string_ntoa( buffer, *( int16_t * )text->var ) ;
            return buffer ;

        case TEXT_CHARARRAY:
            return ( const char * )( text->var );

        case TEXT_POINTER:
            _string_ptoa( buffer, *( void ** ) text->var );
            return buffer ;
    }

    return NULL;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : info_text
 *
 *  Returns information about a text object
 *
 *  PARAMS :
 *  text   Pointer to the text object
 *  bbox   Region to update with the text bounding box
 *
 *  RETURN VALUE :
 *      1 if the text has changed since last frame
 *
 */

static int info_text( TEXT * text, REGION * bbox, int * z, int * drawme )
{
    const char * str = get_text( text );
    REGION prev = *bbox;
    FONT * font;
    int changed = 0;

    * drawme = 0;

    // Splinter
    if ( !str || !*str )
    {
        /*        bbox->x = -2;
                bbox->y = -2;
                bbox->x2 = -2;
                bbox->y2 = -2; */
        return 0;
    }

    font = gr_font_get( text->fontid );
    if ( !font )
    {
        /*        bbox->x = -2;
                bbox->y = -2;
                bbox->x2 = -2;
                bbox->y2 = -2; */
        return 0;
    }

    * drawme = 1;

    * z = text->z;

    /* Calculate the text dimensions */

    text->_x = text->x;
    text->_y = text->y;
    text->_width = gr_text_width( text->fontid, ( const unsigned char * ) str );
    text->_height = gr_text_height_no_margin( text->fontid, ( const unsigned char * ) str );

    /* Update the font's maxheight (if needed) */

    if ( !font->maxheight )
    {
        int c;

        for ( c = 0 ; c < 256 ; c++ )
        {
            if ( !font->glyph[c].bitmap ) continue;
            if ( font->maxheight < ( int )font->glyph[c].bitmap->height + font->glyph[c].yoffset )
                font->maxheight = ( int )font->glyph[c].bitmap->height + font->glyph[c].yoffset;
        }
    }

    /* Adjust top-left coordinates for text alignment */

    switch ( text->alignment )
    {
        case ALIGN_TOP:             // 1
        case ALIGN_CENTER:          // 4
        case ALIGN_BOTTOM:          // 7
            text->_x -= text->_width / 2;
            break;

        case ALIGN_TOP_RIGHT:       // 2
        case ALIGN_CENTER_RIGHT:    // 5
        case ALIGN_BOTTOM_RIGHT:    // 8
            text->_x -= text->_width - 1;
            break;
    }

    switch ( text->alignment )
    {
        case ALIGN_CENTER_LEFT:     // 3
        case ALIGN_CENTER:          // 4
        case ALIGN_CENTER_RIGHT:    // 5
            text->_y -= font->maxheight / 2;
            break;

        case ALIGN_BOTTOM_LEFT:     // 6
        case ALIGN_BOTTOM:          // 7
        case ALIGN_BOTTOM_RIGHT:    // 8
            text->_y -= font->maxheight - 1;
            break;
    }

    /* Fill the bounding box */

    bbox->x = text->_x;
    bbox->y = text->_y;
    bbox->x2 = text->_x + text->_width - 1;
    bbox->y2 = text->_y + text->_height - 1;

    /* Check if the var has changed since last call */

    changed =
        text->color8  != text->last_color8  ||
        text->color16 != text->last_color16 ||
        text->color32 != text->last_color32 ||
        text->z != text->last_z ||
        bbox->x  != prev.x  || bbox->y  != prev.y ||
        bbox->x2 != prev.x2 || bbox->y2 != prev.y2;

    text->last_z = text->z;
    text->last_color8 = text->color8;
    text->last_color16 = text->color16;
    text->last_color32 = text->color32;

    switch ( text->on )
    {
        case TEXT_TEXT:
            return changed;

        case TEXT_STRING:
        case TEXT_FLOAT:
        case TEXT_INT:
        case TEXT_DWORD:
        case TEXT_POINTER:
            if ( text->last_value == *( int * )text->var ) return changed;
            text->last_value = *( int * )text->var;
            return 1;

        case TEXT_BYTE:
        case TEXT_SBYTE:
        case TEXT_CHAR:
            if ( text->last_value == *( uint8_t * )text->var ) return changed;
            text->last_value = *( uint8_t * )text->var;
            return 1;

        case TEXT_WORD:
        case TEXT_SHORT:
            if ( text->last_value == *( uint16_t * )text->var ) return changed;
            text->last_value = *( uint16_t * )text->var;
            return 1;

        case TEXT_CHARARRAY:
            return 1;
    }

    return changed;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : draw_text
 *
 *  Draws a text object
 *
 *  PARAMS :
 *  text   Pointer to the text object
 *  clip   Clipping region
 *
 *  RETURN VALUE :
 *      None
 *
 */

void draw_text( TEXT * text, REGION * clip )
{
    const char * str = get_text( text );
    int save8, save16, save32;
    FONT * font;

    // Splinter
    if ( !str ) return;

    font = gr_font_get( text->fontid );
    if ( !font )
    {
        gr_text_destroy( text->id );
        return;
    }

    /* Draw the text */

    save8 = fntcolor8;
    save16 = fntcolor16;
    save32 = fntcolor32;

    fntcolor8 = text->color8;
    fntcolor16 = text->color16;
    fntcolor32 = text->color32;

    if ( !gr_text_put( 0, clip, text->fontid, text->_x, text->_y, ( const unsigned char * ) str ) ) gr_text_destroy( text->id );

    fntcolor8 = save8;
    fntcolor16 = save16;
    fntcolor32 = save32;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : gr_text_new2
 *
 *  Create a new text, using a fixed text string
 *
 *  PARAMS :
 *  fontid      Font number
 *  x, y, z     Screen coordinates
 *  alignment   Alignment
 *  text        Pointer to text
 *
 *  RETURN VALUE :
 *      None
 *
 */

int gr_text_new2( int fontid, int x, int y, int z, int alignment, const char * text )
{
    int textid = text_nextid ;

    if ( text_nextid == MAX_TEXTS )
    {
        for ( textid = 1 ; textid < MAX_TEXTS ; textid++ ) if ( !texts[textid].on ) break ;

        if ( textid == MAX_TEXTS ) return 0; // error ("Demasiados textos en pantalla") ;
    }
    else
        text_nextid++ ;

    text_count++ ;

    texts[textid].id = textid ;
    texts[textid].on = TEXT_TEXT ;
    texts[textid].fontid = fontid ;
    texts[textid].x = x ;
    texts[textid].y = y ;
    texts[textid].z = z ;
    texts[textid].alignment = alignment ;
    texts[textid].text = text ? strdup( text ) : 0 ;
    texts[textid].color8 = fntcolor8 ;
    texts[textid].color16 = fntcolor16 ;
    texts[textid].color32 = fntcolor32 ;
    texts[textid].objectid = gr_new_object( texts[textid].z, info_text, draw_text, &texts[textid] );
    texts[textid].last_value = 0 ;
    texts[textid].last_z = 0 ;

    return textid ;
}

/* --------------------------------------------------------------------------- */
/*
 *  FUNCTION : gr_text_new
 *
 *  Create a new text, using a fixed text string
 *
 *  PARAMS :
 *  fontid     Font number
 *  x, y       Screen coordinates
 *  alignment  Alignment
 *  text       Pointer to text
 *
 *  RETURN VALUE :
 *      None
 *
 */

int gr_text_new( int fontid, int x, int y, int alignment, const char * text )
{
    return gr_text_new2( fontid, x, y, GLOINT32( libtext, TEXTZ ), alignment, text );
}

/* --------------------------------------------------------------------------- */

int gr_text_new_var( int fontid, int x, int y, int alignment, const void * var, int type )
{
    int textid = gr_text_new( fontid, x, y, alignment, 0 ) ;
    if ( !textid ) return 0 ;
    texts[textid].on = type ;
    if ( type > TEXT_TEXT ) texts[textid].var = var ;
    return textid ;
}

/* --------------------------------------------------------------------------- */

int gr_text_new_var2( int fontid, int x, int y, int z, int alignment, const void * var, int type )
{
    int textid = gr_text_new2( fontid, x, y, z, alignment, 0 ) ;
    if ( !textid ) return 0 ;
    texts[textid].on = type ;
    if ( type > TEXT_TEXT ) texts[textid].var = var ;
    return textid ;
}

/* --------------------------------------------------------------------------- */

void gr_text_move( int textid, int x, int y )
{
    if ( textid > 0 && textid < text_nextid )
    {
        texts[textid].x = x ;
        texts[textid].y = y ;
    }
}

/* --------------------------------------------------------------------------- */

void gr_text_move2( int textid, int x, int y, int z )
{
    if ( textid > 0 && textid < text_nextid )
    {
        texts[textid].x = x ;
        texts[textid].y = y ;
        texts[textid].z = z ;
    }
}

/* --------------------------------------------------------------------------- */

void gr_text_destroy( int textid )
{
    if ( !textid )
    {
        for ( textid = 1 ; textid < text_nextid ; textid++ )
        {
            if ( texts[textid].on )
            {
                gr_destroy_object( texts[textid].objectid );
                if ( texts[textid].text ) free( texts[textid].text ) ;
                texts[textid].on = 0 ;
            }
        }
        text_count = 0 ;
        text_nextid = 1 ;
        return ;
    }
    if ( textid > 0 && textid < text_nextid )
    {
        if ( !texts[textid].on ) return ;

        gr_destroy_object( texts[textid].objectid );
        if ( texts[textid].text ) free( texts[textid].text ) ;
        texts[textid].on = 0 ;
        if ( textid == text_nextid - 1 )
        {
            while ( text_nextid > 1 && !texts[text_nextid-1].on ) text_nextid-- ;
        }
        text_count-- ;
    }
}

/* --------------------------------------------------------------------------- */

int gr_text_width( int fontid, const unsigned char * text )
{
    return gr_text_widthn( fontid, text, strlen( ( char * ) text ) );
}

/* --------------------------------------------------------------------------- */

int gr_text_widthn( int fontid, const unsigned char * text, int n )
{
    int l = 0 ;
    FONT * f ;

    if ( !text || !*text ) return 0;
    if ( fontid < 0 || fontid >= MAX_FONTS || !fonts[fontid] ) return 0; // Incorrect font type

    f = fonts[fontid] ;

    while ( *text && n-- )
    {
        switch ( f->charset )
        {
            case CHARSET_ISO8859:
                l += f->glyph[dos_to_win[*text]].xadvance ;
                break;

            case CHARSET_CP850:
                l += f->glyph[*text].xadvance ;
                break;
        }
        text++;
    }
    return l ;
}

/* --------------------------------------------------------------------------- */

int gr_text_margintop( int fontid, const unsigned char * text )
{
    int minyoffset = 0x7FFFFFFF ;
    FONT * f ;

    if ( !text || !*text ) return 0;
    if ( fontid < 0 || fontid >= MAX_FONTS || !fonts[fontid] ) return 0; // Incorrect font type

    f = fonts[fontid] ;

    while ( *text )
    {
        switch ( f->charset )
        {
            case CHARSET_ISO8859:
                if ( minyoffset > f->glyph[dos_to_win[*text]].yoffset )
                    minyoffset = f->glyph[dos_to_win[*text]].yoffset;
                break;

            case CHARSET_CP850:
                if ( minyoffset > f->glyph[*text].yoffset )
                    minyoffset = f->glyph[*text].yoffset;
                break;
        }
        text++ ;
    }
    return minyoffset ;
}

/* --------------------------------------------------------------------------- */

int gr_text_height_no_margin( int fontid, const unsigned char * text )
{
    int l = 0;
    FONT * f ;

    if ( !text || !*text ) return 0;
    if ( fontid < 0 || fontid >= MAX_FONTS || !fonts[fontid] ) return 0; // Incorrect font type

    f = fonts[fontid] ;

    while ( *text )
    {
        if ( f->glyph[*text].bitmap )
        {
            switch ( f->charset )
            {
                case CHARSET_ISO8859:
                    if ( l < f->glyph[dos_to_win[*text]].yoffset + ( int )f->glyph[dos_to_win[*text]].bitmap->height )
                    {
                        l = f->glyph[dos_to_win[*text]].yoffset + ( int )f->glyph[dos_to_win[*text]].bitmap->height ;
                    }
                    break;

                case CHARSET_CP850:
                    if ( l < f->glyph[*text].yoffset + ( int )f->glyph[*text].bitmap->height )
                    {
                        l = f->glyph[*text].yoffset + ( int )f->glyph[*text].bitmap->height ;
                    }
                    break;
            }
        }
        text++ ;
    }
    return l ;
}

/* --------------------------------------------------------------------------- */

int gr_text_height( int fontid, const unsigned char * text )
{
    int l = gr_text_height_no_margin( fontid, text );
    if ( l ) l -= gr_text_margintop( fontid, text );
    return l ;
}

/* --------------------------------------------------------------------------- */

int gr_text_put( GRAPH * dest, REGION * clip, int fontid, int x, int y, const unsigned char * text )
{
    GRAPH * ch ;
    FONT   * f ;
    uint8_t  current_char;
    int flags ;
    int save8, save16, save32;

    if ( !text || !*text ) return -1;
    if ( fontid < 0 || fontid >= MAX_FONTS || !fonts[fontid] ) return 0; // Incorrect font type

    if ( !dest ) dest = scrbitmap ;

    f = fonts[fontid] ;

    flags = GLODWORD( libtext, TEXT_FLAGS );

    save8 = pixel_color8;
    save16 = pixel_color16;
    save32 = pixel_color32;

    if ( fntcolor8 == -1 )
    {
        gr_setcolor(( dest->format->depth == 8 ) ? gr_find_nearest_color( 255, 255, 255 ) : gr_rgb_depth( dest->format->depth, 255, 255, 255 ) );
    }
    else
    {
        pixel_color8 = fntcolor8;
        pixel_color16 = fntcolor16;
        pixel_color32 = fntcolor32;
    }

    while ( *text )
    {
        switch ( f->charset )
        {
            case CHARSET_ISO8859:
                current_char = dos_to_win[*text];
                break;

            case CHARSET_CP850:
                current_char = *text;
                break;

            default:
                current_char = 0;
                break;
        }

        ch = f->glyph[current_char].bitmap ;
        if ( ch )
        {
            gr_blit( dest, clip, x + f->glyph[current_char].xoffset, y + f->glyph[current_char].yoffset, flags, ch ) ;
        }
        x += f->glyph[current_char].xadvance ;
        text++ ;
    }

    pixel_color8 = save8;
    pixel_color16 = save16;
    pixel_color32 = save32;

    return 1;
}

/* --------------------------------------------------------------------------- */

GRAPH * gr_text_bitmap( int fontid, const char * text, int alignment )
{
    GRAPH * gr ;
    int x, y ;
    FONT   * f ;

    // Splinter
    if ( !text || !*text ) return NULL;
    if ( fontid < 0 || fontid >= MAX_FONTS || !fonts[fontid] ) return NULL; // Incorrect font type

    f = fonts[fontid] ;

    /* Un refresco de paleta en mitad de gr_text_put puede provocar efectos
     * desagradables al modificar el tipo de letra del sistema */

    if ( palette_changed ) gr_refresh_palette() ;

    gr = bitmap_new_syslib( gr_text_width( fontid, ( const unsigned char * ) text ), gr_text_height( fontid, ( const unsigned char * ) text ), sys_pixel_format->depth ) ;
    if ( !gr ) return NULL;

    gr_clear( gr ) ;
    if ( !gr_text_put( gr, 0, fontid, 0, -gr_text_margintop( fontid, ( const unsigned char * ) text ), ( const unsigned char * ) text ) )
    {
        bitmap_destroy( gr );
        return NULL;
    }

    switch ( alignment )
    {
        case ALIGN_TOP_LEFT:    // 0
        case ALIGN_TOP:         // 1
        case ALIGN_TOP_RIGHT:   // 2
            y = 0 ;
            break ;

        case ALIGN_CENTER_LEFT: // 3
        case ALIGN_CENTER:      // 4
        case ALIGN_CENTER_RIGHT:// 5
            y = gr->height / 2 ;
            break ;

        default:
            y = gr->height - 1 ;
            break ;

    }

    switch ( alignment )
    {
        case ALIGN_TOP_LEFT:    // 0
        case ALIGN_CENTER_LEFT: // 3
        case ALIGN_BOTTOM_LEFT: // 6
            x = 0 ;
            break ;

        case ALIGN_TOP:         // 1
        case ALIGN_CENTER:      // 4
        case ALIGN_BOTTOM:      // 7
            x = gr->width / 2 ;
            break ;

        default:
            x = gr->width - 1 ;
            break ;

    }

    bitmap_add_cpoint( gr, x, y ) ;
    return gr ;
}

/* --------------------------------------------------------------------------- */

void gr_text_setcolor( int c )
{
    int r, g, b;

    if ( !c )
    {
        fntcolor8 = 0;
        fntcolor16 = 0;
        fntcolor32 = 0;
    }
    else
    {
        switch ( sys_pixel_format->depth )
        {
            case    8:
            {
                fntcolor8 = c ;
                break;
            }

            case    16:
            {
                gr_get_rgb( c, &r, &g, &b );
                fntcolor8 = gr_find_nearest_color( r, g, b );
                fntcolor16 = c ;
                break;
            }
            case    32:
            {
                gr_get_rgb( c, &r, &g, &b );
                fntcolor8 = gr_find_nearest_color( r, g, b );
                fntcolor32 = c ;
                break;
            }
        }
    }
}

/* --------------------------------------------------------------------------- */

void gr_text_setcolor2( int textid, int c )
{
    int r, g, b;

    if ( textid > 0 && textid < text_nextid )
    {
        if ( !c )
        {
            texts[textid].color8 = 0;
            texts[textid].color16 = 0;
            texts[textid].color32 = 0;
        }
        else
        {
            switch ( sys_pixel_format->depth )
            {
                case    8:
                {
                    texts[textid].color8 = c ;
                    break;
                }

                case    16:
                {
                    gr_get_rgb( c, &r, &g, &b );
                    texts[textid].color8 = gr_find_nearest_color( r, g, b );
                    texts[textid].color16 = c ;
                    break;
                }
                case    32:
                {
                    gr_get_rgb( c, &r, &g, &b );
                    texts[textid].color8 = gr_find_nearest_color( r, g, b );
                    texts[textid].color32 = c ;
                    break;
                }
            }
        }
    }
}

/* --------------------------------------------------------------------------- */

int gr_text_getcolor()
{
    switch ( sys_pixel_format->depth )
    {
        case    8:
        {
            return fntcolor8 ;
        }

        case    16:
        {
            return fntcolor16 ;
        }
        case    32:
        {
            return fntcolor32 ;
        }
    }

    return 0;
}

/* --------------------------------------------------------------------------- */

int gr_text_getcolor2( int textid )
{
    if ( textid > 0 && textid < text_nextid )
    {
        switch ( sys_pixel_format->depth )
        {
            case    8:
            {
                return texts[textid].color8 ;
            }

            case    16:
            {
                return texts[textid].color16 ;
            }
            case    32:
            {
                return texts[textid].color32 ;
            }
        }
    }

    return 0;
}

/* --------------------------------------------------------------------------- */

char * __bgdexport( libtext, modules_dependency )[] =
{
    "libgrbase",
    "libblit",
    "librender",
    "libfont",
    NULL
};

/* --------------------------------------------------------------------------- */
