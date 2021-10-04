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

#include <loadlib.h> /* Must be first include */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#include "bgdc.h"

#include <bgddl.h>

/* ---------------------------------------------------------------------- */

extern void token_dump() ;

static char * import_filename = NULL;
static int import_line = 0;

int imports_aux[512] ;  /* Modules */
int nimports_aux = 0 ;
int imports[512] ;      /* Modules */
int nimports = 0 ;

/* ---------------------------------------------------------------------- */
/* Módulo principal del compilador. Contiene código que inicializa los    */
/* identificadores conocidos, así como las funciones de compilado de      */
/* nivel más alto                                                         */
/* ---------------------------------------------------------------------- */

/* Tipos */
int identifier_dword ;
int identifier_word ;
int identifier_byte ;
int identifier_int ;
int identifier_short ;
int identifier_char ;
int identifier_unsigned ;
int identifier_signed ;
int identifier_string ;
int identifier_float ;
int identifier_struct ;
int identifier_type ;

int identifier_declare ;

int identifier_program ;
int identifier_debug ;
int identifier_begin ;
int identifier_end ;
int identifier_global ;
int identifier_local ;
int identifier_public ;
int identifier_private ;
int identifier_const ;
int identifier_dup ;
int identifier_from ;
int identifier_step ;
int identifier_to ;
int identifier_if ;
int identifier_else ;
int identifier_elseif ;
int identifier_for ;
int identifier_while ;
int identifier_frame ;
int identifier_clone ;

int identifier_yield ;

int identifier_onexit ;
int identifier_onerror ;

int identifier_on ;
int identifier_exit ;
int identifier_error ;
int identifier_resume ;
int identifier_stm_next ;

int identifier_jmp ;
int identifier_call ;

int identifier_switch ;
int identifier_case ;
int identifier_default ;
int identifier_repeat ;
int identifier_until ;
int identifier_loop ;
int identifier_break ;
int identifier_continue ;
int identifier_return ;
int identifier_process ;
int identifier_function ;
int identifier_bandoffset ;
int identifier_offset ;
int identifier_sizeof ;
int identifier_pointer ;

int identifier_and ;
int identifier_or  ;
int identifier_xor ;
int identifier_not ;

int identifier_band ;
int identifier_bor  ;
int identifier_bxor ;
int identifier_bnot ;

int identifier_plus ;
int identifier_minus ;
int identifier_plusplus ;
int identifier_minusminus ;
int identifier_equal ;
int identifier_multiply ;
int identifier_mod ;
int identifier_divide ;
int identifier_semicolon ;
int identifier_colon ;
int identifier_comma ;
int identifier_ror ;
int identifier_rol ;
int identifier_rightp ;
int identifier_leftp ;
int identifier_rightb ;
int identifier_leftb ;
int identifier_point ;
int identifier_twopoints ;

int identifier_eq ;
int identifier_ne ;
int identifier_gte ;
int identifier_lte ;
int identifier_lt ;
int identifier_gt ;
int identifier_question ;

int identifier_plusequal ;
int identifier_minusequal ;
int identifier_multequal ;
int identifier_divequal ;
int identifier_modequal ;
int identifier_orequal ;
int identifier_xorequal ;
int identifier_andequal ;
int identifier_rorequal ;
int identifier_rolequal ;
int identifier_mouse ;
int identifier_include ;

int identifier_import ;
int reserved_words ;
int debug = 0 ;

/* ---------------------------------------------------------------------- */

