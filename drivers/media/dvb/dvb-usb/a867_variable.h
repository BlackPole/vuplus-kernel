#ifndef __VARIABLE_H__
#define __VARIABLE_H__
//this file define variable which initialized by AP
//CFOE------------------------------------------

//These variables are initialized by API.
//Don't change the order of the definition of these variables.


//2k
//BASE Address 0x418B
#define var_addr_base                       0x418b
#define log_addr_base                       0x418d
#define log_data_base                       0x418f
// Do NOT touch the following line: used by script
// --- @xx++ Start variable block
//Initialization..
//API relative
//BASE Address 0x0000
#define trigger_ofsm                        0x0000
#define cfoe_NS_2048_coeff1_25_24           0x0001
#define cfoe_NS_2048_coeff1_23_16           0x0002
#define cfoe_NS_2048_coeff1_15_8            0x0003
#define cfoe_NS_2048_coeff1_7_0             0x0004
#define cfoe_NS_2k_coeff2_24                0x0005
#define cfoe_NS_2k_coeff2_23_16             0x0006
#define cfoe_NS_2k_coeff2_15_8              0x0007
#define cfoe_NS_2k_coeff2_7_0               0x0008

//8k..

#define cfoe_NS_8191_coeff1_25_24           0x0009
#define cfoe_NS_8191_coeff1_23_16           0x000a
#define cfoe_NS_8191_coeff1_15_8            0x000b
#define cfoe_NS_8191_coeff1_7_0             0x000c
#define cfoe_NS_8192_coeff1_25_24           0x000d
#define cfoe_NS_8192_coeff1_23_16           0x000e
#define cfoe_NS_8192_coeff1_15_8            0x000f
#define cfoe_NS_8192_coeff1_7_0             0x0010
#define cfoe_NS_8193_coeff1_25_24           0x0011
#define cfoe_NS_8193_coeff1_23_16           0x0012
#define cfoe_NS_8193_coeff1_15_8            0x0013
#define cfoe_NS_8193_coeff1_7_0             0x0014

#define cfoe_NS_8k_coeff2_24                0x0015
#define cfoe_NS_8k_coeff2_23_16             0x0016
#define cfoe_NS_8k_coeff2_15_8              0x0017
#define cfoe_NS_8k_coeff2_7_0               0x0018

//4k
#define cfoe_NS_4096_coeff1_25_24           0x0019
#define cfoe_NS_4096_coeff1_23_16           0x001a
#define cfoe_NS_4096_coeff1_15_8            0x001b
#define cfoe_NS_4096_coeff1_7_0             0x001c
#define cfoe_NS_4k_coeff2_24                0x001d
#define cfoe_NS_4k_coeff2_23_16             0x001e
#define cfoe_NS_4k_coeff2_15_8              0x001f
#define cfoe_NS_4k_coeff2_7_0               0x0020

#define bfsfcw_fftindex_ratio_7_0           0x0021
#define bfsfcw_fftindex_ratio_15_8          0x0022
#define fftindex_bfsfcw_ratio_7_0           0x0023
#define fftindex_bfsfcw_ratio_15_8          0x0024



#define crystal_clk_7_0                     0x0025
#define crystal_clk_15_8                    0x0026
#define crystal_clk_23_16                   0x0027
#define crystal_clk_31_24                   0x0028


#define bfs_fcw_7_0                         0x0029
#define bfs_fcw_15_8                        0x002a
#define bfs_fcw_22_16                       0x002b

//----------------------------------------------
//statistic performance..

#define qnt_vbc_err_7_0                     0x002c  //snr
#define qnt_vbc_err_15_8                    0x002d  //snr
#define qnt_vbc_err_23_16                   0x002e  //snr
#define r_qnt_vbc_sframe_num                0x002f
#define tpsd_const                          0x0030
#define tpsd_txmod                          0x0031
 
#define rsd_abort_packet_cnt_7_0            0x0032
#define rsd_abort_packet_cnt_15_8           0x0033
#define rsd_bit_err_cnt_7_0                 0x0034
#define rsd_bit_err_cnt_15_8                0x0035
#define rsd_bit_err_cnt_23_16               0x0036
#define r_rsd_packet_unit_7_0               0x0037
#define r_rsd_packet_unit_15_8              0x0038

#define qnt_vbc_sframe_num                  0x0039
#define rsd_packet_unit_7_0                 0x003a
#define rsd_packet_unit_15_8                0x003b

#define tpsd_lock                           0x003c
#define mpeg_lock                           0x003d
#define RsdSequence                         0x003e
#define VtbSequence                         0x003f

#define Training_Mode                       0x0040
#define RESET_STATE                         0x0041
#define unplug_flg                          0x0042
#define aci_0                               0x0043
#define aci_1                               0x0044

#define adcx2                               0x0045
#define tuner_ID                            0x0046
#define empty_channel_status                0x0047
#define signal_strength                     0x0048
#define signal_quality                      0x0049
#define est_rf_level_dbm                    0x004a
#define FreBand                             0x004b
#define suspend_flag                        0x004c
//GUI relative
//Initial OFSM
#define API_Reserved                        0x004d
#define var_ofsm_state                      0x004e
#define OfdmGuiRCN_H                        0x004f
#define OfdmGuiRCN_L                        0x0050
#define antenna_unplugged                   0x0051
#define strong_signal_detected              0x0052
#define channelFlatnessInd                  0x0053
#define Flatness_Ind_nonCmb                 0x0054
#define AutoDetectedSpectrumInv             0x0055
#define IsSpectrumInv                       0x0056
#define strong_detect_bypass                0x0057
#define ss_dtop_bypass                      0x0058
#define retrain_dtop_bypass                 0x0059
#define EnableTimeSlice                     0x005a
#define SynchronizationType                 0x005b
#define ApplyFastSynchronizationToEchoChannel 0x005c
#define ApplyPwmToRfIf                      0x005d
#define ChannelNo                           0x005e

