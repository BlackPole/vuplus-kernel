/*
 * Copyright (C) 2009 Broadcom Corporation
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
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/cpumask.h>
#include <linux/smp.h>
#include <linux/io.h>
#include <linux/bitops.h>

#include <asm/irq.h>
#include <asm/mipsregs.h>
#include <asm/addrspace.h>
#include <asm/irq_cpu.h>
#include <asm/brcmstb/brcmstb.h>

#ifdef CONFIG_SMP
static int next_cpu[NR_IRQS] = { [0 ... NR_IRQS-1] = 0 };
#define NEXT_CPU(irq) next_cpu[irq]

#define TP0_BASE BCHP_HIF_CPU_INTR1_REG_START
#define TP1_BASE BCHP_HIF_CPU_TP1_INTR1_REG_START
#define L1_WR_ALL(word, reg, val) do { \
	L1_WR_##word(TP0_BASE, reg, val); \
	if (cpu_online(1)) \
		L1_WR_##word(TP1_BASE, reg, val); \
	} while (0)

#else

#define TP0_BASE BCHP_HIF_CPU_INTR1_REG_START
#define TP1_BASE TP0_BASE
#define L1_WR_ALL(word, reg, val) do { \
	L1_WR_##word(TP0_BASE, reg, val); \
	} while (0)

#define NEXT_CPU(irq) 0
#endif

#define L1_REG(base, off) ((base) + (off) - BCHP_HIF_CPU_INTR1_REG_START)

#define L1_RD_W0(base, reg) \
	BDEV_RD(L1_REG(base, BCHP_HIF_CPU_INTR1_INTR_W0_##reg))
#define L1_RD_W1(base, reg) \
	BDEV_RD(L1_REG(base, BCHP_HIF_CPU_INTR1_INTR_W1_##reg))

#define L1_WR_W0(base, reg, val) \
	BDEV_WR_RB(L1_REG(base, BCHP_HIF_CPU_INTR1_INTR_W0_##reg), val)
#define L1_WR_W1(base, reg, val) \
	BDEV_WR_RB(L1_REG(base, BCHP_HIF_CPU_INTR1_INTR_W1_##reg), val)

#if defined(BCHP_HIF_CPU_INTR1_INTR_W2_STATUS)

#define L1_RD_W2(base, reg) \
	BDEV_RD(L1_REG(base, BCHP_HIF_CPU_INTR1_INTR_W2_##reg))
#define L1_WR_W2(base, reg, val) \
	BDEV_WR_RB(L1_REG(base, BCHP_HIF_CPU_INTR1_INTR_W2_##reg), val)

#else
/* nop on chips with only 64 L1 interrupts */
#define L1_RD_W2(base, reg)	0
#define L1_WR_W2(base, reg, val) do { } while (0)
#endif

/*
 * For interrupt map, see include/asm-mips/brcmstb/<plat>/bcmintrnum.h
 */

/***********************************************************************
 * INTC (aka L1 interrupt) functions
 ***********************************************************************/

static void brcm_intc_enable(struct irq_data *data)
{
	unsigned int shift;
	unsigned int irq = data->irq;
	unsigned long base = NEXT_CPU(irq) ? TP1_BASE : TP0_BASE;

	if (irq > 0 && irq <= 32) {
		shift = irq - 1;
		L1_WR_W0(base, MASK_CLEAR, (1UL << shift));
	} else if (irq > 32 && irq <= 32+32) {
		shift = irq - 32 - 1;
		L1_WR_W1(base, MASK_CLEAR, (1UL << shift));
	} else if (irq > 64 && irq <= 32+32+32) {
		shift = irq - 64 - 1;
		L1_WR_W2(base, MASK_CLEAR, (1UL << shift));
	} else
		BUG();
}

static void brcm_intc_disable(struct irq_data *data)
{
	unsigned int shift;
	unsigned int irq = data->irq;

	if (irq > 0 && irq <= 32) {
		shift = irq - 1;
		L1_WR_ALL(W0, MASK_SET, (1UL << shift));
	} else if (irq > 32 && irq <= 32+32) {
		shift = irq - 32 - 1;
		L1_WR_ALL(W1, MASK_SET, (1UL << shift));
	} else if (irq > 64 && irq <= 32+32+32) {
		shift = irq - 64 - 1;
		L1_WR_ALL(W2, MASK_SET, (1UL << shift));
	} else
		BUG();
}

