#include "a867_cmd.h"


Byte Cmd_sequence = 0;


Dword Bus_addChecksum (
    IN  Demodulator*    demodulator,
    OUT Dword*          bufferLength,
    OUT Byte*           buffer
) {
    Dword error = Error_NO_ERROR;
    Dword loop = (*bufferLength - 1) / 2;
    Dword remain = (*bufferLength - 1) % 2;
    Dword i;
    Word checksum = 0;

    for (i = 0; i < loop; i++)
        checksum += (Word) (buffer[2 * i + 1] << 8) + (Word) (buffer[2 * i + 2]);
    if (remain)
        checksum += (Word) (buffer[*bufferLength - 1] << 8);
    checksum = ~checksum;
    buffer[*bufferLength] = (Byte) ((checksum & 0xFF00) >> 8);
    buffer[*bufferLength + 1] = (Byte) (checksum & 0x00FF);
    buffer[0] = (Byte) (*bufferLength + 1);
    *bufferLength += 2;

    return (error);
}


Dword Bus_removeChecksum (
    IN  Demodulator*    demodulator,
    OUT Dword*          bufferLength,
    OUT Byte*           buffer
) {
    Dword error = Error_NO_ERROR;
    Dword loop = (*bufferLength - 3) / 2;
    Dword remain = (*bufferLength - 3) % 2;
    Dword i;
    Word checksum = 0;

    for (i = 0; i < loop; i++)
        checksum += (Word) (buffer[2 * i + 1] << 8) + (Word) (buffer[2 * i + 2]);
    if (remain)
        checksum += (Word) (buffer[*bufferLength - 3] << 8);
    checksum = ~checksum;
    if (((Word) (buffer[*bufferLength - 2] << 8) + (Word) (buffer[*bufferLength - 1])) != checksum) {
        error = Error_WRONG_CHECKSUM;
        goto exit;
    }
    if (buffer[2])
        error = Error_FIRMWARE_STATUS | buffer[2];
    buffer[0] = (Byte) (*bufferLength - 3);
    *bufferLength -= 2;

exit :
    return (error);
}


Dword Cmd_writeRegisters (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Processor       processor,
    IN  Dword           registerAddress,
    IN  Byte            registerAddressLength,
    IN  Dword           writeBufferLength,
    IN  Byte*           writeBuffer
) {
    Dword error = Error_NO_ERROR;
    Word command;
    Byte buffer[256];
    Dword bufferLength;
    Dword i, j, k;
    Ganymede* ganymede;
    CmdDescription*     pcmdDesc;
    BusDescription*     pbusDesc;
    Dword maxPktSize;

    User_enterCriticalSection (demodulator);

    if (writeBufferLength == 0) goto exit;
    if (registerAddressLength > 4) {
        error  = Error_PROTOCOL_FORMAT_INVALID;
        goto exit;
    }

    ganymede = (Ganymede*) demodulator;
    pcmdDesc = ganymede->cmdDescription;
    pbusDesc = pcmdDesc->busDescription;
    maxPktSize = pcmdDesc->mailBoxSize;

    i = 0;
    while (i < writeBufferLength) {

        j = (writeBufferLength - i) > (maxPktSize - 12) ? (maxPktSize - 12) : (writeBufferLength - i);
        command = Bus_buildCommand (Command_REG_DEMOD_WRITE, processor, chip);
        buffer[1] = (Byte) (command >> 8);
        buffer[2] = (Byte) command;
        buffer[3] = (Byte) Cmd_sequence++;
        buffer[4] = (Byte) j;
        buffer[5] = (Byte) registerAddressLength;
        buffer[6] = (Byte) ((registerAddress + i) >> 24); /** Get first byte of reg. address  */
        buffer[7] = (Byte) ((registerAddress + i) >> 16); /** Get second byte of reg. address */
        buffer[8] = (Byte) ((registerAddress + i) >> 8);  /** Get third byte of reg. address  */
        buffer[9] = (Byte) (registerAddress + i);         /** Get fourth byte of reg. address */

        for (k = 0; k < j; k++)
            buffer[10 + k] = writeBuffer[i + k];

        bufferLength = j + 10;
        error = Bus_addChecksum (demodulator, &bufferLength, buffer);
        if (error) goto exit;

        error = pbusDesc->busTx (demodulator, bufferLength, buffer);
        if (error) goto exit;

        bufferLength = 5;

        error = pbusDesc->busRx (demodulator, bufferLength, buffer);
        if (error) goto exit;

        error = Bus_removeChecksum (demodulator, &bufferLength, buffer);
        if (error) goto exit;

        i += j;
    }

exit :
    User_leaveCriticalSection (demodulator);
    return (error);
}


