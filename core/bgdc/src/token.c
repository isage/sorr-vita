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

/* Pending: no newline at end of file */

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "bgdc.h"


/* ---------------------------------------------------------------------- */
/* Tokenizador. La función token_next, ampliamente utilizada, recoge el   */
/* siguiente token (identificador, operador, etc) del código fuente, y    */
/* rellena la estructura global "token" con los datos del mismo.          */
/* ---------------------------------------------------------------------- */

int line_count = 0 ; /* Se pone a 0, ya que lo incremente con cada \n, y hasta no obtener un \n no se procesa la linea (Splinter) */
int current_file = 0 ;

static int prepro_sp = 0 ;
static int prepro_stack[1024] ;

static int disable_prepro = 0 ;
static int disable_expand_defines = 0 ;
static int identifiers_as_strings = 0 ;

struct _token token ;
struct _token token_prev ;
struct _token token_saved ;
static int use_saved = 0 ;

typedef struct _define
{
    int     code;
    char *  text;
    int     param_count;
    int     param_id[MAX_MACRO_PARAMS];
}
DEFINE;

static const char * source_ptr;
static char       * source_start;
static const char * old_sources[MAX_SOURCES];
static char       * old_sources_start[MAX_SOURCES];
static int          old_line_counts[MAX_SOURCES];
static int          old_current_file[MAX_SOURCES];
static int          sources = 0;
static DEFINE  *    defines = NULL;
static int          defines_allocated = 0;
static int          defines_count = 0;
static int          id_define;
static int          id_undef;
static int          id_ifdef;
static int          id_ifndef;
static int          id_endif;
static int          id_else;
static int          id_if;

/* --------------------------------------------------------------------------- */

static int token_endfile();

/* ---------------------------------------------------------------------- */

#define SKIP_C89_COMMENTS \
    if (*source_ptr && *source_ptr == '/' && *(source_ptr + 1) == '*') { \
        * ((char *) source_ptr++) = ' '; * ((char *) source_ptr++) = ' '; \
        while (*source_ptr && (*source_ptr != '*' || *(source_ptr + 1) != '/')) { \
            if (*source_ptr == '\n')  { line_count++; * ((char *) source_ptr++) = ' '; continue ;} \
            * ((char *) source_ptr++) = ' '; \
        } \
        if (*source_ptr == '*' && *(source_ptr + 1) == '/') { \
            * ((char *) source_ptr++) = ' '; * ((char *) source_ptr++) = ' '; \
        } \
        continue;\
    }


#define SKIP_C99_COMMENTS \
    if (*source_ptr && *source_ptr == '/' && *(source_ptr + 1) == '/') { \
        * ((char *) source_ptr++) = ' '; * ((char *) source_ptr++) = ' '; \
        while (*source_ptr && *source_ptr != '\n') { \
            if (*source_ptr == '\\' && *(source_ptr + 1) == '\n') { line_count++; * ((char *) source_ptr++) = ' ';} \
            * ((char *) source_ptr++) = ' '; \
        } \
        continue; \
    }


#define SKIP_COMMENTS \
    SKIP_C89_COMMENTS \
    SKIP_C99_COMMENTS

#define SKIP_COMMENTS2 \
    while(1) { \
        SKIP_C89_COMMENTS \
        SKIP_C99_COMMENTS \
        break; \
    }


#define SKIP_SPACES \
    SKIP_COMMENTS2;\
    while (ISSPACE(*source_ptr) || (*source_ptr == '\\' && *(source_ptr + 1) == '\n')) { \
        if (*source_ptr == '\\' && *(source_ptr + 1) == '\n') { source_ptr += 2; line_count++; continue; } \
        if (*source_ptr == '\n') { line_count++; source_ptr++; continue; } \
        SKIP_COMMENTS;\
        source_ptr++; \
    }


#define SKIP_SPACES_UNTIL_LF \
    SKIP_COMMENTS2;\
    while (ISSPACE(*source_ptr) || (*source_ptr == '\\' && *(source_ptr + 1) == '\n')) { \
        if (*source_ptr == '\\' && *(source_ptr + 1) == '\n') { source_ptr += 2; continue; } \
        if (*source_ptr == '\n') break; \
        SKIP_COMMENTS;\
        source_ptr++; \
    }


#define SKIP_SPACES_UNTIL_LF_AND_COUNT_LINES \
    SKIP_COMMENTS2;\
    while (ISSPACE(*source_ptr) || (*source_ptr == '\\' && *(source_ptr + 1) == '\n')) { \
        if (*source_ptr == '\\' && *(source_ptr + 1) == '\n') { source_ptr += 2; line_count++; continue; } \
        if (*source_ptr == '\n') { line_count++; break; } \
        SKIP_COMMENTS;\
        source_ptr++; \
    }


#define SKIP_ALL_UNTIL_LF \
    SKIP_COMMENTS2;\
    while (*source_ptr || (*source_ptr == '\\' && *(source_ptr + 1) == '\n')) { \
        if (*source_ptr == '\\' && *(source_ptr + 1) == '\n') { source_ptr += 2; continue; } \
        if (*source_ptr == '\n') break; \
        SKIP_COMMENTS;\
        source_ptr++; \
    }


