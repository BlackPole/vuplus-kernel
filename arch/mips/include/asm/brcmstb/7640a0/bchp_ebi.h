/***************************************************************************
 *     Copyright (c) 1999-2010, Broadcom Corporation
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
 * Date:           Generated on         Mon May 17 04:47:06 2010
 *                 MD5 Checksum         2140f8c1f86e8a5296b6aebfc26dee55
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: $
 *
 ***************************************************************************/

#ifndef BCHP_EBI_H__
#define BCHP_EBI_H__

/***************************************************************************
 *EBI - EBI Registers
 ***************************************************************************/
#define BCHP_EBI_CS_BASE_0                       0x01400800 /* EBI CS Base 0 Register */
#define BCHP_EBI_CS_CONFIG_0                     0x01400804 /* EBI CS Config 0 Register */
#define BCHP_EBI_CS_BASE_1                       0x01400808 /* EBI CS Base 1 Register */
#define BCHP_EBI_CS_CONFIG_1                     0x0140080c /* EBI CS Config 1 Register */
#define BCHP_EBI_CS_BASE_2                       0x01400810 /* EBI CS Base 2 Register */
#define BCHP_EBI_CS_CONFIG_2                     0x01400814 /* EBI CS Config 2 Register */
#define BCHP_EBI_BURST_CFG_0                     0x01400840 /* EBI Synchronous Intel StrataFlash Burst Configure Register */
#define BCHP_EBI_BURST_CFG_1                     0x01400844 /* EBI Synchronous Intel StrataFlash Burst Configure Register */
#define BCHP_EBI_BURST_CFG_2                     0x01400848 /* EBI Synchronous Intel StrataFlash Burst Configure Register */
#define BCHP_EBI_ECR                             0x01400900 /* EBI Configuration Register */
#define BCHP_EBI_CS_TRISTATE_CFG                 0x01400918 /* EBI CS Tristate Configuration Register */
#define BCHP_EBI_ARRAY_ADDRESS                   0x014009f0 /* EBI Data Array Address */

/***************************************************************************
 *CS_BASE_0 - EBI CS Base 0 Register
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
/* EBI :: CS_CONFIG_0 :: reserved0 [31:30] */
#define BCHP_EBI_CS_CONFIG_0_reserved0_MASK                        0xc0000000
#define BCHP_EBI_CS_CONFIG_0_reserved0_SHIFT                       30

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

/* EBI :: CS_CONFIG_0 :: reserved1 [15:15] */
#define BCHP_EBI_CS_CONFIG_0_reserved1_MASK                        0x00008000
#define BCHP_EBI_CS_CONFIG_0_reserved1_SHIFT                       15

/* EBI :: CS_CONFIG_0 :: split_cs [14:14] */
#define BCHP_EBI_CS_CONFIG_0_split_cs_MASK                         0x00004000
#define BCHP_EBI_CS_CONFIG_0_split_cs_SHIFT                        14

/* EBI :: CS_CONFIG_0 :: mask_en [13:13] */
#define BCHP_EBI_CS_CONFIG_0_mask_en_MASK                          0x00002000
#define BCHP_EBI_CS_CONFIG_0_mask_en_SHIFT                         13

/* EBI :: CS_CONFIG_0 :: ne_sample [12:12] */
#define BCHP_EBI_CS_CONFIG_0_ne_sample_MASK                        0x00001000
#define BCHP_EBI_CS_CONFIG_0_ne_sample_SHIFT                       12

/* EBI :: CS_CONFIG_0 :: reserved2 [11:11] */
#define BCHP_EBI_CS_CONFIG_0_reserved2_MASK                        0x00000800
#define BCHP_EBI_CS_CONFIG_0_reserved2_SHIFT                       11

/* EBI :: CS_CONFIG_0 :: le [10:10] */
#define BCHP_EBI_CS_CONFIG_0_le_MASK                               0x00000400
#define BCHP_EBI_CS_CONFIG_0_le_SHIFT                              10

