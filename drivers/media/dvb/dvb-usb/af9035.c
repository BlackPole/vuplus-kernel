/*
 * Afatech AF9035 DVB USB driver
 *
 * Copyright (C) 2008 Afatech
 * Copyright (C) 2009 Antti Palosaari <crope@iki.fi>
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 * Thanks to TerraTec for a support received.
 */

#include "af9035.h"
#include "af9033.h"
#include "tua9001.h"
#include "mxl5007t.h"
#include "tda18218.h"

static int dvb_usb_af9035_debug;
module_param_named(debug, dvb_usb_af9035_debug, int, 0644);
MODULE_PARM_DESC(debug, "set debugging level" DVB_USB_DEBUG_STATUS);
static int dvb_usb_af9035_remote;
module_param_named(remote, dvb_usb_af9035_remote, int, 0644);
MODULE_PARM_DESC(remote, "select remote");
DVB_DEFINE_MOD_OPT_ADAPTER_NR(adapter_nr);

static DEFINE_MUTEX(af9035_usb_mutex);

static struct af9035_config af9035_config;
static struct dvb_usb_device_properties af9035_properties[1];
static int af9035_properties_count = ARRAY_SIZE(af9035_properties);

static struct af9033_config af9035_af9033_config[] = {
	{
		.demod_address = 0,
		.tuner_address = 0,
		.output_mode = AF9033_TS_MODE_USB,
	}, {
		.demod_address = 0,
		.tuner_address = 0,
		.output_mode = AF9033_TS_MODE_SERIAL,
	}
};

static u8 regmask[8] = {0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff};

static int af9035_rw_udev(struct usb_device *udev, struct af9035_req *req)
{
#define BUF_SIZE 63
	int act_len, ret;
	u8 buf[BUF_SIZE];
	u32 msg_len;
	static u8 seq; /* packet sequence number */
	u16 checksum = 0;
	u8 i;
	u8 rxbuf[255];

	/* buffer overflow check */
	if (req->wlen > (BUF_SIZE - 6) || req->rlen > (BUF_SIZE - 5)) {
		err("too much data wlen:%d rlen:%d", req->wlen, req->rlen);
		return -EINVAL;
	}

	if (mutex_lock_interruptible(&af9035_usb_mutex) < 0)
		return -EAGAIN;

	buf[0] = req->wlen + 3 + 2; /* 3 header + 2 checksum */
	buf[1] = req->mbox;
	buf[2] = req->cmd;
	buf[3] = seq++;
	if (req->wlen)
		memcpy(&buf[4], req->wbuf, req->wlen);

	/* calc and add checksum */
	for (i = 1; i < buf[0]-1; i++) {
		if (i % 2)
			checksum += buf[i] << 8;
		else
			checksum += buf[i];
	}
	checksum = ~checksum;

	buf[buf[0]-1] = (checksum >> 8);
	buf[buf[0]-0] = (checksum & 0xff);

	msg_len = buf[0]+1;

	deb_xfer(">>> ");
	debug_dump(buf, msg_len, deb_xfer);

	/* send req */
	ret = usb_bulk_msg(udev, usb_sndbulkpipe(udev, 0x02), buf, msg_len,
	&act_len, AF9035_USB_TIMEOUT);
	if (ret)
		err("bulk message failed:%d (%d/%d)", ret, msg_len, act_len);
	else
		if (act_len != msg_len)
			ret = -EIO; /* all data is not send */
	if (ret)
		goto error_unlock;

	/* no ack for those packets */
	if (req->cmd == CMD_FW_DOWNLOAD)
		goto exit_unlock;

	/* receive ack and data if read req */
	msg_len = 3 + req->rlen + 2;  /* data len + status + seq + checksum */
	ret = usb_bulk_msg(udev, usb_rcvbulkpipe(udev, 0x81), rxbuf, msg_len,
			   &act_len, AF9035_USB_TIMEOUT);
	if (ret) {
		err("recv bulk message failed:%d", ret);
		ret = -EIO;
		goto error_unlock;
	}

	deb_xfer("<<< ");
	debug_dump(rxbuf, act_len, deb_xfer);
	/* no ack for those packets */
	if (req->cmd == CMD_IR_GET || req->cmd == CMD_IR_SET ) {	
		if (req->rlen)
//asd
			memcpy(req->rbuf, &rxbuf[3], req->rlen);
		goto exit_unlock;
	}	
	/* check status */
	if (rxbuf[2]) {
		err("command:%02x failed:%d", req->cmd, rxbuf[2]);
		ret = -EIO;
		goto error_unlock;
	}

	/* read request, copy returned data to return buf */
	if (req->rlen)
		memcpy(req->rbuf, &rxbuf[3], req->rlen);
error_unlock:
exit_unlock:
	mutex_unlock(&af9035_usb_mutex);

	return ret;
}

static int af9035_write_regs_bis(struct usb_device *d, u8 mbox, u16 reg,
u8 *val, u8 len)
{
	u8 wbuf[6+len];
	struct af9035_req req = {CMD_REG_DEMOD_WRITE, mbox, sizeof(wbuf), wbuf,
		0, NULL};
	wbuf[0] = len;
	wbuf[1] = 2;
	wbuf[2] = 0;
	wbuf[3] = 0;
	wbuf[4] = reg >> 8;
	wbuf[5] = reg & 0xff;
	memcpy(&wbuf[6], val, len);
	return af9035_rw_udev(d, &req);
}

static int af9035_write_regs(struct dvb_usb_device *d, u8 mbox, u16 reg,
u8 *val, u8 len)
{
	return af9035_write_regs_bis(d->udev, mbox, reg, val, len);
}

static int af9035_read_regs_bis(struct usb_device *d, u8 mbox, u16 reg, u8 *val,
	u8 len)
{
	u8 wbuf[] = {len, 2, 0, 0, reg >> 8, reg & 0xff};
	struct af9035_req req = {CMD_REG_DEMOD_READ, mbox, sizeof(wbuf), wbuf,
		len, val};
	return af9035_rw_udev(d, &req);
}

static int af9035_read_regs(struct dvb_usb_device *d, u8 mbox, u16 reg, u8 *val,
	u8 len)
{
	return af9035_read_regs_bis(d->udev, mbox, reg, val, len);
}

static int af9035_write_reg_bis(struct usb_device *d, u8 mbox, u16 reg, u8 val)
{
	return af9035_write_regs_bis(d, mbox, reg, &val, 1);
}

static int af9035_write_reg(struct dvb_usb_device *d, u8 mbox, u16 reg, u8 val)
{
	return af9035_write_regs_bis(d->udev, mbox, reg, &val, 1);
}

