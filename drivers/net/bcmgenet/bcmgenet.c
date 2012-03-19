/*
 * Copyright (c) 2002-2008 Broadcom Corporation
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
 * File Name  : bcmgenet.c
 *
 * Description: This is Linux driver for the broadcom GENET ethernet MAC core.

*/

#define CARDNAME    "bcmgenet"
#define VERSION     "2.0"
#define VER_STR     "v" VERSION " " __DATE__ " " __TIME__

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/interrupt.h>
#include <linux/string.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <linux/pm.h>
#include <linux/clk.h>
#include <linux/version.h>

#include <linux/mii.h>
#include <linux/ethtool.h>
#include <linux/netdevice.h>
#include <linux/inetdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/in.h>

#include <asm/mipsregs.h>
#include <asm/cacheflush.h>
#include <asm/brcmstb/brcmstb.h>
#include <asm/brcmstb/brcmapi.h>
#include "bcmmii.h"
#include "bcmgenet.h"
#include "if_net.h"

#ifdef CONFIG_NET_SCH_MULTIQ

#ifndef CONFIG_BRCM_GENET_V2
#error "This version of GENET doesn't support tx multi queue"
#endif
/* Default # of tx queues for multi queue support */
#define GENET_MQ_CNT		4
/* Default # of bds for each queue for multi queue support */
#define GENET_MQ_BD_CNT		48
/* Default highest priority queue for multi queue support */
#define GENET_Q0_PRIORITY	31

#define GENET_DEFAULT_BD_CNT	\
	(TOTAL_DESC - GENET_MQ_CNT * GENET_MQ_BD_CNT)

static void bcmgenet_init_multiq(struct net_device *dev);

#endif	/*CONFIG_NET_SCH_MULTIQ */

#define RX_BUF_LENGTH		2048
#define RX_BUF_BITS			12
#define SKB_ALIGNMENT		32
#define DMA_DESC_THRES		4
#define HFB_TCP_LEN			19
#define HFB_ARP_LEN			21

/* Tx/Rx DMA register offset, skip 256 descriptors */
#define GENET_TDMA_REG_OFF	(GENET_TDMA_OFF + \
		2*TOTAL_DESC*sizeof(unsigned long))
#define GENET_RDMA_REG_OFF	(GENET_RDMA_OFF + \
		2*TOTAL_DESC*sizeof(unsigned long))

/* --------------------------------------------------------------------------
External, indirect entry points.
--------------------------------------------------------------------------*/
static int bcmgenet_ioctl(struct net_device *dev, struct ifreq *rq, int cmd);
/* --------------------------------------------------------------------------
Called for "ifconfig ethX up" & "down"
--------------------------------------------------------------------------*/
static int bcmgenet_open(struct net_device *dev);
static int bcmgenet_close(struct net_device *dev);
/* --------------------------------------------------------------------------
Watchdog timeout
--------------------------------------------------------------------------*/
static void bcmgenet_timeout(struct net_device *dev);
/* --------------------------------------------------------------------------
Packet transmission.
--------------------------------------------------------------------------*/
static int bcmgenet_xmit(struct sk_buff *skb, struct net_device *dev);
/* --------------------------------------------------------------------------
Set address filtering mode
--------------------------------------------------------------------------*/
static void bcmgenet_set_multicast_list(struct net_device *dev);
/* --------------------------------------------------------------------------
Set the hardware MAC address.
--------------------------------------------------------------------------*/
static int bcmgenet_set_mac_addr(struct net_device *dev, void *p);

/* --------------------------------------------------------------------------
Interrupt routine, for all interrupts except ring buffer interrupts
--------------------------------------------------------------------------*/
static irqreturn_t bcmgenet_isr0(int irq, void *dev_id);
/*---------------------------------------------------------------------------
IRQ handler for ring buffer interrupt.
--------------------------------------------------------------------------*/
static irqreturn_t bcmgenet_isr1(int irq, void *dev_id);
/* --------------------------------------------------------------------------
dev->poll() method
--------------------------------------------------------------------------*/
static int bcmgenet_poll(struct napi_struct *napi, int budget);
static int bcmgenet_ring_poll(struct napi_struct *napi, int budget);
/* --------------------------------------------------------------------------
Process recived packet for descriptor based DMA
--------------------------------------------------------------------------*/
static unsigned int bcmgenet_desc_rx(void *ptr, unsigned int budget);
/* --------------------------------------------------------------------------
Process recived packet for ring buffer DMA
--------------------------------------------------------------------------*/
static unsigned int bcmgenet_ring_rx(void *ptr, unsigned int budget);
/* --------------------------------------------------------------------------
Internal routines
--------------------------------------------------------------------------*/
/* Allocate and initialize tx/rx buffer descriptor pools */
static int bcmgenet_init_dev(struct BcmEnet_devctrl *pDevCtrl);
static void bcmgenet_uninit_dev(struct BcmEnet_devctrl *pDevCtrl);
/* Assign the Rx descriptor ring */
static int assign_rx_buffers(struct BcmEnet_devctrl *pDevCtrl);
/* Initialize the uniMac control registers */
static int init_umac(struct BcmEnet_devctrl *pDevCtrl);
/* Initialize DMA control register */
static void init_edma(struct BcmEnet_devctrl *pDevCtrl);
/* Interrupt bottom-half */
static void bcmgenet_irq_task(struct work_struct *work);
/* power management */
static void bcmgenet_power_down(struct BcmEnet_devctrl *pDevCtrl, int mode);
static void bcmgenet_power_up(struct BcmEnet_devctrl *pDevCtrl, int mode);
/* allocate an skb, the data comes from ring buffer */
static struct sk_buff *__bcmgenet_alloc_skb_from_buf(unsigned char *buf,
		int len, int headroom);
static struct net_device *eth_root_dev;
static int DmaDescThres = DMA_DESC_THRES;
/*
 * HFB data for ARP request.
 * In WoL (Magic Packet or ACPI) mode, we need to response
 * ARP request, so dedicate an HFB to filter the ARP request.
 * NOTE: the last two words are to be filled by destination.
 */
static unsigned int hfb_arp[] = {
	0x000FFFFF, 0x000FFFFF, 0x000FFFFF,	0x00000000,
	0x00000000, 0x00000000, 0x000F0806,	0x000F0001,
	0x000F0800,	0x000F0604, 0x000F0001,	0x00000000,
	0x00000000,	0x00000000,	0x00000000, 0x00000000,
	0x000F0000,	0x000F0000,	0x000F0000,	0x000F0000,
	0x000F0000
};
/* -------------------------------------------------------------------------
 *  The following bcmemac_xxxx() functions are legacy netaccel hook, will be
 *  replaced!
 *  -----------------------------------------------------------------------*/
struct net_device *bcmemac_get_device(void)
{
	return eth_root_dev;
}
EXPORT_SYMBOL(bcmemac_get_device);
/* --------------------------------------------------------------------------
Name: bcmemac_get_free_txdesc
Purpose: Get Current Available TX desc count
-------------------------------------------------------------------------- */
int bcmemac_get_free_txdesc(struct net_device *dev)
{
	struct BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
	return pDevCtrl->txFreeBds;
}
EXPORT_SYMBOL(bcmemac_get_free_txdesc);
/* --------------------------------------------------------------------------
Name: bcmemac_xmit_check
Purpose: Reclaims TX descriptors
-------------------------------------------------------------------------- */
int bcmemac_xmit_check(struct net_device *dev)
{
	struct BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
	struct Enet_CB *txCBPtr;
	unsigned long flags, ret;
	unsigned int c_index;
	int lastTxedCnt = 0, i = 0;
	/*
	* Obtain exclusive access to transmitter.  This is necessary because
	* we might have more than one stack transmitting at once.
	*/
	spin_lock_irqsave(&pDevCtrl->lock, flags);
	/* Compute how many buffers are transmited since last xmit call */
	c_index = pDevCtrl->txDma->tDmaRings[16].tdma_consumer_index;
	c_index &= TOTAL_DESC;

	if (c_index >= pDevCtrl->txLastCIndex)
		lastTxedCnt = c_index - pDevCtrl->txLastCIndex;
	else
		lastTxedCnt = TOTAL_DESC - pDevCtrl->txLastCIndex + c_index;

	TRACE(("c_index=%d lastTxedCnt=%d txLastCIndex=%d\n",
				c_index, lastTxedCnt, pDevCtrl->txLastCIndex));

	/* Recalaim transmitted buffers */
	i = pDevCtrl->txLastCIndex;
	while (lastTxedCnt-- > 0) {
		txCBPtr = &pDevCtrl->txCbs[i];
		if (txCBPtr->skb != NULL) {
			dma_unmap_single(&pDevCtrl->dev->dev,
					txCBPtr->dma_addr,
					txCBPtr->skb->len,
					DMA_TO_DEVICE);
			dev_kfree_skb_any(txCBPtr->skb);
			txCBPtr->skb = NULL;
		}
		pDevCtrl->txFreeBds += 1;
		if (i == (TOTAL_DESC - 1))
			i = 0;
		else
			i++;
	}
	pDevCtrl->txLastCIndex = c_index;
	if (pDevCtrl->txFreeBds > 0) {
		/* Disable txdma bdone/pdone interrupt if we have free tx bds */
		pDevCtrl->intrl2_0->cpu_mask_set |= (UMAC_IRQ_TXDMA_BDONE |
			UMAC_IRQ_TXDMA_PDONE);
		netif_wake_queue(dev);
		ret = 0;
	} else {
		ret = 1;
	}
	spin_unlock_irqrestore(&pDevCtrl->lock, flags);

	return ret;
}
EXPORT_SYMBOL(bcmemac_xmit_check);
/* --------------------------------------------------------------------------
Name: bcmemac_xmit_fragment
Purpose: Send ethernet traffic Buffer DESC and submit to UDMA
-------------------------------------------------------------------------- */
int bcmemac_xmit_fragment(int ch, unsigned char *buf, int buf_len,
		unsigned long tx_flags , struct net_device *dev)
{
	struct BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
	struct Enet_CB *txCBPtr;
	unsigned int write_ptr = 0;

	if (pDevCtrl->txFreeBds == 0)
		return 1;
	/*
	 * We must don't have 64B status block enabled in this case!
	 */
	write_ptr = pDevCtrl->txDma->tDmaRings[16].tdma_write_pointer;
	write_ptr = ((write_ptr & DMA_RW_POINTER_MASK) >> 1);

	/* Obtain transmit control block */
	txCBPtr = &pDevCtrl->txCbs[write_ptr];
	txCBPtr->BdAddr = &pDevCtrl->txBds[write_ptr];
	txCBPtr->dma_addr = dma_map_single(&pDevCtrl->dev->dev, buf,
			buf_len, DMA_TO_DEVICE);

	/*
	 * Add the buffer to the ring.
	 * Set addr and length of DMA BD to be transmitted.
	 */
	txCBPtr->BdAddr->address = txCBPtr->dma_addr;
	txCBPtr->BdAddr->length_status = ((unsigned long)(buf_len))<<16;
	txCBPtr->BdAddr->length_status |= tx_flags | DMA_TX_APPEND_CRC;

	/* Default QTAG for MoCA */
	txCBPtr->BdAddr->length_status |=
		(DMA_TX_QTAG_MASK << DMA_TX_QTAG_SHIFT);

#ifdef CONFIG_BCMGENET_DUMP_DATA
	TRACE(("%s: len %d", __func__, buf_len));
	print_hex_dump(KERN_NOTICE, "", DUMP_PREFIX_ADDRESS,
			16, 1, buf, buf_len, 0);
#endif

	/* Decrement total BD count and advance our write pointer */
	pDevCtrl->txFreeBds -= 1;

	if (write_ptr == pDevCtrl->nrTxBds - 1)
		write_ptr = 0;
	else
		write_ptr++;
	/* advance producer index and write pointer.*/
	pDevCtrl->txDma->tDmaRings[DESC_INDEX].tdma_producer_index += 1;
	pDevCtrl->txDma->tDmaRings[DESC_INDEX].tdma_write_pointer += 2;

	if (pDevCtrl->txFreeBds == 0) {
		TRACE(("%s: %s no transmit queue space -- stopping queues\n",
				dev->name, __func__));
		/* Enable Tx bdone/pdone interrupt !*/
		pDevCtrl->intrl2_0->cpu_mask_clear |= (UMAC_IRQ_TXDMA_BDONE |
				UMAC_IRQ_TXDMA_PDONE);
		netif_stop_queue(dev);
	}
	/* update stats */
	dev->stats.tx_bytes += buf_len;
	dev->stats.tx_packets++;

	dev->trans_start = jiffies;

	return 0;
}
EXPORT_SYMBOL(bcmemac_xmit_fragment);
/* --------------------------------------------------------------------------
Name: bcmemac_xmit_multibuf
Purpose: Send ethernet traffic in multi buffers (hdr, buf, tail)
-------------------------------------------------------------------------- */
int bcmemac_xmit_multibuf(int ch, unsigned char *hdr, int hdr_len,
		unsigned char *buf, int buf_len, unsigned char *tail,
		int tail_len, struct net_device *dev)
{
	unsigned long flags;
	struct BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);

	while (bcmemac_xmit_check(dev))
		;

	/*
	* Obtain exclusive access to transmitter.  This is necessary because
	* we might have more than one stack transmitting at once.
	*/
	spin_lock_irqsave(&pDevCtrl->lock, flags);

	/* Header + Optional payload in two parts */
	if ((hdr_len > 0) && (buf_len > 0) &&
			(tail_len > 0) && (hdr) &&
			(buf) && (tail)) {
		/* Send Header */
		while (bcmemac_xmit_fragment(ch, hdr, hdr_len, DMA_SOP, dev))
			bcmemac_xmit_check(dev);
		/* Send First Fragment */
		while (bcmemac_xmit_fragment(ch, buf, buf_len, 0, dev))
			bcmemac_xmit_check(dev);
		/* Send 2nd Fragment */
		while (bcmemac_xmit_fragment(ch, tail, tail_len, DMA_EOP, dev))
			bcmemac_xmit_check(dev);
	} else if ((hdr_len > 0) && (buf_len > 0) && (hdr) && (buf)) {
		/* Header + Optional payload */
		/* Send Header */
		while (bcmemac_xmit_fragment(ch, hdr, hdr_len, DMA_SOP, dev))
			bcmemac_xmit_check(dev);
		/* Send First Fragment */
		while (bcmemac_xmit_fragment(ch, buf, buf_len, DMA_EOP, dev))
			bcmemac_xmit_check(dev);
	} else if ((hdr_len > 0) && (hdr)) {
		/* Header Only (includes payload) */
		/* Send Header */
		while (bcmemac_xmit_fragment(ch, hdr, hdr_len,
					DMA_SOP | DMA_EOP, dev))
			bcmemac_xmit_check(dev);
	} else {
		spin_unlock_irqrestore(&pDevCtrl->lock, flags);
		return 0; /* Drop the packet */
	}
	spin_unlock_irqrestore(&pDevCtrl->lock, flags);
	return 0;
}
EXPORT_SYMBOL(bcmemac_xmit_multibuf);
static inline void handleAlignment(struct BcmEnet_devctrl *pDevCtrl,
		struct sk_buff *skb)
{
	/*
	 * We request to allocate 2048 + 32 bytes of buffers, and the
	 * dev_alloc_skb() added 16B for NET_SKB_PAD, so we totally
	 * requested 2048+32+16 bytes buffer, the size was aligned to
	 * SMP_CACHE_BYTES, which is 64B.(is it?), so we finnally ended
	 * up got 2112 bytes of buffer! Among which, the first 16B is
	 * reserved for NET_SKB_PAD, to make the skb->data aligned 32B
	 * boundary, we should have enough space to fullfill the 2KB
	 * buffer after alignment!
     */

	unsigned long boundary32, curData, resHeader;

	curData = (unsigned long) skb->data;
	boundary32 = (curData + (SKB_ALIGNMENT - 1)) & ~(SKB_ALIGNMENT - 1);
	resHeader = boundary32 - curData ;
	/* 4 bytes for skb pointer */
	if (resHeader < 4)
		boundary32 += 32;

	resHeader = boundary32 - curData - 4;
	/* We'd have minimum 16B reserved by default. */
	skb_reserve(skb, resHeader);

	*(unsigned int *)skb->data = (unsigned int)skb;
	skb_reserve(skb, 4);
	/*
	 * Make sure it is on 32B boundary, should never happen if our
	 * calculation was correct.
	 */
	if ((unsigned long) skb->data & (SKB_ALIGNMENT - 1)) {
		printk(KERN_WARNING "skb buffer is NOT aligned on %d boundary!\n",
			SKB_ALIGNMENT);
	}

