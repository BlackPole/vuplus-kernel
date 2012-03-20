#include "a867_demodulatorextend.h"
/*
#include "i2cimpl.h"
#include "spiimpl.h"
#include "i2uimpl.h"
#include "i2u.h"
#include "sdioimpl.h"
*/

#include "a867_usb2impl.h"
#include "a867_cmd.h"

#include "a867_Maxlinear_MXL5007.h"
#include "a867_Afa_AF9007.h"

static PidInfo pidInfo;
BusDescription busDesc[] =
{
    /** 0: NULL bus */
    {
        NULL,
        NULL,
        NULL,
        NULL,
    },
    /** 1: I2C bus */
    {
        NULL,
        NULL,
        NULL,
        NULL,
    },
    /** 2: USB bus */
    {
        Usb2_getDriver,
        Usb2_writeControlBus,
        Usb2_readControlBus,
        Usb2_readDataBus,
    },
    /** 3: SPI bus */
    {
        NULL,
        NULL,
        NULL,
        NULL,
    },
    /** 4: SDIO bus */
    {
        NULL,
        NULL,
        NULL,
        NULL,
    },
    /** 5: USB11 bus */
    {
        Usb2_getDriver,
        Usb2_writeControlBus,
        Usb2_readControlBus,
        Usb2_readDataBus,
    },
    /** 6: I2C for old mail box */
    {
        NULL,
        NULL,
        NULL,
        NULL,
    },
    /** 7: USB for old mail box */
    {
        NULL,
        NULL,
        NULL,
        NULL,
    },
};

CmdDescription cmdDesc[] =
{
    /** NULL Bus */
    {
        0,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
    },
    /** 1:I2C Bus */
    {
        255,
        &busDesc[1],
        Cmd_writeRegisters,
        Cmd_writeScatterRegisters,
        Cmd_writeTunerRegisters,
        Cmd_writeEepromValues,
        Cmd_readRegisters,
        Cmd_readScatterRegisters,
        Cmd_readTunerRegisters,
        Cmd_readEepromValues,
        Cmd_modifyRegister,
        Cmd_loadFirmware,
        Cmd_reboot,
        Cmd_sendCommand,
        Cmd_receiveData
    },
    /** 2:USB Bus */
    {
        63,
        &busDesc[2],
        Cmd_writeRegisters,
        Cmd_writeScatterRegisters,
        Cmd_writeTunerRegisters,
        Cmd_writeEepromValues,
        Cmd_readRegisters,
        Cmd_readScatterRegisters,
        Cmd_readTunerRegisters,
        Cmd_readEepromValues,
        Cmd_modifyRegister,
        Cmd_loadFirmware,
        Cmd_reboot,
        Cmd_sendCommand,
        Cmd_receiveData
    },
    /** 3:SPI Bus */
    {
        255,
        &busDesc[3],
        Cmd_writeRegisters,
        Cmd_writeScatterRegisters,
        Cmd_writeTunerRegisters,
        Cmd_writeEepromValues,
        Cmd_readRegisters,
        Cmd_readScatterRegisters,
        Cmd_readTunerRegisters,
        Cmd_readEepromValues,
        Cmd_modifyRegister,
        Cmd_loadFirmware,
        Cmd_reboot,
        Cmd_sendCommand,
        Cmd_receiveData
    },
    /** 4:SDIO Bus */
    {
        255,
        &busDesc[4],
        Cmd_writeRegisters,
        Cmd_writeScatterRegisters,
        Cmd_writeTunerRegisters,
        Cmd_writeEepromValues,
        Cmd_readRegisters,
        Cmd_readScatterRegisters,
        Cmd_readTunerRegisters,
        Cmd_readEepromValues,
        Cmd_modifyRegister,
        Cmd_loadFirmware,
        Cmd_reboot,
        Cmd_sendCommand,
        Cmd_receiveData
    },
    /** 5:USB11 Bus */
    {
        63,
        &busDesc[5],
        Cmd_writeRegisters,
        Cmd_writeScatterRegisters,
        Cmd_writeTunerRegisters,
        Cmd_writeEepromValues,
        Cmd_readRegisters,
        Cmd_readScatterRegisters,
        Cmd_readTunerRegisters,
        Cmd_readEepromValues,
        Cmd_modifyRegister,
        Cmd_loadFirmware,
        Cmd_reboot,
        Cmd_sendCommand,
        Cmd_receiveData
    },
    /** 6:I2C for old mailbox */
    {
        16,
        &busDesc[6],
        NULL,
	    NULL,
	    NULL,
	    NULL,
	    NULL,
	    NULL,
	    NULL,
	    NULL,
	    NULL,
	    NULL,
	    NULL,
	    NULL,
	    NULL
    },
    /** 7:USB for old mailbox */
    {
        16,
        &busDesc[7],
        NULL,
	    NULL,
	    NULL,
	    NULL,
	    NULL,
	    NULL,
	    NULL,
	    NULL,
	    NULL,
	    NULL,
	    NULL,
	    NULL,
	    NULL
    },
};

