#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 33)  
#include <linux/autoconf.h>
#else
#include <generated/autoconf.h>
#endif
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/kref.h>
#include <linux/usb.h>
#include <asm/uaccess.h>
#include <linux/device.h>
#include <linux/string.h>
#include <linux/firmware.h>
#include <linux/delay.h>
#include <linux/vmalloc.h>

#include "a867_usb2impl.h"
#include "a867_af903x.h"

#ifdef UNDER_CE

Handle Usb2_handle = NULL;


Dword Usb2_getDriver (
	IN  Demodulator*	demodulator,
	OUT Handle*			handle
) {
	return (Error_NO_ERROR);
}


Dword Usb2_writeControlBus (
	IN  Demodulator*	demodulator,
	IN  Dword			bufferLength,
	IN  Byte*			buffer
) {
	return (Error_NO_ERROR);
}


Dword Usb2_readControlBus (
	IN  Demodulator*	demodulator,
	IN  Dword			bufferLength,
	OUT Byte*			buffer
) {
	return (Error_NO_ERROR);
}


Dword Usb2_readDataBus (
	IN  Demodulator*	demodulator,
	IN  Dword			bufferLength,
	OUT Byte*			buffer
) {
	return (Error_NO_ERROR);
}

#else

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif

Handle Usb2_handle = 0;
/*
bool (__cdecl *Usb2_initialize) (
);
void (__cdecl *Usb2_finalize) (
);
bool (__cdecl *Usb2_writeControl) (
	Byte*		poutBuf,
	unsigned long		WriteLen,
	unsigned long*		pnBytesWrite
); 
bool (__cdecl *Usb2_readControl) (
	Byte*		pinBuf,
	unsigned long		ReadLen,
	unsigned long*		pnBytesRead
);
bool (__cdecl *Usb2_readData) (
	BYTE*		pinBuf,
	ULONG		ReadLen
);
*/

Dword Usb2_getDriver (
	IN  Demodulator*	demodulator,
	OUT Handle*			handle
) {
	Dword error = Error_NO_ERROR;
/*
	HINSTANCE instance = NULL;

    instance = LoadLibrary ("AF15BDAEX.dll");
    Usb2_initialize = (bool (__cdecl *) (
			)) GetProcAddress (instance, "af15_init");
    Usb2_finalize = (void (__cdecl *) (
			)) GetProcAddress (instance, "af15_exit");
	Usb2_writeControl = (bool (__cdecl *) (
					BYTE*		poutBuf,
					ULONG		WriteLen,
					ULONG*		pnBytesWrite
			)) GetProcAddress (instance, "af15_WriteBulkData");
    Usb2_readControl = (bool (__cdecl *) (
					BYTE*		pinBuf,
					ULONG		ReadLen,
					ULONG*		pnBytesRead
			)) GetProcAddress (instance, "af15_ReadBulkData");
    Usb2_readData = (bool (__cdecl *) (
					BYTE*		pinBuf,
					ULONG		ReadLen
			)) GetProcAddress (instance, "af15_GetTsData");

	if (!Usb2_initialize ())
		error = Error_DRIVER_INVALID;
			
	*handle = (Handle) instance;
*/
	return (error);
}


Dword Usb2_writeControlBus (
	IN  Demodulator*	demodulator,
	IN  Dword			bufferLength,
	IN  Byte*			buffer
) {
//    Ganymede *pGanymede = (Ganymede *)demodulator;
    Dword     ret,act_len;
	    		ret = usb_bulk_msg(usb_get_dev(udevs),
			usb_sndbulkpipe(usb_get_dev(udevs), 0x02),
			buffer,
			bufferLength,
			&act_len,
			1000);
    
	return (Error_NO_ERROR);
}


Dword Usb2_readControlBus (
	IN  Demodulator*	demodulator,
	IN  Dword			bufferLength,
	OUT Byte*			buffer
) {
//    Ganymede *pGanymede = (Ganymede *)demodulator;
	Dword     ret, nBytesRead;
   ret = usb_bulk_msg(usb_get_dev(udevs),
				usb_rcvbulkpipe(usb_get_dev(udevs),129),
				buffer,
				255,
				&nBytesRead,
				1000);
    
	return (Error_NO_ERROR);
}


Dword Usb2_readDataBus (
	IN  Demodulator*	demodulator,
	IN  Dword			bufferLength,
	OUT Byte*			buffer
) {
	return (Error_NO_ERROR);
}
#endif
