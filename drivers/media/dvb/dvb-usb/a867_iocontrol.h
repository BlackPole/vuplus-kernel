/**
 *
 * Copyright (c) 2006 Afa Corporation. All rights reserved.
 *
 * Module Name:
 *   iocontrol.h
 *
 * Abstract:
 *   The structure and IO code for IO control call.
 *
 */

#ifndef __IOCONTROL_H__
#define __IOCONTROL_H__

#ifdef UNDER_CE
#include <winioctl.h>
#else
#endif

#include "a867_type.h"

#define MEDIA_DEVICE_AFADEMOD       0x00000AFA

typedef struct {
    Byte                chipNumber;
    Word                sawBandwidth;
    StreamType          streamType;
    Architecture        architecture;
    Dword               error;
    Byte                reserved[16];
} InitializeRequest, *PInitializeRequest;

typedef struct {
    Dword               error;
    Byte                reserved[16];
} FinalizeRequest, *PFinalizeRequest;

typedef struct {
    StreamType          streamType;
    Dword               error;
    Byte                reserved[16];
} SetStreamTypeRequest, *PSetStreamTypeRequest;

typedef struct {
    Architecture        architecture;
    Dword               error;
    Byte                reserved[16];
} SetArchitectureRequest, *PSetArchitectureRequest;

typedef struct {
    Byte                chip;
    ChannelModulation*  channelModulation;
    Dword               error;
    Byte                reserved[16];
} GetChannelModulationRequest, *PGetChannelModulationRequest;

typedef struct {
    Processor           processor;
    Dword*              version;
    Dword               error;
    Byte                reserved[16];
} GetFirmwareVersionRequest, *PGetFirmwareVersionRequest;

typedef struct {
    Byte                chip;
    Word                bandwidth;
    Dword               frequency;
    Dword               error;
    Byte                reserved[16];
} AcquireChannelRequest, *PAcquireChannelRequest;

typedef struct {
    Byte                chip;
    Bool*               locked;
    Dword               error;
    Byte                reserved[16];
} IsLockedRequest, *PIsLockedRequest;

typedef struct {
    Byte                chip;
    Pid                 pid;
    Dword               error;
    Byte                reserved[16];
} AddPidRequest, *PAddPidRequest;

typedef struct {
    Byte                chip;
    Pid                 pid;
    Dword               error;
    Byte                reserved[16];
} RemovePidRequest, *PRemovePidRequest;

typedef struct {
    Byte            chip;
    Dword           error;
    Byte            reserved[16];
} ResetPidRequest, *PResetPidRequest;

typedef struct {
    Byte                chip;
    Byte                superFrameCount;
    Word                packetUnit;
    Dword               error;
    Byte                reserved[16];
} SetStatisticRangeRequest, *PSetStatisticRangeRequest;

typedef struct {
    Byte                chip;
    Byte*               superFrameCount;
    Word*               packetUnit;
    Dword               error;
    Byte                reserved[16];
} GetStatisticRangeRequest, *PGetStatisticRangeRequest;

typedef struct {
    Byte                chip;
    ChannelStatistic*   channelStatistic;
    Dword               error;
    Byte                reserved[16];
} GetChannelStatisticRequest, *PGetChannelStatisticRequest;

typedef struct {
    Byte                chip;
    Statistic*          statistic;
    Dword               error;
    Byte                reserved[16];
} GetStatisticRequest, *PGetStatisticRequest;

typedef struct {
    Dword*              bufferLength;
    Byte*               buffer;
    Dword               error;
    Byte                reserved[16];
} GetDatagramRequest, *PGetDatagramRequest;

typedef struct {
    Byte            chip;
    Byte            control;
    Dword           error;
    Byte            reserved[16];
} ControlPidFilterRequest, *PControlPidFilterRequest;

typedef struct {
    Byte                chip;
    Byte                control;
    Dword               error;
    Byte                reserved[16];
} ControlPowerSavingRequest, *PControlPowerSavingRequest;

