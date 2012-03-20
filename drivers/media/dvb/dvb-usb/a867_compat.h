/*
 * $Id: compat.h,v 1.1.1.1 2008/07/09 07:30:49 stylon Exp $
 */

#ifndef _COMPAT_H
#define _COMPAT_H

#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24)
#define	KERN_CONT	""
#endif

/* To allow I2C compatibility code to work */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 24)
#include <linux/i2c-dev.h>
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,23)
# define set_freezable()
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,18)
# define minor(x) MINOR(x)
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,13)
# define DEVICE_ATTR(a,b,c,d) CLASS_DEVICE_ATTR(a,b,c,d)
# define device_create_file(a,b) class_device_create_file(a,b)
# define device_remove_file(a,b) class_device_remove_file(a,b)
# define device_register(a) class_device_register(a)
# define device_unregister(a) class_device_unregister(a)
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)
# include <linux/moduleparam.h>
# include <linux/delay.h>
# define need_resched() (current->need_resched)
# define work_struct tq_struct
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,19)
# define BUG_ON(condition) do { if ((condition)!=0) BUG(); } while(0)
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,23)
# define irqreturn_t void
# define IRQ_RETVAL(foobar)
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,71)
# define strlcpy(dest,src,len) strncpy(dest,src,(len)-1)
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
# define iminor(inode) minor(inode->i_rdev)
#endif

#if defined(I2C_ADAP_CLASS_TV_ANALOG) && !defined(I2C_CLASS_TV_ANALOG)
# define  I2C_CLASS_TV_ANALOG  I2C_ADAP_CLASS_TV_ANALOG
# define  I2C_CLASS_TV_DIGITAL I2C_ADAP_CLASS_TV_DIGITAL
#endif

#ifndef __pure
#  define __pure __attribute__((pure))
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,9)
# define __user
# define __kernel
# define __iomem
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,11)
# define pm_message_t                      u32
# define pci_choose_state(pci_dev, state)  (state)
# define PCI_D0                            (0)
# define assert_spin_locked(foobar)
#endif

/* Since v4l-dvb now includes it's own copy of linux/i2c-id.h these
   are no longer necessary */
/*
#if !defined(I2C_ALGO_SAA7134)
#define I2C_ALGO_SAA7134 I2C_HW_B_BT848
#endif
#if !defined(I2C_HW_B_CX2388x)
# define I2C_HW_B_CX2388x I2C_HW_B_BT848
#endif
#if !defined(I2C_HW_SAA7134)
# define I2C_HW_SAA7134 I2C_ALGO_SAA7134
#endif
#if !defined(I2C_HW_SAA7146)
# define I2C_HW_SAA7146 I2C_ALGO_SAA7146
#endif
#if !defined(I2C_HW_B_EM2820)
#define I2C_HW_B_EM2820 0x99
#endif
*/

#ifndef I2C_M_IGNORE_NAK
# define I2C_M_IGNORE_NAK 0x1000
#endif

/* v4l-dvb uses an out of kernel copy of i2c-id.h, which does not have
   some stuff that previous versions of i2c-id.h defined. */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,14) && defined(LINUX_I2C_ID_H)
