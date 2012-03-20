#include "a867_af903x.h"
//#include "a867_firmware.h"
#include "a867_ofdm.h"

#define true 1
#define false 0

#define FW_VER         0x08060000
int dvb_usb_af903x_debug = 0; // disable all level by default
module_param_named(debug,dvb_usb_af903x_debug, int, 0644);
MODULE_PARM_DESC(debug, "set debugging level (info=1, deb_fw=2, deb_fwdata=4, deb_data=8 (or-able)), default=0");

int dvb_usb_af903x_snrdb = 0; // output SNR 16bit format
module_param_named(snrdb,dvb_usb_af903x_snrdb, int, 0644);
MODULE_PARM_DESC(snrdb, "set SNR format (16bit=0, dB decibel=1), default=0");

struct usb_device *udevs = NULL;
struct usb_interface *uintfs = NULL;
PDEVICE_CONTEXT PDC;

  
//************** DRV_ *************//

static DWORD DRV_ResetPID(
    IN	void*	handle,
    IN	BYTE	ucSlaveDemod)
{
	DWORD dwError = Error_NO_ERROR;
    PDEVICE_CONTEXT pdc = (PDEVICE_CONTEXT)handle;
          
    //Clear pidTable
    dwError = Demodulator_resetPid ((Demodulator*) &pdc->Demodulator, ucSlaveDemod);

	return dwError;
}

static DWORD DRV_PIDOnOff(
	void	*handle, 
	BYTE	ucSlaveDemod,
	DWORD	dwOnOff)
{
	DWORD dwError = Error_NO_ERROR;
	PDEVICE_CONTEXT pdc = (PDEVICE_CONTEXT)handle;

	if(dwOnOff)
	{
        dwError = Demodulator_controlPidFilter ((Demodulator*) &pdc->Demodulator, ucSlaveDemod, 1);
	}
	else
	{
        dwError = Demodulator_controlPidFilter ((Demodulator*) &pdc->Demodulator, ucSlaveDemod, 0);
	}

	return dwError;
}

static DWORD DRV_AddPID(
	void	*handle,
	BYTE	ucSlaveDemod,
	DWORD	index,
	Pid		pid)
{
	DWORD dwError = Error_NO_ERROR;

    PDEVICE_CONTEXT pdc = (PDEVICE_CONTEXT)handle;
          
    dwError = Demodulator_addPidToFilter ((Demodulator*) &pdc->Demodulator, ucSlaveDemod,index, pid);

	return dwError;
}

DWORD DRV_RemovePID(
    IN  void*   handle,
    IN  BYTE    ucSlaveDemod,
    IN  Byte    index,
    IN  Pid     pid
)
{
//    deb_data("- Enter %s Function - , index:%d, pid:%x \n",__FUNCTION__, index, pid.value);

    DWORD   dwError = Error_NO_ERROR;

    PDEVICE_CONTEXT pdc = (PDEVICE_CONTEXT)handle;

    dwError = Demodulator_removePidAt ((Demodulator*) &pdc->Demodulator, ucSlaveDemod,index, pid);

    return(dwError);

}



static DWORD DRV_IrTblDownload(IN      void * handle)
{
        DWORD dwError = Error_NO_ERROR;
        PDEVICE_CONTEXT pdc = (PDEVICE_CONTEXT)handle;
        struct file *filp;
        unsigned char b_buf[512] ;
        int i, fileSize;
        mm_segment_t oldfs;

        deb_data("- Enter %s Function -\n",__FUNCTION__);

        oldfs=get_fs();
        set_fs(KERNEL_DS);

        filp=filp_open("/lib/firmware/af35irtbl.bin", O_RDWR,0644);
        if ( IS_ERR(filp) ) {
                deb_data("      LoadIrTable : Can't open file\n");goto exit;}

        if ( (filp->f_op) == NULL ) {
                deb_data("      LoadIrTable : File Operation Method Error!!\n");goto exit;}

        filp->f_pos=0x00;
        fileSize = filp->f_op->read(filp,b_buf,sizeof(b_buf),&filp->f_pos);

        for(i=0; i<fileSize; i++)
        {
              //deb_data("\n Data %d",i); //
              //deb_data("0x%x",b_buf[i]);//
              // dwError = Af901xWriteReg(ucDemod2WireAddr, 0, MERC_IR_TABLE_BASE_ADDR + i, b_buf[i]);
              //if (dwError) goto exit;
        }

        dwError = Demodulator_loadIrTable((Demodulator*) &pdc->Demodulator, (Word)fileSize, b_buf);
        if (dwError) {deb_data("Demodulator_loadIrTable fail"); goto exit;}

        filp_close(filp, NULL);
        set_fs(oldfs);

        return (dwError);
exit:
        deb_data("LoadIrTable fail!\n");
	return dwError;

}

static DWORD  DRV_GetEEPROMConfig2(
        void *      handle,
        BYTE       ucSlaveDemod)
{

	DWORD dwError = Error_NO_ERROR;
    	tWORD    shift = 0;
    	PDEVICE_CONTEXT pdc = (PDEVICE_CONTEXT)handle;
	BYTE btmp = 0;

	deb_data("- Enter %s Function -",__FUNCTION__);
	
    	if(ucSlaveDemod) shift = EEPROM_SHIFT;
    
    	dwError = Demodulator_readRegisters((Demodulator*) &pdc->Demodulator, 0, Processor_LINK, EEPROM_TUNERID+shift, 1, &btmp);
    	if (dwError) goto exit;
    	deb_data("EEPROM_TUNERID%d  = 0x%02X\n", ucSlaveDemod, btmp);		
    	PTI.TunerId = btmp;  

exit:
    
    return(dwError);     
}  

