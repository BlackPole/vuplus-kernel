/*
 * @(#)Afatech_AF9007_EXT.cpp
 *
 * Copyright 2005 Afatech, Inc. All rights reserved.
 */

//#include <stdio.h>
#include "a867_type.h"
#include "a867_error.h"
#include "a867_user.h"
#include "a867_register.h"
#include "a867_standard.h"
#include "a867_Afa_AF9007_Script.h"


Dword AF9007_open (
	IN  Demodulator*	demodulator,
	IN  Byte			chip
) {
	return (Error_NO_ERROR);
}

Dword AF9007_close (
	IN  Demodulator*	demodulator,
	IN  Byte			chip
) {
	return (Error_NO_ERROR);
}

Dword AF9007_set (
	IN  Demodulator*	demodulator,
	IN  Byte			chip,
	IN  Word			bandwidth,
	IN  Dword			frequency
) {
	return (Error_NO_ERROR);
}

TunerDescription tuner_AF9007 = {
    AF9007_open,
    AF9007_close,
    AF9007_set,
    AF9007_scripts,
    AF9007_scriptSets,
    0,                              /** tuner i2c address */
    0,                              /** length of tuner register address */
    36167000,                       /** tuner if */
    True,                           /** spectrum inverse */
    0xFF                            /** tuner id */
};
