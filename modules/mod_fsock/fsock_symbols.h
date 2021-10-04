/*
 * fsock library (aka tcpsock) by Titonus & SplinterGU
 */

/* ---------------------------------------------------------- */

#ifndef __FSOCK_SYMBOLS_H
#define __FSOCK_SYMBOLS_H

#include <bgddl.h>

#ifdef __BGDC__
DLSYSFUNCS  __bgdexport( fsock, functions_exports )[] =
{
    { "FSOCK_INIT"                , "I"      , TYPE_DWORD , 0 },
    { "FSOCK_GETERROR"            , ""       , TYPE_DWORD , 0 },
    { "FSOCK_GETFDSETSIZE"        , ""       , TYPE_DWORD , 0 },
    { "TCPSOCK_OPEN"              , ""       , TYPE_DWORD , 0 },
    { "UDPSOCK_OPEN"              , ""       , TYPE_DWORD , 0 },
    { "FSOCK_SETBLOCK"            , "II"     , TYPE_DWORD , 0 },
    { "FSOCK_CLOSE"               , "I"      , TYPE_DWORD , 0 },
    { "FSOCK_BIND"                , "II"     , TYPE_DWORD , 0 },
    { "TCPSOCK_LISTEN"            , "II"     , TYPE_DWORD , 0 },
    { "TCPSOCK_ACCEPT"            , "IPP"    , TYPE_DWORD , 0 },
    { "TCPSOCK_CONNECT"           , "ISI"    , TYPE_DWORD , 0 },
    { "FSOCK_SELECT"              , "IIII"   , TYPE_DWORD , 0 },
    { "TCPSOCK_SEND"              , "IPI"    , TYPE_DWORD , 0 },
    { "UDPSOCK_SEND"              , "IPISI"  , TYPE_DWORD , 0 },
    { "TCPSOCK_RECV"              , "IPI"    , TYPE_DWORD , 0 },
    { "UDPSOCK_RECV"              , "IPIPP"  , TYPE_DWORD , 0 },
    { "FSOCK_SOCKETSET_CHECK"     , "IIII"   , TYPE_DWORD , 0 },
    { "FSOCK_SOCKETSET_FREE"      , "I"      , TYPE_DWORD , 0 },
    { "FSOCK_SOCKETSET_ADD"       , "II"     , TYPE_DWORD , 0 },
    { "FSOCK_SOCKETSET_DEL"       , "II"     , TYPE_DWORD , 0 },
    { "FSOCK_GET_IPHOST"          , "I"      , TYPE_STRING, 0 },
    { "FSOCK_GET_IPSTR"           , "P"      , TYPE_STRING, 0 },
    { "FSOCK_QUIT"                , ""       , TYPE_DWORD , 0 },
    { "FSOCK_FDZERO"              , "I"      , TYPE_DWORD , 0 },
    { "FSOCK_FDSET"               , "II"     , TYPE_DWORD , 0 },
    { "FSOCK_FDCLR"               , "II"     , TYPE_DWORD , 0 },
    { "FSOCK_FDISSET"             , "II"     , TYPE_DWORD , 0 },
    { 0                     , 0      , 0        , 0 }
};
#else
extern DLSYSFUNCS  __bgdexport( fsock, functions_exports )[];
#endif

#endif
