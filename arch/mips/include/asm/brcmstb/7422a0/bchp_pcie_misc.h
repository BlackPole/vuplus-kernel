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
 * Date:           Generated on         Wed Jun  9 17:04:01 2010
 *                 MD5 Checksum         e12b4c5c08ac555273c26b63d085a2b6
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7422/rdb/a0/bchp_pcie_misc.h $
 * 
 * Hydra_Software_Devel/3   6/10/10 7:28p albertl
 * SW7422-1: Updated to match RDB.
 *
 ***************************************************************************/

#ifndef BCHP_PCIE_MISC_H__
#define BCHP_PCIE_MISC_H__

/***************************************************************************
 *PCIE_MISC - PCI-E Miscellaneous Registers
 ***************************************************************************/
#define BCHP_PCIE_MISC_RESET_CTRL                0x00414000 /* Reset Control Register */
#define BCHP_PCIE_MISC_ECO_CTRL_CORE             0x00414004 /* ECO Core Reset Control Register */
#define BCHP_PCIE_MISC_MISC_CTRL                 0x00414008 /* MISC Control Register */
#define BCHP_PCIE_MISC_CPU_2_PCIE_MEM_WIN0_LO    0x0041400c /* CPU to PCI-E Memory Window 0 Low */
#define BCHP_PCIE_MISC_CPU_2_PCIE_MEM_WIN0_HI    0x00414010 /* CPU to PCI-E Memory Window 0 High */
#define BCHP_PCIE_MISC_CPU_2_PCIE_MEM_WIN1_LO    0x00414014 /* CPU to PCI-E Memory Window 1 Low */
#define BCHP_PCIE_MISC_CPU_2_PCIE_MEM_WIN1_HI    0x00414018 /* CPU to PCI-E Memory Window 1 High */
#define BCHP_PCIE_MISC_CPU_2_PCIE_MEM_WIN2_LO    0x0041401c /* CPU to PCI-E Memory Window 2 Low */
#define BCHP_PCIE_MISC_CPU_2_PCIE_MEM_WIN2_HI    0x00414020 /* CPU to PCI-E Memory Window 2 High */
#define BCHP_PCIE_MISC_CPU_2_PCIE_MEM_WIN3_LO    0x00414024 /* CPU to PCI-E Memory Window 3 Low */
#define BCHP_PCIE_MISC_CPU_2_PCIE_MEM_WIN3_HI    0x00414028 /* CPU to PCI-E Memory Window 3 High */
#define BCHP_PCIE_MISC_RC_BAR1_CONFIG_LO         0x0041402c /* RC BAR1 Configuration Low Register */
#define BCHP_PCIE_MISC_RC_BAR1_CONFIG_HI         0x00414030 /* RC BAR1 Configuration High Register */
#define BCHP_PCIE_MISC_RC_BAR2_CONFIG_LO         0x00414034 /* RC BAR2 Configuration Low Register */
#define BCHP_PCIE_MISC_RC_BAR2_CONFIG_HI         0x00414038 /* RC BAR2 Configuration High Register */
#define BCHP_PCIE_MISC_RC_BAR3_CONFIG_LO         0x0041403c /* RC BAR3 Configuration Low Register */
#define BCHP_PCIE_MISC_RC_BAR3_CONFIG_HI         0x00414040 /* RC BAR3 Configuration High Register */
#define BCHP_PCIE_MISC_MSI_BAR_CONFIG_LO         0x00414044 /* Message Signaled Interrupt Base Address Low Register */
#define BCHP_PCIE_MISC_MSI_BAR_CONFIG_HI         0x00414048 /* Message Signaled Interrupt Base Address High Register */
#define BCHP_PCIE_MISC_MSI_DATA_CONFIG           0x0041404c /* Message Signaled Interrupt Data Configuration Register */
#define BCHP_PCIE_MISC_RC_BAD_ADDRESS_LO         0x00414050 /* RC Bad Address Register Low */
#define BCHP_PCIE_MISC_RC_BAD_ADDRESS_HI         0x00414054 /* RC Bad Address Register High */
#define BCHP_PCIE_MISC_RC_BAD_DATA               0x00414058 /* RC Bad Data Register */
#define BCHP_PCIE_MISC_RC_CONFIG_RETRY_TIMEOUT   0x0041405c /* RC Configuration Retry Timeout Register */
#define BCHP_PCIE_MISC_EOI_CTRL                  0x00414060 /* End of Interrupt Control Register */
#define BCHP_PCIE_MISC_PCIE_CTRL                 0x00414064 /* PCIE Control */
#define BCHP_PCIE_MISC_PCIE_STATUS               0x00414068 /* PCIE Status */
#define BCHP_PCIE_MISC_REVISION                  0x0041406c /* PCIE Revision */
#define BCHP_PCIE_MISC_UBUS_TIMEOUT              0x00414070 /* UBUS Timeout */

