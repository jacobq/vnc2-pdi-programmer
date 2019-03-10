/*
** Filename: vnc2-pdi-programmer.c
**
** Automatically created by Application Wizard 2.0.2
** 
** Part of solution vnc2-pdi-programmer in project vnc2-pdi-programmer
**
** Comments: 
**
** Important: Sections between markers "FTDI:S*" and "FTDI:E*" will be overwritten by
** the Application Wizard
*/

#include "vnc2-pdi-programmer.h"

#define _NOP asm { "NOP" }

/* FTDI:STP Thread Prototypes */
vos_tcb_t *tcbFIRMWARE;
vos_tcb_t *tcbTHREADFILEREADER;

void firmware();
void threadFileReader();
/* FTDI:ETP */

/* FTDI:SDH Driver Handles */
VOS_HANDLE hUSBHOST_1; // USB Host Port 1
VOS_HANDLE hUSBHOST_2; // USB Host Port 2
VOS_HANDLE hGPIO_PORT_B; // GPIO Port B Driver
VOS_HANDLE hTIMER_1; // Timer 1 Driver
VOS_HANDLE hFAT_FILE_SYSTEM_1; // FAT File System for FAT32 and FAT16
VOS_HANDLE hFAT_FILE_SYSTEM_2; // FAT File System for FAT32 and FAT16
VOS_HANDLE hBOMS_1; // Bulk Only Mass Storage for USB disks
VOS_HANDLE hBOMS_2; // Bulk Only Mass Storage for USB disks
/* FTDI:EDH */

/* Declaration for IOMUx setup function */
void iomux_setup(void);

/* Main code - entry point to firmware */
void main(void)
{
	/* FTDI:SDD Driver Declarations */
	// GPIO Port B configuration context
	gpio_context_t gpioContextB;
	// Timer 1 configuration context
	tmr_context_t tmrContext1;
	// USB Host configuration context
	usbhost_context_t usbhostContext;
	/* FTDI:EDD */

	/* FTDI:SKI Kernel Initialisation */
	vos_init(50, VOS_TICK_INTERVAL, VOS_NUMBER_DEVICES);
	vos_set_clock_frequency(VOS_48MHZ_CLOCK_FREQUENCY);
	vos_set_idle_thread_tcb_size(512);
	/* FTDI:EKI */

	iomux_setup();

	/* FTDI:SDI Driver Initialisation */
	// Initialise GPIO B
	gpioContextB.port_identifier = GPIO_PORT_B;
	gpio_init(VOS_DEV_GPIO_PORT_B,&gpioContextB);
	
	// Initialise Timer 1
	//tmrContext1.timer_identifier = 1;
	//tmr_init(VOS_DEV_TIMER_1,&tmrContext1);
	
	// Initialise FAT File System Driver
	fatdrv_init(VOS_DEV_FAT_FILE_SYSTEM_1);
	
	// Initialise FAT File System Driver
	//fatdrv_init(VOS_DEV_FAT_FILE_SYSTEM_2);
	
	// Initialise BOMS Device Driver
	boms_init(VOS_DEV_BOMS_1);
	
	// Initialise BOMS Device Driver
	//boms_init(VOS_DEV_BOMS_2);
	
	
	
	// Initialise USB Host
	usbhostContext.if_count = 8;
	usbhostContext.ep_count = 16;
	usbhostContext.xfer_count = 2;
	usbhostContext.iso_xfer_count = 2;
	//usbhost_init(VOS_DEV_USBHOST_1, VOS_DEV_USBHOST_2, &usbhostContext);
	usbhost_init(VOS_DEV_USBHOST_1, -1, &usbhostContext);
	/* FTDI:EDI */

	/* FTDI:SCT Thread Creation */
	tcbFIRMWARE = vos_create_thread_ex(20, 4096, firmware, "Application", 0);
	tcbTHREADFILEREADER = vos_create_thread_ex(24, 1024, threadFileReader, "FileReader", 0);
	/* FTDI:ECT */

	vos_start_scheduler();

main_loop:
	_NOP
	_NOP
	_NOP
	goto main_loop;
}

/* FTDI:SSP Support Functions */

unsigned char usbhost_connect_state(VOS_HANDLE hUSB)
{
	unsigned char connectstate = PORT_STATE_DISCONNECTED;
	usbhost_ioctl_cb_t hc_iocb;

	if (hUSB)
	{
		hc_iocb.ioctl_code = VOS_IOCTL_USBHOST_GET_CONNECT_STATE;
		hc_iocb.get        = &connectstate;
		vos_dev_ioctl(hUSB, &hc_iocb);

    // repeat if connected to see if we move to enumerated
		if (connectstate == PORT_STATE_CONNECTED)
		{
			vos_dev_ioctl(hUSB, &hc_iocb);
		}
	}
	return connectstate;
}