static DWORD DRV_SetFreqBw(
    	void*	handle,      
     	BYTE 	ucSlaveDemod,
      	DWORD   dwFreq,      
      	WORD	ucBw
)
{
	DWORD dwError = Error_NO_ERROR;
    
	PDEVICE_CONTEXT pdc = (PDEVICE_CONTEXT)handle;

	//Bool bLock = true;

	deb_data("- Enter %s Function -\n ",__FUNCTION__);
	deb_data("       ucSlaveDemod = %d, Freq= %d, BW=%d\n", ucSlaveDemod, dwFreq, ucBw);

    if (pdc->fc[ucSlaveDemod].bEnPID)
    {
		deb_data("		Reset HW PID table\n ");
        //Disable PID filter
		Demodulator_writeRegisterBits ((Demodulator*) &pdc->Demodulator, ucSlaveDemod, Processor_OFDM, p_mp2if_pid_en, mp2if_pid_en_pos, mp2if_pid_en_len, 0);
		
    }
	
    down(&PDC->tunerLock);

    PDC->fc[0].AVerFlags &= ~(0x08);
    PDC->fc[0].AVerFlags |= 0x04;
    PTI.bSettingFreq = true; //before acquireChannel, it is ture;  otherwise, it is false

    if(dwFreq) {
        pdc->fc[ucSlaveDemod].ulDesiredFrequency = dwFreq;
    }
    else {
        dwFreq = pdc->fc[ucSlaveDemod].ulDesiredFrequency;
    }

    if(ucBw) {
        pdc->fc[ucSlaveDemod].ucDesiredBandWidth = ucBw*1000;
	}
    else {
        ucBw = pdc->fc[ucSlaveDemod].ucDesiredBandWidth;
    	}

    deb_data("Real Freq= %d, BW=%d\n", pdc->fc[ucSlaveDemod].ulDesiredFrequency, pdc->fc[ucSlaveDemod].ucDesiredBandWidth);
           

    if(!PTI.bTunerInited){
        deb_data("    Skip SetFreq - Tuner is still off!\n");
        goto exit;
    }
	
    PTI.bTunerOK = false;        

    if (pdc->fc[ucSlaveDemod].ulDesiredFrequency!=0 && pdc->fc[ucSlaveDemod].ucDesiredBandWidth!=0)	
    {
	deb_data("AcquireChannel : Real Freq= %d, BW=%d\n", pdc->fc[ucSlaveDemod].ulDesiredFrequency, pdc->fc[ucSlaveDemod].ucDesiredBandWidth);
	dwError = Demodulator_acquireChannel ((Demodulator*) &pdc->Demodulator, ucSlaveDemod, pdc->fc[ucSlaveDemod].ucDesiredBandWidth, pdc->fc[ucSlaveDemod].ulDesiredFrequency);  
	//PTI.bSettingFreq = false;
    	if (dwError) 
    	{
        	deb_data("Demod_acquireChannel fail! 0x%08x\n", dwError);
        	goto exit;
    	}	
	else //when success acquireChannel, record currentFreq/currentBW.
	{
		pdc->fc[ucSlaveDemod].ulCurrentFrequency = pdc->fc[ucSlaveDemod].ulDesiredFrequency;	
		pdc->fc[ucSlaveDemod].ucCurrentBandWidth = pdc->fc[ucSlaveDemod].ucDesiredBandWidth;  
	}
    }
    else {
       	deb_data("Demod_acquireChannel skipped\n");
    }

    if(pdc->StreamType == StreamType_DVBT_DATAGRAM) {
        PDC->fc[ucSlaveDemod].OvrFlwChk = CHECK_LOCK_LOOPS ;
        PDC->fc[ucSlaveDemod].UnLockCount = 0;
    }

    PTI.bTunerOK = true;

exit:

    PTI.bSettingFreq = false;
    up(&PDC->tunerLock);

    return(dwError);  
}

static DWORD DRV_getFirmwareVersionFromFile( 
	 	Processor	processor, 
		DWORD* 		version
)
{
    DWORD OFDM_VER1 = DVB_OFDM_VERSION1;
    DWORD OFDM_VER2 = DVB_OFDM_VERSION2;
    DWORD OFDM_VER3 = DVB_OFDM_VERSION3;
    DWORD OFDM_VER4 = DVB_OFDM_VERSION4;

    DWORD LINK_VER1 = DVB_LL_VERSION1;
    DWORD LINK_VER2 = DVB_LL_VERSION2;
    DWORD LINK_VER3 = DVB_LL_VERSION3;    
    DWORD LINK_VER4 = DVB_LL_VERSION4;    

    if(processor == Processor_OFDM) {
        *version = (DWORD)( (OFDM_VER1 << 24) + (OFDM_VER2 << 16) + (OFDM_VER3 << 8) + OFDM_VER4);
    }
    else { //LINK
        *version = (DWORD)( (LINK_VER1 << 24) + (LINK_VER2 << 16) + (LINK_VER3 << 8) + LINK_VER4);    
    }
    
    return *version;
}

