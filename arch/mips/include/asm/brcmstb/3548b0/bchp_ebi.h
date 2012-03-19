/***************************************************************************
 *     Copyright (c) 1999-2009, Broadcom Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Wed Aug 26 14:31:15 2009
 *                 MD5 Checksum         93f086426dd96fdf50d33828d68249c0
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: W:/pntruong/views/pntruong_93548_devel_sview/magnum/basemodules/chp/3548/rdb/b0/bchp_ebi.h $
 * 
 * Hydra_Software_Devel/5   8/26/09 5:16p pntruong
 * SW3548-2363: Synced up with central RDB.
 *
 ***************************************************************************/

#ifndef BCHP_EBI_H__
#define BCHP_EBI_H__

/***************************************************************************
 *EBI - EBI Registers
 ***************************************************************************/
#define BCHP_EBI_CS_BASE_0                       0x00000800 /* NAND_CS0b Base Register */
#define BCHP_EBI_CS_CONFIG_0                     0x00000804 /* EBI CS Config 0 Register */
#define BCHP_EBI_CS_BASE_1                       0x00000808 /* EBI CS Base 1 Register */
#define BCHP_EBI_CS_CONFIG_1                     0x0000080c /* EBI CS Config 1 Register */
#define BCHP_EBI_ECR                             0x00000900 /* EBI Configuration Register */
#define BCHP_EBI_CS_TRISTATE_CFG                 0x00000918 /* EBI CS Tristate Configuration Register */
#define BCHP_EBI_TA_CFG                          0x0000091c /* EBI TA Configuration Register */
#define BCHP_EBI_ARRAY_ADDRESS                   0x000009f0 /* EBI Data Array Address */

/***************************************************************************
 *CS_BASE_0 - NAND_CS0b Base Register
 ***************************************************************************/
/* EBI :: CS_BASE_0 :: base_addr [31:13] */
#define BCHP_EBI_CS_BASE_0_base_addr_MASK                          0xffffe000
#define BCHP_EBI_CS_BASE_0_base_addr_SHIFT                         13

/* EBI :: CS_BASE_0 :: reserved0 [12:04] */
#define BCHP_EBI_CS_BASE_0_reserved0_MASK                          0x00001ff0
#define BCHP_EBI_CS_BASE_0_reserved0_SHIFT                         4

/* EBI :: CS_BASE_0 :: size [03:00] */
#define BCHP_EBI_CS_BASE_0_size_MASK                               0x0000000f
#define BCHP_EBI_CS_BASE_0_size_SHIFT                              0
#define BCHP_EBI_CS_BASE_0_size_SIZE_8KB                           0
#define BCHP_EBI_CS_BASE_0_size_SIZE_16KB                          1
#define BCHP_EBI_CS_BASE_0_size_SIZE_32KB                          2
#define BCHP_EBI_CS_BASE_0_size_SIZE_64KB                          3
#define BCHP_EBI_CS_BASE_0_size_SIZE_128KB                         4
#define BCHP_EBI_CS_BASE_0_size_SIZE_256KB                         5
#define BCHP_EBI_CS_BASE_0_size_SIZE_512KB                         6
#define BCHP_EBI_CS_BASE_0_size_SIZE_1MB                           7
#define BCHP_EBI_CS_BASE_0_size_SIZE_2MB                           8
#define BCHP_EBI_CS_BASE_0_size_SIZE_4MB                           9
#define BCHP_EBI_CS_BASE_0_size_SIZE_8MB                           10
#define BCHP_EBI_CS_BASE_0_size_SIZE_16MB                          11
#define BCHP_EBI_CS_BASE_0_size_SIZE_32MB                          12
#define BCHP_EBI_CS_BASE_0_size_SIZE_64MB                          13
#define BCHP_EBI_CS_BASE_0_size_SIZE_128MB                         14
#define BCHP_EBI_CS_BASE_0_size_SIZE_256MB                         15

/***************************************************************************
 *CS_CONFIG_0 - EBI CS Config 0 Register
 ***************************************************************************/
/* EBI :: CS_CONFIG_0 :: mem_io [31:31] */
#define BCHP_EBI_CS_CONFIG_0_mem_io_MASK                           0x80000000
#define BCHP_EBI_CS_CONFIG_0_mem_io_SHIFT                          31