/***************************************************************************
 *RESET_CTRL - Reset Control Register
 ***************************************************************************/
/* PCIE_MISC :: RESET_CTRL :: reserved0 [31:01] */
#define BCHP_PCIE_MISC_RESET_CTRL_reserved0_MASK                   0xfffffffe
#define BCHP_PCIE_MISC_RESET_CTRL_reserved0_SHIFT                  1

/* PCIE_MISC :: RESET_CTRL :: CORE_RESET [00:00] */
#define BCHP_PCIE_MISC_RESET_CTRL_CORE_RESET_MASK                  0x00000001
#define BCHP_PCIE_MISC_RESET_CTRL_CORE_RESET_SHIFT                 0

/***************************************************************************
 *ECO_CTRL_CORE - ECO Core Reset Control Register
 ***************************************************************************/
/* PCIE_MISC :: ECO_CTRL_CORE :: reserved0 [31:16] */
#define BCHP_PCIE_MISC_ECO_CTRL_CORE_reserved0_MASK                0xffff0000
#define BCHP_PCIE_MISC_ECO_CTRL_CORE_reserved0_SHIFT               16

/* PCIE_MISC :: ECO_CTRL_CORE :: ECO_CORE_RST_N [15:00] */
#define BCHP_PCIE_MISC_ECO_CTRL_CORE_ECO_CORE_RST_N_MASK           0x0000ffff
#define BCHP_PCIE_MISC_ECO_CTRL_CORE_ECO_CORE_RST_N_SHIFT          0

/***************************************************************************
 *MISC_CTRL - MISC Control Register
 ***************************************************************************/
/* PCIE_MISC :: MISC_CTRL :: SCB0_SIZE [31:27] */
#define BCHP_PCIE_MISC_MISC_CTRL_SCB0_SIZE_MASK                    0xf8000000
#define BCHP_PCIE_MISC_MISC_CTRL_SCB0_SIZE_SHIFT                   27

/* PCIE_MISC :: MISC_CTRL :: SCB1_SIZE [26:22] */
#define BCHP_PCIE_MISC_MISC_CTRL_SCB1_SIZE_MASK                    0x07c00000
#define BCHP_PCIE_MISC_MISC_CTRL_SCB1_SIZE_SHIFT                   22

/* PCIE_MISC :: MISC_CTRL :: TBD_OPTION_21 [21:21] */
#define BCHP_PCIE_MISC_MISC_CTRL_TBD_OPTION_21_MASK                0x00200000
#define BCHP_PCIE_MISC_MISC_CTRL_TBD_OPTION_21_SHIFT               21

/* PCIE_MISC :: MISC_CTRL :: SCB_MAX_BURST_SIZE [20:20] */
#define BCHP_PCIE_MISC_MISC_CTRL_SCB_MAX_BURST_SIZE_MASK           0x00100000
#define BCHP_PCIE_MISC_MISC_CTRL_SCB_MAX_BURST_SIZE_SHIFT          20

/* PCIE_MISC :: MISC_CTRL :: TBD_OPTION_19 [19:19] */
#define BCHP_PCIE_MISC_MISC_CTRL_TBD_OPTION_19_MASK                0x00080000
#define BCHP_PCIE_MISC_MISC_CTRL_TBD_OPTION_19_SHIFT               19

/* PCIE_MISC :: MISC_CTRL :: TBD_OPTION_18 [18:18] */
#define BCHP_PCIE_MISC_MISC_CTRL_TBD_OPTION_18_MASK                0x00040000
#define BCHP_PCIE_MISC_MISC_CTRL_TBD_OPTION_18_SHIFT               18

/* PCIE_MISC :: MISC_CTRL :: CSR_CFG_MODE [17:17] */
#define BCHP_PCIE_MISC_MISC_CTRL_CSR_CFG_MODE_MASK                 0x00020000
#define BCHP_PCIE_MISC_MISC_CTRL_CSR_CFG_MODE_SHIFT                17

/* PCIE_MISC :: MISC_CTRL :: CSR_CFG_RETRY_EN [16:16] */
#define BCHP_PCIE_MISC_MISC_CTRL_CSR_CFG_RETRY_EN_MASK             0x00010000
#define BCHP_PCIE_MISC_MISC_CTRL_CSR_CFG_RETRY_EN_SHIFT            16

/* PCIE_MISC :: MISC_CTRL :: UBUS_DMA_WR_WITH_REPLY [15:15] */
#define BCHP_PCIE_MISC_MISC_CTRL_UBUS_DMA_WR_WITH_REPLY_MASK       0x00008000
#define BCHP_PCIE_MISC_MISC_CTRL_UBUS_DMA_WR_WITH_REPLY_SHIFT      15

