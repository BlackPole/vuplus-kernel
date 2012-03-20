#ifndef __STANDARD_H__
#define __STANDARD_H__


#include "a867_type.h"
#include "a867_user.h"
#include "a867_error.h"
#include "a867_register.h"
#include "a867_variable.h"
#include "a867_version.h"

#if User_USE_DRIVER
#include <tchar.h>
#include "iocontrol.h"
#endif


/**
 * Write one byte (8 bits) to a specific register in demodulator.
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are
 *        0~7.
 * @param processor The processor of specified register. Because each chip
 *        has two processor so user have to specify the processor. The
 *        possible values are Processor_LINK and Processor_OFDM.
 * @param registerAddress the address of the register to be written.
 * @param value the value to be written.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Standard_writeRegister (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Processor       processor,
    IN  Dword           registerAddress,
    IN  Byte            value
);


/**
 * Write a sequence of bytes to the contiguous registers in demodulator.
 * The maximum burst size is restricted by the capacity of bus. If bus
 * could transfer N bytes in one cycle, then the maximum value of
 * bufferLength would be N - 5.
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are
 *        0~7.
 * @param processor The processor of specified register. Because each chip
 *        has two processor so user have to specify the processor. The
 *        possible values are Processor_LINK and Processor_OFDM.
 * @param registerAddress the start address of the registers to be written.
 * @param bufferLength the number of registers to be written.
 * @param buffer a byte array which is used to store values to be written.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Standard_writeRegisters (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Processor       processor,
    IN  Dword           registerAddress,
    IN  Byte            bufferLength,
    IN  Byte*           buffer
);


/**
 * Write a collection of values to discontiguous registers in demodulator.
 * The maximum burst size is restricted by the capacity of bus. If bus
 * could transfer N bytes in one cycle, then the maximum value of
 * bufferLength would be N - 6 (one more byte to specify tuner address).
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are
 *        0~7.
 * @param processor The processor of specified register. Because each chip
 *        has two processor so user have to specify the processor. The
 *        possible values are Processor_LINK and Processor_OFDM.
 * @param valueSetsLength the number of values to be written.
 * @param valueSets a ValueSet array which is used to store values to be
 *        written.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Standard_writeScatterRegisters (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Processor       processor,
    IN  Byte            valueSetsLength,
    IN  ValueSet*       valueSets
);


/**
 * Write a sequence of bytes to the contiguous registers in slave device.
 * The maximum burst size is restricted by the capacity of bus. If bus
 * could transfer N bytes in one cycle, then the maximum value of
 * bufferLength would be N - 6 (one more byte to specify tuner address).
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are
 *        0~7.
 * @param registerAddress the start address of the registers to be read.
 * @param bufferLength the number of registers to be read.
 * @param buffer a byte array which is used to store values to be read.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Standard_writeTunerRegisters (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Word            registerAddress,
    IN  Byte            bufferLength,
    IN  Byte*           buffer
);


/**
 * Write a sequence of bytes to the contiguous registers in slave device
 * through specified interface (1, 2, 3).
 * The maximum burst size is restricted by the capacity of bus. If bus
 * could transfer N bytes in one cycle, then the maximum value of
 * bufferLength would be N - 6 (one more byte to specify tuner address).
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are
 *        0~7.
 * @param interfaceIndex the index of interface. The possible values are
 *        1~3.
 * @param slaveAddress the I2c address of slave device.
 * @param bufferLength the number of registers to be read.
 * @param buffer a byte array which is used to store values to be read.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Standard_writeGenericRegisters (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Byte            interfaceIndex,
    IN  Byte            slaveAddress,
    IN  Byte            bufferLength,
    IN  Byte*           buffer
);


/**
 * Write a sequence of bytes to the contiguous cells in the EEPROM.
 * The maximum burst size is restricted by the capacity of bus. If bus
 * could transfer N bytes in one cycle, then the maximum value of
 * bufferLength would be N - 5 (firmware will detect EEPROM address).
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are
 *        0~7.
 * @param registerAddress the start address of the cells to be written.
 * @param bufferLength the number of cells to be written.
 * @param buffer a byte array which is used to store values to be written.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Standard_writeEepromValues (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Word            registerAddress,
    IN  Byte            bufferLength,
    IN  Byte*           buffer
);


/**
 * Modify bits in the specific register.
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are
 *        0~7.
 * @param processor The processor of specified register. Because each chip
 *        has two processor so user have to specify the processor. The
 *        possible values are Processor_LINK and Processor_OFDM.
 * @param registerAddress the address of the register to be written.
 * @param position the start position of bits to be modified (0 means the
 *        LSB of the specifyed register).
 * @param length the length of bits.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Standard_writeRegisterBits (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Processor       processor,
    IN  Dword           registerAddress,
    IN  Byte            position,
    IN  Byte            length,
    IN  Byte            value
);


/**
 * Read one byte (8 bits) from a specific register in demodulator.
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are
 *        0~7.
 * @param processor The processor of specified register. Because each chip
 *        has two processor so user have to specify the processor. The
 *        possible values are Processor_LINK and Processor_OFDM.
 * @param registerAddress the address of the register to be read.
 * @param value the pointer used to store the value read from demodulator
 *        register.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Standard_readRegister (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Processor       processor,
    IN  Dword           registerAddress,
    OUT Byte*           value
);


/**
 * Read a sequence of bytes from the contiguous registers in demodulator.
 * The maximum burst size is restricted by the capacity of bus. If bus
 * could transfer N bytes in one cycle, then the maximum value of
 * bufferLength would be N - 5.
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are
 *        0~7.
 * @param processor The processor of specified register. Because each chip
 *        has two processor so user have to specify the processor. The
 *        possible values are Processor_LINK and Processor_OFDM.
 * @param registerAddress the address of the register to be read.
 * @param bufferLength the number of registers to be read.
 * @param buffer a byte array which is used to store values to be read.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Standard_readRegisters (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Processor       processor,
    IN  Dword           registerAddress,
    IN  Byte            bufferLength,
    OUT Byte*           buffer
);


/**
 * Read a collection of values to discontiguous registers from demodulator.
 * The maximum burst size is restricted by the capacity of bus. If bus
 * could transfer N bytes in one cycle, then the maximum value of
 * bufferLength would be N - 6 (one more byte to specify tuner address).
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are
 *        0~7.
 * @param processor The processor of specified register. Because each chip
 *        has two processor so user have to specify the processor. The
 *        possible values are Processor_LINK and Processor_OFDM.
 * @param valueSetsLength the number of values to be read.
 * @param valueSets a ValueSet array which is used to store values to be
 *        read.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Standard_readScatterRegisters (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Processor       processor,
    IN  Byte            valueSetsLength,
    OUT ValueSet*       valueSets
);


/**
 * Read a sequence of bytes from the contiguous registers in tuner.
 * The maximum burst size is restricted by the capacity of bus. If bus
 * could transfer N bytes in one cycle, then the maximum value of
 * bufferLength would be N - 6 (one more byte to specify tuner address).
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are
 *        0~7.
 * @param registerAddress the start address of the registers to be read.
 * @param bufferLength the number of registers to be read.
 * @param buffer a byte array which is used to store values to be read.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Standard_readTunerRegisters (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Word            registerAddress,
    IN  Byte            bufferLength,
    IN  Byte*           buffer
);


/**
 * Read a sequence of bytes from the contiguous registers in slave device
 * through specified interface (1, 2, 3).
 * The maximum burst size is restricted by the capacity of bus. If bus
 * could transfer N bytes in one cycle, then the maximum value of
 * bufferLength would be N - 6 (one more byte to specify tuner address).
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are
 *        0~7.
 * @param interfaceIndex the index of interface. The possible values are
 *        1~3.
 * @param slaveAddress the I2c address of slave device.
 * @param bufferLength the number of registers to be read.
 * @param buffer a byte array which is used to store values to be read.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Standard_readGenericRegisters (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Byte            interfaceIndex,
    IN  Byte            slaveAddress,
    IN  Byte            bufferLength,
    IN  Byte*           buffer
);


/**
 * Read a sequence of bytes from the contiguous cells in the EEPROM.
 * The maximum burst size is restricted by the capacity of bus. If bus
 * could transfer N bytes in one cycle, then the maximum value of
 * bufferLength would be N - 5 (firmware will detect EEPROM address).
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are
 *        0~7.
 * @param registerAddress the start address of the cells to be read.
 * @param registerAddressLength the valid bytes of registerAddress.
 * @param bufferLength the number of cells to be read.
 * @param buffer a byte array which is used to store values to be read.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Standard_readEepromValues (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Word            registerAddress,
    IN  Byte            bufferLength,
    OUT Byte*           buffer
);


/**
 * Read bits of the specified register.
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are
 *        0~7.
 * @param processor The processor of specified register. Because each chip
 *        has two processor so user have to specify the processor. The
 *        possible values are Processor_LINK and Processor_OFDM.
 * @param registerAddress the address of the register to be read.
 * @param position the start position of bits to be read (0 means the
 *        LSB of the specifyed register).
 * @param length the length of bits.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Standard_readRegisterBits (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Processor       processor,
    IN  Dword           registerAddress,
    IN  Byte            position,
    IN  Byte            length,
    OUT Byte*           value
);


/**
 * Send the command to device.
 *
 * @param demodulator the handle of demodulator.
 * @param command the command to be send.
 * @param chip The index of demodulator. The possible values are
 *        0~7.
 * @param processor The processor of specified register. Because each chip
 *        has two processor so user have to specify the processor. The
 *        possible values are Processor_LINK and Processor_OFDM.
 * @param writeBufferLength the number of registers to be write.
 * @param writeBuffer a byte array which is used to store values to be write.
 * @param readBufferLength the number of registers to be read.
 * @param readBuffer a byte array which is used to store values to be read.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Standard_sendCommand (
    IN  Demodulator*    demodulator,
    OUT Word            command,
    IN  Byte            chip,
    IN  Processor       processor,
    IN  Dword           writeBufferLength,
    IN  Byte*           writeBuffer,
    IN  Dword           readBufferLength,
    OUT Byte*           readBuffer
);


/**
 * Get the version of hardware.
 *
 * @param demodulator the handle of demodulator.
 * @param version the version of hardware.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Standard_getHardwareVersion (
    IN  Demodulator*    demodulator,
    OUT Dword*          version
);


/**
 * Get the version of firmware.
 *
 * @param demodulator the handle of demodulator.
 * @param version the version of firmware.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Standard_getFirmwareVersion (
    IN  Demodulator*    demodulator,
    IN  Processor       processor,
    OUT Dword*          version
);


/**
 * Get RF AGC gain.
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are
 *        0~7.
 * @param rfAgc the value of RF AGC.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Standard_getRfAgcGain (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    OUT Byte*           rfAgc
);


/**
 * Get IF AGC gain.
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are
 *        0~7.
 * @param ifAgc the value of IF AGC.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Standard_getIfAgcGain (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    OUT Byte*           ifAgc
);


/**
 * Load the IR table for USB device.
 *
 * @param demodulator the handle of demodulator.
 * @param tableLength The length of IR table.
 * @param table The content of IR table.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Standard_loadIrTable (
    IN  Demodulator*    demodulator,
    IN  Word            tableLength,
    IN  Byte*           table
);


/**
 * Program the bandwidth related parameters to demodulator.
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are
 *        0~7. NOTE: When the architecture is set to Architecture_DCA
 *        this parameter is regard as don't care.
 * @param bandwidth DVB channel bandwidth in MHz. The possible values
 *        are 5, 6, 7, and 8 (MHz).
 * @param adcFrequency The value of desire internal ADC frequency (Hz).
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Standard_selectBandwidth (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Word            bandwidth,          /** KHz                 */
    IN  Dword           adcFrequency        /** Hz, ex: 20480000    */
);