/* EBI :: CS_CONFIG_0 :: ta_wait [30:30] */
#define BCHP_EBI_CS_CONFIG_0_ta_wait_MASK                          0x40000000
#define BCHP_EBI_CS_CONFIG_0_ta_wait_SHIFT                         30

/* EBI :: CS_CONFIG_0 :: wp [29:29] */
#define BCHP_EBI_CS_CONFIG_0_wp_MASK                               0x20000000
#define BCHP_EBI_CS_CONFIG_0_wp_SHIFT                              29

/* EBI :: CS_CONFIG_0 :: wait_count [28:24] */
#define BCHP_EBI_CS_CONFIG_0_wait_count_MASK                       0x1f000000
#define BCHP_EBI_CS_CONFIG_0_wait_count_SHIFT                      24

/* EBI :: CS_CONFIG_0 :: t_hold [23:20] */
#define BCHP_EBI_CS_CONFIG_0_t_hold_MASK                           0x00f00000
#define BCHP_EBI_CS_CONFIG_0_t_hold_SHIFT                          20

/* EBI :: CS_CONFIG_0 :: t_setup [19:16] */
#define BCHP_EBI_CS_CONFIG_0_t_setup_MASK                          0x000f0000
#define BCHP_EBI_CS_CONFIG_0_t_setup_SHIFT                         16

/* EBI :: CS_CONFIG_0 :: cs_hold [15:15] */
#define BCHP_EBI_CS_CONFIG_0_cs_hold_MASK                          0x00008000
#define BCHP_EBI_CS_CONFIG_0_cs_hold_SHIFT                         15

/* EBI :: CS_CONFIG_0 :: split_cs [14:14] */
#define BCHP_EBI_CS_CONFIG_0_split_cs_MASK                         0x00004000
#define BCHP_EBI_CS_CONFIG_0_split_cs_SHIFT                        14

/* EBI :: CS_CONFIG_0 :: mask_en [13:13] */
#define BCHP_EBI_CS_CONFIG_0_mask_en_MASK                          0x00002000
#define BCHP_EBI_CS_CONFIG_0_mask_en_SHIFT                         13

/* EBI :: CS_CONFIG_0 :: ne_sample [12:12] */
#define BCHP_EBI_CS_CONFIG_0_ne_sample_MASK                        0x00001000
#define BCHP_EBI_CS_CONFIG_0_ne_sample_SHIFT                       12

/* EBI :: CS_CONFIG_0 :: m68k [11:11] */
#define BCHP_EBI_CS_CONFIG_0_m68k_MASK                             0x00000800
#define BCHP_EBI_CS_CONFIG_0_m68k_SHIFT                            11

/* EBI :: CS_CONFIG_0 :: le [10:10] */
#define BCHP_EBI_CS_CONFIG_0_le_MASK                               0x00000400
#define BCHP_EBI_CS_CONFIG_0_le_SHIFT                              10

/* EBI :: CS_CONFIG_0 :: fast_read [09:09] */
#define BCHP_EBI_CS_CONFIG_0_fast_read_MASK                        0x00000200
#define BCHP_EBI_CS_CONFIG_0_fast_read_SHIFT                       9
#define BCHP_EBI_CS_CONFIG_0_fast_read_Normal                      0
#define BCHP_EBI_CS_CONFIG_0_fast_read_Fast_Read_Enable            1

/* EBI :: CS_CONFIG_0 :: size_sel [08:08] */
#define BCHP_EBI_CS_CONFIG_0_size_sel_MASK                         0x00000100
#define BCHP_EBI_CS_CONFIG_0_size_sel_SHIFT                        8

/* EBI :: CS_CONFIG_0 :: sync [07:07] */
#define BCHP_EBI_CS_CONFIG_0_sync_MASK                             0x00000080
#define BCHP_EBI_CS_CONFIG_0_sync_SHIFT                            7

/* EBI :: CS_CONFIG_0 :: polarity [06:06] */
#define BCHP_EBI_CS_CONFIG_0_polarity_MASK                         0x00000040
#define BCHP_EBI_CS_CONFIG_0_polarity_SHIFT                        6

/* EBI :: CS_CONFIG_0 :: we_ctl [05:05] */
#define BCHP_EBI_CS_CONFIG_0_we_ctl_MASK                           0x00000020
#define BCHP_EBI_CS_CONFIG_0_we_ctl_SHIFT                          5