	/*
	 *  we don't reserve 2B for IP Header optimization here,
	 *  use skb_pull when receiving packets
	 */
}
/* --------------------------------------------------------------------------
Name: bcmgenet_gphy_link_status
Purpose: GPHY link status monitoring task
-------------------------------------------------------------------------- */
static void bcmgenet_gphy_link_status(struct work_struct *work)
{
	struct BcmEnet_devctrl *pDevCtrl = container_of(work,
			struct BcmEnet_devctrl, bcmgenet_link_work);
	int link;

	link = mii_link_ok(&pDevCtrl->mii);
	if (link && !netif_carrier_ok(pDevCtrl->dev)) {
		mii_setup(pDevCtrl->dev);
		pDevCtrl->dev->flags |= IFF_RUNNING;
		netif_carrier_on(pDevCtrl->dev);
		rtmsg_ifinfo(RTM_NEWLINK, pDevCtrl->dev, IFF_RUNNING);
	} else if (!link && netif_carrier_ok(pDevCtrl->dev)) {
		printk(KERN_INFO "%s: Link is down\n", pDevCtrl->dev->name);
		netif_carrier_off(pDevCtrl->dev);
		pDevCtrl->dev->flags &= ~IFF_RUNNING;
		rtmsg_ifinfo(RTM_DELLINK, pDevCtrl->dev, IFF_RUNNING);
	}
}
/* --------------------------------------------------------------------------
Name: bcmgenet_gphy_link_timer
Purpose: GPHY link status monitoring timer function
-------------------------------------------------------------------------- */
static void bcmgenet_gphy_link_timer(unsigned long data)
{
	struct BcmEnet_devctrl * pDevCtrl = (struct BcmEnet_devctrl *)data;
	schedule_work(&pDevCtrl->bcmgenet_link_work);
	mod_timer(&pDevCtrl->timer, jiffies + HZ);
}
/* --------------------------------------------------------------------------
Name: bcmgenet_open
Purpose: Open and Initialize the EMAC on the chip
-------------------------------------------------------------------------- */
static int bcmgenet_open(struct net_device *dev)
{
	struct BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
	unsigned long dma_ctrl;

	TRACE(("%s: bcmgenet_open\n", dev->name));
	clk_enable(pDevCtrl->clk);

	if (pDevCtrl->phyType == BRCM_PHY_TYPE_INT)
		pDevCtrl->ext->ext_pwr_mgmt |= EXT_ENERGY_DET_MASK;
	/* disable ethernet MAC while updating its registers */
	pDevCtrl->umac->cmd &= ~(CMD_TX_EN | CMD_RX_EN);
	/* disable DMA */
	dma_ctrl = 1 << (DESC_INDEX + DMA_RING_BUF_EN_SHIFT) | DMA_EN;
	pDevCtrl->txDma->tdma_ctrl &= ~dma_ctrl;
	pDevCtrl->rxDma->rdma_ctrl &= ~dma_ctrl;
	pDevCtrl->umac->tx_flush = 1;
	GENET_RBUF_FLUSH_CTRL(pDevCtrl) = 1;
	udelay(10);
	pDevCtrl->umac->tx_flush = 0;
	GENET_RBUF_FLUSH_CTRL(pDevCtrl) = 0;

	/* reset dma, start from begainning of the ring. */
	init_edma(pDevCtrl);
	/* reset internal book keeping variables. */
	pDevCtrl->txLastCIndex = 0;
	pDevCtrl->rxBdAssignPtr = pDevCtrl->rxBds;
	assign_rx_buffers(pDevCtrl);
	pDevCtrl->txFreeBds = pDevCtrl->nrTxBds;

	/*Always enable ring 16 - descriptor ring */
	pDevCtrl->rxDma->rdma_ctrl |= dma_ctrl;
	pDevCtrl->txDma->tdma_ctrl |= dma_ctrl;

	if (pDevCtrl->phyType == BRCM_PHY_TYPE_EXT_MII ||
		pDevCtrl->phyType == BRCM_PHY_TYPE_EXT_RGMII ||
		pDevCtrl->phyType == BRCM_PHY_TYPE_EXT_RGMII_IBS) {
		mod_timer(&pDevCtrl->timer, jiffies);
	}
	/* Start the network engine */
	netif_tx_start_all_queues(dev);
	napi_enable(&pDevCtrl->napi);

	pDevCtrl->umac->cmd |= (CMD_TX_EN | CMD_RX_EN);
	pDevCtrl->dev_opened = 1;

	if (pDevCtrl->phyType == BRCM_PHY_TYPE_INT)
		bcmgenet_power_up(pDevCtrl, GENET_POWER_PASSIVE);
	return 0;
}
/* --------------------------------------------------------------------------
Name: bcmgenet_close
Purpose: Stop communicating with the outside world
Note: Caused by 'ifconfig ethX down'
-------------------------------------------------------------------------- */
static int bcmgenet_close(struct net_device *dev)
{
	struct BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
	int timeout = 0;

	TRACE(("%s: bcmgenet_close\n", dev->name));

	napi_disable(&pDevCtrl->napi);
	netif_tx_stop_all_queues(dev);
	/* Stop Tx DMA */
	pDevCtrl->txDma->tdma_ctrl &= ~DMA_EN;
	while (timeout < 5000) {
		if (pDevCtrl->txDma->tdma_status & DMA_EN)
			break;
		udelay(1);
		timeout++;
	}
	if (timeout == 5000)
		printk(KERN_ERR "Timed out while shutting down Tx DMA\n");

	/* Disable Rx DMA*/
	pDevCtrl->rxDma->rdma_ctrl &= ~DMA_EN;
	timeout = 0;
	while (timeout < 5000) {
		if (pDevCtrl->rxDma->rdma_status & DMA_EN)
			break;
		udelay(1);
		timeout++;
	}
	if (timeout == 5000)
		printk(KERN_ERR "Timed out while shutting down Rx DMA\n");

	pDevCtrl->umac->cmd &= ~(CMD_RX_EN | CMD_TX_EN);

	/* tx reclaim */
	bcmgenet_xmit(NULL, dev);

	if (pDevCtrl->phyType == BRCM_PHY_TYPE_EXT_MII ||
		pDevCtrl->phyType == BRCM_PHY_TYPE_EXT_RGMII ||
		pDevCtrl->phyType == BRCM_PHY_TYPE_EXT_RGMII_IBS) {
		del_timer_sync(&pDevCtrl->timer);
		cancel_work_sync(&pDevCtrl->bcmgenet_link_work);
	}
	pDevCtrl->dev_opened = 0;
	if (pDevCtrl->phyType == BRCM_PHY_TYPE_INT)
		bcmgenet_power_down(pDevCtrl, GENET_POWER_PASSIVE);
	clk_disable(pDevCtrl->clk);
	return 0;
}

/* --------------------------------------------------------------------------
Name: bcmgenet_net_timeout
Purpose:
-------------------------------------------------------------------------- */
static void bcmgenet_timeout(struct net_device *dev)
{
	BUG_ON(dev == NULL);

	TRACE(("%s: bcmgenet_timeout\n", dev->name));

	dev->trans_start = jiffies;

	dev->stats.tx_errors++;
	netif_wake_queue(dev);
}

/* --------------------------------------------------------------------------
Name: bcmgenet_set_multicast_list
Purpose: Set the multicast mode, ie. promiscuous or multicast
-------------------------------------------------------------------------- */
static void bcmgenet_set_multicast_list(struct net_device *dev)
{
	struct BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 35)
	struct netdev_hw_addr *ha;
#else
	struct dev_mc_list *dmi;
#endif
	int i, mc;
#define MAX_MC_COUNT	16

	TRACE(("%s: bcmgenet_set_multicast_list: %08X\n",
		dev->name, dev->flags));

	/* Promiscous mode */
	if (dev->flags & IFF_PROMISC) {
		pDevCtrl->umac->cmd |= CMD_PROMISC;
		pDevCtrl->umac->mdf_ctrl = 0;
		return;
	} else
		pDevCtrl->umac->cmd &= ~CMD_PROMISC;


	/* UniMac doesn't support ALLMULTI */
	if (dev->flags & IFF_ALLMULTI)
		return;

	/* update MDF filter */
	i = 0;
	mc = 0;
	/* Broadcast */
	pDevCtrl->umac->mdf_addr[i] = 0xFFFF;
	pDevCtrl->umac->mdf_addr[i+1] = 0xFFFFFFFF;
	pDevCtrl->umac->mdf_ctrl |= (1 << (MAX_MC_COUNT - mc));
	i += 2;
	mc++;
	/* Unicast*/
	pDevCtrl->umac->mdf_addr[i] = (dev->dev_addr[0]<<8) | dev->dev_addr[1];
	pDevCtrl->umac->mdf_addr[i+1] = dev->dev_addr[2] << 24 |
		dev->dev_addr[3] << 16 |
		dev->dev_addr[4] << 8 |
		dev->dev_addr[5];
	pDevCtrl->umac->mdf_ctrl |= (1 << (MAX_MC_COUNT - mc));
	i += 2;
	mc++;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 35)
	if (netdev_mc_empty(dev) || netdev_mc_count(dev) >= MAX_MC_COUNT)
		return;
	netdev_for_each_mc_addr(ha, dev) {
		pDevCtrl->umac->mdf_addr[i] = ha->addr[0] << 8 | ha->addr[1];
		pDevCtrl->umac->mdf_addr[i+1] = ha->addr[2] << 24 |
			ha->addr[3] << 16 | ha->addr[4] << 8 | ha->addr[5];
		pDevCtrl->umac->mdf_ctrl |= (1 << (MAX_MC_COUNT - mc));
		i += 2;
		mc++;
	}
#else
	if (dev->mc_count == 0 || dev->mc_count > (MAX_MC_COUNT - 1))
		return;
	/* Multicast */
	for (dmi = dev->mc_list; dmi; dmi = dmi->next) {
		pDevCtrl->umac->mdf_addr[i] = (dmi->dmi_addr[0] << 8) |
			dmi->dmi_addr[1];
		pDevCtrl->umac->mdf_addr[i+1] = (dmi->dmi_addr[2] << 24) |
			(dmi->dmi_addr[3] << 16) |
			(dmi->dmi_addr[4] << 8) |
			dmi->dmi_addr[5];
		pDevCtrl->umac->mdf_ctrl |= (1 << (MAX_MC_COUNT - mc));
		i += 2;
		mc++;
	}
#endif
}
/*
 * Set the hardware MAC address.
 */
static int bcmgenet_set_mac_addr(struct net_device *dev, void *p)
{
	struct sockaddr *addr = p;

	if (netif_running(dev))
		return -EBUSY;

	memcpy(dev->dev_addr, addr->sa_data, dev->addr_len);

	return 0;
}
/* --------------------------------------------------------------------------
Name: bcmgenet_select_queue
Purpose: select which xmit queue to use based on skb->queue_mapping.
-------------------------------------------------------------------------- */
static u16 __maybe_unused bcmgenet_select_queue(struct net_device *dev,
		struct sk_buff *skb)
{
	/*
	 * If multi-queue support is enabled, and NET_ACT_SKBEDIT is not
	 * defined, this function simply returns current queue_mapping set
	 * inside skb, this means other modules, (netaccel, for example),
	 * must provide a mechanism to set the queue_mapping before trying
	 * to send a packet.
	 */
	return skb->queue_mapping;
}
/* --------------------------------------------------------------------------
Name: __bcmgenet_alloc_skb_from_buf
Purpose: Allocated an skb from exsiting buffer.
-------------------------------------------------------------------------- */
static struct sk_buff *__bcmgenet_alloc_skb_from_buf(unsigned char *buf,
		int len, int headroom)
{
	struct skb_shared_info *shinfo;
	struct sk_buff *skb;

	skb = kmem_cache_alloc(skbuff_head_cache, GFP_ATOMIC);
	if (!skb)
		return NULL;
	memset(skb, 0, offsetof(struct sk_buff, tail));
	skb->truesize = len + sizeof(struct sk_buff);
	atomic_set(&skb->users, 1);
	skb->head = buf;
	skb->data = buf;
	skb_reset_tail_pointer(skb);
	skb->end = skb->tail + len - sizeof(struct skb_shared_info);
	/* FCLONE_ORIG tell kfree_skb() not to release data */
	skb->cloned = SKB_FCLONE_ORIG;
	/* FLONE_ORIG tells kfree_skb to free skb from skb head cache*/
	skb->fclone = SKB_FCLONE_UNAVAILABLE;

	skb_reserve(skb, headroom);
	shinfo = skb_shinfo(skb);

	/* Set dataref to 2, so upper layer won't free the data buffer */
	atomic_set(&shinfo->dataref, 2);
	shinfo->nr_frags = 0;
	shinfo->gso_size = 0;
	shinfo->gso_segs = 0;
	shinfo->gso_type = 0;
	shinfo->ip6_frag_id = 0;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37)
	shinfo->tx_flags = 0;
#else
	shinfo->tx_flags.flags = 0;
#endif
	shinfo->frag_list = NULL;
	memset(&shinfo->hwtstamps, 0, sizeof(shinfo->hwtstamps));

	return skb;
}
/* --------------------------------------------------------------------------
Name: bcmgenet_alloc_txring_skb
Purpose: Allocated skb for tx ring buffer.
-------------------------------------------------------------------------- */
struct sk_buff *bcmgenet_alloc_txring_skb(struct net_device *dev, int index)
{
	unsigned long flags;
	struct sk_buff *skb = NULL;
	unsigned char *buf;
	struct BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
	if (!(pDevCtrl->txDma->tdma_ctrl &
			(1 << (index + DMA_RING_BUF_EN_SHIFT)))) {
		printk(KERN_ERR "Ring %d is not enabled\n", index);
		BUG();
	}
	spin_lock_irqsave(&pDevCtrl->lock, flags);
	if (pDevCtrl->txRingFreeBds[index] == 0) {
		/*
		 * This shouldn't happen, upper level should
		 * check if the tx queue stopped before calling this.
		 */
		printk(KERN_ERR "%s:%d queue stopped!!\n", __func__, index);
		spin_unlock_irqrestore(&pDevCtrl->lock, flags);
		return skb;
	}
	buf = phys_to_virt(
		pDevCtrl->txDma->tDmaRings[index].tdma_write_pointer);
	skb = __bcmgenet_alloc_skb_from_buf(buf, RX_BUF_LENGTH, 64);

	spin_unlock_irqrestore(&pDevCtrl->lock, flags);

	return skb;
}
EXPORT_SYMBOL(bcmgenet_alloc_txring_skb);
#if defined(CONFIG_BRCM_GENET_V2) && defined(CONFIG_NET_SCH_MULTIQ)
/* --------------------------------------------------------------------------
Name: netif_any_subqueue_stopped
Purpose: return 1 if any subqueue is stopped
-------------------------------------------------------------------------- */
static inline int netif_any_subqueue_stopped(struct net_device *dev)
{
	int i;
	for (i = 0; i < GENET_MQ_CNT + 1; i++) {
		if (__netif_subqueue_stopped(dev, i))
			return 1;
	}
	return 0;
}
#endif
/* --------------------------------------------------------------------------
Name: bcmgenet_get_txcb
Purpose: return tx control data and increment write pointer.
-------------------------------------------------------------------------- */
static struct Enet_CB *bcmgenet_get_txcb(struct net_device *dev,
		int *pos, int index)
{
	struct BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
	struct Enet_CB *txCBPtr = NULL;
#if defined(CONFIG_BRCM_GENET_V2) && defined(CONFIG_NET_SCH_MULTIQ)
	if (index == DESC_INDEX) {
		txCBPtr = pDevCtrl->txCbs;
		txCBPtr += (*pos - GENET_MQ_CNT*GENET_MQ_BD_CNT);
		txCBPtr->BdAddr = &pDevCtrl->txBds[*pos];
		if (*pos == (TOTAL_DESC - 1))
			*pos = (GENET_MQ_CNT*GENET_MQ_BD_CNT);
		else
			*pos += 1;

	} else {
		txCBPtr = pDevCtrl->txRingCBs[index];
		txCBPtr += (*pos - index*GENET_MQ_BD_CNT);
		txCBPtr->BdAddr = &pDevCtrl->txBds[*pos];
		if (*pos == (GENET_MQ_BD_CNT*(index+1) - 1))
			*pos = GENET_MQ_BD_CNT * index;
		else
			*pos += 1;
	}
#else
	txCBPtr = pDevCtrl->txCbs + *pos;
	txCBPtr->BdAddr = &pDevCtrl->txBds[*pos];
	/* Advancing local write pointer */
	if (*pos == (TOTAL_DESC - 1))
		*pos = 0;
	else
		*pos += 1;
#endif

	return txCBPtr;
}
/* --------------------------------------------------------------------------
Name: bcmgenet_tx_reclaim
Purpose: reclaim xmited skb
-------------------------------------------------------------------------- */
static void bcmgenet_tx_reclaim(struct net_device *dev, int index)
{
	struct BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
	unsigned int p_index, c_index;
	struct Enet_CB *txCBPtr;
	int lastTxedCnt = 0, lastCIndex = 0, nrTxBds = 0;

	/* Compute how many buffers are transmited since last xmit call */
	p_index = pDevCtrl->txDma->tDmaRings[index].tdma_producer_index;
	c_index = pDevCtrl->txDma->tDmaRings[index].tdma_consumer_index;


#if defined(CONFIG_BRCM_GENET_V2) && defined(CONFIG_NET_SCH_MULTIQ)
	if (index == DESC_INDEX) {
		lastCIndex = pDevCtrl->txLastCIndex;
		nrTxBds = GENET_DEFAULT_BD_CNT;
	} else {
		lastCIndex = pDevCtrl->txRingCIndex[index];
		nrTxBds = GENET_MQ_BD_CNT;
	}

#else
	lastCIndex = pDevCtrl->txLastCIndex;
	nrTxBds = TOTAL_DESC;
#endif
	p_index &= (nrTxBds - 1);
	c_index &= (nrTxBds - 1);

	if (c_index >= lastCIndex)
		lastTxedCnt = c_index - lastCIndex;
	else
		lastTxedCnt = nrTxBds - lastCIndex + c_index;


	TRACE(("%s: %s index=%d c_index=%d p_index=%d "
			"lastTxedCnt=%d txLastCIndex=%d\n",
			__func__, pDevCtrl->dev->name, index,
			c_index, p_index, lastTxedCnt, lastCIndex));

	/* Recalaim transmitted buffers */
	while (lastTxedCnt-- > 0) {
		if (index == DESC_INDEX)
			txCBPtr = &pDevCtrl->txCbs[lastCIndex];
		else
			txCBPtr = pDevCtrl->txRingCBs[index] + lastCIndex;
		if (txCBPtr->skb != NULL) {
			dma_unmap_single(&pDevCtrl->dev->dev,
					txCBPtr->dma_addr,
					txCBPtr->skb->len,
					DMA_TO_DEVICE);
			dev_kfree_skb_any(txCBPtr->skb);
			txCBPtr->skb = NULL;
			txCBPtr->dma_addr = 0;
		} else if (txCBPtr->dma_addr) {
			dma_unmap_page(&pDevCtrl->dev->dev,
					txCBPtr->dma_addr,
					txCBPtr->dma_len,
					DMA_TO_DEVICE);
			txCBPtr->dma_addr = 0;
		}
		if (index == DESC_INDEX)
			pDevCtrl->txFreeBds += 1;
		else
			pDevCtrl->txRingFreeBds[index] += 1;

		if (lastCIndex == (nrTxBds - 1))
			lastCIndex = 0;
		else
			lastCIndex++;
	}
#if defined(CONFIG_BRCM_GENET_V2) && defined(CONFIG_NET_SCH_MULTIQ)
	if (index == DESC_INDEX) {
		if (pDevCtrl->txFreeBds > (MAX_SKB_FRAGS + 1)
			&& __netif_subqueue_stopped(dev, 0)) {
			netif_wake_subqueue(dev, 0);
		}
		pDevCtrl->txLastCIndex = c_index;
	} else{
		if (pDevCtrl->txRingFreeBds[index] > (MAX_SKB_FRAGS + 1)
			&& __netif_subqueue_stopped(dev, index)) {
			netif_wake_subqueue(dev, index);
		}
		pDevCtrl->txRingCIndex[index] = c_index;
	}
	/*
	 * Disable txdma bdone/pdone interrupt only if all
	 * subqueues are active.
	 */
	if (!netif_any_subqueue_stopped(dev))
		pDevCtrl->intrl2_0->cpu_mask_set |= (UMAC_IRQ_TXDMA_BDONE |
				UMAC_IRQ_TXDMA_PDONE);
#else
	if (pDevCtrl->txFreeBds > (MAX_SKB_FRAGS + 1)
			&& netif_queue_stopped(dev)) {
		/* Disable txdma bdone/pdone interrupt if we have free tx bds */
		pDevCtrl->intrl2_0->cpu_mask_set |= (UMAC_IRQ_TXDMA_BDONE |
				UMAC_IRQ_TXDMA_PDONE);
		netif_wake_queue(dev);
	}
	pDevCtrl->txLastCIndex = c_index;
#endif
}

