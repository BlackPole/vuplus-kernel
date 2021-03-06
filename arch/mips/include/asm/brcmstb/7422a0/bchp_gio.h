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
 * Date:           Generated on         Wed Jun  9 16:45:09 2010
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
 * $brcm_Log: /magnum/basemodules/chp/7422/rdb/a0/bchp_gio.h $
 * 
 * Hydra_Software_Devel/2   6/10/10 7:24p albertl
 * SW7422-1: Updated to match RDB.
 *
 ***************************************************************************/

#ifndef BCHP_GIO_H__
#define BCHP_GIO_H__

/***************************************************************************
 *GIO - GPIO
 ***************************************************************************/
#define BCHP_GIO_ODEN_LO                         0x00406700 /* GENERAL PURPOSE I/O OPEN DRAIN ENABLE [31:0] */
#define BCHP_GIO_DATA_LO                         0x00406704 /* GENERAL PURPOSE I/O DATA [31:0] */
#define BCHP_GIO_IODIR_LO                        0x00406708 /* GENERAL PURPOSE I/O DIRECTION [31:0] */
#define BCHP_GIO_EC_LO                           0x0040670c /* GENERAL PURPOSE I/O EDGE CONFIGURATION [31:0] */
#define BCHP_GIO_EI_LO                           0x00406710 /* GENERAL PURPOSE I/O EDGE INSENSITIVE [31:0] */
#define BCHP_GIO_MASK_LO                         0x00406714 /* GENERAL PURPOSE I/O INTERRUPT MASK [31:0] */
#define BCHP_GIO_LEVEL_LO                        0x00406718 /* GENERAL PURPOSE I/O INTERRUPT TYPE [31:0] */
#define BCHP_GIO_STAT_LO                         0x0040671c /* GENERAL PURPOSE I/O INTERRUPT STATUS [31:0] */
#define BCHP_GIO_ODEN_HI                         0x00406720 /* GENERAL PURPOSE I/O OPEN DRAIN ENABLE [63:32] */
#define BCHP_GIO_DATA_HI                         0x00406724 /* GENERAL PURPOSE I/O DATA [63:32] */
#define BCHP_GIO_IODIR_HI                        0x00406728 /* GENERAL PURPOSE I/O DIRECTION [63:32] */
#define BCHP_GIO_EC_HI                           0x0040672c /* GENERAL PURPOSE I/O EDGE CONFIGURATION [63:32] */
#define BCHP_GIO_EI_HI                           0x00406730 /* GENERAL PURPOSE I/O EDGE INSENSITIVE [63:32] */
#define BCHP_GIO_MASK_HI                         0x00406734 /* GENERAL PURPOSE I/O INTERRUPT MASK [63:32] */
#define BCHP_GIO_LEVEL_HI                        0x00406738 /* GENERAL PURPOSE I/O INTERRUPT TYPE [63:32] */
#define BCHP_GIO_STAT_HI                         0x0040673c /* GENERAL PURPOSE I/O INTERRUPT STATUS [63:32] */
#define BCHP_GIO_ODEN_EXT                        0x00406740 /* GENERAL PURPOSE I/O OPEN DRAIN ENABLE [95:64] (GPIO[89:64], SGPIO[5:0]) */
#define BCHP_GIO_DATA_EXT                        0x00406744 /* GENERAL PURPOSE I/O DATA [95:64] (GPIO[89:64], SGPIO[5:0]) */
#define BCHP_GIO_IODIR_EXT                       0x00406748 /* GENERAL PURPOSE I/O DIRECTION [95:64] (GPIO[89:64], SGPIO[5:0]) */
#define BCHP_GIO_EC_EXT                          0x0040674c /* GENERAL PURPOSE I/O EDGE CONFIGURATION [95:64] (GPIO[89:64], SGPIO[5:0]) */
#define BCHP_GIO_EI_EXT                          0x00406750 /* GENERAL PURPOSE I/O EDGE INSENSITIVE [95:64] (GPIO[89:64], SGPIO[5:0]) */
#define BCHP_GIO_MASK_EXT                        0x00406754 /* GENERAL PURPOSE I/O INTERRUPT MASK [95:64] (GPIO[89:64], SGPIO[5:0]) */
#define BCHP_GIO_LEVEL_EXT                       0x00406758 /* GENERAL PURPOSE I/O INTERRUPT TYPE [95:64] (GPIO[89:64], SGPIO[5:0]) */
#define BCHP_GIO_STAT_EXT                        0x0040675c /* GENERAL PURPOSE I/O INTERRUPT STATUS [95:64] (GPIO[89:64], SGPIO[5:0]) */
#define BCHP_GIO_ODEN_EXT_HI                     0x00406760 /* GENERAL PURPOSE I/O OPEN DRAIN ENABLE [116:96] (GPIO[110:90]) */
#define BCHP_GIO_DATA_EXT_HI                     0x00406764 /* GENERAL PURPOSE I/O DATA [116:96] (GPIO[110:90]) */
#define BCHP_GIO_IODIR_EXT_HI                    0x00406768 /* GENERAL PURPOSE I/O DIRECTION [116:96] (GPIO[110:90]) */
#define BCHP_GIO_EC_EXT_HI                       0x0040676c /* GENERAL PURPOSE I/O EDGE CONFIGURATION [116:96] (GPIO[110:90]) */
#define BCHP_GIO_EI_EXT_HI                       0x00406770 /* GENERAL PURPOSE I/O EDGE INSENSITIVE [116:96] (GPIO[110:90]) */
#define BCHP_GIO_MASK_EXT_HI                     0x00406774 /* GENERAL PURPOSE I/O INTERRUPT MASK [116:96] (GPIO[110:90]) */
#define BCHP_GIO_LEVEL_EXT_HI                    0x00406778 /* GENERAL PURPOSE I/O INTERRUPT TYPE [116:96] (GPIO[110:90]) */
#define BCHP_GIO_STAT_EXT_HI                     0x0040677c /* GENERAL PURPOSE I/O INTERRUPT STATUS [116:96] (GPIO[110:90]) */