Dword Demodulator_setBusTuner (
    IN  Demodulator*    demodulator,
    IN  Word            busId,
    IN  Word            tunerId
) {
    Dword error = Error_NO_ERROR;

    Ganymede* ganymede;
    ganymede = (Ganymede*) demodulator;
    ganymede->cmdDescription = &cmdDesc[busId];
    ganymede->busId = busId;

    switch(tunerId) {
    case Tuner_Afatech_AF9007:
	ganymede->tunerDescription = &tuner_AF9007;
	break;

    case Tuner_Maxlinear_MXL5007:
	ganymede->tunerDescription = &tuner_MXL5007;
	break;

    default:
	error = Error_INVALID_TUNER_TYPE;
	goto exit;
	break;
    }

    if (ganymede->tunerDescription->tunerScript == NULL) {
        ganymede->tunerDescription->tunerScript = NULL;
        ganymede->tunerDescription->tunerScriptSets = NULL;
    }

exit:
    return(error);
}

Dword Demodulator_getChannelStatistic (
    IN  Demodulator*            demodulator,
    IN  Byte                    chip,
    OUT ChannelStatistic*       channelStatistic
) {
    Dword error = Error_NO_ERROR;

#if User_USE_DRIVER
    DWORD number;
    BOOL result;
    GetChannelStatisticRequest request;
    Ganymede* ganymede;

    ganymede = (Ganymede*) demodulator;

    if (ganymede->driver != NULL) {
        request.chip = chip;
        request.channelStatistic = channelStatistic;
        result = DeviceIoControl (
                    ganymede->driver,
                    IOCTL_AFA_DEMOD_GETCHANNELSTATISTIC,
                    &request,
                    sizeof (request),
                    NULL,
                    0,
                    &number,
                    NULL
        );
        error = request.error;
    } else {
        error = Error_DRIVER_INVALID;
    }
#else
    Dword postErrCnt;
    Dword postBitCnt;
    Word rsdAbortCnt;
    Ganymede* ganymede;


    ganymede = (Ganymede*) demodulator;


    /** Get BER if couter is ready, error = Error_RSD_COUNTER_NOT_READY if counter is not ready */
    if (ganymede->architecture == Architecture_PIP) {
        error = Standard_getPostVitBer (demodulator, chip, &postErrCnt, &postBitCnt, &rsdAbortCnt);
        if (error == Error_NO_ERROR) {
            ganymede->channelStatistic[chip].postVitErrorCount = postErrCnt;
            ganymede->channelStatistic[chip].postVitBitCount = postBitCnt;
            ganymede->channelStatistic[chip].abortCount = rsdAbortCnt;
        }
    } else {
        error = Standard_getPostVitBer (demodulator, 0, &postErrCnt, &postBitCnt, &rsdAbortCnt);
        if (error == Error_NO_ERROR) {
            ganymede->channelStatistic[chip].postVitErrorCount = postErrCnt;
            ganymede->channelStatistic[chip].postVitBitCount = postBitCnt;
            ganymede->channelStatistic[chip].abortCount = rsdAbortCnt;
        }
    }

    *channelStatistic = ganymede->channelStatistic[chip];

#endif

    return (error);
}
Dword Demodulator_addPid (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Pid             pid
) {
    Dword error = Error_NO_ERROR;

#if User_USE_DRIVER
    DWORD number;
    BOOL result;
    AddPidRequest request;
    Ganymede* ganymede;

    ganymede = (Ganymede*) demodulator;

    if (ganymede->driver != NULL) {
        request.chip = chip;
        request.pid = pid;
        result = DeviceIoControl (
                    ganymede->driver,
                    IOCTL_AFA_DEMOD_ADDPID,
                    &request,
                    sizeof (request),
                    NULL,
                    0,
                    &number,
                    NULL
        );
        error = request.error;
    } else {
        error = Error_DRIVER_INVALID;
    }
#else
    Byte writeBuffer[2];
    Byte i, j;
    Bool found;
    Ganymede* ganymede;

    ganymede = (Ganymede*) demodulator;

    if (pidInfo.pidinit == False) {
        for (i = 0; i < ganymede->chipNumber; i++) {
            for (j = 0; j < 32; j++) {
                pidInfo.pidtable[i].pid[j] = 0xFFFF;
            }
        }
        pidInfo.pidinit = True;
    }

    /** Enable pid filter */
    if (pidInfo.pidcount == 0) {
        error = Standard_writeRegisterBits (demodulator, chip, Processor_OFDM, p_mp2if_pid_en, mp2if_pid_en_pos, mp2if_pid_en_len, 1);
        if (error) goto exit;
    } else {
        found = False;
        for (i = 0; i < 32; i++) {
            if (pidInfo.pidtable[chip].pid[i] == pid.value) {
                found = True;
                break;
            }
        }
        if (found == True)
            goto exit;
    }

    for (i = 0; i < 32; i++) {
        if (pidInfo.pidtable[chip].pid[i] == 0xFFFF)
            break;
    }
    if (i == 32) {
        error = Error_PID_FILTER_FULL;
        goto exit;
    }

    writeBuffer[0] = (Byte) pid.value;
    writeBuffer[1] = (Byte) (pid.value >> 8);

    error = Standard_writeRegisters (demodulator, chip, Processor_OFDM, p_mp2if_pid_dat_l, 2, writeBuffer);
    if (error) goto exit;

    error = Standard_writeRegisterBits (demodulator, chip, Processor_OFDM, p_mp2if_pid_index_en, mp2if_pid_index_en_pos, mp2if_pid_index_en_len, 1);
    if (error) goto exit;

    error = Standard_writeRegister (demodulator, chip, Processor_OFDM, p_mp2if_pid_index, i);
    if (error) goto exit;

    pidInfo.pidtable[chip].pid[i] = pid.value;
    pidInfo.pidcount++;

exit :
#endif

    return (error);
}