#ifdef CONFIG_SMP
static int brcm_intc_set_affinity(struct irq_data *data, const struct cpumask *dest, bool force)
{
	unsigned int shift;
	unsigned long flags;
	unsigned int irq = data->irq;

	local_irq_save(flags);
	if (irq > 0 && irq <= 32) {
		shift = irq - 1;

		if (cpu_isset(0, *dest)) {
			L1_WR_W0(TP1_BASE, MASK_SET, (1UL << shift));
			L1_WR_W0(TP0_BASE, MASK_CLEAR, (1UL << shift));
			next_cpu[irq] = 0;
		} else {
			L1_WR_W0(TP0_BASE, MASK_SET, (1UL << shift));
			L1_WR_W0(TP1_BASE, MASK_CLEAR, (1UL << shift));
			next_cpu[irq] = 1;
		}
	} else if (irq > 32 && irq <= 64) {
		shift = irq - 32 - 1;
		next_cpu[irq] = 0;

		if (cpu_isset(0, *dest)) {
			L1_WR_W1(TP1_BASE, MASK_SET, (1UL << shift));
			L1_WR_W1(TP0_BASE, MASK_CLEAR, (1UL << shift));
			next_cpu[irq] = 0;
		} else {
			L1_WR_W1(TP0_BASE, MASK_SET, (1UL << shift));
			L1_WR_W1(TP1_BASE, MASK_CLEAR, (1UL << shift));
			next_cpu[irq] = 1;
		}
	} else if (irq > 64 && irq <= 96) {
		shift = irq - 64 - 1;
		next_cpu[irq] = 0;

		if (cpu_isset(0, *dest)) {
			L1_WR_W2(TP1_BASE, MASK_SET, (1UL << shift));
			L1_WR_W2(TP0_BASE, MASK_CLEAR, (1UL << shift));
			next_cpu[irq] = 0;
		} else {
			L1_WR_W2(TP0_BASE, MASK_SET, (1UL << shift));
			L1_WR_W2(TP1_BASE, MASK_CLEAR, (1UL << shift));
			next_cpu[irq] = 1;
		}
	}
	local_irq_restore(flags);
	return 0;
}
#endif /* CONFIG_SMP */

/*
 * THT: These INTC disable the interrupt before calling the IRQ handle_irq
 */
static struct irq_chip brcm_intc_type = {
	.name			= "BRCM L1",
	.irq_ack			= brcm_intc_disable,
	.irq_mask			= brcm_intc_disable,
	.irq_mask_ack		= brcm_intc_disable,
	.irq_unmask			= brcm_intc_enable,
#ifdef CONFIG_SMP
	.irq_set_affinity		= brcm_intc_set_affinity,
#endif /* CONFIG_SMP */
	NULL
};

/*
 * Move the interrupt to the other TP, to balance load (if affinity permits)
 */
static void flip_tp(int irq)
{
#ifdef CONFIG_SMP
	int tp = smp_processor_id();
	unsigned long local_lev1, remote_lev1;
	unsigned long mask = 1 << ((irq - 1) & 0x1f);

	if (tp == 0) {
		local_lev1 = TP0_BASE;
		remote_lev1 = TP1_BASE;
	} else {
		local_lev1 = TP1_BASE;
		remote_lev1 = TP0_BASE;
	}

	if (cpumask_test_cpu(tp ^ 1, irq_desc[irq].irq_data.affinity)) {
		next_cpu[irq] = tp ^ 1;
		if (irq >= 1 && irq <= 32) {
			L1_WR_W0(local_lev1, MASK_SET, mask);
			L1_WR_W0(remote_lev1, MASK_CLEAR, mask);
		}
		if (irq >= 33 && irq <= 64) {
			L1_WR_W1(local_lev1, MASK_SET, mask);
			L1_WR_W1(remote_lev1, MASK_CLEAR, mask);
		}
		if (irq >= 65 && irq <= 96) {
			L1_WR_W2(local_lev1, MASK_SET, mask);
			L1_WR_W2(remote_lev1, MASK_CLEAR, mask);
		}
	}
#endif /* CONFIG_SMP */
}

static void brcm_intc_dispatch(unsigned long base)
{
	u32 pend, shift;

	pend = L1_RD_W0(base, STATUS) & ~L1_RD_W0(base, MASK_STATUS);
	while ((shift = ffs(pend)) != 0) {
		pend ^= (1 << (shift - 1));
		do_IRQ(shift);
		flip_tp(shift);
	}

	pend = L1_RD_W1(base, STATUS) & ~L1_RD_W1(base, MASK_STATUS);
	while ((shift = ffs(pend)) != 0) {
		pend ^= (1 << (shift - 1));
		shift += 32;
		do_IRQ(shift);
		flip_tp(shift);
	}
	pend = L1_RD_W2(base, STATUS) & ~L1_RD_W2(base, MASK_STATUS);
	while ((shift = ffs(pend)) != 0) {
		pend ^= (1 << (shift - 1));
		shift += 64;
		do_IRQ(shift);
		flip_tp(shift);
	}
}