#define SKIP_ALL_UNTIL_LF_AND_COUNT_LINES \
    SKIP_COMMENTS2;\
    while (*source_ptr || (*source_ptr == '\\' && *(source_ptr + 1) == '\n')) { \
        if (*source_ptr == '\\' && *(source_ptr + 1) == '\n') { source_ptr += 2; line_count++; continue; } \
        if (*source_ptr == '\n') { line_count++; break; } \
        SKIP_COMMENTS;\
        source_ptr++; \
    }


#define GET_NEXT_TOKEN_IN_TMPBUFFER \
    buffer_ptr = buffer; \
    while (ISWORDCHAR(*source_ptr)) \
    { \
        if (buffer_ptr == buffer+1023) compile_error (MSG_IDENTIFIER_TOO_LONG); \
        *buffer_ptr++ = TOUPPER(*source_ptr++); \
    } \
    *buffer_ptr++ = 0; \
    token.code = identifier_search_or_add(buffer); \
    token.type = IDENTIFIER;

/* ---------------------------------------------------------------------- */

int n_files = 0;                        /* Includes */
char files[MAX_SOURCES][__MAX_PATH];    /* Includes */
char *source_data[MAX_SOURCES];             /* Includes */

/* ---------------------------------------------------------------------- */

int load_file( char * filename )
{
    long   size;
    file * fp = file_open( filename, "rb0" );
    int n;

    for( n = 0; n < n_files; n++ ) if ( !strcmp( files[n], filename ) ) break;

    if ( n >= n_files )
    {
        if ( n_files == MAX_SOURCES ) compile_error( MSG_TOO_MANY_FILES );
        strcpy( files[n_files], filename );
        if ( !fp ) compile_error( MSG_FILE_NOT_FOUND, filename );
        size = file_size( fp );
        source_data[n_files] = ( char * ) calloc( size + 1, sizeof( char ) );
        if ( !source_data[n_files] ) compile_error( MSG_FILE_TOO_BIG, filename );
        if ( size == 0 ) compile_error( MSG_FILE_EMPTY, filename );
        if ( !file_read( fp, source_data[n_files], size ) ) compile_error( MSG_READ_ERROR, filename );

        source_data[n_files][size] = 0;
        file_close( fp );
        n = n_files++;
    }

    token_init( source_data[n], n );
    return n;
}

/* ---------------------------------------------------------------------- */

void include_file( int bprepro )
{
    static char buffer[1024];
    char * buffer_ptr = buffer;
    int actual_line = line_count;

    SKIP_SPACES_UNTIL_LF_AND_COUNT_LINES;
    if ( *source_ptr == '"' )
    {
        source_ptr++;
        buffer_ptr = buffer;
        while ( *source_ptr && *source_ptr != '"' )
        {
            if ( buffer_ptr == buffer + 1023 ) compile_error( MSG_IDENTIFIER_TOO_LONG );
            *buffer_ptr++ = *source_ptr++;
        }
        if ( *source_ptr == '"' ) source_ptr++;
        *buffer_ptr = 0;
        if ( bprepro )
        {
            SKIP_SPACES_UNTIL_LF_AND_COUNT_LINES;
            if ( *source_ptr == '\n' ) line_count--;
        }
        else
        {
            SKIP_SPACES;
        }
        if ( *source_ptr == ';' )
        {
            if ( bprepro )
            {
                compile_warning( 0,"extra tokens at end of #include directive" );
                SKIP_ALL_UNTIL_LF_AND_COUNT_LINES;
                if ( *source_ptr == '\n' ) line_count--;
            }
            else
                token_next();
        }

        load_file( buffer );
        token_next();
        return;
    }
    line_count = actual_line;
    compile_error( MSG_FILENAME_EXP );
}

/* ---------------------------------------------------------------------- */

int find_define( int code )
{
    int i;

    /* Search for if already defined */
    for ( i = 0; i < defines_count; i++ ) if ( defines[i].code == code ) return i;

    return -1;
}

/* ---------------------------------------------------------------------- */

void add_simple_define( char * macro, char *text )
{
    int code = identifier_search_or_add( macro );

    if ( find_define( code ) != -1 ) compile_error( MSG_MACRO_ERROR, identifier_name( code ) );

    /* Allocate the macro */

    if ( defines_allocated == defines_count )
    {
        defines_allocated += 8;
        defines = ( DEFINE * ) realloc( defines, sizeof( DEFINE ) * defines_allocated );
    }

    defines[defines_count].param_count = -1;
    defines[defines_count].code = code;
    defines[defines_count].text = strdup( text );
    defines_count++;
}

/* ---------------------------------------------------------------------- */
/*
 *  FUNCTION : preprocessor_jumpto
 *
 *  Ignores all of the remaining source file, until
 *  a preprocessor directive with identifier id1
 *  (or id2 if non-zero), and moves the source_ptr
 *  source code pointer just after it.
 *
 *  This function is used by preprocessor()
 *  with a #ifdef, #ifndef or #if directive.
 *
 *  PARAMS :
 *      id1         Identifier of ending directive (i.e. id_endif)
 *      id2         Alternative ending directive (i.e. id_else) or 0 if none
 *
 *  RETURN VALUE :
 *      None
 */