static DWORD  DRV_Initialize(
	    void *      handle
)
{
	DWORD error = Error_NO_ERROR;
	 
	PDEVICE_CONTEXT pdc = (PDEVICE_CONTEXT)handle;

	DWORD fileVersion, cmdVersion = 0; 

	deb_data("- Enter %s Function -\n",__FUNCTION__);

	if(pdc->Demodulator.booted) //from Standard_setBusTuner() > Standard_getFirmwareVersion()
    	{
        	//use "#define version" to get fw version (from firmware.h title)
        	error = DRV_getFirmwareVersionFromFile(Processor_OFDM, &fileVersion);

        	//use "Command_QUERYINFO" to get fw version 
        	error = Demodulator_getFirmwareVersion((Demodulator*) &pdc->Demodulator, Processor_OFDM, &cmdVersion);
        	if(error) deb_data("DRV_Initialize : Demodulator_getFirmwareVersion : error = 0x%08x\n", error);

        	if(cmdVersion != fileVersion)
        	{
            		deb_data("Reboot: Outside Fw = 0x%X, Inside Fw = 0x%X", fileVersion, cmdVersion);               
            		error = Demodulator_reboot((Demodulator*) &pdc->Demodulator);
            		pdc->bBootCode = true;
            		if(error) 
            		{
                		deb_data("Demodulator_reboot : error = 0x%08x\n", error);
                		return error;
            		}
            		else {
                		return Error_NOT_READY;
            		}
        	}
        	else
        	{
            		deb_data("	Fw version is the same!\n");
  	      		error = Error_NO_ERROR;
        	}
	}//pdc->Demodulator.booted
	
ReInit:  //Patch for NIM fail or disappear, Maggie   
    error = Demodulator_initialize ((Demodulator*) &pdc->Demodulator, pdc->Demodulator.chipNumber , 8000, pdc->StreamType, pdc->architecture);    
    if (error) 
    { 
        deb_data("Device initialize fail : 0x%08x\n", error);
        if( ((error&Error_FIRMWARE_STATUS) && (error&0x10)) && (pdc->Demodulator.chipNumber>1) )
        {
            pdc->Demodulator.cmdDescription->sendCommand ((Demodulator*) &pdc->Demodulator, Command_FW_DOWNLOAD_END, 0, Processor_LINK, 0, NULL, 0, NULL);

            deb_data("	Retry to download FW with Single TS\n");
            pdc->Demodulator.chipNumber = 1;
            pdc->bDualTs = false;
            error = Demodulator_writeRegister ((Demodulator*) &pdc->Demodulator, 0, Processor_LINK, 0x417F, 0);
            goto ReInit;
       }
    }
    else {
        deb_data("    Device initialize Ok!!\n");
    }

	error = Demodulator_writeRegisterBits ((Demodulator*) &pdc->Demodulator, 0, Processor_LINK, 
											p_reg_usb_min_len, reg_usb_min_len_pos, reg_usb_min_len_len, 1);

    Demodulator_getFirmwareVersion ((Demodulator*) &pdc->Demodulator, Processor_OFDM, &cmdVersion);
    deb_data("    FwVer OFDM = 0x%X, ", cmdVersion);
    Demodulator_getFirmwareVersion ((Demodulator*) &pdc->Demodulator, Processor_LINK, &cmdVersion);
    deb_data("FwVer LINK = 0x%X\n", cmdVersion);
    
    return error;
	
}

static DWORD DRV_InitDevInfo(
    	void *      handle,
    	BYTE        ucSlaveDemod
)
{
    DWORD dwError = Error_NO_ERROR;    
 
    PDC->fc[ucSlaveDemod].ulCurrentFrequency = 0;  
    PDC->fc[ucSlaveDemod].ucCurrentBandWidth = 0;

    PDC->fc[ucSlaveDemod].ulDesiredFrequency = 0;	
    PDC->fc[ucSlaveDemod].ucDesiredBandWidth = 6000;	

    //For PID Filter Setting
    //PDC->fc[ucSlaveDemod].ulcPIDs = 0;    
    PDC->fc[ucSlaveDemod].bEnPID = true;
    PDC->fc[ucSlaveDemod].bApOn = false;
    PDC->fc[ucSlaveDemod].bResetTs = false;

    PTI.bTunerOK = false;
    PTI.bSettingFreq = false;

    return dwError;
}	

static DWORD DRV_GetEEPROMConfig(    
	void *      handle)
{
	DWORD dwError = Error_NO_ERROR;
	PDEVICE_CONTEXT pdc = (PDEVICE_CONTEXT)handle;
    	BYTE ucSlaveDemod = 0;
	BYTE btmp = 0;

	deb_data("- Enter %s Function -",__FUNCTION__);	
   
	//bIrTblDownload option
	dwError =   Demodulator_readRegisters((Demodulator*) &pdc->Demodulator, 0, Processor_LINK, EEPROM_IRMODE, 1, &btmp);
	if (dwError) return(dwError);
	PDC->bIrTblDownload = btmp ? true:false;
	deb_data(	"EEPROM_IRMODE = 0x%02X, ", btmp);
        deb_data("bIrTblDownload %s\n", PDC->bIrTblDownload?"ON":"OFF");
 
    	PDC->bDualTs = false;
    	PDC->architecture = Architecture_DCA;
    	PDC->Demodulator.chipNumber = 1;    
    	PDC->bDCAPIP = false;


        dwError = DRV_GetEEPROMConfig2(pdc, ucSlaveDemod);
	 if (dwError) return(dwError);  
        dwError = DRV_InitDevInfo(pdc, ucSlaveDemod);
    
   	 return(dwError);     
}   

static DWORD DRV_SetBusTuner(
	 void * handle, 
	 Word busId, 
	 Word tunerId
)
{
	DWORD dwError = Error_NO_ERROR;
	DWORD 	 version = 0;

	PDEVICE_CONTEXT pdc = (PDEVICE_CONTEXT)handle;

	deb_data("- Enter %s Function -",__FUNCTION__);
	deb_data("busId = 0x%x, tunerId =0x%x\n", busId, tunerId);

	if ((pdc->UsbMode==0x0110) && (busId==Bus_USB)) {
        busId=Bus_USB11;    
    }
    
    	dwError = Demodulator_setBusTuner ((Demodulator*) &pdc->Demodulator, busId, tunerId);
	if (dwError) {deb_data("Demodulator_setBusTuner error\n");return dwError;}
	dwError = Demodulator_getFirmwareVersion ((Demodulator*) &pdc->Demodulator, Processor_LINK, &version);
    	if (version != 0) {
        	pdc->Demodulator.booted = True;
    	} 
    	else {
        	pdc->Demodulator.booted = False;
    	}
    	return(dwError); 
}


