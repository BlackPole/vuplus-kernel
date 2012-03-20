#ifndef _AF903X_IOCTL_H_
#define _AF903X_IOCTL_H_

#include <linux/ioctl.h> /* needed for the _IOW etc stuff used later */

/*
 * Ioctl definitions
 */

/* Use 'k' as magic number */
#define AFA_IOC_MAGIC  'k'
/* Please use a different 8-bit number in your code */

//#define AFA_IOCRESETINTERVAL    _IO(AFA_IOC_MAGIC, 0)

/*
 * S means "Set" through a ptr,
 * T means "Tell" directly with the argument value
 * G means "Get": reply by setting through a pointer
 * Q means "Query": response is on the return value
 * X means "eXchange": switch G and S atomically
 * H means "sHift": switch T and Q atomically
 */


/* ... more to come */

#define AFA_IOC_MAXNR 14
#endif /* _AF903X-IOCTL_H_ */
