 /*
 * drivers/mtd/brcmnand/edu.c
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
 * @file edu.c
 * @author Jean Roberge
 */

 
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/jiffies.h>
#include <linux/delay.h>

#include <linux/byteorder/generic.h>

/* THT: Needed for VM addresses */


#include <linux/mm.h>
#include <linux/dma-mapping.h>
#include <asm/page.h>

#include <asm/io.h>
#include <asm/bug.h>

#include "edu.h"
#include "eduproto.h"
#include "brcmnand.h"
#include "brcmnand_priv.h"

//#define EDU_DEBUG
#undef EDU_DEBUG

#ifdef EDU_DEBUG
int edu_debug;
#define PRINTK printk

#else
int edu_debug = 0;
#define PRINTK(...)
#endif


extern int gdebug;



// Debugging 3548
#ifdef CONFIG_BCM3548

#define MEMC_0_L2_R5F_STATUS 			((volatile unsigned long*) 0xb0164000)
#define MEMC_0_L2_R5F_MASK_STATUS 	((volatile unsigned long*) 0xb016400c)
#define MEMC_0_L2_PCI_STATUS 			((volatile unsigned long*) 0xb0164018)
#define MEMC_0_L2_PCI_MASK_STATUS 	((volatile unsigned long*) 0xb0164024)


volatile unsigned long g_MEMC_0_L2_R5F_STATUS;
volatile unsigned long g_MEMC_0_L2_R5F_MASK_STATUS;
volatile unsigned long g_MEMC_0_L2_PCI_STATUS;
volatile unsigned long g_MEMC_0_L2_PCI_MASK_STATUS;


int DisplayMemDebug(void) 
{
	volatile unsigned long _MEMC_0_L2_R5F_STATUS;
	volatile unsigned long _MEMC_0_L2_R5F_MASK_STATUS;
	volatile unsigned long _MEMC_0_L2_PCI_STATUS;
	volatile unsigned long _MEMC_0_L2_PCI_MASK_STATUS;
	int ret = 0;

	_MEMC_0_L2_R5F_STATUS = *MEMC_0_L2_R5F_STATUS;
	_MEMC_0_L2_R5F_MASK_STATUS = *MEMC_0_L2_R5F_MASK_STATUS;
	_MEMC_0_L2_PCI_STATUS = *MEMC_0_L2_PCI_STATUS;
	_MEMC_0_L2_PCI_MASK_STATUS = *MEMC_0_L2_PCI_MASK_STATUS;

	if (g_MEMC_0_L2_R5F_STATUS != _MEMC_0_L2_R5F_STATUS ||
		g_MEMC_0_L2_R5F_MASK_STATUS !=_MEMC_0_L2_R5F_MASK_STATUS ||
		g_MEMC_0_L2_PCI_STATUS !=_MEMC_0_L2_PCI_STATUS ||
		g_MEMC_0_L2_PCI_MASK_STATUS != _MEMC_0_L2_PCI_MASK_STATUS)
	{
		printk("rf5s=%08lx, rf5m=%08lx, pcis=%08lx,pcim=%08lx\nprev: rf5s=%08lx, rf5m=%08lx, pcis=%08lx,pcim=%08lx\n", 
			_MEMC_0_L2_R5F_STATUS,
			_MEMC_0_L2_R5F_MASK_STATUS,
			_MEMC_0_L2_PCI_STATUS,
			_MEMC_0_L2_PCI_MASK_STATUS,
			g_MEMC_0_L2_R5F_STATUS,
			g_MEMC_0_L2_R5F_MASK_STATUS,
			g_MEMC_0_L2_PCI_STATUS,
			g_MEMC_0_L2_PCI_MASK_STATUS);
		
		g_MEMC_0_L2_R5F_STATUS = _MEMC_0_L2_R5F_STATUS; 
		g_MEMC_0_L2_R5F_MASK_STATUS =_MEMC_0_L2_R5F_MASK_STATUS;
		g_MEMC_0_L2_PCI_STATUS =_MEMC_0_L2_PCI_STATUS;
		g_MEMC_0_L2_PCI_MASK_STATUS = _MEMC_0_L2_PCI_MASK_STATUS;
		ret = 1;
	}
	return ret;
}

#else

#define DisplayMemDebug(...) (0)

#endif

/*
 * Returns 1 if OK
 *		0 otherwise
 */