/***************************************************************************
 *ODEN_LO - GENERAL PURPOSE I/O OPEN DRAIN ENABLE [31:0]
 ***************************************************************************/
/* GIO :: ODEN_LO :: oden [31:00] */
#define BCHP_GIO_ODEN_LO_oden_MASK                                 0xffffffff
#define BCHP_GIO_ODEN_LO_oden_SHIFT                                0

/***************************************************************************
 *DATA_LO - GENERAL PURPOSE I/O DATA [31:0]
 ***************************************************************************/
/* GIO :: DATA_LO :: data [31:00] */
#define BCHP_GIO_DATA_LO_data_MASK                                 0xffffffff
#define BCHP_GIO_DATA_LO_data_SHIFT                                0

/***************************************************************************
 *IODIR_LO - GENERAL PURPOSE I/O DIRECTION [31:0]
 ***************************************************************************/
/* GIO :: IODIR_LO :: iodir [31:00] */
#define BCHP_GIO_IODIR_LO_iodir_MASK                               0xffffffff
#define BCHP_GIO_IODIR_LO_iodir_SHIFT                              0

/***************************************************************************
 *EC_LO - GENERAL PURPOSE I/O EDGE CONFIGURATION [31:0]
 ***************************************************************************/
/* GIO :: EC_LO :: edge_config [31:00] */
#define BCHP_GIO_EC_LO_edge_config_MASK                            0xffffffff
#define BCHP_GIO_EC_LO_edge_config_SHIFT                           0

/***************************************************************************
 *EI_LO - GENERAL PURPOSE I/O EDGE INSENSITIVE [31:0]
 ***************************************************************************/
/* GIO :: EI_LO :: edge_insensitive [31:00] */
#define BCHP_GIO_EI_LO_edge_insensitive_MASK                       0xffffffff
#define BCHP_GIO_EI_LO_edge_insensitive_SHIFT                      0

/***************************************************************************
 *MASK_LO - GENERAL PURPOSE I/O INTERRUPT MASK [31:0]
 ***************************************************************************/
