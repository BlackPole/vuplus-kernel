/***************************************************************************
 *     Copyright (c) 1999-2008, Broadcom Corporation
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
 * Date:           Generated on         Mon Apr  7 13:38:42 2008
 *                 MD5 Checksum         ab4b694acfe1d6ced0e4f0443118d2af
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7335/rdb/b0/bchp_usb_ehci1.h $
 * 
 * Hydra_Software_Devel/1   4/7/08 3:20p tdo
 * PR40989: Initial version
 *
 ***************************************************************************/

#ifndef BCHP_USB_EHCI1_H__
#define BCHP_USB_EHCI1_H__

/***************************************************************************
 *USB_EHCI1 - USB EHCI 2 Control Registers
 ***************************************************************************/
#define BCHP_USB_EHCI1_HCCAPBASE                 0x00480500 /* EHCI Capability Register */
#define BCHP_USB_EHCI1_HCSPARAMS                 0x00480504 /* EHCI Structural Parameter */
#define BCHP_USB_EHCI1_HCCPARAMS                 0x00480508 /* EHCI Capability Parameter */
#define BCHP_USB_EHCI1_USBCMD                    0x00480510 /* USB Command Register */
#define BCHP_USB_EHCI1_USBSTS                    0x00480514 /* USB Status  Register */
#define BCHP_USB_EHCI1_USBINTR                   0x00480518 /* USB Interrupt Enable Register */
#define BCHP_USB_EHCI1_FRINDEX                   0x0048051c /* USB Frame Index Register */
#define BCHP_USB_EHCI1_PERIODICLISTBASE          0x00480524 /* Periodic Frame List Base Address Register */
#define BCHP_USB_EHCI1_ASYNCLISTADDR             0x00480528 /* Asynchronous List Address */
#define BCHP_USB_EHCI1_CONFIGFLAG                0x00480550 /* Configured Flag Register */
#define BCHP_USB_EHCI1_PORTSC_0                  0x00480554 /* Port Status/Control Register for Port 0 */
#define BCHP_USB_EHCI1_PORTSC_1                  0x00480558 /* Port Status/Control Register for Port 1 */
#define BCHP_USB_EHCI1_INSNREG00                 0x00480590 /* Microframe Base Value Register */
#define BCHP_USB_EHCI1_INSNREG01                 0x00480594 /* Packet Buffer OUT/IN Threshold Register */
#define BCHP_USB_EHCI1_INSNREG02                 0x00480598 /* Packet Buffer Depth Register */
#define BCHP_USB_EHCI1_INSNREG03                 0x0048059c /* Break Memory Transfer Register */
#define BCHP_USB_EHCI1_INSNREG04                 0x004805a0 /* Debug Register */
#define BCHP_USB_EHCI1_INSNREG05                 0x004805a4 /* UTMI Control and Status Register */

#endif /* #ifndef BCHP_USB_EHCI1_H__ */

/* End of File */