int EDU_buffer_OK(volatile void* vaddr, int command)
{
	unsigned long addr = (unsigned long) vaddr;

// Andover architecture do not use SCB protocol.  EDU SCB conformance fixed in 3.3 or later (7420cx)
#if !(defined(CONFIG_BCM7440) || defined(CONFIG_BCM7601) || defined(CONFIG_BCM7635)\
	|| (CONFIG_MTD_BRCMNAND_VERSION >= CONFIG_MTD_BRCMNAND_VERS_3_3))
      
// Requires 32byte alignment only of platforms other than 7440 and 7601 (and Dune)
	if (addr & 0x1f) {
		// Must be 32-byte-aligned
#ifdef EDU_DEBUG 
// Trying to catch where alignment need to be fixed
printk("Buffer at %p not aligned on 32B boundary: Calling Seq=\n", vaddr);
dump_stack();
#endif
		return 0;
	}
#else
	// Only require alignment on 4 bytes
	if (addr & 0x03) {
		return 0;
	}
#endif

	// TBD: Since we only enable block for MEM0, we should make sure that the physical
	// address falls in MEM0.
	
	else if (addr >= VMALLOC_START && addr < VMALLOC_END) {
		// VM Address
		return 0;
	}

	else if ((addr & 0xe0000000) != KSEG0) { 
		// !KSEG 0
		return 0;
	}


#if 0 //def CONFIG_BCM7420
	else if (command == EDU_WRITE && (addr & 0xff)) { // Write must be aligned on 256B
printk("Write must be aligned on 128B (addr=%08x)\n", addr);
		return 0;
	}
#endif

	// OK to proceed with EDU
	return 1;
}


/*
 * THT: Just calling virt_to_phys() will not work on VM allocated addresses (0xC000_0000-0xCFFF_FFFF)
 * On the other hand, we cannot call kmalloc to allocate the BBT, because of its size.
 */

static unsigned long __maybe_unused EDU_virt_to_phys(volatile void* vaddr)
{
	unsigned long addr = (unsigned long) vaddr;
	unsigned long paddr;
static unsigned long save_addr;

if (edu_debug > 3) printk("-->%s: addr=%08lx\n", __FUNCTION__, addr);
	if (!(addr & KSEG0)) { 
		printk(KERN_ERR "brcmnand EDU: User Space buffers %08lx are not currently supported\n", addr);
		/* THT: Note to self: http://lwn.net/Articles/28548/ */
		BUG();
		goto error_out;
	}
	
	/* If not VM addresses, use the regular function */
	else if (addr < VMALLOC_START || addr > VMALLOC_END) {

	// TBD: Since we only enable block for MEM0 (see EDU_init()), we should make sure that the physical
	// address falls in MEM0.
		paddr =  virt_to_phys(vaddr);

if ((edu_debug > 3) /*|| ((0 == (addr & 0xFFF)) && save_addr != addr) */) 
{printk("paddr= virt_to_phys(vaddr=%08x) = %08lx\n", (unsigned int)addr, (unsigned long)paddr);
}
if (edu_debug > 3) {show_stack(current,NULL);dump_stack();}

save_addr = addr;
		
	}
	
	// Buffers allocated by vmalloc(): We have to find the physical page, error out for now */
	else {
#if 1
		printk(KERN_ERR "%s: Cannot use vmalloc() memory for DMA, addr=%p\n", __FUNCTION__, vaddr);
		BUG();
		goto error_out;
#else
/* ******************** WARNINGS ****************** * 
 *	Cannot use these codes unless we lock down the page (do_mlock())
 *    We also don't know if EDU can DMA over the page.
 *************************************************/
		unsigned long start; // Page start address
		unsigned long pa = 0UL;
		unsigned long pageOffset; // Offset from page start address
		//struct vm_area_struct* vma;
		pgd_t *pgd;
		pud_t *pud;
		pmd_t *pmd;
		pte_t *pte;
		struct page *page;
		unsigned long pfn;
		
		unsigned long flags;

		start = (addr & PAGE_MASK);
		pageOffset = addr - start;
if (edu_debug > 3) printk("--> %s start=%08lx\n", __FUNCTION__, start);
//		vma = find_vma(current->mm, start);
		local_irq_save(flags);
//PRINTK("find_vma returns %p\n", vma);

//PRINTK("Calling pgd_offset, current-=%p\n", current);
//PRINTK("Calling pgd_offset, current->mm=%p\n", current->mm);
		if (current->mm) {
			pgd = pgd_offset(current->mm, start);
		}
		else {
			// If not find it in the kernel page table
			pgd = pgd_offset_k(start);
		}
//PRINTK("pgd=%08x\n", pgd->pgd);
		pud = pud_offset(pgd, start);
//PRINTK("pud=%08x\n", pud->pgd);
		pmd = pmd_offset(pud, start);
//PRINTK("pnd=%08x\n", pmd->pud.pgd);
		pte = pte_offset(pmd, start);

#if 0
PRINTK("Calling vm_normal_page, pte=%08lx\n", pte->pte);
		page = vm_normal_page(vma, start, *pte);

PRINTK("page=%p\n", page);
		if (page) {
			// get_page(page);
			pa = page_to_phys(page);
PRINTK("PA=%08lx\n", pa);
		}
		else {
			PRINTK(KERN_ERR "brcmnand EDU: Unable to find page mapped to %08lx\n", addr);
			goto error_out;
		}
#else
//PRINTK("Calling pte_pfn(pte=%08lx)\n", pte->pte);
		pfn = pte_pfn(*pte);
//PRINTK("pfn=%08lx\n", pfn);
		pa = pfn << PAGE_SHIFT;

/* ******************** WARNINGS ****************** * 
 *	Cannot use this codes unless we lock down the page
 *************************************************/

#endif

		local_irq_restore(flags);

		paddr =  (pa + pageOffset);

if (edu_debug > 3) printk("paddr= VMA() = %08lx\n", paddr);
#endif // if VM address
	}

	return paddr;



error_out:
	return 0UL;


}