/* --------------------------------------------------------------------------
Name: bcmgenet_xmit
Purpose: Send ethernet traffic
-------------------------------------------------------------------------- */
static int bcmgenet_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
	volatile struct tDmaRingRegs *tDma_ring;
	struct Enet_CB *txCBPtr;
	unsigned int write_ptr = 0;
	int i = 0;
	unsigned long flags;
	struct status_64 *Status = NULL;
	int nr_frags = 0, index = DESC_INDEX;

	spin_lock_irqsave(&pDevCtrl->lock, flags);

#if defined(CONFIG_BRCM_GENET_V2) && defined(CONFIG_NET_SCH_MULTIQ)
	if (skb) {
		index = skb_get_queue_mapping(skb);
		/*
		* Mapping strategy:
		* queue_mapping = 0, unclassfieid, packet xmited through ring16
		* queue_mapping = 1, goes to ring 0. (highest priority queue)
		* queue_mapping = 2, goes to ring 1.
		* queue_mapping = 3, goes to ring 2.
		* queue_mapping = 4, goes to ring 3.
		*/
		if (index == 0)
			index = DESC_INDEX;
		else
			index -= 1;
		if (index != DESC_INDEX && index > 3) {
			printk(KERN_ERR "%s: skb->queue_mapping %d is invalid\n",
					__func__, skb_get_queue_mapping(skb));
			spin_unlock_irqrestore(&pDevCtrl->lock, flags);
			dev->stats.tx_errors++;
			dev->stats.tx_dropped++;
			return 1;
		}
		nr_frags = skb_shinfo(skb)->nr_frags;
		if (index == DESC_INDEX) {
			if (pDevCtrl->txFreeBds <= nr_frags + 1) {
				netif_stop_subqueue(dev, 0);
				spin_unlock_irqrestore(&pDevCtrl->lock, flags);
				printk(KERN_ERR "%s: tx ring %d full when queue awake\n",
					__func__, index);
				return 1;
			}
		} else if (pDevCtrl->txRingFreeBds[index] <= nr_frags + 1) {
			netif_stop_subqueue(dev, index);
			spin_unlock_irqrestore(&pDevCtrl->lock, flags);
			printk(KERN_ERR "%s: tx ring %d full when queue awake\n",
					__func__, index);
			return 1;
		}
	}
	/* Reclaim xmited skb for each subqueue */
	for (i = 0; i < GENET_MQ_CNT; i++)
		bcmgenet_tx_reclaim(dev, i);
#else
	if (skb) {
		nr_frags = skb_shinfo(skb)->nr_frags;
		if (pDevCtrl->txFreeBds <= nr_frags + 1) {
			netif_stop_queue(dev);
			spin_unlock_irqrestore(&pDevCtrl->lock, flags);
			printk(KERN_ERR "%s: tx ring full when queue awake\n",
				__func__);
			return 1;
		}
	}
#endif
	bcmgenet_tx_reclaim(dev, DESC_INDEX);
	if (!skb) {
		spin_unlock_irqrestore(&pDevCtrl->lock, flags);
		return 0;
	}
	tDma_ring = &pDevCtrl->txDma->tDmaRings[index];
	/*
	 * If 64 byte status block enabled, must make sure skb has
	 * enough headroom for us to insert 64B status block.
	 */
	if (GENET_TBUF_CTRL(pDevCtrl) & RBUF_64B_EN) {
		if (likely(skb_headroom(skb) < 64)) {
			struct sk_buff *new_skb;
			new_skb = skb_realloc_headroom(skb, 64);
			if (new_skb  == NULL) {
				dev_kfree_skb(skb);
				dev->stats.tx_errors++;
				dev->stats.tx_dropped++;
				spin_unlock_irqrestore(&pDevCtrl->lock, flags);
				return 0;
			} else if (skb->sk) {
				skb_set_owner_w(new_skb, skb->sk);
			}
			dev_kfree_skb(skb);
			skb = new_skb;
		}
		skb_push(skb, 64);
		Status = (struct status_64 *)skb->data;
	}
	write_ptr = (DMA_RW_POINTER_MASK & tDma_ring->tdma_write_pointer) >> 1;

	/* Obtain transmit control block */
	txCBPtr = bcmgenet_get_txcb(dev, &write_ptr, index);

	if (unlikely(!txCBPtr))
		BUG();

	txCBPtr->skb = skb;

	if ((skb->ip_summed  == CHECKSUM_PARTIAL) &&
			(GENET_TBUF_CTRL(pDevCtrl) & RBUF_64B_EN)) {
		u16 offset;
		offset = skb->csum_start - skb_headroom(skb) - 64;
		/* Insert 64B TSB and set the flag */
		Status->tx_csum_info = (offset << STATUS_TX_CSUM_START_SHIFT) |
			(offset + skb->csum_offset) |
			STATUS_TX_CSUM_LV;
	}

	/*
	 * Add the buffer to the ring.
	 * Set addr and length of DMA BD to be transmitted.
	 */
	if (!nr_frags) {
		txCBPtr->dma_addr = dma_map_single(&pDevCtrl->dev->dev,
				skb->data, skb->len, DMA_TO_DEVICE);
		if (!txCBPtr->dma_addr) {
			dev_err(&pDevCtrl->dev->dev, "Tx DMA map failed\n");
			spin_unlock_irqrestore(&pDevCtrl->lock, flags);
			return 0;
		}
		txCBPtr->dma_len = skb->len;
		txCBPtr->BdAddr->address = txCBPtr->dma_addr;
		txCBPtr->BdAddr->length_status = (
			((unsigned long)((skb->len < ETH_ZLEN) ?
			ETH_ZLEN : skb->len)) << 16);
		txCBPtr->BdAddr->length_status |= DMA_SOP | DMA_EOP |
			DMA_TX_APPEND_CRC;

		if (skb->ip_summed  == CHECKSUM_PARTIAL)
			txCBPtr->BdAddr->length_status |= DMA_TX_DO_CSUM;

		/* Default QTAG for MoCA */
		txCBPtr->BdAddr->length_status |= (DMA_TX_QTAG_MASK <<
				DMA_TX_QTAG_SHIFT);
#ifdef CONFIG_BCMGENET_DUMP_DATA
		printk(KERN_NOTICE "%s: data 0x%p len %d",
				__func__, skb->data, skb->len);
		print_hex_dump(KERN_NOTICE, "", DUMP_PREFIX_ADDRESS,
				16, 1, skb->data, skb->len, 0);
#endif
		/* Decrement total BD count and advance our write pointer */
#if defined(CONFIG_BRCM_GENET_V2) && defined(CONFIG_NET_SCH_MULTIQ)
		if (index == DESC_INDEX)
			pDevCtrl->txFreeBds -= 1;
		else
			pDevCtrl->txRingFreeBds[index] -= 1;
#else
		pDevCtrl->txFreeBds -= 1;
#endif
		/* advance producer index and write pointer.*/
		tDma_ring->tdma_producer_index += 1;
		tDma_ring->tdma_write_pointer = (write_ptr << 1);
		/* update stats */
		dev->stats.tx_bytes += ((skb->len < ETH_ZLEN) ?
				ETH_ZLEN : skb->len);
		dev->stats.tx_packets++;

	} else {
		/* xmit head */
		txCBPtr->dma_addr = dma_map_single(&pDevCtrl->dev->dev,
				skb->data, skb_headlen(skb), DMA_TO_DEVICE);
		if (!txCBPtr->dma_addr) {
			dev_err(&pDevCtrl->dev->dev, "Tx DMA map failed\n");
			spin_unlock_irqrestore(&pDevCtrl->lock, flags);
			return 0;
		}
		txCBPtr->dma_len = skb_headlen(skb);
		txCBPtr->BdAddr->address = txCBPtr->dma_addr;
		txCBPtr->BdAddr->length_status = skb_headlen(skb) << 16;
		txCBPtr->BdAddr->length_status |= DMA_SOP | DMA_TX_APPEND_CRC;
		if (skb->ip_summed  == CHECKSUM_PARTIAL)
			txCBPtr->BdAddr->length_status |= DMA_TX_DO_CSUM;

		txCBPtr->BdAddr->length_status |= (DMA_TX_QTAG_MASK <<
				DMA_TX_QTAG_SHIFT);
#ifdef CONFIG_BCMGENET_DUMP_DATA
		printk(KERN_NOTICE "%s: frag head len %d",
				__func__, skb_headlen(skb));
		print_hex_dump(KERN_NOTICE, "", DUMP_PREFIX_ADDRESS,
				16, 1, skb->data, skb_headlen(skb), 0);
#endif
		/* Decrement total BD count and advance our write pointer */
#if defined(CONFIG_BRCM_GENET_V2) && defined(CONFIG_NET_SCH_MULTIQ)
		if (index == DESC_INDEX)
			pDevCtrl->txFreeBds -= 1;
		else
			pDevCtrl->txRingFreeBds[index] -= 1;
#else
		pDevCtrl->txFreeBds -= 1;
#endif
		/* advance producer index and write pointer.*/
		tDma_ring->tdma_producer_index += 1;
		tDma_ring->tdma_write_pointer = (write_ptr << 1);
		dev->stats.tx_bytes += skb_headlen(skb);

		/* xmit fragment */
		for (i = 0; i < nr_frags; i++) {
			skb_frag_t *frag = &skb_shinfo(skb)->frags[i];
			txCBPtr = bcmgenet_get_txcb(dev, &write_ptr, index);

			if (unlikely(!txCBPtr))
				BUG();
			txCBPtr->skb = NULL;
			txCBPtr->dma_addr = dma_map_page(&pDevCtrl->dev->dev,
					frag->page,
					frag->page_offset,
					frag->size,
					DMA_TO_DEVICE);
			if (txCBPtr->dma_addr == 0) {
				printk(KERN_ERR "%s: Tx DMA map failed\n",
					__func__);
				spin_unlock_irqrestore(&pDevCtrl->lock, flags);
				return 0;
			}
			txCBPtr->dma_len = frag->size;
			txCBPtr->BdAddr->address = txCBPtr->dma_addr;
#ifdef CONFIG_BCMGENET_DUMP_DATA
			printk(KERN_NOTICE "%s: frag%d len %d",
					__func__, i, frag->size);
			print_hex_dump(KERN_NOTICE, "", DUMP_PREFIX_ADDRESS,
				16, 1
				page_address(frag->page)+frag->page_offset,
				frag->size, 0);
#endif
			txCBPtr->BdAddr->length_status = (
					(unsigned long)frag->size << 16);
			txCBPtr->BdAddr->length_status |= (DMA_TX_QTAG_MASK <<
					DMA_TX_QTAG_SHIFT);
			if (i == nr_frags - 1)
				txCBPtr->BdAddr->length_status |= DMA_EOP;

#if defined(CONFIG_BRCM_GENET_V2) && defined(CONFIG_NET_SCH_MULTIQ)
			if (index == DESC_INDEX)
				pDevCtrl->txFreeBds -= 1;
			else
				pDevCtrl->txRingFreeBds[index] -= 1;
#else
			pDevCtrl->txFreeBds -= 1;
#endif
			/* advance producer index and write pointer.*/
			tDma_ring->tdma_producer_index += 1;
			tDma_ring->tdma_write_pointer = (write_ptr << 1);
			/* update stats */
			dev->stats.tx_bytes += frag->size;
		}
		dev->stats.tx_packets++;
	}

#if defined(CONFIG_BRCM_GENET_V2) && defined(CONFIG_NET_SCH_MULTIQ)
	if (index == DESC_INDEX) {
		if (pDevCtrl->txFreeBds <= (MAX_SKB_FRAGS + 1))
			netif_stop_subqueue(dev, 0);
	} else if (pDevCtrl->txRingFreeBds[index] <= (MAX_SKB_FRAGS + 1)) {
		netif_stop_subqueue(dev, index);
	}
	/* Enable Tx bdone/pdone interrupt if any subqueue is stopped*/
	if (netif_any_subqueue_stopped(dev))
		pDevCtrl->intrl2_0->cpu_mask_clear |= UMAC_IRQ_TXDMA_BDONE |
			UMAC_IRQ_TXDMA_PDONE;
#else
	if (pDevCtrl->txFreeBds <= (MAX_SKB_FRAGS + 1)) {
		/* Enable Tx bdone/pdone interrupt !*/
		pDevCtrl->intrl2_0->cpu_mask_clear |= UMAC_IRQ_TXDMA_BDONE |
			UMAC_IRQ_TXDMA_PDONE;
		netif_stop_queue(dev);
	}
#endif
	dev->trans_start = jiffies;

	spin_unlock_irqrestore(&pDevCtrl->lock, flags);

	return 0;
}
/* --------------------------------------------------------------------------
Name: bcmgenet_tx_ring_reclaim
Purpose: reclaim xmited skb for a ring buffer
-------------------------------------------------------------------------- */
static void bcmgenet_tx_ring_reclaim(struct net_device *dev, int index,
		unsigned int p_index, unsigned int c_index)
{
	struct BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
	struct Enet_CB *txCBPtr;
	int lastTxedCnt = 0, lastCIndex = 0;
	struct sk_buff *skb;

	/* Compute how many buffers are transmited since last xmit call */

	if (c_index >= pDevCtrl->txRingCIndex[index]) {
		/* index not wrapped */
		lastTxedCnt = c_index - pDevCtrl->txRingCIndex[index];
	} else {
		/* index wrapped */
		lastTxedCnt = pDevCtrl->txRingSize[index] -
			pDevCtrl->txRingCIndex[index] + c_index;
	}
	TRACE(("%s: ring %d: p_index=%d c_index=%d"
			"lastTxedCnt=%d txLastCIndex=%d\n",
			__func__, index, p_index, c_index,
			lastTxedCnt, pDevCtrl->txRingCIndex[index]));

	pDevCtrl->txRingFreeBds[index] += lastTxedCnt;

	lastCIndex = pDevCtrl->txRingCIndex[index];
	pDevCtrl->txRingCIndex[index] = c_index;

	/* free xmited skb */
	while (lastTxedCnt-- > 0) {
		txCBPtr = pDevCtrl->txRingCBs[index] + lastCIndex;
		skb = txCBPtr->skb;
		if (skb != NULL) {
			/*
			 * This will consume skb, we don't want to run
			 * destructor which is to drop the skb.
			 */
			if (skb->destructor != NULL)
				skb->destructor = NULL;
			/* make sure dev_kfree_skb_any() don't free mem. */
			if ((atomic_read(&skb_shinfo(skb)->dataref) &
						SKB_DATAREF_MASK) < 2)
				atomic_set(&(skb_shinfo(skb)->dataref), 2);
			dev_kfree_skb_any(skb);
			txCBPtr->skb = NULL;
		}
		if (lastCIndex == (pDevCtrl->txRingSize[index] - 1))
			lastCIndex = 0;
		else
			lastCIndex++;
	}
	if (pDevCtrl->txRingFreeBds[index] > 0 &&
			netif_queue_stopped(dev)) {
		/*
		 * Disable txdma multibuf done interrupt for this ring
		 * since we have free tx bds.
		 */
		pDevCtrl->intrl2_1->cpu_mask_set |= (1 << index);
		netif_wake_queue(dev);
	}
}
/* --------------------------------------------------------------------------
Name: bcmgenet_ring_xmit
Purpose: Send ethernet traffic through ring buffer
-------------------------------------------------------------------------- */
int __maybe_unused bcmgenet_ring_xmit(struct sk_buff *skb,
		struct net_device *dev, int index, int drop)
{
	struct BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
	volatile struct tDmaRingRegs *tDma_ring;
	struct Enet_CB *txCBPtr;
	struct status_64 *Status;
	unsigned int p_index = 0, c_index = 0;

	/* Compute how many buffers are transmited since last xmit call */
	tDma_ring = &pDevCtrl->txDma->tDmaRings[index];
	p_index = (DMA_P_INDEX_MASK & tDma_ring->tdma_producer_index);
	c_index = (DMA_C_INDEX_MASK & tDma_ring->tdma_consumer_index);

	/* P/C index is 16 bits, we do modulo of RING_SIZE */
	p_index &= (pDevCtrl->txRingSize[index] - 1);
	c_index &= (pDevCtrl->txRingSize[index] - 1);

	bcmgenet_tx_ring_reclaim(dev, index, p_index, c_index);

	if (!skb)
		return 0;
	/* Obtain a tx control block */
	txCBPtr = pDevCtrl->txRingCBs[index] + p_index;
	txCBPtr->skb = skb;

	TRACE(("%s: txCBPtr=0x%08lx skb=0x%08lx skb->head=0x%08lx\n",
		__func__,
		(unsigned long)txCBPtr,
		(unsigned long)skb,
		(unsigned long)skb->head));

