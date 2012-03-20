#ifndef __DEMODULATOREXTEND_H__
#define __DEMODULATOREXTEND_H__


//#include <stdio.h>
//#include <math.h>
#include "a867_type.h"
#include "a867_user.h"
#include "a867_register.h"
#include "a867_error.h"
#include "a867_cmd.h"
/*
#include "i2cimpl.h"
#include "spiimpl.h"
#include "sdioimpl.h"
*/
#include "a867_usb2impl.h"
#include "a867_demodulator.h"


#define Tuner_Panasonic_ENV77H11D5			0x01
#define Tuner_Microtune_MT2060				0x02
#define Tuner_Maxlinear_MXL5003				0x03
#define Tuner_Philip_TD1316AFIHP			0x04
#define Tuner_Freescale_FS803A				0x05
#define Tuner_Quantek_QT1010				0x06
#define Tuner_Panasonic_ENV75H10D8			0x07
#define Tuner_Lg_TDTMG252D					0x08
#define Tuner_Himax_HTXR03A					0x09
#define Tuner_Alps_TDQ44M					0x0A
#define Tuner_Infineon_TUA6045				0x0B
#define Tuner_Infineon_TUA6034				0x0C
#define Tuner_Maxlinear_MXL5005				0x0D
#define Tuner_Thomson_664X					0x0E
#define Tuner_Thomson_6630					0x0F
#define Tuner_Samsung_DTOS403				0x10
#define Tuner_Samsung_DTOS446				0x11
#define Tuner_Freescale_FS803A_DLNA			0x12
#define Tuner_Microtune_MT2060_7SAW			0x13
#define Tuner_Alps_TDQ03					0x14
#define Tuner_Thomson_759X					0x15
#define Tuner_Empire_DTN317					0x16
#define Tuner_Partsnic_PDHTF05D				0x17
#define Tuner_Panasonic_ENG37A30GF			0x18
#define Tuner_Philips_FQD1216ME_MK5			0x19
#define Tuner_Infineon_TUA6041				0x1A
#define Tuner_Philips_TDA18271				0x1B
#define Tuner_Alps_TDQD1X001A				0x1C
#define Tuner_Maxlinear_MXL5005_RSSI		0x1D
#define Tuner_Thomson_75101					0x1E
#define Tuner_Sharp_5056					0x1F
#define Tuner_Freescale_MC44CD02			0x20
#define Tuner_Microtune_MT2260B0			0x21
#define Tuner_Philips_TDA18291HN			0x22
#define Tuner_Microtune_MT2266				0x23
#define	Tuner_Integrant_ITD3020				0x24
#define Tuner_Afatech_PEACOCK				0x25
#define Tuner_Xceive_XC3028L				0x26
#define Tuner_Infineon_TUA9001				0x27
#define Tuner_Fitipower_FC0011				0x28
#define Tuner_Afatech_AF9007				0xFF
#define Tuner_Maxlinear_MXL5007				0xA0

/**
 * Define commands for AGC general set function
 */
#define APO_AGC_SET_RF_ACQUIRE				1
#define APO_AGC_SET_RF_TRACK				2
#define APO_AGC_SET_IF_ACQUIRE				3
#define APO_AGC_SET_IF_TRACK				4
#define APO_AGC_SET_ADC_OUT_DESIRED_S		5
#define APO_AGC_SET_RF_TOP_S				6
#define APO_AGC_SET_IF_TOP_S				7
#define APO_AGC_SET_RF_LOCK_TH_ACQUIRE		8
#define APO_AGC_SET_RF_LOCK_TH_TRACK		9
#define APO_AGC_SET_IF_LOCK_TH_ACQUIRE		10
#define APO_AGC_SET_IF_LOCK_TH_TRACK		11
#define APO_AGC_SET_ADC_OUT_DESIRED_M		12
#define APO_AGC_SET_RF_TOP_M				13
#define APO_AGC_SET_IF_TOP_M				14
#define APO_AGC_SET_RF_TOP					15
#define APO_AGC_SET_IF_TOP					16


/**
 * Define commands for AGC general set function
 */