/*************************************** Internals *******************************************/
#ifdef EDU_DEBUG
void EDU_get_status(void)
{
    uint32_t rd_data;
    rd_data = 0;

    rd_data = EDU_volatileRead(EDU_BASE_ADDRESS  + EDU_DONE);
    printk("INFO: Done count = 0x%08x\n",rd_data);

    rd_data = EDU_volatileRead(EDU_BASE_ADDRESS  + EDU_STATUS);
    printk("INFO: EDU status = 0x%08x\n",rd_data);
    printk("INFO: \t bit1 = pending\n");
    printk("INFO: \t bit0 = active\n");

    rd_data = EDU_volatileRead(EDU_BASE_ADDRESS  + EDU_STOP);
    printk("INFO: Stop reg = 0x%08x\n",rd_data);

    rd_data = EDU_volatileRead(EDU_BASE_ADDRESS + EDU_ERR_STATUS);
    printk("INFO: EDU ERR status = 0x%08x\n",rd_data);
    printk("INFO: \t bit3 = NandWrErr\n");
    printk("INFO: \t bit2 = NandEccUncor\n");
    printk("INFO: \t bit1 = NandEccCor\n");
    printk("INFO: \t bit0 = ErrAck\n");

    rd_data = EDU_volatileRead(EDU_BASE_ADDRESS + BCHP_NAND_ECC_CORR_ADDR);
    printk("INFO: NAND CTRL BCHP_NAND_ECC_CORR_ADDR = 0x%08x\n",rd_data);

    rd_data = EDU_volatileRead(EDU_BASE_ADDRESS + BCHP_NAND_ECC_UNC_ADDR);
    printk("INFO: NAND CTRL BCHP_NAND_ECC_UNC_ADDR = 0x%08x\n",rd_data);

    rd_data = EDU_volatileRead(EDU_BASE_ADDRESS + BCHP_NAND_INTFC_STATUS) & 0xf00000ff;
    printk("INFO: NAND CTRL BCHP_NAND_INTFC_STATUS = 0x%08x\n",rd_data);
}

#else
#define EDU_get_status()
#endif

void EDU_waitForNoPendingAndActiveBit(void)
{
        volatile uint32_t rd_data=0, i=0; 
        unsigned long timeout;

//int saveDbgLvl = edu_debug;

        //PRINTK("Start Polling!\n");
        __sync();
        rd_data = EDU_volatileRead(EDU_STATUS);

//edu_debug = 0;
        timeout = jiffies + msecs_to_jiffies(3000); // 3 sec timeout for now (testing)
        while ((rd_data & 0x00000003) != 0x00000000) /* && (i<cnt) */ 
        {
         
                __sync(); //PLATFORM_IOFLUSH_WAR();
                rd_data = EDU_volatileRead(EDU_STATUS);
                i++;
                if(!time_before(jiffies, timeout))
                {
                   PRINTK("EDU_waitForNoPendingAndActiveBit timeout at 3 SECONDS with i= 0x%.08x!\n", (int)i);
//edu_debug = saveDbgLvl;
                   return;
                }
        }
//edu_debug = saveDbgLvl;
        return;
}