/* EBI :: CS_CONFIG_0 :: fast_read [09:09] */
#define BCHP_EBI_CS_CONFIG_0_fast_read_MASK                        0x00000200
#define BCHP_EBI_CS_CONFIG_0_fast_read_SHIFT                       9
#define BCHP_EBI_CS_CONFIG_0_fast_read_Normal                      0
#define BCHP_EBI_CS_CONFIG_0_fast_read_Fast_Read_Enable            1

/* EBI :: CS_CONFIG_0 :: reserved3 [08:06] */
#define BCHP_EBI_CS_CONFIG_0_reserved3_MASK                        0x000001c0
#define BCHP_EBI_CS_CONFIG_0_reserved3_SHIFT                       6

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
/* EBI :: CS_CONFIG_1 :: reserved0 [31:30] */
#define BCHP_EBI_CS_CONFIG_1_reserved0_MASK                        0xc0000000
#define BCHP_EBI_CS_CONFIG_1_reserved0_SHIFT                       30

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

/* EBI :: CS_CONFIG_1 :: reserved1 [15:15] */
#define BCHP_EBI_CS_CONFIG_1_reserved1_MASK                        0x00008000
#define BCHP_EBI_CS_CONFIG_1_reserved1_SHIFT                       15

/* EBI :: CS_CONFIG_1 :: split_cs [14:14] */
#define BCHP_EBI_CS_CONFIG_1_split_cs_MASK                         0x00004000
#define BCHP_EBI_CS_CONFIG_1_split_cs_SHIFT                        14

/* EBI :: CS_CONFIG_1 :: mask_en [13:13] */
#define BCHP_EBI_CS_CONFIG_1_mask_en_MASK                          0x00002000
#define BCHP_EBI_CS_CONFIG_1_mask_en_SHIFT                         13

/* EBI :: CS_CONFIG_1 :: ne_sample [12:12] */
#define BCHP_EBI_CS_CONFIG_1_ne_sample_MASK                        0x00001000
#define BCHP_EBI_CS_CONFIG_1_ne_sample_SHIFT                       12

/* EBI :: CS_CONFIG_1 :: reserved2 [11:11] */
#define BCHP_EBI_CS_CONFIG_1_reserved2_MASK                        0x00000800
#define BCHP_EBI_CS_CONFIG_1_reserved2_SHIFT                       11

/* EBI :: CS_CONFIG_1 :: le [10:10] */
#define BCHP_EBI_CS_CONFIG_1_le_MASK                               0x00000400
#define BCHP_EBI_CS_CONFIG_1_le_SHIFT                              10

/* EBI :: CS_CONFIG_1 :: fast_read [09:09] */
#define BCHP_EBI_CS_CONFIG_1_fast_read_MASK                        0x00000200
#define BCHP_EBI_CS_CONFIG_1_fast_read_SHIFT                       9
#define BCHP_EBI_CS_CONFIG_1_fast_read_Normal                      0
#define BCHP_EBI_CS_CONFIG_1_fast_read_Fast_Read_Enable            1

/* EBI :: CS_CONFIG_1 :: reserved3 [08:06] */
#define BCHP_EBI_CS_CONFIG_1_reserved3_MASK                        0x000001c0
#define BCHP_EBI_CS_CONFIG_1_reserved3_SHIFT                       6

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
 *CS_BASE_2 - EBI CS Base 2 Register
 ***************************************************************************/
/* EBI :: CS_BASE_2 :: base_addr [31:13] */
#define BCHP_EBI_CS_BASE_2_base_addr_MASK                          0xffffe000
#define BCHP_EBI_CS_BASE_2_base_addr_SHIFT                         13

/* EBI :: CS_BASE_2 :: reserved0 [12:04] */
#define BCHP_EBI_CS_BASE_2_reserved0_MASK                          0x00001ff0
#define BCHP_EBI_CS_BASE_2_reserved0_SHIFT                         4

