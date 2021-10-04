/* ---------------------------------------------------------------------- */
// PSP specific stuff
// mostly based on ScummVM PSP port
/* ---------------------------------------------------------------------- */
#ifdef TARGET_PSP
	#define USERSPACE_ONLY		//don't use kernel mode features

	#include <psptypes.h>
	#include <pspkernel.h>
	#include <stdarg.h>
	#include <pspdebug.h>

/**
 * Define the module info section
 *
 * 2nd arg must 0x1000 so __init is executed in
 * kernelmode for our loaderInit function
 */
#ifndef USERSPACE_ONLY
    PSP_MODULE_INFO("BennuGD PSP", PSP_MODULE_USER, 1, 0);
#else
	PSP_MODULE_INFO("BennuGD PSP", 0, 1, 1);
#endif

/**
 * THREAD_ATTR_USER causes the thread that the startup
 * code (crt0.c) starts this program in to be in usermode
 * even though the module was started in kernelmode
 */
#ifndef USERSPACE_ONLY
	PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);
#endif

#ifndef USERSPACE_ONLY
void MyExceptionHandler(PspDebugRegBlock *regs)
{
	/* Do normal initial dump, setup screen etc */

	pspDebugScreenInit();

	pspDebugScreenSetBackColor(0x00FF0000);
	pspDebugScreenSetTextColor(0xFFFFFFFF);
	pspDebugScreenClear();

	pspDebugScreenPrintf("Exception Details:\n");
	pspDebugDumpException(regs);

	while (1) ;
}

/**
 * Function that is called from _init in kernelmode before the
 * main thread is started in usermode.
 */
__attribute__ ((constructor)) void loaderInit()
{
    pspKernelSetKernelPC();
    pspDebugInstallErrorHandler(MyExceptionHandler);
}
#endif

/* Exit callback */
SceKernelCallbackFunction exit_callback(int arg1, int arg2, void* common)
{
    sceKernelExitGame();
    exit(0);
    return 0;
}

/* Callback thread */
int CallbackThread(SceSize size, void *arg)
{
	int cbid;

	cbid = sceKernelCreateCallback("Exit Callback", (SceKernelCallbackFunction)exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);

	sceKernelSleepThreadCB();
	return 0;
}

/* Sets up the callback thread and returns its thread id */
int SetupCallbacks(void) {
	int thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
	if (thid >= 0) {
		sceKernelStartThread(thid, 0, 0);
	}

	return thid;
} 

#endif // END OF PSP specific stuff
/* ---------------------------------------------------------------------- */