VOS_HANDLE fat_attach(VOS_HANDLE hMSI, unsigned char devFAT)
{
	fat_ioctl_cb_t           fat_ioctl;
	fatdrv_ioctl_cb_attach_t fat_att;
	VOS_HANDLE hFAT;

	// currently the MSI (BOMS or other) must be open
        // open the FAT driver
	hFAT = vos_dev_open(devFAT);

        // attach the FAT driver to the MSI driver
	fat_ioctl.ioctl_code = FAT_IOCTL_FS_ATTACH;
	fat_ioctl.set = &fat_att;
	fat_att.msi_handle = hMSI;
	fat_att.partition = 0;

	if (vos_dev_ioctl(hFAT, &fat_ioctl) != FAT_OK)
	{
        // unable to open the FAT driver
		vos_dev_close(hFAT);
		hFAT = NULL;
	}

	return hFAT;
}

void fat_detach(VOS_HANDLE hFAT)
{
	fat_ioctl_cb_t           fat_ioctl;

	if (hFAT)
	{
		fat_ioctl.ioctl_code = FAT_IOCTL_FS_DETACH;
		fat_ioctl.set = NULL;
		fat_ioctl.get = NULL;

		vos_dev_ioctl(hFAT, &fat_ioctl);
		vos_dev_close(hFAT);
	}
}


VOS_HANDLE boms_attach(VOS_HANDLE hUSB, unsigned char devBOMS)
{
	usbhost_device_handle_ex ifDisk = 0;
	usbhost_ioctl_cb_t hc_iocb;
	usbhost_ioctl_cb_class_t hc_iocb_class;
	msi_ioctl_cb_t boms_iocb;
	boms_ioctl_cb_attach_t boms_att;
	VOS_HANDLE hBOMS;

	// find BOMS class device
	hc_iocb_class.dev_class = USB_CLASS_MASS_STORAGE;
	hc_iocb_class.dev_subclass = USB_SUBCLASS_MASS_STORAGE_SCSI;
	hc_iocb_class.dev_protocol = USB_PROTOCOL_MASS_STORAGE_BOMS;

	// user ioctl to find first hub device
	hc_iocb.ioctl_code = VOS_IOCTL_USBHOST_DEVICE_FIND_HANDLE_BY_CLASS;
	hc_iocb.handle.dif = NULL;
	hc_iocb.set = &hc_iocb_class;
	hc_iocb.get = &ifDisk;

	if (vos_dev_ioctl(hUSB, &hc_iocb) != USBHOST_OK)
	{
		return NULL;
	}

	// now we have a device, intialise a BOMS driver with it
	hBOMS = vos_dev_open(devBOMS);

	// perform attach
	boms_att.hc_handle = hUSB;
	boms_att.ifDev = ifDisk;

	boms_iocb.ioctl_code = MSI_IOCTL_BOMS_ATTACH;
	boms_iocb.set = &boms_att;
	boms_iocb.get = NULL;

	if (vos_dev_ioctl(hBOMS, &boms_iocb) != MSI_OK)
	{
		vos_dev_close(hBOMS);
		hBOMS = NULL;
	}

	return hBOMS;
}

void boms_detach(VOS_HANDLE hBOMS)
{
	msi_ioctl_cb_t boms_iocb;

	if (hBOMS)
	{
		boms_iocb.ioctl_code = MSI_IOCTL_BOMS_DETACH;
		boms_iocb.set = NULL;
		boms_iocb.get = NULL;

		vos_dev_ioctl(hBOMS, &boms_iocb);
		vos_dev_close(hBOMS);
	}
}

/* FTDI:ESP */

void open_drivers(void)
{
        /* Code for opening and closing drivers - move to required places in Application Threads */
        /* FTDI:SDA Driver Open */
        hUSBHOST_1 = vos_dev_open(VOS_DEV_USBHOST_1);
        //hUSBHOST_2 = vos_dev_open(VOS_DEV_USBHOST_2);
        hGPIO_PORT_B = vos_dev_open(VOS_DEV_GPIO_PORT_B);
        //hTIMER_1 = vos_dev_open(VOS_DEV_TIMER_1);
        /* FTDI:EDA */
}