typedef struct {
    Byte                DriverVerion[16];   /** XX.XX.XX.XX Ex., 1.2.3.4            */
    Byte                APIVerion[32];      /** XX.XX.XXXXXXXX.XX Ex., 1.2.3.4  */
    Byte                FWVerionLink[16];   /** XX.XX.XX.XX Ex., 1.2.3.4            */
    Byte                FWVerionOFDM[16];   /** XX.XX.XX.XX Ex., 1.2.3.4            */
    Byte                DateTime[24];       /** Ex.,"2004-12-20 18:30:00" or "DEC 20 2004 10:22:10" with compiler __DATE__ and __TIME__  definitions */
    Byte                Company[8];         /** Ex.,"Afatech"                   */
    Byte                SupportHWInfo[32];  /** Ex.,"Jupiter DVBT/DVBH"         */
    Dword               error;
    Byte                reserved[128];
} DemodDriverInfo, *PDemodDriverInfo;

typedef struct {
    Ensemble*           ensemble;
    Dword               error;
    Byte                reserved[16];
} AcquireEnsembleRequest, *PAcquireEnsembleRequest;

typedef struct {
    Byte*               serviceLength;
    Service*            services;
    Dword               error;
    Byte                reserved[16];
} AcquireServiceRequest, *PAcquireServiceRequest;

typedef struct {
    Service             service;
    Byte*               componentLength;
    Component*          components;
    Dword               error;
    Byte                reserved[16];
} AcquireComponentRequest, *PAcquireComponentRequest;

typedef struct {
    Component           component;
    Dword               error;
    Byte                reserved[16];
} AddComponentRequest, *PAddComponentRequest;

typedef struct {
    Component           component;
    Dword               error;
    Byte                reserved[16];
} RemoveComponentRequest, *PRemoveComponentRequest;

typedef struct {
    Byte                figType;
    Byte                figExtension;
    Dword               error;
    Byte                reserved[16];
} AddFigRequest, *PAddFigRequest;

typedef struct {
    Byte                figType;
    Byte                figExtension;
    Dword               error;
    Byte                reserved[16];
} RemoveFigRequest, *PRemoveFigRequest;


/**
 * Demodulator API commands
 */

/**
 * First, download firmware from host to demodulator. Actually, firmware is
 * put in firmware.h as a part of source code. Therefore, in order to
 * update firmware the host have to re-compile the source code.
 * Second, setting all parameters which will be need at the beginning.
 * Paramters: None
 */
#define IOCTL_AFA_DEMOD_INITIALIZE \
    CTL_CODE( MEDIA_DEVICE_AFADEMOD, 0x003, METHOD_BUFFERED, FILE_ANY_ACCESS )

/**
 * Set the output stream type of chip. Because the device could output in
 * many stream type, therefore host have to choose one type before receive
 * data.
 * Paramters:   DemodStreamType struct
 */
#define IOCTL_AFA_DEMOD_SETSTREAMTYPE \
    CTL_CODE( MEDIA_DEVICE_AFADEMOD, 0x004, METHOD_BUFFERED, FILE_ANY_ACCESS )

/**
 * Set the output stream type of chip. Because the device could output in
 * many stream type, therefore host have to choose one type before receive
 * data.
 * Paramters:   DemodStreamType struct
 */
#define IOCTL_AFA_DEMOD_SETARCHITECTURE \
    CTL_CODE( MEDIA_DEVICE_AFADEMOD, 0x006, METHOD_BUFFERED, FILE_ANY_ACCESS )

/**
 * Set the output stream type of chip. Because the device could output in
 * many stream type, therefore host have to choose one type before receive
 * data.
 * Paramters:   DemodStreamType struct
 */
#define IOCTL_AFA_DEMOD_GETCHANNELMODULATION \
    CTL_CODE( MEDIA_DEVICE_AFADEMOD, 0x008, METHOD_BUFFERED, FILE_ANY_ACCESS )

/**
 * Set the output stream type of chip. Because the device could output in
 * many stream type, therefore host have to choose one type before receive
 * data.
 * Paramters:   DemodStreamType struct
 */