/* EBI :: CS_BASE_2 :: size [03:00] */
#define BCHP_EBI_CS_BASE_2_size_MASK                               0x0000000f
#define BCHP_EBI_CS_BASE_2_size_SHIFT                              0
#define BCHP_EBI_CS_BASE_2_size_SIZE_8KB                           0
#define BCHP_EBI_CS_BASE_2_size_SIZE_16KB                          1
#define BCHP_EBI_CS_BASE_2_size_SIZE_32KB                          2
#define BCHP_EBI_CS_BASE_2_size_SIZE_64KB                          3
#define BCHP_EBI_CS_BASE_2_size_SIZE_128KB                         4
#define BCHP_EBI_CS_BASE_2_size_SIZE_256KB                         5
#define BCHP_EBI_CS_BASE_2_size_SIZE_512KB                         6
#define BCHP_EBI_CS_BASE_2_size_SIZE_1MB                           7
#define BCHP_EBI_CS_BASE_2_size_SIZE_2MB                           8
#define BCHP_EBI_CS_BASE_2_size_SIZE_4MB                           9
#define BCHP_EBI_CS_BASE_2_size_SIZE_8MB                           10
#define BCHP_EBI_CS_BASE_2_size_SIZE_16MB                          11
#define BCHP_EBI_CS_BASE_2_size_SIZE_32MB                          12
#define BCHP_EBI_CS_BASE_2_size_SIZE_64MB                          13
#define BCHP_EBI_CS_BASE_2_size_SIZE_128MB                         14
#define BCHP_EBI_CS_BASE_2_size_SIZE_256MB                         15

/***************************************************************************
 *CS_CONFIG_2 - EBI CS Config 2 Register
 ***************************************************************************/
/* EBI :: CS_CONFIG_2 :: reserved0 [31:30] */
#define BCHP_EBI_CS_CONFIG_2_reserved0_MASK                        0xc0000000
#define BCHP_EBI_CS_CONFIG_2_reserved0_SHIFT                       30

/* EBI :: CS_CONFIG_2 :: wp [29:29] */
#define BCHP_EBI_CS_CONFIG_2_wp_MASK                               0x20000000
#define BCHP_EBI_CS_CONFIG_2_wp_SHIFT                              29

/* EBI :: CS_CONFIG_2 :: wait_count [28:24] */
#define BCHP_EBI_CS_CONFIG_2_wait_count_MASK                       0x1f000000
#define BCHP_EBI_CS_CONFIG_2_wait_count_SHIFT                      24

/* EBI :: CS_CONFIG_2 :: t_hold [23:20] */
#define BCHP_EBI_CS_CONFIG_2_t_hold_MASK                           0x00f00000
#define BCHP_EBI_CS_CONFIG_2_t_hold_SHIFT                          20

/* EBI :: CS_CONFIG_2 :: t_setup [19:16] */
#define BCHP_EBI_CS_CONFIG_2_t_setup_MASK                          0x000f0000
#define BCHP_EBI_CS_CONFIG_2_t_setup_SHIFT                         16

/* EBI :: CS_CONFIG_2 :: reserved1 [15:15] */
#define BCHP_EBI_CS_CONFIG_2_reserved1_MASK                        0x00008000
#define BCHP_EBI_CS_CONFIG_2_reserved1_SHIFT                       15

/* EBI :: CS_CONFIG_2 :: split_cs [14:14] */
#define BCHP_EBI_CS_CONFIG_2_split_cs_MASK                         0x00004000
#define BCHP_EBI_CS_CONFIG_2_split_cs_SHIFT                        14

/* EBI :: CS_CONFIG_2 :: mask_en [13:13] */
#define BCHP_EBI_CS_CONFIG_2_mask_en_MASK                          0x00002000
#define BCHP_EBI_CS_CONFIG_2_mask_en_SHIFT                         13

