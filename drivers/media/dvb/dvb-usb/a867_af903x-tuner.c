#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/delay.h>
#include <linux/dvb/frontend.h>

#include "dvb_frontend.h"
#include "a867_af903x.h"

#define IF2  36150       // IF2 frequency = 36.150 MHz
#define FREF 16000       // Quartz oscillator 16 MHz

static int tuner_set_params(struct dvb_frontend *fe, struct dvb_frontend_parameters *params)
{
	struct tuner_priv *priv=NULL;
	DWORD dwError = Error_NO_ERROR;
	DWORD freq = params->frequency ;// 1000; // Hz -> kHz
	
	priv->bandwidth = (fe->ops.info.type == FE_OFDM) ? params->u.ofdm.bandwidth : 0;

       deb_data("%s - freq : %d , bandwidth : %dn",__FUNCTION__, freq,priv->bandwidth);
	   
	dwError =DL_Tuner_SetFreq(freq,priv->bandwidth);
	if (dwError) deb_data("tuner_set_params Fail !\n");
	
	return 0;
}
static int tuner_get_frequency(struct dvb_frontend *fe, u32 *frequency)
{
	struct tuner_priv *priv = fe->tuner_priv;
	*frequency = priv->frequency;
	return 0;
}

static int tuner_get_bandwidth(struct dvb_frontend *fe, u32 *bandwidth)
{
	struct tuner_priv *priv = fe->tuner_priv;
	*bandwidth = priv->bandwidth;
	return 0;
}

static int tuner_init(struct dvb_frontend *fe)
{

	return 0;
}

static int tuner_sleep(struct dvb_frontend *fe)
{
	return 0;
}

static int tuner_release(struct dvb_frontend *fe)
{
	kfree(fe->tuner_priv);
	fe->tuner_priv = NULL;
	return 0;
}

static const struct dvb_tuner_ops tuner_tuner_ops = {
	.info = {
		.name           = "dvb_usb_tuner",
		.frequency_min  =  48000000,
		.frequency_max  = 860000000,
		.frequency_step =     50000,
	},

	.release       = tuner_release,

	.init          = tuner_init,
	.sleep         = tuner_sleep,

	.set_params    = tuner_set_params,
	.get_frequency = tuner_get_frequency,
	.get_bandwidth = tuner_get_bandwidth
};

/* This functions tries to identify a MT2060 tuner by reading the PART/REV register. This is hasty. */
struct dvb_frontend * tuner_attach(struct dvb_frontend *fe)
{
	struct tuner_priv *priv = NULL;

	priv = kzalloc(sizeof(struct tuner_priv), GFP_KERNEL);
	if (priv == NULL)
		return NULL;

	memcpy(&fe->ops.tuner_ops, &tuner_tuner_ops, sizeof(struct dvb_tuner_ops));

	fe->tuner_priv = priv;
	
	return fe;
}

