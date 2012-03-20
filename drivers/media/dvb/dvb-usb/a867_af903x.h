 
#ifndef _AF903X_H_
#define _AF903X_H_

#define DVB_USB_LOG_PREFIX "AF903X"
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/kref.h>
#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,39)
#include <linux/smp_lock.h>
#else
#include <linux/mutex.h>
#endif
#include <linux/usb.h>
#include <asm/uaccess.h>
#include "dvb-usb.h"
#include "a867_af903x-ioctl.h"
#include "a867_demodulator.h"
#include "a867_userdef.h"
//#include "a867_firmware.h"
#include "a867_type.h"
#include "a867_Common.h"
#include "a867_debug.h"

#define ENABLE_TEST_FUNCTION 0
#define ENABLE_HW_PID 0


//enable snr reading register , default = 0
#define ENABLE_READ_REG 1


#define SUPPORT_AF903X_EVB	0

//***************** from device.h *****************//
#define AFA_USB_DEVICE

#define SLAVE_DEMOD_2WIREADDR  0x3A

#define TS_PACKET_SIZE              	188
#define TS_PACKET_COUNT_HI       348
#define TS_PACKET_COUNT_FU       21

//***************** from driver.h *****************//
#define TS_FRAMES_HI 16
#define TS_FRAMES_FU 128
#define MAX_USB20_IRP_NUM  5
#define MAX_USB11_IRP_NUM  2

//***************** from afdrv.h *****************//
#define GANY_ONLY 0x42F5
#define EEPROM_FLB_OFS  8

#define EEPROM_IRMODE      (GANY_ONLY+EEPROM_FLB_OFS+0x10)   //00:disabled, 01:HID
#define EEPROM_SELSUSPEND  (GANY_ONLY+EEPROM_FLB_OFS+0x28)   //Selective Suspend Mode
#define EEPROM_TSMODE      (GANY_ONLY+EEPROM_FLB_OFS+0x28+1) //0:one ts, 1:dual ts
#define EEPROM_2WIREADDR   (GANY_ONLY+EEPROM_FLB_OFS+0x28+2) //MPEG2 2WireAddr
#define EEPROM_SUSPEND     (GANY_ONLY+EEPROM_FLB_OFS+0x28+3) //Suspend Mode
#define EEPROM_IRTYPE      (GANY_ONLY+EEPROM_FLB_OFS+0x28+4) //0:NEC, 1:RC6
#define EEPROM_SAWBW1      (GANY_ONLY+EEPROM_FLB_OFS+0x28+5)
#define EEPROM_XTAL1       (GANY_ONLY+EEPROM_FLB_OFS+0x28+6) //0:28800, 1:20480
#define EEPROM_SPECINV1    (GANY_ONLY+EEPROM_FLB_OFS+0x28+7)
#define EEPROM_TUNERID     (GANY_ONLY+EEPROM_FLB_OFS+0x30+4) //
#define EEPROM_IFFREQL     (GANY_ONLY+EEPROM_FLB_OFS+0x30) 
#define EEPROM_IFFREQH     (GANY_ONLY+EEPROM_FLB_OFS+0x30+1)   
#define EEPROM_IF1L        (GANY_ONLY+EEPROM_FLB_OFS+0x30+2)   
#define EEPROM_IF1H        (GANY_ONLY+EEPROM_FLB_OFS+0x30+3)
#define EEPROM_SHIFT       (0x10)                 //EEPROM Addr Shift for slave front end

#define CHECK_LOCK_LOOPS	(20)
#define USE_MONITOR_TH		1


extern int dvb_usb_af903x_debug;
extern int dvb_usb_af903x_snrdb;
/*
#if CONFIG_DVB_USB_DEBUG
#define avprintk(dbg, lvl, fmt, args...) \
		do { \
			printk(fmt, ## args); \
		} while(0)
#else
#define avprintk(dbg, lvl, fmt, args...) do {} while(0)
#endif //CONFIG_DVB_USB_DEBUG

#define deb_info(fmt, args...)   avprintk(dvb_usb_af903x_debug,0x01,fmt, ## args)
#define deb_fw(fmt, args...)     avprintk(dvb_usb_af903x_debug,0x02,fmt, ## args)
#define deb_fwdata(fmt, args...) avprintk(dvb_usb_af903x_debug,0x04,fmt, ## args)
#define deb_data(fmt, args...)   avprintk(dvb_usb_af903x_debug,0x08,fmt, ## args)
*/
//#define deb_data(args...)   printk(KERN_NOTICE args)

