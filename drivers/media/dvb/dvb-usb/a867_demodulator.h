#ifndef __GANYMEDE_H__
#define __GANYMEDE_H__


#include "a867_type.h"
#include "a867_user.h"
#include "a867_error.h"
#include "a867_register.h"
#include "a867_variable.h"
#include "a867_cmd.h"
#include "a867_standard.h"
#include "a867_demodulatorextend.h"  /** release1remove */
#include "a867_version.h"

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
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     Ganymede ganymede;
 *
 *     // Set the value of register 0xA000 in demodulator to 0.
 *     error = Demodulator_writeRegister ((Demodulator*) &ganymede, 0, Processor_LINK, 0xA000, 0);
 *     if (error)
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("Success");
 * </pre>
 */
Dword Demodulator_writeRegister (
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
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     Byte buffer[3] = { 0x00, 0x01, 0x02 };
 *     Ganymede ganymede;
 *
 *     // Set the value of register 0xA000 in demodulator to 0.
 *     // Set the value of register 0xA001 in demodulator to 1.
 *     // Set the value of register 0xA002 in demodulator to 2.
 *     error = Demodulator_writeRegisters ((Demodulator*) &ganymede, 0, Processor_LINK, 0xA000, 3, buffer);
 *     if (error)
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("Success");
 * </pre>
 */
Dword Demodulator_writeRegisters (
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
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     ValueSet valueSet[3];
 *     Ganymede ganymede;
 *
 *     // Get the value of register 0xA000, 0xA001 and 0xA002 in demodulator.
 *     valueSet[0].address = 0xA000;
 *     valueSet[0].value = 0x00;
 *     valueSet[1].address = 0xA001;
 *     valueSet[1].value = 0x01;
 *     valueSet[2].address = 0xA002;
 *     valueSet[2].value = 0x02;
 *     error = Demodulator_writeScatterRegisters ((Demodulator*) &ganymede, 0, Processor_LINK, 3, valueSet);
 *     if (error)
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("Success");
 * </pre>
 */
Dword Demodulator_writeScatterRegisters (
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
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     Byte buffer[3] = { 0x00, 0x01, 0x02 };
 *     Ganymede ganymede;
 *
 *     // Set the value of register 0x0000 in tuner to 0.
 *     // Set the value of register 0x0001 in tuner to 1.
 *     // Set the value of register 0x0002 in tuner to 2.
 *     error = Demodulator_writeTunerRegistersWithAddress ((Demodulator*) &ganymede, 0, 0x38, 0x00, 1, 3, buffer);
 *     if (error)
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("Success");
 * </pre>
 */
Dword Demodulator_writeTunerRegisters (
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
Dword Demodulator_writeGenericRegisters (
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
 * @param registerAddressLength the valid bytes of registerAddress.
 * @param bufferLength the number of cells to be written.
 * @param buffer a byte array which is used to store values to be written.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     Byte buffer[3] = { 0x00, 0x01, 0x02 };
 *     Ganymede ganymede;
 *
 *     // Set the value of cell 0x0000 in EEPROM to 0.
 *     // Set the value of cell 0x0001 in EEPROM to 1.
 *     // Set the value of cell 0x0002 in EEPROM to 2.
 *     error = Demodulator_writeEepromValues ((Demodulator*) &ganymede, 0, 0x0000, 3, buffer);
 *     if (error)
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("Success");
 * </pre>
 */
Dword Demodulator_writeEepromValues (
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
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     Ganymede ganymede;
 *
 *     // Modify the LSB of register 0xA000 in demodulator to 0.
 *     error = Demodulator_writeRegisterBits ((Demodulator*) &ganymede, 0, Processor_LINK, 0xA000, 0, 1, 0);
 *     if (error)
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("Success");
 * </pre>
 */
Dword Demodulator_writeRegisterBits (
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
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     Byte value;
 *     Ganymede ganymede;
 *
 *     // Get the value of register 0xA000 in demodulator.
 *     error = Demodulator_readRegister ((Demodulator*) &ganymede, 0, Processor_LINK, 0xA000, &value);
 *     if (error)
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("Success");
 *     printf ("The value of 0xA000 is %2x", value);
 * </pre>
 */
Dword Demodulator_readRegister (
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
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     Byte buffer[3];
 *     Ganymede ganymede;
 *
 *     // Get the value of register 0xA000, 0xA001, 0xA002 in demodulator.
 *     error = Demodulator_readRegisters ((Demodulator*) &ganymede, 0, Processor_LINK, 0xA000, 3, buffer);
 *     if (error)
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("Success");
 *     printf ("The value of 0xA000 is %2x", buffer[0]);
 *     printf ("The value of 0xA001 is %2x", buffer[1]);
 *     printf ("The value of 0xA002 is %2x", buffer[2]);
 * </pre>
 */
Dword Demodulator_readRegisters (
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
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     ValueSet valueSet[3];
 *     Ganymede ganymede;
 *
 *     // Get the value of register 0xA000, 0xA001 and 0xA002 in demodulator.
 *     valueSet[0].address = 0xA000;
 *     valueSet[1].address = 0xA001;
 *     valueSet[2].address = 0xA002;
 *     error = Demodulator_readScatterRegisters ((Demodulator*) &ganymede, 0, Processor_LINK, 3, valueSet);
 *     if (error)
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("Success");
 *     printf ("The value of 0xA000 is %2x", valueSet[0].value);
 *     printf ("The value of 0xA001 is %2x", valueSet[1].value);
 *     printf ("The value of 0xA002 is %2x", valueSet[2].value);
 * </pre>
 */
Dword Demodulator_readScatterRegisters (
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
 * @param registerAddressLength the valid bytes of registerAddress.
 * @param bufferLength the number of registers to be read.
 * @param buffer a byte array which is used to store values to be read.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     Byte buffer[3];
 *     Ganymede ganymede;
 *
 *     // Get the value of register 0x0000, 0x0001, 0x0002 in tuner.
 *     error = Demodulator_readTunerRegisters ((Demodulator*) &ganymede, 0, 0x0000, 3, buffer);
 *     if (error)
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("Success");
 *     printf ("The value of 0x0000 is %2x", buffer[0]);
 *     printf ("The value of 0x0001 is %2x", buffer[1]);
 *     printf ("The value of 0x0002 is %2x", buffer[2]);
 * </pre>
 */
Dword Demodulator_readTunerRegisters (
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
Dword Demodulator_readGenericRegisters (
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
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     Byte buffer[3];
 *     Ganymede ganymede;
 *
 *     // Get the value of cell 0x0000, 0x0001, 0x0002 in EEPROM.
 *     error = Demodulator_readEepromValues ((Demodulator*) &ganymede, 0, 0x0000, 3, buffer);
 *     if (error)
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("Success");
 *     printf ("The value of 0x0000 is %2x", buffer[0]);
 *     printf ("The value of 0x0001 is %2x", buffer[1]);
 *     printf ("The value of 0x0002 is %2x", buffer[2]);
 * </pre>
 */
Dword Demodulator_readEepromValues (
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
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     Byte value;
 *     Ganymede ganymede;
 *
 *     // Read the LSB of register 0xA000 in demodulator.
 *     error = Demodulator_readRegisterBits ((Demodulator*) &ganymede, 0, Processor_LINK, 0xA000, 0, 1, &value);
 *     if (error)
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("Success");
 *     printf ("The value of LSB of 0xA000 is %2x", value);
 * </pre>
 */
Dword Demodulator_readRegisterBits (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Processor       processor,
    IN  Dword           registerAddress,
    IN  Byte            position,
    IN  Byte            length,
    OUT Byte*           value
);


/**
 * Get the version of hardware.
 *
 * @param demodulator the handle of demodulator.
 * @param version the version of hardware.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     Dword version;
 *     Ganymede ganymede;
 *
 *     // Add PID to PID filter.
 *     error = Demodulator_getHardwareVersion ((Demodulator*) &ganymede, &version);
 *     if (error)
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("The version of hardware is : %X", version);
 * </pre>
 */
Dword Demodulator_getHardwareVersion (
    IN  Demodulator*    demodulator,
    OUT Dword*          version
);


/**
 * Get the version of firmware.
 *
 * @param demodulator the handle of demodulator.
 * @param version the version of firmware.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     Dword version;
 *     Ganymede ganymede;
 *
 *     // Get the version of Link layer firmware.
 *     error = Demodulator_getFirmwareVersion ((Demodulator*) &ganymede, Processor_LINK, &version);
 *     if (error)
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("The version of firmware is : %X", version);
 * </pre>
 */
Dword Demodulator_getFirmwareVersion (
    IN  Demodulator*    demodulator,
    IN  Processor       processor,
    OUT Dword*          version
);


/**
 * Add PID to PID filter.
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are
 *        0~7.
 * @param index the index of PID filter.
 * @param pid the PID that will be add to PID filter.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     Pid pid;
 *     Ganymede ganymede;
 *
 *     pid.value = 0x0000;
 *
 *     // Add PID to PID filter.
 *     error = Demodulator_addPidToFilter ((Demodulator*) &ganymede, 0, 1, pid);
 *     if (error)
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("Success");
 * </pre>
 */
Dword Demodulator_addPidToFilter (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Byte            index,
    IN  Pid             pid
);


/**
 * Reset PID filter.
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are
 *        0~7.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     Ganymede ganymede;
 *
 *     error = Demodulator_resetPidFilter ((Demodulator*) &ganymede, 0);
 *     if (error)
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("Success");
 * </pre>
 */
Dword Demodulator_resetPidFilter (
    IN  Demodulator*    demodulator,
    IN  Byte            chip
);


/**
 * Get datagram from device.
 *
 * @param demodulator the handle of demodulator.
 * @param bufferLength the number of registers to be read.
 * @param buffer a byte array which is used to store values to be read.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @return Error_BUFFER_INSUFFICIENT: if buffer is too small.
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     Word bufferLength;
 *     Byte buffer[4096];
 *     Ganymede ganymede;
 *
 *     bufferLength = 4096;
 *     error = Demodulator_getDatagram ((Demodulator*) &ganymede, &bufferLength, buffer);
 *     if (error)
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("Success");
 * </pre>
 */
Dword Demodulator_getDatagram (
    IN  Demodulator*    demodulator,
    OUT Dword*          bufferLength,
    OUT Byte*           buffer
);


/**
 * Get RF AGC gain.
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are
 *        0~7.
 * @param rfAgc the value of RF AGC.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     Byte rfAgc;
 *     Ganymede ganymede;
 *
 *     // Set I2C as the control bus.
 *     error = Demodulator_getRfAgcGain ((Demodulator*) &ganymede, 0, rfAgc);
 *     if (error)
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("Success");
 * </pre>
 */
Dword Demodulator_getRfAgcGain (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    OUT Byte*           rfAgc
);


/**
 * Get IF AGC.
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are
 *        0~7.
 * @param ifAgc the value of IF AGC.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     Byte ifAgc;
 *     Ganymede ganymede;
 *
 *     // Set I2C as the control bus.
 *     error = Demodulator_getIfAgcGain ((Demodulator*) &ganymede, 0, ifAgc);
 *     if (error)
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("Success");
 * </pre>
 */
Dword Demodulator_getIfAgcGain (
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
 * @example <pre>
 * </pre>
 */
Dword Demodulator_loadIrTable (
    IN  Demodulator*    demodulator,
    IN  Word            tableLength,
    IN  Byte*           table
);


/**
 * Load firmware to device
 *
 * @param demodulator the handle of demodulator.
 * @firmwareCodes pointer to fw binary.
 * @firmwareSegments pointer to fw segments.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Demodulator_loadFirmware (
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
 * @param sawBandwidth SAW filter bandwidth in KHz. The possible values
 *        are 6000, 7000, and 8000 (KHz).
 * @param streamType The format of output stream.
 * @param architecture the architecture of demodulator.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     Ganymede ganymede;
 *
 *     // Initialize demodulators.
 *     // SAW Filter  : 8MHz
 *     // Stream Type : IP Datagram.
 *     error = Demodulator_initialize ((Demodulator*) &ganymede, 1, 8, StreamType_IP_DATAGRAM, Architecture_DCA);
 *     if (error)
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("Success");
 * </pre>
 */
Dword Demodulator_initialize (
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
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     Ganymede ganymede;
 *
 *     // Finalize demodulators.
 *     error = Demodulator_finalize ((Demodulator*) &ganymede);
 *     if (error)
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("Success");
 * </pre>
 */
Dword Demodulator_finalize (
    IN  Demodulator*    demodulator
);


/**
 *
 * @param demodulator the handle of demodulator.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_isAgcLocked (
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
Dword Demodulator_isCfoeLocked (
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
Dword Demodulator_isSfoeLocked (
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
Dword Demodulator_isTpsLocked (
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
Dword Demodulator_isMpeg2Locked (
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
 * @see Demodulator_acquireChannel
 */
Dword Demodulator_isLocked (
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
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     Ganymede ganymede;
 *
 *     // Set priority.
 *     error = Demodulator_setPriority ((Demodulator*) &ganymede, 0, Priority_HIGH);
 *     if (error)
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("Success");
 * </pre>
 */
Dword Demodulator_setPriority (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Priority        priority
);


/**
 * Reset demodulator.
 *
 * @param demodulator the handle of demodulator.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     Ganymede ganymede;
 *
 *     // Reset demodulator.
 *     error = Demodulator_reset ((Demodulator*) &ganymede);
 *     if (error)
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("Success");
 * </pre>
 */
Dword Demodulator_reset (
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
 * @example <pre>
 * </pre>
 */
Dword Demodulator_getChannelModulation (
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
 * @example <pre>
 * </pre>
 */
Dword Demodulator_setChannelModulation (
    IN  Demodulator*            demodulator,
    IN  Byte                    chip,
    IN  ChannelModulation*      channelModulation
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
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     Bool locked;
 *     Ganymede ganymede;
 *
 *     error = Demodulator_acquireChannel ((Demodulator*) &ganymede, 0, 8000, 666000);
 *     if (error)
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("Success");
 *
 *     error = Demodulator_isLocked ((Demodulator*) &ganymede, 0, &locked);
 *     if (error)
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("Success");
 *
 *     if (locked == True) {
 *         // In DVB-T mode.
 *         // Start to process TS
 *         // Because DVB-T could be multiplex with DVB-H
 *     }
 * </pre>
 */
Dword Demodulator_acquireChannel (
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
 * Note: After host know all the available channels, and want to change to
 *       specific channel, host have to choose output mode before receive
 *       data. Please refer the example of Demodulator_setStreamType.
 *
 * @param demodulator the handle of demodulator.
 * @param streamType the possible values are
 *        DVB-H:    StreamType_DVBH_DATAGRAM
 *                  StreamType_DVBH_DATABURST
 *        DVB-T:    StreamType_DVBT_DATAGRAM
 *                  StreamType_DVBT_PARALLEL
 *                  StreamType_DVBT_SERIAL
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     Ganymede ganymede;
 *
 *     error = Demodulator_setStreamType ((Demodulator*) &ganymede, StreamType_DVBT_PARALLEL)
 *     if (error)
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("Success");
 * </pre>
 */
Dword Demodulator_setStreamType (
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
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     Ganymede ganymede;
 *
 *     // Set architecture.
 *     error = Demodulator_setArchitecture ((Demodulator*) &ganymede, Architecture_DCA)
 *     if (error)
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("Success");
 * </pre>
 */
Dword Demodulator_setArchitecture (
    IN  Demodulator*    demodulator,
    IN  Architecture    architecture
);


/**
 * Get the length of Data
 * In DVB-T mode, data length should always equals 2K,
 * In DVB-H mode, data length would be the length of IP datagram.
 * NOTE: data can't be transfer via I2C bus, in order to transfer data
 * host must provide SPI bus.
 *
 * @param demodulator the handle of demodulator.
 * @param dataLength the length of data.
 * @param valid True if the data length is valid.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @see Demodulator_addIp
 */
Dword Demodulator_getDataLength (
    IN  Demodulator*    demodulator,
    OUT Dword*          dataLength,
    OUT Bool*           valid
);


/**
 * Get the IP datagram of Data.
 *
 * @param demodulator the handle of demodulator.
 * @param bufferLength the length of buffer.
 * @param buffer buffer used to get Data.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @see Demodulator_addIp
 */
Dword Demodulator_getData (
    IN  Demodulator*    demodulator,
    IN  Dword           bufferLength,
    OUT Byte*           buffer
);


/**
 * Get the type of interrupts.
 *
 * @param demodulator the handle of demodulator.
 * @param interrupts the type of interrupts.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     Interrupt interrupts;
 *     Ganymede ganymede;
 *
 *     // Get the type of interrupts.
 *     error = Demodulator_getInterrupts ((Demodulator*) &ganymede, &interrupts);
 *     if (error)
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("Success");
 *     if (interrupts & Interrupt_VERSION) {
 *         // Get IP version
 *     }
 *     if (interrupts & Interrupt_DVBH) {
 *         // Get DVB-H Data
 *     }
 *     if (interrupts & Interrupt_DVBT) {
 *         // Get DVB-T Data
 *     }
 *     if (interrupts & Interrupt_SIPSI) {
 *         // Get SI/PSI
 *     }
 * </pre>
 */
Dword Demodulator_getInterrupts (
    IN  Demodulator*    demodulator,
    OUT Interrupts*     interrupts
);


/**
 * Clear interrupts flag.
 *
 * @param demodulator the handle of demodulator.
 * @param interrupts interrupts flag.
 * @param packetUnit the number of packet unit for Post-Viterbi.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     Ganymede ganymede;
 *
 *     // Set statistic range.
 *     error = Demodulator_clearInterrupt ((Demodulator*) &ganymede, Interrupt_SIPSI);
 *     if (error)
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("Success");
 * </pre>
 */
Dword Demodulator_clearInterrupt (
    IN  Demodulator*    demodulator,
    IN  Interrupt       interrupt
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
 * @example <pre>
 * </pre>
 */
Dword Demodulator_getSignalQuality (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    OUT Byte*           quality
);


/**
 * Get signal strength
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are
 *        0~7.
 * @param strength The value of signal strength.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_getSignalStrength (
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
 * @example <pre>
 * </pre>
 */
Dword Demodulator_getSignalStrengthDbm (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Long            rfpullUpVolt_X10,     /** RF pull up voltage multiplied by 10   */
    IN  Long            ifpullUpVolt_X10,     /** IF pull up voltage multiplied by 10   */
    OUT Long*           strengthDbm           /** DBm                                   */
);


/**
 * Get post VitBer
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are
 *        0~7.
 * @param postErrorCount error count after viterbi
 * @param postBitCount total count after viterbi
 * @param abortCount error count after reed-soloman
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_getPostVitBer (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    OUT Dword*          postErrorCount,  /** 24 bits */
    OUT Dword*          postBitCount,    /** 16 bits */
    OUT Word*           abortCount
);


/**
 * Set the counting range for Pre-Viterbi and Post-Viterbi.
 *
 * @param demodulator the handle of demodulator.
 * @param frameCount the number of super frame for Pre-Viterbi.
 * @param packetUnit the number of packet unit for Post-Viterbi.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     Ganymede ganymede;
 *
 *     // Set statistic range.
 *     error = Demodulator_setStatisticRange ((Demodulator*) &ganymede, 0, 1, 10000);
 *     if (error)
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("Success");
 * </pre>
 */
Dword Demodulator_setStatisticRange (
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
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     Byte superFrameCount;
 *     Word packetUnit;
 *     Ganymede ganymede;
 *
 *     // Set statistic range.
 *     error = Demodulator_getStatisticRange ((Demodulator*) &ganymede, 0, &superFrameCount, &packetUnit);
 *     if (error)
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("Success");
 * </pre>
 */
Dword Demodulator_getStatisticRange (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Byte*           superFrameCount,
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
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     Statistic statistic;
 *     double preBer;
 *     double postBer;
 *     Ganymede ganymede;
 *
 *     // Set statistic range.
 *     error = Demodulator_getStatistic ((Demodulator*) &ganymede, 0, &statistic);
 *     if (error)
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("Success");
 *     preBer = (double) statistic.preVitErrorCount / (double) statistic.preVitBitCount;
 *     printf ("Pre-Viterbi BER = %f\n", preBer);
 *     postBer = (double) statistic.postVitErrorCount / (double) statistic.postVitBitCount;
 *     printf ("Post-Viterbi BER = %f\n", postBer);
 *     printf ("Abort Count = %d\n", statistic.abortCount);
 *     if (statistic.signalPresented == True)
 *         printf ("Signal Presented = True\n");
 *     else
 *         printf ("Signal Presented = False\n");
 *     if (statistic.signalLocked == True)
 *         printf ("Signal Locked = True\n");
 *     else
 *         printf ("Signal Locked = False\n");
 *     printf ("Signal Quality = %d\n", statistic.signalQuality);
 *     printf ("Signal Strength = %d\n", statistic.signalStrength);
 * </pre>
 */
Dword Demodulator_getStatistic (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    OUT Statistic*      statistic
);


/**
 *
 * @param demodulator the handle of demodulator.
 * @param code the value of IR raw code, the size should be 4 or 6,
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_getIrCode (
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
Dword Demodulator_reboot (
    IN  Demodulator*    demodulator
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
Dword Demodulator_controlPidFilter (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Byte            control
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
Dword Demodulator_controlPowerSaving (
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
Dword Demodulator_controlTunerPowerSaving (
    IN  Demodulator*    demodulator,
    IN  Byte            control
);


/**
 * Set datagram burst size.
 *
 * @param demodulator the handle of demodulator.
 * @param burstSize the burst size.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @return Error_NOT_SUPPORT: if the burst size is not support.
 */
Dword Demodulator_setBurstSize (
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
Dword Demodulator_getBurstSize (
    IN  Demodulator*    demodulator,
    IN  BurstSize*      burstSize
);
#endif