static int af9035_read_reg_bis(struct usb_device *d, u8 mbox, u16 reg, u8 *val)
{
	return af9035_read_regs_bis(d, mbox, reg, val, 1);
}

static int af9035_write_reg_bits_bis(struct usb_device *d, u8 mbox, u16 reg,
	u8 pos, u8 len, u8 val)
{
	int ret;
	u8 tmp, mask;

	ret = af9035_read_reg_bis(d, mbox, reg, &tmp);
	if (ret)
		return ret;

	mask = regmask[len - 1] << pos;
	tmp = (tmp & ~mask) | ((val << pos) & mask);

	return af9035_write_reg_bis(d, mbox, reg, tmp);
}

static int af9035_write_reg_bits(struct dvb_usb_device *d, u8 mbox, u16 reg,
	u8 pos, u8 len, u8 val)
{
	return af9035_write_reg_bits_bis(d->udev, mbox, reg, pos, len, val);
}

static int af9035_read_reg_bits_bis(struct usb_device *d, u8 mbox, u16 reg,
	u8 pos, u8 len, u8 *val)
{
	int ret;
	u8 tmp;

	ret = af9035_read_reg_bis(d, mbox, reg, &tmp);
	if (ret)
		return ret;
	*val = (tmp >> pos) & regmask[len - 1];
	return 0;
}

static int af9035_i2c_xfer(struct i2c_adapter *adap, struct i2c_msg msg[],
	int num)
{
	struct dvb_usb_device *d = i2c_get_adapdata(adap);
	int ret = 0, i = 0;
	u16 reg;
	u8 mbox;

	if (mutex_lock_interruptible(&d->i2c_mutex) < 0)
		return -EAGAIN;

	while (i < num) {
		mbox = msg[i].buf[0];
		reg = msg[i].buf[1] << 8;
		reg += msg[i].buf[2];
		if (num > i + 1 && (msg[i+1].flags & I2C_M_RD)) {
			if (msg[i].addr ==
				af9035_af9033_config[0].demod_address ||
			    msg[i].addr ==
				af9035_af9033_config[1].demod_address) {
				if (af9035_af9033_config[1].demod_address && (msg[i].addr == af9035_af9033_config[1].demod_address))
					mbox += 0x10;
				ret = af9035_read_regs(d, mbox, reg,
					&msg[i+1].buf[0], msg[i+1].len);
			} else {
				/* FIXME */
				u8 wbuf[5];
				u8 rbuf[BUF_SIZE];
				struct af9035_req req = {CMD_REG_TUNER_READ,
					LINK, sizeof(wbuf), wbuf, msg[i + 1].len,
					rbuf};
				if (af9035_af9033_config[1].tuner_address &&
					(msg[i].addr == af9035_af9033_config[1].tuner_address)) {
					msg[i].addr = af9035_af9033_config[0].tuner_address;
					req.mbox += 0x10;
				}
				wbuf[0] = msg[i + 1].len; /* read len */
				wbuf[1] = msg[i].addr; /* tuner i2c addr */
				wbuf[2] = 0x01; /* reg width */
				wbuf[3] = 0x00; /* reg MSB */
				wbuf[4] = msg[i].buf[0]; /* reg LSB */
				ret = af9035_rw_udev(d->udev, &req);
				memcpy (msg[i + 1].buf, rbuf, msg[i + 1].len);
			}
			i += 2;
		} else {
			if (msg[i].addr ==
				af9035_af9033_config[0].demod_address ||
			    msg[i].addr ==
				af9035_af9033_config[1].demod_address) {
				if (af9035_af9033_config[1].demod_address && (msg[i].addr == af9035_af9033_config[1].demod_address))
					mbox += 0x10;
				ret = af9035_write_regs(d, mbox, reg,
					&msg[i].buf[3], msg[i].len-3);
			} else {
				u8 wbuf[BUF_SIZE];
				struct af9035_req req = {CMD_REG_TUNER_WRITE,
					LINK, 4 + msg[i].len, wbuf, 0, NULL};
				if (af9035_af9033_config[1].tuner_address &&
					(msg[i].addr == af9035_af9033_config[1].tuner_address)) {
					msg[i].addr = af9035_af9033_config[0].tuner_address;
					req.mbox += 0x10;
				}
				wbuf[0] = msg[i].len - 1; /* write len */
				wbuf[1] = msg[i].addr; /* tuner i2c addr */
				wbuf[2] = 0x01; /* reg width */
				wbuf[3] = 0x00; /* reg MSB */
				memcpy (&wbuf[4], msg[i].buf, msg[i].len);
				ret = af9035_rw_udev(d->udev, &req);
			}
			i += 1;
		}
		if (ret)
			goto error;

	}
	ret = i;
error:
	mutex_unlock(&d->i2c_mutex);

	return ret;
}

static u32 af9035_i2c_func(struct i2c_adapter *adapter)
{
	return I2C_FUNC_I2C;
}

static struct i2c_algorithm af9035_i2c_algo = {
	.master_xfer = af9035_i2c_xfer,
	.functionality = af9035_i2c_func,
#ifdef NEED_ALGO_CONTROL
	.algo_control = dummy_algo_control,
#endif
};