/**
 * Mask DCA output.
 *
 * @param demodulator the handle of demodulator.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Standard_maskDcaOutput (
    IN  Demodulator*    demodulator
);


/**
 * Load firmware to device
 *
 * @param demodulator the handle of demodulator.
 * @streamType current stream type (useless for Ganymede).
 * @firmwareCodes pointer to fw binary.
 * @firmwareSegments pointer to fw segments.
 * @firmwarePartitions pointer to fw partition (useless for Ganymede).
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Standard_loadFirmware (
    IN  Demodulator*    demodulator,
    IN  Byte*           firmwareCodes,
    IN  Segment*        firmwareSegments,
    IN  Byte*           firmwarePartitions
);

/**
 * First, download firmware from host to demodulator. Actually, firmware is
 * put in firmware.h as a part of source code. Therefore, in order to
 * update firmware the host have to re-compile the source code.
 * Second, setting all parameters which will be need at the beginning.
 *
 * @param demodulator the handle of demodulator.
 * @param chipNumber The total number of demodulators.
 * @param sawBandwidth SAW filter bandwidth in MHz. The possible values
 *        are 6000, 7000, and 8000 (KHz).
 * @param streamType The format of output stream.
 * @param architecture the architecture of demodulator.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Standard_initialize (
    IN  Demodulator*    demodulator,
    IN  Byte            chipNumber,
    IN  Word            sawBandwidth,
    IN  StreamType      streamType,
    IN  Architecture    architecture
);


/**
 * Power off the demodulators.
 *
 * @param demodulator the handle of demodulator.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Standard_finalize (
    IN  Demodulator*    demodulator
);


/**
 *
 * @param demodulator the handle of demodulator.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Standard_isAgcLocked (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    OUT Bool*           locked
);


/**
 *
 * @param demodulator the handle of demodulator.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Standard_isCfoeLocked (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    OUT Bool*           locked
);


/**
 *
 * @param demodulator the handle of demodulator.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Standard_isSfoeLocked (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    OUT Bool*           locked
);


/**
 *
 * @param demodulator the handle of demodulator.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Standard_isTpsLocked (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    OUT Bool*           locked
);


/**
 *
 * @param demodulator the handle of demodulator.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Standard_isMpeg2Locked (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    OUT Bool*           locked
);


/**
 *
 * @param demodulator the handle of demodulator.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @param chip The index of demodulator. The possible values are
 *        0~7. NOTE: When the architecture is set to Architecture_DCA
 *        this parameter is regard as don't care.
 * @param locked the result of frequency tuning. True if there is
 *        demodulator can lock signal, False otherwise.
 * @example <pre>
 * </pre>
 */
