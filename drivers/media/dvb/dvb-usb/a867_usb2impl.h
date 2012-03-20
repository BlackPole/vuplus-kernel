#ifndef __USB2IMPL_H__
#define __USB2IMPL_H__


#include "a867_type.h"
#include "a867_error.h"
#include "a867_user.h"
#include "a867_cmd.h"


Dword Usb2_getDriver (
	IN  Demodulator*	demodulator,
	OUT Handle*			handle
);


Dword Usb2_writeControlBus (
	IN  Demodulator*	demodulator,
	IN  Dword			bufferLength,
	IN  Byte*			buffer
);


Dword Usb2_readControlBus (
	IN  Demodulator*	demodulator,
	IN  Dword			bufferLength,
	OUT Byte*			buffer
);


Dword Usb2_readDataBus (
	IN  Demodulator*	demodulator,
	IN  Dword			bufferLength,
	OUT Byte*			buffer
);

#endif