# define I2C_ALGO_BIT 0x010000
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,9)
#define __le32 __u32
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)) && (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,7))
static inline unsigned long msecs_to_jiffies(const unsigned int m)
{
#if HZ <= 1000 && !(1000 % HZ)
	return (m + (1000 / HZ) - 1) / (1000 / HZ);
#else
#if HZ > 1000 && !(HZ % 1000)
	return m * (HZ / 1000);
#else
	return (m * HZ + 999) / 1000;
#endif
#endif
}
static inline unsigned int jiffies_to_msecs(const unsigned long j)
{
#if HZ <= 1000 && !(1000 % HZ)
	return (1000 / HZ) * j;
#else
#if HZ > 1000 && !(HZ % 1000)
	return (j + (HZ / 1000) - 1)/(HZ / 1000);
#else
	return (j * 1000) / HZ;
#endif
#endif
}
static inline void msleep(unsigned int msecs)
{
	unsigned long timeout = msecs_to_jiffies(msecs);
	while (timeout) {
		set_current_state(TASK_UNINTERRUPTIBLE);
		timeout = schedule_timeout(timeout);
	}
}
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)) && LINUX_VERSION_CODE < KERNEL_VERSION(2,6,9)
static inline unsigned long msleep_interruptible(unsigned int msecs)
{
	unsigned long timeout = msecs_to_jiffies(msecs);

	while (timeout) {
		set_current_state(TASK_INTERRUPTIBLE);
		timeout = schedule_timeout(timeout);
	}
	return jiffies_to_msecs(timeout);
}
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
/* some keys from 2.6.x which are not (yet?) in 2.4.x */
# define KEY_PLAY                207
# define KEY_PRINT		 210
# define KEY_EMAIL         215
# define KEY_SEARCH              217
# define KEY_SELECT 		 0x161
# define KEY_GOTO                0x162
# define KEY_INFO                0x166
# define KEY_CHANNEL             0x16b
# define KEY_LANGUAGE            0x170
# define KEY_SUBTITLE		 0x172
# define KEY_ZOOM                0x174
# define KEY_MODE		 0x175
# define KEY_TV                  0x179
# define KEY_CD                  0x17f
# define KEY_TUNER               0x182
# define KEY_TEXT                0x184
# define KEY_DVD		 0x185
# define KEY_AUDIO               0x188
# define KEY_VIDEO               0x189
# define KEY_RED                 0x18e
# define KEY_GREEN               0x18f
# define KEY_YELLOW              0x190
# define KEY_BLUE                0x191
# define KEY_CHANNELUP           0x192
# define KEY_CHANNELDOWN         0x193
# define KEY_RESTART		 0x198
# define KEY_SHUFFLE     	 0x19a
# define KEY_NEXT                0x197
# define KEY_RADIO               0x181
# define KEY_PREVIOUS            0x19c
# define KEY_MHP                 0x16f
# define KEY_EPG                 0x16d
# define KEY_FASTFORWARD         208
# define KEY_LIST                0x18b
# define KEY_LAST                0x195
# define KEY_CLEAR               0x163
# define KEY_AUX                 0x186
# define KEY_SCREEN              0x177
# define KEY_PC                  0x178
# define KEY_MEDIA               226
# define KEY_SLOW                0x199
# define KEY_OK                  0x160
# define KEY_DIGITS              0x19d
#endif
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,14)
# define KEY_SEND		231
# define KEY_REPLY		232
# define KEY_FORWARDMAIL	233
# define KEY_SAVE		234
# define KEY_DOCUMENTS		235
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)
#define container_of(ptr, type, member) ({                      \
	const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
	(type *)( (char *)__mptr - offsetof(type,member) );})
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,10)
#include <linux/mm.h>
static inline unsigned long vmalloc_to_pfn(void * vmalloc_addr)
{
    return page_to_pfn(vmalloc_to_page(vmalloc_addr));
}

static unsigned long kvirt_to_pa(unsigned long adr)
{
	unsigned long kva, ret;

	kva = (unsigned long) page_address(vmalloc_to_page((void *)adr));
	kva |= adr & (PAGE_SIZE-1); /* restore the offset */
	ret = __pa(kva);
	return ret;
}

#ifndef wait_event_timeout
#define wait_event_timeout(wq, condition, timeout)                   	     \
({                                                                           \
     long __ret = timeout;                                                   \
     if (!(condition))                                                       \
     do {                                                                    \
	     DEFINE_WAIT(__wait);                                            \
	     for (;;) {                                                      \
		     prepare_to_wait(&wq, &__wait, TASK_UNINTERRUPTIBLE);    \
		     if (condition)                                          \
			 break;                                              \
		     __ret = schedule_timeout(__ret);                        \
		     if (!__ret)                                             \
			 break;                                              \
	     }                                                               \
	     finish_wait(&wq, &__wait);                                      \
     } while (0);							     \
     __ret;                                                                  \
})
#endif

#define remap_pfn_range remap_page_range

#endif