/* PCIE_MISC :: MISC_CTRL :: UBUS_WR_WITH_REPLY [14:14] */
#define BCHP_PCIE_MISC_MISC_CTRL_UBUS_WR_WITH_REPLY_MASK           0x00004000
#define BCHP_PCIE_MISC_MISC_CTRL_UBUS_WR_WITH_REPLY_SHIFT          14

/* PCIE_MISC :: MISC_CTRL :: CFG_READ_UR_MODE [13:13] */
#define BCHP_PCIE_MISC_MISC_CTRL_CFG_READ_UR_MODE_MASK             0x00002000
#define BCHP_PCIE_MISC_MISC_CTRL_CFG_READ_UR_MODE_SHIFT            13

/* PCIE_MISC :: MISC_CTRL :: SCB_ACCESS_EN [12:12] */
#define BCHP_PCIE_MISC_MISC_CTRL_SCB_ACCESS_EN_MASK                0x00001000
#define BCHP_PCIE_MISC_MISC_CTRL_SCB_ACCESS_EN_SHIFT               12

/* PCIE_MISC :: MISC_CTRL :: UBUS_PCIE_REPLY_ERR_DIS [11:11] */
#define BCHP_PCIE_MISC_MISC_CTRL_UBUS_PCIE_REPLY_ERR_DIS_MASK      0x00000800
#define BCHP_PCIE_MISC_MISC_CTRL_UBUS_PCIE_REPLY_ERR_DIS_SHIFT     11

/* PCIE_MISC :: MISC_CTRL :: UBUS_REG_ACCESS_RO [10:10] */
#define BCHP_PCIE_MISC_MISC_CTRL_UBUS_REG_ACCESS_RO_MASK           0x00000400
#define BCHP_PCIE_MISC_MISC_CTRL_UBUS_REG_ACCESS_RO_SHIFT          10

/* PCIE_MISC :: MISC_CTRL :: DESC_PRIORITY_EN [09:09] */
#define BCHP_PCIE_MISC_MISC_CTRL_DESC_PRIORITY_EN_MASK             0x00000200
#define BCHP_PCIE_MISC_MISC_CTRL_DESC_PRIORITY_EN_SHIFT            9

/* PCIE_MISC :: MISC_CTRL :: READ_PRIORITY_EN [08:08] */
#define BCHP_PCIE_MISC_MISC_CTRL_READ_PRIORITY_EN_MASK             0x00000100
#define BCHP_PCIE_MISC_MISC_CTRL_READ_PRIORITY_EN_SHIFT            8

/* PCIE_MISC :: MISC_CTRL :: PCIE_RCB_64B_MODE [07:07] */
#define BCHP_PCIE_MISC_MISC_CTRL_PCIE_RCB_64B_MODE_MASK            0x00000080
#define BCHP_PCIE_MISC_MISC_CTRL_PCIE_RCB_64B_MODE_SHIFT           7

/* PCIE_MISC :: MISC_CTRL :: PCIE_OUT_CPL_RO [06:06] */
#define BCHP_PCIE_MISC_MISC_CTRL_PCIE_OUT_CPL_RO_MASK              0x00000040
#define BCHP_PCIE_MISC_MISC_CTRL_PCIE_OUT_CPL_RO_SHIFT             6

/* PCIE_MISC :: MISC_CTRL :: PCIE_IN_CPL_RO [05:05] */
#define BCHP_PCIE_MISC_MISC_CTRL_PCIE_IN_CPL_RO_MASK               0x00000020
#define BCHP_PCIE_MISC_MISC_CTRL_PCIE_IN_CPL_RO_SHIFT              5

/* PCIE_MISC :: MISC_CTRL :: RELAXED_ORDERING [04:04] */
#define BCHP_PCIE_MISC_MISC_CTRL_RELAXED_ORDERING_MASK             0x00000010
#define BCHP_PCIE_MISC_MISC_CTRL_RELAXED_ORDERING_SHIFT            4

/* PCIE_MISC :: MISC_CTRL :: NO_SNOOP [03:03] */
#define BCHP_PCIE_MISC_MISC_CTRL_NO_SNOOP_MASK                     0x00000008
#define BCHP_PCIE_MISC_MISC_CTRL_NO_SNOOP_SHIFT                    3

/* PCIE_MISC :: MISC_CTRL :: TRAFFIC_CLASS [02:00] */
#define BCHP_PCIE_MISC_MISC_CTRL_TRAFFIC_CLASS_MASK                0x00000007
#define BCHP_PCIE_MISC_MISC_CTRL_TRAFFIC_CLASS_SHIFT               0

/***************************************************************************
 *CPU_2_PCIE_MEM_WIN0_LO - CPU to PCI-E Memory Window 0 Low
 ***************************************************************************/
/* PCIE_MISC :: CPU_2_PCIE_MEM_WIN0_LO :: BASE_ADDR [31:27] */
#define BCHP_PCIE_MISC_CPU_2_PCIE_MEM_WIN0_LO_BASE_ADDR_MASK       0xf8000000
#define BCHP_PCIE_MISC_CPU_2_PCIE_MEM_WIN0_LO_BASE_ADDR_SHIFT      27

