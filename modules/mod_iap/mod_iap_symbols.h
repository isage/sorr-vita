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

#ifndef __MODIAP_SYMBOLS_H
#define __MODIAP_SYMBOLS_H

#include <bgddl.h>

#ifdef __BGDC__
DLSYSFUNCS __bgdexport( mod_iap, functions_exports )[] =
{
    { "IAP_INIT"            , "S"     , TYPE_INT       , 0 },
    { "IAP_RECEIPTS_READY"  , ""      , TYPE_INT       , 0 },
    { "IAP_SHUTDOWN"        , ""      , TYPE_UNDEFINED , 0 },
    { "IAP_PURCHASED"       , "S"     , TYPE_INT       , 0 },
    { "IAP_ISOUYA"          , ""      , TYPE_INT       , 0 },
    { "IAP_PURCHASE"        , "SP"    , TYPE_INT       , 0 },
    { 0                     , 0       , 0              , 0 }
};
#else
extern DLSYSFUNCS __bgdexport( mod_iap, functions_exports )[];
extern void __bgdexport( mod_iap, module_finalize )();
#endif

#endif