Dword Standard_isLocked (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    OUT Bool*           locked
);


/**
 * Set priorty of modulation.
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are
 *        0~7.
 * @param priority modulation priority.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Standard_setPriority (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Priority        priority
);


/**
 * Reset demodulator.
 *
 * @param demodulator the handle of demodulator.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Standard_reset (
    IN  Demodulator*    demodulator
);


/**
 * Get channel modulation related information.
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are
 *        0~7.
 * @param channelModulation The modulation of channel.
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 */
Dword Standard_getChannelModulation (
    IN  Demodulator*            demodulator,
    IN  Byte                    chip,
    OUT ChannelModulation*      channelModulation
);


/**
 * Set channel modulation related information.
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are
 *        0~7.
 * @param channelModulation The modulation of channel.
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 */
Dword Standard_setChannelModulation (
    IN  Demodulator*            demodulator,
    IN  Byte                    chip,
    IN  ChannelModulation*      channelModulation
);


/**
 * Set frequency.
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are
 *        0~7.
 * @param frequency The desired frequency.
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 */
Dword Standard_setFrequency (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Dword           frequency
);


/**
 * Specify the bandwidth of channel and tune the channel to the specific
 * frequency. Afterwards, host could use output parameter dvbH to determine
 * if there is a DVB-H signal.
 * In DVB-T mode, after calling this function the output parameter dvbH
 * should return False and host could use output parameter "locked" to check
 * if the channel has correct TS output.
 * In DVB-H mode, after calling this function the output parameter dvbH should
 * return True and host could start get platform thereafter.
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are
 *        0~7. NOTE: When the architecture is set to Architecture_DCA
 *        this parameter is regard as don't care.
 * @param bandwidth The channel bandwidth.
 *        DVB-T: 5000, 6000, 7000, and 8000 (KHz).
 *        DVB-H: 5000, 6000, 7000, and 8000 (KHz).
 *        T-DMB: 5000, 6000, 7000, and 8000 (KHz).
 *        FM: 100, and 200 (KHz).
 * @param frequency the channel frequency in KHz.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Standard_acquireChannel (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Word            bandwidth,
    IN  Dword           frequency
);



/**
 * Set the output stream type of chip. Because the device could output in
 * many stream type, therefore host have to choose one type before receive
 * data.
 *
 * Note: Please refer to the example of Standard_acquireChannel when host want
 *       to detect the available channels.
 * Note: After host know all the available channels, and want to change to
 *       specific channel, host have to choose output mode before receive
 *       data. Please refer the example of Standard_setStreamType.
 *
 * @param demodulator the handle of demodulator.
 * @param streamType the possible values are
 *        DVB-H:    StreamType_DVBH_DATAGRAM
 *                  StreamType_DVBH_DATABURST
 *        DVB-T:    StreamType_DVBT_DATAGRAM
 *                  StreamType_DVBT_PARALLEL
 *                  StreamType_DVBT_SERIAL
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Standard_setStreamType (
    IN  Demodulator*    demodulator,
    IN  StreamType      streamType
);


/**
 * Set the architecture of chip. When two of our device are using, they could
 * be operated in Diversity Combine Architecture (DCA) or (PIP). Therefore,
 * host could decide which mode to be operated.
 *
 * @param demodulator the handle of demodulator.
 * @param architecture the possible values are
 *        Architecture_DCA
 *        Architecture_PIP
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Standard_setArchitecture (
    IN  Demodulator*    demodulator,
    IN  Architecture    architecture
);


/**
 * Set the counting range for Post-Viterbi and Post-Viterbi.
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are
 *        0~7. NOTE: When the architecture is set to Architecture_DCA
 *        this parameter is regard as don't care.
 * @param postErrorCount the number of super frame for Pre-Viterbi.
 * @param postBitCount the number of packet unit for Post-Viterbi.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Standard_getPostVitBer (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    OUT Dword*          postErrorCount, /** 24 bits */
    OUT Dword*          postBitCount,   /** 16 bits */
    OUT Word*           abortCount
);