/* EBI :: CS_CONFIG_0 :: dest_size [04:04] */
#define BCHP_EBI_CS_CONFIG_0_dest_size_MASK                        0x00000010
#define BCHP_EBI_CS_CONFIG_0_dest_size_SHIFT                       4

/* EBI :: CS_CONFIG_0 :: ms_inh [03:03] */
#define BCHP_EBI_CS_CONFIG_0_ms_inh_MASK                           0x00000008
#define BCHP_EBI_CS_CONFIG_0_ms_inh_SHIFT                          3

/* EBI :: CS_CONFIG_0 :: ls_inh [02:02] */
#define BCHP_EBI_CS_CONFIG_0_ls_inh_MASK                           0x00000004
#define BCHP_EBI_CS_CONFIG_0_ls_inh_SHIFT                          2

/* EBI :: CS_CONFIG_0 :: bcachen [01:01] */
#define BCHP_EBI_CS_CONFIG_0_bcachen_MASK                          0x00000002
#define BCHP_EBI_CS_CONFIG_0_bcachen_SHIFT                         1

/* EBI :: CS_CONFIG_0 :: enable [00:00] */
#define BCHP_EBI_CS_CONFIG_0_enable_MASK                           0x00000001
#define BCHP_EBI_CS_CONFIG_0_enable_SHIFT                          0

/***************************************************************************
 *CS_BASE_1 - EBI CS Base 1 Register
 ***************************************************************************/
/* EBI :: CS_BASE_1 :: base_addr [31:13] */
#define BCHP_EBI_CS_BASE_1_base_addr_MASK                          0xffffe000
#define BCHP_EBI_CS_BASE_1_base_addr_SHIFT                         13

/* EBI :: CS_BASE_1 :: reserved0 [12:04] */
#define BCHP_EBI_CS_BASE_1_reserved0_MASK                          0x00001ff0
#define BCHP_EBI_CS_BASE_1_reserved0_SHIFT                         4

/* EBI :: CS_BASE_1 :: size [03:00] */
#define BCHP_EBI_CS_BASE_1_size_MASK                               0x0000000f
#define BCHP_EBI_CS_BASE_1_size_SHIFT                              0
#define BCHP_EBI_CS_BASE_1_size_SIZE_8KB                           0
#define BCHP_EBI_CS_BASE_1_size_SIZE_16KB                          1
#define BCHP_EBI_CS_BASE_1_size_SIZE_32KB                          2
#define BCHP_EBI_CS_BASE_1_size_SIZE_64KB                          3
#define BCHP_EBI_CS_BASE_1_size_SIZE_128KB                         4
#define BCHP_EBI_CS_BASE_1_size_SIZE_256KB                         5
#define BCHP_EBI_CS_BASE_1_size_SIZE_512KB                         6
#define BCHP_EBI_CS_BASE_1_size_SIZE_1MB                           7
#define BCHP_EBI_CS_BASE_1_size_SIZE_2MB                           8
#define BCHP_EBI_CS_BASE_1_size_SIZE_4MB                           9
#define BCHP_EBI_CS_BASE_1_size_SIZE_8MB                           10
#define BCHP_EBI_CS_BASE_1_size_SIZE_16MB                          11
#define BCHP_EBI_CS_BASE_1_size_SIZE_32MB                          12
#define BCHP_EBI_CS_BASE_1_size_SIZE_64MB                          13
#define BCHP_EBI_CS_BASE_1_size_SIZE_128MB                         14
#define BCHP_EBI_CS_BASE_1_size_SIZE_256MB                         15

/***************************************************************************
 *CS_CONFIG_1 - EBI CS Config 1 Register
 ***************************************************************************/
/* EBI :: CS_CONFIG_1 :: mem_io [31:31] */
#define BCHP_EBI_CS_CONFIG_1_mem_io_MASK                           0x80000000
#define BCHP_EBI_CS_CONFIG_1_mem_io_SHIFT                          31

/* EBI :: CS_CONFIG_1 :: ta_wait [30:30] */
#define BCHP_EBI_CS_CONFIG_1_ta_wait_MASK                          0x40000000
#define BCHP_EBI_CS_CONFIG_1_ta_wait_SHIFT                         30

/* EBI :: CS_CONFIG_1 :: wp [29:29] */
#define BCHP_EBI_CS_CONFIG_1_wp_MASK                               0x20000000
#define BCHP_EBI_CS_CONFIG_1_wp_SHIFT                              29