Dword Demodulator_addPidAt (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Byte            index,
    IN  Pid             pid
) {
	return (Demodulator_addPidToFilter (demodulator, chip, index, pid));
}


Dword Demodulator_removePid (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Pid             pid
) {
    Dword error = Error_NO_ERROR;

#if User_USE_DRIVER
    DWORD number;
    BOOL result;
    RemovePidRequest request;
    Ganymede* ganymede;

    ganymede = (Ganymede*) demodulator;

    if (ganymede->driver != NULL) {
        request.chip = chip;
        request.pid = pid;
        result = DeviceIoControl (
                    ganymede->driver,
                    IOCTL_AFA_DEMOD_REMOVEPID,
                    &request,
                    sizeof (request),
                    NULL,
                    0,
                    &number,
                    NULL
        );
        error = request.error;
    } else {
        error = Error_DRIVER_INVALID;
    }
#else
    Byte i;
    Bool found;
    Interrupts interrupts;
    Ganymede* ganymede;

    ganymede = (Ganymede*) demodulator;

    found = False;
    for (i = 0; i < 32; i++) {
        if (pidInfo.pidtable[chip].pid[i] == pid.value) {
            found = True;
            break;
        }
    }
    if (found == False)
        goto exit;

    error = Standard_writeRegisterBits (demodulator, chip, Processor_OFDM, p_mp2if_pid_index_en, mp2if_pid_index_en_pos, mp2if_pid_index_en_len, 0);
    if (error) goto exit;

    error = Standard_writeRegister (demodulator, chip, Processor_OFDM, p_mp2if_pid_index, i);
    if (error) goto exit;

    pidInfo.pidtable[chip].pid[i] = 0xFFFF;

    /** Disable pid filter */
    if (pidInfo.pidcount == 1) {
        error = Standard_writeRegisterBits (demodulator, chip, Processor_OFDM, p_mp2if_pid_en, mp2if_pid_en_pos, mp2if_pid_en_len, 0);

        error = Standard_getInterrupts (demodulator, &interrupts);
        if (error) goto exit;
        if (interrupts & Interrupt_DVBT) {
            error = Standard_clearInterrupt (demodulator, Interrupt_DVBT);
            if (error) goto exit;
        }
    }

    pidInfo.pidcount--;

exit :
#endif

    return (error);
}