/* PCIE_MISC :: CPU_2_PCIE_MEM_WIN0_LO :: reserved0 [26:02] */
#define BCHP_PCIE_MISC_CPU_2_PCIE_MEM_WIN0_LO_reserved0_MASK       0x07fffffc
#define BCHP_PCIE_MISC_CPU_2_PCIE_MEM_WIN0_LO_reserved0_SHIFT      2

/* PCIE_MISC :: CPU_2_PCIE_MEM_WIN0_LO :: ENDIAN_MODE [01:00] */
#define BCHP_PCIE_MISC_CPU_2_PCIE_MEM_WIN0_LO_ENDIAN_MODE_MASK     0x00000003
#define BCHP_PCIE_MISC_CPU_2_PCIE_MEM_WIN0_LO_ENDIAN_MODE_SHIFT    0

/***************************************************************************
 *CPU_2_PCIE_MEM_WIN0_HI - CPU to PCI-E Memory Window 0 High
 ***************************************************************************/
/* PCIE_MISC :: CPU_2_PCIE_MEM_WIN0_HI :: BASE_ADDR [31:00] */
#define BCHP_PCIE_MISC_CPU_2_PCIE_MEM_WIN0_HI_BASE_ADDR_MASK       0xffffffff
#define BCHP_PCIE_MISC_CPU_2_PCIE_MEM_WIN0_HI_BASE_ADDR_SHIFT      0

/***************************************************************************
 *CPU_2_PCIE_MEM_WIN1_LO - CPU to PCI-E Memory Window 1 Low
 ***************************************************************************/
/* PCIE_MISC :: CPU_2_PCIE_MEM_WIN1_LO :: BASE_ADDR [31:27] */
#define BCHP_PCIE_MISC_CPU_2_PCIE_MEM_WIN1_LO_BASE_ADDR_MASK       0xf8000000
#define BCHP_PCIE_MISC_CPU_2_PCIE_MEM_WIN1_LO_BASE_ADDR_SHIFT      27

/* PCIE_MISC :: CPU_2_PCIE_MEM_WIN1_LO :: reserved0 [26:02] */
#define BCHP_PCIE_MISC_CPU_2_PCIE_MEM_WIN1_LO_reserved0_MASK       0x07fffffc
#define BCHP_PCIE_MISC_CPU_2_PCIE_MEM_WIN1_LO_reserved0_SHIFT      2

/* PCIE_MISC :: CPU_2_PCIE_MEM_WIN1_LO :: ENDIAN_MODE [01:00] */
#define BCHP_PCIE_MISC_CPU_2_PCIE_MEM_WIN1_LO_ENDIAN_MODE_MASK     0x00000003
#define BCHP_PCIE_MISC_CPU_2_PCIE_MEM_WIN1_LO_ENDIAN_MODE_SHIFT    0

/***************************************************************************
 *CPU_2_PCIE_MEM_WIN1_HI - CPU to PCI-E Memory Window 1 High
 ***************************************************************************/
/* PCIE_MISC :: CPU_2_PCIE_MEM_WIN1_HI :: BASE_ADDR [31:00] */
#define BCHP_PCIE_MISC_CPU_2_PCIE_MEM_WIN1_HI_BASE_ADDR_MASK       0xffffffff
#define BCHP_PCIE_MISC_CPU_2_PCIE_MEM_WIN1_HI_BASE_ADDR_SHIFT      0

/***************************************************************************
 *CPU_2_PCIE_MEM_WIN2_LO - CPU to PCI-E Memory Window 2 Low
 ***************************************************************************/
/* PCIE_MISC :: CPU_2_PCIE_MEM_WIN2_LO :: BASE_ADDR [31:27] */
#define BCHP_PCIE_MISC_CPU_2_PCIE_MEM_WIN2_LO_BASE_ADDR_MASK       0xf8000000
#define BCHP_PCIE_MISC_CPU_2_PCIE_MEM_WIN2_LO_BASE_ADDR_SHIFT      27

/* PCIE_MISC :: CPU_2_PCIE_MEM_WIN2_LO :: reserved0 [26:02] */
#define BCHP_PCIE_MISC_CPU_2_PCIE_MEM_WIN2_LO_reserved0_MASK       0x07fffffc
#define BCHP_PCIE_MISC_CPU_2_PCIE_MEM_WIN2_LO_reserved0_SHIFT      2

/* PCIE_MISC :: CPU_2_PCIE_MEM_WIN2_LO :: ENDIAN_MODE [01:00] */
#define BCHP_PCIE_MISC_CPU_2_PCIE_MEM_WIN2_LO_ENDIAN_MODE_MASK     0x00000003
#define BCHP_PCIE_MISC_CPU_2_PCIE_MEM_WIN2_LO_ENDIAN_MODE_SHIFT    0