void compile_init()
{
    /* Initialize reserved words */

    identifier_dword        = identifier_add( "DWORD" ) ;
    identifier_word         = identifier_add( "WORD" ) ;
    identifier_byte         = identifier_add( "BYTE" ) ;
    identifier_int          = identifier_add( "INT" ) ;
    identifier_short        = identifier_add( "SHORT" ) ;
    identifier_char         = identifier_add( "CHAR" ) ;
    identifier_unsigned     = identifier_add( "UNSIGNED" ) ;
    identifier_signed       = identifier_add( "SIGNED" ) ;
    identifier_float        = identifier_add( "FLOAT" ) ;
    identifier_string       = identifier_add( "STRING" ) ;
    identifier_struct       = identifier_add( "STRUCT" ) ;

    identifier_declare      = identifier_add( "DECLARE" ) ;

    identifier_include      = identifier_add( "INCLUDE" ) ;

    identifier_program      = identifier_add( "PROGRAM" ) ;
    identifier_debug        = identifier_add( "DEBUG" ) ;
    identifier_const        = identifier_add( "CONST" ) ;
    identifier_begin        = identifier_add( "BEGIN" ) ;
    identifier_end          = identifier_add( "END" ) ;
    identifier_process      = identifier_add( "PROCESS" ) ;
    identifier_function     = identifier_add( "FUNCTION" ) ;
    identifier_global       = identifier_add( "GLOBAL" ) ;
    identifier_local        = identifier_add( "LOCAL" ) ;
    identifier_public       = identifier_add( "PUBLIC" ) ;
    identifier_private      = identifier_add( "PRIVATE" ) ;
    identifier_dup          = identifier_add( "DUP" ) ;
    identifier_from         = identifier_add( "FROM" ) ;
    identifier_to           = identifier_add( "TO" ) ;
    identifier_step         = identifier_add( "STEP" ) ;
    identifier_for          = identifier_add( "FOR" ) ;
    identifier_while        = identifier_add( "WHILE" ) ;
    identifier_repeat       = identifier_add( "REPEAT" ) ;
    identifier_until        = identifier_add( "UNTIL" ) ;
    identifier_switch       = identifier_add( "SWITCH" ) ;
    identifier_case         = identifier_add( "CASE" ) ;
    identifier_default      = identifier_add( "DEFAULT" ) ;
    identifier_loop         = identifier_add( "LOOP" ) ;
    identifier_break        = identifier_add( "BREAK" ) ;
    identifier_continue     = identifier_add( "CONTINUE" ) ;
    identifier_return       = identifier_add( "RETURN" ) ;
    identifier_if           = identifier_add( "IF" ) ;
    identifier_else         = identifier_add( "ELSE" ) ;
    identifier_elseif       = identifier_add( "ELSEIF" ) ;
    identifier_frame        = identifier_add( "FRAME" ) ;
    identifier_clone        = identifier_add( "CLONE" ) ;

    identifier_yield        = identifier_add( "YIELD" ) ;

    identifier_onexit       = identifier_add( "ONEXIT" ) ;
    identifier_onerror      = identifier_add( "ONERROR" ) ;

    identifier_on           = identifier_add( "ON" ) ;
    identifier_exit         = identifier_add( "EXIT" ) ;
    identifier_error        = identifier_add( "ERROR" ) ;
    identifier_resume       = identifier_add( "RESUME" ) ;
    identifier_stm_next     = identifier_add( "NEXT" ) ;

    identifier_jmp          = identifier_add( "JMP" ) ;
    identifier_add_as( "GOTO", identifier_jmp ) ;

    identifier_call         = identifier_add( "CALL" ) ;

    identifier_and          = identifier_add( "AND" );
    identifier_or           = identifier_add( "OR" );
    identifier_xor          = identifier_add( "XOR" );
    identifier_not          = identifier_add( "NOT" );

    identifier_band         = identifier_add( "BAND" );
    identifier_bor          = identifier_add( "BOR" );
    identifier_bxor         = identifier_add( "BXOR" );
    identifier_bnot         = identifier_add( "BNOT" );

    identifier_sizeof       = identifier_add( "SIZEOF" );
    identifier_offset       = identifier_add( "OFFSET" );
    identifier_pointer      = identifier_add( "POINTER" );
    identifier_type         = identifier_add( "TYPE" );

    identifier_bandoffset   = identifier_add( "&" );

    identifier_add_as( "!", identifier_not ) ;
    identifier_add_as( "&&", identifier_and ) ;
    identifier_add_as( "||", identifier_or ) ;
    identifier_add_as( "^^", identifier_xor ) ;

    identifier_add_as( "~", identifier_bnot ) ;
    identifier_add_as( "|", identifier_bor ) ;
    identifier_add_as( "^", identifier_bxor ) ;

    identifier_plus         = identifier_add( "+" ) ;
    identifier_minus        = identifier_add( "-" ) ;
    identifier_plusplus     = identifier_add( "++" ) ;
    identifier_minusminus   = identifier_add( "--" ) ;
    identifier_multiply     = identifier_add( "*" ) ;
    identifier_mod          = identifier_add( "%" ) ;
    identifier_divide       = identifier_add( "/" ) ;
    identifier_equal        = identifier_add( "=" ) ;
    identifier_semicolon    = identifier_add( ";" ) ;
    identifier_comma        = identifier_add( "," ) ;
    identifier_ror          = identifier_add( ">>" ) ;
    identifier_rol          = identifier_add( "<<" ) ;
    identifier_rightp       = identifier_add( ")" ) ;
    identifier_leftp        = identifier_add( "(" ) ;
    identifier_rightb       = identifier_add( "]" ) ;
    identifier_leftb        = identifier_add( "[" ) ;
    identifier_point        = identifier_add( "." ) ;
    identifier_twopoints    = identifier_add( ".." ) ;
    identifier_question     = identifier_add( "?" ) ;

    identifier_add_as( "MOD", identifier_mod ) ;
    identifier_add_as( "ELIF", identifier_elseif );
    identifier_add_as( "ELSIF", identifier_elseif );


    identifier_colon        = identifier_add( ":" ) ;

    identifier_eq           = identifier_add( "==" ) ;
    identifier_ne           = identifier_add( "!=" ) ;
    identifier_gt           = identifier_add( ">" ) ;
    identifier_lt           = identifier_add( "<" ) ;
    identifier_gte          = identifier_add( ">=" ) ;
    identifier_lte          = identifier_add( "<=" ) ;

    identifier_add_as( "<>", identifier_ne ) ;
    identifier_add_as( "=>", identifier_gte ) ;
    identifier_add_as( "=<", identifier_lte ) ;

    identifier_plusequal    = identifier_add( "+=" ) ;
    identifier_andequal     = identifier_add( "&=" ) ;
    identifier_xorequal     = identifier_add( "^=" ) ;
    identifier_orequal      = identifier_add( "|=" ) ;
    identifier_divequal     = identifier_add( "/=" ) ;
    identifier_modequal     = identifier_add( "%=" ) ;
    identifier_multequal    = identifier_add( "*=" ) ;
    identifier_minusequal   = identifier_add( "-=" ) ;
    identifier_rorequal     = identifier_add( ">>=" ) ;
    identifier_rolequal     = identifier_add( "<<=" ) ;

    identifier_import       = identifier_add( "IMPORT" ) ;

    reserved_words          = identifier_next_code() ;

    identifier_mouse        = identifier_add( "MOUSE" ) ; /* Hack */

    varspace_init( &global ) ;
    varspace_init( &local ) ;
    globaldata = segment_new() ;
    localdata = segment_new() ;

}

