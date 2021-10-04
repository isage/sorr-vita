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

#ifndef __MODCURL_SYMBOLS_H
#define __MODCURL_SYMBOLS_H

#include <curl/curl.h>
#include <bgddl.h>

#ifdef __BGDC__
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

DLSYSFUNCS __bgdexport( mod_curl, functions_exports )[] =
{
    { "CURL_INIT"           , ""      , TYPE_INT    , bgd_curl_easy_init      },
    { "CURL_CLEANUP"        , "I"     , TYPE_INT    , bgd_curl_easy_cleanup   },
    { "CURL_FORMADD"        , "IISIS" , TYPE_INT    , bgd_curl_formadd        },
    { "CURL_FORMFREE"       , "I"     , TYPE_INT    , bgd_curl_formfree       },
    { "CURL_SETOPT"         , "III"   , TYPE_INT    , bgd_curl_easy_setopt    },
    { "CURL_SETOPT"         , "IIS"   , TYPE_INT    , bgd_curl_easy_setopt2   },
    { "CURL_PERFORM"        , "IP"    , TYPE_INT    , bgd_curl_easy_perform   },
    { "CURL_FETCH"          , "I"     , TYPE_STRING , bgd_curl_fetch          },
    { 0                     , 0       , 0           , 0                       }
};
#else
extern DLCONSTANT  __bgdexport( mod_curl, constants_def )[];
extern DLSYSFUNCS __bgdexport( mod_curl, functions_exports )[];
extern void __bgdexport( mod_curl, module_initialize )();
extern void __bgdexport( mod_curl, module_finalize )();
#endif

#endif