#define APO_AGC_GET_RF_ACQUIRE				1
#define APO_AGC_GET_RF_TRACK				2
#define APO_AGC_GET_IF_ACQUIRE				3
#define APO_AGC_GET_IF_TRACK				4
#define APO_AGC_GET_RF_LOCK_TH_ACQUIRE		5
#define APO_AGC_GET_RF_LOCK_TH_TRACK		6
#define APO_AGC_GET_IF_LOCK_TH_ACQUIRE		7
#define APO_AGC_GET_IF_LOCK_TH_TRACK		8
#define APO_AGC_GET_RF_MAX					9
#define APO_AGC_GET_RF_MIN					10
#define APO_AGC_GET_RF_TOP_S				11
#define APO_AGC_GET_RF_TOP_M				17
#define APO_AGC_GET_IF_MAX					12
#define APO_AGC_GET_IF_MIN					13
#define APO_AGC_GET_IF_TOP_S				14
#define APO_AGC_GET_IF_TOP_M				18
#define APO_AGC_GET_RF_TOP					19
#define APO_AGC_GET_IF_TOP					20
#define APO_AGC_GET_ADC_OUT_DESIRED_S		15
#define APO_AGC_GET_ADC_OUT_DESIRED_M		16


/**
 * Define Options
 */
#define APO_OPTION_FREQSHIFT				0x00000001
#define APO_OPTION_DYNATOP					0x00000002
#define APO_OPTION_RET_NOW					0x00000004
#define APO_OPTION_REPEAT_RETRAIN			0x00000008


/**
 * Define Demodulator_getDouble index
 */
#define APO_GET_FREQ_SHIFT					1
#define APO_GET_ORIG_RF_TOP					2
#define APO_GET_ORIG_IF_TOP					3
#define APO_GET_FINAL_RF_TOP				4
#define APO_GET_FINAL_IF_TOP				5
#define APO_GET_BEST_RF_TOP					6
#define APO_GET_BEST_IF_TOP					7


/**
 * Define commands for general CE information function
 */
#define APO_AGC_CLEAR_REGS				1
#define APO_AGC_STALL_OFSM_ACCESS		2
#define APO_AGC_RESTORE_OFSM_ACCESS		3


#define APO_DCA_EN_UPPER				0x01
#define APO_DCA_EN_LOWER				0x02
#define APO_DCA_BOTH					0x00


/** keep for internal api release */
/**
 * The type defination of PidTable.
 */
typedef struct {
	Word pid[32];
} PidTable;

typedef struct {
	PidTable pidtable[2];
	Byte pidcount;
	Bool pidinit;
} PidInfo;
/** end keep for internal api release */


extern Word DemodulatorExtend_diversityMode;
extern double DemodulatorExtend_crystalFrequency;
extern Word Tdmb_bitRateTable[64];


/**
 * Set control bus and tuner.
 *
 * @param demodulator the handle of demodulator.
 * @param busId The ID of bus.
 * @param tunerId The ID of tuner.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     Ganymede ganymede;
 *
 *     // Set I2C as the control bus. 
 *     error = Demodulator_setBusTuner ((Demodulator*) &ganymede, Bus_I2C, Tuner_MICROTUNE_MT2060);
 *     if (error) 
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("Success");
 * </pre>
 */
Dword Demodulator_setBusTuner (
	IN  Demodulator*	demodulator,
	IN  Word			busId,
	IN  Word			tunerId
);


/**
 * Set firmware and script.
 *
 * @param demodulator the handle of demodulator.
 * @param crystalFrequency The value of crystal frequency on board (KHz).
 * @param adcFrequency The value of desire internal ADC frequency (Hz).
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     Ganymede ganymede;
 *
 *     // Set frequencies. 
 *     error = Demodulator_setCrystalAdcFrequency ((Demodulator*) &ganymede, 30000, 20156250);
 *     if (error) 
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("Success");
 * </pre>
 */
Dword Demodulator_setCrystalAdcFrequency (
	IN  Demodulator*	demodulator,
	IN  Dword			crystalFrequency,
	IN  Dword			adcFrequency
);


