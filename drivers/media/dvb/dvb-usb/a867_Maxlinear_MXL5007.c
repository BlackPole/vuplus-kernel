/**
 * Maxlinear_MXL5007.cpp
 *
 * Interface btw Afa9035 and Mxl5007T
 */

#include "a867_Maxlinear_MXL5007.h"
#include "a867_Maxlinear_MXL5007_Script.h"
#include "a867_mxl5007t.h"
#include "a867_af903x.h"

//m100 todo MxL5007_TunerConfigS MxL5007_TunerConfig = 0;
struct mxl5007t_config MxL5007_TunerConfig;

Dword MXL5007_WriteI2C(
	IN  Demodulator*	demodulator,
	IN  Byte			chip,
	IN  Byte*			pAddress,
	IN  Byte*			pData,
	IN  Dword			count
) {
    Dword error = Error_NO_ERROR;                /* Status to be returned        */
    Byte buffer[25];
    //Byte numberOfRegisters = 9;
    Byte i;

/** single write */
    for (i = 0; i < count; i++) {
        buffer[0] = *(pAddress + i);
        buffer[1] = *(pData + i);
        //buffer[2] = 0xFE;  /** Latch CMD request by MXL5005 Confidential Datasheet */
		error = Standard_writeTunerRegisters (demodulator, chip, buffer[0], 1, &buffer[1]);
		if (error){
			deb_data("MXL5007_WriteI2C failed");
			goto exit;
		}
    }
exit:
    return (error);
}


UINT32 MxL_I2C_Write(UINT8 DeviceAddr, void* pArray, UINT32 ByteCount, struct mxl5007t_config* myTuner)
{
    Dword error = Error_NO_ERROR;                /* Status to be returned */
    // Byte buffer[25];
    // Byte i;
	Byte* pData;
	pData = (Byte*)pArray;
    
/** single write */
    //buffer[2] = 0xFE;  /** Latch CMD request by MXL5005 Confidential Datasheet */
	if (ByteCount == 1) 
		error = Standard_writeTunerRegisters ((myTuner->demodulator), (myTuner->chip), pData[0], 0, NULL);
	else if (ByteCount > 1)
		error = Standard_writeTunerRegisters ((myTuner->demodulator), (myTuner->chip), pData[0], ByteCount - 1, &pData[1]);
	else
	{	
		error = Error_INVALID_DATA_LENGTH;
		deb_data("MxL_I2C_Write:: ByteCount = 0"); 
	}

	if (error){
		deb_data("MxL_I2C_Write failed");
		goto exit;
	}

exit:
    return (error);
}


UINT32 MxL_I2C_Read(UINT8 DeviceAddr, UINT8 Addr, UINT8* mData, struct mxl5007t_config* myTuner)
{
	Dword error = Error_NO_ERROR;

	error = Standard_writeTunerRegisters(myTuner->demodulator, myTuner->chip, 0xFB, 1, &Addr);
	error = Standard_readTunerRegisters (myTuner->demodulator, myTuner->chip, 0xffff, 1, mData);

	return (error);
}

void MxL_Delay(UINT32 mSec)
{
	User_delay(NULL, mSec);
}