void preprocessor_jumpto( int id, int id2 )
{
    int depth = 1;
    use_saved = 0;

    disable_prepro = 1;

    while ( depth > 0 && *source_ptr )
    {
        SKIP_SPACES_UNTIL_LF_AND_COUNT_LINES;
        if ( *source_ptr == '\n' )
        {
            source_ptr++;

            SKIP_SPACES;

            if ( *source_ptr == '#' )
            {
                source_ptr++;
                SKIP_SPACES_UNTIL_LF_AND_COUNT_LINES;
                if ( *source_ptr == '\n' ) line_count--;

                token_next();
                if ( token.type == IDENTIFIER )
                {
                    if ( token.code == id_endif )
                    {
                        int stck_code = prepro_stack[--prepro_sp];

                        if ( prepro_sp < 0 || (
                                    stck_code != id_else &&
                                    stck_code != id_if &&
                                    stck_code != id_ifdef &&
                                    stck_code != id_ifndef ) )
                            compile_error( "#endif without #if" );
                    }
                    if ( token.code == id_else )
                    {
                        int stck_code = prepro_stack[--prepro_sp];

                        if ( prepro_sp < 0 ||
                                (
                                    stck_code != id_if &&
                                    stck_code != id_ifdef &&
                                    stck_code != id_ifndef
                                )
                           )
                        {
                            if ( stck_code == id_else )
                            {
                                compile_error( "#else after #else" );
                            }
                            else
                            {
                                compile_error( "#else without #if" );
                            }
                        }
                        prepro_stack[prepro_sp++] = token.code;
                    }
                    else if ( token.code == id_ifdef || token.code == id_ifndef || token.code == id_if )
                    {
                        prepro_stack[prepro_sp++] = token.code;
                    }

                    if ( token.code == id_endif /*id || (id2 && token.code == id2)*/ )
                    {
                        depth--;
                    }
                    else if ( token.code == id_else )
                    {
                        depth--;
                        if ( !depth ) break;
                        depth++;
                    }
                    else if ( token.code == id_ifdef || token.code == id_ifndef || token.code == id_if )
                    {
                        depth++;
                    }
                }
                continue;
            }
        }
        source_ptr++;
    }

    if ( token.code != id && ( id2 && token.code != id2 ) ) compile_error( "unbalanced #if/#else/#endif" );

    if ( depth > 0 ) compile_error( "unterminate #if" );

    disable_prepro = 0;

}

/*
 *  FUNCTION : preprocessor_expand
 *
 *  Expands a macro to a malloc'ed text area and moves the
 *  token processing to it using token_init(). This function
 *  is called just after the macro name is read (next token
 *  should be a '(' if any parameters needed)
 *
 *  PARAMS :
 *      None
 *
 *  RETURN VALUE :
 *      None
 */

void preprocessor_expand( DEFINE * def )
{
    const char * param_left[MAX_MACRO_PARAMS];
    const char * param_right[MAX_MACRO_PARAMS];
    const char * begin = NULL;
    const char * old_source = NULL;
    char * text;
    int i, count, depth, allocated, size, part, actual_line_count;

    /* No params - easy case */

    if ( def->param_count == -1 )
    {
        int line = line_count - 1;
        token_init( def->text, current_file );
        line_count = line;
        return;
    }

    /* Find left parenthesis */

    disable_expand_defines++;
    token_next();
    disable_expand_defines--;
    if ( token.type != IDENTIFIER || token.code != identifier_leftp ) compile_error( MSG_EXPECTED, "(" );

    /* Mark parameters' starting and ending positions */

    if ( def->param_count > 0 )
    {
        for ( count = 0; count < def->param_count; count++ )
        {
            depth = 0;
            param_left[count] = source_ptr;

            while ( *source_ptr && ( depth > 0 || ( *source_ptr != ')' && *source_ptr != ',' ) ) )
            {
                if ( *source_ptr == '"' || *source_ptr == '\'' )
                {
                    begin = source_ptr++;
                    while ( *source_ptr && *source_ptr != *begin ) source_ptr++;
                    if ( !*source_ptr ) compile_error( MSG_EXPECTED, "\"" );
                    source_ptr++;
                    continue;
                }
                if ( *source_ptr == '(' ) depth++;
                if ( *source_ptr == ')' ) depth--;
                source_ptr++;
            }
            param_right[count] = source_ptr;
            if ( !*source_ptr ) compile_error( MSG_EXPECTED, ")" );
            if ( *source_ptr == ')' ) break;
            source_ptr++;
        }

        if ( count != def->param_count - 1 || *source_ptr != ')' )
            compile_error( MSG_INCORRECT_PARAMC, identifier_name( def->code ), def->param_count - 1 );
    }
    else
    {
        if ( *source_ptr != ')' )
            compile_error( MSG_INCORRECT_PARAMC, identifier_name( def->code ), def->param_count );
    }

    source_ptr++;

    /* Expand the macro */

    allocated = 128;
    size = 0;
    text = ( char * )calloc( allocated, sizeof( char ) );
    old_source = source_ptr;
    source_ptr = def->text;
    actual_line_count = line_count;

    while ( *source_ptr )
    {
        SKIP_SPACES_UNTIL_LF;
        if ( *source_ptr == '\n' ) break;

        begin = source_ptr;
        SKIP_SPACES_UNTIL_LF;
        if ( *source_ptr )
        {
            SKIP_SPACES_UNTIL_LF;
            if ( !*source_ptr ) break;
            if ( *source_ptr != '\n' )
            {
                disable_expand_defines++;
                token_next();
                disable_expand_defines--;
                if ( token.type == NOTOKEN ) break;
                if ( token.type == IDENTIFIER )
                {
                    /* Next token is an identifier. Search for parameter */

                    for ( i = 0; i < def->param_count; i++ )
                        if ( def->param_id[i] == token.code ) break;

                    if ( i != def->param_count )   /* Parameter found - expand it */
                    {

                        part = param_right[i] - param_left[i];
                        if ( size + part + 1 >= allocated )
                        {
                            allocated += (( part + 256 ) & ~ 127 );
                            text = ( char * )realloc( text, allocated );
                        }
                        text[size++] = ' ';
                        memcpy( text + size, param_left[i], part );
                        size += part;
                        continue;
                    }
                }

                /* No parameter found - copy the token */

                part = source_ptr - begin;
                if ( size + part + 1 >= allocated )
                {
                    allocated += (( part + 256 ) & ~127 );
                    text = ( char * )realloc( text, allocated );
                }
                memcpy( text + size, begin, part );
                size += part;
            }
            else
            {
                line_count++;
                source_ptr++;
            }
        }
    }

    text[size] = 0;
    source_ptr = old_source;
    line_count = actual_line_count;

    /* Now "include" the expanded text "file" */

    token_init( text, current_file );
    line_count = actual_line_count - 1;

    free( text );
}