Dword Cmd_writeScatterRegisters (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Processor       processor,
    IN  Byte            valueSetsAddressLength,
    IN  Byte            valueSetsLength,
    IN  ValueSet*       valueSets
) {
    Dword error = Error_NO_ERROR;
    Word command;
    Byte buffer[256];
    Dword bufferLength;
    Dword i, j, k;
    Ganymede* ganymede;
    CmdDescription*     pcmdDesc;
    BusDescription*     pbusDesc;
    Dword maxPktSize;

    User_enterCriticalSection (demodulator);

    if (valueSetsLength == 0) goto exit;
    if (valueSetsAddressLength > 4) {
        error  = Error_PROTOCOL_FORMAT_INVALID;
        goto exit;
    }

    ganymede = (Ganymede*) demodulator;
    pcmdDesc = ganymede->cmdDescription;
    pbusDesc = pcmdDesc->busDescription;
    maxPktSize = pcmdDesc->mailBoxSize;

    i = 0;
    while (i < valueSetsLength) {
        j = (valueSetsLength - i) > ((maxPktSize - 10) / 3) ? ((maxPktSize - 10) / 3) : (valueSetsLength - i);
        command = Bus_buildCommand (Command_SCATTER_WRITE, processor, chip);
        buffer[1] = (Byte) (command >> 8);
        buffer[2] = (Byte) command;
        buffer[3] = (Byte) Cmd_sequence++;
        buffer[4] = (Byte) 2;                       /** Type 2 */
        if (processor == Processor_LINK)
            buffer[5] = (Byte) 0;
        else
            buffer[5] = (Byte) 1;
        buffer[6] = (Byte) 0;
        buffer[7] = (Byte) j;

        for (k = 0; k < j; k++) {
            buffer[8 + k * 2] = (Byte) (valueSets[k].address >> 8);
            buffer[9 + k * 2] = (Byte) valueSets[k].address;
            buffer[8 + j * 2 + k] = (Byte) valueSets[k].value;
        }

        bufferLength = 8 + j * 3;
        error = Bus_addChecksum (demodulator, &bufferLength, buffer);
        if (error) goto exit;

        error = pbusDesc->busTx (demodulator, bufferLength, buffer);
        if (error) goto exit;

        bufferLength =  5;

        error = pbusDesc->busRx (demodulator, bufferLength, buffer);
        if (error) goto exit;

        error = Bus_removeChecksum (demodulator, &bufferLength, buffer);
        if (error) goto exit;

        i += j;
    }

exit :
    User_leaveCriticalSection (demodulator);
    return (error);
}