/* IRQ2 = L1 interrupt for TP0 */
static void brcm_mips_int2_dispatch(void)
{
	clear_c0_status(STATUSF_IP2);
	brcm_intc_dispatch(TP0_BASE);
	set_c0_status(STATUSF_IP2);
}

#ifdef CONFIG_SMP
/* IRQ3 = L1 interrupt for TP1 */
static void brcm_mips_int3_dispatch(void)
{
	clear_c0_status(STATUSF_IP3);
	brcm_intc_dispatch(TP1_BASE);
	set_c0_status(STATUSF_IP3);
}
#endif

#ifdef CONFIG_BRCM_SHARED_UART_IRQ

/***********************************************************************
 * Support for UPG L2 controller (7400 and older)
 *
 * On newer chips the UARTs have dedicated L1 interrupts, so Linux can
 * just ignore this controller.
 ***********************************************************************/

static irqreturn_t brcm_upg_interrupt(int irq, void *dev_id)
{
	unsigned long pend, shift;

	pend = BDEV_RD(BCHP_IRQ0_IRQSTAT) & BDEV_RD(BCHP_IRQ0_IRQEN);
	while ((shift = ffs(pend)) != 0) {
		pend ^= (1 << (shift - 1));
		do_IRQ(shift + BRCM_UPG_L2_BASE - 1);
	}
	return IRQ_HANDLED;
}

static void brcm_upg_enable(unsigned int irq)
{
	irq -= BRCM_UPG_L2_BASE;
	BUG_ON(irq >= 32);

	BDEV_SET(BCHP_IRQ0_IRQEN, (1UL << irq));
	BDEV_RD(BCHP_IRQ0_IRQEN);
}

static void brcm_upg_disable(unsigned int irq)
{
	irq -= BRCM_UPG_L2_BASE;
	BUG_ON(irq >= 32);

	BDEV_UNSET(BCHP_IRQ0_IRQEN, irq);
	BDEV_RD(BCHP_IRQ0_IRQEN);
}

static struct irq_chip brcm_upg_type = {
	.name			= "BRCM UPG L2",
	.ack			= brcm_upg_disable,
	.mask			= brcm_upg_disable,
	.mask_ack		= brcm_upg_disable,
	.unmask			= brcm_upg_enable,
	NULL
};

#endif /* CONFIG_BRCM_SHARED_UART_IRQ */

/***********************************************************************
 * IRQ setup / dispatch
 ***********************************************************************/

void __init arch_init_irq(void)
{
	int irq;

	mips_cpu_irq_init();

	L1_WR_ALL(W0, MASK_SET, 0xffffffff);
	L1_WR_ALL(W1, MASK_SET, 0xffffffff);
	L1_WR_ALL(W2, MASK_SET, 0xffffffff);

	clear_c0_status(ST0_IE | ST0_IM);

	/* Set up all L1 IRQs */
	for (irq = 1; irq < BRCM_VIRTIRQ_BASE; irq++)
		irq_set_chip_and_handler(irq, &brcm_intc_type,
			handle_level_irq);

#if defined(CONFIG_SMP) && defined(CONFIG_GENERIC_HARDIRQS)
	/* default affinity: 1 (TP0 only) */
	cpumask_clear(irq_default_affinity);
	cpumask_set_cpu(0, irq_default_affinity);
#endif

	/* enable IRQ2 (this runs on TP0).  IRQ3 enabled during TP1 boot. */
	set_c0_status(STATUSF_IP2);

#if !defined(CONFIG_BRCM_SHARED_UART_IRQ)

	/* enable non-shared UART interrupts in the L2 */

#if defined(BCHP_IRQ0_UART_IRQEN_uarta_MASK)
	/* 3548 style - separate register */
	BDEV_WR(BCHP_IRQ0_UART_IRQEN, BCHP_IRQ0_UART_IRQEN_uarta_MASK |
		BCHP_IRQ0_UART_IRQEN_uartb_MASK |
		BCHP_IRQ0_UART_IRQEN_uartc_MASK);
	BDEV_WR(BCHP_IRQ0_IRQEN, 0);
#elif defined(BCHP_IRQ0_IRQEN_uarta_irqen_MASK)
	/* 7405 style - shared with L2 */
	BDEV_WR(BCHP_IRQ0_IRQEN, BCHP_IRQ0_IRQEN_uarta_irqen_MASK
		| BCHP_IRQ0_IRQEN_uartb_irqen_MASK
#if defined(BCHP_IRQ0_IRQEN_uartc_irqen_MASK)
		| BCHP_IRQ0_IRQEN_uartc_irqen_MASK
#endif
		);
#endif

#if defined(CONFIG_BRCM_HAS_PCU_UARTS)
	BDEV_WR(BCHP_TVM_MAIN_INT_CNTL, 0);
	BDEV_WR_F(TVM_MAIN_INT_CNTL, MAIN_UART1_INT_EN, 1);
#endif

#else /* CONFIG_BRCM_SHARED_UART_IRQ */

	/* Set up all UPG L2 interrupts */

	BDEV_WR_RB(BCHP_IRQ0_IRQEN, 0);
	for (irq = BRCM_UPG_L2_BASE; irq <= BRCM_UPG_L2_LAST; irq++)
		irq_set_chip_and_handler(irq, &brcm_upg_type, handle_level_irq);

#endif /* CONFIG_BRCM_SHARED_UART_IRQ */

#if defined(BCHP_HIF_INTR2_CPU_MASK_SET)
	/* mask and clear all HIF L2 interrupts */
	BDEV_WR_RB(BCHP_HIF_INTR2_CPU_MASK_SET, 0xffffffff);
	BDEV_WR_RB(BCHP_HIF_INTR2_CPU_CLEAR, 0xffffffff);
#endif
}