static int af9035_init_endpoint(struct dvb_usb_device *d)
{
	int ret;
	u16 frame_size;
	u8  packet_size;

	if (d->udev->speed == USB_SPEED_FULL) {
		frame_size = TS_USB11_FRAME_SIZE/4;
		packet_size = TS_USB11_MAX_PACKET_SIZE/4;
	} else {
		frame_size = TS_USB20_FRAME_SIZE/4;
		packet_size = TS_USB20_MAX_PACKET_SIZE/4;
	}

	deb_info("%s: USB speed:%d frame_size:%04x packet_size:%02x\n",
		__func__, d->udev->speed, frame_size, packet_size);

	/* enable EP4 reset */
	ret = af9035_write_reg_bits(d, OFDM, p_reg_mp2_sw_rst,
		reg_mp2_sw_rst_pos, reg_mp2_sw_rst_len, 1);
	if (ret)
		goto error;

	/* enable EP5 reset */
	ret = af9035_write_reg_bits(d, OFDM, p_reg_mp2if2_sw_rst,
		reg_mp2if2_sw_rst_pos, reg_mp2if2_sw_rst_len, 1);
	if (ret)
		goto error;

	/* disable EP4 */
	ret = af9035_write_reg_bits(d, LINK, p_reg_ep4_tx_en,
		reg_ep4_tx_en_pos, reg_ep4_tx_en_len, 0);
	if (ret)
		goto error;

	/* disable EP5 */
	ret = af9035_write_reg_bits(d, LINK, p_reg_ep5_tx_en,
		reg_ep5_tx_en_pos, reg_ep5_tx_en_len, 0);
	if (ret)
		goto error;

	/* disable EP4 NAK */
	ret = af9035_write_reg_bits(d, LINK, p_reg_ep4_tx_nak,
		reg_ep4_tx_nak_pos, reg_ep4_tx_nak_len, 0);
	if (ret)
		goto error;

	/* disable EP5 NAK */
	ret = af9035_write_reg_bits(d, LINK, p_reg_ep5_tx_nak,
		reg_ep5_tx_nak_pos, reg_ep5_tx_nak_len, 0);
	if (ret)
		goto error;

	/* enable EP4 */
	ret = af9035_write_reg_bits(d, LINK, p_reg_ep4_tx_en,
		reg_ep4_tx_en_pos, reg_ep4_tx_en_len, 1);
	if (ret)
		goto error;

	/* EP4 xfer length */
	ret = af9035_write_regs(d, LINK, p_reg_ep4_tx_len_7_0,
		(u8 *) &frame_size, sizeof(frame_size));
	if (ret)
		goto error;

	/* EP4 packet size */
	ret = af9035_write_reg(d, LINK, p_reg_ep4_max_pkt, packet_size);
	if (ret)
		goto error;

	/* configure EP5 for dual mode */
	if (af9035_config.dual_mode) {
		/* enable EP5 */
		ret = af9035_write_reg_bits(d, LINK, p_reg_ep5_tx_en,
			reg_ep5_tx_en_pos, reg_ep5_tx_en_len, 1);
		if (ret)
			goto error;

		/* EP5 xfer length */
		ret = af9035_write_regs(d, LINK, p_reg_ep5_tx_len_7_0,
			(u8 *) &frame_size, sizeof(frame_size));
		if (ret)
			goto error;

		/* EP5 packet size */
		ret = af9035_write_reg(d, LINK, p_reg_ep5_max_pkt, packet_size);
		if (ret)
			goto error;
	}

	/* enable / disable mp2if2 */
	ret = af9035_write_reg_bits(d, OFDM, p_reg_mp2if2_en,
		reg_mp2if2_en_pos, reg_mp2if2_en_len, af9035_config.dual_mode);
	if (ret)
		goto error;

	/* enable / disable tsis */
	ret = af9035_write_reg_bits(d, OFDM, p_reg_tsis_en, reg_tsis_en_pos,
		reg_tsis_en_len, af9035_config.dual_mode);
	if (ret)
		goto error;

	/* negate EP4 reset */
	ret = af9035_write_reg_bits(d, OFDM, p_reg_mp2_sw_rst,
		reg_mp2_sw_rst_pos, reg_mp2_sw_rst_len, 0);
	if (ret)
		goto error;

	/* negate EP5 reset */
	ret = af9035_write_reg_bits(d, OFDM, p_reg_mp2if2_sw_rst,
		reg_mp2if2_sw_rst_pos, reg_mp2if2_sw_rst_len, 0);
	if (ret)
		goto error;

error:
	if (ret)
		err("endpoint init failed:%d", ret);
	return ret;
}

static int af9035_init(struct dvb_usb_device *d)
{
	int ret;
	deb_info("%s:\n", __func__);

	ret = af9035_init_endpoint(d);
	if (ret)
		goto error;
error:
	return ret;
}

static int af9035_download_firmware(struct usb_device *udev,
	const struct firmware *fw)
{
	u8 *fw_data_ptr = (u8 *) fw->data;
	int i, j, len, packets, remainder, ret;
	u8 wbuf[1];
	u8 rbuf[4];
	struct af9035_firmware_header fw_hdr;
	struct af9035_req req = {0, LINK, 0, NULL, 1, rbuf};
	struct af9035_req req_fw_dl = {CMD_FW_DOWNLOAD, LINK, 0, NULL, 0, NULL};
	struct af9035_req req_rom = {CMD_SCATTER_WRITE, LINK, 0, NULL, 1, rbuf};
	struct af9035_req req_fw_ver = {CMD_QUERYINFO, LINK, 1, wbuf, 4, rbuf};

	/* read firmware segment info from beginning of the firmware file */
	fw_hdr.segment_count = *fw_data_ptr++;
	deb_info("%s: fw segment count:%d\n", __func__, fw_hdr.segment_count);
	if (fw_hdr.segment_count > SEGMENT_MAX_COUNT) {
		warn("too big firmware segmen count:%d", fw_hdr.segment_count);
		fw_hdr.segment_count = SEGMENT_MAX_COUNT;
	}
	for (i = 0; i < fw_hdr.segment_count; i++) {
		fw_hdr.segment[i].type = (*fw_data_ptr++);
		fw_hdr.segment[i].len  = (*fw_data_ptr++) << 24;
		fw_hdr.segment[i].len += (*fw_data_ptr++) << 16;
		fw_hdr.segment[i].len += (*fw_data_ptr++) <<  8;
		fw_hdr.segment[i].len += (*fw_data_ptr++) <<  0;
		deb_info("%s: fw segment type:%d len:%d\n", __func__,
			fw_hdr.segment[i].type, fw_hdr.segment[i].len);
	}

	#define FW_PACKET_MAX_DATA 57 /* 63-4-2, packet_size-header-checksum */

	/* download all segments */
	for (i = 0; i < fw_hdr.segment_count; i++) {
		deb_info("%s: segment type:%d\n", __func__,
			fw_hdr.segment[i].type);
		if (fw_hdr.segment[i].type == SEGMENT_FW_DOWNLOAD) {
			/* download begin packet */
			req.cmd = CMD_FW_DOWNLOAD_BEGIN;
			ret = af9035_rw_udev(udev, &req);
			if (ret) {
				err("firmware download failed:%d", ret);
				goto error;
			}

			packets = fw_hdr.segment[i].len / FW_PACKET_MAX_DATA;
			remainder = fw_hdr.segment[i].len % FW_PACKET_MAX_DATA;
			len = FW_PACKET_MAX_DATA;
			for (j = 0; j <= packets; j++) {
				if (j == packets)  /* size of the last packet */
					len = remainder;

				req_fw_dl.wlen = len;
				req_fw_dl.wbuf = fw_data_ptr;
				ret = af9035_rw_udev(udev, &req_fw_dl);
				if (ret) {
					err("firmware download failed at " \
						"segment:%d packet:%d err:%d", \
						i, j, ret);
					goto error;
				}
				fw_data_ptr += len;
			}
			/* download end packet */
			req.cmd = CMD_FW_DOWNLOAD_END;
			ret = af9035_rw_udev(udev, &req);
			if (ret) {
				err("firmware download failed:%d", ret);
				goto error;
			}
		} else if (fw_hdr.segment[i].type == SEGMENT_ROM_COPY){
			packets = fw_hdr.segment[i].len / FW_PACKET_MAX_DATA;
			remainder = fw_hdr.segment[i].len % FW_PACKET_MAX_DATA;
			len = FW_PACKET_MAX_DATA;
			for (j = 0; j <= packets; j++) {
				if (j == packets)  /* size of the last packet */
					len = remainder;

				req_rom.wlen = len;
				req_rom.wbuf = fw_data_ptr;
				ret = af9035_rw_udev(udev, &req_rom);
				if (ret) {
					err("firmware download failed at " \
						"segment:%d packet:%d err:%d", \
						i, j, ret);
					goto error;
				}
				fw_data_ptr += len;
			}
		} else {
			deb_info("%s: segment type:%d not implemented\n",
				__func__, fw_hdr.segment[i].type);
		}
	}

	/* firmware loaded, request boot */
	req.cmd = CMD_BOOT;
	ret = af9035_rw_udev(udev, &req);
	if (ret)
		goto error;

	/* ensure firmware starts */
	wbuf[0] = 1;
	ret = af9035_rw_udev(udev, &req_fw_ver);
	if (ret)
		goto error;

	deb_info("%s: reply:%02x %02x %02x %02x\n", __func__,
		rbuf[0], rbuf[1], rbuf[2], rbuf[3]);

	if (!(rbuf[0] || rbuf[1] || rbuf[2] || rbuf[3])) {
		err("firmware did not run");
		ret = -EIO;
	}

error:
	if (ret)
		deb_info("%s: failed:%d\n", __func__, ret);

	return ret;
}