/* EBI :: CS_CONFIG_1 :: wait_count [28:24] */
#define BCHP_EBI_CS_CONFIG_1_wait_count_MASK                       0x1f000000
#define BCHP_EBI_CS_CONFIG_1_wait_count_SHIFT                      24

/* EBI :: CS_CONFIG_1 :: t_hold [23:20] */
#define BCHP_EBI_CS_CONFIG_1_t_hold_MASK                           0x00f00000
#define BCHP_EBI_CS_CONFIG_1_t_hold_SHIFT                          20

/* EBI :: CS_CONFIG_1 :: t_setup [19:16] */
#define BCHP_EBI_CS_CONFIG_1_t_setup_MASK                          0x000f0000
#define BCHP_EBI_CS_CONFIG_1_t_setup_SHIFT                         16

/* EBI :: CS_CONFIG_1 :: cs_hold [15:15] */
#define BCHP_EBI_CS_CONFIG_1_cs_hold_MASK                          0x00008000
#define BCHP_EBI_CS_CONFIG_1_cs_hold_SHIFT                         15

/* EBI :: CS_CONFIG_1 :: split_cs [14:14] */
#define BCHP_EBI_CS_CONFIG_1_split_cs_MASK                         0x00004000
#define BCHP_EBI_CS_CONFIG_1_split_cs_SHIFT                        14

/* EBI :: CS_CONFIG_1 :: mask_en [13:13] */
#define BCHP_EBI_CS_CONFIG_1_mask_en_MASK                          0x00002000
#define BCHP_EBI_CS_CONFIG_1_mask_en_SHIFT                         13

/* EBI :: CS_CONFIG_1 :: ne_sample [12:12] */
#define BCHP_EBI_CS_CONFIG_1_ne_sample_MASK                        0x00001000
#define BCHP_EBI_CS_CONFIG_1_ne_sample_SHIFT                       12

/* EBI :: CS_CONFIG_1 :: m68k [11:11] */
#define BCHP_EBI_CS_CONFIG_1_m68k_MASK                             0x00000800
#define BCHP_EBI_CS_CONFIG_1_m68k_SHIFT                            11

/* EBI :: CS_CONFIG_1 :: le [10:10] */
#define BCHP_EBI_CS_CONFIG_1_le_MASK                               0x00000400
#define BCHP_EBI_CS_CONFIG_1_le_SHIFT                              10

/* EBI :: CS_CONFIG_1 :: fast_read [09:09] */
#define BCHP_EBI_CS_CONFIG_1_fast_read_MASK                        0x00000200
#define BCHP_EBI_CS_CONFIG_1_fast_read_SHIFT                       9
#define BCHP_EBI_CS_CONFIG_1_fast_read_Normal                      0
#define BCHP_EBI_CS_CONFIG_1_fast_read_Fast_Read_Enable            1

/* EBI :: CS_CONFIG_1 :: size_sel [08:08] */
#define BCHP_EBI_CS_CONFIG_1_size_sel_MASK                         0x00000100
#define BCHP_EBI_CS_CONFIG_1_size_sel_SHIFT                        8

/* EBI :: CS_CONFIG_1 :: sync [07:07] */
#define BCHP_EBI_CS_CONFIG_1_sync_MASK                             0x00000080
#define BCHP_EBI_CS_CONFIG_1_sync_SHIFT                            7

/* EBI :: CS_CONFIG_1 :: polarity [06:06] */
#define BCHP_EBI_CS_CONFIG_1_polarity_MASK                         0x00000040
#define BCHP_EBI_CS_CONFIG_1_polarity_SHIFT                        6

/* EBI :: CS_CONFIG_1 :: we_ctl [05:05] */
#define BCHP_EBI_CS_CONFIG_1_we_ctl_MASK                           0x00000020
#define BCHP_EBI_CS_CONFIG_1_we_ctl_SHIFT                          5

/* EBI :: CS_CONFIG_1 :: dest_size [04:04] */
#define BCHP_EBI_CS_CONFIG_1_dest_size_MASK                        0x00000010
#define BCHP_EBI_CS_CONFIG_1_dest_size_SHIFT                       4

