/*
 *  Copyright (c) 2011-2013 Joseba García Echebarria. All rights reserved.
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

#include <stdio.h>
#include <bgddl.h>
#include <xstrings.h>
#include <bgdrtm.h>
#include <curl/curl.h>
#include <SDL.h>

#ifndef MAX_DOWNLOADS
#define MAX_DOWNLOADS 16
#endif

/* --------------------------------------------------------------------------- */

// Data used when downloading to memory
struct MemoryStruct {
    char   *memory;
    int    *strid;
    size_t  size;
};

// Structure with the curl action data
typedef struct
{
    CURL *curl;
    struct curl_httppost *formpost;
    struct curl_httppost *lastptr;
    struct MemoryStruct chunk;  // Used when downloading to a string
    FILE *outfd;                // Used when downloading to a file
    int used;
} curl_info ;

// Struct needed to launch the curl action in a separate thread
typedef struct
{
    int  *id;
    int  *status;
    int ( *fn )();
} bgdata ;

curl_info download_info[MAX_DOWNLOADS];

/* --------------------------------------------------------------------------- */
DLCONSTANT  __bgdexport( mod_curl, constants_def )[] = {
    { "CURLOPT_VERBOSE"            , TYPE_DWORD    , CURLOPT_VERBOSE               },
    { "CURLOPT_HEADER"            , TYPE_DWORD    , CURLOPT_HEADER               },
    { "CURLOPT_NOPROGRESS"            , TYPE_DWORD    , CURLOPT_NOPROGRESS               },
    { "CURLOPT_NOSIGNAL"            , TYPE_DWORD    , CURLOPT_NOSIGNAL               },
    { "CURLOPT_WILDCARDMATCH"            , TYPE_DWORD    , CURLOPT_WILDCARDMATCH               },
    { "CURLOPT_WRITEDATA"            , TYPE_DWORD    , CURLOPT_WRITEDATA               },
    { "CURLOPT_READDATA"            , TYPE_DWORD    , CURLOPT_READDATA               },
    { "CURLOPT_WRITEHEADER"            , TYPE_DWORD    , CURLOPT_WRITEHEADER               },
    { "CURLOPT_FAILONERROR"            , TYPE_DWORD    , CURLOPT_FAILONERROR               },
    { "CURLOPT_URL"            , TYPE_DWORD    , CURLOPT_URL               },
    { "CURLOPT_PROTOCOLS"            , TYPE_DWORD    , CURLOPT_PROTOCOLS               },
    { "CURLOPT_REDIR_PROTOCOLS"            , TYPE_DWORD    , CURLOPT_REDIR_PROTOCOLS               },
    { "CURLOPT_PROXY"            , TYPE_DWORD    , CURLOPT_PROXY               },
    { "CURLOPT_PROXYPORT"            , TYPE_DWORD    , CURLOPT_PROXYPORT               },
    { "CURLOPT_PROXYTYPE"            , TYPE_DWORD    , CURLOPT_PROXYTYPE               },
    { "CURLOPT_NOPROXY"            , TYPE_DWORD    , CURLOPT_NOPROXY               },
    { "CURLOPT_HTTPPROXYTUNNEL"            , TYPE_DWORD    , CURLOPT_HTTPPROXYTUNNEL               },
    { "CURLOPT_SOCKS5_GSSAPI_SERVICE"            , TYPE_DWORD    , CURLOPT_SOCKS5_GSSAPI_SERVICE               },
    { "CURLOPT_SOCKS5_GSSAPI_NEC"            , TYPE_DWORD    , CURLOPT_SOCKS5_GSSAPI_NEC               },
    { "CURLOPT_INTERFACE"            , TYPE_DWORD    , CURLOPT_INTERFACE               },
    { "CURLOPT_LOCALPORT"            , TYPE_DWORD    , CURLOPT_LOCALPORT               },
    { "CURLOPT_LOCALPORTRANGE"            , TYPE_DWORD    , CURLOPT_LOCALPORTRANGE               },
    { "CURLOPT_DNS_CACHE_TIMEOUT"            , TYPE_DWORD    , CURLOPT_DNS_CACHE_TIMEOUT               },
    { "CURLOPT_DNS_USE_GLOBAL_CACHE"            , TYPE_DWORD    , CURLOPT_DNS_USE_GLOBAL_CACHE               },
    { "CURLOPT_BUFFERSIZE"            , TYPE_DWORD    , CURLOPT_BUFFERSIZE               },
    { "CURLOPT_PORT"            , TYPE_DWORD    , CURLOPT_PORT               },
    { "CURLOPT_TCP_NODELAY"            , TYPE_DWORD    , CURLOPT_TCP_NODELAY               },
    { "CURLOPT_ADDRESS_SCOPE"            , TYPE_DWORD    , CURLOPT_ADDRESS_SCOPE               },
    { "CURLOPT_NETRC"            , TYPE_DWORD    , CURLOPT_NETRC               },
    { "CURL_NETRC_OPTIONAL"            , TYPE_DWORD    , CURL_NETRC_OPTIONAL               },
    { "CURL_NETRC_IGNORED"            , TYPE_DWORD    , CURL_NETRC_IGNORED               },
    { "CURL_NETRC_REQUIRED"            , TYPE_DWORD    , CURL_NETRC_REQUIRED               },
    { "CURLOPT_NETRC_FILE"            , TYPE_DWORD    , CURLOPT_NETRC_FILE               },
    { "CURLOPT_USERPWD"            , TYPE_DWORD    , CURLOPT_USERPWD               },
    { "CURLOPT_PROXYUSERPWD"            , TYPE_DWORD    , CURLOPT_PROXYUSERPWD               },
    { "CURLOPT_USERNAME"            , TYPE_DWORD    , CURLOPT_USERNAME               },
    { "CURLOPT_PASSWORD"            , TYPE_DWORD    , CURLOPT_PASSWORD               },
    { "CURLOPT_PROXYUSERNAME"            , TYPE_DWORD    , CURLOPT_PROXYUSERNAME               },
    { "CURLOPT_PROXYPASSWORD"            , TYPE_DWORD    , CURLOPT_PROXYPASSWORD               },
    { "CURLOPT_HTTPAUTH"            , TYPE_DWORD    , CURLOPT_HTTPAUTH               },
    { "CURLAUTH_BASIC"            , TYPE_DWORD    , CURLAUTH_BASIC               },
    { "CURLAUTH_DIGEST"            , TYPE_DWORD    , CURLAUTH_DIGEST               },
    { "CURLAUTH_DIGEST_IE"            , TYPE_DWORD    , CURLAUTH_DIGEST_IE               },
    { "CURLAUTH_GSSNEGOTIATE"            , TYPE_DWORD    , CURLAUTH_GSSNEGOTIATE               },
    { "CURLAUTH_NTLM"            , TYPE_DWORD    , CURLAUTH_NTLM               },
    { "CURLAUTH_ANY"            , TYPE_DWORD    , CURLAUTH_ANY               },
    { "CURLAUTH_ANYSAFE"            , TYPE_DWORD    , CURLAUTH_ANYSAFE               },
    { "CURLAUTH_ONLY"            , TYPE_DWORD    , CURLAUTH_ONLY               },
    { "CURLOPT_TLSAUTH_TYPE"            , TYPE_DWORD    , CURLOPT_TLSAUTH_TYPE               },
    { "CURLOPT_TLSAUTH_USERNAME"            , TYPE_DWORD    , CURLOPT_TLSAUTH_USERNAME               },
    { "CURLOPT_TLSAUTH_PASSWORD"            , TYPE_DWORD    , CURLOPT_TLSAUTH_PASSWORD               },
    { "CURLOPT_PROXYAUTH"            , TYPE_DWORD    , CURLOPT_PROXYAUTH               },
    { "CURLOPT_AUTOREFERER"            , TYPE_DWORD    , CURLOPT_AUTOREFERER               },
    { "CURLOPT_FOLLOWLOCATION"            , TYPE_DWORD    , CURLOPT_FOLLOWLOCATION               },
    { "CURLOPT_UNRESTRICTED_AUTH"            , TYPE_DWORD    , CURLOPT_UNRESTRICTED_AUTH               },
    { "CURLOPT_MAXREDIRS"            , TYPE_DWORD    , CURLOPT_MAXREDIRS               },
    { "CURLOPT_POSTREDIR"            , TYPE_DWORD    , CURLOPT_POSTREDIR               },
    { "CURLOPT_PUT"            , TYPE_DWORD    , CURLOPT_PUT               },
    { "CURLOPT_POST"            , TYPE_DWORD    , CURLOPT_POST               },
    { "CURLOPT_POSTFIELDS"            , TYPE_DWORD    , CURLOPT_POSTFIELDS               },
    { "CURLOPT_COPYPOSTFIELDS"            , TYPE_DWORD    , CURLOPT_COPYPOSTFIELDS               },
    { "CURLOPT_HTTPPOST"            , TYPE_DWORD    , CURLOPT_HTTPPOST               },
    { "CURLOPT_REFERER"            , TYPE_DWORD    , CURLOPT_REFERER               },
    { "CURLOPT_USERAGENT"            , TYPE_DWORD    , CURLOPT_USERAGENT               },
    { "CURLOPT_HTTPHEADER"            , TYPE_DWORD    , CURLOPT_HTTPHEADER               },
    { "CURLOPT_COOKIE"            , TYPE_DWORD    , CURLOPT_COOKIE               },
    { "CURLOPT_COOKIEFILE"            , TYPE_DWORD    , CURLOPT_COOKIEFILE               },
    { "CURLOPT_COOKIEJAR"            , TYPE_DWORD    , CURLOPT_COOKIEJAR               },
    { "CURLOPT_COOKIESESSION"            , TYPE_DWORD    , CURLOPT_COOKIESESSION               },
    { "CURLOPT_COOKIELIST"            , TYPE_DWORD    , CURLOPT_COOKIELIST               },
    { "CURLOPT_HTTPGET"            , TYPE_DWORD    , CURLOPT_HTTPGET               },
    { "CURLOPT_HTTP_VERSION"            , TYPE_DWORD    , CURLOPT_HTTP_VERSION               },
    { "CURL_HTTP_VERSION_NONE"            , TYPE_DWORD    , CURL_HTTP_VERSION_NONE               },
    { "CURL_HTTP_VERSION_1_0"            , TYPE_DWORD    , CURL_HTTP_VERSION_1_0               },
    { "CURL_HTTP_VERSION_1_1"            , TYPE_DWORD    , CURL_HTTP_VERSION_1_1               },
    { "CURLOPT_IGNORE_CONTENT_LENGTH"            , TYPE_DWORD    , CURLOPT_IGNORE_CONTENT_LENGTH               },
    { "CURLOPT_HTTP_CONTENT_DECODING"            , TYPE_DWORD    , CURLOPT_HTTP_CONTENT_DECODING               },
    { "CURLOPT_HTTP_TRANSFER_DECODING"            , TYPE_DWORD    , CURLOPT_HTTP_TRANSFER_DECODING               }
};
/* --------------------------------------------------------------------------- */