DWORD A333TunerPowerControl(
	PDEVICE_CONTEXT pdc,	
	BYTE    ucSlaveDemod,
	bool        bPowerOn
)
{

	DWORD dwError = Error_NO_ERROR;

	if(bPowerOn)
       	PTI.bTunerInited = true;
	else
		PTI.bTunerInited = false;    

    //control oscilator	
   	dwError = Demodulator_writeRegister((Demodulator*) &PDC->Demodulator, 0, Processor_LINK, PDC->Map.GPIO_OSC_en, 1);
    dwError = Demodulator_writeRegister((Demodulator*) &PDC->Demodulator, 0, Processor_LINK, PDC->Map.GPIO_OSC_on, 1);
   	dwError = Demodulator_writeRegister((Demodulator*) &PDC->Demodulator, 0, Processor_LINK, PDC->Map.GPIO_OSC_o, 1); 

    dwError = Demodulator_writeRegister((Demodulator*) &PDC->Demodulator, 0, Processor_LINK, PDC->Map.GPIO_TUR1_en, 1);
   	dwError = Demodulator_writeRegister((Demodulator*) &PDC->Demodulator, 0, Processor_LINK, PDC->Map.GPIO_TUR1_on, 1);
    dwError = Demodulator_writeRegister((Demodulator*) &PDC->Demodulator, 0, Processor_LINK, PDC->Map.GPIO_TUR1_o, 1); 
   	dwError = Demodulator_writeRegister((Demodulator*) &PDC->Demodulator, 0, Processor_LINK, PDC->Map.GPIO_TUR2_en, 1);
    dwError = Demodulator_writeRegister((Demodulator*) &PDC->Demodulator, 0, Processor_LINK, PDC->Map.GPIO_TUR2_on, 1);
   	dwError = Demodulator_writeRegister((Demodulator*) &PDC->Demodulator, 0, Processor_LINK, PDC->Map.GPIO_TUR2_o, 1);

    if(bPowerOn) 
	{
   		dwError=Demodulator_writeRegister((Demodulator*) &PDC->Demodulator, 0, Processor_LINK, PDC->Map.GPIO_OSC_o, 1); 
       	dwError=Demodulator_writeRegister((Demodulator*) &PDC->Demodulator, 0, Processor_LINK, PDC->Map.GPIO_TUR1_o, 1);
       	dwError=Demodulator_writeRegister((Demodulator*) &PDC->Demodulator, 0, Processor_LINK, PDC->Map.GPIO_TUR2_o, 1);
        if(pdc->bTunerPowerOff == true) 
   	    {
       	    dwError = Demodulator_initialize ((Demodulator*) &pdc->Demodulator, pdc->Demodulator.chipNumber, 
											pdc->Demodulator.bandwidth[0], pdc->StreamType, pdc->architecture);  
			pdc->bTunerPowerOff = false;
		}              	        
		
    }
	else 
	{ // power off
	
		if(pdc->architecture == Architecture_PIP)
		{
			if(pdc->fc[0].tunerinfo.bTunerInited == 0 && pdc->fc[1].tunerinfo.bTunerInited == 0) 
           	{                                
               	if(pdc->bTunerPowerOff == false) 
               	{
                   	dwError = Demodulator_finalize((Demodulator*) &pdc->Demodulator);
					pdc->bTunerPowerOff = true;
				}
       	        dwError = Demodulator_writeRegister((Demodulator*)&PDC->Demodulator,0,Processor_LINK,PDC->Map.GPIO_TUR1_o, 0);
           	    dwError = Demodulator_writeRegister((Demodulator*)&PDC->Demodulator,0,Processor_LINK,PDC->Map.GPIO_TUR2_o, 0);
   	        	dwError = Demodulator_writeRegister((Demodulator*)&PDC->Demodulator,0,Processor_LINK,PDC->Map.GPIO_OSC_o, 0);
            }
        }
		else 
		{
			if(pdc->bTunerPowerOff == false) 
            {                
               	dwError = Demodulator_finalize((Demodulator*) &pdc->Demodulator);
	        	pdc->bTunerPowerOff = true;
			}      

			dwError = Demodulator_writeRegister((Demodulator*) &PDC->Demodulator, 0, Processor_LINK, PDC->Map.GPIO_TUR1_o, 0);
			dwError = Demodulator_writeRegister((Demodulator*) &PDC->Demodulator, 0, Processor_LINK, PDC->Map.GPIO_TUR2_o, 0);
			dwError = Demodulator_writeRegister((Demodulator*) &PDC->Demodulator, 0, Processor_LINK, PDC->Map.GPIO_OSC_o, 0);
		}        	
	}

	return dwError;
}

DWORD A337TunerPowerControl(
	PDEVICE_CONTEXT pdc,	
	BYTE    ucSlaveDemod,
	bool        bPowerOn
)
{

	DWORD dwError = Error_NO_ERROR;

	if(bPowerOn)
		PTI.bTunerInited = true;
	else
		PTI.bTunerInited = false;    

	if(bPowerOn) //tuner on
	{
		if(pdc->bTunerPowerOff == true) 
		{
			//use variable to control gpio

			// enable tuner power
			dwError = Demodulator_writeRegister((Demodulator*) &pdc->Demodulator, 0, Processor_LINK, pdc->Map.GPIO_TUR1_en, 1);
			dwError = Demodulator_writeRegister((Demodulator*) &pdc->Demodulator, 0, Processor_LINK, pdc->Map.GPIO_TUR1_on, 1);
			dwError = Demodulator_writeRegister((Demodulator*) &pdc->Demodulator, 0, Processor_LINK, pdc->Map.GPIO_TUR2_en, 1);
			dwError = Demodulator_writeRegister((Demodulator*) &pdc->Demodulator, 0, Processor_LINK, pdc->Map.GPIO_TUR2_on, 1);

			dwError = Demodulator_writeRegister((Demodulator*) &pdc->Demodulator, 0, Processor_LINK, pdc->Map.GPIO_TUR1_o, 1); 
			mdelay(100);
			dwError = Demodulator_writeRegister((Demodulator*) &pdc->Demodulator, 0, Processor_LINK, pdc->Map.GPIO_TUR2_o, 1);
			mdelay(100);
			dwError = Demodulator_writeRegister((Demodulator*) &pdc->Demodulator, 0, Processor_LINK, p_reg_top_gpioh12_en, 1);
			dwError = Demodulator_writeRegister((Demodulator*) &pdc->Demodulator, 0, Processor_LINK, p_reg_top_gpioh12_on, 1);

				
			// reset tuner
			deb_data("A337 reset");
			mdelay(10);
			dwError = Demodulator_writeRegister((Demodulator*) &pdc->Demodulator, 0, Processor_LINK, p_reg_top_gpioh12_o, 0); 
			mdelay(30);
			dwError = Demodulator_writeRegister((Demodulator*) &pdc->Demodulator, 0, Processor_LINK, p_reg_top_gpioh12_o, 1);

			mdelay(300);
			deb_data("pdc->bTunerPowerOff == true\n");
			dwError = Demodulator_initialize ((Demodulator*) &pdc->Demodulator, pdc->Demodulator.chipNumber , pdc->Demodulator.bandwidth[0], pdc->StreamType, pdc->architecture);  
			pdc->bTunerPowerOff = false;
		}              	        
	}
	else //tuner off
	{
		// Bugfix: wrong level of tuner i2c whiling plugging in device.
		dwError = Demodulator_finalize((Demodulator*) &pdc->Demodulator);
		if(pdc->bTunerPowerOff == false) 
		{                
			pdc->bTunerPowerOff = true;

			dwError = Demodulator_writeRegister((Demodulator*) &pdc->Demodulator, 0, Processor_LINK, p_reg_top_gpioh12_o, 0);
			dwError = Demodulator_writeRegister((Demodulator*) &pdc->Demodulator, 0, Processor_LINK, p_reg_top_gpioh12_en, 0);
			dwError = Demodulator_writeRegister((Demodulator*) &pdc->Demodulator, 0, Processor_LINK, p_reg_top_gpioh12_on, 0);
			mdelay(10);

			// disable tuner power
			dwError = Demodulator_writeRegister((Demodulator*) &pdc->Demodulator, 0, Processor_LINK, pdc->Map.GPIO_TUR1_o, 0);
			dwError = Demodulator_writeRegister((Demodulator*) &pdc->Demodulator, 0, Processor_LINK, pdc->Map.GPIO_TUR2_o, 0);
			dwError = Demodulator_writeRegister((Demodulator*) &pdc->Demodulator, 0, Processor_LINK, pdc->Map.GPIO_TUR1_en,0);
			dwError = Demodulator_writeRegister((Demodulator*) &pdc->Demodulator, 0, Processor_LINK, pdc->Map.GPIO_TUR1_on,0);
			dwError = Demodulator_writeRegister((Demodulator*) &pdc->Demodulator, 0, Processor_LINK, pdc->Map.GPIO_TUR2_en,0);
			dwError = Demodulator_writeRegister((Demodulator*) &pdc->Demodulator, 0, Processor_LINK, pdc->Map.GPIO_TUR2_on,0);
		}      
	}

	return dwError;
}


