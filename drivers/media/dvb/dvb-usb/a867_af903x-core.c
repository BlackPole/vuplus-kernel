#include "a867_af903x.h"
#include "a867_aver_version.h"

DEVICE_CONTEXT DC;

#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,25)
DVB_DEFINE_MOD_OPT_ADAPTER_NR(adapter_nr);
#endif

static int map_gpio_by_id(const struct usb_device_id *id, PDEVICE_CONTEXT pdc)
{
	if( id->idProduct == 0xa333 ) {
		pdc->Map.I2C_SLAVE_ADDR = 0x3A;
		pdc->Map.RF_SW_HOST = 0; //chip0
		pdc->Map.GPIO_UHF_en = p_reg_top_gpioh2_en;
		pdc->Map.GPIO_UHF_on = p_reg_top_gpioh2_on;
		pdc->Map.GPIO_UHF_o  = p_reg_top_gpioh2_o;
		pdc->Map.GPIO_VHF_en = p_reg_top_gpioh3_en;
		pdc->Map.GPIO_VHF_on = p_reg_top_gpioh3_on;
		pdc->Map.GPIO_VHF_o  = p_reg_top_gpioh3_o;
		pdc->Map.GPIO_WP_en	= p_reg_top_gpioh7_en;
		pdc->Map.GPIO_WP_on	= p_reg_top_gpioh7_on;
		pdc->Map.GPIO_WP_o   = p_reg_top_gpioh7_o;
		pdc->Map.GPIO_OSC_en = p_reg_top_gpioh6_en;
		pdc->Map.GPIO_OSC_on = p_reg_top_gpioh6_on;
		pdc->Map.GPIO_OSC_o  = p_reg_top_gpioh6_o;
		pdc->Map.GPIO_TUR1_en = p_reg_top_gpioh11_en;
		pdc->Map.GPIO_TUR1_on = p_reg_top_gpioh11_on;
		pdc->Map.GPIO_TUR1_o  = p_reg_top_gpioh11_o;
		pdc->Map.GPIO_TUR2_en = p_reg_top_gpioh12_en;
		pdc->Map.GPIO_TUR2_on = p_reg_top_gpioh12_on;
		pdc->Map.GPIO_TUR2_o  = p_reg_top_gpioh12_o;
		pdc->Map.GPIO_DPWR_en = 0xF000;
		pdc->Map.GPIO_DPWR_on = 0xF000;
		pdc->Map.GPIO_DPWR_o	 = 0xF000;
		pdc->Map.GPIO_DRST_en = 0xF000;
		pdc->Map.GPIO_DRST_on = 0xF000;
		pdc->Map.GPIO_DRST_o  = 0xF000;
		pdc->Map.GPIO_STR_en  = p_reg_top_gpioh8_en;
		pdc->Map.GPIO_STR_on  = p_reg_top_gpioh8_on;
		pdc->Map.GPIO_STR_o   = p_reg_top_gpioh8_o;
		pdc->Map.GPIO_STR_i   = r_reg_top_gpioh8_i;
	}
	else if( id->idProduct == 0xa825 ) {
		pdc->Map.I2C_SLAVE_ADDR = 0x3E;
		//pdc->Map.RF_SW_HOST = 1; //chip1
		pdc->Map.RF_SW_HOST = 0; //chip0
		pdc->Map.GPIO_UHF_en = p_reg_top_gpioh3_en;
		pdc->Map.GPIO_UHF_on = p_reg_top_gpioh3_on;
		pdc->Map.GPIO_UHF_o  = p_reg_top_gpioh3_o;
		pdc->Map.GPIO_VHF_en = p_reg_top_gpioh2_en;	//When UHF, this pin low
		pdc->Map.GPIO_VHF_on = p_reg_top_gpioh2_on;
		pdc->Map.GPIO_VHF_o  = p_reg_top_gpioh2_o;
		pdc->Map.GPIO_WP_en	= p_reg_top_gpioh7_en;
		pdc->Map.GPIO_WP_on	= p_reg_top_gpioh7_on;
		pdc->Map.GPIO_WP_o   = p_reg_top_gpioh7_o;
		pdc->Map.GPIO_OSC_en = p_reg_top_gpioh6_en;	//Use GPIO6 for HW test
		pdc->Map.GPIO_OSC_on = p_reg_top_gpioh6_on;	//GPIO8 can't use.
		pdc->Map.GPIO_OSC_o  = p_reg_top_gpioh6_o;
		pdc->Map.GPIO_TUR1_en = p_reg_top_gpioh11_en;	//Use GPIO11,12 will cause hostB fail.
		pdc->Map.GPIO_TUR1_on = p_reg_top_gpioh11_on;
		pdc->Map.GPIO_TUR1_o  = p_reg_top_gpioh11_o;
		pdc->Map.GPIO_TUR2_en = p_reg_top_gpioh12_en;
		pdc->Map.GPIO_TUR2_on = p_reg_top_gpioh12_on;
		pdc->Map.GPIO_TUR2_o  = p_reg_top_gpioh12_o;
		pdc->Map.GPIO_DPWR_en = p_reg_top_gpioh2_en;
		pdc->Map.GPIO_DPWR_on = p_reg_top_gpioh2_on;
		pdc->Map.GPIO_DPWR_o	 = p_reg_top_gpioh2_o;
		pdc->Map.GPIO_DRST_en = p_reg_top_gpioh5_en;
		pdc->Map.GPIO_DRST_on = p_reg_top_gpioh5_on;
		pdc->Map.GPIO_DRST_o  = p_reg_top_gpioh5_o;
		pdc->Map.GPIO_STR_en  = 0xF000;
		pdc->Map.GPIO_STR_on  = 0xF000;
		pdc->Map.GPIO_STR_o   = 0xF000;
		pdc->Map.GPIO_STR_i   = 0xF000;
	}
	else if(id->idProduct == 0xa337 
		|| id->idProduct == 0xF337
		|| id->idProduct == 0x0337 
		|| id->idProduct == 0xa867 
		|| id->idProduct == 0x0867 
		|| id->idProduct == 0x1867) {
		deb_data("GPIO Mapping : A867\n");
		pdc->Map.I2C_SLAVE_ADDR = 0x3A;
		pdc->Map.RF_SW_HOST = 0; //chip1
		pdc->Map.GPIO_UHF_en = 0xF000;
		pdc->Map.GPIO_UHF_on = 0xF000;
		pdc->Map.GPIO_UHF_o  = 0xF000;
		pdc->Map.GPIO_VHF_en = 0xF000;
		pdc->Map.GPIO_VHF_on = 0xF000;
		pdc->Map.GPIO_VHF_o  = 0xF000;
		pdc->Map.GPIO_WP_en	= p_reg_top_gpioh7_en;
		pdc->Map.GPIO_WP_on	= p_reg_top_gpioh7_on;
		pdc->Map.GPIO_WP_o   = p_reg_top_gpioh7_o;
		pdc->Map.GPIO_OSC_en = 0xF000;
		pdc->Map.GPIO_OSC_on = 0xF000;
		pdc->Map.GPIO_OSC_o  = 0xF000;
		pdc->Map.GPIO_TUR1_en = p_reg_top_gpioh6_en;	//Use GPIO11,12 will cause hostB fail.
		pdc->Map.GPIO_TUR1_on = p_reg_top_gpioh6_on;
		pdc->Map.GPIO_TUR1_o  = p_reg_top_gpioh6_o;
		pdc->Map.GPIO_TUR2_en = p_reg_top_gpioh11_en;
		pdc->Map.GPIO_TUR2_on = p_reg_top_gpioh11_on;
		pdc->Map.GPIO_TUR2_o  = p_reg_top_gpioh11_o;
		pdc->Map.GPIO_DPWR_en = 0xF000;
		pdc->Map.GPIO_DPWR_on = 0xF000;
		pdc->Map.GPIO_DPWR_o	= 0xF000;
		pdc->Map.GPIO_DRST_en = 0xF000;
		pdc->Map.GPIO_DRST_on = 0xF000;
		pdc->Map.GPIO_DRST_o  = 0xF000;
		pdc->Map.GPIO_STR_en  = 0xF000;
		pdc->Map.GPIO_STR_on  = 0xF000;
		pdc->Map.GPIO_STR_o   = 0xF000;
		pdc->Map.GPIO_STR_i   = 0xF000;
		pdc->Map.GPIO_LED_en = p_reg_top_gpioh3_en;	
		pdc->Map.GPIO_LED_on = p_reg_top_gpioh3_on;
		pdc->Map.GPIO_LED_o  = p_reg_top_gpioh3_o;
	}
	else {
		pdc->Map.I2C_SLAVE_ADDR = 0x3E;
		pdc->Map.RF_SW_HOST = 1; //chip1
		pdc->Map.GPIO_UHF_en = p_reg_top_gpioh3_en;
		pdc->Map.GPIO_UHF_on = p_reg_top_gpioh3_on;
		pdc->Map.GPIO_UHF_o  = p_reg_top_gpioh3_o;
		pdc->Map.GPIO_VHF_en = p_reg_top_gpioh2_en;	//When UHF, this pin low
		pdc->Map.GPIO_VHF_on = p_reg_top_gpioh2_on;
		pdc->Map.GPIO_VHF_o  = p_reg_top_gpioh2_o;
		pdc->Map.GPIO_WP_en	= p_reg_top_gpioh7_en;
		pdc->Map.GPIO_WP_on	= p_reg_top_gpioh7_on;
		pdc->Map.GPIO_WP_o   = p_reg_top_gpioh7_o;
		pdc->Map.GPIO_OSC_en = p_reg_top_gpioh6_en;	//Use GPIO6 for HW test
		pdc->Map.GPIO_OSC_on = p_reg_top_gpioh6_on;	//GPIO8 can't use.
		pdc->Map.GPIO_OSC_o  = p_reg_top_gpioh6_o;
		pdc->Map.GPIO_TUR1_en = p_reg_top_gpioh11_en;	//Use GPIO11,12 will cause hostB fail.
		pdc->Map.GPIO_TUR1_on = p_reg_top_gpioh11_on;
		pdc->Map.GPIO_TUR1_o  = p_reg_top_gpioh11_o;
		pdc->Map.GPIO_TUR2_en = p_reg_top_gpioh12_en;
		pdc->Map.GPIO_TUR2_on = p_reg_top_gpioh12_on;
		pdc->Map.GPIO_TUR2_o  = p_reg_top_gpioh12_o;
		pdc->Map.GPIO_DPWR_en = p_reg_top_gpioh2_en;
		pdc->Map.GPIO_DPWR_on = p_reg_top_gpioh2_on;
		pdc->Map.GPIO_DPWR_o	 = p_reg_top_gpioh2_o;
		pdc->Map.GPIO_DRST_en = p_reg_top_gpioh5_en;
		pdc->Map.GPIO_DRST_on = p_reg_top_gpioh5_on;
		pdc->Map.GPIO_DRST_o  = p_reg_top_gpioh5_o;
		pdc->Map.GPIO_STR_en  = 0xF000;
		pdc->Map.GPIO_STR_on  = 0xF000;
		pdc->Map.GPIO_STR_o   = 0xF000;
		pdc->Map.GPIO_STR_i   = 0xF000;
	}
	return 0;
}