/* EBI :: CS_CONFIG_2 :: ne_sample [12:12] */
#define BCHP_EBI_CS_CONFIG_2_ne_sample_MASK                        0x00001000
#define BCHP_EBI_CS_CONFIG_2_ne_sample_SHIFT                       12

/* EBI :: CS_CONFIG_2 :: reserved2 [11:11] */
#define BCHP_EBI_CS_CONFIG_2_reserved2_MASK                        0x00000800
#define BCHP_EBI_CS_CONFIG_2_reserved2_SHIFT                       11

/* EBI :: CS_CONFIG_2 :: le [10:10] */
#define BCHP_EBI_CS_CONFIG_2_le_MASK                               0x00000400
#define BCHP_EBI_CS_CONFIG_2_le_SHIFT                              10

/* EBI :: CS_CONFIG_2 :: fast_read [09:09] */
#define BCHP_EBI_CS_CONFIG_2_fast_read_MASK                        0x00000200
#define BCHP_EBI_CS_CONFIG_2_fast_read_SHIFT                       9
#define BCHP_EBI_CS_CONFIG_2_fast_read_Normal                      0
#define BCHP_EBI_CS_CONFIG_2_fast_read_Fast_Read_Enable            1

/* EBI :: CS_CONFIG_2 :: reserved3 [08:06] */
#define BCHP_EBI_CS_CONFIG_2_reserved3_MASK                        0x000001c0
#define BCHP_EBI_CS_CONFIG_2_reserved3_SHIFT                       6

/* EBI :: CS_CONFIG_2 :: we_ctl [05:05] */
#define BCHP_EBI_CS_CONFIG_2_we_ctl_MASK                           0x00000020
#define BCHP_EBI_CS_CONFIG_2_we_ctl_SHIFT                          5

/* EBI :: CS_CONFIG_2 :: dest_size [04:04] */
#define BCHP_EBI_CS_CONFIG_2_dest_size_MASK                        0x00000010
#define BCHP_EBI_CS_CONFIG_2_dest_size_SHIFT                       4

/* EBI :: CS_CONFIG_2 :: ms_inh [03:03] */
#define BCHP_EBI_CS_CONFIG_2_ms_inh_MASK                           0x00000008
#define BCHP_EBI_CS_CONFIG_2_ms_inh_SHIFT                          3

/* EBI :: CS_CONFIG_2 :: ls_inh [02:02] */
#define BCHP_EBI_CS_CONFIG_2_ls_inh_MASK                           0x00000004
#define BCHP_EBI_CS_CONFIG_2_ls_inh_SHIFT                          2

/* EBI :: CS_CONFIG_2 :: bcachen [01:01] */
#define BCHP_EBI_CS_CONFIG_2_bcachen_MASK                          0x00000002
#define BCHP_EBI_CS_CONFIG_2_bcachen_SHIFT                         1

/* EBI :: CS_CONFIG_2 :: enable [00:00] */
#define BCHP_EBI_CS_CONFIG_2_enable_MASK                           0x00000001
#define BCHP_EBI_CS_CONFIG_2_enable_SHIFT                          0

/***************************************************************************
 *BURST_CFG_0 - EBI Synchronous Intel StrataFlash Burst Configure Register
 ***************************************************************************/
/* EBI :: BURST_CFG_0 :: prefetch_enable [31:31] */
#define BCHP_EBI_BURST_CFG_0_prefetch_enable_MASK                  0x80000000
#define BCHP_EBI_BURST_CFG_0_prefetch_enable_SHIFT                 31

/* EBI :: BURST_CFG_0 :: page_enable [30:30] */
#define BCHP_EBI_BURST_CFG_0_page_enable_MASK                      0x40000000
#define BCHP_EBI_BURST_CFG_0_page_enable_SHIFT                     30

/* EBI :: BURST_CFG_0 :: pfetch_abort_enable [29:29] */
#define BCHP_EBI_BURST_CFG_0_pfetch_abort_enable_MASK              0x20000000
#define BCHP_EBI_BURST_CFG_0_pfetch_abort_enable_SHIFT             29