#define IOCTL_AFA_DEMOD_GETFIRMWAREVERSION \
    CTL_CODE( MEDIA_DEVICE_AFADEMOD, 0x009, METHOD_BUFFERED, FILE_ANY_ACCESS )

/**
 * Specify the bandwidth of channel and tune the channel to the specific
 * frequency. Afterwards, host could use output parameter dvbH to determine
 * if there is a DVB-H signal.
 * In DVB-T mode, after calling this function output parameter dvbH should
 * be False and host could use output parameter "locked" to indicate if the
 * TS is correct.
 * In DVB-H mode, after calling this function output parameter dvbH should
 * be True and host could use Jupiter_acquirePlatorm to get platform.
 * Paramters:   DemodAcqCh struct
 */
#define IOCTL_AFA_DEMOD_ACQUIRECHANNEL \
    CTL_CODE( MEDIA_DEVICE_AFADEMOD, 0x00A, METHOD_BUFFERED, FILE_ANY_ACCESS )

/**
 * Get all the platforms found in current frequency.
 * Paramters:   DemodAcqPlatform struct
 */
#define IOCTL_AFA_DEMOD_ISLOCKED \
    CTL_CODE( MEDIA_DEVICE_AFADEMOD, 0x00B, METHOD_BUFFERED, FILE_ANY_ACCESS )

/**
 * Get all the platforms found in current frequency.
 * Paramters:   DemodAcqPlatform struct
 */
#define IOCTL_AFA_DEMOD_ACQUIREPLATFORM \
    CTL_CODE( MEDIA_DEVICE_AFADEMOD, 0x00C, METHOD_BUFFERED, FILE_ANY_ACCESS )

/**
 * Change the current platform as the specified platform. If the target
 * platform is locate in different frequency, this function will tune to
 * that frequency before setting platform.
 * Paramters:   DemodSetPlatform struct
 */
#define IOCTL_AFA_DEMOD_SETPLATFORM \
    CTL_CODE( MEDIA_DEVICE_AFADEMOD, 0x00D, METHOD_BUFFERED, FILE_ANY_ACCESS )

/**
 * Add IP to IP filter.
 * Paramters:   DemodIp struct
 */
#define IOCTL_AFA_DEMOD_ADDIP \
    CTL_CODE( MEDIA_DEVICE_AFADEMOD, 0x00E, METHOD_BUFFERED, FILE_ANY_ACCESS )

/**
 * Remove IP from IP filter.
 * Paramters:   DemodIp struct
 */
#define IOCTL_AFA_DEMOD_REMOVEIP \
    CTL_CODE( MEDIA_DEVICE_AFADEMOD, 0x00F, METHOD_BUFFERED, FILE_ANY_ACCESS )

/**
 * Add PID to PID filter.
 * Paramters:   DemodPid struct
 */
#define IOCTL_AFA_DEMOD_ADDPID \
    CTL_CODE( MEDIA_DEVICE_AFADEMOD, 0x010, METHOD_BUFFERED, FILE_ANY_ACCESS )

/**
 * Remove PID from PID filter.
 * Paramters:   DemodPid struct
 */
#define IOCTL_AFA_DEMOD_REMOVEPID \
    CTL_CODE( MEDIA_DEVICE_AFADEMOD, 0x011, METHOD_BUFFERED, FILE_ANY_ACCESS )

/**
 * Reset PID from PID filter.
 * Paramters:   ResetPidRequest struct
 */
#define IOCTL_AFA_DEMOD_RESETPID \
    CTL_CODE( MEDIA_DEVICE_AFADEMOD, 0x012, METHOD_BUFFERED, FILE_ANY_ACCESS )

/**
 * Get data of one single section
 * Paramters:   DemodGetStat struct
 */
#define IOCTL_AFA_DEMOD_GETSECTION \
    CTL_CODE( MEDIA_DEVICE_AFADEMOD, 0x013, METHOD_BUFFERED, FILE_ANY_ACCESS )

/**
 * Get the statistic values of demodulator, it includes Pre-Viterbi BER,
 * Post-Viterbi BER, Abort Count, Signal Presented Flag, Signal Locked Flag,
 * Signal Quality, Signal Strength, Delta-T for DVB-H time slicing.
 * Paramters:   DemodGetStat struct
 */