// THT: Write until done clears
void EDU_reset_done(void)
{
	volatile uint32_t rd_data;

//int saveDbgLvl = edu_debug;

	rd_data = EDU_volatileRead(EDU_DONE);

//edu_debug = 0;
	while (rd_data & 0x3) {		
		// Each Write decrement DONE by 1
		EDU_volatileWrite(EDU_DONE, 0);
		__sync();
		rd_data = EDU_volatileRead(EDU_DONE);
	} 
//edu_debug = saveDbgLvl;
}

#if 0 //def EDU_DEBUG  // DO NOT DELETE, MAY BE USEFUL!!!

void print_NandCtrl_Status(void);

// Returns 0 on Done, 1 on Timeout
int EDU_poll_for_done(void)
{
        uint32_t rd_data=0, i=0; 
        unsigned long timeout;

int saveDbgLvl = edu_debug;

        __sync();
        rd_data = EDU_volatileRead(EDU_BASE_ADDRESS  + EDU_DONE);

//edu_debug = 0;

        timeout = jiffies + msecs_to_jiffies(3000); // 3 sec timeout for now (testing)
        while ((rd_data & 0x00000003) == 0x00000000) 
        {
         
                __sync(); //PLATFORM_IOFLUSH_WAR();
                rd_data = EDU_volatileRead(EDU_BASE_ADDRESS  + EDU_DONE);
                i++;
                if(!time_before(jiffies, timeout))
                {
                   PRINTK("EDU_poll_for_done timeout at 3 SECONDS with i= 0x%.08x!\n", (int)i);
//edu_debug = saveDbgLvl;
                   return 1;
                }
        }
//edu_debug = saveDbgLvl;
        return 0;
}






void EDU_checkRegistersValidity(uint32_t external_physical_device_address)
{
       uint32_t result;

        result = EDU_volatileRead(EDU_BASE_ADDRESS + BCHP_NAND_INTFC_STATUS);
        
        result = result & 0xf0000000;        

        if( result != 0xf0000000)
        {
                printk("NAND Status bits are NOT VALID!!\n");
        }

        else if( EDU_volatileRead(EDU_BASE_ADDRESS + BCHP_NAND_CMD_ADDRESS) != external_physical_device_address)
        {
                printk("BCHP_NAND_CMD_ADDRESS not good!\n");
        }

        else if( EDU_volatileRead(EDU_BASE_ADDRESS + BCHP_NAND_CMD_START) != 0x01000000)
        {
                printk("BCHP_NAND_CMD_START not good!\n");
        }

        else if( EDU_volatileRead(EDU_BASE_ADDRESS + BCHP_NAND_SEMAPHORE) != 0x00000000)
        {
                printk("BCHP_NAND_SEMAPHORE not good!\n");
        }

        result = EDU_volatileRead(EDU_BASE_ADDRESS + EDU_ERR_STATUS) & 0x0000000f;
       
        if((result != 0x00000004) && (result != 0x00000000)) 
        {
                printk("EDU_ERR_STATUS = 0x%.08x NOT GOOD\n", result);
        }
}

uint16_t EDU_checkNandCacheAndBuffer(uint32_t buffer, int length)
{
    uint32_t  k;
    uint16_t    error = 0; /* no error */    
    uint32_t  rd_addr, exp_data, act_data;

       for (k=0; k < length; k=k+4) {
                rd_addr = buffer + k;
                if(EDU_volatileRead(EDU_BASE_ADDRESS + BCHP_NAND_FLASH_CACHEi_ARRAY_BASE + k) != EDU_volatileRead(rd_addr))
                {
                        error = 1;
                }
        }
        
     if(error == 1)
     {
         printk("ERROR: BAD DRAM EDU_checkNandCacheAndBuffer at address 0x%8x!!\n", (unsigned int)buffer);
         EDU_get_status();

     }
     else
     {
         //printk("TEST PASSED byteSmoosh at address 0x%8x!!\n", buffer);
     }   

     return error;
}

#endif


#ifndef CONFIG_MTD_BRCMNAND_USE_ISR

// 32-bit register polling
// Poll a register until the reg has the expected value.
// a timeout read count. The value reflects how many reads
// the routine check the register before is gives up.
/*
 * THT: Changed to return if (data & mask) != 0
 * This way, we can return faster if there is a (un) correctable error.
 *
 * returns 0 on timedout,
 * Read data on success or error.
 */

extern void 
dump_nand_regs(struct brcmnand_chip* chip, loff_t offset, uint32_t pa, int which);
#define MAX_DUMPS 10
extern int numDumps;

