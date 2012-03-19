/*
 * drivers/mtd/brcmnand/edu.h
 *
 *  Copyright (c) 2005-2009 Broadcom Corp.
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
 *
 * Module Description: Some defines for the EDU (EBI DMA Unit)
 ************************************************************************* */

#ifndef EDU_H__
#define EDU_H__

/*
 *
 */
#define EDU_WRITE			0
#define EDU_READ			1
#define NAND_CTRL_READY	2



#if 0 //def CONFIG_BCM7440

// #define EDU_RANDOM

#define NUMBER_OF_PASS                  256 * 8 * 10 

#define EDU_BASE_ADDRESS                0xb0000000

#define BCHP_SUN_TOP_CTRL_STRAP_VALUE   0x0040401c

#define HIF_INTR2_EDU_DONE              	0x10000000
#define HIF_INTR2_EDU_ERR              		0x20000000
#define HIF_INTR2_EDU_DONE_MASK    	(HIF_INTR2_EDU_DONE | HIF_INTR2_EDU_ERR)

#define HIF_INTR2_EDU_INTR_MASK 		0x34000000
#define HIF_INTR2_EDU_CLEAR_MASK          0x3c000000
#define HIF_INTR2_CTRL_READY			0x01000000
#define HIF_INTR2_EBI_TIMEOUT			0x00000200	/* Bit 17 */

/***************************************************************************
 *EDU - EDU Registers
 ***************************************************************************/
#define EDU_CONFIG                               0x00002c00 /* EDU Config */
#define EDU_DRAM_ADDR                            0x00002c04 /* DRAM Address for transaction */
#define EDU_EXT_ADDR                             0x00002c08 /* External Address for transaction */
#define EDU_LENGTH                               0x00002c0c /* Length of transaction */
#define EDU_CMD                                  0x00002c10 /* Command Type and Start */
#define EDU_STOP                                 0x00002c14 /* Stop */
#define EDU_STATUS                               0x00002c18 /* EDU Status bits */
#define EDU_DONE                                 0x00002c1c /* EDU Done bits */
#define EDU_ERR_STATUS                           0x00002c20 /* EDU Error Status */

#define EDU_CONFIG_VALUE                         0x00000001 /* NAND MODE */
#define EDU_LENGTH_VALUE                         512 /* Packet Length */

#define EDU_ERR_STATUS_NandECCcor                0x00000002 /* EDU NAND ECC Corr. bit mask */
#define EDU_ERR_STATUS_NandECCuncor              0x00000004 /* EDU NAND ECC UNCorr. bit mask */
#define EDU_ERR_STATUS_NandRBUS               0x00000001 /* EDU NAND RBUS ACK Error */
#define EDU_ERR_STATUS_NandWrite               0x00000008 /* EDU NAND Write (Flash) Error */

#else
#include <asm/brcmstb/brcmstb.h>

#define NUMBER_OF_PASS                  256 * 8 * 10 

//#define EDU_BASE_ADDRESS                0

//#define BCHP_SUN_TOP_CTRL_STRAP_VALUE   0x0040401c

#define HIF_INTR2_EDU_DONE              BCHP_HIF_INTR2_CPU_STATUS_EDU_DONE_INTR_MASK
#define HIF_INTR2_EDU_ERR              	BCHP_HIF_INTR2_CPU_STATUS_EDU_ERR_INTR_MASK

#define HIF_INTR2_EDU_DONE_MASK    (BCHP_HIF_INTR2_CPU_STATUS_EDU_ERR_INTR_MASK \
									| BCHP_HIF_INTR2_CPU_STATUS_EDU_DONE_INTR_MASK \
									)

#define HIF_INTR2_EDU_INTR_MASK   	(BCHP_HIF_INTR2_CPU_CLEAR_EDU_ERR_INTR_MASK \
									| BCHP_HIF_INTR2_CPU_CLEAR_EDU_DONE_INTR_MASK \
									| BCHP_HIF_INTR2_CPU_CLEAR_NAND_UNC_INTR_MASK \
									)
									
#define HIF_INTR2_EDU_CLEAR_MASK   (BCHP_HIF_INTR2_CPU_CLEAR_EDU_ERR_INTR_MASK \
									| BCHP_HIF_INTR2_CPU_CLEAR_EDU_DONE_INTR_MASK \
									| BCHP_HIF_INTR2_CPU_CLEAR_NAND_CORR_INTR_MASK \
									| BCHP_HIF_INTR2_CPU_CLEAR_NAND_UNC_INTR_MASK \
									)

#define HIF_INTR2_CTRL_READY    		BCHP_HIF_INTR2_CPU_CLEAR_NAND_CTLRDY_INTR_MASK

#define HIF_INTR2_EBI_TIMEOUT		BCHP_HIF_INTR2_CPU_STATUS_EBI_TIMEOUT_INTR_MASK



/***************************************************************************
 *EDU - EDU Registers
 ***************************************************************************/
#define EDU_CONFIG 						BCHP_EDU_CONFIG
#define EDU_DRAM_ADDR					BCHP_EDU_DRAM_ADDR
#define EDU_EXT_ADDR					BCHP_EDU_EXT_ADDR
#define EDU_LENGTH						BCHP_EDU_LENGTH
#define EDU_CMD							BCHP_EDU_CMD
#define EDU_STOP						BCHP_EDU_STOP
#define EDU_STATUS						BCHP_EDU_STATUS
#define EDU_DONE						BCHP_EDU_DONE
#define EDU_ERR_STATUS					BCHP_EDU_ERR_STATUS

#define EDU_CONFIG_VALUE				BCHP_EDU_CONFIG_Mode_MASK /* NAND MODE */
//#define EDU_CONFIG_VALUE				0 /* EBI Mode */
#define EDU_LENGTH_VALUE                 	512 /* Packet Length */

#define EDU_ERR_STATUS_NandECCcor  	BCHP_EDU_ERR_STATUS_NandECCcor_MASK
#define EDU_ERR_STATUS_NandECCuncor 	BCHP_EDU_ERR_STATUS_NandECCuncor_MASK 
#define EDU_ERR_STATUS_NandRBUS           BCHP_EDU_ERR_STATUS_ErrAck_MASK
#define EDU_ERR_STATUS_NandWrite          BCHP_EDU_ERR_STATUS_NandWrErr_MASK


#endif // !BCM7440


#endif /* #ifndef EDU_H__ */

/* End of File */