#define IOCTL_AFA_DEMOD_SETSTATISTICRANGE \
    CTL_CODE( MEDIA_DEVICE_AFADEMOD, 0x014, METHOD_BUFFERED, FILE_ANY_ACCESS )

/**
 * Get the statistic values of demodulator, it includes Pre-Viterbi BER,
 * Post-Viterbi BER, Abort Count, Signal Presented Flag, Signal Locked Flag,
 * Signal Quality, Signal Strength, Delta-T for DVB-H time slicing.
 * Paramters:   DemodGetStat struct
 */
#define IOCTL_AFA_DEMOD_GETSTATISTICRANGE \
    CTL_CODE( MEDIA_DEVICE_AFADEMOD, 0x015, METHOD_BUFFERED, FILE_ANY_ACCESS )

/**
 * Get the statistic values of demodulator, it includes Pre-Viterbi BER,
 * Post-Viterbi BER, Abort Count, Signal Presented Flag, Signal Locked Flag,
 * Signal Quality, Signal Strength, Delta-T for DVB-H time slicing.
 * Paramters:   DemodGetStat struct
 */
#define IOCTL_AFA_DEMOD_GETCHANNELSTATISTIC \
    CTL_CODE( MEDIA_DEVICE_AFADEMOD, 0x016, METHOD_BUFFERED, FILE_ANY_ACCESS )

/**
 * Get the statistic values of demodulator, it includes Pre-Viterbi BER,
 * Post-Viterbi BER, Abort Count, Signal Presented Flag, Signal Locked Flag,
 * Signal Quality, Signal Strength, Delta-T for DVB-H time slicing.
 * Paramters:   DemodGetStat struct
 */
#define IOCTL_AFA_DEMOD_GETSTATISTIC \
    CTL_CODE( MEDIA_DEVICE_AFADEMOD, 0x017, METHOD_BUFFERED, FILE_ANY_ACCESS )

/**
 * Get the statistic values of demodulator, it includes Pre-Viterbi BER,
 * Post-Viterbi BER, Abort Count, Signal Presented Flag, Signal Locked Flag,
 * Signal Quality, Signal Strength, Delta-T for DVB-H time slicing.
 * Paramters:   DemodGetStat struct
 */
#define IOCTL_AFA_DEMOD_GETINTERRUPTS \
    CTL_CODE( MEDIA_DEVICE_AFADEMOD, 0x018, METHOD_BUFFERED, FILE_ANY_ACCESS )

/**
 * Get the statistic values of demodulator, it includes Pre-Viterbi BER,
 * Post-Viterbi BER, Abort Count, Signal Presented Flag, Signal Locked Flag,
 * Signal Quality, Signal Strength, Delta-T for DVB-H time slicing.
 * Paramters:   DemodGetStat struct
 */
#define IOCTL_AFA_DEMOD_CLEARINTERRUPT \
    CTL_CODE( MEDIA_DEVICE_AFADEMOD, 0x019, METHOD_BUFFERED, FILE_ANY_ACCESS )

/**
 * Get the statistic values of demodulator, it includes Pre-Viterbi BER,
 * Post-Viterbi BER, Abort Count, Signal Presented Flag, Signal Locked Flag,
 * Signal Quality, Signal Strength, Delta-T for DVB-H time slicing.
 * Paramters:   DemodGetStat struct
 */
#define IOCTL_AFA_DEMOD_GETDATAGRAM \
    CTL_CODE( MEDIA_DEVICE_AFADEMOD, 0x01A, METHOD_BUFFERED, FILE_ANY_ACCESS )

/**
 *
 * Paramters:   DemodControl struct
 */
#define IOCTL_AFA_DEMOD_GETDELTAT \
    CTL_CODE( MEDIA_DEVICE_AFADEMOD, 0x01B, METHOD_BUFFERED, FILE_ANY_ACCESS )

/**
 * Enable PID filter.
 * Paramters:   EnablePidRequest struct
 */
