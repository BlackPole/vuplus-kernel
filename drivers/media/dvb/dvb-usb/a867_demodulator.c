#include "a867_demodulator.h"


Dword Demodulator_writeRegister (
      Demodulator*    demodulator,
      Byte            chip,
      Processor       processor,
      Dword           registerAddress,
      Byte            value
) {
    return (Standard_writeRegister (demodulator, chip, processor, registerAddress, value));
}


Dword Demodulator_writeRegisters (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Processor       processor,
    IN  Dword           registerAddress,
    IN  Byte            bufferLength,
    IN  Byte*           buffer
) {
    return (Standard_writeRegisters (demodulator, chip, processor, registerAddress, bufferLength, buffer));
}


Dword Demodulator_writeScatterRegisters (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Processor       processor,
    IN  Byte            valueSetsLength,
    IN  ValueSet*       valueSets
) {
    return (Standard_writeScatterRegisters (demodulator, chip, processor, valueSetsLength, valueSets));
}


Dword Demodulator_writeTunerRegisters (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Word            registerAddress,
    IN  Byte            bufferLength,
    IN  Byte*           buffer
) {
    return (Standard_writeTunerRegisters (demodulator, chip, registerAddress, bufferLength, buffer));
}


Dword Demodulator_writeGenericRegisters (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Byte            interfaceIndex,
    IN  Byte            slaveAddress,
    IN  Byte            bufferLength,
    IN  Byte*           buffer
) {
    return (Standard_writeGenericRegisters (demodulator, chip, interfaceIndex, slaveAddress, bufferLength, buffer));
}


Dword Demodulator_writeEepromValues (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Word            registerAddress,
    IN  Byte            bufferLength,
    IN  Byte*           buffer
) {
    return (Standard_writeEepromValues (demodulator, chip, registerAddress, bufferLength, buffer));
}


Dword Demodulator_writeRegisterBits (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Processor       processor,
    IN  Dword           registerAddress,
    IN  Byte            position,
    IN  Byte            length,
    IN  Byte            value
)
{
    return (Standard_writeRegisterBits (demodulator, chip, processor, registerAddress, position, length, value));
}


Dword Demodulator_readRegister (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Processor       processor,
    IN  Dword           registerAddress,
    OUT Byte*           value
) {
    return (Standard_readRegister (demodulator, chip, processor, registerAddress, value));
}


Dword Demodulator_readRegisters (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Processor       processor,
    IN  Dword           registerAddress,
    IN  Byte            bufferLength,
    OUT Byte*           buffer
) {
    return (Standard_readRegisters (demodulator, chip, processor, registerAddress, bufferLength, buffer));
}


Dword Demodulator_readScatterRegisters (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Processor       processor,
    IN  Byte            valueSetsLength,
    OUT ValueSet*       valueSets
) {
    return (Standard_readScatterRegisters (demodulator, chip, processor, valueSetsLength, valueSets));
}


Dword Demodulator_readTunerRegisters (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Word            registerAddress,
    IN  Byte            bufferLength,
    IN  Byte*           buffer
) {
    return (Standard_readTunerRegisters (demodulator, chip, registerAddress, bufferLength, buffer));
}


Dword Demodulator_readGenericRegisters (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Byte            interfaceIndex,
    IN  Byte            slaveAddress,
    IN  Byte            bufferLength,
    IN  Byte*           buffer
) {
    return (Standard_readGenericRegisters (demodulator, chip, interfaceIndex, slaveAddress, bufferLength, buffer));
}


Dword Demodulator_readEepromValues (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Word            registerAddress,
    IN  Byte            bufferLength,
    OUT Byte*           buffer
) {
    return (Standard_readEepromValues (demodulator, chip, registerAddress, bufferLength, buffer));
}


Dword Demodulator_readRegisterBits (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Processor       processor,
    IN  Dword           registerAddress,
    IN  Byte            position,
    IN  Byte            length,
    OUT Byte*           value
) {
    return (Standard_readRegisterBits (demodulator, chip, processor, registerAddress, position, length, value));
}


Dword Demodulator_getHardwareVersion (
    IN  Demodulator*    demodulator,
    OUT Dword*          version
) {
    return (Standard_getHardwareVersion (demodulator, version));
}