// Find the first free curl_info index, return numeric index
int findindex() {
    int i=0;

    for (i=0; i<MAX_DOWNLOADS; i++) {
        if (download_info[i].used == 0) {
            // Initialize values, just in case they haven't been already
            download_info[i].formpost   = NULL;
            download_info[i].lastptr    = NULL;
            download_info[i].outfd      = NULL;
            download_info[i].chunk.size = 0;
            download_info[i].used       = 1;
            return i;
        }
    }

    return -1;
}

static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    mem->memory = realloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory == NULL) {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        exit(EXIT_FAILURE);
    }

    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}


/* --------------------------------------------------------------------------- */

/**
 * PREP
 * Helper function preparing params
 **/

static bgdata *prep( int *params )
{
    bgdata *t = ( bgdata* )malloc( sizeof( bgdata ) );

    t->id     = ( int* )params[0];
    t->status = ( int* )params[1];

    return t;
}

/* --------------------------------------------------------------------------- */
/**
 * bgDoLoad
 * Helper function executed in the new thread
 **/

static int bgDoLoad( void *d )
{
    bgdata *t  = ( bgdata* )d;
    *( t->status ) = -2 ;
    *( t->status ) = ( *t->fn )( t->id );
    free( t );
    return 0;
}

// Maps curl_formadd
static int bgd_curl_formadd(INSTANCE * my, int * params) {
    int retval = 0;

    if(params[0] == -1 || params[0] > MAX_DOWNLOADS)
        return -1;

    // Actually perform curl_formadd
    retval = curl_formadd(&download_info[params[0]].formpost,
                 &download_info[params[0]].lastptr,
                 params[1], string_get(params[2]),
                 params[3], string_get(params[4]),
                 CURLFORM_END);

    string_discard(params[2]);
    string_discard(params[4]);

    return retval;
}