/**
 * Set firmware and script.
 *
 * @param demodulator the handle of demodulator.
 * @param firmwareCodes The byte array of firmware code.
 * @param firmwareSegments The segments of firmwares.
 * @param firmwarePartitions The partitions of firmwares.
 * @param scriptSets The sets of script.
 * @param scripts The byte array of script.
 * @param tunerScriptSets The sets of tunerScript.
 * @param tunerScripts The byte array of tunerScript.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 *     Dword error = Error_NO_ERROR;
 *     Byte firmware[65535];
 *     ValueSet script[256];
 *     ValueSet tunerScript[256];
 *     Ganymede ganymede;
 *
 *     // Set I2C as the control bus. 
 *     error = Demodulator_setFirmwareScript ((Demodulator*) &ganymede, firmware, 65535, script, 256, tunerScript, 256);
 *     if (error) 
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("Success");
 * </pre>
 */
Dword Demodulator_setFirmwareScript (
	IN  Demodulator*	demodulator,
	IN  Byte*			firmwareCodes,
	IN  Segment*		firmwareSegments,
	IN  Byte*			firmwarePartitions,
	IN  Word*			scriptSets,
	IN  ValueSet*		scripts,
	IN  Word*           tunerScriptSets,
	IN  ValueSet*       tunerScripts
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
 *     ChannelStatistic channelStatistic;
 *     double preBer;
 *     double postBer;
 *     Ganymede ganymede;
 *
 *     // Set statistic range. 
 *     error = Demodulator_getChannelStatistic ((Demodulator*) &ganymede, 0, &channelStatistic);
 *     if (error) 
 *         printf ("Error Code = %X", error);
 *     else
 *         printf ("Success");
 *     preBer = (double) channelStatistic.preVitErrorCount / (double) channelStatistic.preVitBitCount;
 *     printf ("Pre-Viterbi BER = %f\n", preBer);
 *     postBer = (double) channelStatistic.postVitErrorCount / (double) channelStatistic.postVitBitCount;
 *     printf ("Post-Viterbi BER = %f\n", postBer);
 *     printf ("Abort Count = %d\n", channelStatistic.abortCount);
 * </pre>
 */
Dword Demodulator_getChannelStatistic (
	IN  Demodulator*			demodulator,
	IN  Byte					chip,
	OUT ChannelStatistic*		channelStatistic
);


/**
 * Set the counting range for Pre-Viterbi and Post-Viterbi. 
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are 
 *        0~7. NOTE: When the architecture is set to Architecture_DCA
 *        this parameter is regard as don't care.
 * @param preErrorCount the number of super frame for Pre-Viterbi.
 * @param preBitCount the number of packet unit for Post-Viterbi.
 * @param snr the signal to noise ratio.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Demodulator_getPreVitBer (
	IN  Demodulator*	demodulator,
	IN  Byte			chip,
	OUT Dword*			preErrorCount,
	OUT Dword*			preBitCount,
	OUT double*			snr
);


/**
 * Set the counting range for Pre-Viterbi and Post-Viterbi. 
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are 
 *        0~7. NOTE: When the architecture is set to Architecture_DCA
 *        this parameter is regard as don't care.
 * @param preErrorCount the number of super frame for Pre-Viterbi.
 * @param preBitCount the number of packet unit for Post-Viterbi.
 * @param snr the signal to noise ratio.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Demodulator_getSoftBer (
	IN  Demodulator*	demodulator,
	IN  Byte			chip,
	OUT Dword*			preErrorCount,
	OUT Dword*			preBitCount,
	OUT double*			snr
);


/**
 * This function is used to get signal quality indicator.
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are 
 *        0~7.
 * @param sqi signal quality indicator.
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 *     Byte sqi;
 * 
 *     Demodulator_getSqi (0x38, 0, &sqi);
 * </pre>
 */
Dword Demodulator_getSqi (
	IN  Demodulator*	demodulator,
	IN  Byte			chip,
    OUT Byte*			sqi
);


/**
 * Get IF agc voltage.
 *
 * @param demodulator the handle of demodulator.
 * @param doPullUpVolt The pull up voltage of tunre.
 * @param dopVolt IF AGC voltage to be returned.
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_getIfAgcVoltage (
	IN  Demodulator*	demodulator,
    IN  double			doPullUpVolt,
    OUT double*			dopVolt
);


/**
 * Set maximum RF agc. 
 *
 * @param demodulator the handle of demodulator.
 * @param doMaxRfAgc The maximum value of RF AGC.
 * @param doVolt RF AGC voltage to be set.
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_setMaxRfAgc (
	IN  Demodulator*	demodulator,
    IN	double			doMaxRfAgc,
	IN  double			doVolt
);


/**
 * Set minimum rf agc.
 *
 * @param demodulator the handle of demodulator.
 * @param doMinRfAgc The minimum value of RF AGC.
 * @param doVolt RF AGC voltage to be set.
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_setMinRfAgc (
	IN  Demodulator*	demodulator,
    IN	double			doMinRfAgc,
	IN  double			doVolt
);


/**
 * Set max if agc.
 *
 * @param demodulator the handle of demodulator.
 * @param doMaxIfAgc The maximum value of IF AGC.
 * @param doVolt IF AGC voltage to be set.
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_setMaxIfAgc (
	IN  Demodulator*	demodulator,
    IN	double			doMaxIfAgc,
	IN  double			doVolt
);


/**
 * Set min if agc. 
 *
 * @param demodulator the handle of demodulator.
 * @param doMinIfAgc The minimum value of IF AGC.
 * @param doVolt IF AGC voltage to be set.
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_setMinIfAgc (
	IN  Demodulator*	demodulator,
    IN	double			doMinIfAgc,
	IN  double			doVolt
);


/**
 * General agc set function.
 *
 * @param demodulator the handle of demodulator.
 * @param ucCmd .
 * @param vpParams .
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_setAgc (
	IN  Demodulator*	demodulator,
	IN  Byte			ucCmd,
    IN	Word*			vpParams
);


/**
 * General agc get function.
 *
 * @param demodulator the handle of demodulator.
 * @param ucCmd .
 * @param vpParams .
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_getAgc (
	IN  Demodulator*	demodulator,
	IN  Byte			ucCmd,
    IN	void*			vpParams
);


/**
 * Check if INR detected.
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are 
 *        0~7.
 * @param count INR count.
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_getInrCount (
	IN  Demodulator*	demodulator,
	IN  Byte			chip,
    OUT Word*			count
);


/**
 * Check if CCI happens.
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are 
 *        0~7.
 * @param cci0 1: CCI happen, 0: CCI doesn't happen.
 * @param cci1 1: CCI happen, 0: CCI doesn't happen.
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_isCci (
	IN  Demodulator*	demodulator,
	IN  Byte			chip,
    OUT Bool*			cci0,
    OUT Bool*			cci1
);


/**
 * Check if ACI happens
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are 
 *        0~7.
 * @param aci0 1: ACI happen, 0: ACI doesn't happen.
 * @param aci1 1: ACI happen, 0: ACI doesn't happen.
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_isAci (
	IN  Demodulator*	demodulator,
	IN  Byte			chip,
    OUT Bool*			aci0,
    OUT Bool*			aci1
);


/**
 * Get frequency offset.
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are 
 *        0~7.
 * @param doTs Sampling period.
 * @param lpNormOffset Normalized frequency offset (carrier spacing).
 * @param lpOffset Frequency offset (22 bits) (Hz).
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_getFrequencyOffset (
	IN  Demodulator*	demodulator,
	IN  Byte			chip,
	IN  double			elementaryPeriod,
	OUT Long*			normalizedOffset,
	OUT Long*			offset
);


/**
 * Get sampling clock offset in second
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are 
 *        0~7.
 * @param adcFrequency ADC frequency.
 * @param elementaryPeriod Sampling period.
 * @param offset ADC sampling clock offset in sec.
 * @param offsetPpm ADC sampling clock offset in PPM.
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_getTimeOffset (
	IN  Demodulator*	demodulator,
	IN  Byte			chip,
    IN  double			adcFrequency,
    IN  double			elementaryPeriod,
    OUT double*			offset,
    OUT double*			offsetPpm
);


/**
 * Set IF1 frequency of MT2060.
 *
 * @param demodulator the handle of demodulator.
 * @param dwIf1 The IF1 frequency (KHz).
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_setMT2060If1 (
	IN  Demodulator*	demodulator,
    IN  Dword			dwIf1
);


/**
 * Clear FFT window position valid bit.
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are 
 *        0~7.
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_resetFftWinPos (
	IN  Demodulator*	demodulator,
	IN  Byte			chip
);


/**
 * Clear FFT window position valid bit.
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are 
 *        0~7.
 * @param delta Delta value for FFT window position.
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_getFftWinPos (
	IN  Demodulator*	demodulator,
	IN  Byte			chip,
    IN	Long*			delta	
);


/**
 * Get crystal frequency (KHz).
 *
 * @param demodulator the handle of demodulator.
 * @param fpFreq Crystal frequency.
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_getXtalFreq (
	IN  Demodulator*	demodulator,
    IN	float*			fpFreq
);


/**
 * Test register.
 *
 * @param demodulator the handle of demodulator.
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_testRegister (
	IN  Demodulator*	demodulator
);


/**
 * Dump register.
 *
 * @param demodulator the handle of demodulator.
 * @param cpFileName The name of file to be write.
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_dumpRegister (
	IN  Demodulator*	demodulator,
    IN  char*			cpFileName
);


/**
 * Get frequency response from hardware.
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are 
 *        0~7.
 * @param tone Sub-Carrier Index ( Real Index = 200*wIndex).
 * @param realPart Real part of Constellation value.
 * @param imaginaryPart Imaginary part of Constellation value.
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_getFrequencyResponse (
	IN  Demodulator*	demodulator,
	IN  Byte			chip,
	OUT Word*			tone,
	OUT Long*			realPart,
	OUT Long*			imaginaryPart
);


/**
 * Get constellation value.
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are 
 *        0~7.
 * @param tone Sub-Carrier Index ( Real Index = 200*wIndex).
 * @param realPart Real part of Constellation value.
 * @param imaginaryPart Imaginary part of Constellation value.
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_getConstellation (
	IN  Demodulator*	demodulator,
	IN  Byte			chip,
	IN  Word			tone,
	OUT float*			realPart,
	OUT float*			imaginaryPart
);


/**
 * Capture constellation value (2).
 *
 * @param demodulator the handle of demodulator.
 * @param wIndex Sub-Carrier Index ( Real Index = 200*wIndex).
 * @param wpReal real part of constellation value.
 * @param wpImag imaginary part of constellation value.
 * @param wpH2 H2 value.
 * @param wpRealH real part of H.
 * @param wpImagH imaginary part of H.
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_capConstellation2 (
	IN  Demodulator*	demodulator,
    IN  Word			wIndex,
    OUT Byte*			ucpSymCnt,
    OUT Byte*			ucpReal,
    OUT Byte*			ucpImag,
    OUT Word*			wpRealH,
    OUT Word*			wpImagH
);


/**
 * Get status.
 *
 * @param demodulator the handle of demodulator.
 * @param dwpStatus Pointer to system information.
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_getStatus (
	IN  Demodulator*	demodulator,
    OUT Dword*			dwpStatus
);


/**
 * Get frequency shift.
 *
 * @param demodulator the handle of demodulator.
 * @param index .
 * @param dopShift .
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_getDouble (
	IN  Demodulator*	demodulator,
    IN  Byte			index,
    IN  double*			dopValue
);


/**
 * Get IR byte.
 *
 * @param demodulator the handle of demodulator.
 * @param ucpIRByte IR packet buffer.
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_getIr (
	IN  Demodulator*	demodulator,
    OUT	Byte*			ucpIRByte
);


/**
 * Dump EEPROM.
 *
 * @param demodulator the handle of demodulator.
 * @param dwDelay .
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_dumpEeprom (
	IN  Demodulator*	demodulator,
    IN  Dword			dwDelay
);


/**
 * Load file to EEPROM.
 *
 * @param fileName File name to load to EEPROM.
 * @param dwDelay .
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_loadEeprom (
	IN  Demodulator*	demodulator,
    IN  char*			fileName,
    IN  Dword			dwDelay
);


/**
 *
 * @param demodulator the handle of demodulator.
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_isFecMonEnabled (
	IN  Demodulator*	demodulator,
	OUT Bool*			enabled
);


/**
 * Generate ce information.
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are 
 *        0~7.
 * @param command .
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_genCeInfoFunc (
	IN  Demodulator*	demodulator,
	IN  Byte			chip,
	IN  Byte			command
);


/**
 * Get ce information.
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are 
 *        0~7.
 * @param spCentroid .
 * @param spBias .
 * @param dwpRh0 .
 * @param wpM2 .
 * @param dwpEh2 .
 * @param ucpM2q .
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_getCeInfo (
	IN  Demodulator*	demodulator,
	IN  Byte			chip,
	OUT Short*			spCentroid,
	OUT Short*			spBias,
	OUT Dword*			dwpRh0,
	OUT Word*			wpM2,
	OUT Dword*			dwpEh2,
	OUT Byte*			ucpM2q
);


/**
 * Enable/disable retrain.
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are 
 *        0~7.
 * @param enable .
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_setRetrain (
	IN  Demodulator*	demodulator,
	IN  Byte			chip,
	IN  Byte			enable
);


/**
 * Enable/disable CCIR.
 *
 * @param demodulator the handle of demodulator.
 * @param ucEnable .
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_setCcir (
	IN  Demodulator*	demodulator,
	IN  Byte			ucEnable
);


/**
 * Handle CCIF
 *
 * @param demodulator the handle of demodulator.
 * @param ccifId .
 * @param ctrl .
 * @param ucBw .
 * @param wFreq .
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_handleCcif (
	IN  Demodulator*	demodulator,
	IN  Byte			ccifId,
	IN  Byte			ctrl,
	IN  Byte			ucBw,
	IN  Word			wFreq
);


/**
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are 
 *        0~7.
 * @param level .
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_getAdcDesiredLevel (
	IN  Demodulator*	demodulator,
	IN  Byte			chip,
	OUT Word*			level
);

	
/**
 * Set tuner type
 *
 * @param demodulator the handle of demodulator.
 * @param ucTuner .
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_setTunerType (
	IN  Demodulator*	demodulator,
	IN  Byte			ucTuner
);


/**
 * Set board id
 *
 * @param demodulator the handle of demodulator.
 * @param ucBoard .
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_setBoardId (
	IN  Demodulator*	demodulator,
	IN  Byte			ucBoard
);


/**
 * Get signal strength in Dbm
 *
 * @param demodulator the handle of demodulator.
 * @param ucTunerType tuner type.
 * @param ucBoardId Board ids.
 * @param dopStrength signal strength.
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_getSignalStrengthDBm (
	IN  Demodulator*	demodulator,
	IN  Byte			ucTunerType,
    IN  Byte			ucBoardId,
    OUT double*			dopStrength		
);


/**
 * Program CFOE 2.
 *
 * @param demodulator the handle of demodulator.
 * @param ucBw Current channel bandwidth in MHz.
 * @param dFs ADC sampling frequency.
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_programCFOE2 (
	IN  Demodulator*	demodulator,
    IN  Byte			ucBw,
    IN  double			dFs
);


/**
 *
 * @param demodulator the handle of demodulator.
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_setCalibratAgc (
	IN  Demodulator*	demodulator
);


/**
 *
 * @param demodulator the handle of demodulator.
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_setCrystalFrequency (
	IN  Demodulator*	demodulator,
	IN  double			crystalFrequency
);


/**
 *
 * @param demodulator the handle of demodulator.
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_writeMt2060If1 (
	IN  Demodulator*	demodulator,
	IN  Dword			dwIF1
);


/**
 *
 * @param demodulator the handle of demodulator.
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_getSnr (
	IN  Demodulator*	demodulator,
	IN  Byte			chip,
	OUT double*			snr
);


/**
 *
 * @param demodulator the handle of demodulator.
 * @param speed the I2C speed in KHz.
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_setI2cSpeed (
	IN  Demodulator*	demodulator,
	IN  Dword			speed
);


/**
 * Ask fw to go back to boot code
 *
 * @param demodulator the handle of demodulator.
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_backToBootCode (
	IN  Demodulator*	demodulator
);


/**
 * Control gpio3 (AF9015 use this pin to turn on/off tuner)
 * ucOn = 1 => turn on tuner
 * ucOn = 0 => turn off tuner
 *
 * @param demodulator the handle of demodulator.
 * @param contorl True: Enable, False: Disable;
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_controlTunerPower (
	IN  Demodulator*	demodulator,
	IN  Byte			control
);


/**
 *
 * @param demodulator the handle of demodulator.
 * @param multiplier ADC frequency multiplier;
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_setMultiplier (
	IN  Demodulator*	demodulator,
	IN  Multiplier		multiplier
);


/**
 *
 * @param demodulator the handle of demodulator.
 * @param multiplier ADC frequency multiplier;
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_getMultiplier (
	IN  Demodulator*	demodulator,
	IN  Multiplier*		multiplier
);


/**
 *
 * @param demodulator the handle of demodulator.
 * @param ifFrequency the IF frequency of tuner;
 * @param inversion True if tuner's pectrum is inversed;
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_modifyTunerDescription (
	IN  Demodulator*	demodulator,
	IN  Byte			tunerAddress,
	IN  Byte			registerAddressLength,
	IN  Dword			ifFrequency,
	IN  Bool			inversion
);


/**
 *
 * @param demodulator the handle of demodulator.
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_writeRawData (
	IN  Demodulator*	demodulator,
	IN  Byte			writeBufferLength,
	IN  Byte*			writeBuffer
);


/**
 *
 * @param demodulator the handle of demodulator.
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_readRawData (
	IN  Demodulator*	demodulator,
	IN  Byte			readBufferLength,
	OUT Byte*			readBuffer
);


/**
 * Open tuner.
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are 
 *        0~7.
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 */
Dword Demodulator_openTuner (
	IN  Demodulator*	demodulator,
	IN  Byte			chip
);

/**
 * Set tuner.
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are 
 *        0~7.
 * @param bandwidth The desired bandwidth.
 * @param frequency The desired frequency.
 * @return Error_NO_ERROR: successful, other non-zero error code otherwise.
 */
Dword Demodulator_setTuner (
	IN  Demodulator*	demodulator,
	IN  Byte			chip,
	IN	Word			bandwidth,
	IN  Dword			frequency
);


/**
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are 
 *        0~7.
 * @param block How many block (logical frame) to be check.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_setPostVitAllZeroBlock (
	IN  Demodulator*	demodulator,
	IN  Byte			chip,
	IN  Word			block
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
Dword Demodulator_addPid (
	IN  Demodulator*	demodulator,
	IN  Byte			chip,
	IN  Pid				pid
);


/**
 * Add PID to PID filter by index.
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are 
 *        0~7.
 * @param index the index of PID filter.
 * @param pid the PID that will be add to PID filter.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Demodulator_addPidAt (
	IN  Demodulator*	demodulator,
	IN  Byte			chip,
    IN  Byte            index,
	IN  Pid				pid
);


/**
 * Remove PID from PID filter.
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are 
 *        0~7.
 * @param pid the PID that will be remove from PID filter.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Demodulator_removePid (
	IN  Demodulator*	demodulator,
	IN  Byte			chip,
	IN  Pid				pid
);

/**
 * Remove PID from PID filter by index.
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are 
 *        0~7.
 * @param index the index of PID filter.
 * @param pid the PID that will be remove from PID filter.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Demodulator_removePidAt (
    IN  Demodulator*    demodulator,
    IN  Byte            chip,
	IN  Byte			index,
    IN  Pid             pid
);


/**
 * Reset PID filter.
 *
 * @param demodulator the handle of demodulator.
 * @param chip The index of demodulator. The possible values are 
 *        0~7.
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 */
Dword Demodulator_resetPid (
	IN  Demodulator*	demodulator,
	IN  Byte			chip
);


/**
 * Control Active Sync.
 *
 * @param demodulator the handle of demodulator.
 * @param contorl 0: Disable(BDA Extend), 1: Enable (Active Sync)
 * @return Error_NO_ERROR: successful, non-zero error code otherwise.
 * @example <pre>
 * </pre>
 */
Dword Demodulator_controlActiveSync (
	IN  Demodulator*	demodulator,
	IN  Byte			control
);
#endif