static DWORD DRV_TunerPowerCtrl(
    	void *	handle, 
     	BYTE	ucSlaveDemod,
     	bool		bPowerOn
)
{ 
    DWORD dwError = Error_NO_ERROR;	

    PDEVICE_CONTEXT pdc = (PDEVICE_CONTEXT)handle;

    deb_data("- Enter %s Function , bPowerOn=%d -\n",__FUNCTION__, bPowerOn);

	deb_data("Detected tuner ID: 0x%x\n", pdc->fc[0].tunerinfo.TunerId);

	switch(PDC->idProduct)
	{
	case 0xa337:	//A337
	case 0x0337:	//A867
	case 0xa867:	//A867
    case 0x0867:
    case 0x1867:
	case 0xF337:
		dwError = A337TunerPowerControl(pdc, ucSlaveDemod, bPowerOn);
		break;
	case 0xa333:	//A337 & EVB
	default:	
		dwError = A333TunerPowerControl(pdc, ucSlaveDemod, bPowerOn);
	}

	return dwError;
}

static DWORD DRV_ApCtrl (
      void *      handle,
      Byte        ucSlaveDemod,
      Bool        bOn
)
{
	DWORD dwError = Error_NO_ERROR;

        PDEVICE_CONTEXT pdc = (PDEVICE_CONTEXT)handle;

	deb_data("enter DRV_ApCtrl: ucSlaveDemod = %d, bOn = %s\n", ucSlaveDemod, bOn?"ON":"OFF"); 

      //deb_data("enter DRV_ApCtrl: Demod[%d].GraphBuilt = %d", ucSlaveDemod, pdc->fc[ucSlaveDemod].GraphBuilt); 
	
   	Demodulator_writeRegister((Demodulator*) &PDC->Demodulator, 0, Processor_LINK, PDC->Map.GPIO_LED_en, 1); 
   	Demodulator_writeRegister((Demodulator*) &PDC->Demodulator, 0, Processor_LINK, PDC->Map.GPIO_LED_on, 1); 
   	Demodulator_writeRegister((Demodulator*) &PDC->Demodulator, 0, Processor_LINK, PDC->Map.GPIO_LED_o, bOn?1:0); 


	dwError = DRV_TunerPowerCtrl(handle, ucSlaveDemod, bOn);
       	if(dwError) deb_data("DRV_TunerPowerCtrl Fail: 0x%08x\n", dwError); 

	
    	dwError = Demodulator_controlPowerSaving((Demodulator*) &pdc->Demodulator, ucSlaveDemod, bOn);   
    	if(dwError) deb_data("DRV_ApCtrl: Demodulator_controlPowerSaving error = 0x%08x\n", dwError);
	
    return(dwError);
}


static DWORD DRV_TunerWakeup(
      void *     handle
)
{   
    	DWORD dwError = Error_NO_ERROR;

    	PDEVICE_CONTEXT pdc = (PDEVICE_CONTEXT) handle;

	deb_data("- Enter %s Function -\n",__FUNCTION__);

	//tuner power on
	dwError = Demodulator_writeRegisterBits((Demodulator*) &pdc->Demodulator, 0, Processor_LINK,  p_reg_top_gpioh7_o, reg_top_gpioh7_o_pos, reg_top_gpioh7_o_len, 1);	 

    return(dwError);

}

static DWORD DRV_Reboot(
	void * handle
)
{
    	DWORD dwError = Error_NO_ERROR;

    	PDEVICE_CONTEXT pdc = (PDEVICE_CONTEXT) handle;

	deb_data("- Enter %s Function -\n",__FUNCTION__);

	dwError = Demodulator_reboot((Demodulator*) &pdc->Demodulator);

	return(dwError);
}
//s021+e