	/*
	 * Make sure we have headroom for us to insert 64B status block.
	 */
	if (unlikely(skb_headroom(skb) < 64)) {
		printk(KERN_ERR "no enough headroom for TSB (head=0x%08x)\n",
				(unsigned int)skb->head);
		BUG();
	}
	Status = (struct status_64 *)skb->head;
	Status->length_status = ((unsigned long)((skb->len < ETH_ZLEN) ?
				ETH_ZLEN : skb->len)) << 16;
	Status->length_status += (sizeof(struct status_64) << 16);
	Status->length_status |= DMA_SOP | DMA_EOP | DMA_TX_APPEND_CRC;
	if ((skb->ip_summed  == CHECKSUM_PARTIAL) &&
			(GENET_TBUF_CTRL(pDevCtrl) & RBUF_64B_EN)) {
		u16 offset;
		offset = skb->csum_start - skb_headroom(skb) - 64;
		/* Insert 64B TSB and set the flag */
		Status->tx_csum_info = (offset << STATUS_TX_CSUM_START_SHIFT) |
			(offset + skb->csum_offset) | STATUS_TX_CSUM_LV;
		Status->length_status |= DMA_TX_DO_CSUM;
		TRACE(("Tx Hw Csum: head=0x%08x data=0x%08x "
				"csum_start=%d csum_offset=%d\n",
				(unsigned int)skb->head,
				(unsigned int)skb->data,
				skb->csum_start,
				skb->csum_offset));
	} else {
		Status->tx_csum_info = 0;
	}
	/* Default QTAG for MoCA */
	Status->length_status |= (DMA_TX_QTAG_MASK << DMA_TX_QTAG_SHIFT);
	txCBPtr->dma_addr = dma_map_single(&pDevCtrl->dev->dev,
			skb->head, skb->len + 64, DMA_TO_DEVICE);

#ifdef CONFIG_BCMGENET_DUMP_DATA
	printk(KERN_NOTICE "bcmgenet_xmit: len %d", skb->len);
	print_hex_dump(KERN_NOTICE, "", DUMP_PREFIX_ADDRESS,
				16, 1, skb->head, skb->len + 64, 0);
#endif

	/*
	 * Decrement total BD count and advance our
	 * write pointer/producer index
	 */
	pDevCtrl->txRingFreeBds[index] -= 1;

	if (likely(txCBPtr->dma_addr == tDma_ring->tdma_write_pointer)) {
		unsigned long start_addr = tDma_ring->tdma_start_addr;
		if (unlikely(drop)) {
			/*
			 * Don't xmit current packet pointed by read_pointer,
			 * there is no such mechanism in GENET's TDMA, so we
			 * disable TDMA and increment consumer index/read
			 * pointer to skip this packet as a work around.
			 */
			pDevCtrl->txDma->tdma_ctrl &= ~DMA_EN;
			tDma_ring->tdma_consumer_index += 1;
			if ((tDma_ring->tdma_read_pointer + RX_BUF_LENGTH) >
				tDma_ring->tdma_end_addr) {
				tDma_ring->tdma_read_pointer = start_addr;
			} else {
				tDma_ring->tdma_read_pointer += RX_BUF_LENGTH;
			}
		}
		/* advance producer index and write pointer.*/
		tDma_ring->tdma_producer_index += 1;
		if ((tDma_ring->tdma_write_pointer + RX_BUF_LENGTH) >
			tDma_ring->tdma_end_addr) {
			tDma_ring->tdma_write_pointer = start_addr;
		} else {
			tDma_ring->tdma_write_pointer += RX_BUF_LENGTH;
		}
		if (unlikely(drop))
			pDevCtrl->txDma->tdma_ctrl |= DMA_EN;
	} else {
		/* ooops! how can we get here ?*/
		BUG();
	}

	if (pDevCtrl->txRingFreeBds[index] == 0) {
		TRACE(("%s: no xmit queue space, stopping queue\n", dev->name));
		/* Enable Tx bdone/pdone interrupt !*/
		pDevCtrl->intrl2_0->cpu_mask_clear |= (1 << index);
		netif_stop_subqueue(dev, index);
	}

	if (!drop) {
		/* update stats */
		dev->stats.tx_bytes += ((skb->len < ETH_ZLEN) ?
			ETH_ZLEN : skb->len);
		dev->stats.tx_packets++;
	}
	dev->trans_start = jiffies;
	return 0;
}
/* NAPI polling method*/
static int bcmgenet_poll(struct napi_struct *napi, int budget)
{
	struct BcmEnet_devctrl *pDevCtrl = container_of(napi,
			struct BcmEnet_devctrl, napi);
	volatile struct intrl2Regs *intrl2 = pDevCtrl->intrl2_0;
	volatile struct rDmaRingRegs *rDma_desc;
	unsigned int work_done;
	work_done = bcmgenet_desc_rx(pDevCtrl, budget);

	/* tx reclaim */
	bcmgenet_xmit(NULL, pDevCtrl->dev);
	/* Allocate new SKBs for the BD ring */
	assign_rx_buffers(pDevCtrl);
	/* Advancing our read pointer and consumer index*/
	rDma_desc = &pDevCtrl->rxDma->rDmaRings[DESC_INDEX];
	rDma_desc->rdma_consumer_index += work_done;
	rDma_desc->rdma_read_pointer += (work_done << 1);
	rDma_desc->rdma_read_pointer &= ((TOTAL_DESC << 1)-1);
	if (work_done < budget) {
		napi_complete(napi);
		intrl2->cpu_mask_clear |= UMAC_IRQ_RXDMA_BDONE;
	}
	return work_done;
}
/*
 * NAPI polling for ring buffer.
 */
static int bcmgenet_ring_poll(struct napi_struct *napi, int budget)
{
	struct BcmEnet_devctrl *pDevCtrl = container_of(napi,
			struct BcmEnet_devctrl, ring_napi);
	volatile struct intrl2Regs *intrl2 = pDevCtrl->intrl2_1;
	unsigned int work_done;
	work_done = bcmgenet_ring_rx(pDevCtrl, budget);

	/* tx reclaim */
	bcmgenet_ring_xmit(NULL, pDevCtrl->dev, 0, 0);
	if (work_done < budget) {
		unsigned long bits;
		napi_complete(napi);
		bits = (pDevCtrl->rxDma->rdma_ctrl >> 1) << 16;
		intrl2->cpu_mask_clear |= bits;
	}
	return work_done;
}
/*
 * Interrupt bottom half
 */
static void bcmgenet_irq_task(struct work_struct *work)
{
	struct BcmEnet_devctrl *pDevCtrl = container_of(
			work, struct BcmEnet_devctrl, bcmgenet_irq_work);
	struct net_device *dev;

	dev = pDevCtrl->dev;

	TRACE(("%s\n", __func__));
	/* Cable plugged/unplugged event */
	if (pDevCtrl->irq0_stat & UMAC_IRQ_PHY_DET_R) {
		pDevCtrl->irq0_stat &= ~UMAC_IRQ_PHY_DET_R;
		printk(KERN_CRIT "%s cable plugged in, powering up\n",
				pDevCtrl->dev->name);
		bcmgenet_power_up(pDevCtrl, GENET_POWER_CABLE_SENSE);
	} else if (pDevCtrl->irq0_stat & UMAC_IRQ_PHY_DET_F) {
		pDevCtrl->irq0_stat &= ~UMAC_IRQ_PHY_DET_F;
		printk(KERN_CRIT "%s cable unplugged, powering down\n",
				pDevCtrl->dev->name);
		bcmgenet_power_down(pDevCtrl, GENET_POWER_CABLE_SENSE);
	}
	if (pDevCtrl->irq0_stat & UMAC_IRQ_MPD_R) {
		pDevCtrl->irq0_stat &= ~UMAC_IRQ_MPD_R;
		printk(KERN_CRIT "%s magic packet detected, waking up\n",
				pDevCtrl->dev->name);
		/* disable mpd interrupt */
		pDevCtrl->intrl2_0->cpu_mask_set |= UMAC_IRQ_MPD_R;
		/* disable CRC forward.*/
		pDevCtrl->umac->cmd &= ~CMD_CRC_FWD;
		bcmgenet_power_up(pDevCtrl, GENET_POWER_WOL_MAGIC);

	} else if (pDevCtrl->irq0_stat & (UMAC_IRQ_HFB_SM | UMAC_IRQ_HFB_MM)) {
		pDevCtrl->irq0_stat &= ~(UMAC_IRQ_HFB_SM | UMAC_IRQ_HFB_MM);
		printk(KERN_CRIT "%s ACPI pattern matched, waking up\n",
				pDevCtrl->dev->name);
		/* disable HFB match interrupts */
		pDevCtrl->intrl2_0->cpu_mask_set |= (UMAC_IRQ_HFB_SM |
				UMAC_IRQ_HFB_MM);
		bcmgenet_power_up(pDevCtrl, GENET_POWER_WOL_ACPI);
	}

	/* Link UP/DOWN event */
	if (pDevCtrl->irq0_stat & UMAC_IRQ_LINK_UP) {
		printk(KERN_CRIT "%s Link UP.\n", pDevCtrl->dev->name);
		/* Clear soft-copy of irq status*/
		pDevCtrl->irq0_stat &= ~UMAC_IRQ_LINK_UP;

		if (!(pDevCtrl->umac->cmd & CMD_AUTO_CONFIG)) {
			printk(KERN_CRIT "Auto config phy\n");
			mii_setup(pDevCtrl->dev);
		}
		if (!netif_carrier_ok(pDevCtrl->dev)) {
			pDevCtrl->dev->flags |= IFF_RUNNING;
			netif_carrier_on(pDevCtrl->dev);
			rtmsg_ifinfo(RTM_NEWLINK, pDevCtrl->dev, IFF_RUNNING);
		}

	} else if (pDevCtrl->irq0_stat & UMAC_IRQ_LINK_DOWN) {
		printk(KERN_CRIT "%s Link DOWN.\n", pDevCtrl->dev->name);
		/* clear soft-copy of irq status */
		pDevCtrl->irq0_stat &= ~UMAC_IRQ_LINK_DOWN;
		/* TODO:Disable DMA Rx channels.  */
		/* In case there are packets in the Rx descriptor */
		if (netif_carrier_ok(pDevCtrl->dev)) {
			netif_carrier_off(pDevCtrl->dev);
			pDevCtrl->dev->flags &= ~IFF_RUNNING;
			rtmsg_ifinfo(RTM_DELLINK, pDevCtrl->dev, IFF_RUNNING);
		}
	}
}
/*
 * bcmgenet_ring_rx: ring buffer rx function.
 */
static unsigned int bcmgenet_ring_rx(void *ptr, unsigned int budget)
{
	struct BcmEnet_devctrl *pDevCtrl = ptr;
	volatile struct status_64 *status;
	volatile struct rDmaRingRegs *rDma_ring;
	int i, len, rx_discard_flag = 0;
	struct Enet_CB *cb;
	struct sk_buff *skb;
	unsigned long dmaFlag;
	unsigned int rxpktprocessed = 0, pktcnt = 0, retvalue = 0;
	unsigned int read_ptr = 0, write_ptr = 0, p_index = 0, c_index = 0;

	TRACE(("%s: ifindex=%d irq_stat=0x%08x\n",
		__func__, pDevCtrl->dev->ifindex, pDevCtrl->irq1_stat));

	/* loop for each ring */
	for (i = 0; i < GENET_RX_RING_COUNT; i++) {
		/* Skip if this ring is not eanbled*/
		if (!(pDevCtrl->rxDma->rdma_ctrl &
				(1 << (i + DMA_RING_BUF_EN_SHIFT))))
			continue;
		/* Skip if not outstanding packet for this ring*/
		if (!(pDevCtrl->irq1_stat & (1 << (16 + i))))
			continue;

		rDma_ring = &pDevCtrl->rxDma->rDmaRings[i];
		write_ptr = rDma_ring->rdma_write_pointer;
		read_ptr =  rDma_ring->rdma_read_pointer;
		p_index = rDma_ring->rdma_producer_index;
		p_index &= DMA_P_INDEX_MASK;
		c_index = rDma_ring->rdma_consumer_index;
		c_index &= DMA_C_INDEX_MASK;

		if (p_index < c_index) {
			/* index wrapped */
			if ((DMA_P_INDEX_MASK - c_index + p_index) ==
					(pDevCtrl->rxRingSize[i] - 1))
				rx_discard_flag = 1;
		} else if (p_index > c_index) {
			/* index not wrapped */
			if (p_index - c_index == pDevCtrl->rxRingSize[i])
				rx_discard_flag = 1;
		}

		if (rx_discard_flag) {
			int discard_cnt = rDma_ring->rdma_producer_index >> 16;
			/* Report rx overrun errors */
			pDevCtrl->dev->stats.rx_over_errors += discard_cnt -
				pDevCtrl->rxRingDiscCnt[i];
			pDevCtrl->rxRingDiscCnt[i] = discard_cnt;
		}

		/*
		 * We can't use produer/consumer index to compute how
		 * many outstanding packets are there, because we are not
		 * advancing consumer index right after packets are moved
		 * out of DMA. So we use read/write pointer for the math.
		 */
		if (write_ptr < read_ptr) {
			/* pointer wrapped */
			pktcnt = (rDma_ring->rdma_end_addr + 1 - read_ptr) >>
				(RX_BUF_BITS - 1);
			pktcnt += (write_ptr - rDma_ring->rdma_start_addr) >>
				(RX_BUF_BITS - 1);
		} else if (write_ptr > read_ptr) {
			/* pointer not wrapped */
			pktcnt = (write_ptr - read_ptr) >> (RX_BUF_BITS - 1);
		} else if (write_ptr == read_ptr && p_index != c_index) {
			/* overflowed, some packets are discarded by DMA */
			pktcnt = rDma_ring->rdma_ring_buf_size >> 16;
		}

		TRACE(("%s: p_index=%d c_index=%d write_ptr=0x%08x "
				"read_ptr=0x%08x pktcnt=%d\n",
				__func__, p_index, c_index, write_ptr,
				read_ptr, pktcnt));

		/*Start processing packets */
		while ((rxpktprocessed < pktcnt) &&
				(rxpktprocessed < budget)) {

			unsigned int cbi;
			/*
			 * Find out Which buffer in the ring are we pointing to.
			 */
			cbi = (read_ptr - rDma_ring->rdma_start_addr) >>
				(RX_BUF_BITS - 1);
			cb = pDevCtrl->rxRingCbs[i] + cbi;
			dma_cache_inv((unsigned long)(cb->BdAddr), 64);
			status = (struct status_64 *)cb->BdAddr;
			dmaFlag = status->length_status & 0xffff;
			len = status->length_status >> 16;

			dma_cache_inv((unsigned long)(cb->BdAddr) + 64, len);

			/*
			 * Advancing our read pointer.
			 */
			if (read_ptr + RX_BUF_LENGTH > rDma_ring->rdma_end_addr)
				read_ptr = rDma_ring->rdma_start_addr;
			else
				read_ptr += RX_BUF_LENGTH;
			rDma_ring->rdma_read_pointer = read_ptr;
			/*
			 * per packet processing
			 */
			skb = __bcmgenet_alloc_skb_from_buf(
				(unsigned char *)cb->BdAddr, RX_BUF_LENGTH, 0);
			rxpktprocessed++;
			BUG_ON(skb == NULL);

			TRACE(("%s: cbi=%d skb=0x%08x head=0x%08x dataref=%d\n",
				__func__, cbi,
				(unsigned int)skb, (unsigned int)skb->head,
				(atomic_read(&skb_shinfo(skb)->dataref) &
				 SKB_DATAREF_MASK)));
			/* report errors */
			if (unlikely(!(dmaFlag & DMA_EOP) ||
					!(dmaFlag & DMA_SOP))) {
				/* probably can't do this for scater gather ?*/
				printk(KERN_WARNING "Droping fragmented packet!\n");
				pDevCtrl->dev->stats.rx_dropped++;
				pDevCtrl->dev->stats.rx_errors++;
				dev_kfree_skb_any(cb->skb);
				cb->skb = NULL;
				continue;
			}
			if (unlikely(dmaFlag & (DMA_RX_CRC_ERROR |
							DMA_RX_OV |
							DMA_RX_NO |
							DMA_RX_LG |
							DMA_RX_RXER))) {
				TRACE(("ERROR: dmaFlag=0x%lx\n", dmaFlag));
				if (dmaFlag & DMA_RX_CRC_ERROR)
					pDevCtrl->dev->stats.rx_crc_errors++;
				if (dmaFlag & DMA_RX_OV)
					pDevCtrl->dev->stats.rx_over_errors++;
				if (dmaFlag & DMA_RX_NO)
					pDevCtrl->dev->stats.rx_frame_errors++;
				if (dmaFlag & DMA_RX_LG)
					pDevCtrl->dev->stats.rx_length_errors++;

				pDevCtrl->dev->stats.rx_dropped++;
				pDevCtrl->dev->stats.rx_errors++;
				dev_kfree_skb_any(cb->skb);
				cb->skb = NULL;
				continue;
			} /* error packet */

			skb_put(skb, len);
			/* we must have 64B rx status block enabled.*/
			if (pDevCtrl->rbuf->rbuf_chk_ctrl & RBUF_RXCHK_EN) {
				if (status->rx_csum & STATUS_RX_CSUM_OK) {
					skb->csum = status->rx_csum ;
					/*
					 * Should swap bytes based on
					 * rbuf->endian_ctrl?
					 */
					skb->csum = swab16(skb->csum);
				}
				skb->ip_summed = CHECKSUM_COMPLETE;
			}
			/*
			 * TODO: check filter index and compare with ring index
			 * Report error if not matched
			 */
			skb_pull(skb, 64);
			len -= 64;

			if (pDevCtrl->bIPHdrOptimize) {
				skb_pull(skb, 2);
				len -= 2;
			}

			if (pDevCtrl->umac->cmd & CMD_CRC_FWD) {
				skb_trim(skb, len - 4);
				len -= 4;
			}
#ifdef CONFIG_BCMGENET_DUMP_DATA
			printk(KERN_NOTICE "%s:\n", __func__);
			print_hex_dump(KERN_NOTICE, "", DUMP_PREFIX_ADDRESS,
				16, 1, skb->data, skb->len, 0);
#endif
			/*
			 * Finish setting up the received SKB and send it
			 * to the kernel
			 */
			skb->dev = pDevCtrl->dev;
			skb->protocol = eth_type_trans(skb, pDevCtrl->dev);
			pDevCtrl->dev->stats.rx_packets++;
			pDevCtrl->dev->stats.rx_bytes += len;
			if (dmaFlag & DMA_RX_MULT)
				pDevCtrl->dev->stats.multicast++;

			skb->queue_mapping = i;
			/* Notify kernel */
			netif_receive_skb(skb);
			TRACE(("pushed up to kernel\n"));

		} /* packet process loop */

	} /* ring index loop */

	if (retvalue == 0)
		retvalue = rxpktprocessed;

	return retvalue;;
}
/*
 * bcmgenet_isr1: interrupt handler for ring buffer.
 */