void attach_drivers(void)
{
        /* FTDI:SUA Layered Driver Attach Function Calls */
        hBOMS_1 = boms_attach(hUSBHOST_1, VOS_DEV_BOMS_1);
        //hBOMS_2 = boms_attach(hUSBHOST_2, VOS_DEV_BOMS_2);
        hFAT_FILE_SYSTEM_1 = fat_attach(hBOMS_1, VOS_DEV_FAT_FILE_SYSTEM_1);
        //hFAT_FILE_SYSTEM_2 = fat_attach(hBOMS_2, VOS_DEV_FAT_FILE_SYSTEM_2);
        // TODO attach stdio to file system and stdio interface
        //fsAttach(hFAT_FILE_SYSTEM); // VOS_HANDLE for file system (typically FAT)
        //stdioAttach(hUART); // VOS_HANDLE for stdio interface (typically UART)
        // TODO attach stdio to file system and stdio interface
        //fsAttach(hFAT_FILE_SYSTEM); // VOS_HANDLE for file system (typically FAT)
        //stdioAttach(hUART); // VOS_HANDLE for stdio interface (typically UART)
        /* FTDI:EUA */
}

void close_drivers(void)
{
        /* FTDI:SDB Driver Close */
        vos_dev_close(hUSBHOST_1);
        //vos_dev_close(hUSBHOST_2);
        vos_dev_close(hGPIO_PORT_B);
        //vos_dev_close(hTIMER_1);
        /* FTDI:EDB */
}

/* Application Threads */

void firmware()
{

	#define PDI_CLK (0x01 << 0)
	#define PDI_TX (0x01 << 1)
	#define PDI_RX (0x01 << 2)

	//#define  do { portb_data ^= ; vos_gpio_write_port(GPIO_PORT_B, portb_data) } while(0)
	
	uint8 portb_data = 0x00;
	char buffer[256];
	unsigned int i, j;
	char c;
	unsigned int size = 0;
	FILE *file;


	open_drivers();
	//attach_drivers();
	vos_gpio_set_port_mode(GPIO_PORT_B, PDI_CLK | PDI_TX);

	while (1)
	{
		// wait for enumeration to complete
		vos_delay_msecs(250);

		if (usbhost_connect_state(hUSBHOST_1) == PORT_STATE_ENUMERATED)
		{
			hBOMS_1 = boms_attach(hUSBHOST_1, VOS_DEV_BOMS_1);

			if (hBOMS_1 == NULL)
			{
				// FIXME: handle fault condition
				break;
			}

			hFAT_FILE_SYSTEM_1 = fat_attach(hBOMS_1, VOS_DEV_FAT_FILE_SYSTEM_1);

			if (hFAT_FILE_SYSTEM_1 == NULL)
			{
				// FIXME: handle fault condition
				break;
			}

			// lastly attach the stdio file system to the FAT file system
			fsAttach(hFAT_FILE_SYSTEM_1);

			// now call the stdio runtime functions
			file = fopen("pdi.bin", "r");

			if (file == NULL)
			{
				// FIXME: handle fault condition
				break;
			}

			do
			{
				// (Purposely only load 4 at a time so we can watch multiple xfers on the logic analyzer before we run out of capture points)
				size = fread(buffer, 4, 1, file);
				for (i = 0; i < size; i++)
				{
					c = buffer[i];
					for (j = 0; j < 8; j++)
					{
						if ((c << j) & 0x80)
							portb_data |= PDI_TX;
						else
							portb_data &= ~PDI_TX;
							
						vos_gpio_write_port(GPIO_PORT_B, portb_data);
						portb_data ^= PDI_CLK;
						vos_gpio_write_port(GPIO_PORT_B, portb_data);
						portb_data ^= PDI_CLK;
						vos_gpio_write_port(GPIO_PORT_B, portb_data);
					}
				}
			} while(size > 0);
		
/*
			if (fwrite(tx_buf, strlen(tx_buf), sizeof(char), file) == -1)
			{
				leds = led3;
				vos_gpio_write_port(GPIO_PORT_A, leds);
				vos_delay_msecs(1000);
			}
*/
			if (fclose(file) == -1)
			{
				// FIXME: handle fault condition
			}

			fat_detach(hFAT_FILE_SYSTEM_1);
			boms_detach(hBOMS_1);

			vos_delay_msecs(5000);
		}
	}	
}

void threadFileReader()
{
	/* Thread code to be added here */
	while (1)
	{
		_NOP
		vos_delay_msecs(1000);
	}

}