//release to AAGC document..
#define csi_bypass                          0x005f
#define mobile_bypass                       0x0060
#define EnableSpeedLog                      0x0061

//regression used only..
#define r_rsd_abort_total_packet_7_0        0x0062
#define r_rsd_abort_total_packet_15_8       0x0063
#define r_rsd_abort_total_packet_23_16      0x0064
#define MaxRsdSequence                      0x0065
#define RsdFrameNo                          0x0066
#define MPESuperFrameNo                     0x0067

#define AgcDesiredLevel                     0x0068
#define MinRfGain                           0x0069
#define MaxIfGain                           0x006a
#define RssiOffset                          0x006b
#define RssiResidual                        0x006c
//Dtop

#define strong_weak_signal_default          0x006d
#define unplug_th                           0x006e
#define afe_mem4_rssi_comp                  0x006f

#define aagc_speed_detect_count             0x0070
#define aagc_mobile_thr                     0x0071
#define aagc_nonmobile_thr                  0x0072
#define agc_counter                         0x0073
#define DisableAagcTop                      0x0074
#define AgcReset                            0x0075
#define AgcUp                               0x0076
#define AgcDown                             0x0077
#define AgcHold                             0x0078
#define PwmCtrlHw                           0x0079
#define MaxAgcGain                          0x007a
#define IniAgcGain                          0x007b
#define mccid_bypass                        0x007c
#define CdpfEnDefaultEchoRange              0x007d
#define CdpfIniTestNo                       0x007e
#define timing_err_level                    0x007f
#define timing_retrain_cnt                  0x0080
#define ChannelDiffThr                      0x0081

#define adjacent_on                         0x0082
#define near_adjacent_on                    0x0083
#define adjacent_off                        0x0084
#define near_adjacent_off                   0x0085
#define max_rf_agc_7_0                      0x0086
#define max_rf_agc_9_8                      0x0087
#define rf_top_numerator_s_7_0              0x0088
#define rf_top_numerator_s_9_8              0x0089

#define gui_tdi_lms_en                      0x008a
#define fccid_strobe_scale                  0x008b
#define fccid_strobe_numerator              0x008c
#define fccid_strobe_base                   0x008d
#define use_fccid                           0x008e
#define fft_ave_symbol_num                  0x008f
#define large_tone_num_th_7_0               0x0090
#define large_tone_num_th_15_8              0x0091
#define use_3m_lpf_th                       0x0092
#define ce_var_min_8k                       0x0093
#define ce_var_min_4k                       0x0094
#define ce_var_min_2k                       0x0095
#define ce_var_min_8k_non_flat              0x0096
#define flatness_thr                        0x0097
#define non_mobile_signal_level_offset      0x0098
#define gui_ar_csi_en                       0x0099
#define h2_echo_detected                    0x009a
#define signal_strength_rf_high             0x009b
#define signal_strength_rf_low              0x009c
#define signal_strength_if_high             0x009d
#define signal_strength_if_low              0x009e
//flatness
#define flatness_thr_high                   0x009f
#define flatness_thr_low                    0x00a0

//softbit quality
#define sbq1                                0x00a1
#define sbq2                                0x00a2

//DCA
#define dyna_dca_offset_en                  0x00a3
#define dca_sbq_bad_th                      0x00a4
#define detect_timing_err_en                0x00a5
#define flatness_from_h2_echo               0x00a6

#define timging_error_detection             0x00a7
#define ce_forced_by_rotate                 0x00a8
#define fccid_fft_mask_en                   0x00a9
#define second_fctrl_unforce_en             0x00aa
#define force_fdi0_at_high_mobile_en        0x00ab
#define high_mobile_detected                0x00ac
#define flatness_detection_en               0x00ad
#define ChooseFsteCostFunctionFromCdpf      0x00ae
#define signal_level                        0x00af
#define TryConf2En                          0x00b0
#define Lower_tpsd_lock                     0x00b1
#define Upper_tpsd_lock                     0x00b2

#define AgcCtrlType                         0x00b3
#define opt_LNA_Rssi_scale                  0x00b4
#define StopByTcl                           0x00b5
#define RssiCalibration                     0x00b6
#define AciDesiredSignalLevel_h             0x00b7
#define AciDesiredSignalLevel_l             0x00b8
#define ECO_ASIC                            0x00b9
#define NXP_USE_I2C                         0x00ba
#define rf_freqency_23_16                   0x00bb
#define rf_freqency_15_8                    0x00bc
#define rf_freqency_7_0                     0x00bd
#define iqik_en                             0x00be
#define dcc_en                              0x00bf
#define VHFPinEnTh                          0x00c0
#define ACIdetection                        0x00c1
#define PinDiode                            0x00c2
#define LNA_Gain                            0x00c3
#define RSSI_LNA_ON                         0x00c4
#define var_end                             0x00c5

//BASE Address 0xFFFF
#endif