static irqreturn_t bcmgenet_isr1(int irq, void *dev_id)
{
	struct BcmEnet_devctrl *pDevCtrl = dev_id;
	volatile struct intrl2Regs *intrl2 = pDevCtrl->intrl2_1;

	/* Save irq status for bottom-half processing. */
	pDevCtrl->irq1_stat = intrl2->cpu_stat & ~intrl2->cpu_mask_status;
	/* clear inerrupts*/
	intrl2->cpu_clear |= pDevCtrl->irq1_stat;

	TRACE(("%s: IRQ=0x%x\n", __func__, pDevCtrl->irq1_stat));
	if (pDevCtrl->irq1_stat & 0xffff0000) {
		/*
		 * We use NAPI here, because of the fact that we are NOT
		 * advancing consumer index right after data moved out of
		 * DMA, instead we advance it only when we found out upper
		 * level has consumed it.
		 */
		if (likely(napi_schedule_prep(&pDevCtrl->ring_napi))) {
			/* Disable all rx ring interrupt */
			intrl2->cpu_mask_set |= 0xffff0000;
			__napi_schedule(&pDevCtrl->ring_napi);
		}
	}
	return IRQ_HANDLED;
}
/*
 * bcmgenet_isr0: Handle various interrupts.
 */
static irqreturn_t bcmgenet_isr0(int irq, void *dev_id)
{
	struct BcmEnet_devctrl *pDevCtrl = dev_id;
	volatile struct intrl2Regs *intrl2 = pDevCtrl->intrl2_0;

	/* Save irq status for bottom-half processing. */
	pDevCtrl->irq0_stat = intrl2->cpu_stat & ~intrl2->cpu_mask_status;
	/* clear inerrupts*/
	intrl2->cpu_clear |= pDevCtrl->irq0_stat;

	TRACE(("IRQ=0x%x\n", pDevCtrl->irq0_stat));
#ifndef CONFIG_BCMGENET_RX_DESC_THROTTLE
	if (pDevCtrl->irq0_stat & (UMAC_IRQ_RXDMA_BDONE|UMAC_IRQ_RXDMA_PDONE)) {
		/*
		 * We use NAPI(software interrupt throttling, if
		 * Rx Descriptor throttling is not used.
		 * Disable interrupt, will be enabled in the poll method.
		 */
		if (likely(napi_schedule_prep(&pDevCtrl->napi))) {
			intrl2->cpu_mask_set |= UMAC_IRQ_RXDMA_BDONE;
			__napi_schedule(&pDevCtrl->napi);
		}
	}
#else
	/* Multiple buffer done event. */
	if (pDevCtrl->irq0_stat & UMAC_IRQ_RXDMA_MBDONE) {
		unsigned int work_done;
		volatile struct rDmaRingRegs *rDma_desc;

		rDma_desc = &pDevCtrl->rxDma->rDmaRings[DESC_INDEX];
		pDevCtrl->irq0_stat &= ~UMAC_IRQ_RXDMA_MBDONE;
		TRACE(("%s: %d packets available\n", __func__, DmaDescThres));
		work_done = bcmgenet_desc_rx(pDevCtrl, DmaDescThres);
		/* Allocate new SKBs for the BD ring */
		assign_rx_buffers(pDevCtrl);
		rDma_desc->rdma_consumer_index += work_done;
		rDma_desc->rdma_read_pointer += (work_done << 1);
		rDma_desc->rdma_read_pointer &= ((TOTAL_DESC << 1)-1);
	}
#endif
	if (pDevCtrl->irq0_stat &
			(UMAC_IRQ_TXDMA_BDONE | UMAC_IRQ_TXDMA_PDONE)) {
		/* Tx reclaim */
		bcmgenet_xmit(NULL, pDevCtrl->dev);
	}
	if (pDevCtrl->irq0_stat & (UMAC_IRQ_PHY_DET_R |
				UMAC_IRQ_PHY_DET_F |
				UMAC_IRQ_LINK_UP |
				UMAC_IRQ_LINK_DOWN |
				UMAC_IRQ_HFB_SM |
				UMAC_IRQ_HFB_MM |
				UMAC_IRQ_MPD_R)) {
		/* all other interested interrupts handled in bottom half */
		schedule_work(&pDevCtrl->bcmgenet_irq_work);
	}

	return IRQ_HANDLED;
}
/*
 *  bcmgenet_desc_rx - descriptor based rx process.
 *  this could be called from bottom half, or from NAPI polling method.
 */
static unsigned int bcmgenet_desc_rx(void *ptr, unsigned int budget)
{
	struct BcmEnet_devctrl *pDevCtrl = ptr;
	struct net_device *dev = pDevCtrl->dev;
	struct Enet_CB *cb;
	struct sk_buff *skb;
	unsigned long dmaFlag;
	int len;
	unsigned int rxpktprocessed = 0, rxpkttoprocess = 0;
	unsigned int p_index = 0, c_index = 0, read_ptr = 0;

	p_index = pDevCtrl->rxDma->rDmaRings[DESC_INDEX].rdma_producer_index;
	p_index &= DMA_P_INDEX_MASK;
	c_index = pDevCtrl->rxDma->rDmaRings[DESC_INDEX].rdma_consumer_index;
	c_index &= DMA_C_INDEX_MASK;
	read_ptr = pDevCtrl->rxDma->rDmaRings[DESC_INDEX].rdma_read_pointer;
	read_ptr = ((read_ptr & DMA_RW_POINTER_MASK) >> 1);

	if (p_index < c_index)
		rxpkttoprocess = (DMA_C_INDEX_MASK+1) - c_index + p_index;
	else
		rxpkttoprocess = p_index - c_index;
	TRACE(("RDMA: rxpkttoprocess=%d\n", rxpkttoprocess));

	while ((rxpktprocessed < rxpkttoprocess) &&
			(rxpktprocessed < budget)) {

		dmaFlag = (pDevCtrl->rxBds[read_ptr].length_status & 0xffff);
		len = ((pDevCtrl->rxBds[read_ptr].length_status)>>16);

		TRACE(("%s:p_index=%d c_index=%d read_ptr=%d "
			"len_stat=0x%08lx\n",
			__func__, p_index, c_index, read_ptr,
			pDevCtrl->rxBds[read_ptr].length_status));

		rxpktprocessed++;

		cb = &pDevCtrl->rxCbs[read_ptr];
		skb = cb->skb;
		BUG_ON(skb == NULL);

		pDevCtrl->rxBds[read_ptr].address = 0;

		if (read_ptr == pDevCtrl->nrRxBds-1)
			read_ptr = 0;
		else
			read_ptr++;

		if (unlikely(!(dmaFlag & DMA_EOP) || !(dmaFlag & DMA_SOP))) {
			printk(KERN_WARNING "Droping fragmented packet!\n");
			dev->stats.rx_dropped++;
			dev->stats.rx_errors++;
			dev_kfree_skb_any(cb->skb);
			continue;
		}
		/* report errors */
		if (unlikely(dmaFlag & (DMA_RX_CRC_ERROR |
						DMA_RX_OV |
						DMA_RX_NO |
						DMA_RX_LG |
						DMA_RX_RXER))) {
			TRACE(("ERROR: dmaFlag=0x%x\n", (unsigned int)dmaFlag));
			if (dmaFlag & DMA_RX_CRC_ERROR)
				dev->stats.rx_crc_errors++;
			if (dmaFlag & DMA_RX_OV)
				dev->stats.rx_over_errors++;
			if (dmaFlag & DMA_RX_NO)
				dev->stats.rx_frame_errors++;
			if (dmaFlag & DMA_RX_LG)
				dev->stats.rx_length_errors++;
			dev->stats.rx_dropped++;
			dev->stats.rx_errors++;

			/* discard the packet and advance consumer index.*/
			dev_kfree_skb_any(cb->skb);
			cb->skb = NULL;
			continue;
		} /* error packet */

		skb_put(skb, len);
		if (pDevCtrl->rbuf->rbuf_ctrl & RBUF_64B_EN) {
			struct status_64 *status;
			status = (struct status_64 *)skb->data;
			/* we have 64B rx status block enabled.*/
			if (pDevCtrl->rbuf->rbuf_chk_ctrl & RBUF_RXCHK_EN) {
				if (status->rx_csum & STATUS_RX_CSUM_OK)
					skb->ip_summed = CHECKSUM_UNNECESSARY;
				else
					skb->ip_summed = CHECKSUM_NONE;
			}
			skb_pull(skb, 64);
			len -= 64;
		}

		if (pDevCtrl->bIPHdrOptimize) {
			skb_pull(skb, 2);
			len -= 2;
		}

		if (pDevCtrl->umac->cmd & CMD_CRC_FWD) {
			skb_trim(skb, len - 4);
			len -= 4;
		}
#ifdef CONFIG_BCMGENET_DUMP_DATA
		printk(KERN_NOTICE "bcmgenet_desc_rx : len=%d", skb->len);
		print_hex_dump(KERN_NOTICE, "", DUMP_PREFIX_ADDRESS,
			16, 1, skb->data, skb->len, 0);
#endif

		/*Finish setting up the received SKB and send it to the kernel*/
		skb->dev = pDevCtrl->dev;
		skb->protocol = eth_type_trans(skb, pDevCtrl->dev);
		dev->stats.rx_packets++;
		dev->stats.rx_bytes += len;
		if (dmaFlag & DMA_RX_MULT)
			dev->stats.multicast++;

		/* Notify kernel */
#ifdef CONFIG_BCMGENET_RX_DESC_THROTTLE
		netif_rx(skb);
#else
		netif_receive_skb(skb);
#endif
		cb->skb = NULL;
		TRACE(("pushed up to kernel\n"));
	}

	return rxpktprocessed;
}


/*
 * assign_rx_buffers:
 * Assign skb to RX DMA descriptor.
 */
static int assign_rx_buffers(struct BcmEnet_devctrl *pDevCtrl)
{
	struct sk_buff *skb;
	unsigned short bdsfilled = 0;
	unsigned long flags;
	struct Enet_CB *cb;

	TRACE(("%s\n", __func__));

	/* This function may be called from irq bottom-half. */

#ifndef CONFIG_BCMGENET_RX_DESC_THROTTLE
	(void)flags;
	spin_lock_bh(&pDevCtrl->bh_lock);
#else
	spin_lock_irqsave(&pDevCtrl->lock, flags);
#endif

	/* loop here for each buffer needing assign */
	while (pDevCtrl->rxBdAssignPtr->address == 0) {
		cb = &pDevCtrl->rxCbs[pDevCtrl->rxBdAssignPtr-pDevCtrl->rxBds];
		skb = netdev_alloc_skb(pDevCtrl->dev,
				pDevCtrl->rxBufLen + SKB_ALIGNMENT);
		if (!skb) {
			printk(KERN_ERR " failed to allocate skb for rx\n");
			break;
		}
		handleAlignment(pDevCtrl, skb);

		/* keep count of any BD's we refill */
		bdsfilled++;
		cb->skb = skb;
		cb->dma_addr = dma_map_single(&pDevCtrl->dev->dev,
			skb->data, pDevCtrl->rxBufLen, DMA_FROM_DEVICE);
		/* assign packet, prepare descriptor, and advance pointer */
		pDevCtrl->rxBdAssignPtr->address = cb->dma_addr;
		pDevCtrl->rxBdAssignPtr->length_status =
			(pDevCtrl->rxBufLen << 16);

		/* turn on the newly assigned BD for DMA to use */
		if (pDevCtrl->rxBdAssignPtr ==
				pDevCtrl->rxBds+pDevCtrl->nrRxBds-1)
			pDevCtrl->rxBdAssignPtr = pDevCtrl->rxBds;
		else
			pDevCtrl->rxBdAssignPtr++;

	}

	/* Enable rx DMA incase it was disabled due to running out of rx BD */
	pDevCtrl->rxDma->rdma_ctrl |= DMA_EN;

#ifndef CONFIG_BCMGENET_RX_DESC_THROTTLE
	spin_unlock_bh(&pDevCtrl->bh_lock);
#else
	spin_unlock_irqrestore(&pDevCtrl->lock, flags);
#endif

	TRACE(("%s return bdsfilled=%d\n", __func__, bdsfilled));
	return bdsfilled;
}

/*
 * init_umac: Initializes the uniMac controller
 */
static int init_umac(struct BcmEnet_devctrl *pDevCtrl)
{
	volatile struct uniMacRegs *umac;
	volatile struct intrl2Regs *intrl2;
	struct net_device *dev = pDevCtrl->dev;

	umac = pDevCtrl->umac;;
	intrl2 = pDevCtrl->intrl2_0;

	TRACE(("bcmgenet: init_umac "));

	/* disable MAC while updating its registers */
	umac->cmd = 0 ;

	/* issue soft reset, wait for it to complete */
	umac->cmd = CMD_SW_RESET;
	udelay(1000);
	umac->cmd = 0;
	/* clear tx/rx counter */
	umac->mib_ctrl = MIB_RESET_RX | MIB_RESET_TX | MIB_RESET_RUNT;
	umac->mib_ctrl = 0;

#ifdef MAC_LOOPBACK
	/* Enable GMII/MII loopback */
	umac->cmd |= CMD_LCL_LOOP_EN;
#endif
	umac->max_frame_len = ENET_MAX_MTU_SIZE;
	/*
	 * init rx registers, enable ip header optimization.
	 */
	if (pDevCtrl->bIPHdrOptimize)
		pDevCtrl->rbuf->rbuf_ctrl |= RBUF_ALIGN_2B ;

	umac->mac_0 = (dev->dev_addr[0] << 24 |
			dev->dev_addr[1] << 16 |
			dev->dev_addr[2] << 8  |
			dev->dev_addr[3]);
	umac->mac_1 = dev->dev_addr[4] << 8 | dev->dev_addr[5];

	/* Mask all interrupts.*/
	intrl2->cpu_mask_set = 0xFFFFFFFF;
	intrl2->cpu_clear = 0xFFFFFFFF;
	intrl2->cpu_mask_clear = 0x0;

#ifdef CONFIG_BCMGENET_RX_DESC_THROTTLE
	intrl2->cpu_mask_clear |= UMAC_IRQ_RXDMA_MBDONE;
#else
	intrl2->cpu_mask_clear |= UMAC_IRQ_RXDMA_BDONE;
	TRACE(("%s:Enabling RXDMA_BDONE interrupt\n", __func__));
#endif /* CONFIG_BCMGENET_RX_DESC_THROTTLE */

	/* Monitor cable plug/unpluged event for internal PHY */
	if (pDevCtrl->phyType == BRCM_PHY_TYPE_INT) {
		intrl2->cpu_mask_clear |= (UMAC_IRQ_PHY_DET_R |
				UMAC_IRQ_PHY_DET_F);
		intrl2->cpu_mask_clear |= (UMAC_IRQ_LINK_DOWN |
				UMAC_IRQ_LINK_UP);
		/* Turn on ENERGY_DET interrupt in bcmgenet_open()
		 * TODO: fix me for active standby.
		 */
	} else if (pDevCtrl->phyType == BRCM_PHY_TYPE_EXT_MII ||
			pDevCtrl->phyType == BRCM_PHY_TYPE_EXT_RGMII) {
		intrl2->cpu_mask_clear |= (UMAC_IRQ_LINK_DOWN |
				UMAC_IRQ_LINK_UP);

	} else if (pDevCtrl->phyType == BRCM_PHY_TYPE_MOCA) {
		/*bp_in_en: back-pressure enable */
		GENET_TBUF_BP_MC(pDevCtrl) |= (1 << 16);
		/* bp_mask: back pressure mask */
		GENET_TBUF_BP_MC(pDevCtrl) &= 0xFFFF0000;
	}

	/* Enable rx/tx engine.*/
	TRACE(("done init umac\n"));
	return 0;
}
/*
 * init_edma: Initialize DMA control register
 */
static void init_edma(struct BcmEnet_devctrl *pDevCtrl)
{
#ifdef CONFIG_BCMGENET_RX_DESC_THROTTLE
	int speeds[] = {10, 100, 1000, 2500};
	int sid = 1, timeout;
#endif
	volatile struct rDmaRingRegs *rDma_desc;
	volatile struct tDmaRingRegs *tDma_desc;
	TRACE(("bcmgenet: init_edma\n"));

	/* init rDma */
	pDevCtrl->rxDma->rdma_scb_burst_size = DMA_MAX_BURST_LENGTH;
	/* by default, enable ring 16 (descriptor based) */
	rDma_desc = &pDevCtrl->rxDma->rDmaRings[DESC_INDEX];
	rDma_desc->rdma_write_pointer = 0;
	rDma_desc->rdma_producer_index = 0;
	rDma_desc->rdma_consumer_index = 0;
	rDma_desc->rdma_ring_buf_size = ((TOTAL_DESC << DMA_RING_SIZE_SHIFT) |
		RX_BUF_LENGTH);
	rDma_desc->rdma_start_addr = 0;
	rDma_desc->rdma_end_addr = 2*TOTAL_DESC - 1;
	rDma_desc->rdma_xon_xoff_threshold = ((DMA_FC_THRESH_LO
			<< DMA_XOFF_THRESHOLD_SHIFT) |
			DMA_FC_THRESH_HI);
	rDma_desc->rdma_read_pointer = 0;

#ifdef CONFIG_BCMGENET_RX_DESC_THROTTLE
	/*
	 * Use descriptor throttle, fire irq when multiple packets are done!
	 */
	rDma_desc->rdma_mbuf_done_threshold = DMA_DESC_THRES;
	/*
	 * Enable push timer, force the IRQ_DESC_THROT to fire when timeout
	 * occurs, prevent system slow reponse when handling low throughput.
	 */
	sid = (pDevCtrl->umac->cmd >> CMD_SPEED_SHIFT) & CMD_SPEED_MASK;
	timeout = 2*(DMA_DESC_THRES*ENET_MAX_MTU_SIZE)/speeds[sid];
	pDevCtrl->rxDma->rdma_timeout[DESC_INDEX] = timeout & DMA_TIMEOUT_MASK;
#endif	/* CONFIG_BCMGENET_RX_DESC_THROTTLE */


	/* Init tDma */
	pDevCtrl->txDma->tdma_scb_burst_size = DMA_MAX_BURST_LENGTH;
	/* by default, enable ring DESC_INDEX (descriptor based) */
	tDma_desc = &pDevCtrl->txDma->tDmaRings[DESC_INDEX];
	tDma_desc->tdma_producer_index = 0;
	tDma_desc->tdma_consumer_index = 0;
	tDma_desc->tdma_mbuf_done_threshold = 0;
	/* Disable rate control for now */
	tDma_desc->tdma_flow_period = 0;
#if defined(CONFIG_BRCM_GENET_V2) && defined(CONFIG_NET_SCH_MULTIQ)
	/* Unclassified traffic goes to ring 16 */
	tDma_desc->tdma_ring_buf_size = ((GENET_DEFAULT_BD_CNT <<
				DMA_RING_SIZE_SHIFT) | RX_BUF_LENGTH);
	tDma_desc->tdma_start_addr = 2*(GENET_MQ_CNT*GENET_MQ_BD_CNT);
	tDma_desc->tdma_end_addr = 2*TOTAL_DESC - 1;
	tDma_desc->tdma_read_pointer = 2*(GENET_MQ_CNT*GENET_MQ_BD_CNT);
	tDma_desc->tdma_write_pointer = 2*(GENET_MQ_CNT*GENET_MQ_BD_CNT);
	pDevCtrl->txFreeBds = GENET_DEFAULT_BD_CNT;
	/* initiaize multi xmit queue */
	bcmgenet_init_multiq(pDevCtrl->dev);

#else
	tDma_desc->tdma_ring_buf_size = ((TOTAL_DESC << DMA_RING_SIZE_SHIFT) |
			RX_BUF_LENGTH);
	tDma_desc->tdma_start_addr = 0;
	tDma_desc->tdma_end_addr = 2*TOTAL_DESC - 1;
	tDma_desc->tdma_read_pointer = 0;
	tDma_desc->tdma_write_pointer = 0;
#endif

}
/*-----------------------------------------------------------------------------
 * exported function , Initialize ring buffer
 * dev: device pointer.
 * direction: 0 for rx 1 for tx.
 * id: ring index.
 * size: ring size, number of buffer in the ring, must be power of 2.
 * buf_len: buffer length, must be 32 bytes aligned, we assume 2Kb here.
 * buf: pointer to the buffer, continues memory, if *buf == NULL, buffer will
 * be allocated by this function.
 *----------------------------------------------------------------------------*/