static int af9035_read_eeprom_reg(struct usb_device *udev, u16 reg, u8 *val)
{
	u8 wbuf[] = {1, 2, 0, 0, reg >> 8, reg & 0xff};
	struct af9035_req req = {CMD_REG_DEMOD_READ, LINK, sizeof(wbuf), wbuf,
		1, val};
	return af9035_rw_udev(udev, &req);
}

struct af9035_rc_setup {
	unsigned int id;
	char *rc_codes;
};

static char *af9035_rc_setup_match(unsigned int id,
	const struct af9035_rc_setup *table)
{
	for (; table->rc_codes; table++)
		if (table->id == id)
			return table->rc_codes;
	return NULL;
}

static const struct af9035_rc_setup af9035_rc_setup_modparam[] = {
	{ AF9035_REMOTE_A_LINK_DTU_M, RC_MAP_ALINK_DTU_M },
	{ AF9035_REMOTE_MSI_DIGIVOX_MINI_II_V3, RC_MAP_MSI_DIGIVOX_II },
	{ AF9035_REMOTE_MYGICTV_U718, RC_MAP_TOTAL_MEDIA_IN_HAND },
	{ AF9035_REMOTE_DIGITTRADE_DVB_T, RC_MAP_DIGITTRADE },
	{ AF9035_REMOTE_AVERMEDIA_KS, RC_MAP_AVERMEDIA_RM_KS },
	{ }
};

static const struct af9035_rc_setup af9035_rc_setup_hashes[] = {
	{ 0xb8feb708, RC_MAP_MSI_DIGIVOX_II },
	{ 0xa3703d00, RC_MAP_ALINK_DTU_M },
	{ 0x9b7dc64e, RC_MAP_TOTAL_MEDIA_IN_HAND }, /* MYGICTV U718 */
	{ }
};

static const struct af9035_rc_setup af9035_rc_setup_usbids[] = {
	{ (USB_VID_TERRATEC << 16) + USB_PID_TERRATEC_CINERGY_T_STICK_DUAL_RC,
		RC_MAP_TERRATEC_SLIM },
	{ (USB_VID_VISIONPLUS << 16) + USB_PID_AZUREWAVE_AD_TU700,
		RC_MAP_AZUREWAVE_AD_TU700 },
	{ (USB_VID_VISIONPLUS << 16) + USB_PID_TINYTWIN,
		RC_MAP_AZUREWAVE_AD_TU700 },
	{ (USB_VID_MSI_2 << 16) + USB_PID_MSI_DIGI_VOX_MINI_III,
		RC_MAP_MSI_DIGIVOX_III },
	{ (USB_VID_LEADTEK << 16) + USB_PID_WINFAST_DTV_DONGLE_GOLD,
		RC_MAP_LEADTEK_Y04G0051 },
	{ (USB_VID_AVERMEDIA << 16) + USB_PID_AVERMEDIA_VOLAR_X,
		RC_MAP_AVERMEDIA_M135A },
	{ (USB_VID_AFATECH << 16) + USB_PID_TREKSTOR_DVBT,
		RC_MAP_TREKSTOR },
	{ (USB_VID_KWORLD_2 << 16) + USB_PID_TINYTWIN_2,
		RC_MAP_DIGITALNOW_TINYTWIN },
	{ (USB_VID_GTEK << 16) + USB_PID_TINYTWIN_3,
		RC_MAP_DIGITALNOW_TINYTWIN },
        { (USB_VID_AVERMEDIA << 16) + USB_PID_AVERMEDIA_A867_A,
               RC_MAP_RC6_MCE },
	{ }
};

static void af9035_set_remote_config(struct usb_device *udev,
		struct dvb_usb_device_properties *props)
{
	u16 vid = le16_to_cpu(udev->descriptor.idVendor);
	u16 pid = le16_to_cpu(udev->descriptor.idProduct);

	/* try to load remote based module param */
	props->rc.core.rc_codes = af9035_rc_setup_match(
		dvb_usb_af9035_remote, af9035_rc_setup_modparam);

	/* try to load remote based eeprom hash */
	if (!props->rc.core.rc_codes)
		props->rc.core.rc_codes = af9035_rc_setup_match(
			af9035_config.eeprom_sum, af9035_rc_setup_hashes);

	/* try to load remote based USB ID */
	if (!props->rc.core.rc_codes)
		props->rc.core.rc_codes = af9035_rc_setup_match(
			(vid << 16) + pid, af9035_rc_setup_usbids);