static int af903x_probe(struct usb_interface *intf,
		const struct usb_device_id *id)
{
	int retval = -ENOMEM;
	int i;

	// init GPIO mappings based on device
	map_gpio_by_id(id, &DC);

	deb_data("===af903x usb device pluged in!! ===\n");
	retval = Device_init(interface_to_usbdev(intf),intf,&DC, true);
	if (retval){
                if(retval) deb_data("Device_init Fail: 0x%08x\n", retval);
        }
	
	for (i = 0; i < af903x_device_count; i++) {
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,25)
		if (dvb_usb_device_init(intf, &af903x_properties[i], THIS_MODULE, NULL, adapter_nr) == 0)
#else
		if (dvb_usb_device_init(intf, &af903x_properties[i], THIS_MODULE, NULL) == 0)
#endif
			{deb_data("dvb_usb_device_init success!!\n");return 0;}
	}

	return -ENOMEM;
}

static int af903x_suspend(struct usb_interface *intf, pm_message_t message)
{
	int error;
	deb_data("Enter %s Function, message=0x%ux\n",__FUNCTION__, message.event);
	error = DL_ApCtrl(0);
	if (error) deb_data("DL_ApCtrl error : 0x%x\n", error);

// If selective suspend is not supported, this must be a S3/S4 suspend,
// in which case we choose to reboot AF903x so it can work after resuming on EeePC.
#if !defined(CONFIG_USB_SUSPEND)
	DL_Reboot();	
#endif

	return 0;
}