int bcmgenet_init_ringbuf(struct net_device *dev, int direction,
		unsigned int id, unsigned int size,
		int buf_len, unsigned char **buf)
{
	int speeds[] = {10, 100, 1000, 2500};
	int sid = 1;
	int i, dma_enable, timeout;
	dma_addr_t dma_start;
	struct Enet_CB *cb;
	unsigned long flags;
	volatile struct rDmaRingRegs *rDmaRing;
	volatile struct tDmaRingRegs *tDmaRing;

	struct BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);

	if (id < 0 || id > 15 || size & (size - 1))
		return -EINVAL;

	spin_lock_irqsave(&pDevCtrl->lock, flags);

	if (direction == GENET_ALLOC_RX_RING) {
		if (*buf == NULL) {
			*buf = kmalloc(size * buf_len, GFP_KERNEL);
			if (*buf == NULL) {
				spin_unlock_irqrestore(&pDevCtrl->lock, flags);
				return -ENOMEM;
			}
		}
		cb  = kmalloc(size*sizeof(struct Enet_CB), GFP_KERNEL);
		if (cb == NULL) {
			kfree(buf);
			spin_unlock_irqrestore(&pDevCtrl->lock, flags);
			return -ENOMEM;
		}
		rDmaRing = &pDevCtrl->rxDma->rDmaRings[id];
		pDevCtrl->rxRingCbs[id] = cb;
		pDevCtrl->rxRingSize[id] = size;
		pDevCtrl->rxRingCIndex[id] = 0;
		pDevCtrl->rxRingDiscCnt[id] = 0;

		for (i = 0; i < size; i++) {
			cb->skb = NULL;
			cb->BdAddr = (struct DmaDesc *)(*buf + i*buf_len);
			cb++;
		}

		dma_enable = pDevCtrl->rxDma->rdma_ctrl & DMA_EN;
		pDevCtrl->rxDma->rdma_ctrl &= ~DMA_EN;
		rDmaRing->rdma_producer_index = 0;
		rDmaRing->rdma_consumer_index = 0;
		rDmaRing->rdma_ring_buf_size = ((size << DMA_RING_SIZE_SHIFT) |
				buf_len);
		dma_start = dma_map_single(&dev->dev, *buf, buf_len * size,
				DMA_BIDIRECTIONAL);
		rDmaRing->rdma_start_addr = dma_start;
		rDmaRing->rdma_end_addr = dma_start + size * buf_len - 1;
		rDmaRing->rdma_xon_xoff_threshold = (DMA_FC_THRESH_LO <<
				DMA_XOFF_THRESHOLD_SHIFT) | DMA_FC_THRESH_HI;
		rDmaRing->rdma_read_pointer = dma_start;
		rDmaRing->rdma_write_pointer = dma_start;

		/*
		* Use descriptor throttle, fire interrupt only when multiple
		* packets are done!
		*/
		rDmaRing->rdma_mbuf_done_threshold = DMA_DESC_THRES;
		/*
		* Enable push timer, that is, force the IRQ_DESC_THROT to fire
		* when timeout occurs, to prevent system slow reponse when
		* handling low throughput data.
		*/
		sid = (pDevCtrl->umac->cmd >> CMD_SPEED_SHIFT) & CMD_SPEED_MASK;
		timeout = 16*2*(DMA_DESC_THRES*ENET_MAX_MTU_SIZE)/speeds[sid];
		/* set large pushtimer value to reduce interrupt rate */
		pDevCtrl->rxDma->rdma_timeout[id] = timeout & DMA_TIMEOUT_MASK;

		/* Enable interrupt for this ring */
		pDevCtrl->intrl2_1->cpu_mask_clear |= (1 << (id + 16));
		pDevCtrl->rxDma->rdma_ctrl |= (1<<(id+DMA_RING_BUF_EN_SHIFT));
		if (!(pDevCtrl->rbuf->rbuf_ctrl & RBUF_64B_EN))
			pDevCtrl->rbuf->rbuf_ctrl |= RBUF_64B_EN;
		if (dma_enable)
			pDevCtrl->rxDma->rdma_ctrl |= DMA_EN;
	} else {
		cb  = kmalloc(size*sizeof(struct Enet_CB), GFP_KERNEL);
		if (cb == NULL) {
			spin_unlock_irqrestore(&pDevCtrl->lock, flags);
			return -ENOMEM;
		}
		tDmaRing = &pDevCtrl->txDma->tDmaRings[id];
		pDevCtrl->txRingCBs[id] = cb;
		pDevCtrl->txRingSize[id] = size;
		pDevCtrl->txRingCIndex[id] = 0;

		dma_enable = pDevCtrl->txDma->tdma_ctrl & DMA_EN;
		pDevCtrl->txDma->tdma_ctrl &= ~DMA_EN;
		tDmaRing->tdma_producer_index = 0;
		tDmaRing->tdma_consumer_index = 0;
		tDmaRing->tdma_ring_buf_size = ((size << DMA_RING_SIZE_SHIFT) |
				buf_len);
		dma_start = dma_map_single(&dev->dev, *buf, buf_len * size,
				DMA_BIDIRECTIONAL);
		tDmaRing->tdma_start_addr = dma_start;
		tDmaRing->tdma_end_addr = dma_start + size * buf_len - 1;
		tDmaRing->tdma_flow_period = ENET_MAX_MTU_SIZE << 16;
		tDmaRing->tdma_read_pointer = dma_start;
		tDmaRing->tdma_write_pointer = dma_start;

		if (!(GENET_TBUF_CTRL(pDevCtrl) & RBUF_64B_EN)) {
			GENET_TBUF_CTRL(pDevCtrl) |= RBUF_64B_EN;
			if (dev->needed_headroom < 64)
				dev->needed_headroom += 64;
		}
		pDevCtrl->txDma->tdma_ctrl |= DMA_TSB_SWAP_EN;
		pDevCtrl->txDma->tdma_ctrl |= (1<<(id+DMA_RING_BUF_EN_SHIFT));
		if (dma_enable)
			pDevCtrl->txDma->tdma_ctrl |= DMA_EN;
	}

	spin_unlock_irqrestore(&pDevCtrl->lock, flags);
	return 0;
}
EXPORT_SYMBOL(bcmgenet_init_ringbuf);
/*
 * bcmgenet_uninit_ringbuf : cleanup ring buffer
 * if "free" is non-zero , it will free the buffer.
 */
int bcmgenet_uninit_ringbuf(struct net_device *dev, int direction,
		unsigned int id, int free)
{
	int dma_enable, size = 0, buflen, i;
	struct Enet_CB *cb;
	void *buf;
	unsigned long flags;
	dma_addr_t phys_addr;
	volatile struct rDmaRingRegs *rDmaRing;
	volatile struct tDmaRingRegs *tDmaRing;
	struct BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);

	if (id < 0 || id > 15 || size & (size - 1))
		return -EINVAL;

	spin_lock_irqsave(&pDevCtrl->lock, flags);
	if (direction == GENET_ALLOC_RX_RING) {
		rDmaRing = &pDevCtrl->rxDma->rDmaRings[id];
		size = (rDmaRing->rdma_ring_buf_size >> DMA_RING_SIZE_SHIFT);
		buflen = (0xFFFF & rDmaRing->rdma_ring_buf_size);
		/* Disble this ring first */
		dma_enable = pDevCtrl->rxDma->rdma_ctrl & DMA_EN;
		pDevCtrl->rxDma->rdma_ctrl &= ~DMA_EN;
		pDevCtrl->rxDma->rdma_ctrl &=
			~(1 << (id + DMA_RING_BUF_EN_SHIFT));
		phys_addr = (dma_addr_t)rDmaRing->rdma_start_addr;
		buf = (void *)phys_to_virt(phys_addr);
		dma_unmap_single(&pDevCtrl->dev->dev,
			rDmaRing->rdma_start_addr,
			size * buflen,
			DMA_FROM_DEVICE);

		/*release resources */
		cb = pDevCtrl->rxRingCbs[id];

		for (i = 0; i < size; i++) {
			if (cb->skb != NULL)
				dev_kfree_skb_any(cb->skb);
			cb++;
		}
		kfree(pDevCtrl->rxRingCbs[id]);
		if (free)
			kfree(buf);

		if (dma_enable)
			pDevCtrl->rxDma->rdma_ctrl |= DMA_EN;
	} else {
		tDmaRing = &pDevCtrl->txDma->tDmaRings[id];
		size = (tDmaRing->tdma_ring_buf_size >> DMA_RING_SIZE_SHIFT);
		buflen = (0xFFFF & tDmaRing->tdma_ring_buf_size);
		dma_enable = pDevCtrl->txDma->tdma_ctrl & DMA_EN;
		pDevCtrl->txDma->tdma_ctrl &= ~DMA_EN;
		/* Disble this ring first */
		pDevCtrl->txDma->tdma_ctrl &=
			~(1 << (id + DMA_RING_BUF_EN_SHIFT));
		dma_unmap_single(&pDevCtrl->dev->dev,
				tDmaRing->tdma_start_addr,
				size * buflen,
				DMA_TO_DEVICE);

		/*release resources */
		cb = pDevCtrl->txRingCBs[id];
		kfree(cb);
		/*
		 * if all rings are disabled and tx checksum offloading
		 * is off, disable TSB.
		 */
		if (!(pDevCtrl->txDma->tdma_ctrl & (0xFFFF << 1))
				&& !(dev->features & NETIF_F_IP_CSUM)) {
			GENET_TBUF_CTRL(pDevCtrl) &= ~RBUF_64B_EN;
			if (dev->needed_headroom > 64)
				dev->needed_headroom -= 64;
		}
		if (dma_enable)
			pDevCtrl->rxDma->rdma_ctrl |= DMA_EN;
	}

	spin_unlock_irqrestore(&pDevCtrl->lock, flags);
	return 0;
}
EXPORT_SYMBOL(bcmgenet_uninit_ringbuf);
#if defined(CONFIG_BRCM_GENET_V2) && defined(CONFIG_NET_SCH_MULTIQ)
/*
 * init multi xmit queues, only available for GENET2
 * the queue is partitioned as follows:
 *
 * queue 0 - 3 is priority based, each one has 48 descriptors,
 * with queue 0 being the highest priority queue.
 *
 * queue 16 is the default tx queue, with 64 descriptors.
 */
static void bcmgenet_init_multiq(struct net_device *dev)
{
	int i, dma_enable;
	struct BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);

	dma_enable = pDevCtrl->txDma->tdma_ctrl & DMA_EN;
	pDevCtrl->txDma->tdma_ctrl &= ~DMA_EN;
	/* Enable strict priority arbiter mode */
	pDevCtrl->txDma->tdma_arb_ctrl = 0x2;
	for (i = 0; i < GENET_MQ_CNT; i++) {
		/* first 64 txCbs are reserved for default tx queue (ring 16) */
		pDevCtrl->txRingCBs[i] = pDevCtrl->txCbs +
			GENET_DEFAULT_BD_CNT + i * GENET_MQ_BD_CNT;
		pDevCtrl->txRingSize[i] = GENET_MQ_BD_CNT;
		pDevCtrl->txRingCIndex[i] = 0;
		pDevCtrl->txRingFreeBds[i] = GENET_MQ_BD_CNT;

		pDevCtrl->txDma->tDmaRings[i].tdma_producer_index = 0;
		pDevCtrl->txDma->tDmaRings[i].tdma_consumer_index = 0;
		pDevCtrl->txDma->tDmaRings[i].tdma_ring_buf_size =
			(GENET_MQ_BD_CNT << DMA_RING_SIZE_SHIFT) |
			RX_BUF_LENGTH;
		pDevCtrl->txDma->tDmaRings[i].tdma_start_addr =
			2 * i * GENET_MQ_BD_CNT;
		pDevCtrl->txDma->tDmaRings[i].tdma_end_addr =
			2 * (i + 1)*GENET_MQ_BD_CNT - 1;
		pDevCtrl->txDma->tDmaRings[i].tdma_flow_period =
			ENET_MAX_MTU_SIZE << 16;
		pDevCtrl->txDma->tDmaRings[i].tdma_mbuf_done_threshold = 0;
		pDevCtrl->txDma->tDmaRings[i].tdma_write_pointer =
			2 * i * GENET_MQ_BD_CNT;
		pDevCtrl->txDma->tDmaRings[i].tdma_read_pointer =
			2 * i * GENET_MQ_BD_CNT;

		/* Configure ring as decriptor ring and setup priority */
		pDevCtrl->txDma->tdma_ring_cfg |= (1 << i);
		pDevCtrl->txDma->tdma_priority[0] |=
			((GENET_Q0_PRIORITY - i) << 5*i);
		pDevCtrl->txDma->tdma_ctrl |=
			(1 << (i + DMA_RING_BUF_EN_SHIFT));
	}
	/* Set ring #16 priority */
	pDevCtrl->txDma->tdma_priority[2] |=
		((GENET_Q0_PRIORITY - 4) << 20);
	if (dma_enable)
		pDevCtrl->txDma->tdma_ctrl |= DMA_EN;
}
#endif
/*
 * bcmgenet_init_dev: initialize uniMac devie
 * allocate Tx/Rx buffer descriptors pool, Tx control block pool.
 */
static int bcmgenet_init_dev(struct BcmEnet_devctrl *pDevCtrl)
{
	int i, ret;
	unsigned long base;
	void *ptxCbs, *prxCbs;
	volatile struct DmaDesc *lastBd;

	if (pDevCtrl->phyType == BRCM_PHY_TYPE_MOCA)
		pDevCtrl->clk = clk_get(&pDevCtrl->pdev->dev, "moca");
	else
		pDevCtrl->clk = clk_get(&pDevCtrl->pdev->dev, "enet");
	clk_enable(pDevCtrl->clk);

	TRACE(("%s\n", __func__));
	/* setup buffer/pointer relationships here */
	pDevCtrl->nrTxBds = pDevCtrl->nrRxBds = TOTAL_DESC;
	/* Always use 2KB buffer for 7420*/
	pDevCtrl->rxBufLen = RX_BUF_LENGTH;

	/* register block locations */
	base = pDevCtrl->dev->base_addr;
	pDevCtrl->sys = (struct SysRegs *)(base);
	pDevCtrl->grb = (struct GrBridgeRegs *)(base + GENET_GR_BRIDGE_OFF);
	pDevCtrl->ext = (struct ExtRegs *)(base + GENET_EXT_OFF);
	pDevCtrl->intrl2_0 = (struct intrl2Regs *)(base + GENET_INTRL2_0_OFF);
	pDevCtrl->intrl2_1 = (struct intrl2Regs *)(base + GENET_INTRL2_1_OFF);
	pDevCtrl->rbuf = (struct rbufRegs *)(base + GENET_RBUF_OFF);
	pDevCtrl->umac = (struct uniMacRegs *)(base + GENET_UMAC_OFF);
	pDevCtrl->hfb = (unsigned long *)(base + GENET_HFB_OFF);
	pDevCtrl->txDma = (struct tDmaRegs *)(base + GENET_TDMA_REG_OFF);
	pDevCtrl->rxDma = (struct rDmaRegs *)(base + GENET_RDMA_REG_OFF);

#ifdef CONFIG_BRCM_GENET_V2
	pDevCtrl->tbuf = (struct tbufRegs *)(base + GENET_TBUF_OFF);
	pDevCtrl->hfbReg = (struct hfbRegs *)(base + GENET_HFB_REG_OFF);
#endif

	pDevCtrl->rxBds = (struct DmaDesc *)(base + GENET_RDMA_OFF);
	pDevCtrl->txBds = (struct DmaDesc *)(base + GENET_TDMA_OFF);

	TRACE(("%s: rxbds=0x%08x txbds=0x%08x\n", __func__,
		(unsigned int)pDevCtrl->rxBds, (unsigned int)pDevCtrl->txBds));

	/* alloc space for the tx control block pool */
	ptxCbs = kmalloc(pDevCtrl->nrTxBds*sizeof(struct Enet_CB), GFP_KERNEL);
	if (!ptxCbs) {
		clk_disable(pDevCtrl->clk);
		return -ENOMEM;
	}
	memset(ptxCbs, 0, pDevCtrl->nrTxBds*sizeof(struct Enet_CB));
	pDevCtrl->txCbs = (struct Enet_CB *)ptxCbs;

	/* initialize rx ring pointer variables. */
	pDevCtrl->rxBdAssignPtr = pDevCtrl->rxBds;
	prxCbs = kmalloc(pDevCtrl->nrRxBds*sizeof(struct Enet_CB), GFP_KERNEL);
	if (!prxCbs) {
		ret = -ENOMEM;
		goto error2;
	}
	memset(prxCbs, 0, pDevCtrl->nrRxBds*sizeof(struct Enet_CB));
	pDevCtrl->rxCbs = (struct Enet_CB *)prxCbs;

	/* init the receive buffer descriptor ring */
	for (i = 0; i < pDevCtrl->nrRxBds; i++) {
		(pDevCtrl->rxBds + i)->length_status = (pDevCtrl->rxBufLen<<16);
		(pDevCtrl->rxBds + i)->address = 0;
	}
	lastBd = pDevCtrl->rxBds + pDevCtrl->nrRxBds - 1;

	/* clear the transmit buffer descriptors */
	for (i = 0; i < pDevCtrl->nrTxBds; i++) {
		(pDevCtrl->txBds + i)->length_status = 0<<16;
		(pDevCtrl->txBds + i)->address = 0;
	}
	lastBd = pDevCtrl->txBds + pDevCtrl->nrTxBds - 1;
	pDevCtrl->txFreeBds = pDevCtrl->nrTxBds;

	/* fill receive buffers */
	if (assign_rx_buffers(pDevCtrl) == 0) {
		printk(KERN_ERR "Failed to assign rx buffers\n");
		ret = -ENOMEM;
		goto error1;
	}

	TRACE(("%s done!\n", __func__));
	/* init umac registers */
	if (init_umac(pDevCtrl)) {
		ret = -EFAULT;
		goto error1;
	}

	/* init dma registers */
	init_edma(pDevCtrl);

	TRACE(("%s done!\n", __func__));
	/* if we reach this point, we've init'ed successfully */
	return 0;
error1:
	kfree(prxCbs);
error2:
	kfree(ptxCbs);
	clk_disable(pDevCtrl->clk);

	TRACE(("%s Failed!\n", __func__));
	return ret;
}