Dword Cmd_writeTunerRegisters (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Byte            tunerAddress,
    IN  Word            registerAddress,
    IN  Byte            registerAddressLength,
    IN  Byte            writeBufferLength,
    IN  Byte*           writeBuffer
) {

    Dword error = Error_NO_ERROR;
    Word command;
    Byte buffer[256];
    Dword bufferLength;
    Dword i, j, k;
    Ganymede* ganymede;
    CmdDescription*     pcmdDesc;
    BusDescription*     pbusDesc;
    Dword maxPktSize;

    User_enterCriticalSection (demodulator);

    ganymede = (Ganymede*) demodulator;
    pcmdDesc = ganymede->cmdDescription;
    pbusDesc = pcmdDesc->busDescription;
    maxPktSize = pcmdDesc->mailBoxSize;

	if (writeBufferLength == 0)
	{
        command = Bus_buildCommand (Command_REG_TUNER_WRITE, Processor_LINK, chip);
        buffer[1] = (Byte) (command >> 8);
        buffer[2] = (Byte) command;
        buffer[3] = (Byte) Cmd_sequence++;
        buffer[4] = (Byte) 0;
        buffer[5] = (Byte) tunerAddress;
        buffer[6] = (Byte) registerAddressLength;
        buffer[7] = (Byte) ((registerAddress) >> 8);  /** Get high byte of reg. address */
        buffer[8] = (Byte) (registerAddress);         /** Get low byte of reg. address  */

        bufferLength = 9;
        error = Bus_addChecksum (demodulator, &bufferLength, buffer);
        if (error) goto exit;

        error = pbusDesc->busTx (demodulator, bufferLength, buffer);
        if (error) goto exit;

        bufferLength = 5;

        error = pbusDesc->busRx (demodulator, bufferLength, buffer);
        if (error) goto exit;

        error = Bus_removeChecksum (demodulator, &bufferLength, buffer);
        if (error) goto exit;

		goto exit;
	}

    i = 0;
    while (i < writeBufferLength) {
        j = (writeBufferLength - i) > (maxPktSize - 11) ? (maxPktSize - 11) : (writeBufferLength - i);
        command = Bus_buildCommand (Command_REG_TUNER_WRITE, Processor_LINK, chip);
        buffer[1] = (Byte) (command >> 8);
        buffer[2] = (Byte) command;
        buffer[3] = (Byte) Cmd_sequence++;
        buffer[4] = (Byte) j;
        buffer[5] = (Byte) tunerAddress;
        buffer[6] = (Byte) registerAddressLength;
        buffer[7] = (Byte) ((registerAddress + i) >> 8);  /** Get high byte of reg. address */
        buffer[8] = (Byte) (registerAddress + i);         /** Get low byte of reg. address  */

        for (k = 0; k < j; k++)
            buffer[9 + k] = writeBuffer[ i + k];

        bufferLength = j + 9;
        error = Bus_addChecksum (demodulator, &bufferLength, buffer);
        if (error) goto exit;

        error = pbusDesc->busTx (demodulator, bufferLength, buffer);
        if (error) goto exit;

        bufferLength = 5;

        error = pbusDesc->busRx (demodulator, bufferLength, buffer);
        if (error) goto exit;

        error = Bus_removeChecksum (demodulator, &bufferLength, buffer);
        if (error) goto exit;

        i += j;
    }


exit :    
    User_leaveCriticalSection (demodulator);
    return (error);
}


Dword Cmd_writeEepromValues (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Byte            eepromAddress,
    IN  Word            registerAddress,
    IN  Byte            registerAddressLength,
    IN  Byte            writeBufferLength,
    IN  Byte*           writeBuffer
) {
    Dword error = Error_NO_ERROR;
    Word command;
    Byte buffer[256];
    Dword bufferLength;
    Byte i;
    Ganymede* ganymede;
    CmdDescription*     pcmdDesc;
    BusDescription*     pbusDesc;
    Dword maxPktSize;

    User_enterCriticalSection (demodulator);

    if (writeBufferLength == 0) goto exit;

    ganymede = (Ganymede*) demodulator;
    pcmdDesc = ganymede->cmdDescription;
    pbusDesc = pcmdDesc->busDescription;
    maxPktSize = pcmdDesc->mailBoxSize;

    command = Bus_buildCommand (Command_REG_EEPROM_WRITE, Processor_LINK, chip);
    buffer[1] = (Byte) (command >> 8);
    buffer[2] = (Byte) command;
    buffer[3] = (Byte) Cmd_sequence++;
    buffer[4] = (Byte) writeBufferLength;
    buffer[5] = (Byte) eepromAddress;
    buffer[6] = (Byte) registerAddressLength;
    buffer[7] = (Byte) (registerAddress >> 8);  /** Get high byte of reg. address */
    buffer[8] = (Byte) registerAddress;         /** Get low byte of reg. address  */

    for (i = 0; i < writeBufferLength; i++)
        buffer[9 + i] = writeBuffer[i];

    bufferLength = writeBufferLength + 9;
    error = Bus_addChecksum (demodulator, &bufferLength, buffer);
    if (error) goto exit;

    error = pbusDesc->busTx (demodulator, bufferLength, buffer);
    if (error) goto exit;

    bufferLength = 5;

    error = pbusDesc->busRx (demodulator, bufferLength, buffer);
    if (error) goto exit;

    error = Bus_removeChecksum (demodulator, &bufferLength, buffer);
    if (error) goto exit;

exit :
    User_leaveCriticalSection (demodulator);
    return (error);
}