static DWORD DRV_IsPsbOverflow(
	void *	handle,
	Byte	ucSlaveDemod,
	Bool	*bPsbOverflow
)
{
	DWORD dwError = Error_NO_ERROR;
	Byte ucValue;
        PDEVICE_CONTEXT pdc = (PDEVICE_CONTEXT)handle;
	Bool PsbOverflow;

	deb_data("enter %s: - \n", __FUNCTION__); 

	if( ucSlaveDemod ) { //13
		dwError = Demodulator_readRegister((Demodulator*) &pdc->Demodulator, 0, Processor_OFDM, p_reg_sys_buf_overflow, &ucValue);
		if( dwError ) goto exit;

		PsbOverflow = (ucValue&0x01)? 1:0;
		if( PsbOverflow ) {
			dwError = Demodulator_writeRegister((Demodulator*) &pdc->Demodulator, 0, Processor_OFDM, p_reg_sys_buf_overflow, ucValue&(~0x01));
			if( dwError ) goto exit;
		}
	}
	else { //15
		dwError = Demodulator_readRegister((Demodulator*) &pdc->Demodulator, 0, Processor_OFDM, p_mp2if_psb_overflow, &ucValue);
		if( dwError ) goto exit;

		PsbOverflow = (ucValue&0x01)? 1:0;
		if( PsbOverflow ) {
			dwError = Demodulator_writeRegister((Demodulator*) &pdc->Demodulator, 0, Processor_OFDM, p_mp2if_psb_overflow, ucValue&(~0x01));
			if( dwError ) goto exit;
		}
	}

	if( bPsbOverflow ) *bPsbOverflow = PsbOverflow;
exit:
	return dwError;
}

//************** DL_ *************//
//
DWORD DL_ResetPID(void)
{
	DWORD dwError = Error_NO_ERROR;

    dwError = DRV_ResetPID(PDC, 0);

	return dwError;
}

DWORD DL_AddPID(BYTE index, Pid pid)
{
	DWORD dwError = Error_NO_ERROR;

	dwError = DRV_AddPID(PDC, 0, index, pid);
	
	return dwError;
}

DWORD DL_PIDOnOff(DWORD OnOff)
{
	DWORD dwError = Error_NO_ERROR;

	dwError = DRV_PIDOnOff(PDC, 0, OnOff);

	return dwError;
}

DWORD DL_RemovePID(
    IN  Byte    index,
    IN  Pid     pid
)
{

    DWORD dwError = Error_NO_ERROR;
	
	dwError = DRV_RemovePID(PDC, 0, index, pid);

    return(dwError);
}


DWORD DL_IsPsbOverflow(
	void *	handle,
	Byte	ucSlaveDemod,
	Bool *	bPsbOverflow
)
{
	DWORD dwError = DRV_IsPsbOverflow(handle, ucSlaveDemod, bPsbOverflow);
	return dwError;
}


DWORD DL_MonitorReception(Bool *lock)
{
	DWORD dwError = Error_NO_ERROR;
	BYTE    ucSlaveDemod=0;
	Bool bLock = False;
	ChannelStatistic stat;

	deb_data("- Enter %s Function , OvrFlwChk=%d, UnLockCount=%d-\n",__FUNCTION__,
			PDC->fc[ucSlaveDemod].OvrFlwChk,
			PDC->fc[ucSlaveDemod].UnLockCount);

	down(&PDC->tunerLock);

	if( PDC->fc[ucSlaveDemod].ulDesiredFrequency==0 || PDC->fc[ucSlaveDemod].ucDesiredBandWidth==0 ) {
		if( lock ) *lock = False;
		deb_data("- %s Function skipping\n",__FUNCTION__);
		goto exit;
	}

	// check lock status
	dwError= Demodulator_isLocked((Demodulator*) &PDC->Demodulator, ucSlaveDemod, &bLock);
	if( dwError!=Error_NO_ERROR ) {
		goto exit;
	}

	// consider as unlock if UBC is not zero
	dwError = Demodulator_getChannelStatistic((Demodulator*) &PDC->Demodulator, ucSlaveDemod, &stat);
	if( dwError!=Error_NO_ERROR ) {
		goto exit;
	}

//uncomment this because this causes instability in channel scan.


	// report lock status
	if( lock ) *lock = bLock;
	deb_data("- %s Function, LOCK = %d\n", __FUNCTION__, bLock);

	// stop monitoring
	if(PDC->fc[ucSlaveDemod].OvrFlwChk<1) {
		deb_data("- %s Function end of monitor cycle -\n",__FUNCTION__);

		// if lock is lost for a while, try to reacquire channel
		if( PDC->fc[ucSlaveDemod].UnLockCount >= CHECK_LOCK_LOOPS*2/3) {
			WORD bw = PDC->fc[ucSlaveDemod].ucDesiredBandWidth;
			DWORD freq = PDC->fc[ucSlaveDemod].ulDesiredFrequency;
	

			deb_data("- %s Function need to reacquire channel, freq=%d, bw=%d-\n",__FUNCTION__,
					freq, bw);

			// reacquire channel
			// first power off, then power on
			DRV_ApCtrl (PDC, 0, 0);
			User_delay(0, 500);
			DRV_ApCtrl (PDC, 0, 1);
			User_delay(0, 500);

			// switch to another frequency, say 500MHz
			deb_data("- %s Function switch to 500MHz first -\n",__FUNCTION__);
			Demodulator_acquireChannel ((Demodulator*) &PDC->Demodulator, ucSlaveDemod,
							bw, 500000);
			User_delay(0, 500);
			// now change to original frequency
			deb_data("- %s Function switch to %d KHz later -\n",__FUNCTION__, freq);
			Demodulator_acquireChannel ((Demodulator*) &PDC->Demodulator, ucSlaveDemod,
							bw, freq);
		}	

		// restart monitor cycle
		PDC->fc[ucSlaveDemod].OvrFlwChk = CHECK_LOCK_LOOPS;
		PDC->fc[ucSlaveDemod].UnLockCount = 0;

		dwError = Error_NO_ERROR;
		goto exit;
	}
	
	PDC->fc[ucSlaveDemod].OvrFlwChk--;

	// maintain lock count
	if( !bLock ) PDC->fc[ucSlaveDemod].UnLockCount ++;

	deb_data("- Exit %s Function -\n",__FUNCTION__);

	// avoid race with setfreq
exit:
	up(&PDC->tunerLock);
	return dwError;
}


DWORD DL_GetLocked(Bool *bLock)
{
	DWORD dwError;
	BYTE    ucSlaveDemod=0;

	down(&PDC->tunerLock);

	if( bLock ) {
		dwError= Demodulator_isLocked((Demodulator*) &PDC->Demodulator, ucSlaveDemod, bLock);
	}
	else {
		dwError = Error_NULL_PTR;
	}

	up(&PDC->tunerLock);
	return dwError;
}