/* Uninitialize tx/rx buffer descriptor pools */
static void bcmgenet_uninit_dev(struct BcmEnet_devctrl *pDevCtrl)
{
	int i;

	if (pDevCtrl) {
		/* disable DMA */
		pDevCtrl->rxDma->rdma_ctrl = 0;
		pDevCtrl->txDma->tdma_ctrl = 0;

		for (i = 0; i < pDevCtrl->nrTxBds; i++) {
			if (pDevCtrl->txCbs[i].skb != NULL) {
				dev_kfree_skb(pDevCtrl->txCbs[i].skb);
				pDevCtrl->txCbs[i].skb = NULL;
			}
		}
		for (i = 0; i < pDevCtrl->nrRxBds; i++) {
			if (pDevCtrl->rxCbs[i].skb != NULL) {
				dev_kfree_skb(pDevCtrl->rxCbs[i].skb);
				pDevCtrl->rxCbs[i].skb = NULL;
			}
		}

		/* free the transmit buffer descriptor */
		if (pDevCtrl->txBds)
			pDevCtrl->txBds = NULL;
		/* free the receive buffer descriptor */
		if (pDevCtrl->rxBds)
			pDevCtrl->rxBds = NULL;
		/* free the transmit control block pool */
		kfree(pDevCtrl->txCbs);
		/* free the transmit control block pool */
		kfree(pDevCtrl->rxCbs);

		clk_put(pDevCtrl->clk);
	}
}
/*
 * Program ACPI pattern into HFB. Return filter index if succesful.
 * if user == 1, the data will be copied from user space.
 */
int bcmgenet_update_hfb(struct net_device *dev, unsigned int *data,
		int len, int user)
{
	struct BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
	int filter, offset, count;
	unsigned int *tmp;

	TRACE(("Updating HFB len=0x%d\n", len));
	if (GENET_HFB_CTRL(pDevCtrl) & RBUF_HFB_256B) {
		if (len > 256)
			return -EINVAL;
		count = 8;
		offset = 256;
	} else {
		if (len > 128)
			return -EINVAL;
		count = 16;
		offset = 128;
	}
	/* find next unused filter */
	for (filter = 0; filter < count; filter++) {
		if (!((GENET_HFB_CTRL(pDevCtrl) >>
				(filter + RBUF_HFB_FILTER_EN_SHIFT)) & 0x01))
			break;
	}
	if (filter == count) {
		printk(KERN_ERR "no unused filter available!\n");
		return -EINVAL;	/* all filters have been enabled*/
	}

	if (user) {
		tmp = kmalloc(len*sizeof(unsigned int), GFP_KERNEL);
		if (tmp == NULL) {
			printk(KERN_ERR "%s: Malloc faild\n", __func__);
			return -EFAULT;
		}
		/* copy pattern data */
		if (copy_from_user(tmp, data, len*sizeof(unsigned int)) != 0) {
			printk(KERN_ERR "Failed to copy user data: src=%p, dst=%p\n",
				data, pDevCtrl->hfb + filter*offset);
			return -EFAULT;
		}
	} else {
		tmp = data;
	}
	/* Copy pattern data into HFB registers.*/
	for (count = 0; count < offset; count++) {
		if (count < len)
			pDevCtrl->hfb[filter * offset + count] = *(tmp + count);
		else
			pDevCtrl->hfb[filter * offset + count] = 0;
	}
	if (user)
		kfree(tmp);

	/* set the filter length*/
	GENET_HFB_FLTR_LEN(pDevCtrl, 3-(filter>>2)) |=
		(len*2 << (RBUF_FLTR_LEN_SHIFT * (filter&0x03)));

	/*enable this filter.*/
	GENET_HFB_CTRL(pDevCtrl) |= (1 << (RBUF_HFB_FILTER_EN_SHIFT + filter));

	return filter;

}
EXPORT_SYMBOL(bcmgenet_update_hfb);
/*
 * read ACPI pattern data for a particular filter.
 */
static int bcmgenet_read_hfb(struct net_device *dev, struct acpi_data * u_data)
{
	int filter, offset, count, len;
	struct BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);

	if (get_user(filter, &(u_data->fltr_index))) {
		printk(KERN_ERR "Failed to get user data\n");
		return -EFAULT;
	}

	if (GENET_HFB_CTRL(pDevCtrl) & RBUF_HFB_256B) {
		count = 8;
		offset = 256;
	} else {
		count = 16;
		offset = 128;
	}
	if (filter > count)
		return -EINVAL;

	/* see if this filter is enabled, if not, return length 0 */
	if ((GENET_HFB_CTRL(pDevCtrl) &
			(1 << (filter + RBUF_HFB_FILTER_EN_SHIFT))) == 0) {
		len = 0;
		put_user(len , &u_data->count);
		return 0;
	}
	/* check the filter length, in bytes */
	len = GENET_HFB_FLTR_LEN(pDevCtrl, filter>>2) >>
		(RBUF_FLTR_LEN_SHIFT * (filter & 0x3));
	len &= RBUF_FLTR_LEN_MASK;
	if (u_data->count < len)
		return -EINVAL;
	/* copy pattern data */
	if (copy_to_user((void *)(u_data->p_data),
			(void *)(pDevCtrl->hfb + filter*offset), len)) {
		printk(KERN_ERR "Failed to copy data to user space: src=%p, dst=%p\n",
				pDevCtrl->hfb+filter*offset, u_data->p_data);
		return -EFAULT;
	}
	return len;
}
/*
 * clear the HFB, disable filter indexed by "filter" argument.
 */
static inline void bcmgenet_clear_hfb(struct BcmEnet_devctrl *pDevCtrl,
		int filter)
{
	int offset;

	if (GENET_HFB_CTRL(pDevCtrl) & RBUF_HFB_256B)
		offset = 256;
	else
		offset = 128;

	if (filter == CLEAR_ALL_HFB) {
		GENET_HFB_CTRL(pDevCtrl) &=
			~(0xffff << (RBUF_HFB_FILTER_EN_SHIFT));
		GENET_HFB_CTRL(pDevCtrl) &= ~RBUF_HFB_EN;
	} else {
		/* disable this filter */
		GENET_HFB_CTRL(pDevCtrl) &=
			~(1 << (RBUF_HFB_FILTER_EN_SHIFT + filter));
		/* clear filter length register */
		GENET_HFB_FLTR_LEN(pDevCtrl, (3-(filter>>2))) &=
			~(0xff << (RBUF_FLTR_LEN_SHIFT * (filter & 0x03)));
	}

}
/*
 * Utility function to get interface ip address in kernel space.
 */
static inline unsigned int bcmgenet_getip(struct net_device *dev)
{
	struct net_device *pnet_device;
	unsigned int ip = 0;

	read_lock(&dev_base_lock);
	/* read all devices */
	for_each_netdev(&init_net, pnet_device)
	{
		if ((netif_running(pnet_device)) &&
				(pnet_device->ip_ptr != NULL) &&
				(!strcmp(pnet_device->name, dev->name))) {
			struct in_device *pin_dev;
			pin_dev = (struct in_device *)(pnet_device->ip_ptr);
			ip = htonl(pin_dev->ifa_list->ifa_address);
			break;
		}
	}
	read_unlock(&dev_base_lock);
	return ip;
}

/*
 * ethtool function - get WOL (Wake on LAN) settings,
 * Only Magic Packet Detection is supported through ethtool,
 * the ACPI (Pattern Matching) WOL option is supported in
 * bcmumac_do_ioctl function.
 */
static void bcmgenet_get_wol(struct net_device *dev,
		struct ethtool_wolinfo *wol)
{
	struct BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
	volatile struct uniMacRegs *umac = pDevCtrl->umac;
	wol->supported = WAKE_MAGIC | WAKE_MAGICSECURE;

	if (umac->mpd_ctrl & MPD_EN)
		wol->wolopts = WAKE_MAGIC;
	if (umac->mpd_ctrl & MPD_PW_EN) {
		unsigned short pwd_ms;
		unsigned long pwd_ls;
		wol->wolopts |= WAKE_MAGICSECURE;
		pwd_ls = umac->mpd_pw_ls;
		copy_to_user(&wol->sopass[0], &pwd_ls, 4);
		pwd_ms = umac->mpd_pw_ms & 0xFFFF;
		copy_to_user(&wol->sopass[4], &pwd_ms, 2);
	} else {
		memset(&wol->sopass[0], 0, sizeof(wol->sopass));
	}
}
/*
 * ethtool function - set WOL (Wake on LAN) settings.
 * Only for magic packet detection mode.
 */
static int bcmgenet_set_wol(struct net_device *dev,
		struct ethtool_wolinfo *wol)
{
	struct BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
	volatile struct uniMacRegs *umac = pDevCtrl->umac;
	unsigned int ip;

	if (wol->wolopts & ~(WAKE_MAGIC | WAKE_MAGICSECURE))
		return -EINVAL;

	ip = bcmgenet_getip(dev);
	if (ip  == 0) {
		printk(KERN_WARNING "IP address is not set, can't put in WoL mode\n");
		return -EINVAL;
	} else {
		hfb_arp[HFB_ARP_LEN-2] |= (ip >> 16);
		hfb_arp[HFB_ARP_LEN-1] |= (ip & 0xFFFF);
		/* Enable HFB, to response to ARP request.*/
		if (bcmgenet_update_hfb(dev, hfb_arp, HFB_ARP_LEN, 0) < 0) {
			printk(KERN_ERR "%s: Unable to update HFB\n", __func__);
			return -EFAULT;
		}
		GENET_HFB_CTRL(pDevCtrl) |= RBUF_HFB_EN;
	}
	if (wol->wolopts & WAKE_MAGICSECURE) {
		umac->mpd_pw_ls = *(unsigned long *)&wol->sopass[0];
		umac->mpd_pw_ms = *(unsigned short *)&wol->sopass[4];
		umac->mpd_ctrl |= MPD_PW_EN;
	}
	if (wol->wolopts & WAKE_MAGIC) {
		/* Power down the umac, with magic packet mode.*/
		bcmgenet_power_down(pDevCtrl, GENET_POWER_WOL_MAGIC);
	}
	return 0;
}
/*
 * ethtool function - get generic settings.
 */
static int bcmgenet_get_settings(struct net_device *dev,
		struct ethtool_cmd *cmd)
{
	struct BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
	int rc = 0;

	/* override autoneg on MoCA interface to return link up/down */
	if (pDevCtrl->phyType == BRCM_PHY_TYPE_MOCA) {
		cmd->autoneg = netif_carrier_ok(pDevCtrl->dev);
		cmd->speed = SPEED_1000;
		cmd->duplex = DUPLEX_HALF;
		cmd->port = PORT_BNC;
	} else {
		rc = mii_ethtool_gset(&pDevCtrl->mii, cmd);
	}

	return rc;
}
/*
 * ethtool function - set settings.
 */
static int bcmgenet_set_settings(struct net_device *dev,
		struct ethtool_cmd *cmd)
{
	int err = 0;
	struct BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);

	/* override autoneg on MoCA interface to set link up/down */
	if (pDevCtrl->phyType == BRCM_PHY_TYPE_MOCA) {
		if ((cmd->autoneg == 0) && (netif_carrier_ok(pDevCtrl->dev))) {
			pDevCtrl->dev->flags &= ~IFF_RUNNING;
			netif_carrier_off(pDevCtrl->dev);
			rtmsg_ifinfo(RTM_DELLINK, pDevCtrl->dev, IFF_RUNNING);
		}
		if ((cmd->autoneg != 0) && (!netif_carrier_ok(pDevCtrl->dev))) {
			pDevCtrl->dev->flags |= IFF_RUNNING;
			netif_carrier_on(pDevCtrl->dev);
			rtmsg_ifinfo(RTM_NEWLINK, pDevCtrl->dev, IFF_RUNNING);
		}
	} else {
		err = mii_ethtool_sset(&pDevCtrl->mii, cmd);
		if (err < 0)
			return err;
		mii_setup(dev);

		if (cmd->maxrxpkt != 0)
			DmaDescThres = cmd->maxrxpkt;
	}

	return err;
}
/*
 * ethtool function - get driver info.
 */
static void bcmgenet_get_drvinfo(struct net_device *dev,
		struct ethtool_drvinfo *info)
{
	strncpy(info->driver, CARDNAME, sizeof(info->driver));
	strncpy(info->version, VER_STR, sizeof(info->version));

}
static u32 bcmgenet_get_rx_csum(struct net_device *dev)
{
	struct BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
	if (pDevCtrl->rbuf->rbuf_chk_ctrl & RBUF_RXCHK_EN)
		return 1;

	return 0;
}
static int bcmgenet_set_rx_csum(struct net_device *dev, u32 val)
{
	struct BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
	spin_lock_bh(&pDevCtrl->bh_lock);
	if (val == 0) {
		/*pDevCtrl->rbuf->rbuf_endian_ctrl &= ~RBUF_ENDIAN_NOSWAP;*/
		pDevCtrl->rbuf->rbuf_ctrl &= ~RBUF_64B_EN;
		pDevCtrl->rbuf->rbuf_chk_ctrl &= ~RBUF_RXCHK_EN;
	} else {
		/*pDevCtrl->rbuf->rbuf_endian_ctrl &= ~RBUF_ENDIAN_NOSWAP;*/
		pDevCtrl->rbuf->rbuf_ctrl |= RBUF_64B_EN;
		pDevCtrl->rbuf->rbuf_chk_ctrl |= RBUF_RXCHK_EN ;
	}
	spin_unlock_bh(&pDevCtrl->bh_lock);
	return 0;
}
static u32 bcmgenet_get_tx_csum(struct net_device *dev)
{
	return dev->features & NETIF_F_IP_CSUM;
}
static int bcmgenet_set_tx_csum(struct net_device *dev, u32 val)
{
	unsigned long flags;
	struct BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
	spin_lock_irqsave(&pDevCtrl->lock, flags);
	if (val == 0) {
		dev->features &= ~(NETIF_F_IP_CSUM | NETIF_F_IPV6_CSUM);
		GENET_TBUF_CTRL(pDevCtrl) &= ~RBUF_64B_EN;
		if (dev->needed_headroom > 64)
			dev->needed_headroom -= 64;
	} else {
		dev->features |= NETIF_F_IP_CSUM | NETIF_F_IPV6_CSUM ;
		GENET_TBUF_CTRL(pDevCtrl) |= RBUF_64B_EN;
		if (dev->needed_headroom < 64)
			dev->needed_headroom += 64;
	}
	spin_unlock_irqrestore(&pDevCtrl->lock, flags);
	return 0;
}
static int bcmgenet_set_sg(struct net_device *dev, u32 val)
{
	unsigned long flags;
	struct BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
	if (val && !(dev->features & NETIF_F_IP_CSUM)) {
		printk(KERN_WARNING "Tx Checksum offloading disabled, not setting SG\n");
		return -EINVAL;
	}
	spin_lock_irqsave(&pDevCtrl->lock, flags);
	if (val)
		dev->features |= NETIF_F_SG;
	else
		dev->features &= ~NETIF_F_SG;
	/* must have 64B tx status enabled */
	spin_unlock_irqrestore(&pDevCtrl->lock, flags);
	return 0;
}
static u32 bcmgenet_get_sg(struct net_device *dev)
{
	return dev->features & NETIF_F_SG ;
}
/*
 * standard ethtool support functions.
 */
static struct ethtool_ops bcmgenet_ethtool_ops = {
	.get_settings		= bcmgenet_get_settings,
	.set_settings		= bcmgenet_set_settings,
	.get_drvinfo		= bcmgenet_get_drvinfo,
	.get_wol			= bcmgenet_get_wol,
	.set_wol			= bcmgenet_set_wol,
	.get_rx_csum		= bcmgenet_get_rx_csum,
	.set_rx_csum		= bcmgenet_set_rx_csum,
	.get_tx_csum		= bcmgenet_get_tx_csum,
	.set_tx_csum		= bcmgenet_set_tx_csum,
	.get_sg				= bcmgenet_get_sg,
	.set_sg				= bcmgenet_set_sg,
	.get_link			= ethtool_op_get_link,
};
/*
 * Power down the unimac, based on mode.
 */