Dword Cmd_readRegisters (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Processor       processor,
    IN  Dword           registerAddress,
    IN  Byte            registerAddressLength,
    IN  Dword           readBufferLength,
    OUT Byte*           readBuffer
) {
    Dword error = Error_NO_ERROR;
    Word command;
    Byte buffer[256];
    Dword bufferLength;
    Dword i, j, k;
    Ganymede* ganymede;
    CmdDescription*     pcmdDesc;
    BusDescription*     pbusDesc;
    Dword maxPktSize;

    User_enterCriticalSection (demodulator);

    if (readBufferLength == 0) goto exit;
    if (registerAddressLength > 4) {
        error  = Error_PROTOCOL_FORMAT_INVALID;
        goto exit;
    }

    ganymede = (Ganymede*) demodulator;
    pcmdDesc = ganymede->cmdDescription;
    pbusDesc = pcmdDesc->busDescription;
    maxPktSize = pcmdDesc->mailBoxSize;

    i = 0;
    while (i < readBufferLength) {

        j = (readBufferLength - i) > (maxPktSize - 5) ? (maxPktSize - 5) : (readBufferLength - i);
        command = Bus_buildCommand (Command_REG_DEMOD_READ, processor, chip);
        buffer[1] = (Byte) (command >> 8);
        buffer[2] = (Byte) command;
        buffer[3] = (Byte) Cmd_sequence++;
        buffer[4] = (Byte) j;
        buffer[5] = (Byte) registerAddressLength;
        buffer[6] = (Byte) ((registerAddress + i) >> 24); /** Get first byte of reg. address  */
        buffer[7] = (Byte) ((registerAddress + i) >> 16); /** Get second byte of reg. address */
        buffer[8] = (Byte) ((registerAddress + i) >> 8);  /** Get third byte of reg. address  */
        buffer[9] = (Byte) (registerAddress + i);         /** Get fourth byte of reg. address */

        bufferLength = 10;
        error = Bus_addChecksum (demodulator, &bufferLength, buffer);
        if (error) goto exit;

        error = pbusDesc->busTx (demodulator, bufferLength, buffer);
        if (error) goto exit;

        bufferLength = j + 5;

        error = pbusDesc->busRx (demodulator, bufferLength, buffer);
        if (error) goto exit;

        error = Bus_removeChecksum (demodulator, &bufferLength, buffer);
        if (error) goto exit;
        for (k = 0; k < j; k++) {
            readBuffer[i + k] = buffer[k + 3];
        }

        i += j;
    }

exit :
    User_leaveCriticalSection (demodulator);
    return (error);
}


Dword Cmd_readScatterRegisters (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Processor       processor,
    IN  Byte            valueSetsAddressLength,
    IN  Byte            valueSetsLength,
    OUT ValueSet*       valueSets
) {
    Dword error = Error_NO_ERROR;
    Word command;
    Byte buffer[256];
    Dword bufferLength;
    Dword i, j, k;
    Ganymede* ganymede;
    CmdDescription*     pcmdDesc;
    BusDescription*     pbusDesc;
    Dword maxPktSize;

    User_enterCriticalSection (demodulator);

    if (valueSetsLength == 0) goto exit;
    if (valueSetsAddressLength > 4) {
        error  = Error_PROTOCOL_FORMAT_INVALID;
        goto exit;
    }

    ganymede = (Ganymede*) demodulator;
    pcmdDesc = ganymede->cmdDescription;
    pbusDesc = pcmdDesc->busDescription;
    maxPktSize = pcmdDesc->mailBoxSize;

    i = 0;
    while (i < valueSetsLength) {
        j = (valueSetsLength - i) > ((maxPktSize - 10) / 2) ? ((maxPktSize - 10) / 2) : (valueSetsLength - i);
        command = Bus_buildCommand (Command_SCATTER_READ, processor, chip);
        buffer[1] = (Byte) (command >> 8);
        buffer[2] = (Byte) command;
        buffer[3] = (Byte) Cmd_sequence++;
        buffer[4] = (Byte) 2;                       /** Type 2 */
        if (processor == Processor_LINK)
            buffer[5] = (Byte) 0;
        else
            buffer[5] = (Byte) 1;
        buffer[6] = (Byte) 0;
        buffer[7] = (Byte) j;

        for (k = 0; k < j; k++) {
            buffer[8 + k * 2] = (Byte) (valueSets[k].address >> 8);
            buffer[9 + k * 2] = (Byte) valueSets[k].address;
        }

        bufferLength = 8 + j * 2;
        error = Bus_addChecksum (demodulator, &bufferLength, buffer);
        if (error) goto exit;

        error = pbusDesc->busTx (demodulator, bufferLength, buffer);
        if (error) goto exit;

        bufferLength = j + 5;

        error = pbusDesc->busRx (demodulator, bufferLength, buffer);
        if (error) goto exit;

        error = Bus_removeChecksum (demodulator, &bufferLength, buffer);
        if (error) goto exit;
        for (k = 0; k < j; k++) {
            //(Byte) valueSets[i + k].value = buffer[k + 3];
	      valueSets[i + k].value = (Byte) buffer[k + 3];
        }

        i += j;
    }

exit :
    User_leaveCriticalSection (demodulator);
    return (error);
}