/* EBI :: CS_CONFIG_1 :: ms_inh [03:03] */
#define BCHP_EBI_CS_CONFIG_1_ms_inh_MASK                           0x00000008
#define BCHP_EBI_CS_CONFIG_1_ms_inh_SHIFT                          3

/* EBI :: CS_CONFIG_1 :: ls_inh [02:02] */
#define BCHP_EBI_CS_CONFIG_1_ls_inh_MASK                           0x00000004
#define BCHP_EBI_CS_CONFIG_1_ls_inh_SHIFT                          2

/* EBI :: CS_CONFIG_1 :: bcachen [01:01] */
#define BCHP_EBI_CS_CONFIG_1_bcachen_MASK                          0x00000002
#define BCHP_EBI_CS_CONFIG_1_bcachen_SHIFT                         1

/* EBI :: CS_CONFIG_1 :: enable [00:00] */
#define BCHP_EBI_CS_CONFIG_1_enable_MASK                           0x00000001
#define BCHP_EBI_CS_CONFIG_1_enable_SHIFT                          0

/***************************************************************************
 *ECR - EBI Configuration Register
 ***************************************************************************/
/* EBI :: ECR :: reserved0 [31:28] */
#define BCHP_EBI_ECR_reserved0_MASK                                0xf0000000
#define BCHP_EBI_ECR_reserved0_SHIFT                               28

/* EBI :: ECR :: Ebi_Byte_Swap [27:27] */
#define BCHP_EBI_ECR_Ebi_Byte_Swap_MASK                            0x08000000
#define BCHP_EBI_ECR_Ebi_Byte_Swap_SHIFT                           27
#define BCHP_EBI_ECR_Ebi_Byte_Swap_Byte_Swap_For_32_16_and_8_bit_Xfers 0
#define BCHP_EBI_ECR_Ebi_Byte_Swap_Byte_Swap_For_8_bit_Xfers_Only  1

/* EBI :: ECR :: reserved1 [26:11] */
#define BCHP_EBI_ECR_reserved1_MASK                                0x07fff800
#define BCHP_EBI_ECR_reserved1_SHIFT                               11

/* EBI :: ECR :: timeout_count [10:00] */
#define BCHP_EBI_ECR_timeout_count_MASK                            0x000007ff
#define BCHP_EBI_ECR_timeout_count_SHIFT                           0

/***************************************************************************
 *CS_TRISTATE_CFG - EBI CS Tristate Configuration Register
 ***************************************************************************/
/* EBI :: CS_TRISTATE_CFG :: tristate_ebi_rdb [31:31] */
#define BCHP_EBI_CS_TRISTATE_CFG_tristate_ebi_rdb_MASK             0x80000000
#define BCHP_EBI_CS_TRISTATE_CFG_tristate_ebi_rdb_SHIFT            31

/* EBI :: CS_TRISTATE_CFG :: tristate_ebi_dsb [30:30] */
#define BCHP_EBI_CS_TRISTATE_CFG_tristate_ebi_dsb_MASK             0x40000000
#define BCHP_EBI_CS_TRISTATE_CFG_tristate_ebi_dsb_SHIFT            30

/* EBI :: CS_TRISTATE_CFG :: tristate_ebi_tsb [29:29] */
#define BCHP_EBI_CS_TRISTATE_CFG_tristate_ebi_tsb_MASK             0x20000000
#define BCHP_EBI_CS_TRISTATE_CFG_tristate_ebi_tsb_SHIFT            29

/* EBI :: CS_TRISTATE_CFG :: tristate_ebi_rwb [28:28] */
#define BCHP_EBI_CS_TRISTATE_CFG_tristate_ebi_rwb_MASK             0x10000000
#define BCHP_EBI_CS_TRISTATE_CFG_tristate_ebi_rwb_SHIFT            28

/* EBI :: CS_TRISTATE_CFG :: tristate_ebi_we1b [27:27] */
#define BCHP_EBI_CS_TRISTATE_CFG_tristate_ebi_we1b_MASK            0x08000000
#define BCHP_EBI_CS_TRISTATE_CFG_tristate_ebi_we1b_SHIFT           27

/* EBI :: CS_TRISTATE_CFG :: tristate_ebi_we0b [26:26] */
#define BCHP_EBI_CS_TRISTATE_CFG_tristate_ebi_we0b_MASK            0x04000000
#define BCHP_EBI_CS_TRISTATE_CFG_tristate_ebi_we0b_SHIFT           26