/* EBI :: BURST_CFG_0 :: page_latch_enable [28:28] */
#define BCHP_EBI_BURST_CFG_0_page_latch_enable_MASK                0x10000000
#define BCHP_EBI_BURST_CFG_0_page_latch_enable_SHIFT               28

/* EBI :: BURST_CFG_0 :: pfetch_wrap_enable [27:27] */
#define BCHP_EBI_BURST_CFG_0_pfetch_wrap_enable_MASK               0x08000000
#define BCHP_EBI_BURST_CFG_0_pfetch_wrap_enable_SHIFT              27

/* EBI :: BURST_CFG_0 :: reserved0 [26:24] */
#define BCHP_EBI_BURST_CFG_0_reserved0_MASK                        0x07000000
#define BCHP_EBI_BURST_CFG_0_reserved0_SHIFT                       24

/* EBI :: BURST_CFG_0 :: prefetch_size [23:22] */
#define BCHP_EBI_BURST_CFG_0_prefetch_size_MASK                    0x00c00000
#define BCHP_EBI_BURST_CFG_0_prefetch_size_SHIFT                   22
#define BCHP_EBI_BURST_CFG_0_prefetch_size_SIZE_4_BYTES            0
#define BCHP_EBI_BURST_CFG_0_prefetch_size_SIZE_8_BYTES            1
#define BCHP_EBI_BURST_CFG_0_prefetch_size_SIZE_16_BYTES           2
#define BCHP_EBI_BURST_CFG_0_prefetch_size_SIZE_32_BYTES           3

/* EBI :: BURST_CFG_0 :: page_size [21:20] */
#define BCHP_EBI_BURST_CFG_0_page_size_MASK                        0x00300000
#define BCHP_EBI_BURST_CFG_0_page_size_SHIFT                       20
#define BCHP_EBI_BURST_CFG_0_page_size_SIZE_4_BYTES                0
#define BCHP_EBI_BURST_CFG_0_page_size_SIZE_8_BYTES                1
#define BCHP_EBI_BURST_CFG_0_page_size_SIZE_16_BYTES               2
#define BCHP_EBI_BURST_CFG_0_page_size_SIZE_32_BYTES               3

/* EBI :: BURST_CFG_0 :: page_wait_count [19:16] */
#define BCHP_EBI_BURST_CFG_0_page_wait_count_MASK                  0x000f0000
#define BCHP_EBI_BURST_CFG_0_page_wait_count_SHIFT                 16

/* EBI :: BURST_CFG_0 :: burst_cyc_disable [15:15] */
#define BCHP_EBI_BURST_CFG_0_burst_cyc_disable_MASK                0x00008000
#define BCHP_EBI_BURST_CFG_0_burst_cyc_disable_SHIFT               15

/* EBI :: BURST_CFG_0 :: reserved1 [14:08] */
#define BCHP_EBI_BURST_CFG_0_reserved1_MASK                        0x00007f00
#define BCHP_EBI_BURST_CFG_0_reserved1_SHIFT                       8

/* EBI :: BURST_CFG_0 :: burst_latency [07:04] */
#define BCHP_EBI_BURST_CFG_0_burst_latency_MASK                    0x000000f0
#define BCHP_EBI_BURST_CFG_0_burst_latency_SHIFT                   4

/* EBI :: BURST_CFG_0 :: reserved2 [03:02] */
#define BCHP_EBI_BURST_CFG_0_reserved2_MASK                        0x0000000c
#define BCHP_EBI_BURST_CFG_0_reserved2_SHIFT                       2

/* EBI :: BURST_CFG_0 :: data_hold [01:01] */
#define BCHP_EBI_BURST_CFG_0_data_hold_MASK                        0x00000002
#define BCHP_EBI_BURST_CFG_0_data_hold_SHIFT                       1