	/* try to load remote based USB iManufacturer string */
	if (!props->rc.core.rc_codes && vid == USB_VID_AFATECH) {
		/* Check USB manufacturer and product strings and try
		   to determine correct remote in case of chip vendor
		   reference IDs are used.
		   DO NOT ADD ANYTHING NEW HERE. Use hashes instead. */
		char manufacturer[10];
		memset(manufacturer, 0, sizeof(manufacturer));
		usb_string(udev, udev->descriptor.iManufacturer,
			manufacturer, sizeof(manufacturer));
		if (!strcmp("MSI", manufacturer)) {
			/* iManufacturer 1 MSI
			   iProduct      2 MSI K-VOX */
			props->rc.core.rc_codes = af9035_rc_setup_match(
				AF9035_REMOTE_MSI_DIGIVOX_MINI_II_V3,
				af9035_rc_setup_modparam);
		}
	}

	/* finally load "empty" just for leaving IR receiver enabled */
	if (!props->rc.core.rc_codes)
		props->rc.core.rc_codes = RC_MAP_EMPTY;

	return;
}

static int af9035_read_config(struct usb_device *udev)
{
	int ret;
	u8 val, i, offset = 0;

	/* IR remote controller */
	ret = af9035_read_eeprom_reg(udev, EEPROM_IR_MODE, &val);
	if (ret)
		goto error;
	deb_info("%s: IR mode:%d\n", __func__, val);
	for (i = 0; i < af9035_properties_count; i++) {
		if (val == AF9035_IR_MODE_DISABLED)
			af9035_properties[i].rc.core.rc_codes = NULL;
		else
			af9035_set_remote_config(udev, &af9035_properties[i]);
	}


	/* IR type */
	ret = af9035_read_eeprom_reg(udev, EEPROM_IR_MODE + 0x1C , &val);
	if (ret)
		goto error;
	deb_info("%s: IR type (0nec 1RC6) :%d\n", __func__, val);

	/* TS mode - one or two receivers */
	ret = af9035_read_eeprom_reg(udev, EEPROM_TS_MODE, &val);
	if (ret)
		goto error;
	af9035_config.dual_mode = val;
	deb_info("%s: TS mode:%d\n", __func__, af9035_config.dual_mode);

	/* Set adapter0 buffer size according to USB port speed, adapter1 buffer
	   size can be static because it is enabled only USB2.0 */
	for (i = 0; i < af9035_properties_count; i++) {
		/* USB1.1 set smaller buffersize and disable 2nd adapter */
		if (udev->speed == USB_SPEED_FULL) {
			af9035_properties[i].adapter[0].fe[0].stream.u.bulk.buffersize
				= TS_USB11_MAX_PACKET_SIZE;
			/* disable 2nd adapter because we don't have
			   PID-filters */
			af9035_config.dual_mode = 0;
		} else {
			af9035_properties[i].adapter[0].fe[0].stream.u.bulk.buffersize
				= TS_USB20_FRAME_SIZE;
		}
	}

	if (af9035_config.dual_mode) {
		/* read 2nd demodulator I2C address */
		ret = af9035_read_eeprom_reg(udev, EEPROM_2WIREADDR, &val);
		if (ret)
			goto error;
		deb_info("%s: 2nd demod I2C addr:%02x\n", __func__, val);
		af9035_af9033_config[1].demod_address = val;
		/* enable 2nd adapter */
		for (i = 0; i < af9035_properties_count; i++)
			af9035_properties[i].num_adapters = 2;
	} else {
		 /* disable 2nd adapter */
		for (i = 0; i < af9035_properties_count; i++)
			af9035_properties[i].num_adapters = 1;
	}

	for (i = 0; i < af9035_properties[0].num_adapters; i++) {
		if (i == 1)
			offset =  EEPROM_SHIFT;

		/* saw BW */
		ret = af9035_read_eeprom_reg(udev, EEPROM_SAW_BW1 + offset,
			&val);
		if (ret)
			goto error;
		deb_info("%s: [%d] saw BW:%d\n", __func__, i, val);

		/* xtal */
		ret = af9035_read_eeprom_reg(udev, EEPROM_XTAL1 + offset, &val);
		if (ret)
			goto error;
		deb_info("%s: [%d] xtal:%d\n", __func__, i, val);

		/* RF spectrum inversion */
		ret = af9035_read_eeprom_reg(udev, EEPROM_SPECINV1 + offset,
			&val);
		if (ret)
			goto error;
		deb_info("%s: [%d] RF spectrum inv:%d\n", __func__, i, val);

		/* IF */
		ret = af9035_read_eeprom_reg(udev, EEPROM_IFFREQH1 + offset,
			&val);
		if (ret)
			goto error;
		af9035_af9033_config[i].if_freq = val << 8;
		ret = af9035_read_eeprom_reg(udev, EEPROM_IFFREQL1 + offset,
			&val);
		if (ret)
			goto error;
		af9035_af9033_config[i].if_freq += val;
		deb_info("%s: [%d] IF:%d\n", __func__, i,
			af9035_af9033_config[0].if_freq);

		/* MT2060 IF1 */
		ret = af9035_read_eeprom_reg(udev, EEPROM_IF1H1 + offset, &val);
		if (ret)
			goto error;
		af9035_config.mt2060_if1[i] = val << 8;
		ret = af9035_read_eeprom_reg(udev, EEPROM_IF1L1 + offset, &val);
		if (ret)
			goto error;
		af9035_config.mt2060_if1[i] += val;
		deb_info("%s: [%d] MT2060 IF1:%d\n", __func__, i,
			af9035_config.mt2060_if1[i]);

		/* tuner */
		ret = af9035_read_eeprom_reg(udev, EEPROM_TUNER_ID1 + offset,
			&val);
		if (ret)
			goto error;
		switch (val) {
		case AF9033_TUNER_TUA9001:
		case AF9033_TUNER_TDA18218:
			af9035_af9033_config[i].rf_spec_inv = 1;
			break;
		case AF9033_TUNER_MXL5007t:
			af9035_af9033_config[i].rf_spec_inv = 1;
			break;
		default:
			warn("tuner ID:%d not supported, please report!", val);
			return -ENODEV;
		};

		af9035_af9033_config[i].tuner = val;
		deb_info("%s: [%d] tuner ID:%d\n", __func__, i, val);
	}

error:
	if (ret)
		err("eeprom read failed:%d", ret);

	if (le16_to_cpu(udev->descriptor.idVendor) == USB_VID_AVERMEDIA &&
	   (le16_to_cpu(udev->descriptor.idProduct) == USB_PID_AVERMEDIA_A825 ||
	    le16_to_cpu(udev->descriptor.idProduct) == USB_PID_AVERMEDIA_B835 ||
	    le16_to_cpu(udev->descriptor.idProduct) == USB_PID_AVERMEDIA_A835 ||
	    le16_to_cpu(udev->descriptor.idProduct) == USB_PID_AVERMEDIA_A333_A ||
	    le16_to_cpu(udev->descriptor.idProduct) == USB_PID_AVERMEDIA_A333_B ||
	    le16_to_cpu(udev->descriptor.idProduct) == USB_PID_AVERMEDIA_A867_A ||
	    le16_to_cpu(udev->descriptor.idProduct) == USB_PID_AVERMEDIA_A867_B ||
	    le16_to_cpu(udev->descriptor.idProduct) == USB_PID_AVERMEDIA_A867_C ||
	    le16_to_cpu(udev->descriptor.idProduct) == USB_PID_AVERMEDIA_A867_D ||
	    le16_to_cpu(udev->descriptor.idProduct) == USB_PID_AVERMEDIA_A867_E )) {
		deb_info("%s: AverMedia A825/A835/A867: overriding config\n", __func__);
		/* set correct IF */
		for (i = 0; i < af9035_properties[0].num_adapters; i++) {
			af9035_af9033_config[i].if_freq = 4570000;
		}
	}
	return ret;
}

