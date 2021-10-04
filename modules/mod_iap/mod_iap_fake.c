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

/* --------------------------------------------------------------------------- */

// Calls the initialize function in the Java IAP code
static int bgd_iap_init(INSTANCE * my, int * params) {
    return 0;
}

// Check if the receipts are already available
static int bgd_iap_receipts_ready(INSTANCE * my, int * params) {
    return 0;
}

// Shutdown the IAP code (ouyaFacade)
static void bgd_iap_shutdown() {
}

static int bgd_iap_purchased(INSTANCE * my, int * params) {
    return 0;
}

static int bgd_iap_isouya(INSTANCE * my, int * params) {
    return 0;
}

static int bgd_iap_purchase(INSTANCE * my, int * params) {
    return 0;
}

// Finalize iap
void __bgdexport( mod_iap, module_finalize )() {
}

DLSYSFUNCS __bgdexport( mod_iap, functions_exports )[] =
{
    { "IAP_INIT"            , "S"     , TYPE_INT       , bgd_iap_init           },
    { "IAP_RECEIPTS_READY"  , ""      , TYPE_INT       , bgd_iap_receipts_ready },
    { "IAP_SHUTDOWN"        , ""      , TYPE_UNDEFINED , bgd_iap_shutdown       },
    { "IAP_PURCHASED"       , "S"     , TYPE_INT       , bgd_iap_purchased      },
    { "IAP_ISOUYA"          , ""      , TYPE_INT       , bgd_iap_isouya         },
    { "IAP_PURCHASE"        , "SP"    , TYPE_INT       , bgd_iap_purchase       },
    { 0                     , 0       , 0           , 0                         }
};
