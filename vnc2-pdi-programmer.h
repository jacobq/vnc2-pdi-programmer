/*
** Filename: vnc2-pdi-programmer.h
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

#ifndef _vnc2-pdi-programmer_H_
#define _vnc2-pdi-programmer_H_

#include "vos.h"

/* FTDI:SHF Header Files */
#include "USB.h"
#include "USBHost.h"
#include "GPIO.h"
#include "Timers.h"
#include "FAT.h"
#include "msi.h"
#include "BOMS.h"
#include "stdio.h"
#include "errno.h"
#include "unistd.h"
#include "string.h"
/* FTDI:EHF */

/* FTDI:SDC Driver Constants */
#define VOS_DEV_USBHOST_1 0
#define VOS_DEV_USBHOST_2 1
#define VOS_DEV_GPIO_PORT_B 2
#define VOS_DEV_TIMER_1 3
#define VOS_DEV_FAT_FILE_SYSTEM_1 4
#define VOS_DEV_FAT_FILE_SYSTEM_2 5
#define VOS_DEV_BOMS_1 6
#define VOS_DEV_BOMS_2 7

#define VOS_NUMBER_DEVICES 8
/* FTDI:EDC */

/* FTDI:SXH Externs */
/* FTDI:EXH */

#define _NOP asm { "NOP" }

#define PDI_CLK (0x01 << 0)
#define PDI_TX (0x01 << 1)
#define PDI_RX (0x01 << 2)

//#define  do { portb_data ^= ; vos_gpio_write_port(GPIO_PORT_B, portb_data) } while(0)

// Reader thread needs to know:
//  1. When it can/should read more data from the file into the buffer
//  2. When to start
//  3. How to signal that it's done
//
// Perhaps we should use a circular buffer, but the trouble there
// is that fread expects a linear one, so it doesn't know to wrap
// and thus we'll only be able to read 

#define BUFFER_SIZE 256
char shared_buffer[BUFFER_SIZE];
uint16 shared_buffer_length;
enum {
    FRS_ERROR = -1,
	FRS_NOT_READY,
	FRS_READING,
	FRS_WAITING,
	FRS_FINISHED
} file_reader_state;
uint8 data_consumed;
	
#endif /* _vnc2-pdi-programmer_H_ */