/* EBI :: BURST_CFG_0 :: burst_16 [00:00] */
#define BCHP_EBI_BURST_CFG_0_burst_16_MASK                         0x00000001
#define BCHP_EBI_BURST_CFG_0_burst_16_SHIFT                        0

/***************************************************************************
 *BURST_CFG_1 - EBI Synchronous Intel StrataFlash Burst Configure Register
 ***************************************************************************/
/* EBI :: BURST_CFG_1 :: prefetch_enable [31:31] */
#define BCHP_EBI_BURST_CFG_1_prefetch_enable_MASK                  0x80000000
#define BCHP_EBI_BURST_CFG_1_prefetch_enable_SHIFT                 31

/* EBI :: BURST_CFG_1 :: page_enable [30:30] */
#define BCHP_EBI_BURST_CFG_1_page_enable_MASK                      0x40000000
#define BCHP_EBI_BURST_CFG_1_page_enable_SHIFT                     30

/* EBI :: BURST_CFG_1 :: pfetch_abort_enable [29:29] */
#define BCHP_EBI_BURST_CFG_1_pfetch_abort_enable_MASK              0x20000000
#define BCHP_EBI_BURST_CFG_1_pfetch_abort_enable_SHIFT             29

/* EBI :: BURST_CFG_1 :: page_latch_enable [28:28] */
#define BCHP_EBI_BURST_CFG_1_page_latch_enable_MASK                0x10000000
#define BCHP_EBI_BURST_CFG_1_page_latch_enable_SHIFT               28

/* EBI :: BURST_CFG_1 :: pfetch_wrap_enable [27:27] */
#define BCHP_EBI_BURST_CFG_1_pfetch_wrap_enable_MASK               0x08000000
#define BCHP_EBI_BURST_CFG_1_pfetch_wrap_enable_SHIFT              27

/* EBI :: BURST_CFG_1 :: reserved0 [26:24] */
#define BCHP_EBI_BURST_CFG_1_reserved0_MASK                        0x07000000
#define BCHP_EBI_BURST_CFG_1_reserved0_SHIFT                       24

/* EBI :: BURST_CFG_1 :: prefetch_size [23:22] */
#define BCHP_EBI_BURST_CFG_1_prefetch_size_MASK                    0x00c00000
#define BCHP_EBI_BURST_CFG_1_prefetch_size_SHIFT                   22
#define BCHP_EBI_BURST_CFG_1_prefetch_size_SIZE_4_BYTES            0
#define BCHP_EBI_BURST_CFG_1_prefetch_size_SIZE_8_BYTES            1
#define BCHP_EBI_BURST_CFG_1_prefetch_size_SIZE_16_BYTES           2
#define BCHP_EBI_BURST_CFG_1_prefetch_size_SIZE_32_BYTES           3

/* EBI :: BURST_CFG_1 :: page_size [21:20] */
#define BCHP_EBI_BURST_CFG_1_page_size_MASK                        0x00300000
#define BCHP_EBI_BURST_CFG_1_page_size_SHIFT                       20
#define BCHP_EBI_BURST_CFG_1_page_size_SIZE_4_BYTES                0
#define BCHP_EBI_BURST_CFG_1_page_size_SIZE_8_BYTES                1
#define BCHP_EBI_BURST_CFG_1_page_size_SIZE_16_BYTES               2
#define BCHP_EBI_BURST_CFG_1_page_size_SIZE_32_BYTES               3

/* EBI :: BURST_CFG_1 :: page_wait_count [19:16] */
#define BCHP_EBI_BURST_CFG_1_page_wait_count_MASK                  0x000f0000
#define BCHP_EBI_BURST_CFG_1_page_wait_count_SHIFT                 16

/* EBI :: BURST_CFG_1 :: burst_cyc_disable [15:15] */
#define BCHP_EBI_BURST_CFG_1_burst_cyc_disable_MASK                0x00008000
#define BCHP_EBI_BURST_CFG_1_burst_cyc_disable_SHIFT               15