uint32_t EDU_poll(uint32_t address, uint32_t expect, uint32_t error, uint32_t mask)
{
        uint32_t rd_data=0, i=0; 
	int ret;
        unsigned long timeout;
	 //int retry = 0;
int saveDbgLvl = edu_debug;
        
if (edu_debug)         PRINTK("Start Polling addr=%08x, expect=%08x, mask=%08x, error=%08x!\n", 
	address, expect, mask, error);
        __sync();
        rd_data = EDU_volatileRead(address);
if (numDumps < MAX_DUMPS)
 {
 dump_nand_regs(NULL, 0, 0, numDumps++);
 }
   
//edu_debug = 0;
	  
        timeout = jiffies + msecs_to_jiffies(1000); // 3 sec timeout for now (testing)
 // Testing 1 2 3       
// 	 while ( 0 == ((rd_data & mask) & (expect | error)) ) /* && (i<cnt) */ 
// 	 while ((rd_data & mask) != (expect & mask)) /* && (i<cnt) */
	 while (((rd_data & mask) != (expect & mask)) && !((rd_data & mask) & error))
        {

         	   if ( 0 /*(i %1000000) == 1 */) 
			   {PRINTK("Polling addr=%08x, expect=%08x, mask=%08x!\n", address, expect, mask);
			    PRINTK("EDU_poll read: %08x\n", rd_data);}
			  
                //__sync(); //PLATFORM_IOFLUSH_WAR();
                rd_data = EDU_volatileRead(address);

             // JR+ 2008-02-01 Allow other tasks to run while waiting
                //cond_resched();
                cond_resched();
                // JR- 2008-02-01 Allow other tasks to run while waiting
if (numDumps < MAX_DUMPS)
 {
 dump_nand_regs(NULL, 0, 0, numDumps++);
 }
                   
                i++;
                if(!time_before(jiffies, timeout))
                {
                   PRINTK("EDU_poll timeout at 3 SECONDS (just for testing) with i= 0x%.08x!\n", (int)i);
                   PRINTK("DBG> EDU_poll (A=0x%.08x, X=0x%.08x, E=0x%.08x, M=0x%.08x, R=0x%.08x;\n",
				   	address, expect, error, mask, rd_data);
			EDU_get_status();
                   return 0;
                }
        }
  
        //PRINTK("DBG> EDU_poll (0x%.08x, 0x%.08x, 0x%.08x);\n",address, expect, mask);
        //PRINTK("DBG> EDU_poll i= 0x%.08x!\n", i);
        //PRINTK("\n");
        //PRINTK("End Polling! Number of passes: %d\n", i);

	return rd_data;

}
#endif


void EDU_issue_command(uint32_t dram_addr, uint32_t ext_addr,uint8 cmd)
{



    EDU_volatileWrite(EDU_DRAM_ADDR, dram_addr);
    //EDU_volatileWrite(EDU_PATCH_GLOBAL_REG_RBUS_START + CPU_REGISTER_ADDRESS + EDU_DRAM_ADDR, dram_addr);        
    //PRINTK("\tINFO: EDU_DRAM_ADDR = 0x%08x\n",dram_addr);

    EDU_volatileWrite(EDU_EXT_ADDR, ext_addr);
    //EDU_volatileWrite(EDU_PATCH_GLOBAL_REG_RBUS_START + CPU_REGISTER_ADDRESS + EDU_EXT_ADDR, ext_addr);
    //PRINTK("\tINFO: EDU_EXT_ADDR = 0x%08x\n",ext_addr);

    EDU_volatileWrite(EDU_CMD, cmd);
    //EDU_volatileWrite(EDU_PATCH_GLOBAL_REG_RBUS_START + CPU_REGISTER_ADDRESS + EDU_CMD, cmd);
    //if (cmd == 1)
        //PRINTK("\tINFO: EDU_CMD = READ operation\n");
    //if (cmd == 0)
       //PRINTK("\tINFO: EDU_CMD = WRITE operation\n");
}



uint32_t EDU_get_error_status_register(void)
{
        uint32_t valueOfReg = EDU_volatileRead(EDU_ERR_STATUS);  

        // Clear the error
        EDU_volatileWrite(EDU_ERR_STATUS, 0x00000000);   

        return(valueOfReg);
}