static int af903x_resume(struct usb_interface *intf)
{
	int retval = -ENOMEM;
	deb_data("Enter %s Function\n",__FUNCTION__);
	
	retval = DL_ApCtrl(1);
	if (retval) deb_data("DL_ApCtrl error : 0x%x\n", retval);

	retval = Device_init(interface_to_usbdev(intf),intf,&DC, false);
        if (retval){
                if(retval) deb_data("Device_init Fail: 0x%08x\n", retval);
        }

        return 0;
}

static struct usb_driver af903x_driver = {
#if LINUX_VERSION_CODE <=  KERNEL_VERSION(2,6,15)
	.owner = THIS_MODULE,
#endif
	.name       = "dvb-usb-a867",
	.probe      = af903x_probe,
	.disconnect = dvb_usb_device_exit,
	.id_table   = af903x_usb_id_table,
	.suspend    = af903x_suspend,
	.resume     = af903x_resume,
#if LINUX_VERSION_CODE >  KERNEL_VERSION(2,6,22)
	.reset_resume = af903x_resume,
#endif
	.supports_autosuspend = 1,
};

static int __init af903x_module_init(void)
{
	int result;

	printk("AVerMedia A867 driver module V%s loaded.\n", DRIVER_VER);

	if ((result = usb_register(&af903x_driver))) {
		err("usb_register failed. Error number %d",result);
		return result;
	}
	return 0;
}

static void __exit af903x_module_exit(void)
{
	usb_deregister(&af903x_driver);
	printk("AVerMedia A867 driver module V%s unloaded.\n", DRIVER_VER);
}

module_init (af903x_module_init);
module_exit (af903x_module_exit);

MODULE_AUTHOR("MPD Linux Team");
MODULE_DESCRIPTION("AVerMedia A867");
MODULE_VERSION(DRIVER_VER);
MODULE_LICENSE("GPL");
