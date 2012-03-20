#include "a867_user.h"
#include "a867_af903x.h"


/**
 * Handle for specific bus driver
 */
Handle User_handle = NULL;


/**
 * Totoal number of chip
 */
Byte User_chipNumber;


/**
 * Current index of chip
 */
Byte User_chipIndex;


/**
 * Variable of critical section
 */

Dword User_memoryCopy (
    IN  Demodulator*    demodulator,
    IN  void*           dest,
    IN  void*           src,
    IN  Dword           count
) {
    /*
     *  ToDo:  Add code here
     *
     *  //Pseudo code
     *  memcpy(dest, src, (size_t)count);
	 *  return (0);
     */
	//memcpy (dest, src, (size_t)count);
    return (Error_NO_ERROR);
}


Dword User_memoryFree (
	IN  Demodulator*	demodulator,
	IN  void*			mem
) {
    /*
     *  ToDo:  Add code here
     *
     *  //Pseudo code
     *  free(pMem);
	 *  return (0);
     */
	//free (mem);
	//ExFreePool(mem);
    return (Error_NO_ERROR);
}


Dword User_printf (
	IN  Demodulator*	demodulator,
	IN  const char*		format, 
	IN  ...
) {
    /*
     *  ToDo:  Add code here
     *
     *  //Pseudo code
     *  va_list arg;
     *
     *  va_start(arg, format);
     *  vprintf(format, arg);
     *  va_end(arg);
     *  return (0);
     */
	return (Error_NO_ERROR);
}


Dword User_delay (
    IN  Demodulator*    demodulator,
	IN  Dword			dwMs
) {	
    /*
     *  ToDo:  Add code here
     *
     *  //Pseudo code
     *  delay(dwMs);
     *  return (0);
     */
    //Sleep (dwMs);
	unsigned long j = (HZ*dwMs)/1000;
	set_current_state(TASK_UNINTERRUPTIBLE);
	schedule_timeout(j);

	return (Error_NO_ERROR);
}


Dword User_createCriticalSection (
	IN  Demodulator*	demodulator
) {
    /*
     *  ToDo:  Add code here 
     *
     *  //Pseudo code
     *  return (0);
     */
	return (Error_NO_ERROR);
}


Dword User_deleteCriticalSection (
	IN  Demodulator*	demodulator
) {
    /*
     *  ToDo:  Add code here
     *
     *  //Pseudo code
     *  return (0);
     */
    return (Error_NO_ERROR);
}


Dword User_enterCriticalSection (
    IN  Demodulator*    demodulator
) {
    /*
     *  ToDo:  Add code here
     *
     *  //Pseudo code
     *  return (0);
     */
    PDEVICE_CONTEXT PDC = (PDEVICE_CONTEXT)demodulator->userData;
    if( PDC ) {
	    down(&PDC->regLock); 
    }
    return (Error_NO_ERROR);
}


Dword User_leaveCriticalSection (
    IN  Demodulator*    demodulator
) {
    /*
     *  ToDo:  Add code here
     *
     *  //Pseudo code
     *  return (0);
     */

    PDEVICE_CONTEXT PDC = (PDEVICE_CONTEXT)demodulator->userData;
    if( PDC ) {
	    up(&PDC->regLock); 
    }
    return (Error_NO_ERROR);
}


Dword User_mpegConfig (
    IN  Demodulator*    demodulator
) {
    /*
     *  ToDo:  Add code here
     *
     */
    return (Error_NO_ERROR);
}


Dword User_busTx (
    IN  Demodulator*    demodulator,
    IN  Dword           bufferLength,
    IN  Byte*           buffer
) {
    /*
     *  ToDo:  Add code here
     *
     *  //Pseudo code
     *  short i;
     *
     *  start();
     *  write_i2c(uc2WireAddr);
     *  ack();
     *  for (i = 0; i < bufferLength; i++) {
     *      write_i2c(*(ucpBuffer + i));
     *      ack();
     *  }
     *  stop();
     *
     *  // If no error happened return 0, else return error code.
     *  return (0);
     */
    return (Error_NO_ERROR);
}


Dword User_busRx (
    IN  Demodulator*    demodulator,
    IN  Dword           bufferLength,
    OUT Byte*           buffer
) {
    /*
     *  ToDo:  Add code here
     *
     *  //Pseudo code
     *  short i;
     *
     *  start();
     *  write_i2c(uc2WireAddr | 0x01);
     *  ack();
     *  for (i = 0; i < bufferLength - 1; i++) {
     *      read_i2c(*(ucpBuffer + i));
     *      ack();
     *  }
     *  read_i2c(*(ucpBuffer + bufferLength - 1));
     *  nack();
     *  stop();
     *
     *  // If no error happened return 0, else return error code.
     *  return (0);
     */
    return (Error_NO_ERROR);
}


Dword User_busRxData (
    IN  Demodulator*    demodulator,
    IN  Dword           bufferLength,
    OUT Byte*           buffer
) {
    return (Error_NO_ERROR);
}