/*
 *  FUNCTION : preprocessor
 *
 *  Evaluates a preprocessor directive. This function is called
 *  just after a '#' symbol is found after an end-of-line.
 *
 *  The function moves source_ptr to the next line.
 *
 *  PARAMS :
 *      None
 *
 *  RETURN VALUE :
 *      1 - Include identifier
 *      0 - Other identifier
 */

void preprocessor()
{
    int i, ifdef;
    char * ptr;
    int actual_line_count;

    static int initialized = 0;

    if ( !initialized )
    {
        id_define  = identifier_search_or_add( "DEFINE" );
        id_undef   = identifier_search_or_add( "UNDEF" );
        id_ifdef   = identifier_search_or_add( "IFDEF" );
        id_ifndef  = identifier_search_or_add( "IFNDEF" );
        id_else    = identifier_search_or_add( "ELSE" );
        id_endif   = identifier_search_or_add( "ENDIF" );
        id_if      = identifier_search_or_add( "IF" );
        initialized = 1;
    }

    token_next();

    if ( token.type != IDENTIFIER ) compile_error( MSG_UNKNOWN_PREP );

    /* #define TEXT value */

    if ( token.code == id_define )
    {
        disable_expand_defines++;

        token_next();
        if ( token.type != IDENTIFIER ) compile_error( MSG_INVALID_IDENTIFIER );

        if ( find_define( token.code ) != -1 ) compile_error( MSG_MACRO_ERROR, identifier_name( token.code ) );

        /* Allocate the macro */

        if ( defines_allocated == defines_count )
        {
            defines_allocated += 8;
            defines = ( DEFINE * ) realloc( defines, sizeof( DEFINE ) * defines_allocated );
        }

        defines[defines_count].code = token.code;

        /* Check for parameters: no space allowed between name and ( */

        if ( *source_ptr == '(' )
        {
            source_ptr++;
            for ( defines[defines_count].param_count = i = 0; *source_ptr != ')'; )
            {
                if ( !*source_ptr ) compile_error( MSG_EXPECTED, ")" );
                if ( i == MAX_MACRO_PARAMS ) compile_error( MSG_TOO_MANY_PARAMS );
                token_next();

                if ( token.type != IDENTIFIER || token.code < reserved_words ) compile_error( MSG_INVALID_IDENTIFIER );

                defines[defines_count].param_id[i++] = token.code;
                defines[defines_count].param_count++;

                SKIP_SPACES;
                if ( *source_ptr == ',' ) source_ptr++;
            }
            source_ptr++;
        }
        else
        {
            /* No parameters and no parenthesis */
            defines[defines_count].param_count = -1;
        }

        SKIP_SPACES_UNTIL_LF_AND_COUNT_LINES;

        ptr = ( char * ) source_ptr;

        while ( *ptr && *ptr != '\n' )
            if ( *ptr == '\\' && *( ptr + 1 ) == '\n' )
            {
                *ptr = ' ';
                ptr++;
                *ptr = ' ';
                ptr++;
                line_count++;
            }
            else
                ptr++;

        while ( ptr > source_ptr && ( !*ptr || ISSPACE( *ptr ) ) ) ptr--;

        defines[defines_count].text = ( char * )calloc( ptr - source_ptr + 2, sizeof( char ) );
        strncpy( defines[defines_count].text, source_ptr, ptr - source_ptr + 1 );
        defines[defines_count].text[ptr - source_ptr + 1] = 0;

        defines_count++;

        source_ptr = ptr + 1;

        disable_expand_defines--;

        return;
    }

    /* #undef TEXT */

    if ( token.code == id_undef )
    {
        disable_expand_defines++;

        token_next();
        if ( token.type != IDENTIFIER ) compile_error( MSG_INVALID_IDENTIFIER );

        if (( i = find_define( token.code ) ) != -1 )
        {
            defines_count--;
            if ( defines[i].text ) free( defines[i].text );
            memmove( &defines[i], &defines[i+1], ( defines_count - i ) * sizeof( DEFINE ) );
        }

        disable_expand_defines--;

        return;
    }

    /* #ifdef CONST / #ifndef CONST*/

    if ( token.code == id_ifdef || token.code == id_ifndef )
    {
        ifdef = token.code == id_ifdef;

        prepro_stack[prepro_sp++] = token.code;

        disable_expand_defines++;
        token_next();
        disable_expand_defines--;

        if ( token.type != IDENTIFIER ) compile_error( MSG_INVALID_IDENTIFIER );

        SKIP_SPACES_UNTIL_LF_AND_COUNT_LINES;
        if ( *source_ptr && *source_ptr != '\n' )
        {
            if ( ifdef ) compile_warning( 0,"extra tokens at end of #ifdef directive" );
            else compile_warning( 0,"extra tokens at end of #ifndef directive" );
            SKIP_ALL_UNTIL_LF_AND_COUNT_LINES;
        }

        if ( *source_ptr == '\n' ) line_count--;

        for ( i = 0; i < defines_count; i++ )
        {
            if ( defines[i].code == token.code )
            {
                if ( ifdef ) return;
                break;
            }
        }
        if ( !ifdef && i == defines_count ) return;


        preprocessor_jumpto( id_else, id_endif );

        SKIP_SPACES_UNTIL_LF_AND_COUNT_LINES;
        if ( *source_ptr && *source_ptr != '\n' )
        {
            if ( token.code == id_else ) compile_warning( 0,"extra tokens at end of #else directive" );
            else if ( token.code == id_endif ) compile_warning( 0,"extra tokens at end of #endif directive" );
            SKIP_ALL_UNTIL_LF_AND_COUNT_LINES;
        }
        if ( *source_ptr == '\n' ) line_count--;
        return;
    }

    /* #if */

    if ( token.code == id_if )
    {
        int actual_sources;
        expresion_result res;
        char c;

        prepro_stack[prepro_sp++] = token.code;

        ptr = ( char * ) source_ptr;

        while ( *ptr && *ptr != '\n' && *ptr != ';' )
            if ( *ptr == '\\' && *( ptr + 1 ) == '\n' )
            {
                *ptr = ' ';
                ptr++;
                *ptr = ' ';
                ptr++;
                line_count++;
            }
            else
                ptr++;

        c = *ptr;
        *ptr = '\0';

        actual_line_count = line_count;
        actual_sources = sources;

        token_init( source_ptr, current_file );

        identifiers_as_strings = 1;
        res = compile_expresion( 0, 0, 1, TYPE_DWORD );
        identifiers_as_strings = 0;
        /*
        printf ("exp: asignation: [%d] call: [%d] lvalue: [%d] constant: [%d] value: [%d] lvalue: [%f] type: [%d]\n",
                      res.asignation,
                      res.call,
                      res.lvalue,
                      res.constant,
                      res.value,
                      res.fvalue,
                      typedef_base(res.type));
        */
        if ( sources != actual_sources ) token_endfile();
        *ptr = c;
        source_ptr = ptr;
        line_count = actual_line_count;

        SKIP_SPACES_UNTIL_LF_AND_COUNT_LINES;
        if ( *source_ptr && *source_ptr != '\n' )
        {
            compile_warning( 0,"extra tokens at end of #if directive" );
            SKIP_ALL_UNTIL_LF_AND_COUNT_LINES;
        }
        if ( *source_ptr == '\n' ) line_count--;

        use_saved = 0;
        if ( !res.constant ) compile_error( MSG_CONSTANT_EXP );
        if ( !res.value )
        {
            preprocessor_jumpto( id_else, id_endif );

            SKIP_SPACES_UNTIL_LF_AND_COUNT_LINES;
            if ( *source_ptr && *source_ptr != '\n' )
            {
                if ( token.code == id_else ) compile_warning( 0,"extra tokens at end of #else directive" );
                else if ( token.code == id_endif ) compile_warning( 0,"extra tokens at end of #endif directive" );
                SKIP_ALL_UNTIL_LF_AND_COUNT_LINES;
            }
            if ( *source_ptr == '\n' ) line_count--;
        }

        return;
    }

    /* #else */

    if ( token.code == id_else )
    {
        if ( !prepro_sp ) compile_error( "#else without #if" );

        int stck_code = prepro_stack[--prepro_sp];

        if ( prepro_sp < 0 ||
                (
                    stck_code != id_if &&
                    stck_code != id_ifdef &&
                    stck_code != id_ifndef
                )
           )
        {
            if ( stck_code == id_else )
            {
                compile_error( "#else after #else" );
            }
            else
            {
                compile_error( "#else without #if" );
            }
        }
        prepro_stack[prepro_sp++] = token.code;

        SKIP_SPACES_UNTIL_LF_AND_COUNT_LINES;
        if ( *source_ptr && *source_ptr != '\n' )
        {
            compile_warning( 0,"extra tokens at end of #else directive" );
            SKIP_ALL_UNTIL_LF_AND_COUNT_LINES;
        }
        if ( *source_ptr == '\n' ) line_count--;

        preprocessor_jumpto( id_endif, 0 );

        SKIP_SPACES_UNTIL_LF_AND_COUNT_LINES;
        if ( *source_ptr && *source_ptr != '\n' )
        {
            compile_warning( 0,"extra tokens at end of #endif directive" );
            SKIP_ALL_UNTIL_LF_AND_COUNT_LINES;
        }
        if ( *source_ptr == '\n' ) line_count--;
        return;
    }

    /* #endif */

    if ( token.code == id_endif )
    {
        int stck_code = prepro_stack[--prepro_sp];

        if ( prepro_sp < 0 || (
                    stck_code != id_else &&
                    stck_code != id_if &&
                    stck_code != id_ifdef &&
                    stck_code != id_ifndef ) ) compile_error( "#endif without #if" );

        SKIP_SPACES_UNTIL_LF_AND_COUNT_LINES;
        if ( *source_ptr == '\n' ) line_count--;
        return;
    }

    /* Unknown preprocessor directive */
    compile_error( MSG_UNKNOWN_PREP );
}

