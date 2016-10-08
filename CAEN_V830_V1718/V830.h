/* ###########################################################################
* Program:		SCALAR CAEN V830 Data Acquisition Program header file.
* OS:			Linux Ubuntu 12.04
* VME-USB Bridge:	CAEN V1718
* Prequities:		CAENVME Lib, CAEN USB Driver
* Auth:			Purnendu Kumar(JRF, University of Delhi).
* Date:			January 15, 2014.
* Acknowledgement:	Carlo Tintori(CAEN)
*  ########################################################################### */

#ifndef __V830_H
#define __V830_H

#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdint.h>
#include <CAENVMElib.h>

/* ###########################################################################
*  Parameters for the readout
*  ########################################################################### */

#define MAX_BLT_SIZE    (1024*512)    // Max size of the Block Transfer (in bytes)
#define BUFFER_SIZE     (1024*512)    // Size of the local buffer (in bytes)
#define NUM_EVENTS_BLK  0            // Number of events per block

#define USE_BERR        1
#define USE_POLLING     0
#define USE_INTERRUPTS  0
#define STATUS_ID       0xAA
#define AF_LEVEL        1024
#define IRQ_LEVEL       1


/* ###########################################################################
*  V830 Register Map (this is not complete) refer V830 Manual
*  ########################################################################### */
#define V830_FW_REV            0x1132
#define V830_STATUS_REG        0x110E
#define V830_CTRL_REG          0x1108
#define V830_BLT_EVNUM         0x1130
#define V830_INT_STATUSID      0x1114
#define V830_INT_LEVEL         0x1112
#define V830_ALMOST_FULL       0x112C
#define V830_SW_RESET          0x1120
#define V830_DWELL_TIME        0x1104
#define V830_CTRL_BITSET       0x110A
#define V830_CTRL_BITCLEAR     0x110C
#define V830_CH_ENABLE         0x1100


/* ###########################################################################
*  Global Variables
*  ########################################################################### */

int32_t handle;
uint32_t BaseAddress = 0x22220000;
ulong base_addr = 0xEE000000;
int VMEerror = 0;
char ErrorString[100];

/* ---------------------------------------------------------------------------
   get time in milliseconds
------------------------------------------------------------------------- */
long get_time()
{
long time_ms;

    struct timeval t1;
    struct timezone tz;

    gettimeofday(&t1, &tz);
    time_ms = (t1.tv_sec) * 1000 + t1.tv_usec / 1000;

    return time_ms;
}

/* ---------------------------------------------------------------------------
   Read one 16bit register of the V830
 ------------------------------------------------------------------------- */
int ReadRegister16(uint16_t RegAddress, uint16_t *data)
{
    int ret;

    ret = CAENVME_ReadCycle(handle, BaseAddress + RegAddress, data, cvA32_U_DATA, cvD16);
    if (ret != cvSuccess) {
        printf("Read Error at address %08X\n", BaseAddress + RegAddress);
        getch();
        return -1;
    }
    return 0;
}

/* ---------------------------------------------------------------------------
   Read one 32bit register of the V830
 ------------------------------------------------------------------------- */
int ReadRegister32(uint16_t RegAddress, uint32_t *data)
{
    int ret;

    ret = CAENVME_ReadCycle(handle, BaseAddress + RegAddress, data, cvA32_U_DATA, cvD32);
    if (ret != cvSuccess) {
        printf("Read Error at address %08X\n", BaseAddress + RegAddress);
        getch();
        return -1;
    }
    return 0;
}

/* ---------------------------------------------------------------------------
   Write one 16bit register of the V830
 ------------------------------------------------------------------------- */
int WriteRegister16(uint16_t RegAddress, uint16_t data)
{
    int ret; 

    ret = CAENVME_WriteCycle(handle, BaseAddress + RegAddress, &data, cvA32_U_DATA, cvD16);
    if (ret != cvSuccess) {
        printf("Write Error at address %08X\n", BaseAddress + RegAddress);
        getch();
        return -1;
    }
    return 0;
}

