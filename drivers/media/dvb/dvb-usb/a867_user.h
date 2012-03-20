#ifndef __USER_H__
#define __USER_H__


//#include <stdio.h>
#include "a867_type.h"
#include "a867_error.h"
//#include <linux/smp_lock.h> //for Linux mdelay
#include <linux/delay.h> //for Linux mdelay



#define User_USE_INTERRUPT              0
#define User_USE_DRIVER                 0

/** Define timeout count for acquirePlatform and setPlatform, the default value 300 means 30 seconds. */
#define User_TIMEOUT_COUNT			300

/** Define I2C master speed, the default value 0x0D means 197KHz (1000000000 / (24.4 * 16 * User_I2C_SPEED)). */
#define User_I2C_SPEED              0x0D

/** Define I2C address of secondary chip when Diversity mode or PIP mode is active. */
#define User_I2C_ADDRESS			0x3A//0x38

/** Define USB frame size */
#define User_USB20_MAX_PACKET_SIZE      512
//j010+s
//#define User_USB20_FRAME_SIZE           (188 * 348)
#define User_USB20_FRAME_SIZE           (188 * TS_PACKET_COUNT)
//j010+e
#define User_USB20_FRAME_SIZE_DW        (User_USB20_FRAME_SIZE / 4)
#define User_USB11_MAX_PACKET_SIZE      64
#define User_USB11_FRAME_SIZE           (188 * 21)
#define User_USB11_FRAME_SIZE_DW        (User_USB11_FRAME_SIZE / 4)

typedef     unsigned char   tBYTE;      // 1 byte
typedef     unsigned short  tWORD;      // 2 bytes
typedef     unsigned int	tDWORD;     // 4 bytes
typedef     int             tINT;       // 4 bytes
typedef     void *          tHANDLE;

/**
 * Memory copy Function
 */
Dword User_memoryCopy (
    IN  Demodulator*    demodulator,
    IN  void*           dest,
    IN  void*           src,
    IN  Dword           count
);


/**
 * Memory free Function
 */
Dword User_memoryFree (
	IN  Demodulator*	demodulator,
	IN  void*			mem
);


/**
 * Print Function
 */
Dword User_printf (
	IN  Demodulator*	demodulator,
	IN  const char*		format, 
	IN  ...
);


/**
 * Delay Function
 */
Dword User_delay (
    IN  Demodulator*    demodulator,
	IN  Dword			dwMs
);


/**
 * Creat and initialize critical section
 */
Dword User_createCriticalSection (
	IN  Demodulator*	demodulator
);


/**
 * Delete critical section
 */
Dword User_deleteCriticalSection (
	IN  Demodulator*	demodulator
);


/**
 * Enter critical section
 */
Dword User_enterCriticalSection (
    IN  Demodulator*    demodulator
);


/**
 * Leave critical section
 */
Dword User_leaveCriticalSection (
    IN  Demodulator*    demodulator
);


/**
 * Config MPEG2 interface
 */
Dword User_mpegConfig (
    IN  Demodulator*    demodulator
);


/**
 * Write data via "Control Bus"
 * I2C mode : uc2WireAddr mean demodulator chip address, the default value is 0x38
 * USB mode : uc2WireAddr is useless, don't have to send this data
 */
Dword User_busTx (
    IN  Demodulator*    demodulator,
    IN  Dword           bufferLength,
    IN  Byte*           buffer
);


/**
 * Read data via "Control Bus"
 * I2C mode : uc2WireAddr mean demodulator chip address, the default value is 0x38
 * USB mode : uc2WireAddr is useless, don't have to send this data
 */
Dword User_busRx (
    IN  Demodulator*    demodulator,
    IN  Dword           bufferLength,
    OUT Byte*           buffer
);


/**
 * Read data via "Data Bus"
 * I2C mode : uc2WireAddr mean demodulator chip address, the default value is 0x38
 * USB mode : uc2WireAddr is useless, don't have to send this data
 */
Dword User_busRxData (
    IN  Demodulator*    demodulator,
    IN  Dword           bufferLength,
    OUT Byte*           buffer
);
#endif