/* ---------------------------------------------------------------------- */

void compile_error( const char *fmt, ... )
{
    char text[4000] ;
    char * fname = ( import_filename ) ? import_filename : (( current_file != -1 && files[current_file] && *files[current_file] ) ? files[current_file] : NULL );

    va_list ap;
    va_start( ap, fmt );
    vsprintf( text, fmt, ap );
    va_end( ap );

    fprintf( stdout, MSG_COMPILE_ERROR,
            ( fname && ( fname[0] != '/' && fname[0] != '\\' && fname[1] != ':' ) ) ?  main_path : "",
            fname ? fname : "N/A",
            ( import_filename ) ? import_line : line_count,
            text ) ;
    fprintf( stdout, " ( token error: " );
    token_dump() ;
    fprintf( stdout, " ).\n" );
    exit( 2 ) ;
}

/* ---------------------------------------------------------------------- */

void compile_warning( int notoken, const char *fmt, ... )
{
    char text[4000] ;
    char * fname = ( import_filename ) ? import_filename : (( current_file != -1 && files[current_file] && *files[current_file] ) ? files[current_file] : NULL );
    
    va_list ap;
    va_start( ap, fmt );
    vsprintf( text, fmt, ap );
    va_end( ap );
    
    fprintf( stdout, MSG_COMPILE_WARNING,
            ( fname && ( fname[0] != '/' && fname[0] != '\\' && fname[1] != ':' ) ) ?  main_path : "",
            fname ? fname : "N/A",
            ( import_filename ) ? import_line : line_count,
            text ) ;
    if ( !notoken )
    {
        fprintf( stdout, " ( token warning: " );
        token_dump() ;
        fprintf( stdout, " ).\n" );
    } else {
        fprintf( stdout, ".\n" );
    }
    
}

/* ---------------------------------------------------------------------- */

static char * trim( char * ptr )
{
    char * ostr = ptr;
    char * bptr = ptr;

    while ( *ptr == ' ' || *ptr == '\n' || *ptr == '\r' || *ptr == '\t' ) ptr++;

    while ( *ptr ) *bptr++ = *ptr++;

    while ( bptr > ostr && ( bptr[-1] == ' ' || bptr[-1] == '\n' || bptr[-1] == '\r' || bptr[-1] == '\t' ) ) bptr--;

    *bptr = 0;

    return ( ostr );
}

/* ---------------------------------------------------------------------- */

static int import_exists( char * libname )
{
    int n;
    for ( n = 0; n < nimports_aux; n++ ) if ( !strcmp( libname, string_get( imports_aux[n] ) ) ) return n;
    return -1;
}

/* ---------------------------------------------------------------------- */

void compile_type()
{
    int code;
    segment  * s = segment_new() ;
    VARSPACE * v = varspace_new() ;
    TYPEDEF    t = typedef_new( TYPE_STRUCT );

    t.chunk[0].count = 1 ;

    token_next() ;
    if (( code = token.code ) < reserved_words || token.type != IDENTIFIER ) compile_error( MSG_INVALID_TYPE ) ;

    t.varspace = v ;
    typedef_name( t, code ) ;
    segment_name( s, code ) ;

    /* (2006/11/19 19:34 GMT-03:00, Splinter - jj_arg@yahoo.com) */
    compile_varspace( v, s, 0, 1, 0, NULL, 0, 0 ) ;
    if ( token.code != identifier_end ) compile_error( MSG_NO_END ) ;
}

/* ---------------------------------------------------------------------- */

static char * modules_exts[] =
{
    ".dll",
    ".dylib",
    ".so",
    NULL
} ;

static char * dlsearchpath[] =
{
    "modules",
    "mod",
    "mods",
    "extensions",
    "plugins",
    "data",
    NULL
};