/* EBI :: BURST_CFG_1 :: reserved1 [14:08] */
#define BCHP_EBI_BURST_CFG_1_reserved1_MASK                        0x00007f00
#define BCHP_EBI_BURST_CFG_1_reserved1_SHIFT                       8

/* EBI :: BURST_CFG_1 :: burst_latency [07:04] */
#define BCHP_EBI_BURST_CFG_1_burst_latency_MASK                    0x000000f0
#define BCHP_EBI_BURST_CFG_1_burst_latency_SHIFT                   4

/* EBI :: BURST_CFG_1 :: reserved2 [03:02] */
#define BCHP_EBI_BURST_CFG_1_reserved2_MASK                        0x0000000c
#define BCHP_EBI_BURST_CFG_1_reserved2_SHIFT                       2

/* EBI :: BURST_CFG_1 :: data_hold [01:01] */
#define BCHP_EBI_BURST_CFG_1_data_hold_MASK                        0x00000002
#define BCHP_EBI_BURST_CFG_1_data_hold_SHIFT                       1

/* EBI :: BURST_CFG_1 :: burst_16 [00:00] */
#define BCHP_EBI_BURST_CFG_1_burst_16_MASK                         0x00000001
#define BCHP_EBI_BURST_CFG_1_burst_16_SHIFT                        0

/***************************************************************************
 *BURST_CFG_2 - EBI Synchronous Intel StrataFlash Burst Configure Register
 ***************************************************************************/
/* EBI :: BURST_CFG_2 :: prefetch_enable [31:31] */
#define BCHP_EBI_BURST_CFG_2_prefetch_enable_MASK                  0x80000000
#define BCHP_EBI_BURST_CFG_2_prefetch_enable_SHIFT                 31

/* EBI :: BURST_CFG_2 :: page_enable [30:30] */
#define BCHP_EBI_BURST_CFG_2_page_enable_MASK                      0x40000000
#define BCHP_EBI_BURST_CFG_2_page_enable_SHIFT                     30

/* EBI :: BURST_CFG_2 :: pfetch_abort_enable [29:29] */
#define BCHP_EBI_BURST_CFG_2_pfetch_abort_enable_MASK              0x20000000
#define BCHP_EBI_BURST_CFG_2_pfetch_abort_enable_SHIFT             29

/* EBI :: BURST_CFG_2 :: page_latch_enable [28:28] */
#define BCHP_EBI_BURST_CFG_2_page_latch_enable_MASK                0x10000000
#define BCHP_EBI_BURST_CFG_2_page_latch_enable_SHIFT               28

/* EBI :: BURST_CFG_2 :: pfetch_wrap_enable [27:27] */
#define BCHP_EBI_BURST_CFG_2_pfetch_wrap_enable_MASK               0x08000000
#define BCHP_EBI_BURST_CFG_2_pfetch_wrap_enable_SHIFT              27

/* EBI :: BURST_CFG_2 :: reserved0 [26:24] */
#define BCHP_EBI_BURST_CFG_2_reserved0_MASK                        0x07000000
#define BCHP_EBI_BURST_CFG_2_reserved0_SHIFT                       24

/* EBI :: BURST_CFG_2 :: prefetch_size [23:22] */
#define BCHP_EBI_BURST_CFG_2_prefetch_size_MASK                    0x00c00000
#define BCHP_EBI_BURST_CFG_2_prefetch_size_SHIFT                   22
#define BCHP_EBI_BURST_CFG_2_prefetch_size_SIZE_4_BYTES            0
#define BCHP_EBI_BURST_CFG_2_prefetch_size_SIZE_8_BYTES            1
#define BCHP_EBI_BURST_CFG_2_prefetch_size_SIZE_16_BYTES           2
#define BCHP_EBI_BURST_CFG_2_prefetch_size_SIZE_32_BYTES           3