/***************************************************************************
 *CPU_2_PCIE_MEM_WIN2_HI - CPU to PCI-E Memory Window 2 High
 ***************************************************************************/
/* PCIE_MISC :: CPU_2_PCIE_MEM_WIN2_HI :: BASE_ADDR [31:00] */
#define BCHP_PCIE_MISC_CPU_2_PCIE_MEM_WIN2_HI_BASE_ADDR_MASK       0xffffffff
#define BCHP_PCIE_MISC_CPU_2_PCIE_MEM_WIN2_HI_BASE_ADDR_SHIFT      0

/***************************************************************************
 *CPU_2_PCIE_MEM_WIN3_LO - CPU to PCI-E Memory Window 3 Low
 ***************************************************************************/
/* PCIE_MISC :: CPU_2_PCIE_MEM_WIN3_LO :: BASE_ADDR [31:27] */
#define BCHP_PCIE_MISC_CPU_2_PCIE_MEM_WIN3_LO_BASE_ADDR_MASK       0xf8000000
#define BCHP_PCIE_MISC_CPU_2_PCIE_MEM_WIN3_LO_BASE_ADDR_SHIFT      27

/* PCIE_MISC :: CPU_2_PCIE_MEM_WIN3_LO :: reserved0 [26:02] */
#define BCHP_PCIE_MISC_CPU_2_PCIE_MEM_WIN3_LO_reserved0_MASK       0x07fffffc
#define BCHP_PCIE_MISC_CPU_2_PCIE_MEM_WIN3_LO_reserved0_SHIFT      2

/* PCIE_MISC :: CPU_2_PCIE_MEM_WIN3_LO :: ENDIAN_MODE [01:00] */
#define BCHP_PCIE_MISC_CPU_2_PCIE_MEM_WIN3_LO_ENDIAN_MODE_MASK     0x00000003
#define BCHP_PCIE_MISC_CPU_2_PCIE_MEM_WIN3_LO_ENDIAN_MODE_SHIFT    0

/***************************************************************************
 *CPU_2_PCIE_MEM_WIN3_HI - CPU to PCI-E Memory Window 3 High
 ***************************************************************************/
/* PCIE_MISC :: CPU_2_PCIE_MEM_WIN3_HI :: BASE_ADDR [31:00] */
#define BCHP_PCIE_MISC_CPU_2_PCIE_MEM_WIN3_HI_BASE_ADDR_MASK       0xffffffff
#define BCHP_PCIE_MISC_CPU_2_PCIE_MEM_WIN3_HI_BASE_ADDR_SHIFT      0

/***************************************************************************
 *RC_BAR1_CONFIG_LO - RC BAR1 Configuration Low Register
 ***************************************************************************/
/* PCIE_MISC :: RC_BAR1_CONFIG_LO :: MATCH_ADDRESS [31:16] */
#define BCHP_PCIE_MISC_RC_BAR1_CONFIG_LO_MATCH_ADDRESS_MASK        0xffff0000
#define BCHP_PCIE_MISC_RC_BAR1_CONFIG_LO_MATCH_ADDRESS_SHIFT       16

/* PCIE_MISC :: RC_BAR1_CONFIG_LO :: reserved0 [15:05] */
#define BCHP_PCIE_MISC_RC_BAR1_CONFIG_LO_reserved0_MASK            0x0000ffe0
#define BCHP_PCIE_MISC_RC_BAR1_CONFIG_LO_reserved0_SHIFT           5

/* PCIE_MISC :: RC_BAR1_CONFIG_LO :: SIZE [04:00] */
#define BCHP_PCIE_MISC_RC_BAR1_CONFIG_LO_SIZE_MASK                 0x0000001f
#define BCHP_PCIE_MISC_RC_BAR1_CONFIG_LO_SIZE_SHIFT                0

/***************************************************************************
 *RC_BAR1_CONFIG_HI - RC BAR1 Configuration High Register
 ***************************************************************************/
/* PCIE_MISC :: RC_BAR1_CONFIG_HI :: MATCH_ADDRESS [31:00] */
#define BCHP_PCIE_MISC_RC_BAR1_CONFIG_HI_MATCH_ADDRESS_MASK        0xffffffff
#define BCHP_PCIE_MISC_RC_BAR1_CONFIG_HI_MATCH_ADDRESS_SHIFT       0

/***************************************************************************
 *RC_BAR2_CONFIG_LO - RC BAR2 Configuration Low Register
 ***************************************************************************/
/* PCIE_MISC :: RC_BAR2_CONFIG_LO :: MATCH_ADDRESS [31:16] */
#define BCHP_PCIE_MISC_RC_BAR2_CONFIG_LO_MATCH_ADDRESS_MASK        0xffff0000
#define BCHP_PCIE_MISC_RC_BAR2_CONFIG_LO_MATCH_ADDRESS_SHIFT       16

