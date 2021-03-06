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
 * Date:           Generated on         Tue Nov  2 15:33:28 2010
 *                 MD5 Checksum         d5654099cee13d096484a8849c56a604
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7358/rdb/a0/bchp_misb_bridge.h $
 * 
 * Hydra_Software_Devel/2   11/2/10 5:39p pntruong
 * SW7358-2: Synced up with central RDB.
 *
 ***************************************************************************/

#ifndef BCHP_MISB_BRIDGE_H__
#define BCHP_MISB_BRIDGE_H__

/***************************************************************************
 *MISB_BRIDGE - MISB_BRIDGE Registers
 ***************************************************************************/
#define BCHP_MISB_BRIDGE_CORE_REV_ID             0x00410400 /* MISB Bridge Revision ID Register. */
#define BCHP_MISB_BRIDGE_EXCEPTION_VECTOR_OFFSET 0x00410404 /* "Exception Vector Offset Address" */
#define BCHP_MISB_BRIDGE_PROCESSOR_ID            0x00410408 /* Processor ID Register. */
#define BCHP_MISB_BRIDGE_WG_MODE_N_TIMEOUT       0x0041040c /* Write Gathering Mode & Timeout. */
#define BCHP_MISB_BRIDGE_MISB_SPLIT_MODE         0x00410410 /* MISB Split Mode */

/***************************************************************************
 *CORE_REV_ID - MISB Bridge Revision ID Register.
 ***************************************************************************/
/* MISB_BRIDGE :: CORE_REV_ID :: reserved0 [31:16] */
#define BCHP_MISB_BRIDGE_CORE_REV_ID_reserved0_MASK                0xffff0000
#define BCHP_MISB_BRIDGE_CORE_REV_ID_reserved0_SHIFT               16

/* MISB_BRIDGE :: CORE_REV_ID :: MAJOR [15:08] */
#define BCHP_MISB_BRIDGE_CORE_REV_ID_MAJOR_MASK                    0x0000ff00
#define BCHP_MISB_BRIDGE_CORE_REV_ID_MAJOR_SHIFT                   8

/* MISB_BRIDGE :: CORE_REV_ID :: MINOR [07:00] */
#define BCHP_MISB_BRIDGE_CORE_REV_ID_MINOR_MASK                    0x000000ff
#define BCHP_MISB_BRIDGE_CORE_REV_ID_MINOR_SHIFT                   0

/***************************************************************************
 *EXCEPTION_VECTOR_OFFSET - "Exception Vector Offset Address"
 ***************************************************************************/
/* MISB_BRIDGE :: EXCEPTION_VECTOR_OFFSET :: ADDRS [31:00] */
#define BCHP_MISB_BRIDGE_EXCEPTION_VECTOR_OFFSET_ADDRS_MASK        0xffffffff
#define BCHP_MISB_BRIDGE_EXCEPTION_VECTOR_OFFSET_ADDRS_SHIFT       0

/***************************************************************************
 *PROCESSOR_ID - Processor ID Register.
 ***************************************************************************/
/* MISB_BRIDGE :: PROCESSOR_ID :: reserved0 [31:08] */
#define BCHP_MISB_BRIDGE_PROCESSOR_ID_reserved0_MASK               0xffffff00
#define BCHP_MISB_BRIDGE_PROCESSOR_ID_reserved0_SHIFT              8

/* MISB_BRIDGE :: PROCESSOR_ID :: ID [07:00] */
#define BCHP_MISB_BRIDGE_PROCESSOR_ID_ID_MASK                      0x000000ff
#define BCHP_MISB_BRIDGE_PROCESSOR_ID_ID_SHIFT                     0

/***************************************************************************
 *WG_MODE_N_TIMEOUT - Write Gathering Mode & Timeout.
 ***************************************************************************/
/* MISB_BRIDGE :: WG_MODE_N_TIMEOUT :: reserved0 [31:10] */
#define BCHP_MISB_BRIDGE_WG_MODE_N_TIMEOUT_reserved0_MASK          0xfffffc00
#define BCHP_MISB_BRIDGE_WG_MODE_N_TIMEOUT_reserved0_SHIFT         10

/* MISB_BRIDGE :: WG_MODE_N_TIMEOUT :: MODE [09:08] */
#define BCHP_MISB_BRIDGE_WG_MODE_N_TIMEOUT_MODE_MASK               0x00000300
#define BCHP_MISB_BRIDGE_WG_MODE_N_TIMEOUT_MODE_SHIFT              8
#define BCHP_MISB_BRIDGE_WG_MODE_N_TIMEOUT_MODE_MODE_0             0
#define BCHP_MISB_BRIDGE_WG_MODE_N_TIMEOUT_MODE_MODE_1             1
#define BCHP_MISB_BRIDGE_WG_MODE_N_TIMEOUT_MODE_MODE_2             2

/* MISB_BRIDGE :: WG_MODE_N_TIMEOUT :: TIMEOUT [07:00] */
#define BCHP_MISB_BRIDGE_WG_MODE_N_TIMEOUT_TIMEOUT_MASK            0x000000ff
#define BCHP_MISB_BRIDGE_WG_MODE_N_TIMEOUT_TIMEOUT_SHIFT           0

/***************************************************************************
 *MISB_SPLIT_MODE - MISB Split Mode
 ***************************************************************************/
/* MISB_BRIDGE :: MISB_SPLIT_MODE :: reserved0 [31:01] */
#define BCHP_MISB_BRIDGE_MISB_SPLIT_MODE_reserved0_MASK            0xfffffffe
#define BCHP_MISB_BRIDGE_MISB_SPLIT_MODE_reserved0_SHIFT           1

/* MISB_BRIDGE :: MISB_SPLIT_MODE :: SPLIT_MODE [00:00] */
#define BCHP_MISB_BRIDGE_MISB_SPLIT_MODE_SPLIT_MODE_MASK           0x00000001
#define BCHP_MISB_BRIDGE_MISB_SPLIT_MODE_SPLIT_MODE_SHIFT          0
#define BCHP_MISB_BRIDGE_MISB_SPLIT_MODE_SPLIT_MODE_ENABLE         1
#define BCHP_MISB_BRIDGE_MISB_SPLIT_MODE_SPLIT_MODE_DISABLE        0

#endif /* #ifndef BCHP_MISB_BRIDGE_H__ */

/* End of File */