void EDU_init(void)
{
	
printk(KERN_INFO "-->%s:\n", __FUNCTION__);

//edu_debug = 4;
        EDU_volatileWrite(EDU_CONFIG, EDU_CONFIG_VALUE);

        EDU_volatileWrite(EDU_LENGTH, EDU_LENGTH_VALUE);

 #ifdef CONFIG_BCM7440
 // THT: *** Caution: These hard-coded values only work on 7440bx
 
        // Writing to PCI control register (Init PCI Window is now Here)
        
	  // THT: PCI_GEN_GISB_WINDOW_SIZE = ENABLE_256MB_GISB_WINDOW
	  // Enable 256MB GISB Window to allow PCI to get on the EBI bus
        EDU_volatileWrite(0x128, 0x00000001);

 	  // THT: Initiate watchdog timeout for GISB Arbiter
 	  // SUN_GISB_ARB_TIMER = 0x10000
        EDU_volatileWrite(0x040600c, 0x00010000);

#elif defined( CONFIG_BCM7601 ) || defined( CONFIG_BCM7635 )
	{
#define ENABLE_256MB_GISB_WINDOW 0x1
		uint32_t PCI_GEN_GISB_WINDOW_SIZE = 0x0000011c;
		uint32_t SUN_GISB_ARB_TIMER = 0x0040600c;
        
	  	// THT: PCI_GEN_GISB_WINDOW_SIZE = ENABLE_256MB_GISB_WINDOW
	  	// Enable 256MB GISB Window to allow PCI to get on the EBI bus
        	EDU_volatileWrite(PCI_GEN_GISB_WINDOW_SIZE, ENABLE_256MB_GISB_WINDOW);

 	 	 // THT: Initiate watchdog timeout for GISB Arbiter
        	EDU_volatileWrite(SUN_GISB_ARB_TIMER, 0x00010000);
	}
		
#elif defined( CONFIG_BCM3548 )
	// Make sure that RTS grant some cycle to EDU, or we have to steal some
	{
		uint32_t MEMC_0_1_CLIENT_INFO_45 = 0x001610b8;

		/* Bits 08-20 are all 1 == Blocked */
		if ((BDEV_RD(MEMC_0_1_CLIENT_INFO_45) & 0x001fff00) == 0x001fff00) {
			printk("%s: MEMC_0_1_CLIENT_INFO_45 = %08lx overwritten.  Please fix your RTS\n", __FUNCTION__, 
				BDEV_RD(MEMC_0_1_CLIENT_INFO_45));
			BDEV_WR(MEMC_0_1_CLIENT_INFO_45, 0x001a92bc);
		}
	}

#elif defined( CONFIG_BCM7420 )
	// Make sure that RTS grants some cycle to EDU, or we have to steal some from RR
	{
#define BLOCKED_OUT 0x001fff00
#define RR_ENABLED	0x80   /* Bit 7 */
		uint32_t MEMC_0_1_CLIENT_INFO_17= 0x003c1048;
		volatile unsigned long memc_client_17;
#define ENABLE_256MB_GISB_WINDOW 0x1
		uint32_t PCI_GEN_GISB_WINDOW_SIZE = 0x0044011c;
		uint32_t SUN_GISB_ARB_TIMER = 0x0040000c;
#define PARK_ON_EBI (1 << 7)
#define PARK_ON_MASK (0xFE)
		uint32_t PCI_GEN_PCI_CTRL = 0x00440104;
		volatile unsigned long pci_gen_pci_ctrl;

        
		/* Bits 08-20 are all 1 == Blocked */
		memc_client_17 = BDEV_RD(MEMC_0_1_CLIENT_INFO_17);
		printk("MEMC_0_1_CLIENT_INFO_17 Before=%08lx\n", memc_client_17);
		if (((memc_client_17 & 0x001fff00) == 0x001fff00) && !(memc_client_17 & RR_ENABLED)) {
			printk("%s: MEMC_0_1_CLIENT_INFO_17 = %08lx overwritten.  Please fix your RTS\n", __FUNCTION__, 
				memc_client_17);
			BDEV_WR(MEMC_0_1_CLIENT_INFO_17, memc_client_17|RR_ENABLED);
			printk("MEMC_0_1_CLIENT_INFO_17 After=%08lx\n", BDEV_RD(MEMC_0_1_CLIENT_INFO_17));
		}

		  // THT: PCI_GEN_GISB_WINDOW_SIZE = ENABLE_256MB_GISB_WINDOW
		  // Enable 256MB GISB Window to allow PCI to get on the EBI bus
	        EDU_volatileWrite(PCI_GEN_GISB_WINDOW_SIZE, ENABLE_256MB_GISB_WINDOW);

	 	  // THT: Initiate watchdog timeout for GISB Arbiter
	 	  // SUN_GISB_ARB_TIMER = 0x10000
	        EDU_volatileWrite(SUN_GISB_ARB_TIMER, 0x00010000);

		  // Park PCI bus on EBI
		  pci_gen_pci_ctrl = BDEV_RD(PCI_GEN_PCI_CTRL);
		  pci_gen_pci_ctrl &= ~PARK_ON_MASK;
		  pci_gen_pci_ctrl |= PARK_ON_EBI;
		  EDU_volatileWrite(PCI_GEN_PCI_CTRL, pci_gen_pci_ctrl);
	}
#endif

DisplayMemDebug();

        // Clear the interrupt for next time
        EDU_volatileWrite(BCHP_HIF_INTR2_CPU_CLEAR, HIF_INTR2_EDU_CLEAR_MASK|HIF_INTR2_CTRL_READY); 
PRINTK("<--%s:\n", __FUNCTION__);

#ifdef CONFIG_MTD_BRCMNAND_USE_ISR
	ISR_init();
#endif


//edu_debug = 0;
}