static void import_module( const char * filename )
{
    int         libid ;
    void        * library = NULL;

    char        ** globals_def = NULL;
    char        ** locals_def = NULL;
    DLCONSTANT  * constants_def = NULL;
    DLSYSFUNCS  * functions_exports = NULL;
    char        ** modules_dependency = NULL;
    char        ** types_def = NULL;

    char        soname[ __MAX_PATH ], fullsoname[ __MAX_PATH ], **spath ;
    char        * ptr;
    char        ** pex;

#if defined( __MONOLITHIC__ )
#define DLLEXT      ".fakelib"
#define SIZEDLLEXT  8
#elif defined( WIN32 )
#define DLLEXT      ".dll"
#define SIZEDLLEXT  4
#elif defined(TARGET_MAC)
#define DLLEXT      ".dylib"
#define SIZEDLLEXT  6
#else
#define DLLEXT      ".so"
#define SIZEDLLEXT  3
#endif

    strncpy( soname, filename, sizeof( soname ) );

    for ( ptr = soname; *ptr; ptr++ )
    {
        if ( *ptr == PATH_CHAR_SEP ) *ptr = PATH_CHAR_ISEP ;
        else *ptr = TOLOWER( *ptr );
    }

    pex = modules_exts;
    while ( pex && * pex )
    {
        int nlen = strlen( soname );
        int elen = strlen( *pex );
        if ( nlen > elen && strcmp( &soname[nlen - elen], *pex ) == 0 )
        {
            soname[nlen - elen] = '\0' ;
            pex = modules_exts;
        }
        else
        {
            pex++;
        }
    }

    if ( import_exists(( char * )soname ) != -1 ) return;

    filename = soname;
    libid = string_new( soname );

    imports_aux[nimports_aux++] = libid;

    strcat( soname, DLLEXT );

    fullsoname[0] = '\0';

    library  = dlibopen( filename ) ;

    spath = dlsearchpath;
    while( !library && spath && *spath )
    {
#ifdef _WIN32
        sprintf( fullsoname, "%s%s\\%s", appexepath, *spath, filename );
#else
        sprintf( fullsoname, "%s%s/%s", appexepath, *spath, filename );
#endif
        library  = dlibopen( fullsoname ) ;
        spath++;
    }

    if ( !library ) compile_error( MSG_LIBRARY_NOT_FOUND, filename ) ;

    modules_dependency = ( char ** ) _dlibaddr( library, "modules_dependency" ) ;

    if ( modules_dependency )
    {
        char * old_import_filename = import_filename;
        while ( *modules_dependency )
        {
            if ( import_exists( *modules_dependency ) == -1 )
            {
                import_filename = *modules_dependency ;
                import_module( *modules_dependency );
                import_filename = NULL ;
            }
            modules_dependency++;
        }
        import_filename = old_import_filename;
    }

    imports[nimports++] = libid;

    constants_def = ( DLCONSTANT * ) _dlibaddr( library, "constants_def" ) ;
    if ( constants_def )
    {
        while ( constants_def->name )
        {
            int code = identifier_search_or_add( constants_def->name ) ;
            constants_add( code, typedef_new( constants_def->type ), constants_def->code ) ;
            constants_def++ ;
        }
    }

    types_def = ( char ** ) _dlibaddr( library, "types_def" ) ;
    if ( types_def && *types_def )
    {
        token_init( *types_def, -1 ) ;
        token_next();
        while ( token.type == IDENTIFIER && token.code == identifier_type )
        {
            compile_type() ;
            token_next();
        }
    }

    globals_def = ( char ** ) _dlibaddr( library, "globals_def" ) ;
    if ( globals_def && *globals_def )
    {
        VARSPACE * v[] = {&local, NULL};
        token_init( *globals_def, -1 ) ;
        compile_varspace( &global, globaldata, 1, 1, 0, v, DEFAULT_ALIGNMENT, 1 ) ;
    }

    locals_def  = ( char ** ) _dlibaddr( library, "locals_def" ) ;
    if ( locals_def && *locals_def )
    {
        VARSPACE * v[] = {&global, NULL};

        token_init( *locals_def, -1 ) ;
        compile_varspace( &local, localdata, 1, 1, 0, v, DEFAULT_ALIGNMENT, 1 ) ;
    }

    functions_exports = ( DLSYSFUNCS * ) _dlibaddr( library, "functions_exports" ) ;
    if ( functions_exports )
    {
        while ( functions_exports->name )
        {
            sysproc_add( functions_exports->name, functions_exports->paramtypes, functions_exports->type, functions_exports->func );
            functions_exports++;
        }
    }
}

/* ---------------------------------------------------------------------- */

void import_files( char * filename )
{
    file * fp = file_open( filename, "rb0" );
    char libname[__MAX_PATH];

    if ( !fp ) return;

    import_line = 0;

    while ( !file_eof( fp ) )
    {
        import_line++;
        if ( !file_gets( fp, libname, sizeof( libname ) ) ) break;
        trim( libname );
        if ( *libname == '\0' ) continue;
        if ( import_exists( libname ) != -1 ) continue;
        import_filename = filename ;
        import_module( libname );
        import_filename = NULL ;
    }

    file_close( fp );
}

/* ---------------------------------------------------------------------- */

void import_mod( char * libname )
{
    import_line = 0;
    if ( import_exists( libname ) != -1 ) return;
    import_filename = libname ;
    import_module( libname );
    import_filename = NULL ;
}

/* ---------------------------------------------------------------------- */

void compile_import( void )
{
    no_include_this_file = 1;

    token_next() ;
    if ( token.type != STRING ) compile_error( MSG_STRING_EXP ) ;

    import_module( string_get( token.code ) );
}

/* ---------------------------------------------------------------------- */

void compile_constants()
{
    int code;
    expresion_result res;

    for ( ;; )
    {
        token_next() ;
        if ( token.type == NOTOKEN ) break ;

        if ( token.type != IDENTIFIER ) compile_error( MSG_CONSTANT_NAME_EXP ) ;

        if ( token.code == identifier_semicolon ) continue ;

        if ( token.code == identifier_begin ||
             token.code == identifier_local ||
             token.code == identifier_public ||
             token.code == identifier_private ||
             token.code == identifier_global )
        {
            token_back() ;
            return ;
        }

        if ( token.code == identifier_end ) return ;

        if ( token.code < reserved_words ) compile_error( MSG_INVALID_IDENTIFIER );

        code = token.code ;

        token_next() ;
        if ( token.type != IDENTIFIER || token.code != identifier_equal ) compile_error( MSG_EXPECTED, "=" ) ;

        res = compile_expresion( 1, 0, 0, 0 ) ;
        if ( !typedef_is_float( res.type ) && !typedef_is_string( res.type ) && !typedef_is_integer( res.type ) ) compile_error( MSG_INVALID_TYPE );

        constants_add( code, res.type, typedef_base( res.type ) == TYPE_FLOAT ? *( int * )&res.fvalue : res.value ) ;

        token_next() ;
        if ( token.type == IDENTIFIER && token.code == identifier_semicolon ) continue ;

        compile_error( MSG_EXPECTED, ";" ) ;
    }
}

/* ---------------------------------------------------------------------- */

