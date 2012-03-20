/**
 * Maxlinear_MXL5007.h
 *
 * Interface btw Afa9035 and Mxl5007T
 */

#ifndef __Maxlinear_MXL5007_H__
#define __Maxlinear_MXL5007_H__

#include "a867_type.h"
#include "a867_error.h"
#include "a867_user.h"
#include "a867_register.h"
#include "a867_standard.h"
#include "a867_mxl5007t.h"
#include "a867_Common.h"	//for Tuner_struct
typedef unsigned int	UINT32;
typedef unsigned char	UINT8;

extern TunerDescription tuner_MXL5007;

/**
 *
 */

UINT32 MxL_I2C_Write(UINT8 DeviceAddr, void* pArray, UINT32 ByteCount, struct mxl5007t_config* myTuner);
UINT32 MxL_I2C_Read(UINT8 DeviceAddr, UINT8 Addr, UINT8* mData, struct mxl5007t_config* myTuner);
void MxL_Delay(UINT32 mSec);


/**
 *
 */
Dword MXL5007_open (
	IN  Demodulator*	demodulator,
	IN  Byte			chip
);


/**
 *
 */
Dword MXL5007_close (
	IN  Demodulator*	demodulator,
	IN  Byte			chip
);


/**
 *
 */
Dword MXL5007_set (
	IN  Demodulator*	demodulator,
	IN  Byte			chip,
    IN  Word			bandwidth,
    IN  Dword			frequency
);

#endif //__Maxlinear_MXL5007_H__