/* EBI :: CS_TRISTATE_CFG :: reserved0 [25:24] */
#define BCHP_EBI_CS_TRISTATE_CFG_reserved0_MASK                    0x03000000
#define BCHP_EBI_CS_TRISTATE_CFG_reserved0_SHIFT                   24

/* EBI :: CS_TRISTATE_CFG :: tristate_ebi_addr [23:23] */
#define BCHP_EBI_CS_TRISTATE_CFG_tristate_ebi_addr_MASK            0x00800000
#define BCHP_EBI_CS_TRISTATE_CFG_tristate_ebi_addr_SHIFT           23

/* EBI :: CS_TRISTATE_CFG :: reserved1 [22:02] */
#define BCHP_EBI_CS_TRISTATE_CFG_reserved1_MASK                    0x007ffffc
#define BCHP_EBI_CS_TRISTATE_CFG_reserved1_SHIFT                   2

/* EBI :: CS_TRISTATE_CFG :: tristate_ebi_cs1 [01:01] */
#define BCHP_EBI_CS_TRISTATE_CFG_tristate_ebi_cs1_MASK             0x00000002
#define BCHP_EBI_CS_TRISTATE_CFG_tristate_ebi_cs1_SHIFT            1

/* EBI :: CS_TRISTATE_CFG :: tristate_ebi_cs0 [00:00] */
#define BCHP_EBI_CS_TRISTATE_CFG_tristate_ebi_cs0_MASK             0x00000001
#define BCHP_EBI_CS_TRISTATE_CFG_tristate_ebi_cs0_SHIFT            0

/***************************************************************************
 *TA_CFG - EBI TA Configuration Register
 ***************************************************************************/
/* EBI :: TA_CFG :: reserved0 [31:02] */
#define BCHP_EBI_TA_CFG_reserved0_MASK                             0xfffffffc
#define BCHP_EBI_TA_CFG_reserved0_SHIFT                            2

/* EBI :: TA_CFG :: use_ta1 [01:01] */
#define BCHP_EBI_TA_CFG_use_ta1_MASK                               0x00000002
#define BCHP_EBI_TA_CFG_use_ta1_SHIFT                              1

/* EBI :: TA_CFG :: reserved1 [00:00] */
#define BCHP_EBI_TA_CFG_reserved1_MASK                             0x00000001
#define BCHP_EBI_TA_CFG_reserved1_SHIFT                            0

/***************************************************************************
 *ARRAY_ADDRESS - EBI Data Array Address
 ***************************************************************************/
/* EBI :: ARRAY_ADDRESS :: ADDRESS [31:09] */
#define BCHP_EBI_ARRAY_ADDRESS_ADDRESS_MASK                        0xfffffe00
#define BCHP_EBI_ARRAY_ADDRESS_ADDRESS_SHIFT                       9

/* EBI :: ARRAY_ADDRESS :: reserved0 [08:03] */
#define BCHP_EBI_ARRAY_ADDRESS_reserved0_MASK                      0x000001f8
#define BCHP_EBI_ARRAY_ADDRESS_reserved0_SHIFT                     3

/* EBI :: ARRAY_ADDRESS :: CS_SEL [02:00] */
#define BCHP_EBI_ARRAY_ADDRESS_CS_SEL_MASK                         0x00000007
#define BCHP_EBI_ARRAY_ADDRESS_CS_SEL_SHIFT                        0

/***************************************************************************
 *DATA_ARRAY%i - EBI Data Array Read/Write Access
 ***************************************************************************/
#define BCHP_EBI_DATA_ARRAYi_ARRAY_BASE                            0x00000a00
#define BCHP_EBI_DATA_ARRAYi_ARRAY_START                           0
#define BCHP_EBI_DATA_ARRAYi_ARRAY_END                             127
#define BCHP_EBI_DATA_ARRAYi_ARRAY_ELEMENT_SIZE                    32

/***************************************************************************
 *DATA_ARRAY%i - EBI Data Array Read/Write Access
 ***************************************************************************/
/* EBI :: DATA_ARRAYi :: WORD [31:00] */
#define BCHP_EBI_DATA_ARRAYi_WORD_MASK                             0xffffffff
#define BCHP_EBI_DATA_ARRAYi_WORD_SHIFT                            0


#endif /* #ifndef BCHP_EBI_H__ */

/* End of File */