/* vm_insert_page() was added in 2.6.15 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,15) && defined(_LINUX_MM_H)
static inline int vm_insert_page(struct vm_area_struct *vma,
	unsigned long addr, struct page *page)
{
	return remap_pfn_range(vma, addr, page_to_pfn(page), PAGE_SIZE,
			       vma->vm_page_prot);
}
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,9)
#ifndef kcalloc
#define kcalloc(n,size,flags)			\
({						\
  void * __ret = NULL;				\
  __ret = kmalloc(n * size, flags);		\
  if (__ret)					\
	 memset(__ret, 0, n * size);		\
  __ret;					\
})
#endif
#endif

/* try_to_freeze() lost its argument.  Must appear after linux/sched.h */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,13) && defined(_LINUX_SCHED_H)
# if LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,0)
#  define try_to_freeze() try_to_freeze(PF_FREEZE)
# else
#  define try_to_freeze() (0)
# endif
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,14)
#ifndef kzalloc
#define kzalloc(size, flags)				\
({							\
	void *__ret = kmalloc(size, flags);		\
	if (__ret)					\
		memset(__ret, 0, size);			\
	__ret;						\
})
#endif
#endif

/* The class_device system didn't appear until 2.5.69 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
#define class_device_create_file(a, b) (0)
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,13)
# define class_device_create(a, b, c, d, e, f, g, h) class_simple_device_add(a, c, d, e, f, g, h)
# define class_device_destroy(a, b) class_simple_device_remove(b)
# define class_create(a, b) class_simple_create(a, b)
# define class_destroy(a) class_simple_destroy(a)
#elif LINUX_VERSION_CODE < KERNEL_VERSION(2,6,15)
# define class_device_create(a, b, c, d, e, f, g, h) class_device_create(a, c, d, e, f, g, h)
#endif
/* device_create/destroy added in 2.6.18 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18)
/* on older kernels, class_device_create will in turn be a compat macro */
# define device_create(a, b, c, d, e, f, g) class_device_create(a, NULL, c, b, d, e, f, g)
# define device_destroy(a, b) class_device_destroy(a, b)
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,15)
# define input_allocate_device() kzalloc(sizeof(struct input_dev),GFP_KERNEL);
# define input_free_device(input_dev) kfree(input_dev)
# ifdef _INPUT_H  /* input.h must be included _before_ compat.h for this to work */
   /* input_register_device() was changed to return an error code in 2.6.15 */
#  define input_register_device(x) (input_register_device(x), 0)
# endif
#endif

#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,15)
#define DEFINE_MUTEX(a) DECLARE_MUTEX(a)
#define mutex_lock_interruptible(a) down_interruptible(a)
#define mutex_unlock(a) up(a)
#define mutex_lock(a) down(a)
#define mutex_init(a) init_MUTEX(a)
#define mutex_trylock(a) down_trylock(a)
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,14) && defined(_LINUX_SCHED_H)
static inline signed long __sched
schedule_timeout_interruptible(signed long timeout)
{
	__set_current_state(TASK_INTERRUPTIBLE);
	return schedule_timeout(timeout);
}
#endif

/* New 4GB DMA zone was added in 2.6.15-rc2 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,15)
#  define __GFP_DMA32	__GFP_DMA
#endif

/* setup_timer() helper added 10/31/05, 2.6.15-rc1 */
/* Need linux/timer.h to be included for struct timer_list */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,15) && defined(_LINUX_TIMER_H)
static inline void setup_timer(struct timer_list * timer,
			       void (*function)(unsigned long),
			       unsigned long data)
{
	timer->function = function;
	timer->data = data;
	init_timer(timer);
}
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18)
#define IRQF_SHARED		SA_SHIRQ
#define IRQF_DISABLED		SA_INTERRUPT
#endif

/* linux/usb.h must be included _before_ compat.h for this code to get
   turned on.  We can not just include usb.h here, because there is a
   lot of code which will not compile if it has usb.h included, due to
   conflicts with symbol names.  */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,13) && \
    defined(__LINUX_USB_H) && defined(_INPUT_H)
#include <linux/input.h>
/* Found in linux/usb_input.h in 2.6.13 */
/* Moved to linux/usb/input.h in 2.6.18 */
static inline void
usb_to_input_id(const struct usb_device *dev, struct input_id *id)
{
	id->bustype = BUS_USB;
	id->vendor = le16_to_cpu(dev->descriptor.idVendor);
	id->product = le16_to_cpu(dev->descriptor.idProduct);
	id->version = le16_to_cpu(dev->descriptor.bcdDevice);
}
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,19)
# define PCIAGP_FAIL 0

#define vmalloc_32_user(a) vmalloc_32(a)

#endif

