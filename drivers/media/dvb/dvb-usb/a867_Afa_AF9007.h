/**
 * @(#)Afatech_AF9007_EXT.h
 *
 * Copyright 2005 Afatech, Inc. All rights reserved.
 */
#ifndef __Afatech_AF9007_EXT_H__
#define __Afatech_AF9007_EXT_H__


extern TunerDescription tuner_AF9007;


/**
 *
 */
Dword AF9007_open (
	IN  Demodulator*	demodulator,
	IN  Byte			chip
);

/**
 *
 */
Dword AF9007_close (
	IN  Demodulator*	demodulator,
	IN  Byte			chip
);

/**
 *
 */
Dword AF9007_set (
	IN  Demodulator*	demodulator,
	IN  Byte			chip,
    IN  Word			bandwidth,
    IN  Dword			frequency
);
#endif