/**
 * Get siganl quality.
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are
 *        0~7. NOTE: When the architecture is set to Architecture_DCA
 *        this parameter is regard as don't care.
 * @param quality The value of signal quality.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Standard_getSignalQuality (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    OUT Byte*           quality
);


/**
 * Get siganl strength.
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are
 *        0~7. NOTE: When the architecture is set to Architecture_DCA
 *        this parameter is regard as don't care.
 * @param strength The value of signal strength.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Standard_getSignalStrength (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    OUT Byte*           strength
);


/**
 * Get signal strength in dbm
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are
 *        0~7.
 * @param rfpullUpVolt_X10 the pullup voltag of RF multiply 10.
 * @param ifpullUpVolt_X10 the pullup voltag of IF multiply 10.
 * @param strengthDbm The value of signal strength in DBm.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Standard_getSignalStrengthDbm (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Long            rfpullUpVolt_X10,     /** RF pull up voltage multiplied by 10   */
    IN  Long            ifpullUpVolt_X10,     /** IF pull up voltage multiplied by 10   */
    OUT Long*           strengthDbm           /** DBm                                   */
);


/**
 * Set the counting range for Pre-Viterbi and Post-Viterbi.
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are
 *        0~7. NOTE: When the architecture is set to Architecture_DCA
 *        this parameter is regard as don't care.
 * @param frameCount the number of super frame for Pre-Viterbi.
 * @param packetUnit the number of packet unit for Post-Viterbi.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Standard_setStatisticRange (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Byte            superFrameCount,
    IN  Word            packetUnit
);


/**
 * Get the counting range for Pre-Viterbi and Post-Viterbi.
 *
 * @param demodulator the handle of demodulator.
 * @param frameCount the number of super frame for Pre-Viterbi.
 * @param packetUnit the number of packet unit for Post-Viterbi.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Standard_getStatisticRange (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Byte*           frameCount,
    IN  Word*           packetUnit
);


/**
 * Get the statistic values of demodulator, it includes Pre-Viterbi BER,
 * Post-Viterbi BER, Abort Count, Signal Presented Flag, Signal Locked Flag,
 * Signal Quality, Signal Strength, Delta-T for DVB-H time slicing.
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are
 *        0~7.
 * @param statistic the structure that store all statistic values.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Standard_getStatistic (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    OUT Statistic*      statistic
);


/**
 * Get interrupt status.
 *
 * @param demodulator the handle of demodulator.
 * @param interrupts the type of interrupts.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Standard_getInterrupts (
    IN  Demodulator*    demodulator,
    OUT Interrupts*     interrupts
);


/**
 * Clear interrupt status.
 *
 * @param demodulator the handle of demodulator.
 * @param interrupt interrupt name.
 * @param packetUnit the number of packet unit for Post-Viterbi.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Standard_clearInterrupt (
    IN  Demodulator*    demodulator,
    IN  Interrupt       interrupt
);


/**
 * Get data length.
 * In DVB-T mode, data length should always equals 2K,
 * In DVB-H mode, data length would be the length of IP datagram.
 * NOTE: data can't be transfer via I2C bus, in order to transfer data
 * host must provide SPI bus.
 *
 * @param demodulator the handle of demodulator.
 * @param dataLength the length of data.
 * @param valid True if the data length is valid.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Standard_getDataLength (
    IN  Demodulator*    demodulator,
    OUT Dword*          dataLength,
    OUT Bool*           valid
);


/**
 * Get DVB-T data.
 *
 * @param demodulator the handle of demodulator.
 * @param bufferLength the length of buffer.
 * @param buffer buffer used to get Data.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Standard_getData (
    IN  Demodulator*    demodulator,
    IN  Dword           bufferLength,
    OUT Byte*           buffer
);


/**
 * Get datagram from device.
 *
 * @param demodulator the handle of demodulator.
 * @param bufferLength the number of registers to be read.
 * @param buffer a byte array which is used to store values to be read.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @return Error_BUFFER_INSUFFICIENT: if buffer is too small.
 */