/* ---------------------------------------------------------------------------
   Write one 32bit register of the V830
 ------------------------------------------------------------------------- */
int WriteRegister32(uint16_t RegAddress, uint32_t data)
{
    int ret; 

    ret = CAENVME_WriteCycle(handle, BaseAddress + RegAddress, &data, cvA32_U_DATA, cvD32);
    if (ret != cvSuccess) {

        printf("Write Error at address %08X\n", BaseAddress + RegAddress);
        getch();
        return -1;
    }
    return 0;
}

/* ---------------------------------------------------------------------------
   Read one block of data
------------------------------------------------------------------------- */
int ReadBlock(uint32_t *buffer, int MaxSize, int *nw)
{
    int ret = 0, nb; 

    *nw = 0;
	ret = CAENVME_FIFOBLTReadCycle(handle, BaseAddress , (unsigned char *)buffer, MAX_BLT_SIZE, cvA32_U_BLT, cvD32, &nb);
	
	if ((ret != cvSuccess) && (ret != cvBusError)) {
	    printf("Block Read Error: nb=%d, ret=%d\n", nb, ret);
	    getch();
	    return -1;
	}

    *nw = nb/4;
    return 0;
}


/* ---------------------------------------------------------------------------
   Read one or more events from the V830
 ------------------------------------------------------------------------- */
int ReadEvents(uint32_t *buffer, int *nw)
{
    int i, ret = 0; 
    uint16_t StatusId, StatusReg;
    uint32_t header, EventSize;

    *nw = 0;

    /* Wait for data ready */
    if (USE_INTERRUPTS) {
        CAENVME_IRQEnable(handle, 1 << IRQ_LEVEL);
        ret = CAENVME_IRQWait(handle, 1 << IRQ_LEVEL, 500);
        CAENVME_IRQDisable(handle, 1 << IRQ_LEVEL);
        if (ret != cvSuccess)  /* Timeout => no data */
            return 0;  
        CAENVME_IACKCycle(handle, IRQ_LEVEL, &StatusId, cvD32);
        if (StatusId != STATUS_ID) /* not an interrupt from my board */
            return 0;

    } else if (USE_POLLING) {
        for(i=0; i<50; i++) { 
            ReadRegister16(V830_STATUS_REG, &StatusReg);
            if (StatusReg & 0x1)
                break;
            Sleep(10);
        }
        if (i == 50)  /* Timeout => no data */
            return 0;
    } 

    if (USE_BERR) {
        ret = ReadBlock(buffer, BUFFER_SIZE/4, nw);
    } else {
        /* read the event header using a single read cycle */
	ReadRegister32(0, &header);
        EventSize = (header >> 18) & 0x3F;
        if (EventSize > (BUFFER_SIZE/4)) {
            printf("Warning: local buffer size too small to contain the event(s)\n");
            getch();
            return -1;
        }
        /* read the rest of the event with an exact size Block Transfer */
        ret = ReadBlock(buffer, EventSize, nw);
    }
    
    return ret;
}
/*******************************************************************************/
 /*                              READ_REG                                      */
/*******************************************************************************/
ushort read_reg(ushort reg_addr)
{
	ushort data=0;
	CVErrorCodes ret;
	ret = CAENVME_ReadCycle(handle, base_addr + reg_addr, &data, cvA32_U_DATA, cvD16);
	if(ret != cvSuccess) {
		sprintf(ErrorString, "Cannot read at address %08X\n", (uint32_t)(base_addr + reg_addr));
		VMEerror = 1;
    }
	return(data);
}

/*******************************************************************************/
/*                                WRITE_REG                                    */
/*******************************************************************************/
void write_reg(ushort reg_addr, ushort data)
{
	CVErrorCodes ret;
	ret = CAENVME_WriteCycle(handle, base_addr + reg_addr, &data, cvA32_U_DATA, cvD16);
	if(ret != cvSuccess) {
		sprintf(ErrorString, "Cannot write at address %08X\n", (uint32_t)(base_addr + reg_addr));
		VMEerror = 1;
    }
}
      
#endif