DWORD DL_GetSignalStrength(u16 *strength)
{
	DWORD dwError = Error_NO_ERROR;
	BYTE    ucSlaveDemod=0;
	Byte 	str;
	deb_data("- Enter %s Function -\n",__FUNCTION__);	 	

	dwError = Demodulator_getSignalStrength((Demodulator*) &PDC->Demodulator, ucSlaveDemod, &str);

	if( strength ) *strength = str;

	return dwError;
}

DWORD DL_GetSignalQuality(u16 *squality)
{
	DWORD dwError = Error_NO_ERROR;
	BYTE    ucSlaveDemod=0;
	Byte 	str;
	deb_data("- Enter %s Function -\n",__FUNCTION__);	 	

	dwError = Demodulator_getSignalQuality((Demodulator*) &PDC->Demodulator, ucSlaveDemod, &str);

	if( squality ) *squality = str;

	return dwError;
}


DWORD DL_GetChannelStat(u32 *ber, u32 *berbits, u32 *ubc)
{
	DWORD dwError = Error_NO_ERROR;
	BYTE    ucSlaveDemod=0;
	ChannelStatistic stat;
	deb_data("- Enter %s Function -\n",__FUNCTION__);	 	

	dwError = Demodulator_getChannelStatistic((Demodulator*) &PDC->Demodulator, ucSlaveDemod, &stat);
	// ignore error because saved value is returned in stat
	if( ber ) *ber = stat.postVitErrorCount;
	if( berbits ) *berbits = stat.postVitBitCount;
	if( ubc ) *ubc = stat.abortCount;

	return(dwError);
}


static DWORD DL_Initialize(
	    void *      handle
)
{
    	DWORD dwError = Error_NO_ERROR;  
    
     	dwError = DRV_Initialize(handle);   

	return (dwError); 
    
}

static DWORD DL_SetBusTuner(
	 void * handle, 
	 Word busId, 
	 Word tunerId
)
{

	DWORD dwError = Error_NO_ERROR;
	
    	dwError = DRV_SetBusTuner(handle, busId, tunerId);

	return (dwError);

}

static DWORD  DL_GetEEPROMConfig(
	 void *      handle
)
{   
    DWORD dwError = Error_NO_ERROR;

    dwError = DRV_GetEEPROMConfig(handle);

    return(dwError);
} 

static DWORD DL_TunerWakeup(
      void *     handle
)
{    
	DWORD dwError = Error_NO_ERROR;

	 dwError = DRV_TunerWakeup(handle);
   
    	return(dwError);
}
static DWORD  DL_IrTblDownload(
      void *     handle
)
{
        DWORD dwError = Error_NO_ERROR;

	dwError = DRV_IrTblDownload(handle);

        return(dwError);
}


DWORD DL_TunerPowerCtrl(u8 bPowerOn)
{
	DWORD dwError = Error_NO_ERROR;

	deb_data("enter DL_TunerPowerCtrl:  bOn = %s\n", bPowerOn?"ON":"OFF");

    	dwError = DRV_TunerPowerCtrl(PDC, 0, bPowerOn);

    	return (dwError);
}
//EXPORT_SYMBOL(DL_TunerPowerCtrl);


DWORD DL_ApCtrl (
      Bool        bOn
)
{
        DWORD dwError = Error_NO_ERROR;
	BYTE    ucSlaveDemod=0;
	//Bool bLock;

	down(&PDC->powerLock);

	deb_data("Enter DL_ApCtrl:  bOn = %s, use_cnt=%d\n", bOn?"ON":"OFF", PDC->power_use_count);

	// implement power management based on reference counting
	if( bOn ) PDC->power_use_count++;
	else PDC->power_use_count--;
	if( PDC->power_use_count < 0 ) PDC->power_use_count = 0;

	if( bOn && PDC->power_use_count==1 ) {

		deb_data("DL_ApCtrl: call DRV_ApCtrl(ON)\n");
	    	dwError = DRV_ApCtrl (PDC, 0, 1);
	}
	else if( !bOn && PDC->power_use_count==0 ) {
		deb_data("DL_ApCtrl: call DRV_ApCtrl(OFF)\n");

	    	dwError = DRV_ApCtrl (PDC, 0, 0);
		PDC->fc[ucSlaveDemod].ulDesiredFrequency = 0;
		PDC->fc[ucSlaveDemod].ucDesiredBandWidth = 0;
		PDC->fc[ucSlaveDemod].OvrFlwChk = 0;
	}

	up(&PDC->powerLock);

	deb_data("Exit DL_ApCtrl:  bOn = %s, dwError = %d\n", bOn?"ON":"OFF", dwError);
    	return(dwError);
}

// return 1 if the difference between freq1 & freq2 is smaller or equal than t.
static inline int Is_Within_Tolerance(u32 freq1, u32 freq2, u32 t)
{
	u32 diff = (freq1>freq2)? (freq1-freq2) : (freq2-freq1);
	if( diff<=t ) return 1;
	else return 0;
}


DWORD DL_Tuner_SetFreq(u32 dwFreq,u8 ucBw)
{

	DWORD dwError = Error_NO_ERROR;
	BYTE    ucSlaveDemod=0;
	
	deb_data("- Enter %s Function -\n",__FUNCTION__);
	if ( (PDC->fc[ucSlaveDemod].ulDesiredFrequency!=dwFreq 
		&& !Is_Within_Tolerance(PDC->fc[ucSlaveDemod].ulDesiredFrequency, dwFreq, 125) )
			|| PDC->fc[ucSlaveDemod].ucDesiredBandWidth!=ucBw*1000) 
	 	dwError = DRV_SetFreqBw(PDC, ucSlaveDemod, dwFreq, ucBw);
	else
		deb_data("     the same Frequency & BandWidth\n");
	 
    	return(dwError);	
}

