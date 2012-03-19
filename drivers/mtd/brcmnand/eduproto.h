/*
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
 * @file eduproto.h
 * @author Jean Roberge
 *
 * @brief Prototypes for EDU Support Software
 */

#ifndef _EDUPROTO_H_
#define _EDUPROTO_H_

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/init.h>

#include <linux/jiffies.h>

#include <linux/byteorder/generic.h>
#include <linux/timer.h>

#include <asm/io.h>
#include <asm/bug.h>

#include "brcmnand.h"
#include "edu.h"	// For EDU_USE_ISR

/******************************************************************************
* Prototyping
*******************************************************************************/
extern uint32_t tb_r(uint32_t);
extern void tb_w(uint32_t, uint32_t);
extern uint32_t tb_poll(uint32_t, uint32_t, uint32_t);
extern int byteSmoosh(int);

extern void EDU_issue_command(uint32_t, uint32_t, uint8_t);
extern void get_edu_status(void);
extern void edu_check_data(uint32_t, uint32_t, uint32_t);


extern void EDU_initialize_timer(void);
extern void EDU_start_timer(void);
extern void EDU_stop_timer(void);
extern void EDU_print_results(unsigned long data_size);
extern void EDU_get_status(void);

static inline uint32_t EDU_volatileRead(uint32_t reg) { return BDEV_RD(reg); }
static inline void EDU_volatileWrite(uint32_t reg, uint32_t val) { BDEV_WR(reg, val); }

#ifdef CONFIG_MTD_BRCMNAND_USE_ISR
extern uint32_t ISR_wait_for_completion(void);

#else
extern uint32_t EDU_poll(uint32_t, uint32_t, uint32_t, uint32_t);
#endif


extern void EDU_init(void);
extern int EDU_write(volatile const void*, uint32_t, uint32_t*);
extern int EDU_read(volatile void*, uint32_t);

extern uint32_t EDU_get_error_status_register(void);

// Returns 0 on Done, 1 on Timeout
extern int EDU_poll_for_done(void);

// THT: Write until done clears
extern void EDU_reset_done(void);

extern void EDU_waitForNoPendingAndActiveBit(void);

#endif // _EDUPROTO_H_
