//--------------------------------------------------------------------------------------------------------
// Design Name: V1190B Microcontroller programming Functions and opcode declaration
// Engineer: Purnendu Kumar
// Copyright (C) {2016} {Purnendu Kumar} http://engineeringxd.com
// Create Date: 02/26/2016 04:10:41 PM
//
// Target Devices: CAEN V1190B (Multihit TDC), A2818 as communication 
// Tool Versions: 1
// Description: This header file is written in c which containd declaration of opcodes and functions to program the inbuit Micro-controller.
// Dependencies: GCC, CAENVMELIB, LINUX  
// 
// Revision: 1
// Additional Comments:
// 
//----------------------------------------------------------------------------------------------------------

// Define header file for CAEN V1190Bmicro registers and functions
#ifndef __V1190Bmicro_H
#define __V1190Bmicro_H
//----------------------------------------------------------------------------------------------------------

//Include files
#include<stdio.h>
#include<CAENVMElib.h>
#include<CAENVMEtypes.h>
#include<CAENVMEoslib.h>
#include "V1190Bsettings.h"
#include "V1190B.h"
//-----------------------------------------------------------------------------------------------------------

//opcode definition
#define OC_TRIG_MATCH 	0x0000
#define OC_CONT_STOR	0x0100
#define OC_W_WID	0x1000
#define OC_W_OFF	0x1100
#define OC_EXT_MARG	0x1200
#define OC_REJ_MARG	0x1300
#define OC_EN_TRIG_TIME_SUB	0x1400
#define OC_DIS_TRIG_TIME_SUB	0x1500
#define OC_RD_TRG_CONFIG	0x1600		//to read trigger configuration
#define OC_EDGE_DET	0x2200
#define OC_EDGE_LSB	0x2400
#define OC_PW_RES	0x2500
#define OC_DEAD_TIME	0x2800
#define OC_EN_HEAD_TRAIL	0x3000
#define OC_MAX_HITS	0x3300
#define OC_EN_TDC_ERR_STATUS	0x3900
#define OC_FIFO_SIZE	0x3B00
#define OC_EN_CH	0x4000
#define OC_EN_ALL_CH	0x4200
#define OC_DIS_CH	0x4300
#define OC_DLL_CLOCK	0xC800
//-----------------------------------------------------------------------------------------------------------

//Microcontroller handshake
void micro_hs(){	
	int d=0;
	while(d!=0x01)
	{
		 CAENVME_ReadCycle(handle,V1190B_BA+MICROHS,&d,cvA32_U_DATA,cvD16);
		 d=d&0x0001;//waiting for handshake in microcontroller and VME
	}
}
//------------------------------------------------------------------------------------------------------------

//Setting TDC trigger parameters via writing opcode to microcontroller
void micro_trigger(){
if (TRIGGER_ENABLE) {// trigger mode operation
	micro_hs();
	write_reg16(MICRO, OC_TRIG_MATCH); //setting trigger matching mode via opcode
	printf("Trigger Enabled for V1190B\n");
	
	micro_hs();
	write_reg16(MICRO, OC_W_WID); //setting trigger window
	micro_hs();
	write_reg16(MICRO, WINDOW_WIDTH); //setting trigger window width
	printf("Assigned window width = %dns\n",WINDOW_WIDTH*25);

	micro_hs();
	write_reg16(MICRO, OC_W_OFF); //setting window offset
	micro_hs();
	write_reg16(MICRO, WINDOW_OFFSET); //setting window offset value
	printf("Assigned window offset = %dns\n",WINDOW_OFFSET*25);	

	micro_hs();
	write_reg16(MICRO, OC_EXT_MARG); //setting window search margin
	micro_hs();
	write_reg16(MICRO, SEARCH_MARGIN); //setting window search margin value
	printf("Assigned search margin = %dns\n",SEARCH_MARGIN*25);

	micro_hs();
	write_reg16(MICRO, OC_REJ_MARG); //setting reject margin
	micro_hs();
	write_reg16(MICRO, REJECT_MARGIN); //setting reject margin value
	printf("Assigned reject margin = %dns\n",REJECT_MARGIN*25);

	if(TRIGGER_TIME_SUB){
		micro_hs();
		write_reg16(MICRO, OC_EN_TRIG_TIME_SUB); //setting trigger time subtraction
		printf("Trigger subtraction enabled\n");
	}
	else{
		micro_hs();
		write_reg16(MICRO, OC_DIS_TRIG_TIME_SUB); //disabling trigger time subtraction
		printf("Trigger subtraction disabled\n");
	}

	printf("TRIGGER: TDC Set to run in trigger mode.\n\n");	}

else{ //contininuous mode of operation
	micro_hs();
	write_reg16(MICRO, OC_CONT_STOR);//continuous mode enabled
	printf("TRIGGER: TDC Set to run in continious mode.\n\n");	}
}
//--------------------------------------------------------------------------------------------------------------