Dword Demodulator_removePidAt (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
	IN  Byte			index,
    IN  Pid             pid
) {
    Dword error = Error_NO_ERROR;

#if User_USE_DRIVER
	DWORD number;
	BOOL result;
	RemovePidAtRequest request;
	Ganymede* ganymede;

	ganymede = (Ganymede*) demodulator;
	
	if (ganymede->driver != NULL) {
		request.chip = chip;
		request.index = index;
		request.pid = pid;
		result = DeviceIoControl (
					ganymede->driver,
					IOCTL_AFA_DEMOD_REMOVEPIDAT,
					&request,
					sizeof (request),
					NULL,
					0,
					&number,
					NULL
		);
		error = request.error;
	} else {
		error = Error_DRIVER_INVALID;
	}
#else
	Ganymede* ganymede;

	ganymede = (Ganymede*) demodulator;

	error = Standard_writeRegisterBits (demodulator, chip, Processor_OFDM, p_mp2if_pid_index_en, mp2if_pid_index_en_pos, mp2if_pid_index_en_len, 0);
	if (error) goto exit;

	error = Standard_writeRegister (demodulator, chip, Processor_OFDM, p_mp2if_pid_index, index);
	if (error) goto exit;
exit :
#endif

	return (error);
}


Dword Demodulator_resetPid (
    IN  Demodulator*    demodulator,
    IN  Byte            chip
) {
    Dword error = Error_NO_ERROR;

#if User_USE_DRIVER
#else
    Byte i;
    Ganymede* ganymede;

    ganymede = (Ganymede*) demodulator;

    for (i = 0; i < 32; i++) {
        pidInfo.pidtable[chip].pid[i] = 0xFFFF;
    }
    error = Standard_writeRegisterBits (demodulator, chip, Processor_OFDM, p_mp2if_pid_rst, mp2if_pid_rst_pos, mp2if_pid_rst_len, 1);
    if (error) goto exit;

    pidInfo.pidcount = 0;

exit :
#endif

    return (error);
}


#ifdef UNDER_CE
#else
extern long ActiveSync;
#endif

Dword Demodulator_controlActiveSync (
    IN Demodulator* demodulator,
    IN Byte         control
) {
#ifdef UNDER_CE
    if (control == 0)
        ActiveSync = 0;
    else
        ActiveSync = 1;
#endif

    return (Error_NO_ERROR);
}