/* EBI :: BURST_CFG_2 :: page_size [21:20] */
#define BCHP_EBI_BURST_CFG_2_page_size_MASK                        0x00300000
#define BCHP_EBI_BURST_CFG_2_page_size_SHIFT                       20
#define BCHP_EBI_BURST_CFG_2_page_size_SIZE_4_BYTES                0
#define BCHP_EBI_BURST_CFG_2_page_size_SIZE_8_BYTES                1
#define BCHP_EBI_BURST_CFG_2_page_size_SIZE_16_BYTES               2
#define BCHP_EBI_BURST_CFG_2_page_size_SIZE_32_BYTES               3

/* EBI :: BURST_CFG_2 :: page_wait_count [19:16] */
#define BCHP_EBI_BURST_CFG_2_page_wait_count_MASK                  0x000f0000
#define BCHP_EBI_BURST_CFG_2_page_wait_count_SHIFT                 16

/* EBI :: BURST_CFG_2 :: burst_cyc_disable [15:15] */
#define BCHP_EBI_BURST_CFG_2_burst_cyc_disable_MASK                0x00008000
#define BCHP_EBI_BURST_CFG_2_burst_cyc_disable_SHIFT               15

/* EBI :: BURST_CFG_2 :: reserved1 [14:08] */
#define BCHP_EBI_BURST_CFG_2_reserved1_MASK                        0x00007f00
#define BCHP_EBI_BURST_CFG_2_reserved1_SHIFT                       8

/* EBI :: BURST_CFG_2 :: burst_latency [07:04] */
#define BCHP_EBI_BURST_CFG_2_burst_latency_MASK                    0x000000f0
#define BCHP_EBI_BURST_CFG_2_burst_latency_SHIFT                   4

/* EBI :: BURST_CFG_2 :: reserved2 [03:02] */
#define BCHP_EBI_BURST_CFG_2_reserved2_MASK                        0x0000000c
#define BCHP_EBI_BURST_CFG_2_reserved2_SHIFT                       2

/* EBI :: BURST_CFG_2 :: data_hold [01:01] */
#define BCHP_EBI_BURST_CFG_2_data_hold_MASK                        0x00000002
#define BCHP_EBI_BURST_CFG_2_data_hold_SHIFT                       1

/* EBI :: BURST_CFG_2 :: burst_16 [00:00] */
#define BCHP_EBI_BURST_CFG_2_burst_16_MASK                         0x00000001
#define BCHP_EBI_BURST_CFG_2_burst_16_SHIFT                        0

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

/* EBI :: CS_TRISTATE_CFG :: reserved0 [26:03] */
#define BCHP_EBI_CS_TRISTATE_CFG_reserved0_MASK                    0x07fffff8
#define BCHP_EBI_CS_TRISTATE_CFG_reserved0_SHIFT                   3

/* EBI :: CS_TRISTATE_CFG :: tristate_ebi_cs2 [02:02] */
#define BCHP_EBI_CS_TRISTATE_CFG_tristate_ebi_cs2_MASK             0x00000004
#define BCHP_EBI_CS_TRISTATE_CFG_tristate_ebi_cs2_SHIFT            2

/* EBI :: CS_TRISTATE_CFG :: tristate_ebi_cs1 [01:01] */
#define BCHP_EBI_CS_TRISTATE_CFG_tristate_ebi_cs1_MASK             0x00000002
#define BCHP_EBI_CS_TRISTATE_CFG_tristate_ebi_cs1_SHIFT            1

/* EBI :: CS_TRISTATE_CFG :: tristate_ebi_cs0 [00:00] */
#define BCHP_EBI_CS_TRISTATE_CFG_tristate_ebi_cs0_MASK             0x00000001
#define BCHP_EBI_CS_TRISTATE_CFG_tristate_ebi_cs0_SHIFT            0

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
#define BCHP_EBI_DATA_ARRAYi_ARRAY_BASE                            0x01400a00
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