/* GIO :: MASK_LO :: irq_mask [31:00] */
#define BCHP_GIO_MASK_LO_irq_mask_MASK                             0xffffffff
#define BCHP_GIO_MASK_LO_irq_mask_SHIFT                            0

/***************************************************************************
 *LEVEL_LO - GENERAL PURPOSE I/O INTERRUPT TYPE [31:0]
 ***************************************************************************/
/* GIO :: LEVEL_LO :: level [31:00] */
#define BCHP_GIO_LEVEL_LO_level_MASK                               0xffffffff
#define BCHP_GIO_LEVEL_LO_level_SHIFT                              0

/***************************************************************************
 *STAT_LO - GENERAL PURPOSE I/O INTERRUPT STATUS [31:0]
 ***************************************************************************/
/* GIO :: STAT_LO :: irq_status [31:00] */
#define BCHP_GIO_STAT_LO_irq_status_MASK                           0xffffffff
#define BCHP_GIO_STAT_LO_irq_status_SHIFT                          0

/***************************************************************************
 *ODEN_HI - GENERAL PURPOSE I/O OPEN DRAIN ENABLE [63:32]
 ***************************************************************************/
/* GIO :: ODEN_HI :: oden [31:00] */
#define BCHP_GIO_ODEN_HI_oden_MASK                                 0xffffffff
#define BCHP_GIO_ODEN_HI_oden_SHIFT                                0

/***************************************************************************
 *DATA_HI - GENERAL PURPOSE I/O DATA [63:32]
 ***************************************************************************/
/* GIO :: DATA_HI :: data [31:00] */
#define BCHP_GIO_DATA_HI_data_MASK                                 0xffffffff
#define BCHP_GIO_DATA_HI_data_SHIFT                                0

/***************************************************************************
 *IODIR_HI - GENERAL PURPOSE I/O DIRECTION [63:32]
 ***************************************************************************/
/* GIO :: IODIR_HI :: iodir [31:00] */
#define BCHP_GIO_IODIR_HI_iodir_MASK                               0xffffffff
#define BCHP_GIO_IODIR_HI_iodir_SHIFT                              0

/***************************************************************************
 *EC_HI - GENERAL PURPOSE I/O EDGE CONFIGURATION [63:32]
 ***************************************************************************/
/* GIO :: EC_HI :: edge_config [31:00] */
#define BCHP_GIO_EC_HI_edge_config_MASK                            0xffffffff
#define BCHP_GIO_EC_HI_edge_config_SHIFT                           0

/***************************************************************************
 *EI_HI - GENERAL PURPOSE I/O EDGE INSENSITIVE [63:32]
 ***************************************************************************/
/* GIO :: EI_HI :: edge_insensitive [31:00] */
#define BCHP_GIO_EI_HI_edge_insensitive_MASK                       0xffffffff
#define BCHP_GIO_EI_HI_edge_insensitive_SHIFT                      0

/***************************************************************************
 *MASK_HI - GENERAL PURPOSE I/O INTERRUPT MASK [63:32]
 ***************************************************************************/
/* GIO :: MASK_HI :: irq_mask [31:00] */
#define BCHP_GIO_MASK_HI_irq_mask_MASK                             0xffffffff
#define BCHP_GIO_MASK_HI_irq_mask_SHIFT                            0

/***************************************************************************
 *LEVEL_HI - GENERAL PURPOSE I/O INTERRUPT TYPE [63:32]
 ***************************************************************************/
/* GIO :: LEVEL_HI :: level [31:00] */
#define BCHP_GIO_LEVEL_HI_level_MASK                               0xffffffff
#define BCHP_GIO_LEVEL_HI_level_SHIFT                              0

/***************************************************************************
 *STAT_HI - GENERAL PURPOSE I/O INTERRUPT STATUS [63:32]
 ***************************************************************************/
/* GIO :: STAT_HI :: irq_status [31:00] */
#define BCHP_GIO_STAT_HI_irq_status_MASK                           0xffffffff
#define BCHP_GIO_STAT_HI_irq_status_SHIFT                          0

