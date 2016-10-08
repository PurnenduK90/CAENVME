//---------------------------------------------------------------------------------------------------
// Design Name: V1190B Functions and register declaration
// Engineer: Purnendu Kumar
// Copyright (C) {2016} {Purnendu Kumar} http://engineeringxd.com
// Create Date: 02/26/2016 11:57:15 AM
//
// Target Devices: CAEN V1190B (Multihit TDC), A2818 as communication 
// Tool Versions: 1
// Description: This header file is written in c which containd declaration of registers and functions with its definition which will be used in main data acquisition program from TDC.
// Dependencies: GCC, CAENVMELIB, LINUX  
// 
// Revision: 1
// Additional Comments:
// 
//-----------------------------------------------------------------------------------------------------

// Define header file for CAEN V1190B registers and functions
#ifndef __V1190B_H
#define __V1190B_H
//-----------------------------------------------------------------------------------------------------

// include files
#include<stdio.h>
#include<stdlib.h>
#include<CAENVMElib.h>
#include<CAENVMEtypes.h>
#include<CAENVMEoslib.h>
#include<ctype.h>
#include<string.h>
#include<stdint.h>
#include<sys/time.h>
#include<math.h>
#include<unistd.h>
//#include <iostream>
//#include <fstream>
#include <stdlib.h>
//#include <cmath>
//-----------------------------------------------------------------------------------------------------

// Registers address declaration
#define V1190B_BA	0x22220000 	//V1190B base address
#define OUI	0x402C  		//Manufacturer Identifier
#define V1190B_FW_VER	0x1026 		//firmware revision
#define V1190B_RESET	0x1014 		//reset the board
#define V1190B_SWRESET	0x1016		//Software reset
#define INTLVL	0x100A 			//interrupt level
#define INTVEC	0x100c 			//interrupt vector (STATUS/ID)
#define ALFULLLVL	0x1022 		//almost full level register
#define CTRLREG	0x1000			//control register
#define MICROHS	0x1030			//micro handshake register
#define MICRO	0x102E			//micro register (to send opcode)

//--------------------------------------------------------------------------------------------------------

//Global Variables
    	CVBoardTypes bd = cvV2718;  //BdType->VME bard
    	short link_type = 0;        //link type
    	short bdnum = 0;            //board number in the link
    	int32_t handle;
	int VMEerror = 0;
	CAEN_BYTE activeIRQ;
//---------------------------------------------------------------------------------------------------------

//base exponent function
int power(int base, int exp)
{
long long value=1;
	while (exp!=0)
	{
		value*=base;  /* value = value*base; */
		--exp;
	}
	return value;
}
//----------------------------------------------------------------------------------------------------------

//VME Bus initialization and reset function
int init(){
    	if(CAENVME_Init(bd, link_type, bdnum, &handle) !=cvSuccess) {
		printf("Error while VME crate initilization... \nPlease check the bus driver and connections\n");
		return -1;}
	else{
	printf("VME crate successfully Initilized\n");
     	CAENVME_SystemReset(handle);
     	usleep(800*1000); // required by R/o module
     	printf("System reset done\n");
     	CAENVME_IRQDisable(handle, activeIRQ); 
	return 0;}// Disable those lines}
}
//----------------------------------------------------------------------------------------------------------

// Read a 16 bit register of the V1190
uint16_t read_reg16(uint16_t RegAddress, uint16_t *data)
{
	int ret;

	ret = CAENVME_ReadCycle(handle, V1190B_BA + RegAddress, data, cvA32_U_DATA, cvD16);
	if (ret != cvSuccess) {
		printf("Read Error at address %08X\n", V1190B_BA + RegAddress);
		getch();
		return -1;
	}
	return 0;
}
//----------------------------------------------------------------------------------------------------------

// Write a 16 bit register of the V1190
int write_reg16(uint16_t RegAddress, uint16_t data)
{
	int ret; 

	ret = CAENVME_WriteCycle(handle, V1190B_BA + RegAddress, &data, cvA32_U_DATA, cvD16);
	if (ret != cvSuccess) {
		printf("Write Error at address %08X\n", V1190B_BA + RegAddress);
		getch();
		return -1;
	}
	return 0;
}
//-----------------------------------------------------------------------------------------------------------


	
#endif