Dword Standard_getDatagram (
    IN  Demodulator*    demodulator,
    OUT Dword*          bufferLength,
    OUT Byte*           buffer
);


/**
 *
 * @param demodulator the handle of demodulator.
 * @param code the value of IR raw code, the size should be 4 or 6,
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Standard_getIrCode (
    IN  Demodulator*    demodulator,
    OUT Dword*          code
);


/**
 * Return to boot code
 *
 * @param demodulator the handle of demodulator.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Standard_reboot (
    IN  Demodulator*    demodulator
);


/**
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are
 *        0~7.
 * @param contorl 1: Power up, 0: Power down;
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Standard_controlPowerSaving (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Byte            control
);


/**
 *
 * @param demodulator the handle of demodulator.
 * @param contorl 1: Power up, 0: Power down;
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Standard_controlTunerPowerSaving (
    IN  Demodulator*    demodulator,
    IN  Byte            control
);


/**
 *
 * @param demodulator the handle of demodulator.
 * @param code the address of function pointer in firmware.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Standard_runCode (
    IN  Demodulator*    demodulator,
    IN  Word            code
);


/**
 * Control PID fileter
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are
 *        0~7.
 * @param contorl 0: Disable, 1: Enable.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Standard_controlPidFilter (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Byte            control
);


/**
 * Reset PID filter.
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are
 *        0~7.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Standard_resetPidFilter (
    IN  Demodulator*    demodulator,
    IN  Byte            chip
);


/**
 * Add PID to PID filter.
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are
 *        0~7.
 * @param pid the PID that will be add to PID filter.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Standard_addPidToFilter (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Byte            index,
    IN  Pid             pid
);


/**
 * Set datagram burst size.
 *
 * @param demodulator the handle of demodulator.
 * @param burstSize the burst size.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @return Error_NOT_SUPPORT: if the burst size is not support.
 */
Dword Standard_setBurstSize (
    IN  Demodulator*    demodulator,
    IN  BurstSize       burstSize
);


/**
 * Get datagram burst size.
 *
 * @param demodulator the handle of demodulator.
 * @param burstSize the burst size.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @return Error_NOT_SUPPORT: if the burst size is not support.
 */
Dword Standard_getBurstSize (
    IN  Demodulator*    demodulator,
    IN  BurstSize*      burstSize
);
#endif