#define deb_force(args...) printk(KERN_DEBUG args);
#define deb_info(args...) if (dvb_usb_af903x_debug & 1) printk(KERN_DEBUG args);
#define deb_fw(args...) if (dvb_usb_af903x_debug & 2) printk(KERN_DEBUG args);
#define deb_fwdata(args...)   if (dvb_usb_af903x_debug & 4) printk(KERN_DEBUG args);
#define deb_data(args...)   if (dvb_usb_af903x_debug & 8) printk(KERN_DEBUG args);

typedef struct _GPIO_MAPPINGS {
	unsigned short  I2C_SLAVE_ADDR;
	unsigned short  RF_SW_HOST;
	int GPIO_UHF_en;
	int GPIO_UHF_on;
	int GPIO_UHF_o;
	int GPIO_VHF_en;
	int GPIO_VHF_on;
	int GPIO_VHF_o;
	int GPIO_WP_en;
	int GPIO_WP_on;
	int GPIO_WP_o;
	int GPIO_OSC_en;
	int GPIO_OSC_on;
	int GPIO_OSC_o;
	int GPIO_TUR1_en;
	int GPIO_TUR1_on;
	int GPIO_TUR1_o;
	int GPIO_TUR2_en;
	int GPIO_TUR2_on;
	int GPIO_TUR2_o;
	int GPIO_DPWR_en;
	int GPIO_DPWR_on;
	int GPIO_DPWR_o;
	int GPIO_DRST_en;
	int GPIO_DRST_on;
	int GPIO_DRST_o;
	int GPIO_STR_en;
	int GPIO_STR_on;
	int GPIO_STR_o;
	int GPIO_STR_i;
	int GPIO_LED_en;
	int GPIO_LED_on;
	int GPIO_LED_o;
} GPIO_MAPPINGS, *PGPIO_MAPPINGS;

//***************** from device.h *****************//
typedef struct _TUNER_INFO {

    Bool bTunerInited;
    Bool bSettingFreq;
    BYTE TunerId;
    Bool bTunerOK;
    Tuner_struct MXL5005_Info;

} TUNER_INFO, *PTUNER_INFO;


typedef struct  _FILTER_INFO{
    int filternum;
    Bool onoff;
}  FILTER_INFO;


typedef struct _FILTER_CONTEXT_HW {
    DWORD ulCurrentFrequency;
    WORD  ucCurrentBandWidth;  
    DWORD ulDesiredFrequency;
    WORD  ucDesiredBandWidth;   
    //ULONG ulBandWidth;   
    Bool bTimerOn;
   // PKSFILTER filter;
    Byte GraphBuilt;
    TUNER_INFO tunerinfo; 
    //SIGNAL_STATISTICS ss;
    //SIGNAL_RETRAIN sr;  
    //DWORD   gdwOrigFCW;     //move from AF901x.cpp [global variable]
    //BYTE    gucOrigUnplugTh; //move from AF901x.cpp [global variable]
    //BYTE    gucPreShiftIdx;    //move from AF901x.cpp [global variable]    
   // PKSFILTERFACTORY  pFilterFactory;
    int  bEnPID;
    int ulcPIDs;
    FILTER_INFO aulPIDs[32];
    Bool bApOn;
    int bResetTs;
    Byte OvrFlwChk;
    Byte UnLockCount;

    BYTE AVerFlags;
} FILTER_CONTEXT_HW, *PFILTER_CONTEXT_HW;  