// Maps curl_formfree
static int bgd_curl_formfree(INSTANCE * my, int * params) {
    if(params[0] == -1 || params[0] > MAX_DOWNLOADS)
        return -1;

    // Actually perform curl_formfree
    curl_formfree(download_info[params[0]].formpost);

    return 0;
}

// Maps curl_easy_init
static int bgd_curl_easy_init(INSTANCE * my, int * params) {
    int i;

    // Get the index of the connection
    if( (i = findindex()) == -1)
        return -1;

    download_info[i].curl = curl_easy_init();

    if (download_info[i].curl == NULL)
        return -1;

    return i;
}

// Maps curl_easy_cleanup
static int bgd_curl_easy_cleanup(INSTANCE * my, int * params) {
    if(params[0] == -1 || params[0] > MAX_DOWNLOADS)
        return -1;

    download_info[params[0]].used = 0;

    curl_easy_cleanup(download_info[params[0]].curl);

    return 0;
}

// Maps curl_easy_setopt for options which require an integer
static int bgd_curl_easy_setopt(INSTANCE * my, int * params) {
    if(params[0] == -1 || params[0] > MAX_DOWNLOADS)
        return -1;

    CURLcode retval;

    // Actually perform curl_easy_setopt
    switch (params[1]) {
        case CURLOPT_HTTPPOST:
            retval = curl_easy_setopt(download_info[params[0]].curl,
                                      CURLOPT_HTTPPOST,
                                      download_info[params[0]].formpost);
            break;

        default:
            retval = curl_easy_setopt(download_info[params[0]].curl,
                                      params[1],
                                      params[2]);
            break;
    }

    return (int)retval;
}