#define IOCTL_AFA_DEMOD_CONTROLPIDFILTER \
    CTL_CODE( MEDIA_DEVICE_AFADEMOD, 0x01C, METHOD_BUFFERED, FILE_ANY_ACCESS )

/**
 *
 * Paramters:   DemodControl struct
 */
#define IOCTL_AFA_DEMOD_CONTROLTIMESLICING \
    CTL_CODE( MEDIA_DEVICE_AFADEMOD, 0x01D, METHOD_BUFFERED, FILE_ANY_ACCESS )

/**
 *
 * Paramters:   DemodControl struct
 */
#define IOCTL_AFA_DEMOD_CONTROLPOWERSAVING \
    CTL_CODE( MEDIA_DEVICE_AFADEMOD, 0x01E, METHOD_BUFFERED, FILE_ANY_ACCESS )

/**
 * Power off the demodulators.
 * Paramters:   None
 */
#define IOCTL_AFA_DEMOD_FINALIZE \
    CTL_CODE( MEDIA_DEVICE_AFADEMOD, 0x01F, METHOD_BUFFERED, FILE_ANY_ACCESS )

/**
 * Get driver information.
 * Paramters:   DemodDriverInfo struct
 */
#define IOCTL_AFA_DEMOD_GETDRIVERINFO \
    CTL_CODE( MEDIA_DEVICE_AFADEMOD, 0x020, METHOD_BUFFERED, FILE_ANY_ACCESS )

/**
 * Get ensemble.
 * Paramters:   DemodDriverInfo struct
 */
#define IOCTL_AFA_DEMOD_ACQUIREENSEMBLE \
    CTL_CODE( MEDIA_DEVICE_AFADEMOD, 0x021, METHOD_BUFFERED, FILE_ANY_ACCESS )

/**
 * Get service.
 * Paramters:   DemodDriverInfo struct
 */
#define IOCTL_AFA_DEMOD_ACQUIRESERVICE \
    CTL_CODE( MEDIA_DEVICE_AFADEMOD, 0x022, METHOD_BUFFERED, FILE_ANY_ACCESS )

/**
 * Get component.
 * Paramters:   DemodDriverInfo struct
 */
#define IOCTL_AFA_DEMOD_ACQUIRECOMPONENT \
    CTL_CODE( MEDIA_DEVICE_AFADEMOD, 0x023, METHOD_BUFFERED, FILE_ANY_ACCESS )

/**
 * Add component.
 * Paramters:   DemodDriverInfo struct
 */
#define IOCTL_AFA_DEMOD_ADDCOMPONENT \
    CTL_CODE( MEDIA_DEVICE_AFADEMOD, 0x024, METHOD_BUFFERED, FILE_ANY_ACCESS )

/**
 * Remove component.
 * Paramters:   DemodDriverInfo struct
 */
#define IOCTL_AFA_DEMOD_REMOVECOMPONENT \
    CTL_CODE( MEDIA_DEVICE_AFADEMOD, 0x025, METHOD_BUFFERED, FILE_ANY_ACCESS )

/**
 * Add FIG.
 * Paramters:   DemodDriverInfo struct
 */
#define IOCTL_AFA_DEMOD_ADDFIG \
    CTL_CODE( MEDIA_DEVICE_AFADEMOD, 0x026, METHOD_BUFFERED, FILE_ANY_ACCESS )

/**
 * Remove FIG.
 * Paramters:   DemodDriverInfo struct
 */
#define IOCTL_AFA_DEMOD_REMOVEFIG \
    CTL_CODE( MEDIA_DEVICE_AFADEMOD, 0x027, METHOD_BUFFERED, FILE_ANY_ACCESS )


/**
 * Demodulator Misc API commands
 */

typedef struct {
    Byte            chip;
    Processor       processor;
    Dword           registerAddress;
    Byte            bufferLength;
    Byte            buffer[256];
    Dword           error;
    Byte            reserved[16];
} WriteRegistersRequest, *PWriteRegistersRequest;