typedef struct _DEVICE_CONTEXT {
    FILTER_CONTEXT_HW fc[2];
    Byte DeviceNo;
    Bool bBootCode;
    Bool bEP12Error;
    Bool bEP45Error;
    //bool bDebugMsg;
    //bool bDevExist;
    Bool bDualTs;
    Bool bIrTblDownload;
    Byte BulkOutData[256];
    u32 WriteLength;
    Bool bSurpriseRemoval;
    Bool bDevNotResp;
    Bool bEnterSuspend;
    Bool bSupportSuspend;
    Bool bSupportSelSuspend;
    u16 regIdx; 
    Byte eepromIdx;
    u16 UsbMode;
    u16 MaxPacketSize;
    u32 MaxIrpSize;
    u32 TsFrames;
    u32 TsFrameSize;
    u32 TsFrameSizeDw;
    u32 TsPacketCount;
    //BYTE  ucDemod2WireAddr;
    //USB_IDLE_CALLBACK_INFO cbinfo;          // callback info for selective suspend          // our selective suspend IRP    

    Bool    bSelectiveSuspend;
    u32   ulActiveFilter;
    //BYTE    ucSerialNo; 
    Architecture architecture;
    //BYTE Tuner_Id;
    StreamType StreamType;
    Bool bDCAPIP;
    Bool bSwapFilter;
    Byte FilterCnt;
    Bool  bTunerPowerOff;
    //PKSPIN PinSave;
    Byte UsbCtrlTimeOut;
	
   Ganymede Demodulator;
   
    Bool ForceWrite;

    GPIO_MAPPINGS Map;

    struct semaphore powerLock;
    int power_use_count;
    struct semaphore tunerLock;

    unsigned short idVendor;
    unsigned short idProduct;

    struct semaphore regLock;
	
} DEVICE_CONTEXT, *PDEVICE_CONTEXT;

#define PTI             (PDC->fc[ucSlaveDemod].tunerinfo)   //TunerInfo pointer



struct af903x_ofdm_channel {
	u32 RF_kHz;
	u8  Bw;
	s16 nfft;
	s16 guard;
	s16 nqam;
	s16 vit_hrch;
	s16 vit_select_hp;
	s16 vit_alpha;
	s16 vit_code_rate_hp;
	s16 vit_code_rate_lp;
	u8  intlv_native;
};

struct tuner_priv {
        struct tuner_config *cfg;
        struct i2c_adapter   *i2c;

        u32 frequency;
        u32 bandwidth;
        u16 if1_freq;
        u8  fmfreq;
};

extern struct dvb_frontend * tuner_attach(struct dvb_frontend *fe);
extern struct dvb_frontend * af903x_attach(u8 TMP);
extern struct dvb_usb_device_properties af903x_properties[];
extern struct usb_device_id af903x_usb_id_table[];
extern struct usb_device *udevs;
extern struct usb_interface *uintfs;
extern PDEVICE_CONTEXT PDC;
extern int af903x_device_count;

extern void af903x_start_monitor_thread(struct dvb_frontend *demod);
extern void af903x_stop_monitor_thread(struct dvb_frontend *demod);

extern DWORD Device_init(struct usb_device *udev, struct usb_interface *uintf, PDEVICE_CONTEXT PDCs, Bool bBoot);
extern DWORD DL_ApCtrl (Bool bOn);
extern DWORD DL_Tuner_SetBW(u8 ucBw);
extern DWORD DL_Tuner_SetFreq(u32 ucFreq,u8 ucBw);
extern DWORD DL_ReSetInterval(void);
extern DWORD DL_GetChannelStat(u32 *ber, u32 *berbits, u32 *ubc);
extern DWORD DL_GetSignalStrength(u16 *strength);
extern DWORD DL_GetSignalQuality(u16 *strength);
extern DWORD DL_GetLocked(Bool *bLock);
extern DWORD DL_MonitorReception(Bool *bLock);
extern DWORD DL_IsPsbOverflow(void *handle, Byte ucSlaveDemod, Bool *bPsvOverflow);
extern DWORD DL_Reboot(void);
extern DWORD DL_ResetPID(void);
extern DWORD DL_AddPID(BYTE index, Pid pid);
extern DWORD DL_PIDOnOff(DWORD OnOff);
extern DWORD DL_RemovePID(BYTE index, Pid pid);

#endif