static int af9035_rc_query(struct dvb_usb_device *d)
{

// 	{ 0x05, 0x12, KEY_POWER },       /* POWER */
	struct af9035_state *priv = d->priv;
	int ret;
	u8 buf[6];

	struct af9035_req req = {
				    CMD_IR_GET, 
				    LINK, 
				    0, 
				    0, 
				    sizeof(buf), 
				    buf
				};
	ret = af9035_rw_udev(d->udev, &req);	
	if (ret)
		goto error;
	
/*
//Eddi 
    error = Standard_sendCommand (demodulator, Command_IR_GET, 0, Processor_LINK, 0, NULL, 4, readBuffer);^M
    if (error) goto exit;^M
^M
    *code = (Dword) ((readBuffer[0] << 24) + (readBuffer[1] << 16) + (readBuffer[2] << 8) + readBuffer[3]);^M
*/

    if ( buf[02] != 0xe7 ) {
    	deb_info("%s: %02x %02x %02x %02x %02x %02x \n", __func__,
	      	buf[00], buf[01], buf[02], buf[03], buf[04], buf[05]);	
/* quanti elementi ha sto buf ?? */

// asd
    	priv->rc_keycode = buf[00] << 24 | buf[01] << 16 | buf[02] << 8 | buf[03];
    	deb_info("%s: keycode 0x%08x \n", __func__, priv->rc_keycode);
    	rc_keydown(d->rc_dev, priv->rc_keycode, 0);
    }

error:
	if (ret)
		err("%s: failed:%d", __func__, ret);

	return ret;
}


static int af9035_aux_init(struct usb_device *d)
{
	int ret;
	u8 tmp, i;

	/* get demod crystal and ADC freqs */
	ret = af9035_read_reg_bits_bis(d, LINK,
		r_io_mux_pwron_clk_strap, io_mux_pwron_clk_strap_pos,
		io_mux_pwron_clk_strap_len, &tmp);
	if (ret)
		goto error;

	for (i = 0; i < af9035_properties[0].num_adapters; i++) {
		af9035_af9033_config[i].crystal_clock =
			clock_table[tmp].crystal;
		af9035_af9033_config[i].adc_clock =
			clock_table[tmp].adc;
	}

	/* write 2nd demod I2C address to device */
	ret = af9035_write_reg_bis(d, LINK, 0x417f,
		af9035_af9033_config[1].demod_address);
	if (ret)
		goto error;

	/* enable / disable clock out for 2nd demod for power saving */
	ret = af9035_write_reg_bis(d, LINK, p_reg_top_clkoen,
		af9035_config.dual_mode);

error:
	return ret;
}

static int af9035_identify_state(struct usb_device *udev,
				 struct dvb_usb_device_properties *props,
				 struct dvb_usb_device_description **desc,
				 int *cold)
{
	int ret;
	u8 wbuf[1] = {1};
	u8 rbuf[4];
	struct af9035_req req = {CMD_QUERYINFO, 0, sizeof(wbuf), wbuf,
		sizeof(rbuf), rbuf};

	ret = af9035_rw_udev(udev, &req);
	if (ret)
		return ret;

	deb_info("%s: reply:%02x %02x %02x %02x\n", __func__,
		rbuf[0], rbuf[1], rbuf[2], rbuf[3]);
	if (rbuf[0] || rbuf[1] || rbuf[2] || rbuf[3])
		*cold = 0;
	else
		*cold = 1;

	return ret;
}

static int af9035_af9033_frontend_attach(struct dvb_usb_adapter *adap)
{
	/* attach demodulator */
	adap->fe_adap[0].fe = dvb_attach(af9033_attach, &af9035_af9033_config[adap->id],
		&adap->dev->i2c_adap);

	return adap->fe_adap[0].fe == NULL ? -ENODEV : 0;
}

static struct tua9001_config af9035_tua9001_config[] = {
	{
		.i2c_address = 0xc0,
	} , {
		.i2c_address = 0xc1,
	}
};

static struct tda18218_config af9035_tda18218_config = {
	.i2c_address = 0xc0,
	.i2c_wr_max = 17,
};

static struct mxl5007t_config af9035_mxl5007t_config[] = {
	{
		.xtal_freq_hz = MxL_XTAL_24_MHZ,
		.if_freq_hz = MxL_IF_4_57_MHZ,
		.invert_if = 0,
		.loop_thru_enable = 0,
		.clk_out_enable = 0,
		.clk_out_amp = MxL_CLKOUT_AMP_0_94V,
	} , 
	{
		.xtal_freq_hz = MxL_XTAL_24_MHZ,
		.if_freq_hz = MxL_IF_4_57_MHZ,
		.invert_if = 0,
		.loop_thru_enable = 1,
		.clk_out_enable = 1,
		.clk_out_amp = MxL_CLKOUT_AMP_0_94V,
	}
};