// Maps curl_easy_setopt for options which require a string
static int bgd_curl_easy_setopt2(INSTANCE * my, int * params) {
    if(params[0] == -1 || params[0] > MAX_DOWNLOADS)
        return -1;

    CURLcode retval;

    // Actually perform curl_easy_setopt
    switch (params[1]) {
            // Handle some special cases
        case CURLOPT_WRITEDATA:
            // Point the output file pointer to the given location
            download_info[params[0]].outfd = fopen(string_get(params[2]), "wb");
            string_discard(params[2]);
            if(download_info[params[0]].outfd == NULL)
                return -1;

            retval = curl_easy_setopt(download_info[params[0]].curl,
                                      CURLOPT_WRITEDATA,
                                      download_info[params[0]].outfd);
            break;

        default:
            retval = curl_easy_setopt(download_info[params[0]].curl,
                                      params[1],
                                      string_get(params[2]));
            string_discard(params[2]);
            break;
    }

    return (int)retval;
}

// Maps curl_easy_setopt when downloading data to a string directly
static int bgd_curl_easy_setopt3(INSTANCE * my, int * params) {
    if(params[0] == -1 || params[0] > MAX_DOWNLOADS)
        return -1;

    CURLcode retval;

    // Actually perform curl_easy_setopt
    switch (params[1]) {
        // When called with an integer, download to a string
        case CURLOPT_WRITEDATA:
            // Initialization
            download_info[params[0]].chunk.memory = malloc(1);

            // Set writefunction and writedata to the appropriate values
            curl_easy_setopt(download_info[params[0]].curl,
                             CURLOPT_WRITEFUNCTION,
                             WriteMemoryCallback);
            download_info[params[0]].chunk.strid = (int *)params[2];
            retval = curl_easy_setopt(download_info[params[0]].curl,
                                      CURLOPT_WRITEDATA,
                                      (void *)&(download_info[params[0]].chunk) );
            break;

        default:
            retval = -1;
            break;
    }

    return (int)retval;
}

// Actual perform function
int curl_perform(int id) {
    if(download_info[id].curl == NULL)
        return -1;

    int retval = 0;

    // Perform download, this function won't quit until it's done
    retval = curl_easy_perform(download_info[id].curl);

    // If downloading to a file, close its file descriptor
    if (download_info[id].outfd != NULL) {
        fclose(download_info[id].outfd);
    } else if (download_info[id].chunk.size > 0) {
        // Create the string for the user
        // printf("Output from CURL:\n%s\n", download_info[id].chunk.memory);
        *(download_info[id].chunk.strid) = string_new(download_info[id].chunk.memory);
        string_use( *(download_info[id].chunk.strid) );

        // Free used memory
        free(download_info[id].chunk.memory);
    }

    return retval;
}

// Map curl_easy_perform
static int bgd_curl_easy_perform(INSTANCE * my, int * params) {
    bgdata *t = prep( params );
    t->fn = curl_perform;

    SDL_CreateThread( bgDoLoad, "CURL perform thread", (void *)t );

    return 0;
}

// Initialize libcurl
void __bgdexport( mod_curl, module_initialize )() {
    curl_global_init(CURL_GLOBAL_ALL);
}

// Finalize libcurl
void __bgdexport( mod_curl, module_finalize )() {
    curl_global_cleanup();
}

DLSYSFUNCS __bgdexport( mod_curl, functions_exports )[] =
{
    { "CURL_INIT"           , ""      , TYPE_INT    , bgd_curl_easy_init      },
    { "CURL_CLEANUP"        , "I"     , TYPE_INT    , bgd_curl_easy_cleanup   },
    { "CURL_FORMADD"        , "IISIS" , TYPE_INT    , bgd_curl_formadd        },
    { "CURL_FORMFREE"       , "I"     , TYPE_INT    , bgd_curl_formfree       },
    { "CURL_SETOPT"         , "III"   , TYPE_INT    , bgd_curl_easy_setopt    },
    { "CURL_SETOPT"         , "IIS"   , TYPE_INT    , bgd_curl_easy_setopt2   },
    { "CURL_SETOPT"         , "IIP"   , TYPE_INT    , bgd_curl_easy_setopt3   },
    { "CURL_PERFORM"        , "IP"    , TYPE_INT    , bgd_curl_easy_perform   },
//    { "CURL_FETCH"          , "I"     , TYPE_STRING , bgd_curl_fetch          },
    { 0                     , 0       , 0           , 0                       }
};
