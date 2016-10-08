//---------------------------------------------------------------------------------------------------
// Design Name: V1190B Parameters declaration
// Engineer: Purnendu Kumar
// Copyright (C) {2016} {Purnendu Kumar} http://engineeringxd.com
// Create Date: 02/26/2016 05:35:33 PM
//
// Target Devices: CAEN V1190B (Multihit TDC), A2818 as communication 
// Tool Versions: 1
// Description: This header file is written in c which contains declaration of Parameters for setting TDC.
// Dependencies: GCC, CAENVMELIB, LINUX  
// 
// Revision: 1
// Additional Comments:
// 
//-----------------------------------------------------------------------------------------------------

//Definition
#ifndef __V1190Bsettings_H
#define __V1190Bsettings_H
//-----------------------------------------------------------------------------------------------------

// Define parameters
#define STATUS	0x0001
#define ALL_FL	0x0006
#define CTRL_REG	0x0000	
#define TRIGGER_ENABLE	1	//0->disable,1->enable
#define WINDOW_WIDTH	4	// value*25ns
#define WINDOW_OFFSET	-3	//for post trigger +ve and pre trigger -ve  value*25ns
#define SEARCH_MARGIN	1 
#define REJECT_MARGIN	0	
#define TRIGGER_TIME_SUB	1	//0->disable,1->enable
#define EDGE_DETECT	1	//0->pair,1->trailing,2->leading,3->trailing and leading
#define EDGE_LSB	2	//0->800ps,1->200ps,2->100ps
#define PAIR_RES	0	//16 bit word bit [0:2]->leading edge resolution, bit[8:11]->pulse width resolution
#define DEAD_TIME	0	//0->5ns,1->10ns,2->30ns,3->100ns
#define MAX_HITS	9	//0->0,1->1,2->2,3->4,4->8,5->16,6->32,7->64,8->128,9->unlimited
#define FIFO_SIZE	3	//0->2 words,1->4 words,2->8 words,3->16 words,4->32 words,5->64 words,6->128 words,7->256 words
#define ENABLE_ALL_TDC_CHANNEL 0//1->all channels enabled
#define NOOFCHANNEL	2
#define CLOCK_SRC	3	//0->40MHz, 1->40MHz(PLL), 2->160MHz(PLL), 3->320MHZ(PLL)
#define TRGREFCHNL	33	//reference trigger channel
#define BUFFER_SIZE     (1024*1024)    // Size of the local buffer (in bytes)

#endif