#ifndef CONFIG_MTD_BRCMNAND_ISR_QUEUE // batch mode

/*
 * THT: 07/31/08: This does not work.  One has to write the 512B Array from the NAND controller into 
 * the EXT registers for it to work.  Will fix it when I come back.
 */
int EDU_write(volatile const void* virtual_addr_buffer, 
	uint32_t external_physical_device_address,
	uint32_t* physAddr)
{
	//uint32_t  phys_mem;
	// uint32_t  rd_data;
	//unsigned long flags;

edu_debug = gdebug;

#if 0
	phys_mem = EDU_virt_to_phys((void *)virtual_addr_buffer);

#else
	// EDU is not a PCI device
	// THT: TBD: Need to adjust for cache line size here, especially on 7420.
	*physAddr = dma_map_single(NULL, virtual_addr_buffer, EDU_LENGTH_VALUE, DMA_TO_DEVICE);
#endif

	if (!(*physAddr)) {
		return (-1);
	}

//edu_debug = 4;
	
//printk("EDU_write: vBuff: %p physDev: %08x, PA=%08x\n", 
//	virtual_addr_buffer, external_physical_device_address, phys_mem);

#ifdef CONFIG_MTD_BRCMNAND_USE_ISR
	down(&gEduIsrData.lock);
 	gEduIsrData.edu_ldw = external_physical_device_address;
 	gEduIsrData.physAddr = *physAddr;
	
	/*
	 * Enable L2 Interrupt
	 */
	gEduIsrData.cmd = EDU_WRITE;
	gEduIsrData.opComplete = ISR_OP_SUBMITTED;
	gEduIsrData.status = 0;
	
	/* On write we wait for both DMA done|error and Flash Status */
	gEduIsrData.mask = HIF_INTR2_EDU_CLEAR_MASK|HIF_INTR2_CTRL_READY;
	gEduIsrData.expect = HIF_INTR2_EDU_DONE;
	gEduIsrData.error = HIF_INTR2_EDU_ERR;
	gEduIsrData.intr = HIF_INTR2_EDU_DONE_MASK|HIF_INTR2_CTRL_READY;

	up(&gEduIsrData.lock);
	ISR_enable_irq(&gEduIsrData);

#else
	EDU_volatileWrite(BCHP_HIF_INTR2_CPU_CLEAR, HIF_INTR2_EDU_CLEAR_MASK);
#endif

	//EDU_volatileWrite(EDU_DONE, 0x00000000); 
	EDU_reset_done();
	EDU_volatileWrite(EDU_ERR_STATUS, 0x00000000); 

	EDU_volatileWrite(EDU_LENGTH, EDU_LENGTH_VALUE);

	//EDU_waitForNoPendingAndActiveBit();

//	Already covered by dma_map_single()
//	dma_cache_wback((unsigned long) virtual_addr_buffer, EDU_LENGTH_VALUE);

	EDU_issue_command(*physAddr, external_physical_device_address, EDU_WRITE); /* 1: Is a Read, 0 Is a Write */

//      rd_data = EDU_poll(EDU_BASE_ADDRESS  + BCHP_HIF_INTR2_CPU_STATUS, HIF_INTR2_EDU_DONE, HIF_INTR2_EDU_DONE);
//      EDU_volatileWrite(EDU_BASE_ADDRESS  + EDU_DONE, 0x00000000);

//edu_debug = 0;
//printk("<-- %s\n", __FUNCTION__);

	return 0;
}