static int af9035_tuner_attach(struct dvb_usb_adapter *adap)
{
	int ret;
	deb_info("%s: \n", __func__);

	switch (af9035_af9033_config[adap->id].tuner) {
	case AF9033_TUNER_TUA9001:
		af9035_af9033_config[adap->id].tuner_address = af9035_tua9001_config[adap->id].i2c_address;
		af9035_af9033_config[adap->id].tuner_address += adap->id;
		if (adap->id == 0) {
			/* gpiot3 TUA9001 RESETN
			   gpiot2 TUA9001 RXEN */
			ret = af9035_write_reg_bits(adap->dev, LINK,
				p_reg_top_gpiot2_en, reg_top_gpiot2_en_pos,
				reg_top_gpiot2_en_len, 1);
			ret = af9035_write_reg_bits(adap->dev, LINK,
				p_reg_top_gpiot2_on, reg_top_gpiot2_on_pos,
				reg_top_gpiot2_on_len, 1);
			ret = af9035_write_reg_bits(adap->dev, LINK,
				p_reg_top_gpiot3_en, reg_top_gpiot3_en_pos,
				reg_top_gpiot3_en_len, 1);
			ret = af9035_write_reg_bits(adap->dev, LINK,
				p_reg_top_gpiot3_on, reg_top_gpiot3_on_pos,
				reg_top_gpiot3_on_len, 1);

			/* reset tuner */
			ret = af9035_write_reg_bits(adap->dev, LINK, p_reg_top_gpiot3_o,
				 reg_top_gpiot3_o_pos, reg_top_gpiot3_o_len, 0);
			msleep(1);
			ret = af9035_write_reg_bits(adap->dev, LINK, p_reg_top_gpiot3_o,
				 reg_top_gpiot3_o_pos, reg_top_gpiot3_o_len, 1);

			/* activate tuner - TODO: do that like I2C gate control */
			ret = af9035_write_reg_bits(adap->dev, LINK, p_reg_top_gpiot2_o,
				 reg_top_gpiot2_o_pos, reg_top_gpiot2_o_len, 1);
		}

		ret = dvb_attach(tua9001_attach, adap->fe_adap[0].fe, &adap->dev->i2c_adap,
			&af9035_tua9001_config[adap->id]) == NULL ? -ENODEV : 0;

		break;
	case AF9033_TUNER_MXL5007t:
		af9035_af9033_config[adap->id].tuner_address = 0xc0;
		af9035_af9033_config[adap->id].tuner_address += adap->id;
		if (adap->id == 0) {
			ret = af9035_write_reg(adap->dev, LINK,
				p_reg_top_gpioh12_en,
				1);

			ret = af9035_write_reg(adap->dev, LINK,
				p_reg_top_gpioh12_on,
				1);

			ret = af9035_write_reg(adap->dev, LINK,
				p_reg_top_gpioh12_o,
				0);

			msleep(30);

			ret = af9035_write_reg(adap->dev, LINK,
				p_reg_top_gpioh12_o,
				1);

			msleep(300);

			ret = af9035_write_reg(adap->dev, LINK,
				p_reg_top_gpioh4_en,
				1);

			ret = af9035_write_reg(adap->dev, LINK,
				p_reg_top_gpioh4_on,
				1);

			ret = af9035_write_reg(adap->dev, LINK,
				p_reg_top_gpioh4_o,
				0);

			ret = af9035_write_reg(adap->dev, LINK,
				p_reg_top_gpioh3_en,
				1);

			ret = af9035_write_reg(adap->dev, LINK,
				p_reg_top_gpioh3_on,
				1);

			ret = af9035_write_reg(adap->dev, LINK,
				p_reg_top_gpioh3_o,
				1);
		}

		ret = dvb_attach(mxl5007t_attach, adap->fe_adap[0].fe, &adap->dev->i2c_adap,
			af9035_af9033_config[adap->id].tuner_address,
			&af9035_mxl5007t_config[adap->id]) == NULL ? -ENODEV : 0;

		break;
	case AF9033_TUNER_TDA18218:
		af9035_af9033_config[adap->id].tuner_address = af9035_tua9001_config[adap->id].i2c_address;
		af9035_af9033_config[adap->id].tuner_address += adap->id;
		if (adap->id == 0) {
			/* gpiot3 TUA9001 RESETN
			   gpiot2 TUA9001 RXEN */
			ret = af9035_write_reg_bits(adap->dev, LINK, p_reg_top_gpiot2_en,
				 reg_top_gpiot2_en_pos, reg_top_gpiot2_en_len, 1);
			ret = af9035_write_reg_bits(adap->dev, LINK, p_reg_top_gpiot2_on,
				 reg_top_gpiot2_on_pos, reg_top_gpiot2_on_len, 1);
			ret = af9035_write_reg_bits(adap->dev, LINK,
				 p_reg_top_gpiot3_en, reg_top_gpiot3_en_pos,
				 reg_top_gpiot3_en_len, 1);
			ret = af9035_write_reg_bits(adap->dev, LINK,
				 p_reg_top_gpiot3_on, reg_top_gpiot3_on_pos,
				 reg_top_gpiot3_on_len, 1);

			/* reset tuner */
			ret = af9035_write_reg_bits(adap->dev, LINK, p_reg_top_gpiot3_o,
				 reg_top_gpiot3_o_pos, reg_top_gpiot3_o_len, 0);
			msleep(1);
			ret = af9035_write_reg_bits(adap->dev, LINK, p_reg_top_gpiot3_o,
				 reg_top_gpiot3_o_pos, reg_top_gpiot3_o_len, 1);

			/* activate tuner - TODO: do that like I2C gate control */
			ret = af9035_write_reg_bits(adap->dev, LINK, p_reg_top_gpiot2_o,
				 reg_top_gpiot2_o_pos, reg_top_gpiot2_o_len, 1);
		}

		ret = dvb_attach(tda18218_attach, adap->fe_adap[0].fe, &adap->dev->i2c_adap,
		   &af9035_tda18218_config) == NULL ? -ENODEV : 0;

		break;
	default:
		ret = -ENODEV;
		err("unknown tuner ID:%d",
			af9035_af9033_config[adap->id].tuner);
	}

	return ret;
}