typedef struct {
    Byte            chip;
    Word            registerAddress;
    Byte            bufferLength;
    Byte            buffer[256];
    Dword           error;
    Byte            reserved[16];
} WriteTunerRegistersRequest, *PWriteTunerRegistersRequest;

typedef struct {
    Byte            chip;
    Word            registerAddress;
    Byte            bufferLength;
    Byte            buffer[256];
    Dword           error;
    Byte            reserved[16];
} WriteEepromValuesRequest, *PWriteEepromValuesRequest;

typedef struct {
    Byte            chip;
    Processor       processor;
    Dword           registerAddress;
    Byte            position;
    Byte            length;
    Byte            value;
    Dword           error;
    Byte            reserved[16];
} WriteRegisterBitsRequest, *PWriteRegisterBitsRequest;

typedef struct {
    Byte            chip;
    Processor       processor;
    Word            variableIndex;
    Byte            bufferLength;
    Byte            buffer[256];
    Dword           error;
    Byte            reserved[16];
} SetVariablesRequest, *PSetVariablesRequest;

typedef struct {
    Byte            chip;
    Processor       processor;
    Word            variableIndex;
    Byte            position;
    Byte            length;
    Byte            value;
    Dword           error;
    Byte            reserved[16];
} SetVariableBitsRequest, *PSetVariableBitsRequest;

typedef struct {
    Byte            chip;
    Processor       processor;
    Dword           registerAddress;
    Byte            bufferLength;
    Byte*           buffer;
    Dword           error;
    Byte            reserved[16];
} ReadRegistersRequest, *PReadRegistersRequest;

typedef struct {
    Byte            chip;
    Word            registerAddress;
    Byte            bufferLength;
    Byte*           buffer;
    Dword           error;
    Byte            reserved[16];
} ReadTunerRegistersRequest, *PReadTunerRegistersRequest;

typedef struct {
    Byte            chip;
    Word            registerAddress;
    Byte            bufferLength;
    Byte*           buffer;
    Dword           error;
    Byte            reserved[16];
} ReadEepromValuesRequest, *PReadEepromValuesRequest;

typedef struct {
    Byte            chip;
    Processor       processor;
    Dword           registerAddress;
    Byte            position;
    Byte            length;
    Byte*           value;
    Dword           error;
    Byte            reserved[16];
} ReadRegisterBitsRequest, *PReadRegisterBitsRequest;

typedef struct {
    Byte            chip;
    Processor       processor;
    Word            variableIndex;
    Byte            bufferLength;
    Byte*           buffer;
    Dword           error;
    Byte            reserved[16];
} GetVariablesRequest, *PGetVariablesRequest;

typedef struct {
    Byte            chip;
    Processor       processor;
    Word            variableIndex;
    Byte            position;
    Byte            length;
    Byte*           value;
    Dword           error;
    Byte            reserved[16];
} GetVariableBitsRequest, *PGetVariableBitsRequest;


/**
 * Write a sequence of bytes to the contiguous registers in demodulator.
 * Paramters:   DemodRegs struct
 */
#define IOCTL_AFA_DEMOD_WRITEREGISTERS \
    CTL_CODE( MEDIA_DEVICE_AFADEMOD, 0x101, METHOD_BUFFERED, FILE_ANY_ACCESS )

/**
 * Write a sequence of bytes to the contiguous registers in tuner.
 * Paramters:   DemodTunerRegs struct
 */
#define IOCTL_AFA_DEMOD_WRITETUNERREGISTERS \
    CTL_CODE( MEDIA_DEVICE_AFADEMOD, 0x102, METHOD_BUFFERED, FILE_ANY_ACCESS )

/**
 * Write a sequence of bytes to the contiguous cells in the EEPROM.
 * Paramters:   DemodEEPROMVaules struct
 */
#define IOCTL_AFA_DEMOD_WRITEEEPROMVALUES \
    CTL_CODE( MEDIA_DEVICE_AFADEMOD, 0x103, METHOD_BUFFERED, FILE_ANY_ACCESS )

/**
 * Write one byte to the contiguous registers in demodulator.
 * Paramters:   DemodRegs struct
 */