DWORD DL_Tuner_SetBW(u8 ucBw)
{
	DWORD dwError = Error_NO_ERROR;
	BYTE    ucSlaveDemod=0;
	deb_data("- Enter %s Function -\n",__FUNCTION__);	 	
	if (PDC->fc[ucSlaveDemod].ucDesiredBandWidth!=ucBw*1000)
	 	dwError =  DRV_SetFreqBw(PDC, ucSlaveDemod, 0, ucBw);
	else
		deb_data("     the same BandWidth\n");

	return(dwError);
}

DWORD DL_ReSetInterval(void)
{
         DWORD dwError = Error_NO_ERROR;


         return(dwError);
}

DWORD DL_Reboot(void)
{
	DWORD dwError = Error_NO_ERROR;

	deb_data("- Enter %s Function -\n",__FUNCTION__);	 	

	dwError = DRV_Reboot(PDC);

	return(dwError);
}


DWORD Device_init(struct usb_device *udev,struct usb_interface *uintf, PDEVICE_CONTEXT PDCs, Bool bBoot)
{
	 DWORD error = Error_NO_ERROR;
	 BYTE filterIdx=0;
	 udevs=udev;
	 uintfs=uintf;
	 PDC=PDCs;

	deb_data("- Enter %s Function -\n",__FUNCTION__);

	//************* Set Device init Info *************//
	PDC->bEnterSuspend = false;
    	PDC->bSurpriseRemoval = false;
    	PDC->bDevNotResp = false;
    	PDC->bSelectiveSuspend = false; 
	PDC->bTunerPowerOff = false;

	if (bBoot)
	{
		PDC->bSupportSelSuspend = false;
		PDC->Demodulator.userData = (Handle)PDC;
		PDC->Demodulator.chipNumber =1;
		PDC->architecture=Architecture_DCA;
		PDC->Demodulator.frequency[0] = 666000;
		PDC->Demodulator.bandwidth[0] = 8000;
		PDC->bIrTblDownload = false;
		PDC->fc[0].tunerinfo.TunerId = 0;
		PDC->fc[1].tunerinfo.TunerId = 0;
		PDC->bDualTs=false;	
        	PDC->FilterCnt = 0;
		PDC->StreamType = StreamType_DVBT_DATAGRAM;
		PDC->UsbCtrlTimeOut = 1;

		//init_MUTEX(&PDC->powerLock);
		//init_MUTEX(&PDC->tunerLock);
		sema_init(&PDC->powerLock, 1);
		sema_init(&PDC->tunerLock, 1);
		
		PDC->power_use_count = 0;

		PDC->idVendor = udev->descriptor.idVendor;
		PDC->idProduct = udev->descriptor.idProduct;

		PDC->Demodulator.GPIO8Value[0] = 2;
		PDC->Demodulator.GPIO8Value[1] = 2;

		PDC->fc[0].AVerFlags = 0x00;
		PDC->fc[1].AVerFlags = 0x00;
		
		//init_MUTEX(&PDC->regLock);
		sema_init(&PDC->regLock, 1);
	}
	else {
        	PDC->UsbCtrlTimeOut = 5;
    	}//bBoot

#ifdef AFA_USB_DEVICE 	
	if (bBoot) {
		//************* Set USB Info *************//
		PDC->MaxPacketSize = 0x0200; //default USB2.0
		PDC->UsbMode = (PDC->MaxPacketSize == 0x200)?0x0200:0x0110;  
		deb_data("USB mode= 0x%x\n", PDC->UsbMode);

		PDC->TsPacketCount = (PDC->UsbMode == 0x200)?TS_PACKET_COUNT_HI:TS_PACKET_COUNT_FU;
		PDC->TsFrames = (PDC->UsbMode == 0x200)?TS_FRAMES_HI:TS_FRAMES_FU;
		PDC->TsFrameSize = TS_PACKET_SIZE*PDC->TsPacketCount;
		PDC->TsFrameSizeDw = PDC->TsFrameSize/4;
	}
	PDC->bEP12Error = false;
    	PDC->bEP45Error = false; 
    	PDC->ForceWrite = false;    
    	PDC->ulActiveFilter = 0;
#else
    	PDC->bSupportSuspend = false; 
#endif//AFA_USB_DEVICE
	
#ifdef AFA_USB_DEVICE
	if(bBoot)
    	{
		//patch for eeepc
        	error = DL_SetBusTuner (PDC, Bus_USB, Tuner_Afatech_AF9007);
        	PDC->UsbCtrlTimeOut = 5;
        
        	error = DL_SetBusTuner (PDC, Bus_USB, Tuner_Afatech_AF9007);
        	if (error)
        	{
            		deb_data("First DL_SetBusTuner fail : 0x%08x\n",error );
            		goto Exit;
        	}

        	error =DL_GetEEPROMConfig(PDC);
        	if (error)
        	{
            		deb_data("DL_GetEEPROMConfig fail : 0x%08x\n", error);
            		goto Exit;
        	}
	}//bBoot
	
	error = DL_SetBusTuner(PDC, Bus_USB, PDC->fc[0].tunerinfo.TunerId);
    	if (error)
    	{
        	deb_data("DL_SetBusTuner fail!\n");
        	goto Exit;
    	}

	if(PDC->Demodulator.chipNumber == 1 && PDC->Demodulator.booted) //warm-boot/(S1)
	{
		error = DL_TunerWakeup(PDC);
	}
	if(error) deb_data("DL_NIMReset or DL_NIMSuspend or DL_TunerWakeup fail!\n"); 

	error = DL_Initialize(PDC);
    	if (error) 
    	{
        	deb_data("DL_Initialize fail! 0x%08x\n", error);
        	goto Exit;
    	}
	
	if (PDC->bIrTblDownload) 
    	{
        	error = DL_IrTblDownload(PDC);
       	 	if (error) deb_data("DL_IrTblDownload fail");
    	}

    	//for (filterIdx=0; filterIdx< pdc->Demodulator.chipNumber; filterIdx++) 
    	//{  
		

        	if (bBoot)
        	{
			//Bool bLock;
            		error = DRV_ApCtrl(PDC, filterIdx, false);
            		if (error) deb_data("%d: DRV_ApCtrl Fail!\n", filterIdx);
        	} 

	deb_data("        %s success!! \n",__FUNCTION__);

Exit:
#endif //AFA_USB_DEVICE

	return (error);
}
//EXPORT_SYMBOL(Device_init);