void token_init( const char * source, int file )
{
    char * ptr;
    char * clean_source;

    if ( sources == MAX_SOURCES ) compile_error( MSG_TOO_MANY_INCLUDES );

    if ( !source )
    {
        fprintf( stdout, "token_init: no source\n" );
        exit( 1 );
    }

    /* Perform cleaning of the source file */

    clean_source = ( char * ) calloc( strlen( source ) + 2, sizeof( char ) );
    ptr = clean_source;
    *ptr++ = '\n';          /* Adds a blank line to detect first-line # directives */
    while ( *source )
    {
        if ( *source == '\r' && source[1] == '\n' )
        {
            source += 2;
            *ptr++ = '\n';
        }
        else
        {
            *ptr++ = *source++;
        }
    }
    *ptr = 0;

    /* Store the old source pointer */

    old_line_counts   [sources] = line_count;
    old_current_file  [sources] = current_file;
    old_sources       [sources] = source_ptr;
    old_sources_start [sources] = source_start;
    sources++;

    /* Use the new source */

    line_count = 0;
    current_file = file;
    source_ptr = clean_source;
    source_start = clean_source;

    use_saved = 0;
}

int token_endfile()
{
    if ( source_start )
    {
        free( source_start );
        source_start = 0;
    }
    if ( sources > 0 )
    {
        sources--;
        line_count = old_line_counts[sources];
        current_file = old_current_file[sources];
        source_ptr = old_sources[sources];
        source_start = old_sources_start[sources];
        use_saved = 0;
    }

    if ( sources < 1 )
    {
        if ( prepro_sp > 0 ) compile_error( "unbalanced #if/#else/#endif" );
    }
    return 0;
}