#define IOCTL_AFA_DEMOD_WRITEREGISTERBITS \
    CTL_CODE( MEDIA_DEVICE_AFADEMOD, 0x104, METHOD_BUFFERED, FILE_ANY_ACCESS )

/**
 * Write a sequence of bytes to the contiguous variables in demodulator.
 * Paramters:   DemodVariables struct
 */
#define IOCTL_AFA_DEMOD_SETVARIABLES \
    CTL_CODE( MEDIA_DEVICE_AFADEMOD, 0x105, METHOD_BUFFERED, FILE_ANY_ACCESS )

/**
 * Write a sequence of bytes to the contiguous variables in demodulator.
 * Paramters:   DemodVariables struct
 */
#define IOCTL_AFA_DEMOD_SETVARIABLEBITS \
    CTL_CODE( MEDIA_DEVICE_AFADEMOD, 0x106, METHOD_BUFFERED, FILE_ANY_ACCESS )

/**
 * Read a sequence of bytes from the contiguous registers in demodulator.
 * Paramters:   DemodRegs struct
 */
#define IOCTL_AFA_DEMOD_READREGISTERS \
    CTL_CODE( MEDIA_DEVICE_AFADEMOD, 0x108, METHOD_BUFFERED, FILE_ANY_ACCESS )

/**
 * Read a sequence of bytes from the contiguous registers in tuner.
 * Paramters:   DemodTunerRegs
 */
#define IOCTL_AFA_DEMOD_READTUNERREGISTERS \
    CTL_CODE( MEDIA_DEVICE_AFADEMOD, 0x109, METHOD_BUFFERED, FILE_ANY_ACCESS )

/**
 * Read a sequence of bytes from the contiguous cells in the EEPROM.
 * Paramters:   DemodEEPROMVaules struct
 */
#define IOCTL_AFA_DEMOD_READEEPROMVALUES \
    CTL_CODE( MEDIA_DEVICE_AFADEMOD, 0x10A, METHOD_BUFFERED, FILE_ANY_ACCESS )

/**
 * Read a sequence of bytes from the contiguous registers in demodulator.
 * Paramters:   DemodRegs struct
 */
#define IOCTL_AFA_DEMOD_READREGISTERBITS \
    CTL_CODE( MEDIA_DEVICE_AFADEMOD, 0x10B, METHOD_BUFFERED, FILE_ANY_ACCESS )

/**
 * Read a sequence of bytes from the contiguous variables in demodulator.
 * Paramters:   DemodVariables struct
 */
#define IOCTL_AFA_DEMOD_GETVARIABLES \
    CTL_CODE( MEDIA_DEVICE_AFADEMOD, 0x10C, METHOD_BUFFERED, FILE_ANY_ACCESS )

/**
 * Read a sequence of bytes from the contiguous variables in demodulator.
 * Paramters:   DemodVariables struct
 */
#define IOCTL_AFA_DEMOD_GETVARIABLEBITS \
    CTL_CODE( MEDIA_DEVICE_AFADEMOD, 0x10D, METHOD_BUFFERED, FILE_ANY_ACCESS )



/**
 * Demodulator Stream control API commands
 */

typedef struct {
    Byte            chip;
    Dword           error;
    Byte            reserved[16];
} StartCaptureRequest, *PStartCaptureRequest;

typedef struct {
    Byte            chip;
    Dword           error;
    Byte            reserved[16];
} StopCaptureRequest, *PStopCaptureRequest;

/**
 * Start capture data stream
 * Paramters: DemodDriverInfo struct
 */
#define IOCTL_AFA_DEMOD_STARTCAPTURE \
    CTL_CODE( MEDIA_DEVICE_AFADEMOD, 0x200, METHOD_BUFFERED, FILE_ANY_ACCESS )


/**
 * Stop capture data stream
 * Paramters:   DemodDriverInfo struct
 */
#define IOCTL_AFA_DEMOD_STOPCAPTURE \
    CTL_CODE( MEDIA_DEVICE_AFADEMOD, 0x201, METHOD_BUFFERED, FILE_ANY_ACCESS )


#endif