Dword Cmd_readTunerRegisters (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Byte            tunerAddress,
    IN  Word            registerAddress,
    IN  Byte            registerAddressLength,
    IN  Byte            readBufferLength,
    IN  Byte*           readBuffer
) {
    Dword error = Error_NO_ERROR;
    Word command;
    Byte buffer[256];
    Dword bufferLength;
    Dword i, j, k;
    Ganymede* ganymede;
    CmdDescription*     pcmdDesc;
    BusDescription*     pbusDesc;
    Dword maxPktSize;

    User_enterCriticalSection (demodulator);

    if (readBufferLength == 0) goto exit;

    ganymede = (Ganymede*) demodulator;
    pcmdDesc = ganymede->cmdDescription;
    pbusDesc = pcmdDesc->busDescription;
    maxPktSize = pcmdDesc->mailBoxSize;

    i = 0;
    while (i < readBufferLength) {
        j = (readBufferLength - i) > (maxPktSize - 5) ? (maxPktSize - 5) : (readBufferLength - i);
        command = Bus_buildCommand (Command_REG_TUNER_READ, Processor_LINK, chip);
        buffer[1] = (Byte) (command >> 8);
        buffer[2] = (Byte) command;
        buffer[3] = (Byte) Cmd_sequence++;
        buffer[4] = (Byte) j;
        buffer[5] = (Byte) tunerAddress;
        buffer[6] = (Byte) registerAddressLength;
        buffer[7] = (Byte) ((registerAddress + i) >> 8);  /** Get high byte of reg. address */
        buffer[8] = (Byte) (registerAddress + i);         /** Get low byte of reg. address  */

        bufferLength = 9;
        error = Bus_addChecksum (demodulator, &bufferLength, buffer);
        if (error) goto exit;

        error = pbusDesc->busTx (demodulator, bufferLength, buffer);
        if (error) goto exit;

        bufferLength = j + 5;

        error = pbusDesc->busRx (demodulator, bufferLength, buffer);
        if (error) goto exit;

        error = Bus_removeChecksum (demodulator, &bufferLength, buffer);
        if (error) goto exit;
        for (k = 0; k < j; k++) {
            readBuffer[i + k] = buffer[k + 3];
        }

        i += j;
    }

exit :
    User_leaveCriticalSection (demodulator);
    return (error);
}


Dword Cmd_readEepromValues (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Byte            eepromAddress,
    IN  Word            registerAddress,
    IN  Byte            registerAddressLength,
    IN  Byte            readBufferLength,
    OUT Byte*           readBuffer
) {
    Dword error = Error_NO_ERROR;
    Word command;
    Byte buffer[256];
    Dword bufferLength;
    Byte i;
    Ganymede* ganymede;
    CmdDescription*     pcmdDesc;
    BusDescription*     pbusDesc;
    Dword maxPktSize;

    User_enterCriticalSection (demodulator);

    if (readBufferLength == 0) goto exit;

    ganymede = (Ganymede*) demodulator;
    pcmdDesc = ganymede->cmdDescription;
    pbusDesc = pcmdDesc->busDescription;
    maxPktSize = pcmdDesc->mailBoxSize;

    command = Bus_buildCommand (Command_REG_EEPROM_READ, Processor_LINK, chip);
    buffer[1] = (Byte) (command >> 8);
    buffer[2] = (Byte) command;
    buffer[3] = (Byte) Cmd_sequence++;
    buffer[4] = (Byte) readBufferLength;
    buffer[5] = (Byte) eepromAddress;
    buffer[6] = (Byte) registerAddressLength;
    buffer[7] = (Byte) (registerAddress >> 8);  /** Get high byte of reg. address */
    buffer[8] = (Byte) registerAddress;         /** Get low byte of reg. address  */

    bufferLength = 9;
    error = Bus_addChecksum (demodulator, &bufferLength, buffer);
    if (error) goto exit;

    error = pbusDesc->busTx (demodulator, bufferLength, buffer);
    if (error) goto exit;

    bufferLength = readBufferLength + 5;
    error = pbusDesc->busRx (demodulator, bufferLength, buffer);
    if (error) goto exit;

    error = Bus_removeChecksum (demodulator, &bufferLength, buffer);
    if (error) goto exit;
    for (i = 0; i < readBufferLength; i++) {
        readBuffer[i] = buffer[i + 3];
    }

exit :
    User_leaveCriticalSection (demodulator);
    return (error);
}