Dword Demodulator_getFirmwareVersion (
    IN  Demodulator*    demodulator,
    IN  Processor       processor,
    OUT Dword*          version
) {
    return (Standard_getFirmwareVersion (demodulator, processor, version));
}


Dword Demodulator_getRfAgcGain (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    OUT Byte*           rfAgc
) {
    return (Standard_getRfAgcGain (demodulator, chip, rfAgc));
}


Dword Demodulator_getIfAgcGain (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    OUT Byte*           ifAgc
) {
    return (Standard_getIfAgcGain (demodulator, chip, ifAgc));
}


Dword Demodulator_controlPidFilter (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Byte            control
) {
    return (Standard_controlPidFilter (demodulator, chip, control));
}


Dword Demodulator_addPidToFilter (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Byte            index,
    IN  Pid             pid
) {
    return (Standard_addPidToFilter (demodulator, chip, index, pid));
}


Dword Demodulator_resetPidFilter (
    IN  Demodulator*    demodulator,
    IN  Byte            chip
) {
    return (Standard_resetPidFilter (demodulator, chip));
}


Dword Demodulator_loadIrTable (
    IN  Demodulator*    demodulator,
    IN  Word            tableLength,
    IN  Byte*           table
) {
    return (Standard_loadIrTable (demodulator, tableLength, table));
}


Dword Demodulator_loadFirmware (
    IN  Demodulator*    demodulator,
    IN  Byte*           firmwareCodes,
    IN  Segment*        firmwareSegments,
    IN  Byte*           firmwarePartitions
) {

    return (Standard_loadFirmware (demodulator, firmwareCodes, firmwareSegments, firmwarePartitions));
}


Dword Demodulator_initialize (
    IN  Demodulator*    demodulator,
    IN  Byte            chipNumber,
    IN  Word            sawBandwidth,
    IN  StreamType      streamType,
    IN  Architecture    architecture
) {
    return (Standard_initialize (demodulator, chipNumber, sawBandwidth, streamType, architecture));
}


Dword Demodulator_finalize (
    IN  Demodulator*    demodulator
) {
    return (Standard_finalize (demodulator));
}


Dword Demodulator_isAgcLocked (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    OUT Bool*           locked
) {
    return (Standard_isAgcLocked (demodulator, chip, locked));
}


Dword Demodulator_isCfoeLocked (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    OUT Bool*           locked
) {
    return (Standard_isCfoeLocked (demodulator, chip, locked));
}


Dword Demodulator_isSfoeLocked (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    OUT Bool*           locked
) {
    return (Standard_isSfoeLocked (demodulator, chip, locked));
}


Dword Demodulator_isTpsLocked (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    OUT Bool*           locked
) {
    return (Standard_isTpsLocked (demodulator, chip, locked));
}


Dword Demodulator_isMpeg2Locked (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    OUT Bool*           locked
) {
    return (Standard_isMpeg2Locked (demodulator, chip, locked));
}


Dword Demodulator_isLocked (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    OUT Bool*           locked
)
{
    return (Standard_isLocked (demodulator, chip, locked));
}


Dword Demodulator_setPriority (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Priority        priority
)
{
    return (Standard_setPriority (demodulator, chip, priority));
}


Dword Demodulator_reset (
    IN  Demodulator*    demodulator
) {
    return (Standard_reset (demodulator));
}


Dword Demodulator_getChannelModulation (
    IN  Demodulator*            demodulator,
    IN  Byte                    chip,
    OUT ChannelModulation*      channelModulation
) {
    return (Standard_getChannelModulation (demodulator, chip, channelModulation));
}


Dword Demodulator_setChannelModulation (
    IN  Demodulator*            demodulator,
    IN  Byte                    chip,
    IN  ChannelModulation*      channelModulation
) {
    return (Standard_setChannelModulation (demodulator, chip, channelModulation));
}


Dword Demodulator_acquireChannel (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Word            bandwidth,
    IN  Dword           frequency
) {
    return (Standard_acquireChannel (demodulator, chip, bandwidth, frequency));
}


Dword Demodulator_setStreamType (
    IN  Demodulator*    demodulator,
    IN  StreamType      streamType
) {
    return (Standard_setStreamType (demodulator, streamType));
}