void token_dump()
{
    if ( token.type == NUMBER ) fprintf( stdout, "%d", token.code );
    else if ( token.type == FLOAT ) fprintf( stdout, "%f", token.value );
    else if ( token.type == STRING ) fprintf( stdout, "\"%s\"", string_get( token.code ) );
    else if ( token.type == IDENTIFIER ) fprintf( stdout, "\"%s\"", identifier_name( token.code ) );
    else if ( token.type == LABEL ) fprintf( stdout, "\"%s\"", identifier_name( token.code ) );
    else if ( token.type == NOTOKEN ) fprintf( stdout, "EOF" );
    else fprintf( stdout, "unknown" );
}

extern int c_type_initialized;

void token_next()
{
    static int  i, len;
    static char buffer[1024];
    char * buffer_ptr = buffer;

    if ( !source_ptr )
    {
        token.type = NOTOKEN;
        return;
    }

    if ( use_saved )
    {
        token        = token_saved;
        line_count   = token.line;
        current_file = token.file;
        use_saved    = 0;
        return;
    }
    token.line = line_count;
    token.file = current_file;
    token_prev = token;

    while ( 1 )
    {
        SKIP_SPACES;

        if ( !disable_prepro && *source_ptr == '#' )
        {
            int line;
            const char * old_source_ptr;

            identifiers_as_strings = 0;

            /* Comandos de preprocesador */
            source_ptr++;

            line = line_count;

            SKIP_SPACES_UNTIL_LF_AND_COUNT_LINES;

            if ( *source_ptr == '\n' )
            {
                token.code = identifier_search_or_add( "#" );
                token.type = IDENTIFIER;
                line_count = line;
                compile_error( MSG_IDENTIFIER_EXP );
            }

            old_source_ptr = source_ptr;

            if ( ISWORDFIRST( *source_ptr ) )
            {
                GET_NEXT_TOKEN_IN_TMPBUFFER;

                /* #include "file.h" */

                if ( token.code == identifier_include && !use_saved )
                {
                    include_file( 1 );
                    return;
                }

                source_ptr = old_source_ptr;
            }

            preprocessor();

            buffer_ptr = buffer;
            *buffer_ptr = '\0';
            continue;
        }

        if ( !*source_ptr )
        {
            /* Casos de bloques manuales */
            if ( current_file == -1 )
            {
                token_endfile();
                token.type = NOTOKEN;
                return;
            }

            while ( !*source_ptr )
            {
                if ( sources == 0 )
                {
                    token.type = NOTOKEN;
                    return;
                }

                token_endfile();

                if ( !source_ptr )
                {
                    token.type = NOTOKEN;
                    return;
                }
            }
            continue;
        }

        /* Ignora comentarios */

        SKIP_COMMENTS;
        if ( !*source_ptr )
        {
            token.type = NOTOKEN;
            return;
        }

        /* Cadenas */

        if ( *source_ptr == '"' || *source_ptr == '\'' )
        {
            token.type = STRING;
            token.code = string_compile( &source_ptr );
            token.line = line_count;
            token.file = current_file;
            return;
        }

        /* Operadores de más de un caracter */

        len = 0;

        if ( *source_ptr == '<' )
        {
            if ( source_ptr[1] == '<' )
            {
                if ( source_ptr[2] == '=' ) len = 3;
                else                      len = 2;
            }
            else if ( source_ptr[1] == '>' )    len = 2;
            else if ( source_ptr[1] == '=' )    len = 2;
            else                              len = 1;
        }
        else if ( *source_ptr == '>' )
        {
            if ( source_ptr[1] == '>' )
            {
                if ( source_ptr[2] == '=' ) len = 3;
                else                      len = 2;
            }
            else if ( source_ptr[1] == '=' )    len = 2;
            else if ( source_ptr[1] == '>' )    len = 2;
            else                              len = 1;
        }
        else if ( *source_ptr == '|' )
        {
            if ( source_ptr[1] == '|' )
            {
                if ( source_ptr[2] == '=' ) len = 3;
                else                      len = 2;
            }
            else if ( source_ptr[1] == '=' )    len = 2;
            else                              len = 1;
        }
        else if ( *source_ptr == '=' )
        {
            if ( source_ptr[1] == '=' )     len = 2;
            else if ( source_ptr[1] == '>' )    len = 2;
            else if ( source_ptr[1] == '<' )    len = 2;
            else                              len = 1;
        }
        else if ( *source_ptr == '.' )
        {
            if ( source_ptr[1] == '.' )     len = 2;
            else                          len = 1;
        }
        else if ( strchr( "!&^%*+-/", *source_ptr ) )
        {
            if ( source_ptr[1] == '=' )     len = 2;
            else if ( strchr( "+-&^", *source_ptr ) && source_ptr[1] == *source_ptr ) len = 2;
            else                          len = 1;
        }

        if ( len )
        {
            strncpy( buffer, source_ptr, len );
            buffer[len] = 0;
            source_ptr += len;
            token.code = identifier_search_or_add( buffer );
            token.type = IDENTIFIER;
            token.line = line_count;
            token.file = current_file;
            return;
        }

        /* Numbers */

        if ( ISNUM( *source_ptr ) )
        {
            const char * ptr;
            double num = 0, dec;
            int base = 10;

            /* Hex/Bin/Octal numbers with the h/b/o sufix */
            ptr = source_ptr;
            while ( ISNUM( *ptr ) || ( *ptr >= 'a' && *ptr <= 'f' ) || ( *ptr >= 'A' && *ptr <= 'F' ) ) ptr++;

            if ( *ptr != 'h' && *ptr != 'H' && *ptr != 'o' && *ptr != 'O' && ( ptr[-1] == 'b' || ptr[-1] == 'B' ) ) ptr--;

            if ( *ptr == 'b' || *ptr == 'B' )
                base = 2;
            if ( *ptr == 'h' || *ptr == 'H' )
                base = 16;
            if ( *ptr == 'o' || *ptr == 'O' )
                base = 8;

            token.code = 0 ; /* for ints values */

            /* Calculate the number value */

            while ( ISNUM( *source_ptr ) || ( base > 10 && ISALNUM( *source_ptr ) ) )
            {
                if ( base == 2 && *source_ptr != '0' && *source_ptr != '1' ) break;
                if ( base == 8 && ( *source_ptr < '0' || *source_ptr > '7' ) ) break;
                if ( base == 10 && !ISNUM( *source_ptr ) ) break;
                if ( base == 16 && !ISNUM( *source_ptr ) && ( TOUPPER( *source_ptr ) < 'A' || TOUPPER( *source_ptr ) > 'F' ) ) break;

                if ( ISNUM( *source_ptr ) )
                {
                    num = num * base + ( *source_ptr - '0' );
                    token.code = token.code * base + ( *source_ptr - '0' );
                    source_ptr++;
                    continue;
                }
                if ( *source_ptr >= 'a' && *source_ptr <= 'f' && base > 10 )
                {
                    num = num * base + ( *source_ptr - 'a' + 10 );
                    token.code = token.code * base + ( *source_ptr - 'a' + 10 );
                    source_ptr++;
                    continue;
                }
                if ( *source_ptr >= 'A' && *source_ptr <= 'F' && base > 10 )
                {
                    num = num * base + ( *source_ptr - 'A' + 10 );
                    token.code = token.code * base + ( *source_ptr - 'A' + 10 );
                    source_ptr++;
                    continue;
                }
            }
            token.type = NUMBER;
            token.value = ( float )num;

            /* We have the integer part now - convert to int/float */

            if ( *source_ptr == '.' && base == 10 )
            {
                source_ptr++;
                if ( !ISNUM( *source_ptr ) )
                    source_ptr--;
                else
                {
                    dec = 1.0 / ( double )base;
                    while ( ISNUM( *source_ptr ) || ( base > 100 && ISALNUM( *source_ptr ) ) )
                    {
                        if ( ISNUM( *source_ptr ) ) num = num + dec * ( *source_ptr++ - '0' );
                        if ( *source_ptr >= 'a' && *source_ptr <= 'f' && base > 10 ) num = num + dec * ( *source_ptr++ - 'a' + 10 );
                        if ( *source_ptr >= 'A' && *source_ptr <= 'F' && base > 10 ) num = num + dec * ( *source_ptr++ - 'A' + 10 );
                        dec /= ( double )base;
                    }
                    token.type  = FLOAT;
                    token.value = ( float )num;
                }
            }

            /* Skip the base sufix */

            if ( base == 16 && ( *source_ptr == 'h' || *source_ptr == 'H' ) ) source_ptr++;
            if ( base == 8  && ( *source_ptr == 'o' || *source_ptr == 'O' ) ) source_ptr++;
            if ( base == 2  && ( *source_ptr == 'b' || *source_ptr == 'B' ) ) source_ptr++;

            token.line = line_count;
            token.file = current_file;
            return;
        }

        /* Identificadores */
        if ( ISWORDFIRST( *source_ptr ) )
        {
            int maybe_label = source_ptr[-1] == '\n';
            GET_NEXT_TOKEN_IN_TMPBUFFER;

            token.line = line_count;
            token.file = current_file;

            if ( maybe_label && *source_ptr == ':' )
            {
                source_ptr++;
                token.code = identifier_search_or_add( buffer );
                token.type = LABEL;
                return;
            }

            /* Search for #define constant inclusion at this point */

            if ( !disable_expand_defines )
            {
                if ( !strcmp( buffer, "__FILE__" ) )
                {
                    token.type = STRING;
                    token.code = string_new(( current_file != -1 && files[current_file] && *files[current_file] ) ? files[current_file] : "N/A" );
                    token.line = line_count;
                    token.file = current_file;
                    return;
                }

                if ( !strcmp( buffer, "__LINE__" ) )
                {
                    token.type = NUMBER;
                    token.code = ( int )line_count;
                    token.value = ( float )line_count;
                    token.line = line_count;
                    token.file = current_file;
                    return;
                }

                for ( i = 0; i < defines_count; i++ )
                {
                    if ( defines[i].code == token.code )
                    {
                        preprocessor_expand( &defines[i] );
                        token_next();
                        token.line = line_count;
                        token.file = current_file;
                        return;
                    }
                }
            }

            /* In a #if, all identifiers are strings */

            if ( identifiers_as_strings )
            {
                token.type = STRING;
                token.code = string_new( buffer );
                token.line = line_count;
                token.file = current_file;
                return;
            }

            /* Include */

            if ( !disable_prepro && token.code == identifier_include && !use_saved )
            {
                include_file( 0 );
                return;
            }
            return;
        }

        /* 1-char operator or invalid symbol */

        if ( !*source_ptr ) break;

        if ( *source_ptr > 0 && *source_ptr < 32 ) compile_error( MSG_INVALID_CHAR );

        *buffer_ptr++ = *source_ptr++;
        *buffer_ptr = 0;

        token.code = identifier_search_or_add( buffer );
        token.type = IDENTIFIER;
        token.line = line_count;
        token.file = current_file;

        return;
    }

    token.type = NOTOKEN;
    return;        /* End-of-file */
}

void token_back()
{
    if ( use_saved )
    {
        fprintf( stdout, "Error: two token_back in a row\n" );
        exit( 1 );
    }

    token.line      = line_count;
    token.file      = current_file;
    token_saved     = token;

    token           = token_prev;

    line_count      = token.line;
    current_file    = token.file;

    use_saved = 1;
}

tok_pos token_pos()
{
    tok_pos tp;

    tp.use_saved    = use_saved;
    tp.token_saved  = token_saved;
    tp.token        = token;
    tp.line_count   = line_count;
    tp.current_file = current_file;
    tp.token_prev   = token_prev;
    tp.source_ptr   = source_ptr;

    return tp;
}

void token_set_pos( tok_pos tp )
{
    use_saved       = tp.use_saved;
    token_saved     = tp.token_saved;
    token           = tp.token;
    line_count      = tp.line_count;
    current_file    = tp.current_file;
    token_prev      = tp.token_prev;
    source_ptr      = tp.source_ptr;
}