Dword Cmd_modifyRegister (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
    IN  Processor       processor,
    IN  Dword           registerAddress,
    IN  Byte            registerAddressLength,
    IN  Byte            position,
    IN  Byte            length,
    IN  Byte            value
) {
    Dword error = Error_NO_ERROR;
    Word command;
    Byte buffer[256];
    Dword bufferLength;
    Byte temp;
    Ganymede* ganymede;
    CmdDescription*     pcmdDesc;
    BusDescription*     pbusDesc;
    Dword maxPktSize;

    User_enterCriticalSection (demodulator);

    if (registerAddressLength > 4) {
        error  = Error_PROTOCOL_FORMAT_INVALID;
        goto exit;
    }

    ganymede = (Ganymede*) demodulator;
    pcmdDesc = ganymede->cmdDescription;
    pbusDesc = pcmdDesc->busDescription;
    maxPktSize = pcmdDesc->mailBoxSize;

    command = Bus_buildCommand (Command_REG_DEMOD_READ, processor, chip);
    buffer[1] = (Byte) (command >> 8);
    buffer[2] = (Byte) command;
    buffer[3] = (Byte) Cmd_sequence++;
    buffer[4] = (Byte) 1;
    buffer[5] = (Byte) registerAddressLength;
    buffer[6] = (Byte) (registerAddress >> 24); /** Get first byte of reg. address  */
    buffer[7] = (Byte) (registerAddress >> 16); /** Get second byte of reg. address */
    buffer[8] = (Byte) (registerAddress >> 8);  /** Get third byte of reg. address  */
    buffer[9] = (Byte) registerAddress;         /** Get fourth byte of reg. address */

    bufferLength = 10;
    error = Bus_addChecksum (demodulator, &bufferLength, buffer);
    if (error) goto exit;

    error = pbusDesc->busTx (demodulator, bufferLength, buffer);
    if (error) goto exit;

    bufferLength = 6;

    error = pbusDesc->busRx (demodulator, bufferLength, buffer);
    if (error) goto exit;

    error = Bus_removeChecksum (demodulator, &bufferLength, buffer);
    if (error) goto exit;
    temp = buffer[3];
    temp = REG_CREATE (value, temp, position, length);

    command = Bus_buildCommand (Command_REG_DEMOD_WRITE, processor, chip);
    buffer[1] = (Byte) (command >> 8);
    buffer[2] = (Byte) command;
    buffer[3] = (Byte) Cmd_sequence++;
    buffer[4] = (Byte) 1;
    buffer[5] = (Byte) registerAddressLength;
    buffer[6] = (Byte) (registerAddress >> 24); /** Get first byte of reg. address  */
    buffer[7] = (Byte) (registerAddress >> 16); /** Get second byte of reg. address */
    buffer[8] = (Byte) (registerAddress >> 8);  /** Get third byte of reg. address  */
    buffer[9] = (Byte) registerAddress;         /** Get fourth byte of reg. address */
    buffer[10] = temp;

    bufferLength = 11;
    error = Bus_addChecksum (demodulator, &bufferLength, buffer);
    if (error) goto exit;

    error = pbusDesc->busTx (demodulator, bufferLength, buffer);
    if (error) goto exit;

    bufferLength = 5;

    error = pbusDesc->busRx (demodulator, bufferLength, buffer);
    if (error) goto exit;

    error = Bus_removeChecksum (demodulator, &bufferLength, buffer);
    if (error) goto exit;

exit :
    User_leaveCriticalSection (demodulator);
    return (error);
}


