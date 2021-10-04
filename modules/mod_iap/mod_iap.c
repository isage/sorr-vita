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
#include <jni.h>
#include <pthread.h>
#include <android/log.h>

/* --------------------------------------------------------------------------- */

static JavaVM* mJavaVM;
static jclass mActivityClass;
static int *purchase_status;

JNIEnv* BGD_JNI_GetEnv(void) {
    /* From http://developer.android.com/guide/practices/jni.html
     * All threads are Linux threads, scheduled by the kernel.
     * They're usually started from managed code (using Thread.start), but they can also be created elsewhere and then
     * attached to the JavaVM. For example, a thread started with pthread_create can be attached with the
     * JNI AttachCurrentThread or AttachCurrentThreadAsDaemon functions. Until a thread is attached, it has no JNIEnv,
     * and cannot make JNI calls.
     * Attaching a natively-created thread causes a java.lang.Thread object to be constructed and added to the "main"
     * ThreadGroup, making it visible to the debugger. Calling AttachCurrentThread on an already-attached thread
     * is a no-op.
     * Note: You can call this function any number of times for the same thread, there's no harm in it
     */
    
    JNIEnv *env;
    int status = (*mJavaVM)->AttachCurrentThread(mJavaVM, &env, NULL);
    if(status < 0) {
        __android_log_print(ANDROID_LOG_ERROR, "IAP", "failed to attach current thread");
        return 0;
    }
    
    return env;
}

// Calls the initialize function in the Java IAP code
static int bgd_iap_init(INSTANCE * my, int * params) {
    jmethodID mid;
    JNIEnv *mEnv = BGD_JNI_GetEnv();
    mid = (*mEnv)->GetStaticMethodID(mEnv, mActivityClass, "init", "(Ljava/lang/String;)I");
    if (mid) {
        jstring devID = (*mEnv)->NewStringUTF(mEnv,string_get(params[0]));
        string_discard(params[0]);
        (*mEnv)->CallStaticIntMethod(mEnv, mActivityClass, mid, devID);
        (*mEnv)->DeleteLocalRef(mEnv, devID);
    }
    
    return 0;
}

// Check if the receipts are already available
static int bgd_iap_receipts_ready(INSTANCE * my, int * params) {
    jmethodID mid;
    
    JNIEnv *mEnv = BGD_JNI_GetEnv();
    mid = (*mEnv)->GetStaticMethodID(mEnv, mActivityClass, "receiptsReady","()Z");
    if(mid) {
        jboolean success = (*mEnv)->CallStaticIntMethod(mEnv, mActivityClass, mid);
        return success ? 1 : 0;
    }
    
    return 0;
}

// Shutdown the IAP code (ouyaFacade)
static void bgd_iap_shutdown() {
    jmethodID mid;
    
    JNIEnv *mEnv = BGD_JNI_GetEnv();
    mid = (*mEnv)->GetStaticMethodID(mEnv, mActivityClass, "shutdown","()V");
    if(mid) {
        (*mEnv)->CallStaticIntMethod(mEnv, mActivityClass, mid);
    }
}

// Try to determine if player has already purchased given product ID
static int bgd_iap_purchased(INSTANCE * my, int * params) {
    jmethodID mid;
    
    JNIEnv *mEnv = BGD_JNI_GetEnv();
    mid = (*mEnv)->GetStaticMethodID(mEnv, mActivityClass, "isPurchased", "(Ljava/lang/String;)Z");
    if(mid) {
        jstring prodID = (*mEnv)->NewStringUTF(mEnv,string_get(params[0]));
        string_discard(params[0]);
        jboolean success = (*mEnv)->CallStaticBooleanMethod(mEnv, mActivityClass, mid, prodID);
        (*mEnv)->DeleteLocalRef(mEnv, prodID);
        
        return success ? 1 : 0;
    }
    
    return 0;
}

// Is the game being run in a real Ouya?
static int bgd_iap_isouya(INSTANCE * my, int * params) {
    jmethodID mid;
    
    JNIEnv *mEnv = BGD_JNI_GetEnv();
    mid = (*mEnv)->GetStaticMethodID(mEnv, mActivityClass, "isOuya","()Z");
    if(mid) {
        jboolean success = (*mEnv)->CallStaticBooleanMethod(mEnv, mActivityClass, mid);
        return success ? 1 : 0;
    }
    
    return 0;
}

// Start the purchase for the given product ID
static int bgd_iap_purchase(INSTANCE * my, int * params) {
    jmethodID mid;

    // Mark the purchase as "In-progress"
    purchase_status = ( int* )params[1];
    *( purchase_status ) = -2;
    
    JNIEnv *mEnv = BGD_JNI_GetEnv();
    mid = (*mEnv)->GetStaticMethodID(mEnv, mActivityClass, "purchaseProduct", "(Ljava/lang/String;)V");
    if(mid) {
        jstring prodID = (*mEnv)->NewStringUTF(mEnv,string_get(params[0]));
        string_discard(params[0]);
        (*mEnv)->CallStaticVoidMethod(mEnv, mActivityClass, mid, prodID);
        (*mEnv)->DeleteLocalRef(mEnv, prodID);
    }
    
    return 0;
}

// Called when the Java code is first started, so that we can store the
// global reference for the class
void Java_org_bennugd_iap_iap_setJNI(JNIEnv* env, jclass cls) {
    if((*env)->GetJavaVM(env, &mJavaVM) < 0) {
        __android_log_print(ANDROID_LOG_ERROR, "IAP", "Couldn't get JavaVM* things will fail! :(");
    }
    
    mActivityClass = (jclass)((*env)->NewGlobalRef(env, cls));
}

// Called when the Java code is first started, so that we can store the
// global reference for the class
void Java_org_bennugd_iap_iap_updatePurchaseStatus(JNIEnv* env, jclass cls, jint value) {
    int val = value;
    *( purchase_status ) = value;
}

// Finalize iap
void __bgdexport( mod_iap, module_finalize )() {
    bgd_iap_shutdown();
    
    JNIEnv *mEnv = BGD_JNI_GetEnv();
    (*mEnv)->DeleteGlobalRef(mEnv, mActivityClass);
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
