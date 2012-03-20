/*
 *  mxl5007t.c - driver for the MaxLinear MxL5007T silicon tuner
 *
 *  Copyright (C) 2008, 2009 Michael Krufky <mkrufky@linuxtv.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * The code is modified to accommodate the AF903x source code on 2010/6/7
 */

#include <linux/i2c.h>
#include <linux/types.h>
#include <linux/videodev2.h>
#include <linux/slab.h>
#include "a867_mxl5007t.h"

/* ------------------------------------------------------------------------- */

#define mxl_printk(kern, fmt, arg...) \
	printk(kern "%s: " fmt "\n", __func__, ##arg)

#define mxl_err(fmt, arg...) \
	mxl_printk(KERN_ERR, "%d: " fmt, __LINE__, ##arg)

#define mxl_warn(fmt, arg...) \
	mxl_printk(KERN_WARNING, fmt, ##arg)

#define mxl_info(fmt, arg...) \
	mxl_printk(KERN_INFO, fmt, ##arg)

#define mxl_debug(fmt, arg...)				\
({							\
	if (1)				\
		mxl_printk(KERN_DEBUG, fmt, ##arg);	\
})

#define mxl_fail(ret)							\
({									\
	int __ret;							\
	__ret = (ret < 0);						\
	if (__ret)							\
		mxl_printk(KERN_ERR, "error %d on line %d",		\
			   ret, __LINE__);				\
	__ret;								\
})

/* ------------------------------------------------------------------------- */

#define MHz 1000000


#include "a867_Maxlinear_MXL5007.h"


struct reg_pair_t {
	u8 reg;
	u8 val;
};

/* ------------------------------------------------------------------------- */

static struct reg_pair_t init_tab[] = {
	{ 0x02, 0x06 },
	{ 0x03, 0x48 },
	{ 0x05, 0x04 },
	{ 0x06, 0x10 },
	{ 0x2e, 0x15 }, /* OVERRIDE */
	{ 0x30, 0x10 }, /* OVERRIDE */
	{ 0x45, 0x58 }, /* OVERRIDE */
	{ 0x48, 0x19 }, /* OVERRIDE */
	{ 0x52, 0x03 }, /* OVERRIDE */
	{ 0x53, 0x44 }, /* OVERRIDE */
	{ 0x6a, 0x4b }, /* OVERRIDE */
	{ 0x76, 0x00 }, /* OVERRIDE */
	{ 0x78, 0x18 }, /* OVERRIDE */
	{ 0x7a, 0x17 }, /* OVERRIDE */
	{ 0x85, 0x06 }, /* OVERRIDE */
	{ 0x01, 0x01 }, /* TOP_MASTER_ENABLE */
	{ 0, 0 }
};

static struct reg_pair_t init_tab_cable[] = {
	{ 0x02, 0x06 },
	{ 0x03, 0x48 },
	{ 0x05, 0x04 },
	{ 0x06, 0x10 },
	{ 0x09, 0x3f },
	{ 0x0a, 0x3f },
	{ 0x0b, 0x3f },
	{ 0x2e, 0x15 }, /* OVERRIDE */
	{ 0x30, 0x10 }, /* OVERRIDE */
	{ 0x45, 0x58 }, /* OVERRIDE */
	{ 0x48, 0x19 }, /* OVERRIDE */
	{ 0x52, 0x03 }, /* OVERRIDE */
	{ 0x53, 0x44 }, /* OVERRIDE */
	{ 0x6a, 0x4b }, /* OVERRIDE */
	{ 0x76, 0x00 }, /* OVERRIDE */
	{ 0x78, 0x18 }, /* OVERRIDE */
	{ 0x7a, 0x17 }, /* OVERRIDE */
	{ 0x85, 0x06 }, /* OVERRIDE */
	{ 0x01, 0x01 }, /* TOP_MASTER_ENABLE */
	{ 0, 0 }
};

/* ------------------------------------------------------------------------- */

static struct reg_pair_t reg_pair_rftune[] = {
	{ 0x0f, 0x00 }, /* abort tune */
	{ 0x0c, 0x15 },
	{ 0x0d, 0x40 },
	{ 0x0e, 0x0e },
	{ 0x1f, 0x87 }, /* OVERRIDE */
	{ 0x20, 0x1f }, /* OVERRIDE */
	{ 0x21, 0x87 }, /* OVERRIDE */
	{ 0x22, 0x1f }, /* OVERRIDE */
	{ 0x80, 0x01 }, /* freq dependent */
	{ 0x0f, 0x01 }, /* start tune */
	{ 0, 0 }
};

/* ------------------------------------------------------------------------- */



/* ------------------------------------------------------------------------- */

/* called by _init and _rftun to manipulate the register arrays */

static void set_reg_bits(struct reg_pair_t *reg_pair, u8 reg, u8 mask, u8 val)
{
	unsigned int i = 0;

	while (reg_pair[i].reg || reg_pair[i].val) {
		if (reg_pair[i].reg == reg) {
			reg_pair[i].val &= ~mask;
			reg_pair[i].val |= val;
		}
		i++;

	}
	return;
}

static void copy_reg_bits(struct reg_pair_t *reg_pair1,
			  struct reg_pair_t *reg_pair2)
{
	unsigned int i, j;

	i = j = 0;

	while (reg_pair1[i].reg || reg_pair1[i].val) {
		while (reg_pair2[j].reg || reg_pair2[j].val) {
			if (reg_pair1[i].reg != reg_pair2[j].reg) {
				j++;
				continue;
			}
			reg_pair2[j].val = reg_pair1[i].val;
			break;
		}
		i++;
	}
	return;
}

/* ------------------------------------------------------------------------- */

static void mxl5007t_set_mode_bits(struct mxl5007t_state *state,
				   enum mxl5007t_mode mode,
				   s32 if_diff_out_level)
{
	switch (mode) {
	case MxL_MODE_ATSC:
		set_reg_bits(state->tab_init, 0x06, 0x1f, 0x12);
		break;
	case MxL_MODE_DVBT:
		set_reg_bits(state->tab_init, 0x06, 0x1f, 0x11);
		break;
	case MxL_MODE_ISDBT:
		set_reg_bits(state->tab_init, 0x06, 0x1f, 0x10);
		break;
	case MxL_MODE_CABLE:
		set_reg_bits(state->tab_init_cable, 0x09, 0xff, 0xc1);
		set_reg_bits(state->tab_init_cable, 0x0a, 0xff,
			     8 - if_diff_out_level);
		set_reg_bits(state->tab_init_cable, 0x0b, 0xff, 0x17);
		break;
	default:
		mxl_fail(-EINVAL);
	}
	return;
}

static void mxl5007t_set_if_freq_bits(struct mxl5007t_state *state,
				      enum mxl5007t_if_freq if_freq,
				      int invert_if)
{
	u8 val;

	switch (if_freq) {
	case MxL_IF_4_MHZ:
		val = 0x00;
		break;
	case MxL_IF_4_5_MHZ:
		val = 0x02;
		break;
	case MxL_IF_4_57_MHZ:
		val = 0x03;
		break;
	case MxL_IF_5_MHZ:
		val = 0x04;
		break;
	case MxL_IF_5_38_MHZ:
		val = 0x05;
		break;
	case MxL_IF_6_MHZ:
		val = 0x06;
		break;
	case MxL_IF_6_28_MHZ:
		val = 0x07;
		break;
	case MxL_IF_9_1915_MHZ:
		val = 0x08;
		break;
	case MxL_IF_35_25_MHZ:
		val = 0x09;
		break;
	case MxL_IF_36_15_MHZ:
		val = 0x0a;
		break;
	case MxL_IF_44_MHZ:
		val = 0x0b;
		break;
	default:
		mxl_fail(-EINVAL);
		return;
	}
	set_reg_bits(state->tab_init, 0x02, 0x0f, val);

	/* set inverted IF or normal IF */
	set_reg_bits(state->tab_init, 0x02, 0x10, invert_if ? 0x10 : 0x00);

	return;
}

static void mxl5007t_set_xtal_freq_bits(struct mxl5007t_state *state,
					enum mxl5007t_xtal_freq xtal_freq)
{
	switch (xtal_freq) {
	case MxL_XTAL_16_MHZ:
		/* select xtal freq & ref freq */
		set_reg_bits(state->tab_init, 0x03, 0xf0, 0x00);
		set_reg_bits(state->tab_init, 0x05, 0x0f, 0x00);
		break;
	case MxL_XTAL_20_MHZ:
		set_reg_bits(state->tab_init, 0x03, 0xf0, 0x10);
		set_reg_bits(state->tab_init, 0x05, 0x0f, 0x01);
		break;
	case MxL_XTAL_20_25_MHZ:
		set_reg_bits(state->tab_init, 0x03, 0xf0, 0x20);
		set_reg_bits(state->tab_init, 0x05, 0x0f, 0x02);
		break;
	case MxL_XTAL_20_48_MHZ:
		set_reg_bits(state->tab_init, 0x03, 0xf0, 0x30);
		set_reg_bits(state->tab_init, 0x05, 0x0f, 0x03);
		break;
	case MxL_XTAL_24_MHZ:
		set_reg_bits(state->tab_init, 0x03, 0xf0, 0x40);
		set_reg_bits(state->tab_init, 0x05, 0x0f, 0x04);
		break;
	case MxL_XTAL_25_MHZ:
		set_reg_bits(state->tab_init, 0x03, 0xf0, 0x50);
		set_reg_bits(state->tab_init, 0x05, 0x0f, 0x05);
		break;
	case MxL_XTAL_25_14_MHZ:
		set_reg_bits(state->tab_init, 0x03, 0xf0, 0x60);
		set_reg_bits(state->tab_init, 0x05, 0x0f, 0x06);
		break;
	case MxL_XTAL_27_MHZ:
		set_reg_bits(state->tab_init, 0x03, 0xf0, 0x70);
		set_reg_bits(state->tab_init, 0x05, 0x0f, 0x07);
		break;
	case MxL_XTAL_28_8_MHZ:
		set_reg_bits(state->tab_init, 0x03, 0xf0, 0x80);
		set_reg_bits(state->tab_init, 0x05, 0x0f, 0x08);
		break;
	case MxL_XTAL_32_MHZ:
		set_reg_bits(state->tab_init, 0x03, 0xf0, 0x90);
		set_reg_bits(state->tab_init, 0x05, 0x0f, 0x09);
		break;
	case MxL_XTAL_40_MHZ:
		set_reg_bits(state->tab_init, 0x03, 0xf0, 0xa0);
		set_reg_bits(state->tab_init, 0x05, 0x0f, 0x0a);
		break;
	case MxL_XTAL_44_MHZ:
		set_reg_bits(state->tab_init, 0x03, 0xf0, 0xb0);
		set_reg_bits(state->tab_init, 0x05, 0x0f, 0x0b);
		break;
	case MxL_XTAL_48_MHZ:
		set_reg_bits(state->tab_init, 0x03, 0xf0, 0xc0);
		set_reg_bits(state->tab_init, 0x05, 0x0f, 0x0c);
		break;
	case MxL_XTAL_49_3811_MHZ:
		set_reg_bits(state->tab_init, 0x03, 0xf0, 0xd0);
		set_reg_bits(state->tab_init, 0x05, 0x0f, 0x0d);
		break;
	default:
		mxl_fail(-EINVAL);
		return;
	}

	return;
}

static struct reg_pair_t *mxl5007t_calc_init_regs(struct mxl5007t_state *state,
						  enum mxl5007t_mode mode)
{
	struct mxl5007t_config *cfg = state->config;

	memcpy(state->tab_init, init_tab, sizeof(init_tab));
	memcpy(state->tab_init_cable, init_tab_cable, sizeof(init_tab_cable));

	mxl5007t_set_mode_bits(state, mode, cfg->if_diff_out_level);
	mxl5007t_set_if_freq_bits(state, cfg->if_freq_hz, cfg->invert_if);
	mxl5007t_set_xtal_freq_bits(state, cfg->xtal_freq_hz);

	set_reg_bits(state->tab_init, 0x04, 0x01, cfg->loop_thru_enable);
	set_reg_bits(state->tab_init, 0x03, 0x08, cfg->clk_out_enable << 3);
	set_reg_bits(state->tab_init, 0x03, 0x07, cfg->clk_out_amp);

	if (mode >= MxL_MODE_CABLE) {
		copy_reg_bits(state->tab_init, state->tab_init_cable);
		return state->tab_init_cable;
	} else
		return state->tab_init;
}

static void mxl5007t_set_bw_bits(struct mxl5007t_state *state,
				 enum mxl5007t_bw_mhz bw)
{
	u8 val;

	switch (bw) {
	case MxL_BW_6MHz:
		val = 0x15; /* set DIG_MODEINDEX, DIG_MODEINDEX_A,
			     * and DIG_MODEINDEX_CSF */
		break;
	case MxL_BW_7MHz:
		val = 0x2a;
		break;
	case MxL_BW_8MHz:
		val = 0x3f;
		break;
	default:
		mxl_fail(-EINVAL);
		return;
	}
	set_reg_bits(state->tab_rftune, 0x0c, 0x3f, val);

	return;
}

static struct
reg_pair_t *mxl5007t_calc_rf_tune_regs(struct mxl5007t_state *state,
				       u32 rf_freq, enum mxl5007t_bw_mhz bw)
{
	u32 dig_rf_freq = 0;
	u32 temp;
	u32 frac_divider = 1000000;
	unsigned int i;

	memcpy(state->tab_rftune, reg_pair_rftune, sizeof(reg_pair_rftune));

	mxl5007t_set_bw_bits(state, bw);

	/* Convert RF frequency into 16 bits =>
	 * 10 bit integer (MHz) + 6 bit fraction */
	dig_rf_freq = rf_freq / MHz;

	temp = rf_freq % MHz;

	for (i = 0; i < 6; i++) {
		dig_rf_freq <<= 1;
		frac_divider /= 2;
		if (temp > frac_divider) {
			temp -= frac_divider;
			dig_rf_freq++;
		}
	}

	/* add to have shift center point by 7.8124 kHz */
	if (temp > 7812)
		dig_rf_freq++;

	set_reg_bits(state->tab_rftune, 0x0d, 0xff, (u8) dig_rf_freq);
	set_reg_bits(state->tab_rftune, 0x0e, 0xff, (u8) (dig_rf_freq >> 8));

	if (rf_freq >= 333000000)
		set_reg_bits(state->tab_rftune, 0x80, 0x40, 0x40);

	return state->tab_rftune;
}

/* ------------------------------------------------------------------------- */

static int mxl5007t_write_reg(struct mxl5007t_state *state, u8 reg, u8 val)
{
	u8 buf[] = { reg, val };
	int ret = MxL_I2C_Write(state->config->I2C_Addr, buf, 2, state->config);
	if (ret) {
		mxl_err("failed!");
		return -EREMOTEIO;
	}
	
	return ret;
}

static int mxl5007t_write_regs(struct mxl5007t_state *state,
			       struct reg_pair_t *reg_pair)
{
	unsigned int i = 0;
	int ret = 0;

	while ((ret == 0) && (reg_pair[i].reg || reg_pair[i].val)) {
		ret = mxl5007t_write_reg(state,
		reg_pair[i].reg, reg_pair[i].val);
		i++;
	}

	return ret;
}

/* unused
static int mxl5007t_read_reg(struct mxl5007t_state *state, u8 reg, u8 *val)
{
	int ret = MxL_I2C_Read(state->config->I2C_Addr, reg, val, state->config);

	if (ret) {
		mxl_err("failed!");
		return -EREMOTEIO;
	}
	return ret;
}

*/

static int mxl5007t_soft_reset(struct mxl5007t_state *state)
{
	u8 d = 0xff;
	
	int ret = MxL_I2C_Write(state->config->I2C_Addr, &d, 1, state->config);
	if (ret) {
		mxl_err("failed!");
		return -EREMOTEIO;
	}
	return ret;
}

static int mxl5007t_tuner_init(struct mxl5007t_state *state,
			       enum mxl5007t_mode mode)
{
	struct reg_pair_t *init_regs;
	int ret;

	ret = mxl5007t_soft_reset(state);
	if (mxl_fail(ret))
		goto fail;

	/* calculate initialization reg array */
	init_regs = mxl5007t_calc_init_regs(state, mode);

	ret = mxl5007t_write_regs(state, init_regs);
	if (mxl_fail(ret))
		goto fail;
	mdelay(1);
fail:
	return ret;
}

static int mxl5007t_tuner_rf_tune(struct mxl5007t_state *state, u32 rf_freq_hz,
				  enum mxl5007t_bw_mhz bw)
{
	struct reg_pair_t *rf_tune_regs;
	int ret;

	/* calculate channel change reg array */
	rf_tune_regs = mxl5007t_calc_rf_tune_regs(state, rf_freq_hz, bw);

	ret = mxl5007t_write_regs(state, rf_tune_regs);
	if (mxl_fail(ret))
		goto fail;
	msleep(3);
fail:
	return ret;
}

/* ------------------------------------------------------------------------- */

/* unused
static int mxl5007t_synth_lock_status(struct mxl5007t_state *state,
				      int *rf_locked, int *ref_locked)
{
	u8 d;
	int ret;

	*rf_locked = 0;
	*ref_locked = 0;

	ret = mxl5007t_read_reg(state, 0xd8, &d);
	if (mxl_fail(ret))
		goto fail;

	if ((d & 0x0c) == 0x0c)
		*rf_locked = 1;

	if ((d & 0x03) == 0x03)
		*ref_locked = 1;
fail:
	return ret;
}
*/

/* ------------------------------------------------------------------------- */

int a867_mxl5007t_set_params(struct mxl5007t_state *state, enum mxl5007t_bw_mhz bw, u32 freq)
{
	int ret;

	mutex_lock(&state->lock);

	ret = mxl5007t_tuner_init(state, state->config->Mode);
	if (mxl_fail(ret))
		goto fail;

	ret = mxl5007t_tuner_rf_tune(state, freq, bw);
	if (mxl_fail(ret))
		goto fail;

	state->frequency = freq;	//hz
	state->bandwidth = bw;		//6, 7 ,8
fail:
	mutex_unlock(&state->lock);

	return ret;
}

/* ------------------------------------------------------------------------- */

/* unused
static int mxl5007t_init(struct mxl5007t_state *state)
{
	int ret;

	// wake from standby 
	ret = mxl5007t_write_reg(state, 0x01, 0x01);
	mxl_fail(ret);

	return ret;
}

*/

/* unused
static int mxl5007t_sleep(struct mxl5007t_state *state)
{
	int ret;

	// enter standby mode 
	ret = mxl5007t_write_reg(state, 0x01, 0x00);
	mxl_fail(ret);
	ret = mxl5007t_write_reg(state, 0x0f, 0x00);
	mxl_fail(ret);

	return ret;
}
*/

/* ------------------------------------------------------------------------- */

/* unused
static int mxl5007t_get_frequency(struct mxl5007t_state *state, u32 *frequency)
{
	*frequency = state->frequency;
	return 0;
}

*/

/* unused
static int mxl5007t_get_bandwidth(struct mxl5007t_state *state, u32 *bandwidth)
{
	*bandwidth = state->bandwidth;
	return 0;
}
*/

void a867_mxl5007t_release(struct mxl5007t_state *state)
{
	if( state ) {
		if( state->tab_init ) kfree(state->tab_init);
		if( state->tab_init_cable ) kfree(state->tab_init_cable);
		if( state->tab_rftune ) kfree(state->tab_rftune);
		state->config->state = NULL;
		kfree(state);
	}
}

/* unused
static int mxl5007t_get_chip_id(struct mxl5007t_state *state)
{
	char *name;
	int ret;
	u8 id;

	ret = mxl5007t_read_reg(state, 0xd9, &id);
	if (mxl_fail(ret))
		goto fail;

	switch (id) {
	case MxL_5007_V1_F1:
		name = "MxL5007.v1.f1";
		break;
	case MxL_5007_V1_F2:
		name = "MxL5007.v1.f2";
		break;
	case MxL_5007_V2_100_F1:
		name = "MxL5007.v2.100.f1";
		break;
	case MxL_5007_V2_100_F2:
		name = "MxL5007.v2.100.f2";
		break;
	case MxL_5007_V2_200_F1:
		name = "MxL5007.v2.200.f1";
		break;
	case MxL_5007_V2_200_F2:
		name = "MxL5007.v2.200.f2";
		break;
	case MxL_5007_V4:
		name = "MxL5007T.v4";
		break;
	default:
		name = "MxL5007T";
		printk(KERN_WARNING "%s: unknown rev (%02x)\n", __func__, id);
		id = MxL_UNKNOWN_ID;
	}
	state->chip_id = id;
	return 0;

fail:
	state->chip_id = MxL_UNKNOWN_ID;
	return ret;
}
*/

void a867_mxl5007t_attach(struct mxl5007t_config *cfg)
{
	struct mxl5007t_state *state;

	state = kzalloc(sizeof(struct mxl5007t_state), GFP_KERNEL);
	if( state == NULL ) return;

	mutex_init(&state->lock);

	state->config = cfg;
	cfg->state = state;
	state->tab_init = kzalloc(sizeof(init_tab), GFP_KERNEL);
	if( !state->tab_init ) goto error;

	state->tab_init_cable = kzalloc(sizeof(init_tab_cable), GFP_KERNEL);
	if( !state->tab_init_cable ) goto error;

	state->tab_rftune = kzalloc(sizeof(reg_pair_rftune), GFP_KERNEL);
	if( !state->tab_rftune ) goto error;
	return;
error:
	printk("fail to allocate memory\n");
	return;
}


/*
 * Overrides for Emacs so that we follow Linus's tabbing style.
 * ---------------------------------------------------------------------------
 * Local variables:
 * c-basic-offset: 8
 * End:
 */