Dword Demodulator_setArchitecture (
    IN  Demodulator*    demodulator,
    IN  Architecture    architecture
) {
    return (Standard_setArchitecture (demodulator, architecture));
}


Dword Demodulator_getSignalQuality (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    OUT Byte*           quality
) {
    return (Standard_getSignalQuality (demodulator, chip, quality));
}


Dword Demodulator_getSignalStrength (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    OUT Byte*           strength
) {
    return (Standard_getSignalStrength (demodulator, chip, strength));
}


Dword Demodulator_getSignalStrengthDbm (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Long            rfpullUpVolt_X10,     /** RF pull up voltage multiplied by 10 */
    IN  Long            ifpullUpVolt_X10,     /** IF pull up voltage multiplied by 10 */
    OUT Long*           strengthDbm           /** DBm                                */
) {
    return (Standard_getSignalStrengthDbm (demodulator, chip, rfpullUpVolt_X10, ifpullUpVolt_X10, strengthDbm));
}

Dword Demodulator_getPostVitBer (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    OUT Dword*          postErrorCount,  /** 24 bits */
    OUT Dword*          postBitCount,    /** 16 bits */
    OUT Word*           abortCount
){
	return (Standard_getPostVitBer(demodulator, chip, postErrorCount, postBitCount, abortCount));
}


Dword Demodulator_setStatisticRange (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Byte            superFrameCount,
    IN  Word            packetUnit
) {
    return (Standard_setStatisticRange (demodulator, chip, superFrameCount, packetUnit));
}


Dword Demodulator_getStatisticRange (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Byte*           superFrameCount,
    IN  Word*           packetUnit
) {
    return (Standard_getStatisticRange (demodulator, chip, superFrameCount, packetUnit));
}


Dword Demodulator_getStatistic (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    OUT Statistic*      statistic
) {
    return (Standard_getStatistic (demodulator, chip, statistic));
}


Dword Demodulator_getInterrupts (
    IN  Demodulator*    demodulator,
    OUT Interrupts*     interrupts
) {
    return (Standard_getInterrupts (demodulator, interrupts));
}


Dword Demodulator_clearInterrupt (
    IN  Demodulator*    demodulator,
    IN  Interrupt       interrupt
) {
    return (Standard_clearInterrupt (demodulator, interrupt));
}


Dword Demodulator_getDataLength (
    IN  Demodulator*    demodulator,
    OUT Dword*          dataLength,
    OUT Bool*           valid
) {
    return (Standard_getDataLength (demodulator, dataLength, valid));
}


//jamie: The function is unused.
Dword Demodulator_getData (
    IN  Demodulator*    demodulator,
    IN  Dword           bufferLength,
    OUT Byte*           buffer
) {
    return (Standard_getData (demodulator, bufferLength, buffer));
}

//jamie: The function is unused.
Dword Demodulator_getDatagram (
    IN  Demodulator*    demodulator,
    OUT Dword*          bufferLength,
    OUT Byte*           buffer
) {
    return (Standard_getDatagram (demodulator, bufferLength, buffer));
}


Dword Demodulator_getIrCode (
    IN  Demodulator*    demodulator,
    OUT Dword*          code
)  {
    return (Standard_getIrCode (demodulator, code));
}


Dword Demodulator_reboot (
    IN  Demodulator*    demodulator
)  {
    return (Standard_reboot (demodulator));
}


Dword Demodulator_controlPowerSaving (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Byte            control
) {
    return (Standard_controlPowerSaving (demodulator, chip, control));
}


Dword Demodulator_controlTunerPowerSaving (
    IN  Demodulator*    demodulator,
    IN  Byte            control
) {
    return (Standard_controlTunerPowerSaving (demodulator, control));
}


Dword Demodulator_setBurstSize (
    IN  Demodulator*    demodulator,
    IN  BurstSize       burstSize
) {
    return (Standard_setBurstSize (demodulator, burstSize));
}


Dword Demodulator_getBurstSize (
    IN  Demodulator*    demodulator,
    IN  BurstSize*      burstSize
) {
    return (Standard_getBurstSize (demodulator, burstSize));
}