/***************************************************************************
 *ODEN_EXT - GENERAL PURPOSE I/O OPEN DRAIN ENABLE [95:64] (GPIO[89:64], SGPIO[5:0])
 ***************************************************************************/
/* GIO :: ODEN_EXT :: oden [31:00] */
#define BCHP_GIO_ODEN_EXT_oden_MASK                                0xffffffff
#define BCHP_GIO_ODEN_EXT_oden_SHIFT                               0

/***************************************************************************
 *DATA_EXT - GENERAL PURPOSE I/O DATA [95:64] (GPIO[89:64], SGPIO[5:0])
 ***************************************************************************/
/* GIO :: DATA_EXT :: data [31:00] */
#define BCHP_GIO_DATA_EXT_data_MASK                                0xffffffff
#define BCHP_GIO_DATA_EXT_data_SHIFT                               0

/***************************************************************************
 *IODIR_EXT - GENERAL PURPOSE I/O DIRECTION [95:64] (GPIO[89:64], SGPIO[5:0])
 ***************************************************************************/
/* GIO :: IODIR_EXT :: iodir [31:00] */
#define BCHP_GIO_IODIR_EXT_iodir_MASK                              0xffffffff
#define BCHP_GIO_IODIR_EXT_iodir_SHIFT                             0

/***************************************************************************
 *EC_EXT - GENERAL PURPOSE I/O EDGE CONFIGURATION [95:64] (GPIO[89:64], SGPIO[5:0])
 ***************************************************************************/
/* GIO :: EC_EXT :: edge_config [31:00] */
#define BCHP_GIO_EC_EXT_edge_config_MASK                           0xffffffff
#define BCHP_GIO_EC_EXT_edge_config_SHIFT                          0

/***************************************************************************
 *EI_EXT - GENERAL PURPOSE I/O EDGE INSENSITIVE [95:64] (GPIO[89:64], SGPIO[5:0])
 ***************************************************************************/
/* GIO :: EI_EXT :: edge_insensitive [31:00] */
#define BCHP_GIO_EI_EXT_edge_insensitive_MASK                      0xffffffff
#define BCHP_GIO_EI_EXT_edge_insensitive_SHIFT                     0

/***************************************************************************
 *MASK_EXT - GENERAL PURPOSE I/O INTERRUPT MASK [95:64] (GPIO[89:64], SGPIO[5:0])
 ***************************************************************************/
/* GIO :: MASK_EXT :: irq_mask [31:00] */
#define BCHP_GIO_MASK_EXT_irq_mask_MASK                            0xffffffff
#define BCHP_GIO_MASK_EXT_irq_mask_SHIFT                           0

/***************************************************************************
 *LEVEL_EXT - GENERAL PURPOSE I/O INTERRUPT TYPE [95:64] (GPIO[89:64], SGPIO[5:0])
 ***************************************************************************/
/* GIO :: LEVEL_EXT :: level [31:00] */
#define BCHP_GIO_LEVEL_EXT_level_MASK                              0xffffffff
#define BCHP_GIO_LEVEL_EXT_level_SHIFT                             0

/***************************************************************************
 *STAT_EXT - GENERAL PURPOSE I/O INTERRUPT STATUS [95:64] (GPIO[89:64], SGPIO[5:0])
 ***************************************************************************/
/* GIO :: STAT_EXT :: irq_status [31:00] */
#define BCHP_GIO_STAT_EXT_irq_status_MASK                          0xffffffff
#define BCHP_GIO_STAT_EXT_irq_status_SHIFT                         0

/***************************************************************************
 *ODEN_EXT_HI - GENERAL PURPOSE I/O OPEN DRAIN ENABLE [116:96] (GPIO[110:90])
 ***************************************************************************/
/* GIO :: ODEN_EXT_HI :: reserved0 [31:03] */
#define BCHP_GIO_ODEN_EXT_HI_reserved0_MASK                        0xfffffff8
#define BCHP_GIO_ODEN_EXT_HI_reserved0_SHIFT                       3