Dword MXL5007_open (
	IN  Demodulator*	demodulator,
	IN  Byte			chip
) {
	Ganymede* ganymede;
	ganymede = (Ganymede*) demodulator;
	MxL5007_TunerConfig.demodulator = demodulator;
	MxL5007_TunerConfig.chip = chip;

	//MxL5007_TunerConfig.I2C_Addr = MxL_I2C_ADDR_096;	// 7-bit address
	MxL5007_TunerConfig.I2C_Addr = ganymede->tunerDescription->tunerAddress; //Get from tunerDescription

	//MxL5007_TunerConfig.IF_Freq = MxL_IF_4_57_MHZ;
	switch( ganymede->tunerDescription->ifFrequency ) {
	case 4000000:
		MxL5007_TunerConfig.if_freq_hz = MxL_IF_4_MHZ; break;
	case 4500000:
		MxL5007_TunerConfig.if_freq_hz = MxL_IF_4_5_MHZ; break;
	case 4570000:
		MxL5007_TunerConfig.if_freq_hz = MxL_IF_4_57_MHZ; break;
	case 5000000:
		MxL5007_TunerConfig.if_freq_hz = MxL_IF_5_MHZ; break;
	case 5380000:
		MxL5007_TunerConfig.if_freq_hz = MxL_IF_5_38_MHZ; break;
	case 6000000:
		MxL5007_TunerConfig.if_freq_hz = MxL_IF_6_MHZ; break;
	case 6280000:
		MxL5007_TunerConfig.if_freq_hz = MxL_IF_6_28_MHZ; break;
	case 9191500:
		MxL5007_TunerConfig.if_freq_hz = MxL_IF_9_1915_MHZ; break;
	case 35250000:
		MxL5007_TunerConfig.if_freq_hz = MxL_IF_35_25_MHZ; break;
	case 36150000:
		MxL5007_TunerConfig.if_freq_hz = MxL_IF_36_15_MHZ; break;
	case 44000000:
		MxL5007_TunerConfig.if_freq_hz = MxL_IF_44_MHZ; break;
	default:
		MxL5007_TunerConfig.if_freq_hz = MxL_IF_4_57_MHZ;
		printk("IF unsupported\n");
		break;
	}

	//MxL5007_TunerConfig.IF_Spectrum = MxL_NORMAL_IF;
	if (ganymede->tunerDescription->inversion == True)
		MxL5007_TunerConfig.invert_if = 1; //Get from tunerDescription
	else
		MxL5007_TunerConfig.invert_if = 0;

	MxL5007_TunerConfig.Mode = MxL_MODE_DVBT;
	MxL5007_TunerConfig.if_diff_out_level = -8; //Setting for Cable mode only
	MxL5007_TunerConfig.xtal_freq_hz = MxL_XTAL_24_MHZ;	//24Mhz
	MxL5007_TunerConfig.clk_out_enable = 0;
	MxL5007_TunerConfig.clk_out_amp = MxL_CLKOUT_AMP_0_94V;
	
	if( ganymede->booted )
	a867_mxl5007t_attach(&MxL5007_TunerConfig);
	deb_data("MxL5007 Open");

    return (Error_NO_ERROR);
}


Dword MXL5007_close (
	IN  Demodulator*	demodulator,
	IN  Byte			chip
) {
	a867_mxl5007t_release(MxL5007_TunerConfig.state);
    return (Error_NO_ERROR);
}


Dword MXL5007_set (
	IN  Demodulator*	demodulator,
	IN  Byte			chip,
	IN  Word			bandwidth,
	IN  Dword			frequency
) {
	// Dword status = 0;       

	deb_data("MxL5007 Set");
	if(	a867_mxl5007t_set_params(MxL5007_TunerConfig.state, (enum mxl5007t_bw_mhz) (bandwidth / 1000), frequency * 1000) ) {
		deb_data("MxL5007 Set fail");
		return (Error_WRITE_TUNER_FAIL);
	}

	return (Error_NO_ERROR);
}


Dword SwPowerCtrlMXL5007(
	IN  Demodulator*	demodulator,
	IN  Byte			chip,
	IN  Byte			control			/** 0 for power down mode, 1 for normal operation mode */
)
{
    return (Error_NO_ERROR);
}



TunerDescription tuner_MXL5007 = {
    MXL5007_open,
    MXL5007_close,
    MXL5007_set,
    MXL5007_scripts,
    MXL5007_scriptSets,
    0xC0,                           /** tuner i2c address */
    1,                              /** length of tuner register address */
    4570000,				                /* ref. mxl5007t.h/mxl5007t_if_freq */
    False                           /** spectrum inverse */
};