/* PCIE_MISC :: RC_BAR2_CONFIG_LO :: reserved0 [15:05] */
#define BCHP_PCIE_MISC_RC_BAR2_CONFIG_LO_reserved0_MASK            0x0000ffe0
#define BCHP_PCIE_MISC_RC_BAR2_CONFIG_LO_reserved0_SHIFT           5

/* PCIE_MISC :: RC_BAR2_CONFIG_LO :: SIZE [04:00] */
#define BCHP_PCIE_MISC_RC_BAR2_CONFIG_LO_SIZE_MASK                 0x0000001f
#define BCHP_PCIE_MISC_RC_BAR2_CONFIG_LO_SIZE_SHIFT                0

/***************************************************************************
 *RC_BAR2_CONFIG_HI - RC BAR2 Configuration High Register
 ***************************************************************************/
/* PCIE_MISC :: RC_BAR2_CONFIG_HI :: MATCH_ADDRESS [31:00] */
#define BCHP_PCIE_MISC_RC_BAR2_CONFIG_HI_MATCH_ADDRESS_MASK        0xffffffff
#define BCHP_PCIE_MISC_RC_BAR2_CONFIG_HI_MATCH_ADDRESS_SHIFT       0

/***************************************************************************
 *RC_BAR3_CONFIG_LO - RC BAR3 Configuration Low Register
 ***************************************************************************/
/* PCIE_MISC :: RC_BAR3_CONFIG_LO :: MATCH_ADDRESS [31:16] */
#define BCHP_PCIE_MISC_RC_BAR3_CONFIG_LO_MATCH_ADDRESS_MASK        0xffff0000
#define BCHP_PCIE_MISC_RC_BAR3_CONFIG_LO_MATCH_ADDRESS_SHIFT       16

/* PCIE_MISC :: RC_BAR3_CONFIG_LO :: reserved0 [15:05] */
#define BCHP_PCIE_MISC_RC_BAR3_CONFIG_LO_reserved0_MASK            0x0000ffe0
#define BCHP_PCIE_MISC_RC_BAR3_CONFIG_LO_reserved0_SHIFT           5

/* PCIE_MISC :: RC_BAR3_CONFIG_LO :: SIZE [04:00] */
#define BCHP_PCIE_MISC_RC_BAR3_CONFIG_LO_SIZE_MASK                 0x0000001f
#define BCHP_PCIE_MISC_RC_BAR3_CONFIG_LO_SIZE_SHIFT                0

/***************************************************************************
 *RC_BAR3_CONFIG_HI - RC BAR3 Configuration High Register
 ***************************************************************************/
/* PCIE_MISC :: RC_BAR3_CONFIG_HI :: MATCH_ADDRESS [31:00] */
#define BCHP_PCIE_MISC_RC_BAR3_CONFIG_HI_MATCH_ADDRESS_MASK        0xffffffff
#define BCHP_PCIE_MISC_RC_BAR3_CONFIG_HI_MATCH_ADDRESS_SHIFT       0

/***************************************************************************
 *MSI_BAR_CONFIG_LO - Message Signaled Interrupt Base Address Low Register
 ***************************************************************************/
/* PCIE_MISC :: MSI_BAR_CONFIG_LO :: MATCH_ADDRESS [31:02] */
#define BCHP_PCIE_MISC_MSI_BAR_CONFIG_LO_MATCH_ADDRESS_MASK        0xfffffffc
#define BCHP_PCIE_MISC_MSI_BAR_CONFIG_LO_MATCH_ADDRESS_SHIFT       2

/* PCIE_MISC :: MSI_BAR_CONFIG_LO :: reserved0 [01:01] */
#define BCHP_PCIE_MISC_MSI_BAR_CONFIG_LO_reserved0_MASK            0x00000002
#define BCHP_PCIE_MISC_MSI_BAR_CONFIG_LO_reserved0_SHIFT           1

/* PCIE_MISC :: MSI_BAR_CONFIG_LO :: ENABLE [00:00] */
#define BCHP_PCIE_MISC_MSI_BAR_CONFIG_LO_ENABLE_MASK               0x00000001
#define BCHP_PCIE_MISC_MSI_BAR_CONFIG_LO_ENABLE_SHIFT              0

/***************************************************************************
 *MSI_BAR_CONFIG_HI - Message Signaled Interrupt Base Address High Register
 ***************************************************************************/
/* PCIE_MISC :: MSI_BAR_CONFIG_HI :: MATCH_ADDRESS [31:00] */
#define BCHP_PCIE_MISC_MSI_BAR_CONFIG_HI_MATCH_ADDRESS_MASK        0xffffffff
#define BCHP_PCIE_MISC_MSI_BAR_CONFIG_HI_MATCH_ADDRESS_SHIFT       0