//Configering TDC via writing opcode to microcontroller
void micro_tdc_confg(){
	micro_hs();
	write_reg16(MICRO, OC_EN_TDC_ERR_STATUS);//accessing error enable resistor
 
	micro_hs();
	write_reg16(MICRO, OC_EDGE_DET);
	micro_hs();
	write_reg16(MICRO, EDGE_DETECT);//setting detection mode
	if (EDGE_DETECT==0) printf("Pair detection mode enabled\n");
	else if (EDGE_DETECT==1) printf("Only trailing mode enabled\n");
	else if (EDGE_DETECT==2) printf("Only leading mode enabled\n");
	else if (EDGE_DETECT==3) printf("Trailing and leading mode enabled\n");
	else printf("Invalid detection mode setting\n");

	micro_hs();
	write_reg16(MICRO, OC_EDGE_LSB);
	micro_hs();
	write_reg16(MICRO, EDGE_LSB);//setting resolution for leading and tailing edge
	if (EDGE_LSB==0) printf("Resolution= 800ps\n");
	else if (EDGE_LSB==1) printf("Resolution= 200ps\n");
	else if (EDGE_LSB==2) printf("Resolution= 100ps\n");
	else printf("Invalid EDGE_LSB setting\n");

	micro_hs();
	write_reg16(MICRO, OC_PW_RES);
	micro_hs();
	write_reg16(MICRO, PAIR_RES);//set Lead time and pulse width resolution when pair mode is enabled
	printf("Pair Resolution= %04x , see description (user manual rev 13 page 55)\n",PAIR_RES & 0xFFFF);

	micro_hs();
	write_reg16(MICRO, OC_DEAD_TIME);
	micro_hs();
	write_reg16(MICRO, DEAD_TIME);//setting dead time
	if (DEAD_TIME==0) printf("Dead time= ~5ns\n");
	else if (DEAD_TIME==1) printf("Resolution= ~10ns\n");
	else if (DEAD_TIME==2) printf("Resolution= ~30ns\n");
	else if (DEAD_TIME==3) printf("Resolution= ~100ns\n");
	else printf("Invalid Dead time setting\n");

	micro_hs();
	write_reg16(MICRO, OC_MAX_HITS);
	micro_hs();
	write_reg16(MICRO, MAX_HITS);//setting number of maximum hits per event
	if (MAX_HITS==0) printf("Maximum 0 hits exceptable\n");
	else if (MAX_HITS==0) printf("Maximum 1 hits exceptable\n");
	else if ((MAX_HITS>1)&(MAX_HITS<=8)) printf("Maximum %d hits exceptable\n",power(2,MAX_HITS-1));
	else if (MAX_HITS==9) printf("Maximum hits is set to infinity (limited by buffer)\n");
	else printf("Invalid max hits setting\n");

	micro_hs();
	write_reg16(MICRO, OC_FIFO_SIZE);
	micro_hs();
	write_reg16(MICRO, FIFO_SIZE);//setting size of FIFO buffer
	printf("FIFO buffer size =%d WORDS\n",power(2,(FIFO_SIZE+1)));

	micro_hs();
	write_reg16(MICRO, OC_DIS_CH);	//all channels disabled

	if(ENABLE_ALL_TDC_CHANNEL){
		micro_hs();
		write_reg16(MICRO, OC_EN_ALL_CH);	//all channels enabled
		printf("All channels enabled");
	}
	else{
		int chn;
		for(chn=0;chn<NOOFCHANNEL;chn++){	//0 to NOOFCHANNEL enabled 
			micro_hs();
			write_reg16(MICRO, OC_EN_CH+chn);
			}
		printf("%d channels enabled.\n",NOOFCHANNEL);
	}

	micro_hs();
	write_reg16(MICRO, OC_EN_HEAD_TRAIL);	//Enable TDC Header and trailer

	micro_hs();
	write_reg16(MICRO, OC_DLL_CLOCK);
	micro_hs();
	write_reg16(MICRO, CLOCK_SRC);//setting clock source
	if (CLOCK_SRC==0) printf("40MHz Direct Clock\n");
	else if (CLOCK_SRC==1) printf("40MHz Clock from PLL\n");
	else if (CLOCK_SRC==2) printf("160MHz Clock from PLL\n");
	else if (CLOCK_SRC==3) printf("320MHz Clock from PLL\n");
	else printf("Invalid Clock source setting\n");

	printf("CONFIGURATION: TDC Configuration complete\n\n");
}

#endif