Dword Cmd_loadFirmware (
    IN  Demodulator*    demodulator,
    IN  Dword           length,
    IN  Byte*           firmware
) {
    Dword error = Error_NO_ERROR;
    Word command;
    Dword loop;
    Dword remain;
    Dword i, j, k;
    Byte buffer[256];
    Dword bufferLength;
    Ganymede* ganymede;
    CmdDescription*     pcmdDesc;
    BusDescription*     pbusDesc;
    Dword maxPktSize;

    User_enterCriticalSection (demodulator);

    ganymede = (Ganymede*) demodulator;
    pcmdDesc = ganymede->cmdDescription;
    pbusDesc = pcmdDesc->busDescription;
    maxPktSize = pcmdDesc->mailBoxSize;

    loop = length / (maxPktSize - 6);
    remain = length % (maxPktSize - 6);
    k = 0;
    command = Bus_buildCommand (Command_FW_DOWNLOAD, Processor_LINK, 0);
    for (i = 0; i < loop; i++) {
        buffer[1] = (Byte) (command >> 8);
        buffer[2] = (Byte) command;
        buffer[3] = (Byte) Cmd_sequence++;
        for (j = 0; j < (maxPktSize - 6); j++)
            buffer[4 + j] = firmware[k++];
        bufferLength = (maxPktSize - 2);
        error = Bus_addChecksum (demodulator, &bufferLength, buffer);
        if (error) goto exit;

        error = pbusDesc->busTx (demodulator, bufferLength, buffer);
        if (error) goto exit;
    }
    if (remain) {
        buffer[1] = (Byte) (command >> 8);
        buffer[2] = (Byte) command;
        buffer[3] = (Byte) Cmd_sequence++;
        for (j = 0; j < remain; j++)
            buffer[4 + j] = firmware[k++];
        bufferLength = (Word) (4 + remain);
        error = Bus_addChecksum (demodulator, &bufferLength, buffer);
        if (error) goto exit;

        error = pbusDesc->busTx (demodulator, bufferLength, buffer);
        if (error) goto exit;
    }

exit :
    User_leaveCriticalSection (demodulator);
    return (error);
}


Dword Cmd_reboot (
    IN  Demodulator*    demodulator,
    IN  Byte            chip
) {
    Dword error = Error_NO_ERROR;
    Word command;
    Byte buffer[256];
    Dword bufferLength;

    Ganymede* ganymede;
    CmdDescription*     pcmdDesc;
    BusDescription*     pbusDesc;
    Dword maxPktSize;

    ganymede = (Ganymede*) demodulator;
    pcmdDesc = ganymede->cmdDescription;
    pbusDesc = pcmdDesc->busDescription;
    maxPktSize = pcmdDesc->mailBoxSize;

    User_enterCriticalSection (demodulator);

    command = Bus_buildCommand (Command_REBOOT, Processor_LINK, chip);
    buffer[1] = (Byte) (command >> 8);
    buffer[2] = (Byte) command;
    buffer[3] = (Byte) Cmd_sequence++;
    bufferLength = 4;
    error = Bus_addChecksum (demodulator, &bufferLength, buffer);
    if (error) goto exit;

    error = pbusDesc->busTx (demodulator, bufferLength, buffer);
    if (error) goto exit;

exit :
    User_leaveCriticalSection (demodulator);
    return (error);
}