/***************************************************************************
 *MSI_DATA_CONFIG - Message Signaled Interrupt Data Configuration Register
 ***************************************************************************/
/* PCIE_MISC :: MSI_DATA_CONFIG :: MASK [31:16] */
#define BCHP_PCIE_MISC_MSI_DATA_CONFIG_MASK_MASK                   0xffff0000
#define BCHP_PCIE_MISC_MSI_DATA_CONFIG_MASK_SHIFT                  16

/* PCIE_MISC :: MSI_DATA_CONFIG :: DATA [15:00] */
#define BCHP_PCIE_MISC_MSI_DATA_CONFIG_DATA_MASK                   0x0000ffff
#define BCHP_PCIE_MISC_MSI_DATA_CONFIG_DATA_SHIFT                  0

/***************************************************************************
 *RC_BAD_ADDRESS_LO - RC Bad Address Register Low
 ***************************************************************************/
/* PCIE_MISC :: RC_BAD_ADDRESS_LO :: ADDRESS [31:02] */
#define BCHP_PCIE_MISC_RC_BAD_ADDRESS_LO_ADDRESS_MASK              0xfffffffc
#define BCHP_PCIE_MISC_RC_BAD_ADDRESS_LO_ADDRESS_SHIFT             2

/* PCIE_MISC :: RC_BAD_ADDRESS_LO :: reserved0 [01:01] */
#define BCHP_PCIE_MISC_RC_BAD_ADDRESS_LO_reserved0_MASK            0x00000002
#define BCHP_PCIE_MISC_RC_BAD_ADDRESS_LO_reserved0_SHIFT           1

/* PCIE_MISC :: RC_BAD_ADDRESS_LO :: VALID [00:00] */
#define BCHP_PCIE_MISC_RC_BAD_ADDRESS_LO_VALID_MASK                0x00000001
#define BCHP_PCIE_MISC_RC_BAD_ADDRESS_LO_VALID_SHIFT               0

/***************************************************************************
 *RC_BAD_ADDRESS_HI - RC Bad Address Register High
 ***************************************************************************/
/* PCIE_MISC :: RC_BAD_ADDRESS_HI :: ADDRESS [31:00] */
#define BCHP_PCIE_MISC_RC_BAD_ADDRESS_HI_ADDRESS_MASK              0xffffffff
#define BCHP_PCIE_MISC_RC_BAD_ADDRESS_HI_ADDRESS_SHIFT             0

/***************************************************************************
 *RC_BAD_DATA - RC Bad Data Register
 ***************************************************************************/
/* PCIE_MISC :: RC_BAD_DATA :: DATA [31:00] */
#define BCHP_PCIE_MISC_RC_BAD_DATA_DATA_MASK                       0xffffffff
#define BCHP_PCIE_MISC_RC_BAD_DATA_DATA_SHIFT                      0

/***************************************************************************
 *RC_CONFIG_RETRY_TIMEOUT - RC Configuration Retry Timeout Register
 ***************************************************************************/
/* PCIE_MISC :: RC_CONFIG_RETRY_TIMEOUT :: TIMER_VALUE [31:00] */
#define BCHP_PCIE_MISC_RC_CONFIG_RETRY_TIMEOUT_TIMER_VALUE_MASK    0xffffffff
#define BCHP_PCIE_MISC_RC_CONFIG_RETRY_TIMEOUT_TIMER_VALUE_SHIFT   0

/***************************************************************************
 *EOI_CTRL - End of Interrupt Control Register
 ***************************************************************************/
/* PCIE_MISC :: EOI_CTRL :: reserved0 [31:01] */
#define BCHP_PCIE_MISC_EOI_CTRL_reserved0_MASK                     0xfffffffe
#define BCHP_PCIE_MISC_EOI_CTRL_reserved0_SHIFT                    1

/* PCIE_MISC :: EOI_CTRL :: EOI [00:00] */
#define BCHP_PCIE_MISC_EOI_CTRL_EOI_MASK                           0x00000001
#define BCHP_PCIE_MISC_EOI_CTRL_EOI_SHIFT                          0

/***************************************************************************
 *PCIE_CTRL - PCIE Control
 ***************************************************************************/
/* PCIE_MISC :: PCIE_CTRL :: reserved0 [31:02] */
#define BCHP_PCIE_MISC_PCIE_CTRL_reserved0_MASK                    0xfffffffc
#define BCHP_PCIE_MISC_PCIE_CTRL_reserved0_SHIFT                   2

/* PCIE_MISC :: PCIE_CTRL :: PCIE_PME_REQUEST [01:01] */
#define BCHP_PCIE_MISC_PCIE_CTRL_PCIE_PME_REQUEST_MASK             0x00000002
#define BCHP_PCIE_MISC_PCIE_CTRL_PCIE_PME_REQUEST_SHIFT            1