static void bcmgenet_power_down(struct BcmEnet_devctrl *pDevCtrl, int mode)
{
	struct net_device *dev;
	int retries = 0;

	dev = pDevCtrl->dev;
	switch (mode) {
	case GENET_POWER_CABLE_SENSE:
#if 0
		/*
		 * EPHY bug, setting ext_pwr_down_dll and ext_pwr_down_phy cause
		 * link IRQ bouncing.
		 */
		pDevCtrl->ext->ext_pwr_mgmt |= (EXT_PWR_DOWN_PHY |
				EXT_PWR_DOWN_DLL | EXT_PWR_DOWN_BIAS);
#else
		/* Workaround for putting EPHY in iddq mode. */
		pDevCtrl->mii.mdio_write(dev, pDevCtrl->phyAddr, 0x1f, 0x008b);
		pDevCtrl->mii.mdio_write(dev, pDevCtrl->phyAddr, 0x10, 0x01c0);
		pDevCtrl->mii.mdio_write(dev, pDevCtrl->phyAddr, 0x14, 0x7000);
		pDevCtrl->mii.mdio_write(dev, pDevCtrl->phyAddr, 0x1f, 0x000f);
		pDevCtrl->mii.mdio_write(dev, pDevCtrl->phyAddr, 0x10, 0x20d0);
		pDevCtrl->mii.mdio_write(dev, pDevCtrl->phyAddr, 0x1f, 0x000b);

#endif
		break;
	case GENET_POWER_WOL_MAGIC:
		/* ENable CRC forward */
		pDevCtrl->umac->cmd |= CMD_CRC_FWD;
		pDevCtrl->umac->mpd_ctrl |= MPD_EN;
		while (!(pDevCtrl->rbuf->rbuf_status & RBUF_STATUS_WOL)) {
			retries++;
			if (retries > 5) {
				printk(KERN_CRIT "bcmumac_power_down polling wol mode timeout\n");
				pDevCtrl->umac->mpd_ctrl &= ~MPD_EN;
				return;
			}
			udelay(100);
		}
		/* Service Rx BD untill empty */
		pDevCtrl->intrl2_0->cpu_mask_clear |= UMAC_IRQ_MPD_R;
		pDevCtrl->intrl2_0->cpu_mask_clear |= (UMAC_IRQ_HFB_MM |
				UMAC_IRQ_HFB_SM);
		break;
	case GENET_POWER_WOL_ACPI:
		GENET_HFB_CTRL(pDevCtrl) |= RBUF_ACPI_EN;
		while (!(pDevCtrl->rbuf->rbuf_status & RBUF_STATUS_WOL)) {
			retries++;
			if (retries > 5) {
				printk(KERN_CRIT "bcmumac_power_down polling wol mode timeout\n");
				GENET_HFB_CTRL(pDevCtrl) &= ~RBUF_ACPI_EN;
				return;
			}
			udelay(100);
		}
		/* Service RX BD untill empty */
		pDevCtrl->intrl2_0->cpu_mask_clear |= (UMAC_IRQ_HFB_MM |
				UMAC_IRQ_HFB_SM);
		break;
	case GENET_POWER_PASSIVE:
		/* Power down LED */
		pDevCtrl->mii.mdio_write(pDevCtrl->dev,
				pDevCtrl->phyAddr, MII_BMCR, BMCR_RESET);
		pDevCtrl->ext->ext_pwr_mgmt |= (EXT_PWR_DOWN_PHY |
				EXT_PWR_DOWN_DLL | EXT_PWR_DOWN_BIAS);
		break;
	default:
		break;
	}

}
static void bcmgenet_power_up(struct BcmEnet_devctrl *pDevCtrl, int mode)
{
	switch (mode) {
	case GENET_POWER_CABLE_SENSE:
#if 0
		pDevCtrl->ext->ext_pwr_mgmt &= ~EXT_PWR_DOWN_DLL;
		pDevCtrl->ext->ext_pwr_mgmt &= ~EXT_PWR_DOWN_PHY;
		pDevCtrl->ext->ext_pwr_mgmt &= ~EXT_PWR_DOWN_BIAS;
#endif
		/* enable APD */
		pDevCtrl->ext->ext_pwr_mgmt |= EXT_PWR_DN_EN_LD;
		pDevCtrl->ext->ext_pwr_mgmt |= EXT_PHY_RESET;
		udelay(5);
		pDevCtrl->ext->ext_pwr_mgmt &= ~EXT_PHY_RESET;
		/* enable 64 clock MDIO */
		pDevCtrl->mii.mdio_write(pDevCtrl->dev, pDevCtrl->phyAddr, 0x1d,
				0x1000);
		pDevCtrl->mii.mdio_read(pDevCtrl->dev, pDevCtrl->phyAddr, 0x1d);
		break;
	case GENET_POWER_WOL_MAGIC:
		pDevCtrl->umac->mpd_ctrl &= ~MPD_EN;
		/*
		 * If ACPI is enabled at the same time, disable it, since
		 * we have been waken up.
		 */
		if (!(GENET_HFB_CTRL(pDevCtrl) & RBUF_ACPI_EN)) {
			GENET_HFB_CTRL(pDevCtrl) &= RBUF_ACPI_EN;
			/* Stop monitoring ACPI interrupts */
			pDevCtrl->intrl2_0->cpu_mask_set |= (UMAC_IRQ_HFB_SM |
					UMAC_IRQ_HFB_MM);
		}
		bcmgenet_clear_hfb(pDevCtrl, CLEAR_ALL_HFB);
		break;
	case GENET_POWER_WOL_ACPI:
		GENET_HFB_CTRL(pDevCtrl) &= ~RBUF_ACPI_EN;
		/*
		 * If Magic packet is enabled at the same time, disable it,
		 */
		if (!(pDevCtrl->umac->mpd_ctrl & MPD_EN)) {
			pDevCtrl->umac->mpd_ctrl &= ~MPD_EN;
			/* Stop monitoring magic packet IRQ */
			pDevCtrl->intrl2_0->cpu_mask_set |= UMAC_IRQ_MPD_R;
			/* Disable CRC Forward */
			pDevCtrl->umac->cmd &= ~CMD_CRC_FWD;
		}
		bcmgenet_clear_hfb(pDevCtrl, CLEAR_ALL_HFB);
		break;
	case GENET_POWER_PASSIVE:

		pDevCtrl->ext->ext_pwr_mgmt &= ~EXT_PWR_DOWN_DLL;
		pDevCtrl->ext->ext_pwr_mgmt &= ~EXT_PWR_DOWN_PHY;
		pDevCtrl->ext->ext_pwr_mgmt &= ~EXT_PWR_DOWN_BIAS;
		/* enable APD */
		pDevCtrl->ext->ext_pwr_mgmt |= EXT_PWR_DN_EN_LD;
		pDevCtrl->ext->ext_pwr_mgmt |= EXT_PHY_RESET;
		udelay(5);
		pDevCtrl->ext->ext_pwr_mgmt &= ~EXT_PHY_RESET;
		/* enable 64 clock MDIO */
		pDevCtrl->mii.mdio_write(pDevCtrl->dev, pDevCtrl->phyAddr, 0x1d,
				0x1000);
		pDevCtrl->mii.mdio_read(pDevCtrl->dev, pDevCtrl->phyAddr, 0x1d);
	default:
		break;
	}
}
/*
 * ioctl handle special commands that are not present in ethtool.
 */
static int bcmgenet_ioctl(struct net_device *dev, struct ifreq *rq, int cmd)
{
	struct BcmEnet_devctrl *pDevCtrl = netdev_priv(dev);
	unsigned long flags;
	struct acpi_data *u_data;
	int val = 0;

	/* we can add sub-command in ifr_data if we need to in the future */
	switch (cmd) {
	case SIOCSACPISET:
		spin_lock_irqsave(&pDevCtrl->lock, flags);
		bcmgenet_power_down(pDevCtrl, GENET_POWER_WOL_ACPI);
		spin_unlock_irqrestore(&pDevCtrl->lock, flags);
		break;
	case SIOCSACPICANCEL:
		spin_lock_irqsave(&pDevCtrl->lock, flags);
		bcmgenet_power_up(pDevCtrl, GENET_POWER_WOL_ACPI);
		spin_unlock_irqrestore(&pDevCtrl->lock, flags);
		break;
	case SIOCSPATTERN:
		u_data = (struct acpi_data *)rq->ifr_data;
		val =  bcmgenet_update_hfb(dev, (unsigned int *)u_data->p_data,
				u_data->count, 1);
		if (val >= 0)
			put_user(val, &u_data->fltr_index);
		break;
	case SIOCGPATTERN:
		u_data = (struct acpi_data *)rq->ifr_data;
		val = bcmgenet_read_hfb(dev, u_data);
		break;
	case SIOCGMIIPHY:
	case SIOCGMIIREG:
	case SIOCSMIIREG:
		val = generic_mii_ioctl(&pDevCtrl->mii, if_mii(rq), cmd, NULL);
		break;
	default:
		val = -EINVAL;
		break;
	}

	return val;
}
static const struct net_device_ops bcmgenet_netdev_ops = {
	.ndo_open = bcmgenet_open,
	.ndo_stop = bcmgenet_close,
	.ndo_start_xmit = bcmgenet_xmit,
#ifdef CONFIG_NET_SCH_MULTIQ
	.ndo_select_queue = bcmgenet_select_queue,
#endif
	.ndo_tx_timeout = bcmgenet_timeout,
	.ndo_set_multicast_list = bcmgenet_set_multicast_list,
	.ndo_set_mac_address = bcmgenet_set_mac_addr,
	.ndo_do_ioctl = bcmgenet_ioctl,
};
static int bcmgenet_drv_probe(struct platform_device *pdev)
{
	struct resource *mres, *ires;
	void __iomem *base;
	unsigned long res_size;
	int err = -EIO;
	/*
	 * bcmemac and bcmgenet use same platform data structure.
	 */
	struct bcmemac_platform_data *cfg = pdev->dev.platform_data;
	struct BcmEnet_devctrl *pDevCtrl;
	struct net_device *dev;

	mres = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	ires = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (!mres || !ires) {
		printk(KERN_ERR "%s: can't get resources\n", __func__);
		return -EIO;
	}
	res_size = mres->end - mres->start + 1;
	if (!request_mem_region(mres->start, res_size, CARDNAME)) {
		printk(KERN_ERR "%s: can't request mem region: start: 0x%x size: %lu\n",
				CARDNAME, mres->start, res_size);
		return -ENODEV;
	}
	base = ioremap(mres->start, mres->end - mres->start + 1);
	TRACE(("%s: base=0x%x\n", __func__, (unsigned int)base));

	if (!base) {
		printk(KERN_ERR "%s: can't ioremap\n", __func__);
		return -EIO;
	}

#ifdef CONFIG_NET_SCH_MULTIQ
	dev = alloc_etherdev_mq(sizeof(*(pDevCtrl)), GENET_MQ_CNT+1);
#else
	dev = alloc_etherdev(sizeof(*pDevCtrl));
#endif
	if (dev == NULL) {
		printk(KERN_ERR "bcmgenet: can't allocate net device\n");
		err = -ENOMEM;
		goto err0;
	}
	dev->base_addr = (unsigned long)base;
	pDevCtrl = (struct BcmEnet_devctrl *)netdev_priv(dev);
	SET_NETDEV_DEV(dev, &pdev->dev);
	dev_set_drvdata(&pdev->dev, pDevCtrl);
	memcpy(dev->dev_addr, cfg->macaddr, 6);
	dev->irq = pDevCtrl->irq0;
	dev->watchdog_timeo         = 2*HZ;
	SET_ETHTOOL_OPS(dev, &bcmgenet_ethtool_ops);
	dev->netdev_ops = &bcmgenet_netdev_ops;
	netif_napi_add(dev, &pDevCtrl->napi, bcmgenet_poll, 64);
	netif_napi_add(dev, &pDevCtrl->ring_napi, bcmgenet_ring_poll, 64);

	netdev_boot_setup_check(dev);

	pDevCtrl->dev = dev;
	pDevCtrl->irq0 = platform_get_irq(pdev, 0);
	pDevCtrl->irq1 = platform_get_irq(pdev, 1);
	pDevCtrl->devnum = pdev->id;
	/* NOTE: with fast-bridge , must turn this off! */
	pDevCtrl->bIPHdrOptimize = 1;

	spin_lock_init(&pDevCtrl->lock);
	spin_lock_init(&pDevCtrl->bh_lock);
	mutex_init(&pDevCtrl->mdio_mutex);
	/* Mii wait queue */
	init_waitqueue_head(&pDevCtrl->wq);

	pDevCtrl->phyType = cfg->phy_type;
	pDevCtrl->pdev = pdev;

	/* Init GENET registers, Tx/Rx buffers */
	if (bcmgenet_init_dev(pDevCtrl) < 0)
		goto err1;

	if (cfg->phy_id == BRCM_PHY_ID_AUTO) {
		if (mii_probe(dev, cfg) < 0) {
			printk(KERN_ERR "No PHY detected, not registering interface:%d\n",
					pdev->id);
			clk_disable(pDevCtrl->clk);
			goto err1;
		} else {
			printk(KERN_CRIT "Found PHY at Address %d\n",
					pDevCtrl->phyAddr);
		}

	} else {
		pDevCtrl->phyAddr = cfg->phy_id;
	}
	mii_init(dev);

	INIT_WORK(&pDevCtrl->bcmgenet_irq_work, bcmgenet_irq_task);

	if (request_irq(pDevCtrl->irq0, bcmgenet_isr0, IRQF_SHARED,
				dev->name, pDevCtrl) < 0) {
		printk(KERN_ERR "can't request IRQ %d\n", pDevCtrl->irq0);
		goto err2;
	}
	if (request_irq(pDevCtrl->irq1, bcmgenet_isr1, IRQF_SHARED,
				dev->name, pDevCtrl) < 0) {
		printk(KERN_ERR "can't request IRQ %d\n", pDevCtrl->irq1);
		free_irq(pDevCtrl->irq0, pDevCtrl);
		goto err2;
	}
	netif_carrier_off(pDevCtrl->dev);

	if (pDevCtrl->phyType == BRCM_PHY_TYPE_EXT_MII ||
		pDevCtrl->phyType == BRCM_PHY_TYPE_EXT_RGMII ||
		pDevCtrl->phyType == BRCM_PHY_TYPE_EXT_RGMII_IBS) {
		/* No Link status IRQ */
		INIT_WORK(&pDevCtrl->bcmgenet_link_work,
				bcmgenet_gphy_link_status);
		init_timer(&pDevCtrl->timer);
		pDevCtrl->timer.data = (unsigned long)pDevCtrl;
		pDevCtrl->timer.function = bcmgenet_gphy_link_timer;
	} else {
		/* check link status */
		mii_setup(dev);
	}
	dev->features |= NETIF_F_SG | NETIF_F_IP_CSUM;
	err = register_netdev(dev);
	if (err != 0)
		goto err2;
	/* Turn off these features by default */
	bcmgenet_set_tx_csum(dev, 0);
	bcmgenet_set_sg(dev, 0);

	pDevCtrl->next_dev = eth_root_dev;
	eth_root_dev = dev;
	clk_disable(pDevCtrl->clk);

	return 0;

err2:
	clk_disable(pDevCtrl->clk);
	bcmgenet_uninit_dev(pDevCtrl);
err1:
	iounmap(base);
	free_netdev(dev);
err0:
	release_mem_region(mres->start, res_size);
	return err;
}

static int bcmgenet_drv_remove(struct platform_device *pdev)
{
	struct BcmEnet_devctrl *pDevCtrl = dev_get_drvdata(&pdev->dev);

	unregister_netdev(pDevCtrl->dev);
	free_irq(pDevCtrl->irq0, pDevCtrl);
	free_irq(pDevCtrl->irq1, pDevCtrl);
	bcmgenet_uninit_dev(pDevCtrl);
	iounmap((void __iomem *)pDevCtrl->base_addr);
	free_netdev(pDevCtrl->dev);
	return 0;
}

static int bcmgenet_drv_suspend(struct platform_device *pdev,
	pm_message_t state)
{
	int val = 0;
#ifdef PM_WOL
	struct ethtool_wolinfo wolinfo;
#endif
	struct BcmEnet_devctrl *pDevCtrl = dev_get_drvdata(&pdev->dev);

#ifdef PM_WOL
	wolinfo.wolopts = WAKE_MAGIC;

	if (bcmgenet_set_wol(pDevCtrl->dev, &wolinfo) < 0)
		printk(KERN_WARN "Device %s is not entering WoL mode\n",
				pDevCtrl->dev->name);
#endif
	cancel_work_sync(&pDevCtrl->bcmgenet_irq_work);
	if (pDevCtrl->dev_opened && !pDevCtrl->dev_asleep) {
		pDevCtrl->dev_asleep = 1;
		val = bcmgenet_close(pDevCtrl->dev);
		/*
		 * After close call, dev_opened flag will be 0,
		 * we need to remember what was the state before
		 * going into suspend mode.
		 */
		pDevCtrl->dev_opened = 1;
	} else
		val = 0;

	return val;
}

static int bcmgenet_drv_resume(struct platform_device *pdev)
{
	int val = 0;
	struct BcmEnet_devctrl *pDevCtrl = dev_get_drvdata(&pdev->dev);

	if (pDevCtrl->dev_opened)
		val = bcmgenet_open(pDevCtrl->dev);
	else
		val = 0;
	if (pDevCtrl->dev_asleep)
		pDevCtrl->dev_asleep = 0;

#ifdef PM_WOL
	/* wakeup from WoL mode */
	bcmgenet_power_up(pDevCtrl, GENET_POWER_WOL_MAGIC);

#endif

	return val;
}

static struct platform_driver bcmgenet_plat_drv = {
	.probe =		bcmgenet_drv_probe,
	.remove =		bcmgenet_drv_remove,
	.suspend =		bcmgenet_drv_suspend,
	.resume =		bcmgenet_drv_resume,
	.driver = {
		.name =		"bcmgenet",
		.owner =	THIS_MODULE,
	},
};

static int bcmgenet_module_init(void)
{
	platform_driver_register(&bcmgenet_plat_drv);
	return 0;
}

static void bcmgenet_module_cleanup(void)
{
	platform_driver_unregister(&bcmgenet_plat_drv);
}

module_init(bcmgenet_module_init);
module_exit(bcmgenet_module_cleanup);
MODULE_LICENSE("GPL");