#ifdef CONFIG_BRCM_SHARED_UART_IRQ
static int brcm_setup_upg_irq(void)
{
	int ret;

	ret = request_irq(BRCM_IRQ_UPG, brcm_upg_interrupt,
		0, "brcm_shared_upg", NULL);
	if (ret)
		printk(KERN_ERR "error: can't request UPG interrupt\n");
	return ret;
}
core_initcall(brcm_setup_upg_irq);
#endif

asmlinkage void plat_irq_dispatch(void)
{
	unsigned int pend = ((read_c0_cause() & read_c0_status()) >> 8) & 0xff;
	unsigned int shift;

	while ((shift = ffs(pend)) != 0) {
		shift--;
		pend ^= 1 << shift;
		if (shift == 2)
			brcm_mips_int2_dispatch();
#ifdef CONFIG_SMP
		else if (unlikely(shift == 3))
			brcm_mips_int3_dispatch();
#endif
		else
			do_IRQ(MIPS_CPU_IRQ_BASE + shift);
	}
}

/***********************************************************************
 * Power management
 ***********************************************************************/

static unsigned long brcm_irq_state[3];

void brcm_irq_standby_enter(int wake_irq)
{
	/* save the current state, then mask everything */
	brcm_irq_state[0] = L1_RD_W0(TP0_BASE, MASK_STATUS);
	L1_WR_W0(TP0_BASE, MASK_SET, 0xffffffff);
	brcm_irq_state[1] = L1_RD_W1(TP0_BASE, MASK_STATUS);
	L1_WR_W1(TP0_BASE, MASK_SET, 0xffffffff);
	brcm_irq_state[2] = L1_RD_W2(TP0_BASE, MASK_STATUS);
	L1_WR_W2(TP0_BASE, MASK_SET, 0xffffffff);

	/* unmask the wakeup IRQ */
	if (wake_irq > 0 && wake_irq <= 32)
		L1_WR_W0(TP0_BASE, MASK_CLEAR, 1 << (wake_irq - 1));
	else if (wake_irq > 32 && wake_irq <= 64)
		L1_WR_W1(TP0_BASE, MASK_CLEAR, 1 << (wake_irq - 33));
	else if (wake_irq > 64 && wake_irq <= 96)
		L1_WR_W2(TP0_BASE, MASK_CLEAR, 1 << (wake_irq - 65));
}

void brcm_irq_standby_exit(void)
{
	/* restore the saved L1 state */
	L1_WR_W0(TP0_BASE, MASK_SET, 0xffffffff);
	L1_WR_W0(TP0_BASE, MASK_CLEAR, ~brcm_irq_state[0]);
	L1_WR_W1(TP0_BASE, MASK_SET, 0xffffffff);
	L1_WR_W1(TP0_BASE, MASK_CLEAR, ~brcm_irq_state[1]);
	L1_WR_W2(TP0_BASE, MASK_SET, 0xffffffff);
	L1_WR_W2(TP0_BASE, MASK_CLEAR, ~brcm_irq_state[2]);
}