static struct usb_device_id af9035_usb_table[] = {
/*  0 */{USB_DEVICE(USB_VID_AFATECH,   USB_PID_AFATECH_AF9035_1000)},
	{USB_DEVICE(USB_VID_AFATECH,   USB_PID_AFATECH_AF9035_1001)},
	{USB_DEVICE(USB_VID_AFATECH,   USB_PID_AFATECH_AF9035_1002)},
	{USB_DEVICE(USB_VID_AFATECH,   USB_PID_AFATECH_AF9035_1003)},
	{USB_DEVICE(USB_VID_AFATECH,   USB_PID_AFATECH_AF9035_9035)},
/*  5 */{USB_DEVICE(USB_VID_TERRATEC,  USB_PID_TERRATEC_CINERGY_T_STICK)},
/*  6 */{USB_DEVICE(USB_VID_AVERMEDIA, USB_PID_AVERMEDIA_A825)},
	{USB_DEVICE(USB_VID_AVERMEDIA, USB_PID_AVERMEDIA_B835)},
	{USB_DEVICE(USB_VID_AVERMEDIA, USB_PID_AVERMEDIA_A835)},
/*  9 */{USB_DEVICE(USB_VID_AVERMEDIA, USB_PID_AVERMEDIA_A333_A)},
/* 10 */{USB_DEVICE(USB_VID_AVERMEDIA, USB_PID_AVERMEDIA_A333_B)},
/* 11 */{USB_DEVICE(USB_VID_AVERMEDIA, USB_PID_AVERMEDIA_A867_A)},
/* 12 */{USB_DEVICE(USB_VID_AVERMEDIA, USB_PID_AVERMEDIA_A867_B)},
/* 13 */{USB_DEVICE(USB_VID_AVERMEDIA, USB_PID_AVERMEDIA_A867_C)},
/* 14 */{USB_DEVICE(USB_VID_AVERMEDIA, USB_PID_AVERMEDIA_A867_D)},
/* 15 */{USB_DEVICE(USB_VID_AVERMEDIA, USB_PID_AVERMEDIA_A867_E)},
	{0},
};

MODULE_DEVICE_TABLE(usb, af9035_usb_table);

#define AF9035_RC_INTERVAL 200
static struct dvb_usb_device_properties af9035_properties[] = {
	{
		.caps = DVB_USB_IS_AN_I2C_ADAPTER,

		.usb_ctrl = DEVICE_SPECIFIC,
		.download_firmware = af9035_download_firmware,
		.firmware = "dvb-usb-af9035-01.fw",
		.no_reconnect = 1,

		.size_of_priv = sizeof(struct af9035_state),

		.adapter = {
			{
                        .num_frontends = 1,                                                                                                  
                        .fe = {{
				.frontend_attach =
					af9035_af9033_frontend_attach,
				.tuner_attach = af9035_tuner_attach,
				.stream = {
					.type = USB_BULK,
					.count = 4,
					.endpoint = 0x84,
				},
			}},
			},
			{
                        .num_frontends = 1,                                                                                                  
                        .fe = {{
 				.frontend_attach =
					af9035_af9033_frontend_attach,
				.tuner_attach = af9035_tuner_attach,
				.stream = {
					.type = USB_BULK,
					.count = 4,
					.endpoint = 0x85,
					.u = {
						.bulk = {
							.buffersize =
						TS_USB20_FRAME_SIZE,
						}
					}
				},
			}},
			}
		},

		.identify_state = af9035_identify_state,

		.rc.core = {
			.protocol         = RC_TYPE_NEC,
			.module_name      = "af9035",
			.rc_query         = af9035_rc_query,
			.rc_interval      = AF9035_RC_INTERVAL,
			.allowed_protos   = RC_TYPE_NEC,
		},

		.i2c_algo = &af9035_i2c_algo,

		.num_device_descs = 6,
		.devices = {
			{
				.name = "Afatech AF9035 DVB-T USB2.0 stick",
				.cold_ids = {&af9035_usb_table[0],
					     &af9035_usb_table[1],
					     &af9035_usb_table[2],
					     &af9035_usb_table[3],
					     &af9035_usb_table[4], NULL},
				.warm_ids = {NULL},
			},
			{
				.name = "TerraTec Cinergy T Stick",
				.cold_ids = {&af9035_usb_table[5], NULL},
				.warm_ids = {NULL},
			},
			{
				.name = "Avermedia TwinStar",
				.cold_ids = {&af9035_usb_table[6], NULL},
				.warm_ids = {NULL},
			},
			{
				.name = "Avermedia AverTV Volar HD (A835)",
				.cold_ids = {&af9035_usb_table[7],
					     &af9035_usb_table[8],NULL},
				.warm_ids = {NULL},
			},
			{
				.name = "Avermedia Digital Receiver (A833)",
				.cold_ids = {&af9035_usb_table[9],
					     &af9035_usb_table[10],NULL},
				.warm_ids = {NULL},
			},
			{
				.name = "Avermedia Sky Digital Pen (A867)",
				.cold_ids = {&af9035_usb_table[11],
					     &af9035_usb_table[12],
					     &af9035_usb_table[13],
					     &af9035_usb_table[14],
					     &af9035_usb_table[15],NULL},
				.warm_ids = {NULL},
			},
		}
	},
};

static int af9035_usb_probe(struct usb_interface *intf,
			    const struct usb_device_id *id)
{
	int ret = 0;
	struct dvb_usb_device *d = NULL;
	struct usb_device *udev = interface_to_usbdev(intf);
	u8 i;

	deb_info("%s: interface:%d\n", __func__,
		intf->cur_altsetting->desc.bInterfaceNumber);

	/* interface 0 is used by DVB-T receiver and
	   interface 1 is for remote controller (HID) */
	if (intf->cur_altsetting->desc.bInterfaceNumber == 0) {
		ret = af9035_read_config(udev);
		if (ret)
			return ret;

		ret = af9035_aux_init(udev);
		if (ret)
			return ret;

		for (i = 0; i < af9035_properties_count; i++) {
			ret = dvb_usb_device_init(intf, &af9035_properties[i],
				THIS_MODULE, &d, adapter_nr);
			if (!ret)
				break;
			if (ret != -ENODEV)
				return ret;
		}
		if (ret)
			return ret;

		if (d)
			ret = af9035_init(d);
	}

	return ret;
}

/* usb specific object needed to register this driver with the usb subsystem */
static struct usb_driver af9035_usb_driver = {
	.name = "dvb_usb_af9035",
	.probe = af9035_usb_probe,
	.disconnect = dvb_usb_device_exit,
	.id_table = af9035_usb_table,
};

/* module stuff */
static int __init af9035_usb_module_init(void)
{
	int ret;
	ret = usb_register(&af9035_usb_driver);
	if (ret)
		err("module init failed:%d", ret);

	return ret;
}

static void __exit af9035_usb_module_exit(void)
{
	/* deregister this driver from the USB subsystem */
	usb_deregister(&af9035_usb_driver);
}

module_init(af9035_usb_module_init);
module_exit(af9035_usb_module_exit);

MODULE_AUTHOR("Antti Palosaari <crope@iki.fi>");
MODULE_DESCRIPTION("Afatech AF9035 driver");
MODULE_LICENSE("GPL");