Dword Cmd_sendCommand (
    IN  Demodulator*    demodulator,
    IN  Word            command,
    IN  Byte            chip,
    IN  Processor       processor,
    IN  Dword           writeBufferLength,
    IN  Byte*           writeBuffer,
    IN  Dword           readBufferLength,
    OUT Byte*           readBuffer
) {
    Dword error = Error_NO_ERROR;
    Byte buffer[256];
    Dword bufferLength;
    Dword i, j, k;
    Ganymede* ganymede;
    CmdDescription*     pcmdDesc;
    BusDescription*     pbusDesc;
    Dword maxPktSize;

    User_enterCriticalSection (demodulator);

    ganymede = (Ganymede*) demodulator;
    pcmdDesc = ganymede->cmdDescription;
    pbusDesc = pcmdDesc->busDescription;
    maxPktSize = pcmdDesc->mailBoxSize;

    if (writeBufferLength == 0) {
        command = Bus_buildCommand (command, processor, chip);
        buffer[1] = (Byte) (command >> 8);
        buffer[2] = (Byte) command;
        buffer[3] = (Byte) Cmd_sequence++;
        bufferLength = 4;
        error = Bus_addChecksum (demodulator, &bufferLength, buffer);
        if (error) goto exit;

        error = pbusDesc->busTx (demodulator, bufferLength, buffer);
        if (error) goto exit;
    } else {
        i = 0;
        while (i < writeBufferLength) {
            j = (writeBufferLength - i) > (maxPktSize - 6) ? (maxPktSize - 6) : (writeBufferLength - i);
            command = Bus_buildCommand (command, processor, chip);
            buffer[1] = (Byte) (command >> 8);
            buffer[2] = (Byte) command;
            buffer[3] = (Byte) Cmd_sequence++;
            for (k = 0; k < j; k++)
                buffer[k + 4] = writeBuffer[i + k];

            bufferLength = j + 4;
            error = Bus_addChecksum (demodulator, &bufferLength, buffer);
            if (error) goto exit;

            error = pbusDesc->busTx (demodulator, bufferLength, buffer);
            if (error) goto exit;

            i += j;
        }
    }

    if (readBufferLength == 0) {
        bufferLength = 5;

        error = pbusDesc->busRx (demodulator, bufferLength, buffer);
        if (error) goto exit;

        error = Bus_removeChecksum (demodulator, &bufferLength, buffer);
        if (error) goto exit;
    } else {
        i = 0;
        while (i < readBufferLength) {
            j = (readBufferLength - i) > (maxPktSize - 5) ? (maxPktSize - 5) : (readBufferLength - i);

            bufferLength = j + 5;

            error = pbusDesc->busRx (demodulator, bufferLength, buffer);
            if (error) goto exit;

            error = Bus_removeChecksum (demodulator, &bufferLength, buffer);
            if (error) goto exit;
            for (k = 0; k < j; k++)
                readBuffer[i + k] = buffer[k + 3];

            i += j;
        }
    }

exit :
    User_leaveCriticalSection (demodulator);
    return (error);
}


Dword Cmd_receiveData (
    IN  Demodulator*    demodulator,
    IN  Dword           registerAddress,
    IN  Dword           readBufferLength,
    OUT Byte*           readBuffer
) {
    Dword error = Error_NO_ERROR;
    Word command;
    Byte buffer[256];
    Dword bufferLength;
    Ganymede* ganymede;
    CmdDescription*     pcmdDesc;
    BusDescription*     pbusDesc;
    Dword maxPktSize;

    if (readBufferLength == 0) goto exit;

    ganymede = (Ganymede*) demodulator;
    pcmdDesc = ganymede->cmdDescription;
    pbusDesc = pcmdDesc->busDescription;
    maxPktSize = pcmdDesc->mailBoxSize;

    User_enterCriticalSection (demodulator);

    command = Bus_buildCommand (Command_DATA_READ, Processor_LINK, 0);
    buffer[1] = (Byte) (command >> 8);
    buffer[2] = (Byte) command;
    buffer[3] = (Byte) Cmd_sequence++;
    buffer[4] = (Byte) ((readBufferLength >> 16)  & 0xFF);
    buffer[5] = (Byte) ((readBufferLength >> 8)  & 0xFF);
    buffer[6] = (Byte) (readBufferLength  & 0xFF);
    buffer[7] = (Byte) ((registerAddress >> 16)  & 0xFF);
    buffer[8] = (Byte) ((registerAddress >> 8)  & 0xFF);
    buffer[9] = (Byte) (registerAddress  & 0xFF);

    bufferLength = 10;
    error = Bus_addChecksum (demodulator, &bufferLength, buffer);
    if (error) goto exit;

    error = pbusDesc->busTx (demodulator, bufferLength, buffer);
    if (error) goto exit;

    error = pbusDesc->busRxData (demodulator, readBufferLength, readBuffer);

exit :
    User_leaveCriticalSection (demodulator);
    return (error);
}


Word Cmd_busId = Bus_USB;
CmdDescription Cmd_busDescription = {
    0,
    NULL,
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
};