/*
 * Returns INTR2 status on success
 * 0 on timeout.
 */
int EDU_read(volatile void* virtual_addr_buffer, uint32_t external_physical_device_address)
{
	uint32_t  phys_mem;
	// uint32_t  rd_data;
	int ret;
	//int retries = 4;
	//unsigned long flags;
		

//static int toggle;
//static int save_debug;

#if 0
edu_debug = gdebug;
if (external_physical_device_address == 0x1f7fc400) {toggle=1; save_debug=gdebug; gdebug = edu_debug = 4; }
//else if (external_physical_device_address < 0x1f7fc600 && toggle) {gdebug = edu_debug = save_debug;toggle=0;}
if (toggle) edu_debug = 4;
#endif

//PRINTK("--> %s: vAddr=%p, ext=%08x\n", __FUNCTION__, virtual_addr_buffer, external_physical_device_address);
#if 0
	phys_mem = EDU_virt_to_phys((void *)virtual_addr_buffer);
	if (!phys_mem) {
		return (-1);
	}
#else
	// THT: TBD: Need to adjust for cache line size here, especially on 7420.
	phys_mem = dma_map_single(NULL, virtual_addr_buffer, EDU_LENGTH_VALUE, DMA_FROM_DEVICE);
#endif

if (edu_debug) PRINTK("EDU_read: vBuff: %p physDev: %08x, PA=%08x\n", 
virtual_addr_buffer, external_physical_device_address, phys_mem);

 #ifdef CONFIG_MTD_BRCMNAND_USE_ISR
 	down(&gEduIsrData.lock);
 	gEduIsrData.edu_ldw = external_physical_device_address;
 	gEduIsrData.physAddr = phys_mem;
	
	/*
	 * Enable L2 Interrupt
	 */
	gEduIsrData.cmd = EDU_READ;
	gEduIsrData.opComplete = ISR_OP_SUBMITTED;
	gEduIsrData.status = 0;


	// We must also wait for Ctlr_Ready, otherwise the OOB is not correct, since we read the OOB bytes off the controller

	gEduIsrData.mask = HIF_INTR2_EDU_CLEAR_MASK|HIF_INTR2_CTRL_READY;
	gEduIsrData.expect = HIF_INTR2_EDU_DONE;
	// On error we also want Ctrlr-Ready because for COR ERR, the Hamming WAR depends on the OOB bytes.
	gEduIsrData.error = HIF_INTR2_EDU_ERR;
	gEduIsrData.intr = HIF_INTR2_EDU_DONE_MASK;
	up(&gEduIsrData.lock);
	
	ISR_enable_irq(&gEduIsrData);
#else

        EDU_volatileWrite(BCHP_HIF_INTR2_CPU_CLEAR, HIF_INTR2_EDU_CLEAR_MASK);
#endif


        EDU_reset_done();

        EDU_volatileWrite(EDU_ERR_STATUS, 0x00000000);
        
	 EDU_volatileWrite(EDU_LENGTH, EDU_LENGTH_VALUE);

	 EDU_waitForNoPendingAndActiveBit();

#ifdef CONFIG_BCM3548
// Debug, print out MEMC L2 status bit
	if (DisplayMemDebug()) 
		printk("MEMC Changed at flash addr %08x, DRAM Addr=%08x\n", 
			external_physical_device_address, phys_mem); 
#endif
	  

#ifdef CONFIG_MTD_BRCMNAND_USE_ISR
	do {
		EDU_issue_command(phys_mem, external_physical_device_address, EDU_READ);
		ret = ISR_wait_for_completion();
	} while (0);  //while (ret == (uint32_t) (ERESTARTSYS) && retries-- > 0);

#else
	EDU_issue_command(phys_mem, external_physical_device_address, EDU_READ); /* 1: Is a Read, 0 Is a Write */
	ret = EDU_poll(BCHP_HIF_INTR2_CPU_STATUS, 
        	HIF_INTR2_EDU_DONE, 
        	HIF_INTR2_EDU_ERR, 
        	HIF_INTR2_EDU_DONE_MASK);
#endif

	(void) dma_unmap_single(NULL, phys_mem, EDU_LENGTH_VALUE, DMA_FROM_DEVICE);

if (edu_debug) PRINTK("<-- %s ret=%08x\n", __FUNCTION__, ret);
//edu_debug = 0;
if (edu_debug > 3 && ret) {show_stack(current,NULL);dump_stack();}
        return ret;
} 

#endif // Batch mode

