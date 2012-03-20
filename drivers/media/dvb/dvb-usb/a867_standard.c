#include "a867_standard.h"
//#include <math.h>
#include "a867_cmd.h"
#include "a867_user.h"
#include "a867_af903x.h"

// turn on/off debug in this file
#define DEBUG_STANDARD	0

#if !DEBUG_STANDARD
	#undef deb_info
	#define deb_info(fmt, args...) do {} while(0)
#endif

#include "a867_firmware.h"

#define Standard_MAX_BIT                8
#define Standard_MAX_CLOCK              12
#define Standard_MAX_BAND               3


const Byte Standard_bitMask[Standard_MAX_BIT] = {
    0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF
};

const ClockTable Standard_clockTable[Standard_MAX_CLOCK] =
{
    {   20480,      20480000    },      /** FPGA     */
    {   16384,      20480000    },      /** 16.38MHz */
    {   20480,      20480000    },      /** 20.48MHz */
    {   36000,      20250000    },      /** 36.00MHz */
    {   30000,      20156250    },      /** 30.00MHz */
    {   26000,      20583333    },      /** 26.00MHz */
    {   28000,      20416667    },      /** 28.00MHz */
    {   32000,      20500000    },      /** 32.00MHz */
    {   34000,      20187500    },      /** 34.00MHz */
    {   24000,      20500000    },      /** 24.00MHz */
    {   22000,      20625000    },      /** 22.00MHz */
    {   12000,      20250000    }       /** 12.00MHz */
};

const BandTable Standard_bandTable[Standard_MAX_BAND] =
{
    {    174000,     230000     },      /** VHF    */
    {    350000,     900000     },      /** UHF    */
    {   1670000,    1680000     }       /** L-BAND */
};

#if User_USE_DRIVER
Dword Standard_getDriver (
    IN  Demodulator*    demodulator,
    OUT Handle*         handle
) {
    Dword error = Error_NO_ERROR;
    TCHAR registry1[100] = TEXT("\\Drivers\\SDCARD\\ClientDrivers\\Custom\\MANF-0296-CARDID-5347-FUNC-1");
    TCHAR registry2[100] = TEXT("\\Drivers\\SDCARD\\ClientDrivers\\Custom\\MANF-03BE-CARDID-0001-FUNC-1");
    TCHAR name[256];
    TCHAR shortBuffer[32];
    DWORD len = 16;
    DWORD i;
    HKEY hKey, hSubKey;
    DWORD size;

    /** Open the HKLM\Drivers\Active key in registry */
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("Drivers\\Active"), 0, 0, &hKey) == ERROR_SUCCESS) {
        /** Get subkeys count */
        if (RegQueryInfoKey(hKey, NULL, NULL, NULL, &i, NULL, NULL, NULL, NULL, NULL, NULL, NULL) == ERROR_SUCCESS) {
            /** Browse subkeys in reverse order (pluggable cards are not at the beginning of the list !) */
            while (i) {
                i--;
                /** Select the subkey */
                size = sizeof(shortBuffer);
                if (RegEnumKeyEx(hKey, i, shortBuffer, &size, NULL, NULL, NULL, NULL) == ERROR_SUCCESS) {
                    /** Open the subkey */
                    shortBuffer[sizeof(shortBuffer)-1] = '\0';
                    _stprintf(name, TEXT("Drivers\\Active\\%s"), shortBuffer);
                    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, name, 0, 0, &hSubKey) == ERROR_SUCCESS) {
                        size = sizeof(name);
                        if (RegQueryValueEx(hSubKey, TEXT("Key"), NULL, NULL, (LPBYTE) name, &size) == ERROR_SUCCESS) {
                            if ((_tcsncmp(name, registry1, _tcsclen(registry1)) == 0) || (_tcsncmp(name, registry2, _tcsclen(registry2)) == 0)) {
                                /** This is the good PnPID, now get the serial com in the "Name" value */
                                size = len;
                                if (RegQueryValueEx(hSubKey, TEXT("Name"), NULL, NULL, (LPBYTE) name, &size) == ERROR_SUCCESS) {
                                    /** Found ! */
                                    RegCloseKey(hSubKey);
                                    RegCloseKey(hKey);
                                    /** OK */
                                    goto exit;
                                }
                            }
                        }

                    /** Close the subkey */
                    RegCloseKey(hSubKey);
                    }
                }
            }
        }
    }
    /** Close the key */
    RegCloseKey(hKey);

exit :
    *handle = CreateFile (name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

    return (error);
}
#endif


Dword Standard_writeRegister (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Processor       processor,
    IN  Dword           registerAddress,
    IN  Byte            value
) {
    Dword ret;
//    deb_info("Enter %s -\n", __FUNCTION__);
    ret = (Standard_writeRegisters (demodulator, chip, processor, registerAddress, 1, &value));
    if(ret) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, ret);
    }
    return ret;
		  
}