void compile_process()
{
    PROCDEF * proc, * external_proc ;
    VARIABLE  * var ;
    int is_declare = 0 ;
    int is_function = 0 ;
    int type_implicit = 1;
    int code, tcode, params ;
    BASETYPE type, typeb ;
    TYPEDEF ctype, ctypeb ;

    int signed_prefix = 0;
    int unsigned_prefix = 0;

    tcode = token.code ;

    if ( tcode == identifier_declare )   /* Es una declaracion? */
    {
        is_declare = 1;
        token_next() ;
        tcode = token.code;
    }

    /* Es funcion? */
    if ( tcode == identifier_function ) is_function = 1;

    if (( tcode == identifier_process || tcode == identifier_function ) )   /* Si proceso o funcion, obtengo el signo */
    {
        token_next() ;
        tcode = token.code;
    }

    if ( token.code == identifier_signed ) /* signed */
    {
        signed_prefix = 1;
        token_next();
        tcode = token.code;
    }
    else if ( token.code == identifier_unsigned ) /* unsigned */
    {
        unsigned_prefix = 1;
        token_next();
        tcode = token.code;
    }

    if ( segment_by_name( token.code ) )   /* Nombre del Segmento al que pertenece */
    {
        tcode = identifier_pointer ;
        token_next() ;
    }

    if ( identifier_is_basic_type( token.code ) )   /* Salto identificador de tipo basico */
    {
        tcode = token.code ;
        token_next();
    }

    while ( token.code == identifier_pointer || token.code == identifier_multiply ) /* Salto el indentificador POINTER */
    {
        tcode = token.code ;
        token_next() ;
    }

    /* Check if the process name is valid */

    if ( token.type != IDENTIFIER || token.code < reserved_words ) compile_error( MSG_PROCNAME_EXP ) ;

    code = token.code ;

    /* Create the process if it is not defined already */
    proc = procdef_search( code ) ;
    if ( !proc ) proc = procdef_new( procdef_getid(), code ) ;
    else if ( proc->defined ) compile_error( MSG_PROC_ALREADY_DEFINED );
    else if ( is_declare && proc->declared ) compile_error( MSG_PROC_ALREADY_DECLARED );

    /* is declaration? */
    if ( !is_declare ) proc->defined = 1 ;

    /* is a function? */
    if ( is_function )
    {
        if ( is_declare && proc->declared && !( proc->flags & PROC_FUNCTION ) ) compile_error( MSG_PROTO_ERROR ) ;
        proc->flags |= PROC_FUNCTION;
    }

    /* Get function/process return type */

    if ( tcode == identifier_float )
    {
        if ( is_declare && proc->declared && proc->type != TYPE_FLOAT ) compile_error( MSG_PROTO_ERROR ) ;
        proc->type = TYPE_FLOAT ;
    }

    if ( tcode == identifier_string )
    {
        if ( is_declare && proc->declared && proc->type != TYPE_STRING ) compile_error( MSG_PROTO_ERROR ) ;
        proc->type = TYPE_STRING ;
    }

    if ( tcode == identifier_word )
    {
        if ( is_declare && proc->declared && proc->type != ( signed_prefix ? TYPE_SHORT : TYPE_WORD ) ) compile_error( MSG_PROTO_ERROR ) ;
        proc->type = signed_prefix ? TYPE_SHORT : TYPE_WORD;
        signed_prefix = unsigned_prefix = 0;
    }

    if ( tcode == identifier_dword )
    {
        if ( is_declare && proc->declared && proc->type != ( signed_prefix ? TYPE_INT : TYPE_DWORD ) ) compile_error( MSG_PROTO_ERROR ) ;
        proc->type = signed_prefix ? TYPE_INT : TYPE_DWORD;
        signed_prefix = unsigned_prefix = 0;
    }

    if ( tcode == identifier_byte )
    {
        if ( is_declare && proc->declared && proc->type != ( signed_prefix ? TYPE_SBYTE : TYPE_BYTE ) ) compile_error( MSG_PROTO_ERROR ) ;
        proc->type = signed_prefix ? TYPE_SBYTE : TYPE_BYTE;
        signed_prefix = unsigned_prefix = 0;
    }

    if ( tcode == identifier_int )
    {
        if ( is_declare && proc->declared && proc->type != ( unsigned_prefix ? TYPE_DWORD : TYPE_INT ) ) compile_error( MSG_PROTO_ERROR ) ;
        proc->type = unsigned_prefix ? TYPE_DWORD : TYPE_INT;
        signed_prefix = unsigned_prefix = 0;
    }

    if ( tcode == identifier_short )
    {
        if ( is_declare && proc->declared && proc->type != ( unsigned_prefix ? TYPE_WORD : TYPE_SHORT ) ) compile_error( MSG_PROTO_ERROR ) ;
        proc->type = unsigned_prefix ? TYPE_WORD : TYPE_SHORT;
        signed_prefix = unsigned_prefix = 0;
    }

    if ( tcode == identifier_char )
    {
        if ( is_declare && proc->declared && proc->type != TYPE_CHAR ) compile_error( MSG_PROTO_ERROR ) ;
        proc->type = TYPE_CHAR;
    }

    if ( tcode == identifier_pointer || tcode == identifier_multiply )
    {
        if ( is_declare && proc->declared && proc->type != TYPE_POINTER ) compile_error( MSG_PROTO_ERROR ) ;
        proc->type = TYPE_POINTER ;
    }

    if ( signed_prefix || unsigned_prefix ) compile_error( MSG_INVALID_TYPE );

    /* Parse the process parameters */

    token_next() ;
    if ( token.type != IDENTIFIER || token.code != identifier_leftp ) compile_error( MSG_EXPECTED, "(" ) ;

    token_next() ;

    params = 0 ;
    type = TYPE_INT;
    typeb = TYPE_INT;
    type_implicit = 1;
    ctype = typedef_new( type ) ;
    ctypeb = ctype ;
    signed_prefix = unsigned_prefix = 0;
    external_proc = NULL;

    while ( token.type != IDENTIFIER || token.code != identifier_rightp )
    {
        type = typeb;
        ctype = ctypeb;

        /* Recogo signo del parametro */
        if ( token.type == IDENTIFIER && token.code == identifier_unsigned )
        {
            unsigned_prefix = 1;
            token_next();
        }
        else if ( token.type == IDENTIFIER && token.code == identifier_signed )
        {
            signed_prefix = 1;
            token_next();
        }

        /* Recogo tipo del parametro */
        if ( token.type == IDENTIFIER && token.code == identifier_dword )
        {
            type_implicit = 0;
            type = signed_prefix ? TYPE_INT : TYPE_DWORD;
            unsigned_prefix = signed_prefix = 0;
            ctype = typedef_new( type ) ;
            external_proc = NULL;
            token_next() ;
        }
        else if ( token.type == IDENTIFIER && token.code == identifier_int )
        {
            type_implicit = 0;
            type = unsigned_prefix ? TYPE_DWORD : TYPE_INT;
            unsigned_prefix = signed_prefix = 0;
            ctype = typedef_new( type ) ;
            external_proc = NULL;
            token_next() ;
        }
        else if ( token.type == IDENTIFIER && token.code == identifier_word )
        {
            type_implicit = 0;
            type = signed_prefix ? TYPE_SHORT : TYPE_WORD ;
            unsigned_prefix = signed_prefix = 0;
            ctype = typedef_new( type ) ;
            external_proc = NULL;
            token_next() ;
        }
        else if ( token.type == IDENTIFIER && token.code == identifier_short )
        {
            type_implicit = 0;
            type = unsigned_prefix ? TYPE_WORD : TYPE_SHORT;
            unsigned_prefix = signed_prefix = 0;
            ctype = typedef_new( type ) ;
            external_proc = NULL;
            token_next() ;
        }
        else if ( token.type == IDENTIFIER && token.code == identifier_byte )
        {
            type_implicit = 0;
            type = signed_prefix ? TYPE_SBYTE : TYPE_BYTE ;
            unsigned_prefix = signed_prefix = 0;
            ctype = typedef_new( type ) ;
            external_proc = NULL;
            token_next() ;
        }
        else if ( token.type == IDENTIFIER && token.code == identifier_char )
        {
            type_implicit = 0;
            type = TYPE_CHAR ;
            ctype = typedef_new( type ) ;
            external_proc = NULL;
            token_next() ;
        }
        else if ( token.type == IDENTIFIER && token.code == identifier_string )
        {
            type_implicit = 0;
            type = TYPE_STRING ;
            ctype = typedef_new( type ) ;
            external_proc = NULL;
            token_next() ;
        }
        else if ( token.type == IDENTIFIER && token.code == identifier_float )
        {
            type_implicit = 0;
            type = TYPE_FLOAT ;
            ctype = typedef_new( type ) ;
            external_proc = NULL;
            token_next() ;
        }
        else if ( token.type == IDENTIFIER && segment_by_name( token.code ) )
        {
            type_implicit = 0;
            type = TYPE_STRUCT ;
            ctype = *typedef_by_name( token.code ) ;
            external_proc = NULL;
            token_next() ;
        }
        else if ( !external_proc && ( external_proc = procdef_search( token.code ) ) )    /* Variables tipo proceso, Splinter */
        {
            type_implicit = 0;
            type = TYPE_INT ;
            ctype = typedef_new( type ) ;
            token_next();
        }

        if ( signed_prefix || unsigned_prefix )
        {
            compile_error( MSG_INVALID_TYPE );
            signed_prefix = unsigned_prefix = 0;
        }

        typeb = type;
        ctypeb = ctype;

        while ( token.type == IDENTIFIER && ( token.code == identifier_pointer || token.code == identifier_multiply ) )
        {
            type_implicit = 0;
            type = TYPE_POINTER ;
            ctype = typedef_pointer( ctype ) ;
            token_next() ;
        }

        if ( type == TYPE_STRUCT )
        {
            type_implicit = 0;
            type = TYPE_POINTER ;
            ctype = typedef_pointer( ctype ) ;
        }

        if ( token.type != IDENTIFIER || token.code < reserved_words ) compile_error( MSG_INVALID_PARAM ) ;

        if ( constants_search( token.code ) ) compile_error( MSG_CONSTANT_REDECLARED_AS_VARIABLE ) ;

        /* Check if the process was used before declared */
        if ( !proc->declared )
        {
            var = varspace_search( &local, token.code ) ;
            if ( var )
            {
                /* El parámetro es en realidad un local */
                if ( type_implicit )
                {
                    type = typedef_base( var->type );
                    ctype = var->type;
                }
                if ( typedef_base( var->type ) != type )
                {
                    if ( typedef_is_integer( var->type ) && typedef_is_integer( typedef_new( type ) ) )
                    {
                        /*
                            A parameter was used before the process is declared. The
                            data type declared is different from the data type used,
                            but both are integers. The error is ignored, but no
                            conversion is done. This can lead to type conversion issues.
                        */
                    }
                    else
                        compile_error( MSG_INVALID_PARAMT ) ;
                }
                codeblock_add( &proc->code, MN_LOCAL, var->offset ) ;
                codeblock_add( &proc->code, MN_PRIVATE, proc->pridata->current ) ;
                codeblock_add( &proc->code, MN_PTR, 0 ) ;

                if ( typedef_base( var->type ) == TYPE_STRING ) codeblock_add( &proc->code, MN_LET | MN_STRING, 0 ) ;
                else codeblock_add( &proc->code, MN_LET, 0 ) ;

                codeblock_add( &proc->code, MN_POP, 0 ) ;

                if ( proc->privars->reserved == proc->privars->count ) varspace_alloc( proc->privars, 16 ) ;

                proc->privars->vars[proc->privars->count].type   = typedef_new( TYPE_DWORD );
                proc->privars->vars[proc->privars->count].offset = proc->pridata->current ;
                proc->privars->vars[proc->privars->count].code   = -1 ;

                proc->privars->count++ ;

                segment_add_dword( proc->pridata, 0 ) ;
            }
            else
            {
                var = varspace_search( &global, token.code );
                if ( var )
                {
                    /* El parámetro es en realidad un global */
                    if ( type_implicit )
                    {
                        type = typedef_base( var->type );
                        ctype = var->type;
                    }

                    if ( typedef_base( var->type ) != type ) compile_error( MSG_INVALID_PARAMT ) ;

                    codeblock_add( &proc->code, MN_GLOBAL, var->offset ) ;
                    codeblock_add( &proc->code, MN_PRIVATE, proc->pridata->current ) ;
                    codeblock_add( &proc->code, MN_PTR, 0 ) ;

                    if ( typedef_base( var->type ) == TYPE_STRING ) codeblock_add( &proc->code, MN_LET | MN_STRING, 0 ) ;
                    else codeblock_add( &proc->code, MN_LET, 0 ) ;

                    codeblock_add( &proc->code, MN_POP, 0 ) ;

                    if ( proc->privars->reserved == proc->privars->count ) varspace_alloc( proc->privars, 16 ) ;

                    proc->privars->vars[proc->privars->count].type   = typedef_new( TYPE_DWORD );
                    proc->privars->vars[proc->privars->count].offset = proc->pridata->current ;
                    proc->privars->vars[proc->privars->count].code   = -1 ;

                    proc->privars->count++ ;

                    segment_add_dword( proc->pridata, 0 ) ;
                }
                else
                {
                    /* Crear la variable privada */
                    if ( proc->privars->reserved == proc->privars->count ) varspace_alloc( proc->privars, 16 ) ;

                    if ( type == TYPE_STRING ) varspace_varstring( proc->privars, proc->pridata->current ) ;

                    proc->privars->vars[proc->privars->count].type   = ctype;
                    proc->privars->vars[proc->privars->count].offset = proc->pridata->current ;
                    proc->privars->vars[proc->privars->count].code   = token.code ;
                    if ( external_proc ) proc->privars->vars[proc->privars->count].type.varspace = external_proc->pubvars;

                    proc->privars->count++ ;

                    segment_add_dword( proc->pridata, 0 ) ;
                }
            }
        }
        else
        {
            if ( proc->paramtype[params] != type || proc->paramname[params] != token.code ) compile_error( MSG_PROTO_ERROR );
        }

        if ( proc->params != -1 )
        {
            /* El proceso fue usado previamente */

            if ( proc->paramtype[params] == TYPE_UNDEFINED ) proc->paramtype[params] = type ;
            else if (( proc->paramtype[params] == TYPE_DWORD || proc->paramtype[params] == TYPE_INT ) &&
                     (  type == TYPE_DWORD ||
                        type == TYPE_INT   ||
                        type == TYPE_BYTE  ||
                        type == TYPE_WORD  ||
                        type == TYPE_SHORT ||
                        type == TYPE_SBYTE
                     ) ) proc->paramtype[params] = type ;
            else if ( type == TYPE_DWORD && ( proc->paramtype[params] == TYPE_BYTE || proc->paramtype[params] == TYPE_WORD ) ) proc->paramtype[params] = type ;
            else if ( proc->paramtype[params] != type ) compile_error( MSG_INVALID_PARAMT ) ;
        }
        else proc->paramtype[params] = type;

        proc->paramname[params] = token.code;

        params++ ;

        if ( params == MAX_PARAMS ) compile_error( MSG_TOO_MANY_PARAMS ) ;

        token_next() ;
        if ( token.type == IDENTIFIER && token.code == identifier_comma ) token_next() ;

    } /* END while (token.type != IDENTIFIER || token.code != identifier_rightp) */

    if ( proc->params == -1 ) proc->params = params ;
    else if ( proc->params != params ) compile_error( MSG_INCORRECT_PARAMC, identifier_name( code ), proc->params ) ;

    token_next() ;

    if ( token.type == IDENTIFIER && token.code == identifier_semicolon ) token_next() ;

    /* Compile LOCAL/PRIVATE/PUBLIC sections on process/function.
       NOTE: LOCAL section here considere as PUBLIC section */

    while ( token.type == IDENTIFIER && (
            token.code == identifier_local  ||
            token.code == identifier_public ||
            token.code == identifier_private ) )
    {
        if (( !proc->declared ) && ( token.code == identifier_local || token.code == identifier_public ) )
        {
            /* (2006/11/19 19:34 GMT-03:00, Splinter - jj_arg@yahoo.com) */
            /* Ahora las declaraciones locales, son solo locales al proceso, pero visibles desde todo proceso */
            /* Se permite declarar local/publica una variable que haya sido declarada global, es una variable propia, no es la global */
            VARSPACE * v[] = {&local, proc->privars, NULL};
            compile_varspace( proc->pubvars, proc->pubdata, 1, 1, 0, v, DEFAULT_ALIGNMENT, 0 ) ;
        }
        else if ( token.code == identifier_private )
        {
            /* (2006/11/19 19:34 GMT-03:00, Splinter - jj_arg@yahoo.com) */
            /* Se permite declarar privada una variable que haya sido declarada global, es una variable propia, no es la global */
            VARSPACE * v[] = {&local, proc->pubvars, NULL};
            compile_varspace( proc->privars, proc->pridata, 1, 1, 0, v, DEFAULT_ALIGNMENT, 0 ) ;
        }

        token_next() ;
    }

    /* Gestiona procesos cuyos parámetros son variables locales */

    if ( !is_declare )
    {
        if ( token.type != IDENTIFIER || token.code != identifier_begin ) compile_error( MSG_NO_BEGIN ) ;

        compile_block( proc ) ;

        if ( token.type == IDENTIFIER && token.code == identifier_else ) compile_error( MSG_ELSE_WOUT_IF ) ;
    }

    if ( token.type != IDENTIFIER || token.code != identifier_end ) compile_error( MSG_NO_END ) ;

    if ( !is_declare ) codeblock_add( &proc->code, MN_END, 0 ) ;

    proc->declared = 1 ;

}