/* GIO :: ODEN_EXT_HI :: oden [02:00] */
#define BCHP_GIO_ODEN_EXT_HI_oden_MASK                             0x00000007
#define BCHP_GIO_ODEN_EXT_HI_oden_SHIFT                            0

/***************************************************************************
 *DATA_EXT_HI - GENERAL PURPOSE I/O DATA [116:96] (GPIO[110:90])
 ***************************************************************************/
/* GIO :: DATA_EXT_HI :: data [31:00] */
#define BCHP_GIO_DATA_EXT_HI_data_MASK                             0xffffffff
#define BCHP_GIO_DATA_EXT_HI_data_SHIFT                            0

/***************************************************************************
 *IODIR_EXT_HI - GENERAL PURPOSE I/O DIRECTION [116:96] (GPIO[110:90])
 ***************************************************************************/
/* GIO :: IODIR_EXT_HI :: reserved0 [31:03] */
#define BCHP_GIO_IODIR_EXT_HI_reserved0_MASK                       0xfffffff8
#define BCHP_GIO_IODIR_EXT_HI_reserved0_SHIFT                      3

/* GIO :: IODIR_EXT_HI :: iodir [02:00] */
#define BCHP_GIO_IODIR_EXT_HI_iodir_MASK                           0x00000007
#define BCHP_GIO_IODIR_EXT_HI_iodir_SHIFT                          0

/***************************************************************************
 *EC_EXT_HI - GENERAL PURPOSE I/O EDGE CONFIGURATION [116:96] (GPIO[110:90])
 ***************************************************************************/
/* GIO :: EC_EXT_HI :: edge_config [31:00] */
#define BCHP_GIO_EC_EXT_HI_edge_config_MASK                        0xffffffff
#define BCHP_GIO_EC_EXT_HI_edge_config_SHIFT                       0

/***************************************************************************
 *EI_EXT_HI - GENERAL PURPOSE I/O EDGE INSENSITIVE [116:96] (GPIO[110:90])
 ***************************************************************************/
/* GIO :: EI_EXT_HI :: reserved0 [31:27] */
#define BCHP_GIO_EI_EXT_HI_reserved0_MASK                          0xf8000000
#define BCHP_GIO_EI_EXT_HI_reserved0_SHIFT                         27

/* GIO :: EI_EXT_HI :: edge_insensitive [26:00] */
#define BCHP_GIO_EI_EXT_HI_edge_insensitive_MASK                   0x07ffffff
#define BCHP_GIO_EI_EXT_HI_edge_insensitive_SHIFT                  0

/***************************************************************************
 *MASK_EXT_HI - GENERAL PURPOSE I/O INTERRUPT MASK [116:96] (GPIO[110:90])
 ***************************************************************************/
/* GIO :: MASK_EXT_HI :: irq_mask [31:00] */
#define BCHP_GIO_MASK_EXT_HI_irq_mask_MASK                         0xffffffff
#define BCHP_GIO_MASK_EXT_HI_irq_mask_SHIFT                        0

/***************************************************************************
 *LEVEL_EXT_HI - GENERAL PURPOSE I/O INTERRUPT TYPE [116:96] (GPIO[110:90])
 ***************************************************************************/
/* GIO :: LEVEL_EXT_HI :: level [31:00] */
#define BCHP_GIO_LEVEL_EXT_HI_level_MASK                           0xffffffff
#define BCHP_GIO_LEVEL_EXT_HI_level_SHIFT                          0

/***************************************************************************
 *STAT_EXT_HI - GENERAL PURPOSE I/O INTERRUPT STATUS [116:96] (GPIO[110:90])
 ***************************************************************************/
/* GIO :: STAT_EXT_HI :: irq_status [31:00] */
#define BCHP_GIO_STAT_EXT_HI_irq_status_MASK                       0xffffffff
#define BCHP_GIO_STAT_EXT_HI_irq_status_SHIFT                      0

#endif /* #ifndef BCHP_GIO_H__ */

/* End of File */