/* PCIE_MISC :: PCIE_CTRL :: PCIE_L23_REQUEST [00:00] */
#define BCHP_PCIE_MISC_PCIE_CTRL_PCIE_L23_REQUEST_MASK             0x00000001
#define BCHP_PCIE_MISC_PCIE_CTRL_PCIE_L23_REQUEST_SHIFT            0

/***************************************************************************
 *PCIE_STATUS - PCIE Status
 ***************************************************************************/
/* PCIE_MISC :: PCIE_STATUS :: reserved0 [31:12] */
#define BCHP_PCIE_MISC_PCIE_STATUS_reserved0_MASK                  0xfffff000
#define BCHP_PCIE_MISC_PCIE_STATUS_reserved0_SHIFT                 12

/* PCIE_MISC :: PCIE_STATUS :: PCIE_PM_STATE [11:10] */
#define BCHP_PCIE_MISC_PCIE_STATUS_PCIE_PM_STATE_MASK              0x00000c00
#define BCHP_PCIE_MISC_PCIE_STATUS_PCIE_PM_STATE_SHIFT             10

/* PCIE_MISC :: PCIE_STATUS :: PCIE_WAKE [09:09] */
#define BCHP_PCIE_MISC_PCIE_STATUS_PCIE_WAKE_MASK                  0x00000200
#define BCHP_PCIE_MISC_PCIE_STATUS_PCIE_WAKE_SHIFT                 9

/* PCIE_MISC :: PCIE_STATUS :: PCIE_PME_EVENT [08:08] */
#define BCHP_PCIE_MISC_PCIE_STATUS_PCIE_PME_EVENT_MASK             0x00000100
#define BCHP_PCIE_MISC_PCIE_STATUS_PCIE_PME_EVENT_SHIFT            8

/* PCIE_MISC :: PCIE_STATUS :: reserved1 [07:07] */
#define BCHP_PCIE_MISC_PCIE_STATUS_reserved1_MASK                  0x00000080
#define BCHP_PCIE_MISC_PCIE_STATUS_reserved1_SHIFT                 7

/* PCIE_MISC :: PCIE_STATUS :: PCIE_LINK_IN_L23 [06:06] */
#define BCHP_PCIE_MISC_PCIE_STATUS_PCIE_LINK_IN_L23_MASK           0x00000040
#define BCHP_PCIE_MISC_PCIE_STATUS_PCIE_LINK_IN_L23_SHIFT          6

/* PCIE_MISC :: PCIE_STATUS :: PCIE_DL_ACTIVE [05:05] */
#define BCHP_PCIE_MISC_PCIE_STATUS_PCIE_DL_ACTIVE_MASK             0x00000020
#define BCHP_PCIE_MISC_PCIE_STATUS_PCIE_DL_ACTIVE_SHIFT            5

/* PCIE_MISC :: PCIE_STATUS :: PCIE_PHYLINKUP [04:04] */
#define BCHP_PCIE_MISC_PCIE_STATUS_PCIE_PHYLINKUP_MASK             0x00000010
#define BCHP_PCIE_MISC_PCIE_STATUS_PCIE_PHYLINKUP_SHIFT            4

/* PCIE_MISC :: PCIE_STATUS :: PCIE_ERR_STATUS [03:00] */
#define BCHP_PCIE_MISC_PCIE_STATUS_PCIE_ERR_STATUS_MASK            0x0000000f
#define BCHP_PCIE_MISC_PCIE_STATUS_PCIE_ERR_STATUS_SHIFT           0

/***************************************************************************
 *REVISION - PCIE Revision
 ***************************************************************************/
/* PCIE_MISC :: REVISION :: reserved0 [31:16] */
#define BCHP_PCIE_MISC_REVISION_reserved0_MASK                     0xffff0000
#define BCHP_PCIE_MISC_REVISION_reserved0_SHIFT                    16

/* PCIE_MISC :: REVISION :: MAJOR [15:08] */
#define BCHP_PCIE_MISC_REVISION_MAJOR_MASK                         0x0000ff00
#define BCHP_PCIE_MISC_REVISION_MAJOR_SHIFT                        8

/* PCIE_MISC :: REVISION :: MINOR [07:00] */
#define BCHP_PCIE_MISC_REVISION_MINOR_MASK                         0x000000ff
#define BCHP_PCIE_MISC_REVISION_MINOR_SHIFT                        0

/***************************************************************************
 *UBUS_TIMEOUT - UBUS Timeout
 ***************************************************************************/
/* PCIE_MISC :: UBUS_TIMEOUT :: TIMER_VALUE [31:00] */
#define BCHP_PCIE_MISC_UBUS_TIMEOUT_TIMER_VALUE_MASK               0xffffffff
#define BCHP_PCIE_MISC_UBUS_TIMEOUT_TIMER_VALUE_SHIFT              0

#endif /* #ifndef BCHP_PCIE_MISC_H__ */

/* End of File */