/* ---------------------------------------------------------------------- */

void compile_program()
{
    /* Ahora lo del program es opcional :-P */

    token_next() ;
    if ( token.type == IDENTIFIER && token.code == identifier_program )
    {
        token_next() ;
        if ( token.type != IDENTIFIER || token.code < reserved_words ) compile_error( MSG_PROGRAM_NAME_EXP ) ;

        token_next() ;
        if ( token.type != IDENTIFIER || token.code != identifier_semicolon ) compile_error( MSG_EXPECTED, ";" ) ;
    }
    else token_back() ;

    for ( ;; )
    {
        token_next() ;

        while ( token.type == IDENTIFIER && token.code == identifier_semicolon ) token_next() ;

        if ( token.type == IDENTIFIER && token.code == identifier_import ) compile_import() ;
        else if ( token.type == IDENTIFIER && token.code == identifier_const ) compile_constants() ;
        else if ( token.type == IDENTIFIER && token.code == identifier_local )
        {
            /* (2006/11/19 19:34 GMT-03:00, Splinter - jj_arg@yahoo.com) */
            VARSPACE * v[] = { &global, NULL };
            compile_varspace( &local, localdata, 1, 1, 0, v, DEFAULT_ALIGNMENT, 0 ) ;
        }
        else if ( token.type == IDENTIFIER && token.code == identifier_global )
        {
            /* (2006/11/19 19:34 GMT-03:00, Splinter - jj_arg@yahoo.com) */
            VARSPACE * v[] = { &local, NULL };
            compile_varspace( &global, globaldata, 1, 1, 0, v, DEFAULT_ALIGNMENT, 0 ) ;
        }
        else if ( token.type == IDENTIFIER && token.code == identifier_private )
        {
            /* (2006/11/19 19:34 GMT-03:00, Splinter - jj_arg@yahoo.com) */
            VARSPACE * v[] = { &local, &global, NULL };
            compile_varspace( mainproc->privars, mainproc->pridata, 1, 1, 0, v, DEFAULT_ALIGNMENT, 0 ) ;
        }
        else if ( token.type == IDENTIFIER && token.code == identifier_begin )
        {
            if ( mainproc->defined )
            {
                /* Hack para poder redefinir el proceso principal */
                mainproc->code.current -= 1 ;
            }

            mainproc->defined = 1 ;

            compile_block( mainproc ) ;

            if ( token.type == IDENTIFIER && token.code == identifier_else ) compile_error( MSG_ELSE_WOUT_IF ) ;

            if ( token.type != IDENTIFIER || token.code != identifier_end ) compile_error( MSG_NO_END ) ;

            codeblock_add( &mainproc->code, MN_END, 0 ) ;
        }
        else if ( token.type == IDENTIFIER && token.code == identifier_type ) compile_type(); /* Tipo de dato definido por el usuario */
        else if ( token.type == IDENTIFIER &&
                  (
                    token.code == identifier_process  ||
                    token.code == identifier_function ||
                    token.code == identifier_declare  ||
                    identifier_is_basic_type( token.type )
                  ) ) compile_process() ; /* Definición de proceso */
        else if ( segment_by_name( token.code ) ) compile_process() ;
        else break ;
    }

    if ( global.count && debug )
    {
        printf( "\n---- Global variables\n\n" ) ;
        varspace_dump( &global, 0 ) ;
    }

    if ( local.count && debug )
    {
        printf( "\n---- Local variables\n\n" ) ;
        varspace_dump( &local, 0 ) ;
        /* segment_dump (localdata) ; */
    }

    if ( token.type != NOTOKEN ) compile_error( MSG_UNEXPECTED_TOKEN ) ;

    program_postprocess() ;

    if ( debug ) program_dumpprocesses() ;

    if ( !libmode && !mainproc->defined ) compile_error( MSG_NO_MAIN ) ;

    if ( debug )
    {
        identifier_dump() ;
        string_dump( NULL ) ;
    }
}

/* ---------------------------------------------------------------------- */