/* bool type and enum-based definition of true and false was added in 2.6.19 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,19)
typedef int bool;
#define true 1
#define false 0
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,22)
#define sony_pic_camera_command(a,b) sonypi_camera_command(a,b)

#define SONY_PIC_COMMAND_SETCAMERAAGC        SONYPI_COMMAND_SETCAMERAAGC
#define SONY_PIC_COMMAND_SETCAMERABRIGHTNESS SONYPI_COMMAND_SETCAMERABRIGHTNESS
#define SONY_PIC_COMMAND_SETCAMERACOLOR      SONYPI_COMMAND_SETCAMERACOLOR
#define SONY_PIC_COMMAND_SETCAMERACONTRAST   SONYPI_COMMAND_SETCAMERACONTRAST
#define SONY_PIC_COMMAND_SETCAMERAHUE        SONYPI_COMMAND_SETCAMERAHUE
#define SONY_PIC_COMMAND_SETCAMERAPICTURE    SONYPI_COMMAND_SETCAMERAPICTURE
#define SONY_PIC_COMMAND_SETCAMERASHARPNESS  SONYPI_COMMAND_SETCAMERASHARPNESS
#define SONY_PIC_COMMAND_SETCAMERA           SONYPI_COMMAND_SETCAMERA
#endif

/* Parameter to pci_match_device() changed in 2.6.13-rc2 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,13) && defined(LINUX_PCI_H)
#define pci_match_device(drv, dev)	pci_match_device((drv)->id_table, dev)
#endif

/* pci_dev got a new revision field in 2.6.23-rc1 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,23) && defined(LINUX_PCI_H)
/* Just make it easier to subsitute pci_dev->revision with
 * v4l_compat_pci_rev(pci_dev).  It's too bad there isn't some kind of context
 * sensitive macro in C that could do this for us.  */
static inline u8 v4l_compat_pci_rev(struct pci_dev *pci)
{ u8 rev; pci_read_config_byte(pci, PCI_REVISION_ID, &rev); return rev; }
#endif

/* ALSA removed a bunch of typedefs and renamed some structs in 2.6.16 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,16)
# ifdef __SOUND_CORE_H
#  define snd_card _snd_card /* struct _snd_card became struct snd_card */
#  define snd_pcm _snd_pcm
#  undef snd_device
#  define snd_device _snd_device
# endif
# ifdef __SOUND_PCM_H
#  define snd_pcm_substream _snd_pcm_substream
#  define snd_pcm_hardware _snd_pcm_hardware
#  define snd_pcm_runtime _snd_pcm_runtime
#  define snd_pcm_ops _snd_pcm_ops
# endif
# ifdef __SOUND_ASOUND_H
#  define snd_pcm_hw_params sndrv_pcm_hw_params
#  define snd_ctl_elem_info sndrv_ctl_elem_info
#  define snd_ctl_elem_value sndrv_ctl_elem_value
# endif
# ifdef __SOUND_CONTROL_H
#  undef snd_kcontrol
#  define snd_kcontrol _snd_kcontrol
#  define snd_kcontrol_new _snd_kcontrol_new
# endif
#endif

#if defined(COMPAT_PCM_TO_RATE_BIT) && defined(__SOUND_PCM_H)
/* New alsa core utility function */
static inline unsigned int snd_pcm_rate_to_rate_bit(unsigned int rate)
{
	static const unsigned int rates[] = { 5512, 8000, 11025, 16000, 22050,
		32000, 44100, 48000, 64000, 88200, 96000, 176400, 192000 };
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(rates); i++)
		if (rates[i] == rate)
			return 1u << i;
	return SNDRV_PCM_RATE_KNOT;
}
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
# define task_pid_nr(current) ((current)->pid)

# define sg_init_table(a,b)
# define sg_page(p) (sg->page)
# define sg_set_page(sglist,pg,sz,off)					\
do {									\
	struct scatterlist *p=sglist;					\
	p->page   = pg;							\
	p->length = sz;							\
	p->offset = off;						\
} while (0)
#endif

#ifndef BIT_MASK
# define BIT_MASK(nr)            (1UL << ((nr) % BITS_PER_LONG))
# define BIT_WORD(nr)            ((nr) / BITS_PER_LONG)
#endif

#endif
/*
 * Local variables:
 * c-basic-offset: 8
 * End:
 */