Dword Standard_writeRegisters (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Processor       processor,
    IN  Dword           registerAddress,
    IN  Byte            bufferLength,
    IN  Byte*           buffer
) {
    Dword error = Error_NO_ERROR;

#if User_USE_DRIVER
    DWORD number;
    BOOL result;
    WriteRegistersRequest request;
    Ganymede* ganymede;

    ganymede = (Ganymede*) demodulator;

    if (ganymede->driver != NULL) {
        request.chip = chip;
        request.processor = processor;
        request.registerAddress = registerAddress;
        request.bufferLength = bufferLength;
        User_memoryCopy (demodulator, request.buffer, buffer, bufferLength);
        result = DeviceIoControl (
                    ganymede->driver,
                    IOCTL_AFA_DEMOD_WRITEREGISTERS,
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
    Byte registerAddressLength;
    Ganymede* ganymede;

//    deb_info("Enter %s -\n", __FUNCTION__);
    ganymede = (Ganymede*) demodulator;

    if (processor == Processor_LINK) {
        if (registerAddress > 0x000000FF) {
            registerAddressLength = 2;
        } else {
            registerAddressLength = 1;
        }
    } else {
            registerAddressLength = 2;
    }
    if (ganymede->cmdDescription->writeRegisters != NULL) {
        error = ganymede->cmdDescription->writeRegisters (demodulator, chip, processor, registerAddress, registerAddressLength, bufferLength, buffer);
    }
#endif

    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_writeScatterRegisters (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Processor       processor,
    IN  Byte            valueSetsLength,
    IN  ValueSet*       valueSets
) {
    Dword error = Error_NO_ERROR;
    Byte valueSetsAddressLength;
    Ganymede* ganymede;

//    deb_info("Enter %s -\n", __FUNCTION__);
    ganymede = (Ganymede*) demodulator;

    if (processor == Processor_LINK) {
        valueSetsAddressLength = 2;
    } else {
        valueSetsAddressLength = 2;
    }
    if (ganymede->cmdDescription->writeScatterRegisters != NULL) {
        error = ganymede->cmdDescription->writeScatterRegisters (demodulator, chip, processor, valueSetsAddressLength, valueSetsLength, valueSets);
    }

    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_writeTunerRegisters (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Word            registerAddress,
    IN  Byte            bufferLength,
    IN  Byte*           buffer
) {
    Dword error = Error_NO_ERROR;
    Ganymede* ganymede;

    deb_info("Enter %s -\n", __FUNCTION__);
    ganymede = (Ganymede*) demodulator;

    if (ganymede->cmdDescription->writeTunerRegisters != NULL) {
        error = ganymede->cmdDescription->writeTunerRegisters (demodulator, chip, ganymede->tunerDescription->tunerAddress, registerAddress, ganymede->tunerDescription->registerAddressLength, bufferLength, buffer);
    }
    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_writeGenericRegisters (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Byte            interfaceIndex,
    IN  Byte            slaveAddress,
    IN  Byte            bufferLength,
    IN  Byte*           buffer
) {
    Byte writeBuffer[256];
    Byte i;
    Ganymede* ganymede;
    Dword error;

    deb_info("Enter %s -\n", __FUNCTION__);
    ganymede = (Ganymede*) demodulator;

    writeBuffer[0] = bufferLength;
    writeBuffer[1] = interfaceIndex;
    writeBuffer[2] = slaveAddress;

    for (i = 0; i < bufferLength; i++) {
        writeBuffer[3 + i] = buffer[i];
    }
    error = (Standard_sendCommand (demodulator, Command_GENERIC_WRITE, chip, Processor_LINK, bufferLength + 3, writeBuffer, 0, NULL));

    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return error;
}


Dword Standard_writeEepromValues (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Word            registerAddress,
    IN  Byte            bufferLength,
    IN  Byte*           buffer
) {
    Dword error = Error_NO_ERROR;

#if User_USE_DRIVER
    DWORD number;
    BOOL result;
    WriteEepromValuesRequest request;
    Ganymede* ganymede;

    ganymede = (Ganymede*) demodulator;

    if (ganymede->driver != NULL) {
        request.chip = chip;
        request.registerAddress = registerAddress;
        request.bufferLength = bufferLength;
        User_memoryCopy (demodulator, request.buffer, buffer, bufferLength);
        result = DeviceIoControl (
                    ganymede->driver,
                    IOCTL_AFA_DEMOD_WRITEEEPROMVALUES,
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
    Byte eepromAddress;
    Byte registerAddressLength;
    Ganymede* ganymede;

    deb_info("Enter %s -\n", __FUNCTION__);
    ganymede = (Ganymede*) demodulator;

    /** Read EEPROM address. */
    /*error = ganymede->cmdDescription.readRegisters (demodulator, chip, Processor_LINK, 0xBFF0, 1, &eepromAddress);
    if (error) goto exit;
    */
    eepromAddress = 0x01;

    /** Read EEPROM valid length of register. */
    /*error = ganymede->cmdDescription.readRegisters (demodulator, chip, Processor_LINK, 0xBFF1, 1, &registerAddressLength);
    if (error) goto exit;
    */
    registerAddressLength = 0x01;

    if (ganymede->cmdDescription->writeEepromValues != NULL) {
        error = ganymede->cmdDescription->writeEepromValues (demodulator, chip, eepromAddress, registerAddress, registerAddressLength, bufferLength, buffer);
    }

#endif

    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_writeRegisterBits (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Processor       processor,
    IN  Dword           registerAddress,
    IN  Byte            position,
    IN  Byte            length,
    IN  Byte            value
) {
    Dword error = Error_NO_ERROR;

#if User_USE_DRIVER
    DWORD number;
    BOOL result;
    WriteRegisterBitsRequest request;
    Ganymede* ganymede;

    deb_info("Enter %s -\n", __FUNCTION__);
    ganymede = (Ganymede*) demodulator;

    if (ganymede->driver != NULL) {
        request.chip = chip;
        request.processor = processor;
        request.registerAddress = registerAddress;
        request.position = position;
        request.length = length;
        request.value = value;
        result = DeviceIoControl (
                    ganymede->driver,
                    IOCTL_AFA_DEMOD_WRITEREGISTERBITS,
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
    Byte registerAddressLength;
    Ganymede* ganymede;

    ganymede = (Ganymede*) demodulator;

    if (processor == Processor_LINK) {
        if (registerAddress > 0x000000FF) {
            registerAddressLength = 2;
        } else {
            registerAddressLength = 1;
        }
    } else {
        registerAddressLength = 2;
    }
    if (length == 8) {
        if (ganymede->cmdDescription->writeRegisters != NULL) {
            error = ganymede->cmdDescription->writeRegisters (demodulator, chip, processor, registerAddress, registerAddressLength, 1, &value);
        }
    } else {
        if (ganymede->cmdDescription->modifyRegister != NULL) {
            error = ganymede->cmdDescription->modifyRegister (demodulator, chip, processor, registerAddress, registerAddressLength, position, length, value);
        }
    }
#endif

    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_readRegister (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Processor       processor,
    IN  Dword           registerAddress,
    OUT Byte*           value
) {
    Dword error;
//    deb_info("Enter %s -\n", __FUNCTION__);

    error = (Standard_readRegisters (demodulator, chip, processor, registerAddress, 1, value));
    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return error;
}


Dword Standard_readRegisters (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Processor       processor,
    IN  Dword           registerAddress,
    IN  Byte            bufferLength,
    OUT Byte*           buffer
) {
    Dword error = Error_NO_ERROR;

#if User_USE_DRIVER
    DWORD number;
    BOOL result;
    ReadRegistersRequest request;
    Ganymede* ganymede;

    ganymede = (Ganymede*) demodulator;

    if (ganymede->driver != NULL) {
        request.chip = chip;
        request.processor = processor;
        request.registerAddress = registerAddress;
        request.bufferLength = bufferLength;
        request.buffer = buffer;
        result = DeviceIoControl (
                    ganymede->driver,
                    IOCTL_AFA_DEMOD_READREGISTERS,
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
    Byte registerAddressLength;
    Ganymede* ganymede;

//    deb_info("Enter %s -\n", __FUNCTION__);
    ganymede = (Ganymede*) demodulator;

    if (processor == Processor_LINK) {
        if (registerAddress > 0x000000FF) {
            registerAddressLength = 2;
        } else {
            registerAddressLength = 1;
        }
    } else {
        registerAddressLength = 2;
    }
    if (ganymede->cmdDescription->readRegisters != NULL) {
        error = ganymede->cmdDescription->readRegisters (demodulator, chip, processor, registerAddress, registerAddressLength, bufferLength, buffer);
    }
#endif

    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_readScatterRegisters (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Processor       processor,
    IN  Byte            valueSetsLength,
    OUT ValueSet*       valueSets
) {
    Dword error = Error_NO_ERROR;
    Byte valueSetsAddressLength;
    Ganymede* ganymede;

//    deb_info("Enter %s -\n", __FUNCTION__);
    ganymede = (Ganymede*) demodulator;

    if (processor == Processor_LINK) {
        valueSetsAddressLength = 2;
    } else {
        valueSetsAddressLength = 2;
    }

    if (ganymede->cmdDescription->readScatterRegisters != NULL) {
        error = ganymede->cmdDescription->readScatterRegisters (demodulator, chip, processor, valueSetsAddressLength, valueSetsLength, valueSets);
    }
    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_readTunerRegisters (
	IN  Demodulator*    demodulator,
	IN  Byte            chip,
	IN  Word            registerAddress,
	IN  Byte            bufferLength,
	IN  Byte*           buffer
) {
	Dword error = Error_NO_ERROR;

#if User_USE_DRIVER
	DWORD number;
	BOOL result;
	ReadTunerRegistersRequest request;
	Ganymede* ganymede;

	ganymede = (Ganymede*) demodulator;

	if (ganymede->driver != NULL) {
		request.chip = chip;
		request.registerAddress = registerAddress;
		request.bufferLength = bufferLength;
		request.buffer = buffer;
		result = DeviceIoControl (
					ganymede->driver,
					IOCTL_AFA_DEMOD_READTUNERREGISTERS,
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

	deb_info("Enter %s -\n", __FUNCTION__);
	ganymede = (Ganymede*) demodulator;

	if (ganymede->cmdDescription->readTunerRegisters != NULL) {

	if (registerAddress == 0xffff && ganymede->tunerDescription->registerAddressLength == 1)
	{
		error = ganymede->cmdDescription->readTunerRegisters (demodulator, chip, 
		ganymede->tunerDescription->tunerAddress, registerAddress, 0, bufferLength, buffer);
	}
	else
	{
		error = ganymede->cmdDescription->readTunerRegisters (demodulator, chip, 
		ganymede->tunerDescription->tunerAddress, registerAddress, 
		ganymede->tunerDescription->registerAddressLength, bufferLength, buffer);
	}
    }
#endif

	if(error) 
	{
		deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
	}
	return (error);
}


Dword Standard_readGenericRegisters (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Byte            interfaceIndex,
    IN  Byte            slaveAddress,
    IN  Byte            bufferLength,
    IN  Byte*           buffer
) {
    Byte writeBuffer[3];
    Ganymede* ganymede;
    Dword error;

    deb_info("Enter %s -\n", __FUNCTION__);
    ganymede = (Ganymede*) demodulator;

    writeBuffer[0] = bufferLength;
    writeBuffer[1] = interfaceIndex;
    writeBuffer[2] = slaveAddress;

    error = (Standard_sendCommand (demodulator, Command_GENERIC_READ, chip, Processor_LINK, 3, writeBuffer, bufferLength, buffer));
    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return error;
}


Dword Standard_readEepromValues (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Word            registerAddress,
    IN  Byte            bufferLength,
    OUT Byte*           buffer
) {
    Dword error = Error_NO_ERROR;

#if User_USE_DRIVER
    DWORD number;
    BOOL result;
    ReadEepromValuesRequest request;
    Ganymede* ganymede;

    ganymede = (Ganymede*) demodulator;

    if (ganymede->driver != NULL) {
        request.chip = chip;
        request.registerAddress = registerAddress;
        request.bufferLength = bufferLength;
        request.buffer = buffer;
        result = DeviceIoControl (
                    ganymede->driver,
                    IOCTL_AFA_DEMOD_READEEPROMVALUES,
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
    Byte eepromAddress;
    Byte registerAddressLength;
    Ganymede* ganymede;

    deb_info("Enter %s -\n", __FUNCTION__);
    ganymede = (Ganymede*) demodulator;

    /** Read EEPROM address. */
    /*error = ganymede->cmdDescription.readRegisters (demodulator, chip, Processor_LINK, 0xBFF0, 1, &eepromAddress);
    if (error) goto exit;
    */
    eepromAddress = 0x01;

    /** Read EEPROM valid length of register. */
    /*error = ganymede->cmdDescription.readRegisters (demodulator, chip, Processor_LINK, 0xBFF1, 1, &registerAddressLength);
    if (error) goto exit;
    */
    registerAddressLength = 0x01;

    if (ganymede->cmdDescription->readEepromValues != NULL) {
        error = ganymede->cmdDescription->readEepromValues (demodulator, chip, eepromAddress, registerAddress, registerAddressLength, bufferLength, buffer);
    }

#endif

    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_readRegisterBits (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Processor       processor,
    IN  Dword           registerAddress,
    IN  Byte            position,
    IN  Byte            length,
    OUT Byte*           value
) {
    Dword error = Error_NO_ERROR;

#if User_USE_DRIVER
    DWORD number;
    BOOL result;
    ReadRegisterBitsRequest request;
    Ganymede* ganymede;

    ganymede = (Ganymede*) demodulator;

    if (ganymede->driver != NULL) {
        request.chip = chip;
        request.processor = processor;
        request.registerAddress = registerAddress;
        request.position = position;
        request.length = length;
        request.value = value;
        result = DeviceIoControl (
                    ganymede->driver,
                    IOCTL_AFA_DEMOD_READREGISTERBITS,
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
    Byte temp = 0;
    Byte registerAddressLength;
    Ganymede* ganymede;

    deb_info("Enter %s -\n", __FUNCTION__);
    ganymede = (Ganymede*) demodulator;

    if (processor == Processor_LINK) {
        if (registerAddress > 0x000000FF) {
            registerAddressLength = 2;
        } else {
            registerAddressLength = 1;
        }
    } else {
        registerAddressLength = 2;
    }
    if (ganymede->cmdDescription->readRegisters != NULL) {
        error = ganymede->cmdDescription->readRegisters (demodulator, chip, processor, registerAddress, registerAddressLength, 1, &temp);
    }
    if (error) goto exit;

    if (length == 8) {
        *value = temp;
    } else {
        temp = REG_GET (temp, position, length);
        *value = temp;
    }

#endif

exit :
    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_sendCommand (
    IN  Demodulator*    demodulator,
    OUT Word            command,
    IN  Byte            chip,
    IN  Processor       processor,
    IN  Dword           writeBufferLength,
    IN  Byte*           writeBuffer,
    IN  Dword           readBufferLength,
    OUT Byte*           readBuffer
) {
    Dword error = Error_NO_ERROR;

#if User_USE_DRIVER
#else
    Ganymede* ganymede;

    deb_info("Enter %s -\n", __FUNCTION__);
    ganymede = (Ganymede*) demodulator;

    if (ganymede->cmdDescription->sendCommand != NULL) {
        error = ganymede->cmdDescription->sendCommand (demodulator, command, chip, processor, writeBufferLength, writeBuffer, readBufferLength, readBuffer);
    }
#endif
    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_getHardwareVersion (
    IN  Demodulator*    demodulator,
    OUT Dword*          version
) {
    Dword error = Error_NO_ERROR;

#if User_USE_DRIVER
#else
    Byte hwVer0;
    Byte hwVer1;

    deb_info("Enter %s -\n", __FUNCTION__);
    error = Standard_readRegister (demodulator, 0, Processor_OFDM, 0xFFF0, &hwVer0);
    if (error) goto exit;
    error = Standard_readRegister (demodulator, 0, Processor_OFDM, 0xFFF1, &hwVer1);
    if (error) goto exit;

    /** HW Version = HWVer + Top_Ver */
    *version = (Dword) (hwVer1 << 8) + (Dword) hwVer0;

exit :
#endif

    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_getFirmwareVersion (
    IN  Demodulator*    demodulator,
    IN  Processor       processor,
    OUT Dword*          version
) {
    Dword error = Error_NO_ERROR;

#if User_USE_DRIVER
    DWORD number;
    BOOL result;
    GetFirmwareVersionRequest request;
    Ganymede* ganymede;

    ganymede = (Ganymede*) demodulator;

    if (ganymede->driver != NULL) {
        request.processor = processor;
        request.version = version;
        result = DeviceIoControl (
                    ganymede->driver,
                    IOCTL_AFA_DEMOD_GETFIRMWAREVERSION,
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
    Byte writeBuffer[1];
    Byte readBuffer[4];
    Ganymede* ganymede;

    deb_info("Enter %s -\n", __FUNCTION__);
    ganymede = (Ganymede*) demodulator;

    if ((ganymede->busId == Bus_I2M) || (ganymede->busId == Bus_I2U)) {
        *version = 0xFFFFFFFF;
        goto exit;
    }

    writeBuffer[0] = 1;
    error = Standard_sendCommand (demodulator, Command_QUERYINFO, 0, processor, 1, writeBuffer, 4, readBuffer);
    if (error) goto exit;

    *version = (Dword) (((Dword) readBuffer[0] << 24) + ((Dword) readBuffer[1] << 16) + ((Dword) readBuffer[2] << 8) + (Dword) readBuffer[3]);

exit :
#endif

    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_getPostVitBer (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    OUT Dword*          postErrorCount,  /** 24 bits */
    OUT Dword*          postBitCount,    /** 16 bits */
    OUT Word*           abortCount
) {
    Dword error = Error_NO_ERROR;
    Dword errorCount;
    Dword bitCount;
    Byte buffer[7];
    Word abort;
    Ganymede* ganymede;

    deb_info("Enter %s -\n", __FUNCTION__);
    ganymede = (Ganymede*) demodulator;

    *postErrorCount = 0;
    *postBitCount  = 0;

    error = Standard_readRegisters (demodulator, chip, Processor_OFDM, rsd_abort_packet_cnt_7_0, r_rsd_packet_unit_15_8 - rsd_abort_packet_cnt_7_0 + 1, buffer);
    if (error) goto exit;

    abort = ((Word) buffer[rsd_abort_packet_cnt_15_8 - rsd_abort_packet_cnt_7_0] << 8) + buffer[rsd_abort_packet_cnt_7_0 - rsd_abort_packet_cnt_7_0];
    errorCount = ((Dword) buffer[rsd_bit_err_cnt_23_16 - rsd_abort_packet_cnt_7_0] << 16) + ((Dword) buffer[rsd_bit_err_cnt_15_8 - rsd_abort_packet_cnt_7_0] << 8) + buffer[rsd_bit_err_cnt_7_0 - rsd_abort_packet_cnt_7_0];
    bitCount = ((Dword) buffer[r_rsd_packet_unit_15_8 - rsd_abort_packet_cnt_7_0] << 8) + buffer[r_rsd_packet_unit_7_0 - rsd_abort_packet_cnt_7_0];
    if (bitCount == 0) {
        /*error = Error_RSD_PKT_CNT_0;*/
        *postErrorCount = 1;
        *postBitCount = 2;
        *abortCount = 1000;
        goto exit;
    }

    *abortCount = abort;
    bitCount = bitCount - (Dword)abort;
    if (bitCount == 0) {
        *postErrorCount = 1;
        *postBitCount  = 2;
    } else {
        *postErrorCount = errorCount - (Dword) abort * 8 * 8;
        *postBitCount  = bitCount * 204 * 8;
    }

exit :
    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_resetPostVitErrorCount (
    IN  Demodulator*    demodulator,
    IN  Byte            chip
) {
    Dword error = Error_NO_ERROR;

    deb_info("Enter %s -\n", __FUNCTION__);
    /** Reset counter for next time we get post Viterbi BER */
    error = Standard_writeRegisterBits (demodulator, chip, Processor_OFDM, p_reg_rsd_ber_rdy, reg_rsd_ber_rdy_pos, reg_rsd_ber_rdy_len, 1);

    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_wordToInt (
    IN  Demodulator*    demodulator,
    short*              value,
    Word                input
) {
    Dword error = Error_NO_ERROR;
    short volt;
    //Dword dwUpVoltX100 = 330;

    deb_info("Enter %s -\n", __FUNCTION__);
    input = input & 0x03FF;

    if (input >= 512) {
        volt = (short) input - (short) 0x400ul;
    } else {
        volt = (short) input;
    }

    *value = volt;

    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_getRfAgcGain (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    OUT Byte*           rfAgc
) {
    Dword   error = Error_NO_ERROR;

    deb_info("Enter %s -\n", __FUNCTION__);
#if User_USE_DRIVER
#else
    /** get rf_agc_control */
    error = Standard_readRegisterBits (demodulator, chip, Processor_OFDM, r_reg_aagc_rf_gain, reg_aagc_rf_gain_pos, reg_aagc_rf_gain_len, rfAgc);
#endif

    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_getIfAgcGain (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    OUT Byte*           ifAgc
) {
    Dword error = Error_NO_ERROR;

    deb_info("Enter %s -\n", __FUNCTION__);
#if User_USE_DRIVER
#else
    /** get if_agc_control */
    error = Standard_readRegisterBits (demodulator, chip, Processor_OFDM, r_reg_aagc_if_gain, reg_aagc_if_gain_pos, reg_aagc_if_gain_len, ifAgc);
#endif

    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_getAgcGain (
    IN  Demodulator*    demodulator,
    IN  Word            registerHi,
    IN  Word            registerLo,
    IN  Byte            position,
    IN  Byte            length,
    OUT Word            *value
) {
    Dword error = Error_NO_ERROR;
    Byte temp0;
    Byte temp1;
    deb_info("Enter %s -\n", __FUNCTION__);

    error = Standard_readRegister (demodulator, 0, Processor_OFDM, registerLo, &temp0);
    if (error) goto exit;
    error = Standard_readRegisterBits (demodulator, 0, Processor_OFDM, registerHi, position, length, &temp1);
    if (error) goto exit;

    *value = (Word) (temp1 << 8) + (Word) temp0;

exit :
    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_setAgcGain (
    IN  Demodulator*    demodulator,
    IN  Word            registerHi,
    IN  Word            registerLo,
    IN  Byte            position,
    IN  Byte            length,
    IN  Word            value
) {
    Dword error = Error_NO_ERROR;
    Byte temp0;
    Byte temp1;

    deb_info("Enter %s -\n", __FUNCTION__);
    temp0 = (Byte) (value & 0x00FF);
    temp1 = (Byte) ((value & 0x0300) >> 8);

    error = Standard_writeRegister (demodulator, 0, Processor_OFDM, registerLo, temp0);
    if (error) goto exit;
    error = Standard_writeRegisterBits (demodulator, 0, Processor_OFDM, registerHi, position, length, temp1);

exit :
    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_resetAgc (
    IN  Demodulator*    demodulator
) {
    Dword error = Error_NO_ERROR;
    //Byte temp = 0;

    deb_info("Enter %s -\n", __FUNCTION__);
    error = Standard_writeRegisterBits (demodulator, 0, Processor_OFDM, p_reg_aagc_top_th_dis, reg_aagc_top_th_dis_pos, reg_aagc_top_th_dis_len, 0x00);
    if (error) goto exit;
    error = Standard_writeRegisterBits (demodulator, 0, Processor_OFDM, p_reg_aagc_top_reload, reg_aagc_top_reload_pos, reg_aagc_top_reload_len, 0x01);
    if (error) goto exit;
    error = Standard_writeRegisterBits (demodulator, 0, Processor_OFDM, p_reg_agc_rst, reg_agc_rst_pos, reg_agc_rst_len, 0x01);

exit :
    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_getSignalQuality (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    OUT Byte*           quality
) {
    Dword error = Error_NO_ERROR;

    deb_info("Enter %s -\n", __FUNCTION__);
    error = Standard_readRegister (demodulator, chip, Processor_OFDM, signal_quality, quality);
    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_getSignalStrength (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    OUT Byte*           strength
) {
    Dword error = Error_NO_ERROR;

    deb_info("Enter %s -\n", __FUNCTION__);
    error = Standard_readRegister (demodulator, chip, Processor_OFDM, signal_strength, strength);
    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_getSignalStrengthDbm (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Long            rfpullUpVolt_X10,     /** RF pull up voltage multiplied by 10 */
    IN  Long            ifpullUpVolt_X10,     /** IF pull up voltage multiplied by 10 */
    OUT Long*           strengthDbm           /** DBm                                 */
)
{
    Dword error = Error_NO_ERROR;
    Byte temp;
    deb_info("Enter %s -\n", __FUNCTION__);

    if ((rfpullUpVolt_X10 == 0) || (ifpullUpVolt_X10 == 0)) {
        error = Error_INV_PULLUP_VOLT;
        goto exit;
    }

    error = Standard_readRegister (demodulator, chip, Processor_OFDM, est_rf_level_dbm, &temp);
    if (error) goto exit;

    *strengthDbm = (Long) (temp * -1);

exit :
    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_divider (
    IN  Demodulator*    demodulator,
    IN  Dword           a,
    IN  Dword           b,
    IN  Dword           x
) {
    Dword answer = 0;
    Dword c = 0;
    Dword i = 0;

    if (a > b) {
        c = a / b;
        a = a - c * b;
    }

    for (i = 0; i < x; i++) {
        if (a >= b) {
            answer += 1;
            a-=b;
        }
        a <<= 1;
        answer <<= 1;
    }

    answer = (c << (Long) x) + answer;

    return (answer);
}


Dword Standard_computeCrystal (
    IN  Demodulator*    demodulator,
    IN  Long            crystalFrequency,   /** Crystal frequency (Hz) */
    OUT Dword*          crystal
) {
    Dword   error = Error_NO_ERROR;
    deb_info("Enter %s -\n", __FUNCTION__);

    *crystal = (Long) Standard_divider (demodulator, (Dword) crystalFrequency, 1000000ul, 19ul);

    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_computeAdc (
    IN  Demodulator*    demodulator,
    IN  Long            adcFrequency,       /** ADC frequency (Hz) */
    OUT Dword*          adc
)
{
    Dword   error = Error_NO_ERROR;
    deb_info("Enter %s -\n", __FUNCTION__);

    *adc = (Long) Standard_divider (demodulator, (Dword) adcFrequency, 1000000ul, 19ul);

    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_computeFcw (
    IN  Demodulator*    demodulator,
    IN  Long            adcFrequency,       /** ADC frequency (Hz)    */
    IN  Long            ifFrequency,        /** IF frequency (Hz)     */
    IN  Bool            inversion,          /** RF spectrum inversion */
    OUT Dword*          fcw
) {
    Dword error = Error_NO_ERROR;
    Long ifFreq;
    Long adcFreq;
    Long adcFreqHalf;
    Long adcFreqSample;
    Long invBfs;
    Long controlWord;
    Byte adcMultiplier;

    deb_info("Enter %s -\n", __FUNCTION__);
    adcFreq = adcFrequency;
    ifFreq = ifFrequency;
    adcFreqHalf = adcFreq / 2;

    if (inversion == True)
        ifFreq = -1 * ifFreq;

    adcFreqSample = ifFreq;

    if (adcFreqSample >= 0)
        invBfs = 1;
    else {
        invBfs = -1;
        adcFreqSample = adcFreqSample * -1;
    }

    while (adcFreqSample > adcFreqHalf)
        adcFreqSample = adcFreqSample - adcFreq;

    /** Sample, spectrum at positive frequency */
    if(adcFreqSample >= 0)
        invBfs = invBfs * -1;
    else {
        invBfs = invBfs * 1;
        adcFreqSample = adcFreqSample * (-1);       /** Absolute value */
    }

    controlWord = (Long) Standard_divider (demodulator, (Dword) adcFreqSample, (Dword) adcFreq, 23ul);

    if (invBfs == -1) {
        controlWord *= -1;
    }

    /** Get ADC multiplier */
    error = Standard_readRegister (demodulator, 0, Processor_OFDM, adcx2, &adcMultiplier);
    if (error) goto exit;

    if (adcMultiplier == 1) {
        controlWord /= 2;
    }

    *fcw = controlWord & 0x7FFFFF;

exit :
    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_selectBandwidth (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Word            bandwidth,          /** KHz              */
    IN  Dword           adcFrequency        /** Hz, ex: 20480000 */
) {
    Dword error = Error_NO_ERROR;
    Dword coeff1_2048Nu;
    Dword coeff1_4096Nu;
    Dword coeff1_8191Nu;
    Dword coeff1_8192Nu;
    Dword coeff1_8193Nu;
    Dword coeff2_2k;
    Dword coeff2_4k;
    Dword coeff2_8k;
    Word bfsfcw_fftindex_ratio;
    Word fftindex_bfsfcw_ratio;

    Byte temp0;
    Byte temp1;
    Byte temp2;
    Byte temp3;
    Byte buffer[36];
    Byte bw;
    Byte adcMultiplier;
    Ganymede* ganymede;

    deb_info("Enter %s -\n", __FUNCTION__);
    if (bandwidth == 5000)
        bw = 3;
    else if (bandwidth == 6000)
        bw = 0;
    else if (bandwidth == 7000)
        bw = 1;
    else if (bandwidth == 8000)
        bw = 2;
    else {
        error = Error_INVALID_BW;
        goto exit;
    }

    ganymede = (Ganymede*) demodulator;

    error = Standard_writeRegisterBits (demodulator, chip, Processor_OFDM, g_reg_bw, reg_bw_pos, reg_bw_len, bw);
    if (error) goto exit;

    /** Program CFOE */
    if (adcFrequency == 20156250) {
        if (bandwidth == 5000) {
            coeff1_2048Nu = 0x02449b5c;
            coeff1_4096Nu = 0x01224dae;
            coeff1_8191Nu = 0x00912b60;
            coeff1_8192Nu = 0x009126d7;
            coeff1_8193Nu = 0x0091224e;
            coeff2_2k = 0x01224dae;
            coeff2_4k = 0x009126d7;
            coeff2_8k = 0x0048936b;
            bfsfcw_fftindex_ratio = 0x0387;
            fftindex_bfsfcw_ratio = 0x0122;
        } else if (bandwidth == 6000) {
            coeff1_2048Nu = 0x02b8ba6e;
            coeff1_4096Nu = 0x015c5d37;
            coeff1_8191Nu = 0x00ae340d;
            coeff1_8192Nu = 0x00ae2e9b;
            coeff1_8193Nu = 0x00ae292a;
            coeff2_2k = 0x015c5d37;
            coeff2_4k = 0x00ae2e9b;
            coeff2_8k = 0x0057174e;
            bfsfcw_fftindex_ratio = 0x02f1;
            fftindex_bfsfcw_ratio = 0x015c;
        } else if (bandwidth == 7000) {
            coeff1_2048Nu = 0x032cd980;
            coeff1_4096Nu = 0x01966cc0;
            coeff1_8191Nu = 0x00cb3cba;
            coeff1_8192Nu = 0x00cb3660;
            coeff1_8193Nu = 0x00cb3007;
            coeff2_2k = 0x01966cc0;
            coeff2_4k = 0x00cb3660;
            coeff2_8k = 0x00659b30;
            bfsfcw_fftindex_ratio = 0x0285;
            fftindex_bfsfcw_ratio = 0x0196;
        } else if (bandwidth == 8000) {
            coeff1_2048Nu = 0x03a0f893;
            coeff1_4096Nu = 0x01d07c49;
            coeff1_8191Nu = 0x00e84567;
            coeff1_8192Nu = 0x00e83e25;
            coeff1_8193Nu = 0x00e836e3;
            coeff2_2k = 0x01d07c49;
            coeff2_4k = 0x00e83e25;
            coeff2_8k = 0x00741f12;
            bfsfcw_fftindex_ratio = 0x0234;
            fftindex_bfsfcw_ratio = 0x01d0;
        } else {
            error = Error_INVALID_BW;
            goto exit;
        }
    } else if (adcFrequency == 20187500) {
        if (bandwidth == 5000) {
            coeff1_2048Nu = 0x0243b546;
            coeff1_4096Nu = 0x0121daa3;
            coeff1_8191Nu = 0x0090f1d9;
            coeff1_8192Nu = 0x0090ed51;
            coeff1_8193Nu = 0x0090e8ca;
            coeff2_2k = 0x0121daa3;
            coeff2_4k = 0x0090ed51;
            coeff2_8k = 0x004876a9;
            bfsfcw_fftindex_ratio = 0x0388;
            fftindex_bfsfcw_ratio = 0x0122;
        } else if (bandwidth == 6000) {
            coeff1_2048Nu = 0x02b7a654;
            coeff1_4096Nu = 0x015bd32a;
            coeff1_8191Nu = 0x00adef04;
            coeff1_8192Nu = 0x00ade995;
            coeff1_8193Nu = 0x00ade426;
            coeff2_2k = 0x015bd32a;
            coeff2_4k = 0x00ade995;
            coeff2_8k = 0x0056f4ca;
            bfsfcw_fftindex_ratio = 0x02f2;
            fftindex_bfsfcw_ratio = 0x015c;
        } else if (bandwidth == 7000) {
            coeff1_2048Nu = 0x032b9761;
            coeff1_4096Nu = 0x0195cbb1;
            coeff1_8191Nu = 0x00caec30;
            coeff1_8192Nu = 0x00cae5d8;
            coeff1_8193Nu = 0x00cadf81;
            coeff2_2k = 0x0195cbb1;
            coeff2_4k = 0x00cae5d8;
            coeff2_8k = 0x006572ec;
            bfsfcw_fftindex_ratio = 0x0286;
            fftindex_bfsfcw_ratio = 0x0196;
        } else if (bandwidth == 8000) {
            coeff1_2048Nu = 0x039f886f;
            coeff1_4096Nu = 0x01cfc438;
            coeff1_8191Nu = 0x00e7e95b;
            coeff1_8192Nu = 0x00e7e21c;
            coeff1_8193Nu = 0x00e7dadd;
            coeff2_2k = 0x01cfc438;
            coeff2_4k = 0x00e7e21c;
            coeff2_8k = 0x0073f10e;
            bfsfcw_fftindex_ratio = 0x0235;
            fftindex_bfsfcw_ratio = 0x01d0;
        } else {
            error = Error_INVALID_BW;
            goto exit;
        }
    } else if (adcFrequency == 20250000) {
        if (bandwidth == 5000) {
            coeff1_2048Nu = 0x0241eb3b;
            coeff1_4096Nu = 0x0120f59e;
            coeff1_8191Nu = 0x00907f53;
            coeff1_8192Nu = 0x00907acf;
            coeff1_8193Nu = 0x0090764b;
            coeff2_2k = 0x0120f59e;
            coeff2_4k = 0x00907acf;
            coeff2_8k = 0x00483d67;
            bfsfcw_fftindex_ratio = 0x038b;
            fftindex_bfsfcw_ratio = 0x0121;
        } else if (bandwidth == 6000) {
            coeff1_2048Nu = 0x02b580ad;
            coeff1_4096Nu = 0x015ac057;
            coeff1_8191Nu = 0x00ad6597;
            coeff1_8192Nu = 0x00ad602b;
            coeff1_8193Nu = 0x00ad5ac1;
            coeff2_2k = 0x015ac057;
            coeff2_4k = 0x00ad602b;
            coeff2_8k = 0x0056b016;
            bfsfcw_fftindex_ratio = 0x02f4;
            fftindex_bfsfcw_ratio = 0x015b;
        } else if (bandwidth == 7000) {
            coeff1_2048Nu = 0x03291620;
            coeff1_4096Nu = 0x01948b10;
            coeff1_8191Nu = 0x00ca4bda;
            coeff1_8192Nu = 0x00ca4588;
            coeff1_8193Nu = 0x00ca3f36;
            coeff2_2k = 0x01948b10;
            coeff2_4k = 0x00ca4588;
            coeff2_8k = 0x006522c4;
            bfsfcw_fftindex_ratio = 0x0288;
            fftindex_bfsfcw_ratio = 0x0195;
        } else if (bandwidth == 8000) {
            coeff1_2048Nu = 0x039cab92;
            coeff1_4096Nu = 0x01ce55c9;
            coeff1_8191Nu = 0x00e7321e;
            coeff1_8192Nu = 0x00e72ae4;
            coeff1_8193Nu = 0x00e723ab;
            coeff2_2k = 0x01ce55c9;
            coeff2_4k = 0x00e72ae4;
            coeff2_8k = 0x00739572;
            bfsfcw_fftindex_ratio = 0x0237;
            fftindex_bfsfcw_ratio = 0x01ce;
        } else {
            error = Error_INVALID_BW;
            goto exit;
        }
    } else if (adcFrequency == 20583333) {
        if (bandwidth == 5000) {
            coeff1_2048Nu = 0x02402402;
            coeff1_4096Nu = 0x01201201;
            coeff1_8191Nu = 0x00900d81;
            coeff1_8192Nu = 0x00900901;
            coeff1_8193Nu = 0x00900480;
            coeff2_2k = 0x01201201;
            coeff2_4k = 0x00900901;
            coeff2_8k = 0x00480480;
            bfsfcw_fftindex_ratio = 0x038e;
            fftindex_bfsfcw_ratio = 0x0120;
        } else if (bandwidth == 6000) {
            coeff1_2048Nu = 0x02b35e69;
            coeff1_4096Nu = 0x0159af35;
            coeff1_8191Nu = 0x00acdd01;
            coeff1_8192Nu = 0x00acd79a;
            coeff1_8193Nu = 0x00acd234;
            coeff2_2k = 0x0159af35;
            coeff2_4k = 0x00acd79a;
            coeff2_8k = 0x00566bcd;
            bfsfcw_fftindex_ratio = 0x02f6;
            fftindex_bfsfcw_ratio = 0x015a;
        } else if (bandwidth == 7000) {
            coeff1_2048Nu = 0x032698d0;
            coeff1_4096Nu = 0x01934c68;
            coeff1_8191Nu = 0x00c9ac81;
            coeff1_8192Nu = 0x00c9a634;
            coeff1_8193Nu = 0x00c99fe7;
            coeff2_2k = 0x01934c68;
            coeff2_4k = 0x00c9a634;
            coeff2_8k = 0x0064d31a;
            bfsfcw_fftindex_ratio = 0x028a;
            fftindex_bfsfcw_ratio = 0x0193;
        } else if (bandwidth == 8000) {
            coeff1_2048Nu = 0x0399d337;
            coeff1_4096Nu = 0x01cce99b;
            coeff1_8191Nu = 0x00e67c02;
            coeff1_8192Nu = 0x00e674ce;
            coeff1_8193Nu = 0x00e66d9a;
            coeff2_2k = 0x01cce99b;
            coeff2_4k = 0x00e674ce;
            coeff2_8k = 0x00733a67;
            bfsfcw_fftindex_ratio = 0x0239;
            fftindex_bfsfcw_ratio = 0x01cd;
        } else {
            error = Error_INVALID_BW;
            goto exit;
        }
    } else if (adcFrequency == 20416667) {
        if (bandwidth == 5000) {
            coeff1_2048Nu = 0x023d337f;
            coeff1_4096Nu = 0x011e99c0;
            coeff1_8191Nu = 0x008f515a;
            coeff1_8192Nu = 0x008f4ce0;
            coeff1_8193Nu = 0x008f4865;
            coeff2_2k = 0x011e99c0;
            coeff2_4k = 0x008f4ce0;
            coeff2_8k = 0x0047a670;
            bfsfcw_fftindex_ratio = 0x0393;
            fftindex_bfsfcw_ratio = 0x011f;
        } else if (bandwidth == 6000) {
            coeff1_2048Nu = 0x02afd765;
            coeff1_4096Nu = 0x0157ebb3;
            coeff1_8191Nu = 0x00abfb39;
            coeff1_8192Nu = 0x00abf5d9;
            coeff1_8193Nu = 0x00abf07a;
            coeff2_2k = 0x0157ebb3;
            coeff2_4k = 0x00abf5d9;
            coeff2_8k = 0x0055faed;
            bfsfcw_fftindex_ratio = 0x02fa;
            fftindex_bfsfcw_ratio = 0x0158;
        } else if (bandwidth == 7000) {
            coeff1_2048Nu = 0x03227b4b;
            coeff1_4096Nu = 0x01913da6;
            coeff1_8191Nu = 0x00c8a518;
            coeff1_8192Nu = 0x00c89ed3;
            coeff1_8193Nu = 0x00c8988e;
            coeff2_2k = 0x01913da6;
            coeff2_4k = 0x00c89ed3;
            coeff2_8k = 0x00644f69;
            bfsfcw_fftindex_ratio = 0x028d;
            fftindex_bfsfcw_ratio = 0x0191;
        } else if (bandwidth == 8000) {
            coeff1_2048Nu = 0x03951f32;
            coeff1_4096Nu = 0x01ca8f99;
            coeff1_8191Nu = 0x00e54ef7;
            coeff1_8192Nu = 0x00e547cc;
            coeff1_8193Nu = 0x00e540a2;
            coeff2_2k = 0x01ca8f99;
            coeff2_4k = 0x00e547cc;
            coeff2_8k = 0x0072a3e6;
            bfsfcw_fftindex_ratio = 0x023c;
            fftindex_bfsfcw_ratio = 0x01cb;
        } else {
            error = Error_INVALID_BW;
            goto exit;
        }
    } else if (adcFrequency == 20480000) {
        if (bandwidth == 5000) {
            coeff1_2048Nu = 0x023b6db7;
            coeff1_4096Nu = 0x011db6db;
            coeff1_8191Nu = 0x008edfe5;
            coeff1_8192Nu = 0x008edb6e;
            coeff1_8193Nu = 0x008ed6f7;
            coeff2_2k = 0x011db6db;
            coeff2_4k = 0x008edb6e;
            coeff2_8k = 0x00476db7;
            bfsfcw_fftindex_ratio = 0x0396;
            fftindex_bfsfcw_ratio = 0x011e;
        } else if (bandwidth == 6000) {
            coeff1_2048Nu = 0x02adb6db;
            coeff1_4096Nu = 0x0156db6e;
            coeff1_8191Nu = 0x00ab7312;
            coeff1_8192Nu = 0x00ab6db7;
            coeff1_8193Nu = 0x00ab685c;
            coeff2_2k = 0x0156db6e;
            coeff2_4k = 0x00ab6db7;
            coeff2_8k = 0x0055b6db;
            bfsfcw_fftindex_ratio = 0x02fd;
            fftindex_bfsfcw_ratio = 0x0157;
        } else if (bandwidth == 7000) {
            coeff1_2048Nu = 0x03200000;
            coeff1_4096Nu = 0x01900000;
            coeff1_8191Nu = 0x00c80640;
            coeff1_8192Nu = 0x00c80000;
            coeff1_8193Nu = 0x00c7f9c0;
            coeff2_2k = 0x01900000;
            coeff2_4k = 0x00c80000;
            coeff2_8k = 0x00640000;
            bfsfcw_fftindex_ratio = 0x028f;
            fftindex_bfsfcw_ratio = 0x0190;
        } else if (bandwidth == 8000) {
            coeff1_2048Nu = 0x03924925;
            coeff1_4096Nu = 0x01c92492;
            coeff1_8191Nu = 0x00e4996e;
            coeff1_8192Nu = 0x00e49249;
            coeff1_8193Nu = 0x00e48b25;
            coeff2_2k = 0x01c92492;
            coeff2_4k = 0x00e49249;
            coeff2_8k = 0x00724925;
            bfsfcw_fftindex_ratio = 0x023d;
            fftindex_bfsfcw_ratio = 0x01c9;
        } else {
            error = Error_INVALID_BW;
            goto exit;
        }
    } else if (adcFrequency == 20500000) {
        if (bandwidth == 5000) {
            coeff1_2048Nu = 0x023adeff;
            coeff1_4096Nu = 0x011d6f80;
            coeff1_8191Nu = 0x008ebc36;
            coeff1_8192Nu = 0x008eb7c0;
            coeff1_8193Nu = 0x008eb34a;
            coeff2_2k = 0x011d6f80;
            coeff2_4k = 0x008eb7c0;
            coeff2_8k = 0x00475be0;
            bfsfcw_fftindex_ratio = 0x0396;
            fftindex_bfsfcw_ratio = 0x011d;
        } else if (bandwidth == 6000) {
            coeff1_2048Nu = 0x02ad0b99;
            coeff1_4096Nu = 0x015685cc;
            coeff1_8191Nu = 0x00ab4840;
            coeff1_8192Nu = 0x00ab42e6;
            coeff1_8193Nu = 0x00ab3d8c;
            coeff2_2k = 0x015685cc;
            coeff2_4k = 0x00ab42e6;
            coeff2_8k = 0x0055a173;
            bfsfcw_fftindex_ratio = 0x02fd;
            fftindex_bfsfcw_ratio = 0x0157;
        } else if (bandwidth == 7000) {
            coeff1_2048Nu = 0x031f3832;
            coeff1_4096Nu = 0x018f9c19;
            coeff1_8191Nu = 0x00c7d44b;
            coeff1_8192Nu = 0x00c7ce0c;
            coeff1_8193Nu = 0x00c7c7ce;
            coeff2_2k = 0x018f9c19;
            coeff2_4k = 0x00c7ce0c;
            coeff2_8k = 0x0063e706;
            bfsfcw_fftindex_ratio = 0x0290;
            fftindex_bfsfcw_ratio = 0x0190;
        } else if (bandwidth == 8000) {
            coeff1_2048Nu = 0x039164cb;
            coeff1_4096Nu = 0x01c8b266;
            coeff1_8191Nu = 0x00e46056;
            coeff1_8192Nu = 0x00e45933;
            coeff1_8193Nu = 0x00e45210;
            coeff2_2k = 0x01c8b266;
            coeff2_4k = 0x00e45933;
            coeff2_8k = 0x00722c99;
            bfsfcw_fftindex_ratio = 0x023e;
            fftindex_bfsfcw_ratio = 0x01c9;
        } else {
            error = Error_INVALID_BW;
            goto exit;
        }
    } else if (adcFrequency == 20625000) {
        if (bandwidth == 5000) {
            coeff1_2048Nu = 0x02376948;
            coeff1_4096Nu = 0x011bb4a4;
            coeff1_8191Nu = 0x008ddec1;
            coeff1_8192Nu = 0x008dda52;
            coeff1_8193Nu = 0x008dd5e3;
            coeff2_2k = 0x011bb4a4;
            coeff2_4k = 0x008dda52;
            coeff2_8k = 0x0046ed29;
            bfsfcw_fftindex_ratio = 0x039c;
            fftindex_bfsfcw_ratio = 0x011c;
        } else if (bandwidth == 6000) {
            coeff1_2048Nu = 0x02a8e4bd;
            coeff1_4096Nu = 0x0154725e;
            coeff1_8191Nu = 0x00aa3e81;
            coeff1_8192Nu = 0x00aa392f;
            coeff1_8193Nu = 0x00aa33de;
            coeff2_2k = 0x0154725e;
            coeff2_4k = 0x00aa392f;
            coeff2_8k = 0x00551c98;
            bfsfcw_fftindex_ratio = 0x0302;
            fftindex_bfsfcw_ratio = 0x0154;
        } else if (bandwidth == 7000) {
            coeff1_2048Nu = 0x031a6032;
            coeff1_4096Nu = 0x018d3019;
            coeff1_8191Nu = 0x00c69e41;
            coeff1_8192Nu = 0x00c6980c;
            coeff1_8193Nu = 0x00c691d8;
            coeff2_2k = 0x018d3019;
            coeff2_4k = 0x00c6980c;
            coeff2_8k = 0x00634c06;
            bfsfcw_fftindex_ratio = 0x0294;
            fftindex_bfsfcw_ratio = 0x018d;
        } else if (bandwidth == 8000) {
            coeff1_2048Nu = 0x038bdba6;
            coeff1_4096Nu = 0x01c5edd3;
            coeff1_8191Nu = 0x00e2fe02;
            coeff1_8192Nu = 0x00e2f6ea;
            coeff1_8193Nu = 0x00e2efd2;
            coeff2_2k = 0x01c5edd3;
            coeff2_4k = 0x00e2f6ea;
            coeff2_8k = 0x00717b75;
            bfsfcw_fftindex_ratio = 0x0242;
            fftindex_bfsfcw_ratio = 0x01c6;
        } else {
            error = Error_INVALID_BW;
            goto exit;
        }
    } else {
        error = Error_INVALID_XTAL_FREQ;
        goto exit;
    }


    /** Get ADC multiplier */
    error = Standard_readRegister (demodulator, 0, Processor_OFDM, adcx2, &adcMultiplier);
    if (error) goto exit;

    if (adcMultiplier == 1) {
        coeff1_2048Nu /= 2;
        coeff1_4096Nu /= 2;
        coeff1_8191Nu /= 2;
        coeff1_8192Nu /= 2;
        coeff1_8193Nu /= 2 ;
        coeff2_2k /= 2;
        coeff2_4k /= 2;
        coeff2_8k /= 2;
    }

    /** Write coeff1_2048Nu */
    /** Get Byte0 */
    temp0 = (Byte) (coeff1_2048Nu & 0x000000FF);
    /** Get Byte1 */
    temp1 = (Byte) ((coeff1_2048Nu & 0x0000FF00) >> 8);
    /** Get Byte2 */
    temp2 = (Byte) ((coeff1_2048Nu & 0x00FF0000) >> 16);
    /** Get Byte3 */
    temp3 = (Byte) ((coeff1_2048Nu & 0x03000000) >> 24);

    /** Gig endian to make 8051 happy */
    buffer[cfoe_NS_2048_coeff1_25_24 - cfoe_NS_2048_coeff1_25_24] = temp3;
    buffer[cfoe_NS_2048_coeff1_23_16 - cfoe_NS_2048_coeff1_25_24] = temp2;
    buffer[cfoe_NS_2048_coeff1_15_8 - cfoe_NS_2048_coeff1_25_24] = temp1;
    buffer[cfoe_NS_2048_coeff1_7_0 - cfoe_NS_2048_coeff1_25_24] = temp0;

    /** Write coeff2_2k */
    /** Get Byte0 */
    temp0 = (Byte) ((coeff2_2k & 0x000000FF));
    /** Get Byte1 */
    temp1 = (Byte) ((coeff2_2k & 0x0000FF00) >> 8);
    /** Get Byte2 */
    temp2 = (Byte) ((coeff2_2k & 0x00FF0000) >> 16);
    /** Get Byte3 */
    temp3 = (Byte) ((coeff2_2k & 0x01000000) >> 24);

    /** Gig endian to make 8051 happy */
    buffer[cfoe_NS_2k_coeff2_24 - cfoe_NS_2048_coeff1_25_24] = temp3;
    buffer[cfoe_NS_2k_coeff2_23_16 - cfoe_NS_2048_coeff1_25_24] = temp2;
    buffer[cfoe_NS_2k_coeff2_15_8 - cfoe_NS_2048_coeff1_25_24] = temp1;
    buffer[cfoe_NS_2k_coeff2_7_0 - cfoe_NS_2048_coeff1_25_24] = temp0;

    /** Write coeff1_8191Nu */
    /** Get Byte0 */
    temp0 = (Byte) ((coeff1_8191Nu & 0x000000FF));
    /** Get Byte1 */
    temp1 = (Byte) ((coeff1_8191Nu & 0x0000FF00) >> 8);
    /** Get Byte2 */
    temp2 = (Byte) ((coeff1_8191Nu & 0x00FFC000) >> 16);
    /** Get Byte3 */
    temp3 = (Byte) ((coeff1_8191Nu & 0x03000000) >> 24);

    /** Big endian to make 8051 happy */
    buffer[cfoe_NS_8191_coeff1_25_24 - cfoe_NS_2048_coeff1_25_24] = temp3;
    buffer[cfoe_NS_8191_coeff1_23_16 - cfoe_NS_2048_coeff1_25_24] = temp2;
    buffer[cfoe_NS_8191_coeff1_15_8 - cfoe_NS_2048_coeff1_25_24] = temp1;
    buffer[cfoe_NS_8191_coeff1_7_0 - cfoe_NS_2048_coeff1_25_24] = temp0;

    /** Write coeff1_8192Nu */
    /** Get Byte0 */
    temp0  = (Byte) (coeff1_8192Nu & 0x000000FF);
    /** Get Byte1 */
    temp1 = (Byte) ((coeff1_8192Nu & 0x0000FF00) >> 8);
    /** Get Byte2 */
    temp2 = (Byte) ((coeff1_8192Nu & 0x00FFC000) >> 16);
    /** Get Byte3 */
    temp3 = (Byte) ((coeff1_8192Nu & 0x03000000) >> 24);

    /** Gig endian to make 8051 happy */
    buffer[cfoe_NS_8192_coeff1_25_24 - cfoe_NS_2048_coeff1_25_24] = temp3;
    buffer[cfoe_NS_8192_coeff1_23_16 - cfoe_NS_2048_coeff1_25_24] = temp2;
    buffer[cfoe_NS_8192_coeff1_15_8 - cfoe_NS_2048_coeff1_25_24] = temp1;
    buffer[cfoe_NS_8192_coeff1_7_0 - cfoe_NS_2048_coeff1_25_24] = temp0;

    /** Write coeff1_8193Nu */
    /** Get Byte0 */
    temp0 = (Byte) ((coeff1_8193Nu & 0x000000FF));
    /** Get Byte1 */
    temp1 = (Byte) ((coeff1_8193Nu & 0x0000FF00) >> 8);
    /** Get Byte2 */
    temp2 = (Byte) ((coeff1_8193Nu & 0x00FFC000) >> 16);
    /** Get Byte3 */
    temp3 = (Byte) ((coeff1_8193Nu & 0x03000000) >> 24);

    /** Big endian to make 8051 happy */
    buffer[cfoe_NS_8193_coeff1_25_24 - cfoe_NS_2048_coeff1_25_24] = temp3;
    buffer[cfoe_NS_8193_coeff1_23_16 - cfoe_NS_2048_coeff1_25_24] = temp2;
    buffer[cfoe_NS_8193_coeff1_15_8 - cfoe_NS_2048_coeff1_25_24] = temp1;
    buffer[cfoe_NS_8193_coeff1_7_0 - cfoe_NS_2048_coeff1_25_24] = temp0;

    /** Write coeff2_8k */
    /** Get Byte0 */
    temp0 = (Byte) ((coeff2_8k & 0x000000FF));
    /** Get Byte1 */
    temp1 = (Byte) ((coeff2_8k & 0x0000FF00) >> 8);
    /** Get Byte2 */
    temp2 = (Byte) ((coeff2_8k & 0x00FF0000) >> 16);
    /** Get Byte3 */
    temp3 = (Byte) ((coeff2_8k & 0x01000000) >> 24);

    /** Big endian to make 8051 happy */
    buffer[cfoe_NS_8k_coeff2_24 - cfoe_NS_2048_coeff1_25_24] = temp3;
    buffer[cfoe_NS_8k_coeff2_23_16 - cfoe_NS_2048_coeff1_25_24] = temp2;
    buffer[cfoe_NS_8k_coeff2_15_8 - cfoe_NS_2048_coeff1_25_24] = temp1;
    buffer[cfoe_NS_8k_coeff2_7_0 - cfoe_NS_2048_coeff1_25_24] = temp0;

    /** Write coeff1_4096Nu */
    /** Get Byte0 */
    temp0 = (Byte) (coeff1_4096Nu & 0x000000FF);
    /** Get Byte1 */
    temp1 = (Byte) ((coeff1_4096Nu & 0x0000FF00) >> 8);
    /** Get Byte2 */
    temp2 = (Byte) ((coeff1_4096Nu & 0x00FF0000) >> 16);
    /** Get Byte3[1:0] */
    /** Bit[7:2] will be written soon and so don't have to care them */
    temp3 = (Byte) ((coeff1_4096Nu & 0x03000000) >> 24);

    /** Big endian to make 8051 happy */
    buffer[cfoe_NS_4096_coeff1_25_24 - cfoe_NS_2048_coeff1_25_24] = temp3;
    buffer[cfoe_NS_4096_coeff1_23_16 - cfoe_NS_2048_coeff1_25_24] = temp2;
    buffer[cfoe_NS_4096_coeff1_15_8 - cfoe_NS_2048_coeff1_25_24] = temp1;
    buffer[cfoe_NS_4096_coeff1_7_0 - cfoe_NS_2048_coeff1_25_24] = temp0;

    /** Write coeff2_4k */
    /** Get Byte0 */
    temp0 = (Byte) ((coeff2_4k & 0x000000FF));
    /** Get Byte1 */
    temp1 = (Byte) ((coeff2_4k & 0x0000FF00) >> 8);
    /** Get Byte2 */
    temp2 = (Byte) ((coeff2_4k & 0x00FF0000) >> 16);
    /** Get Byte3 */
    temp3 = (Byte) ((coeff2_4k & 0x01000000) >> 24);

    /** Big endian to make 8051 happy */
    buffer[cfoe_NS_4k_coeff2_24 - cfoe_NS_2048_coeff1_25_24] = temp3;
    buffer[cfoe_NS_4k_coeff2_23_16 - cfoe_NS_2048_coeff1_25_24] = temp2;
    buffer[cfoe_NS_4k_coeff2_15_8 - cfoe_NS_2048_coeff1_25_24] = temp1;
    buffer[cfoe_NS_4k_coeff2_7_0 - cfoe_NS_2048_coeff1_25_24] = temp0;

    /** Get Byte0 */
    temp0 = (Byte) (bfsfcw_fftindex_ratio & 0x00FF);
    /** Get Byte1 */
    temp1 = (Byte) ((bfsfcw_fftindex_ratio & 0xFF00) >> 8);

    /** Big endian to make 8051 happy */
    buffer[bfsfcw_fftindex_ratio_15_8 - cfoe_NS_2048_coeff1_25_24] = temp1;
    buffer[bfsfcw_fftindex_ratio_7_0 - cfoe_NS_2048_coeff1_25_24] = temp0;

    /** Get Byte0 */
    temp0 = (Byte) (fftindex_bfsfcw_ratio & 0x00FF);
    /** Get Byte1 */
    temp1 = (Byte) ((fftindex_bfsfcw_ratio & 0xFF00) >> 8);

    /** Big endian to make 8051 happy */
    buffer[fftindex_bfsfcw_ratio_15_8 - cfoe_NS_2048_coeff1_25_24] = temp1;
    buffer[fftindex_bfsfcw_ratio_7_0 - cfoe_NS_2048_coeff1_25_24] = temp0;

    error = Standard_writeRegisters (demodulator, chip, Processor_OFDM, cfoe_NS_2048_coeff1_25_24, 36, buffer);
    if (error) goto exit;

exit :
    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_loadFirmware (
    IN  Demodulator*    demodulator,
    IN  Byte*           firmwareCodes,
    IN  Segment*        firmwareSegments,
    IN  Byte*           firmwarePartitions
) {
    Dword error = Error_NO_ERROR;
    Dword beginPartition;
    Dword endPartition;
    Dword version;
    Dword firmwareLength;
    Byte* firmwareCodesPointer;
    Word command;
    Dword i;
    Ganymede* ganymede;

    deb_info("Enter %s -\n", __FUNCTION__);
    ganymede = (Ganymede*) demodulator;

    /** Set I2C master clock speed. */
    error = Standard_writeRegister (demodulator, 0, Processor_LINK, p_reg_one_cycle_counter_tuner, User_I2C_SPEED);
    if (error) goto exit;

    firmwareCodesPointer = firmwareCodes;
    beginPartition = 0;
    endPartition = firmwarePartitions[0];

    for (i = beginPartition; i < endPartition; i++) {
        firmwareLength = firmwareSegments[i].segmentLength;
        if (firmwareSegments[i].segmentType == 0) {
            /** Dwonload firmware */
            error = Standard_sendCommand (demodulator, Command_FW_DOWNLOAD_BEGIN, 0, Processor_LINK, 0, NULL, 0, NULL);
            if (error) goto exit;
            if (ganymede->cmdDescription->loadFirmware != NULL) {
                error = ganymede->cmdDescription->loadFirmware (demodulator, firmwareLength, firmwareCodesPointer);
            }
            if (error) goto exit;
            error = Standard_sendCommand (demodulator, Command_FW_DOWNLOAD_END, 0, Processor_LINK, 0, NULL, 0, NULL);
            if (error) goto exit;
        } else if (firmwareSegments[i].segmentType == 1) {
            /** Copy firmware */
            error = Standard_sendCommand (demodulator, Command_SCATTER_WRITE, 0, Processor_LINK, firmwareLength, firmwareCodesPointer, 0, NULL);
            if (error) goto exit;
        } else {
            /** Direct write firmware */
            command = (Word) (firmwareCodesPointer[0] << 8) + (Word) firmwareCodesPointer[1];
            error = Standard_sendCommand (demodulator, command, 0, Processor_LINK, firmwareLength - 2, firmwareCodesPointer + 2, 0, NULL);
            if (error) goto exit;
        }
        firmwareCodesPointer += firmwareLength;
    }

    /** Boot */
    error = Standard_sendCommand (demodulator, Command_BOOT, 0, Processor_LINK, 0, NULL, 0, NULL);
    if (error) goto exit;

    User_delay (demodulator, 10);

    /** Check if firmware is running */
    version = 0;
    error = Standard_getFirmwareVersion (demodulator, Processor_LINK, &version);
    if (error) goto exit;
    if (version == 0)
        error = Error_BOOT_FAIL;

exit :
    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_loadScript (
    IN  Demodulator*    demodulator,
    IN  StreamType      streamType,
    IN  Word*           scriptSets,
    IN  ValueSet*       scripts,
    IN  Word*           tunerScriptSets,
    IN  ValueSet*       tunerScripts
) {
    Dword error = Error_NO_ERROR;
    Word beginScript;
    Word endScript;
    //Byte adcType = 0;
    Byte i;
    Word j;
	Byte value;
    Ganymede* ganymede;

    deb_info("Enter %s -\n", __FUNCTION__);
    ganymede = (Ganymede*) demodulator;


    if ((scriptSets[0] != 0) && (scripts != NULL))
    {
        beginScript = 0;
        endScript = scriptSets[0];

        for (i = 0; i < ganymede->chipNumber; i++) {
            /** Load OFSM init script */
            for (j = beginScript; j < endScript; j++) {
                error = Standard_writeRegister (demodulator, i, Processor_OFDM, scripts[j].address, scripts[j].value);
                if (error) goto exit;
            }
        }
    }

	/** Detect difference between A and B */
	error = Standard_readRegister (demodulator, 0, Processor_LINK, 0x384F, &value);
	if (error) goto exit;

    if ((tunerScriptSets[0] != 0) && (tunerScripts != NULL))
    {
        if (tunerScriptSets[1] != 0 && value == 0xFF)
        {
            beginScript = tunerScriptSets[0];
            endScript = tunerScriptSets[0] + tunerScriptSets[1];
		} else {
            beginScript = 0;
            endScript = tunerScriptSets[0];
        }

        for (i = 0; i < ganymede->chipNumber; i++) {
            /** Load tuner init script */
            for (j = beginScript; j < endScript; j++) {
                error = Standard_writeRegister (demodulator, i, Processor_OFDM, tunerScripts[j].address, tunerScripts[j].value);
                if (error) goto exit;
            }
        }
    }

exit :
    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_loadIrTable (
    IN  Demodulator*    demodulator,
    IN  Word            tableLength,
    IN  Byte*           table
) {
    Dword error = Error_NO_ERROR;
    Byte baseHigh;
    Byte baseLow;
    Word registerBase;
    Word i;

    deb_info("Enter %s -\n", __FUNCTION__);
    error = Standard_readRegister (demodulator, 0, Processor_LINK, 0x417F, &baseHigh);
    if (error) goto exit;
    error = Standard_readRegister (demodulator, 0, Processor_LINK, 0x4180, &baseLow);
    if (error) goto exit;

    registerBase = (Word) (baseHigh << 8) + (Word) baseLow;

    if (registerBase) {
        for (i = 0; i < tableLength; i++) {
            error = Standard_writeRegister (demodulator, 0, Processor_LINK, registerBase + i, table[i]);
            if (error) goto exit;
        }
    }

exit :
    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_initialize (
    IN  Demodulator*    demodulator,
    IN  Byte            chipNumber,
    IN  Word            sawBandwidth,
    IN  StreamType      streamType,
    IN  Architecture    architecture
) {
    Dword error = Error_NO_ERROR;

#if User_USE_DRIVER
    DWORD number;
    BOOL result;
    InitializeRequest request;
    Ganymede* ganymede;

    ganymede = (Ganymede*) demodulator;

    ganymede->driver = NULL;
    error = Standard_getDriver (demodulator, &ganymede->driver);

    if (ganymede->driver != NULL) {
        request.chipNumber = chipNumber;
        request.sawBandwidth = sawBandwidth;
        request.streamType = streamType;
        request.architecture = architecture;
        result = DeviceIoControl (
                    ganymede->driver,
                    IOCTL_AFA_DEMOD_INITIALIZE,
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
    Dword crystal = 0;
    Dword adc = 0;
    Dword fcw = 0;
    Byte buffer[4];
	Dword version = 0;
    Word* tunerScriptSets = NULL;
    ValueSet* tunerScripts = NULL;


    Byte i;
    Ganymede* ganymede;

    deb_info("Enter %s -\n", __FUNCTION__);
    ganymede = (Ganymede*) demodulator;

    ganymede->chipNumber = chipNumber;
    ganymede->options = 0x0000;
    ganymede->fcw = 0x00000000;
    ganymede->frequency[0] = 642000;
	ganymede->frequency[1] = 642000;
    ganymede->initialized = False;

    if (ganymede->busId == 0xFFFF) {
        goto exit;
    }


    if (ganymede->tunerDescription->tunerId == 0xFFFF) {
        goto exit;
    }

	error = Standard_getFirmwareVersion (demodulator, Processor_LINK, &version);
	if (error) goto exit;
	if (version != 0) {
		ganymede->booted = True;
	} else {
		ganymede->booted = False;
	}




    ganymede->firmwareCodes = Firmware_codes;
    ganymede->firmwareSegments = Firmware_segments;
#if 1 //j017,set to 1  //j014+s 
    ganymede->firmwarePartitions = Firmware_partitions;
#else
    ganymede->firmwarePartitions = Firmware_new_partitions;
#endif //j014+e
    ganymede->scriptSets = Firmware_scriptSets;
    ganymede->scripts = Firmware_scripts;

    /** Set up by default tunerDescription */ /** release1remove */
    tunerScriptSets = ganymede->tunerDescription->tunerScriptSets;
    tunerScripts = ganymede->tunerDescription->tunerScript;


    error = Standard_readRegisterBits (demodulator, 0, Processor_LINK, r_io_mux_pwron_clk_strap, io_mux_pwron_clk_strap_pos, io_mux_pwron_clk_strap_len, &i);
    if (error) goto exit;

    ganymede->crystalFrequency = Standard_clockTable[i].crystalFrequency;
    ganymede->adcFrequency = Standard_clockTable[i].adcFrequency;

    ganymede->dataReady = False;

    /** Write secondary I2C address to device */
    /** it is needed to write i2c address prior to fw-downloading */
    if (ganymede->chipNumber > 1) {
		error = Standard_writeRegister (demodulator, 0, Processor_LINK, 0x417F, ((PDEVICE_CONTEXT)demodulator->userData)->Map.I2C_SLAVE_ADDR);
        if (error) goto exit;
    } else {
        error = Standard_writeRegister (demodulator, 0, Processor_LINK, 0x417F, 0x00);
        if (error) goto exit;
    }

    if (ganymede->firmwareCodes != NULL) {
        if (ganymede->booted == False) {
            error = Standard_loadFirmware (demodulator, ganymede->firmwareCodes, ganymede->firmwareSegments, ganymede->firmwarePartitions);
            if (error) goto exit;
            ganymede->booted = True;
        }
    }

    /** Detect the HostA or HostB */
    error = Standard_readRegisterBits (demodulator, 0, Processor_LINK, r_io_mux_pwron_hosta, io_mux_pwron_hosta_pos, io_mux_pwron_hosta_len, &ganymede->hostInterface[0]);
    if (error) goto exit;


    /** Tell firmware the type of tuner. */
    for (i = 0; i < ganymede->chipNumber; i++) {
		error = Standard_writeRegister (demodulator, i, Processor_LINK, p_reg_link_ofsm_dummy_15_8, (Byte) ganymede->tunerDescription->tunerId);
        if (error) goto exit;

        /** Set read-update bit to 1 for constellation */
        error = Standard_writeRegisterBits (demodulator, i, Processor_OFDM, p_reg_feq_read_update, reg_feq_read_update_pos, reg_feq_read_update_len, 1);
        if (error) goto exit;

        /** Enable FEC Monitor */
        error = Standard_writeRegisterBits (demodulator, i, Processor_OFDM, p_fec_vtb_rsd_mon_en, fec_vtb_rsd_mon_en_pos, fec_vtb_rsd_mon_en_len, 1);
        if (error) goto exit;
    }

    /** Compute ADC and load them to device */
    error = Standard_computeCrystal (demodulator, (Long) ganymede->crystalFrequency * 1000, &crystal);
    if (error) goto exit;

    buffer[0] = (Byte) (crystal & 0x000000FF);
    buffer[1] = (Byte) ((crystal & 0x0000FF00) >> 8);
    buffer[2] = (Byte) ((crystal & 0x00FF0000) >> 16);
    buffer[3] = (Byte) ((crystal & 0xFF000000) >> 24);
    for (i = 0; i < ganymede->chipNumber; i++) {
        error = Standard_writeRegisters (demodulator, i, Processor_OFDM, crystal_clk_7_0, 4, buffer);
        if (error) goto exit;
    }

    /** Compute ADC and load them to device */
    error = Standard_computeAdc (demodulator, (Long) ganymede->adcFrequency, &adc);
    if (error) goto exit;

    buffer[0] = (Byte) (adc & 0x000000FF);
    buffer[1] = (Byte) ((adc & 0x0000FF00) >> 8);
    buffer[2] = (Byte) ((adc & 0x00FF0000) >> 16);
    for (i = 0; i < ganymede->chipNumber; i++) {
        error = Standard_writeRegisters (demodulator, i, Processor_OFDM, p_reg_f_adc_7_0, 3, buffer);
        if (error) goto exit;
    }

    /** Compute FCW and load them to device */
    error = Standard_computeFcw (demodulator, (Long) ganymede->adcFrequency, (Long) ganymede->tunerDescription->ifFrequency, ganymede->tunerDescription->inversion, &fcw);
    if (error) goto exit;
    ganymede->fcw = fcw;

    buffer[0] = (Byte) (fcw & 0x000000FF);
    buffer[1] = (Byte) ((fcw & 0x0000FF00) >> 8);
    buffer[2] = (Byte) ((fcw & 0x007F0000) >> 16);
    for (i = 0; i < ganymede->chipNumber; i++) {
        error = Standard_writeRegisters (demodulator, i, Processor_OFDM, bfs_fcw_7_0, bfs_fcw_22_16 - bfs_fcw_7_0 + 1, buffer);    
        if (error) goto exit;
    }

    /** Set the desired stream type */
    error = Standard_setStreamType (demodulator, streamType);
    if (error) goto exit;

    /** Set the desired architecture type */
    error = Standard_setArchitecture (demodulator, architecture);
    if (error) goto exit;

    if (ganymede->scripts != NULL) {
        error = Standard_loadScript (demodulator, streamType, ganymede->scriptSets, ganymede->scripts, tunerScriptSets, tunerScripts);
        if (error) goto exit;
    }

    if (ganymede->tunerDescription->openTuner != NULL) {
        if ((ganymede->busId != Bus_I2M) && (ganymede->busId != Bus_I2U)) {
            for (i = 0; i < ganymede->chipNumber; i++) {
                error = ganymede->tunerDescription->openTuner (demodulator, i);
                if (error) goto exit;
            }
        }
    }

#if User_USE_INTERRUPT
    if (ganymede->busId == Bus_SDIO) {
        error = Standard_writeRegisterBits (demodulator, 0, Processor_LINK, p_reg_sdioc_external_int_en, reg_sdioc_external_int_en_pos, reg_sdioc_external_int_en_len, 1);
        if (error) goto exit;
    }
#endif

	for (i = 0; i< ganymede->chipNumber; i++) {

		/** Set H/W MPEG2 locked detection **/
		error = Standard_writeRegister (demodulator, i, Processor_LINK, p_reg_top_lock3_out, 1);
		if (error) goto exit;

		/** Set registers for driving power 0xD830 **/
		error = Standard_writeRegister (demodulator, i, Processor_LINK, p_reg_top_padmiscdr2, 1);
		if (error) goto exit;

		/** Set registers for driving power 0xD831 **/
		error = Standard_writeRegister (demodulator, i, Processor_LINK, p_reg_top_padmiscdr4, 0);
		if (error) goto exit;

		/** Set registers for driving power 0xD832 **/
		error = Standard_writeRegister (demodulator, i, Processor_LINK, p_reg_top_padmiscdr8, 0);
		if (error) goto exit;
	}

    ganymede->initialized = True;

#endif
exit:

    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_finalize (
    IN  Demodulator*    demodulator
) {
    Dword error = Error_NO_ERROR;

#if User_USE_DRIVER
    DWORD number;
    BOOL result;
    FinalizeRequest request;
    Ganymede* ganymede;

    ganymede = (Ganymede*) demodulator;

    if (ganymede->driver != NULL) {
        result = DeviceIoControl (
                    ganymede->driver,
                    IOCTL_AFA_DEMOD_FINALIZE,
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
    Ganymede* ganymede;

    deb_info("Enter %s -\n", __FUNCTION__);
    ganymede = (Ganymede*) demodulator;

    if (ganymede->tunerDescription->closeTuner != NULL) {
        for (i = 0; i < ganymede->chipNumber; i++) {
            error = ganymede->tunerDescription->closeTuner (demodulator, i);
        }
    }

#endif

    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_isAgcLocked (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    OUT Bool*           locked
) {
    Dword error = Error_NO_ERROR;

#if User_USE_DRIVER
#else
    Byte temp;

    deb_info("Enter %s -\n", __FUNCTION__);
    *locked = False;

    error = Standard_readRegisterBits (demodulator, chip, Processor_OFDM, p_agc_lock, agc_lock_pos, agc_lock_len, &temp);
    if (error) goto exit;
    if (temp) *locked = True;

exit :
#endif

    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_isCfoeLocked (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    OUT Bool*           locked
) {
    Dword error = Error_NO_ERROR;

#if User_USE_DRIVER
#else
    Byte temp;

    deb_info("Enter %s -\n", __FUNCTION__);
    *locked = False;

    error = Standard_readRegisterBits (demodulator, chip, Processor_OFDM, r_reg_cfoe_divg_flag, reg_cfoe_divg_flag_pos, reg_cfoe_divg_flag_len, &temp);
    if (error) goto exit;
    if (!temp) *locked = True;

exit :
#endif

    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_isSfoeLocked (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    OUT Bool*           locked
) {
    Dword error = Error_NO_ERROR;

#if User_USE_DRIVER
#else
    Byte temp;

    deb_info("Enter %s -\n", __FUNCTION__);
    *locked = False;

    error = Standard_readRegisterBits (demodulator, chip, Processor_OFDM, r_reg_sfoe_divg_flag, reg_sfoe_divg_flag_pos, reg_sfoe_divg_flag_len, &temp);
    if (error) goto exit;
    if (!temp) *locked = True;

exit :
#endif

    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_isTpsLocked (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    OUT Bool*           locked
) {
    Dword error = Error_NO_ERROR;

#if User_USE_DRIVER
#else
    Byte temp;

    deb_info("Enter %s -\n", __FUNCTION__);
    *locked = False;

    error = Standard_readRegisterBits (demodulator, chip, Processor_OFDM, p_fd_tpsd_lock, fd_tpsd_lock_pos, fd_tpsd_lock_len, &temp);
    if (error) goto exit;
    if (temp) *locked = True;

exit :
#endif

    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_isMpeg2Locked (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    OUT Bool*           locked
) {
    Dword error = Error_NO_ERROR;

#if User_USE_DRIVER
#else
    Byte temp;

    deb_info("Enter %s -\n", __FUNCTION__);
    *locked = False;

    error = Standard_readRegisterBits (demodulator, chip, Processor_OFDM, r_mp2if_sync_byte_locked, mp2if_sync_byte_locked_pos, mp2if_sync_byte_locked_len, &temp);
    if (error) goto exit;
    if (temp) *locked = True;

exit :
#endif

    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_isLocked (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    OUT Bool*           locked
) {
    Dword error = Error_NO_ERROR;

    Word emptyLoop = 0;
    Word tpsLoop = 0;
    Word mpeg2Loop = 0;
    Byte channels[2];
    Byte begin;
    Byte end;
    Byte i;
    Byte emptyChannel = 1;
    Byte tpsLocked = 0;

#if User_USE_DRIVER
    DWORD number;
    BOOL result;
    IsLockedRequest request;
    Ganymede* ganymede;

    ganymede = (Ganymede*) demodulator;

    if (ganymede->driver != NULL) {
        request.chip = chip;
        request.locked = locked;
        result = DeviceIoControl (
                    ganymede->driver,
                    IOCTL_AFA_DEMOD_ISLOCKED,
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

    deb_info("Enter %s -\n", __FUNCTION__);
    ganymede = (Ganymede*) demodulator;

    
    *locked = False;

    if (ganymede->architecture == Architecture_DCA) {
        begin = 0;
        end = ganymede->chipNumber;
    } else {
        begin = chip;
        end = begin + 1;
    }

    for (i = begin; i < end; i++) {
        ganymede->statistic[i].signalPresented = False;
        ganymede->statistic[i].signalLocked = False;
        ganymede->statistic[i].signalQuality = 0;
        ganymede->statistic[i].signalStrength = 0;
    }

    channels[0] = 2;
    channels[1] = 2;
    while (emptyLoop < 40) {
        for (i = begin; i < end; i++) {
            error = Standard_readRegister (demodulator, i, Processor_OFDM, empty_channel_status, &channels[i]);
            if (error) goto exit;
        }
        if ((channels[0] == 1) || (channels[1] == 1)) {
            emptyChannel = 0;
            break;
        }
        if ((channels[0] == 2) && (channels[1] == 2)) {
            emptyChannel = 1;
            goto exit;
        }
        User_delay (demodulator, 25);
        emptyLoop++;
    }

    if (emptyChannel == 1) goto exit;

    while (tpsLoop < 50) {
        for (i = begin; i < end; i++) {
            /** Empty channel */
            error = Standard_isTpsLocked (demodulator, i, &ganymede->statistic[i].signalPresented);
            if (error) goto exit;
            if (ganymede->statistic[i].signalPresented == True) {
                tpsLocked = 1;
                break;
            }
        }

        if (tpsLocked == 1) break;

        User_delay (demodulator, 25);
        tpsLoop++;
    }

    if (tpsLocked == 0) goto exit;

    while (mpeg2Loop < 40) {
        if (ganymede->architecture == Architecture_DCA) {
            error = Standard_isMpeg2Locked (demodulator, 0, &ganymede->statistic[0].signalLocked);
            if (error) goto exit;
            if (ganymede->statistic[0].signalLocked == True) {
                for (i = begin; i < end; i++) {
                    ganymede->statistic[i].signalQuality = 80;
                    ganymede->statistic[i].signalStrength = 80;
                }
                *locked = True;
                break;
            }
        } else {
            error = Standard_isMpeg2Locked (demodulator, chip, &ganymede->statistic[chip].signalLocked);
            if (error) goto exit;
            if (ganymede->statistic[chip].signalLocked == True) {
                ganymede->statistic[chip].signalQuality = 80;
                ganymede->statistic[chip].signalStrength = 80;
                *locked = True;
                break;
            }
        }
        User_delay (demodulator, 25);
        mpeg2Loop++;
    }
    for (i = begin; i < end; i++) {
        ganymede->statistic[i].signalQuality = 0;
        ganymede->statistic[i].signalStrength = 20;
    }

exit:
#endif

    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_setPriority (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Priority        priority
)
{
    Dword error = Error_NO_ERROR;

#if User_USE_DRIVER
#else
    Byte pri;
    Byte cr;
    Byte valuew_7_0 = 0;
    Byte valuew_10_8 = 0;
    Byte nfvaluew_7_0 = 0;
    Byte nfvaluew_10_8 = 0;

    deb_info("Enter %s -\n", __FUNCTION__);
    if (priority == Priority_HIGH) {
        pri = 1;
        /** Get code rate here */
        error = Standard_readRegisterBits (demodulator, chip, Processor_OFDM, g_reg_tpsd_hpcr, reg_tpsd_hpcr_pos, reg_tpsd_hpcr_len, &cr);
        if (error) goto exit;
    } else {
        pri = 0;
        /** Get code rate here */
        error = Standard_readRegisterBits (demodulator, chip, Processor_OFDM, g_reg_tpsd_lpcr, reg_tpsd_lpcr_pos, reg_tpsd_lpcr_len, &cr);
        if (error) goto exit;
    }

    error = Standard_writeRegisterBits (demodulator, chip, Processor_OFDM, g_reg_dec_pri, reg_dec_pri_pos, reg_dec_pri_len, pri);
    if (error) goto exit;

    /** Set sbx quantizer */
    switch (cr) {
        case 0 :
            valuew_7_0 = 0x20;
            valuew_10_8 = 0x03;
            nfvaluew_7_0 = 0x00;
            nfvaluew_10_8 = 0x01;
            break;
        case 1 :
            valuew_7_0 = 0x5E;
            valuew_10_8 = 0x01;
            nfvaluew_7_0 = 0x80;
            nfvaluew_10_8 = 0x00;
            break;
        case 2 :
            valuew_7_0 = 0x2C;
            valuew_10_8 = 0x01;
            nfvaluew_7_0 = 0x55;
            nfvaluew_10_8 = 0x00;
            break;
        case 3 :
            valuew_7_0 = 0xFA;
            valuew_10_8 = 0x00;
            nfvaluew_7_0 = 0x40;
            nfvaluew_10_8 = 0x00;
            break;
        case 4 :
            valuew_7_0 = 0xC8;
            valuew_10_8 = 0x00;
            nfvaluew_7_0 = 0x33;
            nfvaluew_10_8 = 0x00;
            break;
    }

    error = Standard_writeRegister (demodulator, chip, Processor_OFDM, p_reg_qnt_valuew_7_0, valuew_7_0);
    if (error) goto exit;
    error = Standard_writeRegister (demodulator, chip, Processor_OFDM, p_reg_qnt_valuew_10_8, valuew_10_8);
    if (error) goto exit;
    error = Standard_writeRegister (demodulator, chip, Processor_OFDM, p_reg_qnt_nfvaluew_7_0, nfvaluew_7_0);
    if (error) goto exit;
    error = Standard_writeRegister (demodulator, chip, Processor_OFDM, p_reg_qnt_nfvaluew_10_8, nfvaluew_10_8);
    if (error) goto exit;

exit :
#endif

    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_reset (
    IN  Demodulator*    demodulator
) {
    Dword error = Error_NO_ERROR;

#if User_USE_DRIVER
#else
    Byte value;
    Byte i;
    Byte j;
    Ganymede* ganymede;

    deb_info("Enter %s -\n", __FUNCTION__);
    ganymede = (Ganymede*) demodulator;

    for (i = 0; i < ganymede->chipNumber; i++) {

        /** Enable OFDM reset */
        error = Standard_writeRegisterBits (demodulator, i, Processor_OFDM, I2C_reg_ofdm_rst_en, reg_ofdm_rst_en_pos, reg_ofdm_rst_en_len, 0x01);
        if (error) goto exit;

        /** Start reset mechanism */
        value = 0x00;
        error = Standard_writeRegisters (demodulator, i, Processor_OFDM, RESET_STATE, 1, &value);
        if (error) goto exit;

        /** Clear ofdm reset */
        for (j = 0; j < 150; j++) {
            error = Standard_readRegisterBits (demodulator, i, Processor_OFDM, I2C_reg_ofdm_rst, reg_ofdm_rst_pos, reg_ofdm_rst_len, &value);
            if (error) goto exit;
            if (value) break;
            User_delay (demodulator, 10);
        }

        if (j == 150) {
            error = Error_RESET_TIMEOUT;
            goto exit;
        }

        error = Standard_writeRegisterBits (demodulator, i, Processor_OFDM, I2C_reg_ofdm_rst, reg_ofdm_rst_pos, reg_ofdm_rst_len, 0);
        if (error) goto exit;

        /** Disable OFDM reset */
        error = Standard_writeRegisterBits (demodulator, i, Processor_OFDM, I2C_reg_ofdm_rst_en, reg_ofdm_rst_en_pos, reg_ofdm_rst_en_len, 0x00);
        if (error) goto exit;
    }

exit :
#endif

    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_programFcw (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Long            shift,          /** Hz */
    IN  Dword           adcFrequency    /** Hz */
)
{
    Dword error = Error_NO_ERROR;
    Dword fcw;
    Long fcwShift;
    Byte temp0;
    Byte temp1;
    Byte temp2;
    Ganymede* ganymede;

    deb_info("Enter %s -\n", __FUNCTION__);
    ganymede = (Ganymede*) demodulator;

    /** Get shift freq */
    fcwShift = (shift * 8 * 1024 + (Long) adcFrequency / (2 * 1024)) / (Long) adcFrequency * 1024;

    fcw  = (Dword) ((Long) ganymede->fcw + fcwShift);

    temp0 = (Byte) (fcw  & 0x000000FF);
    temp1 = (Byte) ((fcw & 0x0000FF00) >> 8);
    temp2 = (Byte) ((fcw & 0x007F0000) >> 16);

    error = Standard_writeRegister (demodulator, chip, Processor_OFDM, p_reg_bfs_fcw_7_0, temp0);
    if (error) goto exit;
    error = Standard_writeRegister (demodulator, chip, Processor_OFDM, p_reg_bfs_fcw_15_8, temp1);
    if (error) goto exit;
    error = Standard_writeRegister (demodulator, chip, Processor_OFDM, p_reg_bfs_fcw_22_16, temp2);
    if (error) goto exit;

exit :
    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_getChannelModulation (
    IN  Demodulator*            demodulator,
    IN  Byte                    chip,
    OUT ChannelModulation*      channelModulation
) {
    Dword error = Error_NO_ERROR;

#if User_USE_DRIVER
    DWORD number;
    BOOL result;
    GetChannelModulationRequest request;
    Ganymede* ganymede;

    ganymede = (Ganymede*) demodulator;

    if (ganymede->driver != NULL) {
        request.chip = chip;
        request.channelModulation = channelModulation;
        result = DeviceIoControl (
                    ganymede->driver,
                    IOCTL_AFA_DEMOD_GETCHANNELMODULATION,
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
    Byte temp;
    Ganymede* ganymede;

    deb_info("Enter %s -\n", __FUNCTION__);
    ganymede = (Ganymede*) demodulator;

    /** Get constellation type */
    error = Standard_readRegisterBits (demodulator, chip, Processor_OFDM, g_reg_tpsd_const, reg_tpsd_const_pos, reg_tpsd_const_len, &temp);
    if (error) goto exit;
    channelModulation->constellation = (Constellation) temp;

    /** Get TPS hierachy and alpha value */
    error = Standard_readRegisterBits (demodulator, chip, Processor_OFDM, g_reg_tpsd_hier, reg_tpsd_hier_pos, reg_tpsd_hier_len, &temp);
    if (error) goto exit;
    channelModulation->hierarchy = (Hierarchy)temp;

    /** Get high/low priority */
    error = Standard_readRegisterBits (demodulator, chip, Processor_OFDM, g_reg_dec_pri, reg_dec_pri_pos, reg_dec_pri_len, &temp);
    if (error) goto exit;
    if (temp)
        channelModulation->priority = Priority_HIGH;
    else
        channelModulation->priority = Priority_LOW;

    /** Get high code rate */
    error = Standard_readRegisterBits (demodulator, chip, Processor_OFDM, g_reg_tpsd_hpcr, reg_tpsd_hpcr_pos, reg_tpsd_hpcr_len, &temp);
    if (error) goto exit;
    channelModulation->highCodeRate = (CodeRate) temp;

    /** Get low code rate */
    error = Standard_readRegisterBits (demodulator, chip, Processor_OFDM, g_reg_tpsd_lpcr, reg_tpsd_lpcr_pos, reg_tpsd_lpcr_len, &temp);
    if (error) goto exit;
    channelModulation->lowCodeRate  = (CodeRate) temp;

    /** Get guard interval */
    error = Standard_readRegisterBits (demodulator, chip, Processor_OFDM, g_reg_tpsd_gi, reg_tpsd_gi_pos, reg_tpsd_gi_len, &temp);
    if (error) goto exit;
    channelModulation->interval = (Interval) temp;

    /** Get FFT mode */
    error = Standard_readRegisterBits (demodulator, chip, Processor_OFDM, g_reg_tpsd_txmod, reg_tpsd_txmod_pos, reg_tpsd_txmod_len, &temp);
    if (error) goto exit;
    channelModulation->transmissionMode = (TransmissionModes) temp;

    /** Get bandwidth */
    error = Standard_readRegisterBits (demodulator, chip, Processor_OFDM, g_reg_bw, reg_bw_pos, reg_bw_len, &temp);
    if (error) goto exit;
    channelModulation->bandwidth = (Bandwidth) temp;

    /** Get frequency */
    channelModulation->frequency = ganymede->frequency[chip];

exit :
#endif

    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_setChannelModulation (
    IN  Demodulator*            demodulator,
    IN  Byte                    chip,
    IN  ChannelModulation*      channelModulation
) {
    Dword error = Error_NO_ERROR;

#if User_USE_DRIVER
#else
    Byte temp;
    Ganymede* ganymede;

    deb_info("Enter %s -\n", __FUNCTION__);
    ganymede = (Ganymede*) demodulator;

    /** Set constellation type */
    error = Standard_writeRegisterBits (demodulator, chip, Processor_OFDM, g_reg_tpsd_const, reg_tpsd_const_pos, reg_tpsd_const_len, (Byte) channelModulation->constellation);
    if (error) goto exit;

    /** Set TPS hierachy and alpha value */
    error = Standard_writeRegisterBits (demodulator, chip, Processor_OFDM, g_reg_tpsd_hier, reg_tpsd_hier_pos, reg_tpsd_hier_len, (Byte) channelModulation->hierarchy);
    if (error) goto exit;

    /** Set high/low priority */
    if (channelModulation->priority == Priority_HIGH)
        temp = 1;
    else
        temp = 0;
    error = Standard_writeRegisterBits (demodulator, chip, Processor_OFDM, g_reg_dec_pri, reg_dec_pri_pos, reg_dec_pri_len, temp);
    if (error) goto exit;

    /** Set high code rate */
    error = Standard_writeRegisterBits (demodulator, chip, Processor_OFDM, g_reg_tpsd_hpcr, reg_tpsd_hpcr_pos, reg_tpsd_hpcr_len, (Byte) channelModulation->highCodeRate);
    if (error) goto exit;

    /** Set low code rate */
    error = Standard_writeRegisterBits (demodulator, chip, Processor_OFDM, g_reg_tpsd_lpcr, reg_tpsd_lpcr_pos, reg_tpsd_lpcr_len, (Byte) channelModulation->lowCodeRate);
    if (error) goto exit;

    /** Set guard interval */
    error = Standard_writeRegisterBits (demodulator, chip, Processor_OFDM, g_reg_tpsd_gi, reg_tpsd_gi_pos, reg_tpsd_gi_len, (Byte) channelModulation->interval);
    if (error) goto exit;

    /** Set FFT mode */
    error = Standard_writeRegisterBits (demodulator, chip, Processor_OFDM, g_reg_tpsd_txmod, reg_tpsd_txmod_pos, reg_tpsd_txmod_len, (Byte) channelModulation->transmissionMode);
    if (error) goto exit;

    /** Set bandwidth */
    error = Standard_writeRegisterBits (demodulator, chip, Processor_OFDM, g_reg_bw, reg_bw_pos, reg_bw_len, (Byte) channelModulation->bandwidth);
    if (error) goto exit;

exit :
#endif

    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}

Dword Standard_setFrequency (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Dword           frequency
) {
    Dword error = Error_NO_ERROR;
    Byte band;
    Byte i;
    Ganymede* ganymede;

    deb_info("Enter %s -\n", __FUNCTION__);
    ganymede = (Ganymede*) demodulator;

    /** Clear easy mode flag first */
    error = Standard_writeRegister (demodulator, chip, Processor_OFDM, Training_Mode, 0x00);
    if (error) goto exit;

    /** Clear empty_channel_status lock flag */
    error = Standard_writeRegister (demodulator, chip, Processor_OFDM, empty_channel_status, 0x00);
    if (error) goto exit;

    /** Clear MPEG2 lock flag */
    error = Standard_writeRegisterBits (demodulator, chip, Processor_OFDM, r_mp2if_sync_byte_locked, mp2if_sync_byte_locked_pos, mp2if_sync_byte_locked_len, 0x00);
    if (error) goto exit;

    /** Determine frequency band */
    band = 0xFF;
    for (i = 0; i < Standard_MAX_BAND; i++) {
        if ((frequency >= Standard_bandTable[i].minimum) && (frequency <= Standard_bandTable[i].maximum)) {
            band = i;
            break;
        }
    }
    error = Standard_writeRegister (demodulator, chip, Processor_OFDM, FreBand, band);
    if (error) goto exit;

    if (ganymede->tunerDescription->setTuner != NULL) {
        if ((ganymede->busId != Bus_I2M) && (ganymede->busId != Bus_I2U)) {
            error = ganymede->tunerDescription->setTuner (demodulator, chip, ganymede->bandwidth[chip], frequency);
            if (error) goto exit;
        }
    }

    /** Trigger ofsm */
    error = Standard_writeRegister (demodulator, chip, Processor_OFDM, trigger_ofsm, 0);
    if (error) goto exit;

    ganymede->frequency[chip] = frequency;

exit :
    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_setFrequencyForRetrain (
    IN  Demodulator*            demodulator,
    IN  Byte                    chip,
    IN  Dword                   frequency,
    IN  Word                    bandwidth,
    IN  ChannelModulation*      channelModulation
)
{
    Dword error = Error_NO_ERROR;
    Ganymede* ganymede;

    deb_info("Enter %s -\n", __FUNCTION__);
    ganymede = (Ganymede*) demodulator;

    /** Clear MPEG2 lock flag first */
    error = Standard_writeRegisterBits (demodulator, chip, Processor_OFDM, r_mp2if_sync_byte_locked, mp2if_sync_byte_locked_pos, mp2if_sync_byte_locked_len, 0);
    if (error) goto exit;

    /** Set frequency for all chips */
    if (ganymede->tunerDescription->setTuner != NULL) {
        error = ganymede->tunerDescription->setTuner (demodulator, chip, bandwidth, frequency);
        if (error) goto exit;
    }

    /** Trigger ofsm */
    error = Standard_writeRegister (demodulator, chip, Processor_OFDM, trigger_ofsm, 0);
    if (error) goto exit;

exit :
    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_maskDcaOutput (
    IN  Demodulator*    demodulator
) {
    Dword error = Error_NO_ERROR;
    Byte i;
    Bool dcaValid = False;
    Ganymede* ganymede;

    deb_info("Enter %s -\n", __FUNCTION__);
    ganymede = (Ganymede*) demodulator;

    if ((ganymede->chipNumber > 1) && (ganymede->architecture == Architecture_DCA))
        dcaValid = True;

    if (dcaValid == True) {
        for (i = 0; i < ganymede->chipNumber; i++) {
            error = Standard_writeRegisterBits (demodulator, i, Processor_OFDM, p_reg_dca_upper_out_en, reg_dca_upper_out_en_pos, reg_dca_upper_out_en_len, 0);
            if (error) goto exit;
        }
        User_delay (demodulator, 5);
    }

exit :
    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_acquireChannel (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Word            bandwidth,
    IN  Dword           frequency
) {
    Dword error = Error_NO_ERROR;

    Byte begin;
    Byte end;
    Byte i;

#if User_USE_DRIVER
    DWORD number;
    BOOL result;
    AcquireChannelRequest request;
    Ganymede* ganymede;

    ganymede = (Ganymede*) demodulator;

    if (ganymede->driver != NULL) {
        request.chip = chip;
        request.frequency = frequency;
        request.bandwidth = bandwidth;
        result = DeviceIoControl (
                    ganymede->driver,
                    IOCTL_AFA_DEMOD_ACQUIRECHANNEL,
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
    deb_info("Enter %s , bw=%d, freq=%d\n", __FUNCTION__, bandwidth, frequency);

    ganymede = (Ganymede*) demodulator;

    if (ganymede->architecture == Architecture_DCA) {
        begin = 0;
        end = ganymede->chipNumber;
    } else {
        begin = chip;
        end = begin + 1;
    }

    for (i = begin; i < end; i++) {
        error = Standard_selectBandwidth (demodulator, i, bandwidth, ganymede->adcFrequency);
        if (error) goto exit;
		ganymede->bandwidth[i] = bandwidth;
    }

    error = Standard_maskDcaOutput (demodulator);
    if (error) goto exit;

    /** Set frequency */
    for (i = begin; i < end; i++) {
        error = Standard_setFrequency (demodulator, i, frequency);
        if (error) goto exit;
    }

exit :
#endif

    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_setStreamType (
    IN  Demodulator*    demodulator,
    IN  StreamType      streamType
) {
    Dword error = Error_NO_ERROR;

#if User_USE_DRIVER
    DWORD number;
    BOOL result;
    SetStreamTypeRequest request;
    Ganymede* ganymede;

    ganymede = (Ganymede*) demodulator;

    if (ganymede->driver != NULL) {
        request.streamType = streamType;
        result = DeviceIoControl (
                    ganymede->driver,
                    IOCTL_AFA_DEMOD_SETSTREAMTYPE,
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
    //Dword warning = Error_NO_ERROR;
    Ganymede* ganymede;
    Byte i;

    deb_info("Enter %s -\n", __FUNCTION__);
    ganymede = (Ganymede*) demodulator;

    /** Enable DVB-T interrupt if next stream type is StreamType_DVBT_DATAGRAM */
    if (streamType == StreamType_DVBT_DATAGRAM) {
        for (i = 0; i < ganymede->chipNumber; i++) {
            error = Standard_writeRegisterBits (demodulator, i, Processor_LINK, p_reg_dvbt_inten, reg_dvbt_inten_pos, reg_dvbt_inten_len, 1);
            if (error) goto exit;
            if ((ganymede->busId == Bus_USB) || (ganymede->busId == Bus_USB11)) {
                error = Standard_writeRegisterBits (demodulator, i, Processor_OFDM, p_reg_mpeg_full_speed, reg_mpeg_full_speed_pos, reg_mpeg_full_speed_len, 0);
                if (error) goto exit;
            } else {
                error = Standard_writeRegisterBits (demodulator, i, Processor_OFDM, p_reg_mpeg_full_speed, reg_mpeg_full_speed_pos, reg_mpeg_full_speed_len, 1);
                if (error) goto exit;
            }
        }
    }

    /** Enable DVB-T mode */
    for (i = 0; i < ganymede->chipNumber; i++) {
        error = Standard_writeRegisterBits (demodulator, i, Processor_LINK, p_reg_dvbt_en, reg_dvbt_en_pos, reg_dvbt_en_len, 1);
        if (error) goto exit;
    }

    /** Enter sub mode */
    switch (streamType) {

	case StreamType_NONE : 
		goto exit;
		break;

        case StreamType_DVBT_DATAGRAM :
            if ((ganymede->busId == Bus_USB) || (ganymede->busId == Bus_USB11)) {
                error = Standard_writeRegisterBits (demodulator, 0, Processor_OFDM, p_mp2if_mpeg_ser_mode, mp2if_mpeg_ser_mode_pos, mp2if_mpeg_ser_mode_len, 0);
                if (error) goto exit;
                error = Standard_writeRegisterBits (demodulator, 0, Processor_OFDM, p_mp2if_mpeg_par_mode, mp2if_mpeg_par_mode_pos, mp2if_mpeg_par_mode_len, 0);
                if (error) goto exit;
                /** Fix current leakage */
                if (ganymede->chipNumber > 1) {
                    if (ganymede->hostInterface[0]) {
                        error = Standard_writeRegisterBits (demodulator, 0, Processor_LINK, p_reg_top_hostb_mpeg_ser_mode, reg_top_hostb_mpeg_ser_mode_pos, reg_top_hostb_mpeg_ser_mode_len, 0);
                        if (error) goto exit;
                        error = Standard_writeRegisterBits (demodulator, 0, Processor_LINK, p_reg_top_hostb_mpeg_par_mode, reg_top_hostb_mpeg_par_mode_pos, reg_top_hostb_mpeg_par_mode_len, 1);
                        if (error) goto exit;
                    } else {
                        error = Standard_writeRegisterBits (demodulator, 0, Processor_LINK, p_reg_top_hosta_mpeg_ser_mode, reg_top_hosta_mpeg_ser_mode_pos, reg_top_hosta_mpeg_ser_mode_len, 0);
                        if (error) goto exit;
                        error = Standard_writeRegisterBits (demodulator, 0, Processor_LINK, p_reg_top_hosta_mpeg_par_mode, reg_top_hosta_mpeg_par_mode_pos, reg_top_hosta_mpeg_par_mode_len, 1);
                        if (error) goto exit;
                    }
                } else {
                    error = Standard_writeRegisterBits (demodulator, 0, Processor_LINK, p_reg_top_hosta_mpeg_ser_mode, reg_top_hosta_mpeg_ser_mode_pos, reg_top_hosta_mpeg_ser_mode_len, 0);
                    if (error) goto exit;
                    error = Standard_writeRegisterBits (demodulator, 0, Processor_LINK, p_reg_top_hosta_mpeg_par_mode, reg_top_hosta_mpeg_par_mode_pos, reg_top_hosta_mpeg_par_mode_len, 1);
                    if (error) goto exit;
                    error = Standard_writeRegisterBits (demodulator, 0, Processor_LINK, p_reg_top_hostb_mpeg_ser_mode, reg_top_hostb_mpeg_ser_mode_pos, reg_top_hostb_mpeg_ser_mode_len, 0);
                    if (error) goto exit;
                    error = Standard_writeRegisterBits (demodulator, 0, Processor_LINK, p_reg_top_hostb_mpeg_par_mode, reg_top_hostb_mpeg_par_mode_pos, reg_top_hostb_mpeg_par_mode_len, 1);
                    if (error) goto exit;
                }
            } else {
                error = Standard_writeRegisterBits (demodulator, 0, Processor_OFDM, p_mp2if_mpeg_ser_mode, mp2if_mpeg_ser_mode_pos, mp2if_mpeg_ser_mode_len, 0);
                if (error) goto exit;
                error = Standard_writeRegisterBits (demodulator, 0, Processor_OFDM, p_mp2if_mpeg_par_mode, mp2if_mpeg_par_mode_pos, mp2if_mpeg_par_mode_len, 0);
                if (error) goto exit;

                /** Fix current leakage */
                if (ganymede->chipNumber > 1) {
                    error = Standard_writeRegisterBits (demodulator, 0, Processor_LINK, p_reg_top_hosta_mpeg_ser_mode, reg_top_hosta_mpeg_ser_mode_pos, reg_top_hosta_mpeg_ser_mode_len, 0);
                    if (error) goto exit;
                    error = Standard_writeRegisterBits (demodulator, 0, Processor_LINK, p_reg_top_hosta_mpeg_par_mode, reg_top_hosta_mpeg_par_mode_pos, reg_top_hosta_mpeg_par_mode_len, 0);
                    if (error) goto exit;
                    error = Standard_writeRegisterBits (demodulator, 0, Processor_LINK, p_reg_top_hostb_mpeg_ser_mode, reg_top_hostb_mpeg_ser_mode_pos, reg_top_hostb_mpeg_ser_mode_len, 0);
                    if (error) goto exit;
                    error = Standard_writeRegisterBits (demodulator, 0, Processor_LINK, p_reg_top_hostb_mpeg_par_mode, reg_top_hostb_mpeg_par_mode_pos, reg_top_hostb_mpeg_par_mode_len, 0);
                    if (error) goto exit;
                } else {
                    if (ganymede->hostInterface[0]) {
                        error = Standard_writeRegisterBits (demodulator, 0, Processor_LINK, p_reg_top_hostb_mpeg_ser_mode, reg_top_hostb_mpeg_ser_mode_pos, reg_top_hostb_mpeg_ser_mode_len, 0);
                        if (error) goto exit;
                        error = Standard_writeRegisterBits (demodulator, 0, Processor_LINK, p_reg_top_hostb_mpeg_par_mode, reg_top_hostb_mpeg_par_mode_pos, reg_top_hostb_mpeg_par_mode_len, 1);
                        if (error) goto exit;
                    } else {
                        error = Standard_writeRegisterBits (demodulator, 0, Processor_LINK, p_reg_top_hosta_mpeg_ser_mode, reg_top_hosta_mpeg_ser_mode_pos, reg_top_hosta_mpeg_ser_mode_len, 0);
                        if (error) goto exit;
                        error = Standard_writeRegisterBits (demodulator, 0, Processor_LINK, p_reg_top_hosta_mpeg_par_mode, reg_top_hosta_mpeg_par_mode_pos, reg_top_hosta_mpeg_par_mode_len, 1);
                        if (error) goto exit;
                    }
                }
            }
            break;
        case StreamType_DVBT_PARALLEL :
            for (i = 0; i < ganymede->chipNumber; i++) {
                error = Standard_writeRegisterBits (demodulator, i, Processor_OFDM, p_mp2if_mpeg_ser_mode, mp2if_mpeg_ser_mode_pos, mp2if_mpeg_ser_mode_len, 0);
                if (error) goto exit;
                error = Standard_writeRegisterBits (demodulator, i, Processor_OFDM, p_mp2if_mpeg_par_mode, mp2if_mpeg_par_mode_pos, mp2if_mpeg_par_mode_len, 1);
                if (error) goto exit;

                if (i == 0) {
                    if (ganymede->hostInterface[0]) {
                        /** HostA interface is enabled */
                        error = Standard_writeRegisterBits (demodulator, i, Processor_LINK, p_reg_top_hosta_mpeg_ser_mode, reg_top_hosta_mpeg_ser_mode_pos, reg_top_hosta_mpeg_ser_mode_len, 0);
                        if (error) goto exit;
                        error = Standard_writeRegisterBits (demodulator, i, Processor_LINK, p_reg_top_hosta_mpeg_par_mode, reg_top_hosta_mpeg_par_mode_pos, reg_top_hosta_mpeg_par_mode_len, 1);
                        if (error) goto exit;
                    } else {
                        /** HostB interface is enabled */
                        error = Standard_writeRegisterBits (demodulator, i, Processor_LINK, p_reg_top_hostb_mpeg_ser_mode, reg_top_hostb_mpeg_ser_mode_pos, reg_top_hostb_mpeg_ser_mode_len, 0);
                        if (error) goto exit;
                        error = Standard_writeRegisterBits (demodulator, i, Processor_LINK, p_reg_top_hostb_mpeg_par_mode, reg_top_hostb_mpeg_par_mode_pos, reg_top_hostb_mpeg_par_mode_len, 1);
                        if (error) goto exit;
                    }
                } else {
                    /** HostA interface is enabled */
                    error = Standard_writeRegisterBits (demodulator, i, Processor_LINK, p_reg_top_hosta_mpeg_ser_mode, reg_top_hosta_mpeg_ser_mode_pos, reg_top_hosta_mpeg_ser_mode_len, 0);
                    if (error) goto exit;
                    error = Standard_writeRegisterBits (demodulator, i, Processor_LINK, p_reg_top_hosta_mpeg_par_mode, reg_top_hosta_mpeg_par_mode_pos, reg_top_hosta_mpeg_par_mode_len, 1);
                    if (error) goto exit;
                }
            }
            break;
        case StreamType_DVBT_SERIAL :
            for (i = 0; i < ganymede->chipNumber; i++) {
                error = Standard_writeRegisterBits (demodulator, i, Processor_OFDM, p_mp2if_mpeg_ser_mode, mp2if_mpeg_ser_mode_pos, mp2if_mpeg_ser_mode_len, 1);
                if (error) goto exit;

                if (i == 0) {
                    if (ganymede->hostInterface[0]) {
                        /** HostA interface is enabled */
                        error = Standard_writeRegisterBits (demodulator, i, Processor_LINK, p_reg_top_hosta_mpeg_ser_mode, reg_top_hosta_mpeg_ser_mode_pos, reg_top_hosta_mpeg_ser_mode_len, 1);
                        if (error) goto exit;
                    } else {
                        /** HostB interface is enabled */
                        error = Standard_writeRegisterBits (demodulator, i, Processor_LINK, p_reg_top_hostb_mpeg_ser_mode, reg_top_hostb_mpeg_ser_mode_pos, reg_top_hostb_mpeg_ser_mode_len, 1);
                        if (error) goto exit;
                    }
                } else {
                    /** HostA interface is enabled */
                    error = Standard_writeRegisterBits (demodulator, i, Processor_LINK, p_reg_top_hosta_mpeg_ser_mode, reg_top_hosta_mpeg_ser_mode_pos, reg_top_hosta_mpeg_ser_mode_len, 1);
                    if (error) goto exit;
                }
            }
            break;
    }
    error = User_mpegConfig (demodulator);

    ganymede->streamType = streamType;

exit :
#endif

    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_setArchitecture (
    IN  Demodulator*    demodulator,
    IN  Architecture    architecture
) {
    Dword error = Error_NO_ERROR;

#if User_USE_DRIVER
    DWORD number;
    BOOL result;
    SetArchitectureRequest request;
    Ganymede* ganymede;

    ganymede = (Ganymede*) demodulator;

    if (ganymede->driver != NULL) {
        request.architecture = architecture;
        result = DeviceIoControl (
                    ganymede->driver,
                    IOCTL_AFA_DEMOD_SETARCHITECTURE,
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
    Word frameSize;
    Byte packetSize;
    Byte buffer[2];
    Byte standAlone[2];
    Byte upperChip[2];
    Byte upperHost[2];
    Byte lowerChip[2];
    Byte lowerHost[2];
    Byte dcaEnable[2];
    Byte phaseLatch[2];
    Byte fpgaLatch[2];
    Byte i;
    Bool pipValid = False;
    Ganymede* ganymede;

    deb_info("Enter %s -\n", __FUNCTION__);
    ganymede = (Ganymede*) demodulator;

    if (architecture == Architecture_DCA) {
        for (i = 0; i < ganymede->chipNumber; i++) {
            standAlone[i] = 0;
            upperChip[i] = 0;
            upperHost[i] = 0;
            lowerChip[i] = 0;
            lowerHost[i] = 0;
            dcaEnable[i] = 1;
            phaseLatch[i] = 0;
            fpgaLatch[i] = 0;
        }
        if (ganymede->chipNumber == 1) {
            standAlone[0] = 1;
            dcaEnable[0] = 0;
        } else {
            upperChip[ganymede->chipNumber - 1] = 1;
            upperHost[0] = 1;
            lowerChip[0] = 1;
            lowerHost[ganymede->chipNumber - 1] = 1;
            phaseLatch[0] = 1;
            phaseLatch[ganymede->chipNumber - 1] = 1;
            fpgaLatch[0] = 0x66;
            fpgaLatch[ganymede->chipNumber - 1] = 0x66;
        }
    } else {
        for (i = 0; i < ganymede->chipNumber; i++) {
            standAlone[i] = 1;
            upperChip[i] = 0;
            upperHost[i] = 0;
            lowerChip[i] = 0;
            lowerHost[i] = 0;
            dcaEnable[i] = 0;
            phaseLatch[i] = 0;
            fpgaLatch[i] = 0;
        }
    }

    if (ganymede->initialized == True) {
        error = Standard_maskDcaOutput (demodulator);
        if (error) goto exit;
    }

    /** Set upper chip first in order to avoid I/O conflict */
    for (i = ganymede->chipNumber; i > 0; i--) {
        /** Set dca_upper_chip */
        error = Standard_writeRegisterBits (demodulator, i - 1, Processor_OFDM, p_reg_dca_upper_chip, reg_dca_upper_chip_pos, reg_dca_upper_chip_len, upperChip[i - 1]);
        if (error) goto exit;
        if (i == 1) {
            if ((ganymede->busId == Bus_USB) || (ganymede->busId == Bus_USB11)) {
                if (ganymede->hostInterface[0]) {
                    error = Standard_writeRegisterBits (demodulator, i - 1, Processor_LINK, p_reg_top_hosta_dca_upper, reg_top_hosta_dca_upper_pos, reg_top_hosta_dca_upper_len, upperHost[i - 1]);
                    if (error) goto exit;
                    error = Standard_writeRegisterBits (demodulator, i - 1, Processor_LINK, p_reg_top_hostb_dca_upper, reg_top_hostb_dca_upper_pos, reg_top_hostb_dca_upper_len, 0);
                    if (error) goto exit;
                } else {
                    error = Standard_writeRegisterBits (demodulator, i - 1, Processor_LINK, p_reg_top_hostb_dca_upper, reg_top_hostb_dca_upper_pos, reg_top_hostb_dca_upper_len, upperHost[i - 1]);
                    if (error) goto exit;
                    error = Standard_writeRegisterBits (demodulator, i - 1, Processor_LINK, p_reg_top_hosta_dca_upper, reg_top_hosta_dca_upper_pos, reg_top_hosta_dca_upper_len, 0);
                    if (error) goto exit;
                }
            } else {
                if (ganymede->hostInterface[0]) {
                    error = Standard_writeRegisterBits (demodulator, i - 1, Processor_LINK, p_reg_top_hostb_dca_upper, reg_top_hostb_dca_upper_pos, reg_top_hostb_dca_upper_len, upperHost[i - 1]);
                    if (error) goto exit;
                    error = Standard_writeRegisterBits (demodulator, i - 1, Processor_LINK, p_reg_top_hosta_dca_upper, reg_top_hosta_dca_upper_pos, reg_top_hosta_dca_upper_len, 0);
                    if (error) goto exit;
                } else {
                    error = Standard_writeRegisterBits (demodulator, i - 1, Processor_LINK, p_reg_top_hosta_dca_upper, reg_top_hosta_dca_upper_pos, reg_top_hosta_dca_upper_len, upperHost[i - 1]);
                    if (error) goto exit;
                    error = Standard_writeRegisterBits (demodulator, i - 1, Processor_LINK, p_reg_top_hostb_dca_upper, reg_top_hostb_dca_upper_pos, reg_top_hostb_dca_upper_len, 0);
                    if (error) goto exit;
                }
            }
        } else {
            error = Standard_writeRegisterBits (demodulator, i - 1, Processor_LINK, p_reg_top_hostb_dca_upper, reg_top_hostb_dca_upper_pos, reg_top_hostb_dca_upper_len, upperHost[i - 1]);
            if (error) goto exit;
            error = Standard_writeRegisterBits (demodulator, i - 1, Processor_LINK, p_reg_top_hosta_dca_upper, reg_top_hosta_dca_upper_pos, reg_top_hosta_dca_upper_len, 0);
            if (error) goto exit;
        }

        /** Set dca_lower_chip */
        error = Standard_writeRegisterBits (demodulator, i - 1, Processor_OFDM, p_reg_dca_lower_chip, reg_dca_lower_chip_pos, reg_dca_lower_chip_len, lowerChip[i - 1]);
        if (error) goto exit;
        if (i == 1) {
            if ((ganymede->busId == Bus_USB) || (ganymede->busId == Bus_USB11)) {
                if (ganymede->hostInterface[0]) {
                    error = Standard_writeRegisterBits (demodulator, i - 1, Processor_LINK, p_reg_top_hosta_dca_lower, reg_top_hosta_dca_lower_pos, reg_top_hosta_dca_lower_len, lowerHost[i - 1]);
                    if (error) goto exit;
                    error = Standard_writeRegisterBits (demodulator, i - 1, Processor_LINK, p_reg_top_hostb_dca_lower, reg_top_hostb_dca_lower_pos, reg_top_hostb_dca_lower_len, 0);
                    if (error) goto exit;
                } else {
                    error = Standard_writeRegisterBits (demodulator, i - 1, Processor_LINK, p_reg_top_hostb_dca_lower, reg_top_hostb_dca_lower_pos, reg_top_hostb_dca_lower_len, lowerHost[i - 1]);
                    if (error) goto exit;
                    error = Standard_writeRegisterBits (demodulator, i - 1, Processor_LINK, p_reg_top_hosta_dca_lower, reg_top_hosta_dca_lower_pos, reg_top_hosta_dca_lower_len, 0);
                    if (error) goto exit;
                }
            } else {
                if (ganymede->hostInterface[0]) {
                    error = Standard_writeRegisterBits (demodulator, i - 1, Processor_LINK, p_reg_top_hostb_dca_lower, reg_top_hostb_dca_lower_pos, reg_top_hostb_dca_lower_len, lowerHost[i - 1]);
                    if (error) goto exit;
                    error = Standard_writeRegisterBits (demodulator, i - 1, Processor_LINK, p_reg_top_hosta_dca_lower, reg_top_hosta_dca_lower_pos, reg_top_hosta_dca_lower_len, 0);
                    if (error) goto exit;
                } else {
                    error = Standard_writeRegisterBits (demodulator, i - 1, Processor_LINK, p_reg_top_hosta_dca_lower, reg_top_hosta_dca_lower_pos, reg_top_hosta_dca_lower_len, lowerHost[i - 1]);
                    if (error) goto exit;
                    error = Standard_writeRegisterBits (demodulator, i - 1, Processor_LINK, p_reg_top_hostb_dca_lower, reg_top_hostb_dca_lower_pos, reg_top_hostb_dca_lower_len, 0);
                    if (error) goto exit;
                }
            }
        } else {
            error = Standard_writeRegisterBits (demodulator, i - 1, Processor_LINK, p_reg_top_hostb_dca_lower, reg_top_hostb_dca_lower_pos, reg_top_hostb_dca_lower_len, lowerHost[i - 1]);
            if (error) goto exit;
            error = Standard_writeRegisterBits (demodulator, i - 1, Processor_LINK, p_reg_top_hosta_dca_lower, reg_top_hosta_dca_lower_pos, reg_top_hosta_dca_lower_len, 0);
            if (error) goto exit;
        }

        /** Set phase latch */
        error = Standard_writeRegisterBits (demodulator, i - 1, Processor_OFDM, p_reg_dca_platch, reg_dca_platch_pos, reg_dca_platch_len, phaseLatch[i - 1]);
        if (error) goto exit;

        /** Set fpga latch */
        error = Standard_writeRegisterBits (demodulator, i - 1, Processor_OFDM, p_reg_dca_fpga_latch, reg_dca_fpga_latch_pos, reg_dca_fpga_latch_len, fpgaLatch[i - 1]);
        if (error) goto exit;
    }

    for (i = 0; i < ganymede->chipNumber; i++) {
        /** Set stand alone */
        error = Standard_writeRegisterBits (demodulator, i, Processor_OFDM, p_reg_dca_stand_alone, reg_dca_stand_alone_pos, reg_dca_stand_alone_len, standAlone[i]);
        if (error) goto exit;

        /** Set DCA enable */
        error = Standard_writeRegisterBits (demodulator, i, Processor_OFDM, p_reg_dca_en, reg_dca_en_pos, reg_dca_en_len, dcaEnable[i]);
        if (error) goto exit;
    }

    if (ganymede->initialized == True) {
        for (i = 0; i < ganymede->chipNumber; i++) {
            error = Standard_writeRegister (demodulator, i, Processor_OFDM, trigger_ofsm, 0);
            if (error) goto exit;
        }
    }


    if ((ganymede->busId == Bus_USB) || (ganymede->busId == Bus_USB11)) {
        frameSize = User_USB20_FRAME_SIZE_DW;
        packetSize = (Byte) (User_USB20_MAX_PACKET_SIZE / 4);

        if (ganymede->busId == Bus_USB11) {
            frameSize   = User_USB11_FRAME_SIZE_DW;
            packetSize = (Byte) (User_USB11_MAX_PACKET_SIZE / 4);
        }

        if ((ganymede->chipNumber > 1) && (architecture == Architecture_PIP))
            pipValid = True;
#if 0 //j011+s
        /** Reset EP4 */
        error = Standard_writeRegisterBits (demodulator, 0, Processor_OFDM, p_reg_mp2_sw_rst, reg_mp2_sw_rst_pos, reg_mp2_sw_rst_len, 1);
        if (error) goto exit;
#endif //j011+e

        /** Reset EP5 */
        error = Standard_writeRegisterBits (demodulator, 0, Processor_OFDM, p_reg_mp2if2_sw_rst, reg_mp2if2_sw_rst_pos, reg_mp2if2_sw_rst_len, 1);
        if (error) goto exit;

        /** Disable EP4 */
        error = Standard_writeRegisterBits (demodulator, 0, Processor_LINK, p_reg_ep4_tx_en, reg_ep4_tx_en_pos, reg_ep4_tx_en_len, 0);
        if (error) goto exit;

        /** Disable EP5 */
        error = Standard_writeRegisterBits (demodulator, 0, Processor_LINK, p_reg_ep5_tx_en, reg_ep5_tx_en_pos, reg_ep5_tx_en_len, 0);
        if (error) goto exit;

        /** Disable EP4 NAK */
        error = Standard_writeRegisterBits (demodulator, 0, Processor_LINK, p_reg_ep4_tx_nak, reg_ep4_tx_nak_pos, reg_ep4_tx_nak_len, 0);
        if (error) goto exit;

        /** Disable EP5 NAK */
        error = Standard_writeRegisterBits (demodulator, 0, Processor_LINK, p_reg_ep5_tx_nak, reg_ep5_tx_nak_pos, reg_ep5_tx_nak_len, 0);
        if (error) goto exit;

        /** Enable EP4 */
        error = Standard_writeRegisterBits (demodulator, 0, Processor_LINK, p_reg_ep4_tx_en, reg_ep4_tx_en_pos, reg_ep4_tx_en_len, 1);
        if (error) goto exit;

        /** Set EP4 transfer length */
        buffer[p_reg_ep4_tx_len_7_0 - p_reg_ep4_tx_len_7_0] = (Byte) frameSize;
        buffer[p_reg_ep4_tx_len_15_8 - p_reg_ep4_tx_len_7_0] = (Byte) (frameSize >> 8);
        error = Standard_writeRegisters (demodulator, 0, Processor_LINK, p_reg_ep4_tx_len_7_0, 2, buffer);

        /** Set EP4 packet size */
        error = Standard_writeRegister (demodulator, 0, Processor_LINK, p_reg_ep4_max_pkt, packetSize);
        if (error) goto exit;

        if (pipValid == True) {
            /** Enable EP5 */
            error = Standard_writeRegisterBits (demodulator, 0, Processor_LINK, p_reg_ep5_tx_en, reg_ep5_tx_en_pos, reg_ep5_tx_en_len, 1);
            if (error) goto exit;

            /** Set EP5 transfer length */
            buffer[p_reg_ep5_tx_len_7_0 - p_reg_ep5_tx_len_7_0] = (Byte) frameSize;
            buffer[p_reg_ep5_tx_len_15_8 - p_reg_ep5_tx_len_7_0] = (Byte) (frameSize >> 8);
            error = Standard_writeRegisters (demodulator, 0, Processor_LINK, p_reg_ep5_tx_len_7_0, 2, buffer);

            /** Set EP5 packet size */
            error = Standard_writeRegister (demodulator, 0, Processor_LINK, p_reg_ep5_max_pkt, packetSize);
            if (error) goto exit;
        }


        /** Disable 15 SER/PAR mode */
        error = Standard_writeRegisterBits (demodulator, 0, Processor_OFDM, p_mp2if_mpeg_ser_mode, mp2if_mpeg_ser_mode_pos, mp2if_mpeg_ser_mode_len, 0);
        if (error) goto exit;
        error = Standard_writeRegisterBits (demodulator, 0, Processor_OFDM, p_mp2if_mpeg_par_mode, mp2if_mpeg_par_mode_pos, mp2if_mpeg_par_mode_len, 0);
        if (error) goto exit;

        if (pipValid == True) {
            /** Enable mp2if2 */
            error = Standard_writeRegisterBits (demodulator, 0, Processor_OFDM, p_reg_mp2if2_en, reg_mp2if2_en_pos, reg_mp2if2_en_len, 1);
            if (error) goto exit;

            for (i = 1; i < ganymede->chipNumber; i++) {
                /** Enable serial mode */
                error = Standard_writeRegisterBits (demodulator, i, Processor_OFDM, p_mp2if_mpeg_ser_mode, mp2if_mpeg_ser_mode_pos, mp2if_mpeg_ser_mode_len, 1);
                if (error) goto exit;

                /** Enable HostB serial */
                error = Standard_writeRegisterBits (demodulator, i, Processor_LINK, p_reg_top_hostb_mpeg_ser_mode, reg_top_hostb_mpeg_ser_mode_pos, reg_top_hostb_mpeg_ser_mode_len, 1);
                if (error) goto exit;
            }

            /** Enable tsis */
            error = Standard_writeRegisterBits (demodulator, 0, Processor_OFDM, p_reg_tsis_en, reg_tsis_en_pos, reg_tsis_en_len, 1);
            if (error) goto exit;
        } else {
            /** Disable mp2if2 */
            error = Standard_writeRegisterBits (demodulator, 0, Processor_OFDM, p_reg_mp2if2_en, reg_mp2if2_en_pos, reg_mp2if2_en_len, 0);
            if (error) goto exit;

            for (i = 1; i < ganymede->chipNumber; i++) {
                /** Disable serial mode */
                error = Standard_writeRegisterBits (demodulator, i, Processor_OFDM, p_mp2if_mpeg_ser_mode, mp2if_mpeg_ser_mode_pos, mp2if_mpeg_ser_mode_len, 0);
                if (error) goto exit;

                /** Disable HostB serial */
                error = Standard_writeRegisterBits (demodulator, i, Processor_LINK, p_reg_top_hostb_mpeg_ser_mode, reg_top_hostb_mpeg_ser_mode_pos, reg_top_hostb_mpeg_ser_mode_len, 0);
                if (error) goto exit;
            }

            /** Disable tsis */
            error = Standard_writeRegisterBits (demodulator, 0, Processor_OFDM, p_reg_tsis_en, reg_tsis_en_pos, reg_tsis_en_len, 0);
            if (error) goto exit;
        }

        /** Negate EP4 reset */
        error = Standard_writeRegisterBits (demodulator, 0, Processor_OFDM, p_reg_mp2_sw_rst, reg_mp2_sw_rst_pos, reg_mp2_sw_rst_len, 0);
        if (error) goto exit;

        /** Negate EP5 reset */
        error = Standard_writeRegisterBits (demodulator, 0, Processor_OFDM, p_reg_mp2if2_sw_rst, reg_mp2if2_sw_rst_pos, reg_mp2if2_sw_rst_len, 0);
        if (error) goto exit;

        if (pipValid == True) {
            /** Split 15 PSB to 1K + 1K and enable flow control */
            error = Standard_writeRegisterBits (demodulator, 0, Processor_OFDM, p_reg_mp2if2_half_psb, reg_mp2if2_half_psb_pos, reg_mp2if2_half_psb_len, 0);
            if (error) goto exit;
            error = Standard_writeRegisterBits (demodulator, 0, Processor_OFDM, p_reg_mp2if_stop_en, reg_mp2if_stop_en_pos, reg_mp2if_stop_en_len, 1);
            if (error) goto exit;

            for (i = 1; i < ganymede->chipNumber; i++) {
                error = Standard_writeRegisterBits (demodulator, i, Processor_OFDM, p_reg_mpeg_full_speed, reg_mpeg_full_speed_pos, reg_mpeg_full_speed_len, 0);
                if (error) goto exit;
                error = Standard_writeRegisterBits (demodulator, i, Processor_OFDM, p_reg_mp2if_stop_en, reg_mp2if_stop_en_pos, reg_mp2if_stop_en_len, 0);
                if (error) goto exit;
            }
        }
    }

    ganymede->architecture = architecture;

exit:
#endif

    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_setStatisticRange (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Byte            superFrameCount,
    IN  Word            packetUnit
) {
    Dword error = Error_NO_ERROR;

#if User_USE_DRIVER
    DWORD number;
    BOOL result;
    SetStatisticRangeRequest request;
    Ganymede* ganymede;

    ganymede = (Ganymede*) demodulator;

    if (ganymede->driver != NULL) {
        request.chip = chip;
        request.superFrameCount = superFrameCount;
        request.packetUnit = packetUnit;
        result = DeviceIoControl (
                    ganymede->driver,
                    IOCTL_AFA_DEMOD_SETSTATISTICRANGE,
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
    Byte temp0;
    Byte temp1;

    deb_info("Enter %s -\n", __FUNCTION__);
    /** Set super frame count */
    error = Standard_writeRegister (demodulator, chip, Processor_OFDM, qnt_vbc_sframe_num, superFrameCount);
    if (error) goto exit;

    /** Set packet unit. */
    temp0 = (Byte) packetUnit;
    temp1 = (Byte) (packetUnit >> 8);
    error = Standard_writeRegisters (demodulator, chip, Processor_OFDM, rsd_packet_unit_7_0, 1, &temp0);
    if (error) goto exit;
    error = Standard_writeRegisters (demodulator, chip, Processor_OFDM, rsd_packet_unit_15_8, 1, &temp1);
    if (error) goto exit;

exit:
#endif

    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_getStatisticRange (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Byte*           frameCount,
    IN  Word*           packetUnit
) {
    Dword error = Error_NO_ERROR;

#if User_USE_DRIVER
    DWORD number;
    BOOL result;
    GetStatisticRangeRequest request;
    Ganymede* ganymede;

    ganymede = (Ganymede*) demodulator;

    if (ganymede->driver != NULL) {
        request.chip = chip;
        request.superFrameCount = superFrameCount;
        request.packetUnit = packetUnit;
        result = DeviceIoControl (
                    ganymede->driver,
                    IOCTL_AFA_DEMOD_GETSTATISTICRANGE,
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
    Byte temp0;
    Byte temp1;

    deb_info("Enter %s -\n", __FUNCTION__);
    /** Get super frame count */
    error = Standard_readRegister (demodulator, chip, Processor_OFDM, qnt_vbc_sframe_num, frameCount);
    if (error) goto exit;

    /** Get packet unit. */
    error = Standard_readRegisters (demodulator, chip, Processor_OFDM, r_rsd_packet_unit_7_0, 1, &temp0);
    if (error) goto exit;
    error = Standard_readRegisters (demodulator, chip, Processor_OFDM, r_rsd_packet_unit_15_8, 1, &temp1);
    if (error) goto exit;
    *packetUnit = (Word) (temp1 << 8) + (Word) temp0;

exit:
#endif

    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_getStatistic (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    OUT Statistic*      statistic
) {
    Dword error = Error_NO_ERROR;

#if User_USE_DRIVER
    DWORD number;
    BOOL result;
    GetStatisticRequest request;
    Ganymede* ganymede;

    ganymede = (Ganymede*) demodulator;

    if (ganymede->driver != NULL) {
        request.chip = chip;
        request.statistic = statistic;
        result = DeviceIoControl (
                    ganymede->driver,
                    IOCTL_AFA_DEMOD_GETSTATISTIC,
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
    Byte quality;
    Byte strength;
    Byte buffer[2];

    deb_info("Enter %s -\n", __FUNCTION__);
    ganymede = (Ganymede*) demodulator;

    /** Get statistic by stream type */
    error = Standard_readRegisters (demodulator, chip, Processor_OFDM, tpsd_lock, mpeg_lock - tpsd_lock + 1, buffer);
    if (error) goto exit;

    if (buffer[tpsd_lock - tpsd_lock])
        ganymede->statistic[chip].signalPresented = True;
    else
        ganymede->statistic[chip].signalPresented = False;

    if (buffer[mpeg_lock - tpsd_lock])
        ganymede->statistic[chip].signalLocked = True;
    else
        ganymede->statistic[chip].signalLocked = False;

    error = Standard_getSignalQuality (demodulator, chip, &quality);
    if (error) goto exit;

    ganymede->statistic[chip].signalQuality = quality;

    error = Standard_getSignalStrength (demodulator, chip, &strength);
    if (error) goto exit;

    ganymede->statistic[chip].signalStrength = strength;

    *statistic = ganymede->statistic[chip];

exit :
#endif

    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_getInterrupts (
    IN  Demodulator*    demodulator,
    OUT Interrupts*     interrupts
) {
    Dword error = Error_NO_ERROR;
    Ganymede* ganymede;
    Byte value = 0;

    deb_info("Enter %s -\n", __FUNCTION__);
    ganymede = (Ganymede*) demodulator;

    /** Get interrupts by stream type */


    *interrupts = Interrupt_NONE;

    /** Read the interrupts register to determine the type of interrupts. */
    error = Standard_readRegister (demodulator, 0, Processor_LINK, r_link_ofsm_dvbt_int, &value);
    if (error) goto exit;

    if (value & 0x04) {
        ganymede->dataReady = True;
        *interrupts |= Interrupt_DVBT;
    }

exit:
    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_clearInterrupt (
    IN  Demodulator*    demodulator,
    IN  Interrupt       interrupt
) {
    Dword error = Error_NO_ERROR;
    Ganymede* ganymede;
    Byte value = 0;

    deb_info("Enter %s -\n", __FUNCTION__);
    ganymede = (Ganymede*) demodulator;

    /** Clear interrupt by stream type */

    //error = ganymede->dvbtStandardDescription.clearInterrupt (demodulator, interrupt);
    value = (Byte) interrupt;

    /** Clear the specific interrupt. */
    error = Standard_writeRegister (demodulator, 0, Processor_LINK, p_reg_dvbt_intsts, value);

    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_getDataLength (
    IN  Demodulator*    demodulator,
    OUT Dword*          dataLength,
    OUT Bool*           valid           /** used in DVBH mode */
) {
    Dword error = Error_NO_ERROR;
    Ganymede* ganymede;

    deb_info("Enter %s -\n", __FUNCTION__);
    ganymede = (Ganymede*) demodulator;

    /** Get data length by stream type */
    switch (ganymede->burstSize) {
        case BurstSize_1024 :
            *dataLength = 1024;
            break;
        case BurstSize_2048 :
            *dataLength = 2048;
            break;
        case BurstSize_4096 :
            *dataLength = 4096;
            break;
    }
    *valid = True;

    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_getData (
    IN  Demodulator*    demodulator,
    IN  Dword           bufferLength,
    OUT Byte*           buffer
) {
    Dword error = Error_NO_ERROR;
    Ganymede* ganymede;

    deb_info("Enter %s -\n", __FUNCTION__);
    ganymede = (Ganymede*) demodulator;

    /** Get data by stream type */

    //error = ganymede->dvbtStandardDescription.getData (demodulator, bufferLength, buffer);

    if (bufferLength == 0) {
        error = Error_INVALID_DATA_LENGTH;
        goto exit;
    }


    /** IP datagram is locate in a special register 0xF00000 */
    error = ganymede->cmdDescription->receiveData (demodulator, 0xF00000, bufferLength, buffer);
    if (error) goto exit;

    ganymede->dataReady = False;

exit:
    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


/** combine Standard_getLength and Standard_getData */
Dword Standard_getDatagram (
    IN  Demodulator*    demodulator,
    OUT Dword*          bufferLength,
    OUT Byte*           buffer
) {
    Dword error = Error_NO_ERROR;

#if User_USE_DRIVER
    DWORD number;
    BOOL result;
    GetDatagramRequest request;
    Ganymede* ganymede;

    ganymede = (Ganymede*) demodulator;

    if (ganymede->driver != NULL) {
        request.bufferLength = bufferLength;
        request.buffer = buffer;
        result = DeviceIoControl (
                    ganymede->driver,
                    IOCTL_AFA_DEMOD_GETDATAGRAM,
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
    Dword length = 0;
    Byte value;
    // Bool ready = False;

    deb_info("Enter %s -\n", __FUNCTION__);
    ganymede = (Ganymede*) demodulator;

    /** Get datagram by stream type */

    //error = ganymede->dvbtStandardDescription.getDatagram (demodulator, bufferLength, buffer);

#if User_USE_INTERRUPT
#else

    error = Standard_readRegisterBits (demodulator, 0, Processor_LINK, r_link_ofsm_ip_valid, link_ofsm_ip_valid_pos, link_ofsm_ip_valid_len, &value);
    if (error) goto exit;

    if (value) {
        ganymede->dataReady = True;
    }
    if (ganymede->dataReady == False) {
        *bufferLength = 0;
        error = Error_NOT_READY;
        goto exit;
    }
#endif
    switch (ganymede->burstSize) {
        case BurstSize_1024 :
            length = 1024;
            break;
        case BurstSize_2048 :
            length = 2048;
            break;
        case BurstSize_4096 :
            length = 4096;
            break;
    }
    if (*bufferLength >= length) {
        //error = Dvbt_getData (demodulator, length, (Byte*) buffer);
        //if (error) goto exit;

        if (bufferLength == 0) {
            error = Error_INVALID_DATA_LENGTH;
            goto exit;
        }


        /** IP datagram is locate in a special register 0xF00000 */
        error = ganymede->cmdDescription->receiveData (demodulator, 0xF00000, length, buffer);
        if (error) goto exit;

        *bufferLength = length;

        ganymede->dataReady = False;

        *bufferLength = length;
    } else {
        error = Error_BUFFER_INSUFFICIENT;
    }


exit:
#endif

    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}

/** get ir raw code (4 bytes) */
Dword Standard_getIrCode (
    IN  Demodulator*    demodulator,
    OUT Dword*          code
)  {
    Dword error = Error_NO_ERROR;
    Byte readBuffer[4];
    Ganymede* ganymede;

    deb_info("Enter %s -\n", __FUNCTION__);
    ganymede = (Ganymede*) demodulator;

    error = Standard_sendCommand (demodulator, Command_IR_GET, 0, Processor_LINK, 0, NULL, 4, readBuffer);
    if (error) goto exit;

    *code = (Dword) ((readBuffer[0] << 24) + (readBuffer[1] << 16) + (readBuffer[2] << 8) + readBuffer[3]);

exit :
    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_reboot (
    IN  Demodulator*    demodulator
)  {
    Dword error = Error_NO_ERROR;
    Dword version;
    Byte i;
    Ganymede* ganymede;

    deb_info("Enter %s -\n", __FUNCTION__);
    ganymede = (Ganymede*) demodulator;

    error = Standard_getFirmwareVersion (demodulator, Processor_LINK, &version);
    if (error) goto exit;
    if (version == 0xFFFFFFFF) goto exit;       /** I2M and I2U */
    if (version != 0) {
        for (i = ganymede->chipNumber; i > 0; i--) {
            error = ganymede->cmdDescription->reboot (demodulator, i - 1);
            if (error) goto exit;
            User_delay (demodulator, 1);
        }

        User_delay (demodulator, 10);

        version = 1;
        for (i = 0; i < 30; i++) {
            error = Standard_getFirmwareVersion (demodulator, Processor_LINK, &version);
            if (error == Error_NO_ERROR) break;
            User_delay (demodulator, 10);
        }
        if (error) goto exit;
        if (version != 0)
            error = Error_REBOOT_FAIL;
    }
    for (i = ganymede->chipNumber; i > 0; i--) {
        error = Standard_writeRegisterBits (demodulator, i - 1, Processor_LINK, p_reg_p_dmb_sw_reset, reg_p_dmb_sw_reset_pos, reg_p_dmb_sw_reset_len, 1);
        if (error) goto exit;
    }

    ganymede->booted = False;

exit :
    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_controlPowerSaving (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Byte            control
) {
    Dword error = Error_NO_ERROR;

#if User_USE_DRIVER
    DWORD number;
    BOOL result;
    ControlPowerSavingRequest request;
    Ganymede* ganymede;

    ganymede = (Ganymede*) demodulator;

    if (ganymede->driver != NULL) {
        request.chip = chip;
        request.control = control;
        result = DeviceIoControl (
                    ganymede->driver,
                    IOCTL_AFA_DEMOD_CONTROLPOWERSAVING,
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
    Byte temp;
    Byte begin;
    Byte end;
    Byte i;
    Byte j;
    Ganymede* ganymede;

    deb_info("Enter %s control=%d-\n", __FUNCTION__, control);
    ganymede = (Ganymede*) demodulator;

    if (ganymede->architecture == Architecture_DCA) {
        begin = 0;
        end = ganymede->chipNumber;
    } else {
        begin = chip;
        end = begin + 1;
    }

    if (control) {
        /** Power up case */
        if ((ganymede->busId == Bus_USB) || (ganymede->busId == Bus_USB11)) {
            for (i = begin; i < end; i++) {
                error = Standard_writeRegisterBits (demodulator, i, Processor_OFDM, p_reg_afe_mem0, 3, 1, 0);
                if (error) goto exit;
                error = Standard_writeRegister (demodulator, i, Processor_OFDM, suspend_flag, 0);
                if (error) goto exit;
				error = Standard_writeRegister (demodulator, i, Processor_OFDM, trigger_ofsm, 0);
				if (error) goto exit;

            }
        } else {  /** TS, SPI, and SDIO case */
            /** not implemented yet */
        }

        /** Fixed current leakage */
        switch (ganymede->busId) {
            case Bus_SPI :
            case Bus_SDIO :
            case Bus_USB :
            case Bus_USB11 :
                if (ganymede->chipNumber > 1) {
                    for (i = 1; i < ganymede->chipNumber; i++) {
                        /** Disable HostA parallel */
                        error = Standard_writeRegisterBits (demodulator, i, Processor_LINK, p_reg_top_hosta_mpeg_ser_mode, reg_top_hosta_mpeg_ser_mode_pos, reg_top_hosta_mpeg_ser_mode_len, 0);
                        if (error) goto exit;
                        error = Standard_writeRegisterBits (demodulator, i, Processor_LINK, p_reg_top_hosta_mpeg_par_mode, reg_top_hosta_mpeg_par_mode_pos, reg_top_hosta_mpeg_par_mode_len, 0);
                        if (error) goto exit;
                    }
                }
                break;
        }
    } else {
        /** Power down case */
        if ((ganymede->busId == Bus_USB) || (ganymede->busId == Bus_USB11)) {
            for (i = begin; i < end; i++) {
                error = Standard_writeRegister (demodulator, i, Processor_OFDM, suspend_flag, 1);
                if (error) goto exit;
                error = Standard_writeRegister (demodulator, i, Processor_OFDM, trigger_ofsm, 0);
                if (error) goto exit;

                for (j = 0; j < 150; j++) {
                    error = Standard_readRegister (demodulator, i, Processor_OFDM, suspend_flag, &temp);
                    if (error) goto exit;
                    if (!temp) break;
                    User_delay (demodulator, 10);
                }
                error = Standard_writeRegisterBits (demodulator, i, Processor_OFDM, p_reg_afe_mem0, 3, 1, 1);
                if (error) goto exit;
            }
        } else {  /** TS SPI SDIO */
            /** not implemented yet */
        }

        /** Fixed current leakage */
        switch (ganymede->busId) {
            case Bus_SPI :
            case Bus_SDIO :
            case Bus_USB :
            case Bus_USB11 :
                if (ganymede->chipNumber > 1) {
                    for (i = 1; i < ganymede->chipNumber; i++) {
                        /** Enable HostA parallel */
                        error = Standard_writeRegisterBits (demodulator, i, Processor_LINK, p_reg_top_hosta_mpeg_ser_mode, reg_top_hosta_mpeg_ser_mode_pos, reg_top_hosta_mpeg_ser_mode_len, 0);
                        if (error) goto exit;
                        error = Standard_writeRegisterBits (demodulator, i, Processor_LINK, p_reg_top_hosta_mpeg_par_mode, reg_top_hosta_mpeg_par_mode_pos, reg_top_hosta_mpeg_par_mode_len, 1);
                        if (error) goto exit;
                    }
                }
                break;
        }
    }

exit :
#endif

    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_controlTunerPowerSaving (
    IN  Demodulator*    demodulator,
	IN  Byte            control
) {
	Dword error = Error_NO_ERROR;

    deb_info("Enter %s control=%d-\n", __FUNCTION__, control);
	if (control) {
		error = Standard_writeRegister (demodulator, 0, Processor_LINK, p_reg_top_gpioh7_en, 1);
		if (error) goto exit;

		error = Standard_writeRegister (demodulator, 0, Processor_LINK, p_reg_top_gpioh7_on, 1);
		if (error) goto exit;

		error = Standard_writeRegister (demodulator, 0, Processor_LINK, p_reg_top_gpioh7_o, 1);
		if (error) goto exit;
	} else {
		error = Standard_writeRegister (demodulator, 0, Processor_LINK, p_reg_top_gpioh7_en, 1);
		if (error) goto exit;

		error = Standard_writeRegister (demodulator, 0, Processor_LINK, p_reg_top_gpioh7_on, 1);
		if (error) goto exit;

		error = Standard_writeRegister (demodulator, 0, Processor_LINK, p_reg_top_gpioh7_o, 0);
		if (error) goto exit;
	}

exit:
    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
	return (error);
}


Dword Standard_runCode (
    IN  Demodulator*    demodulator,
    IN  Word            code
) {
    Dword error = Error_NO_ERROR;
    Byte writeBuffer[2];
    Ganymede* ganymede;

    ganymede = (Ganymede*) demodulator;

    writeBuffer[0] = (Byte) (code >> 8);
    writeBuffer[1] = (Byte) code;
    error = Standard_sendCommand (demodulator, Command_RUN_CODE, 0, Processor_LINK, 2, writeBuffer, 0, NULL);
    if (error) goto exit;

exit :
    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_controlPidFilter (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Byte            control
) {
    Dword error = Error_NO_ERROR;

#if User_USE_DRIVER
    DWORD number;
    BOOL result;
    ControlPidFilterRequest request;
    Ganymede* ganymede;

    ganymede = (Ganymede*) demodulator;

    if (ganymede->driver != NULL) {
        request.chip = chip;
        request.control = control;
        result = DeviceIoControl (
                    ganymede->driver,
                    IOCTL_AFA_DEMOD_CONTROLPIDFILTER,
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
    deb_info("Enter %s -\n", __FUNCTION__);
    error = Standard_writeRegisterBits (demodulator, chip, Processor_OFDM, p_mp2if_pid_en, mp2if_pid_en_pos, mp2if_pid_en_len, control);
#endif

    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_resetPidFilter (
    IN  Demodulator*    demodulator,
    IN  Byte            chip
) {
    Dword error = Error_NO_ERROR;

#if User_USE_DRIVER
#else
    Ganymede* ganymede;
    deb_info("Enter %s -\n", __FUNCTION__);

    ganymede = (Ganymede*) demodulator;

    error = Standard_writeRegisterBits (demodulator, chip, Processor_OFDM, p_mp2if_pid_rst, mp2if_pid_rst_pos, mp2if_pid_rst_len, 1);
    if (error) goto exit;

exit :
#endif

    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}

Dword Standard_addPidToFilter (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Byte            index,
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
    Ganymede* ganymede;

    deb_info("Enter %s -\n", __FUNCTION__);
    ganymede = (Ganymede*) demodulator;

    /** Enable pid filter */
    error = Standard_writeRegisterBits (demodulator, chip, Processor_OFDM, p_mp2if_pid_en, mp2if_pid_en_pos, mp2if_pid_en_len, 1);
    if (error) goto exit;

    writeBuffer[0] = (Byte) pid.value;
    writeBuffer[1] = (Byte) (pid.value >> 8);

    error = Standard_writeRegisters (demodulator, chip, Processor_OFDM, p_mp2if_pid_dat_l, 2, writeBuffer);
    if (error) goto exit;

    error = Standard_writeRegisterBits (demodulator, chip, Processor_OFDM, p_mp2if_pid_index_en, mp2if_pid_index_en_pos, mp2if_pid_index_en_len, 1);
    if (error) goto exit;

    error = Standard_writeRegister (demodulator, chip, Processor_OFDM, p_mp2if_pid_index, index);
    if (error) goto exit;

exit :
#endif

    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_setBurstSize (
    IN  Demodulator*    demodulator,
    IN  BurstSize       burstSize
) {
    Dword error = Error_NO_ERROR;
    Byte i;
    Ganymede* ganymede;

    deb_info("Enter %s -\n", __FUNCTION__);
    ganymede = (Ganymede*) demodulator;


    if (burstSize == BurstSize_4096) {
        error = Error_NOT_SUPPORT;
        goto exit;
    }


    switch (burstSize) {
        case BurstSize_1024 :
            for (i = 0; i < ganymede->chipNumber; i++) {
                error = Standard_writeRegisterBits (demodulator, i, Processor_LINK, p_reg_dvbt_path, reg_dvbt_path_pos, reg_dvbt_path_len, 1);
                if (error) goto exit;
                error = Standard_writeRegisterBits (demodulator, i, Processor_LINK, p_reg_dvbt_bufsize, reg_dvbt_bufsize_pos, reg_dvbt_bufsize_len, 1);
                if (error) goto exit;
            }
            ganymede->burstSize = BurstSize_1024;
            break;
        case BurstSize_2048 :
            for (i = 0; i < ganymede->chipNumber; i++) {
                error = Standard_writeRegisterBits (demodulator, i, Processor_LINK, p_reg_dvbt_path, reg_dvbt_path_pos, reg_dvbt_path_len, 1);
                if (error) goto exit;
                error = Standard_writeRegisterBits (demodulator, i, Processor_LINK, p_reg_dvbt_bufsize, reg_dvbt_bufsize_pos, reg_dvbt_bufsize_len, 0);
                if (error) goto exit;
            }
            ganymede->burstSize = BurstSize_2048;

            break;
        case BurstSize_4096 :
            for (i = 0; i < ganymede->chipNumber; i++) {
                error = Standard_writeRegisterBits (demodulator, i, Processor_LINK, p_reg_dvbt_path, reg_dvbt_path_pos, reg_dvbt_path_len, 0);
                if (error) goto exit;
                error = Standard_writeRegisterBits (demodulator, i, Processor_LINK, p_reg_dvbt_bufsize, reg_dvbt_bufsize_pos, reg_dvbt_bufsize_len, 1);
                if (error) goto exit;
            }
            ganymede->burstSize = BurstSize_4096;
            break;
    }

exit :
    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}


Dword Standard_getBurstSize (
    IN  Demodulator*    demodulator,
    IN  BurstSize*      burstSize
) {
    Dword error = Error_NO_ERROR;
    Byte path;
    Byte size;

    deb_info("Enter %s -\n", __FUNCTION__);
    error = Standard_readRegisterBits (demodulator, 0, Processor_LINK, p_reg_dvbt_path, reg_dvbt_path_pos, reg_dvbt_path_len, &path);
    if (error) goto exit;
    error = Standard_readRegisterBits (demodulator, 0, Processor_LINK, p_reg_dvbt_bufsize, reg_dvbt_bufsize_pos, reg_dvbt_bufsize_len, &size);
    if (error) goto exit;

    if (path) {
        if (size) {
            *burstSize = BurstSize_1024;
        } else {
            *burstSize = BurstSize_2048;
        }
    } else {
        if (size) {
            *burstSize = BurstSize_4096;
        } else {
            *burstSize = BurstSize_2048;
        }
    }

exit :
    if(error) {
	deb_info("%s error, ret=0x%x\n", __FUNCTION__, error);
    }
    return (error);
}
