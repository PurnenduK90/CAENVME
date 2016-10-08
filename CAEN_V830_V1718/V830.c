/* ###########################################################################
* Program:		SCALAR CAEN V830 Data Acquisition Program
* Discriminator used:	V814
* OS:			Linux Ubuntu 12.04
* VME-USB Bridge:	CAEN V1718
* Prequities:		CAENVME Lib, CAEN USB Driver
* Auth:			Purnendu Kumar(JRF, University of Delhi).
* Date:			January 15, 2014.
* Acknowledgement:	Carlo Tintori(CAEN)
*  ########################################################################### */

#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdint.h>
#include <CAENVMElib.h>
#include <string.h>
#include "keyb.h"
#include "V830.h"

#define ulong  unsigned long
#define ushort unsigned short
#define uchar  unsigned char
#define MAX_DATE 12

int main(void)
{
    int ret, nw=0, pnt=0;
    uint32_t *buffer;
    uint16_t data16;
    
    int IsHeader=1;
    int nch, ch=0, wcnt=0, quit=0;
    char c;
    FILE *fout;
    char fname[100]="data.txt";         //User can change the data file name every time if want to save each data in seprate file.
    fout = fopen(fname, "w"); 
    buffer = malloc(BUFFER_SIZE);

/*****************************************************************************
               Setting Parameters(Change the parameters as desired)
                  Make again if you do any change in program
******************************************************************************/
	
/*---------------Discriminator-----------------------------------------------*/

	ulong CFD_OutputWidth = 255;	//Discriminator output width; 255 for 95ns, 0 for 5ns 
	ulong CFD_ChannelMask = 0x00; 	//No of channels Enabled; ch 0 1 2  only (Hex Value)
	
	ulong CFD_Threshold0 = 50;   	//Discriminator threshold (ch 0); mV
	ulong CFD_Threshold1 = 20;   	//Discriminator threshold (ch 1); mV
	ulong CFD_Threshold2 = 20;   	//Discriminator threshold (ch 2); mV
	ulong CFD_Threshold3 = 50;   	//Discriminator threshold (ch 3); mV
	ulong CFD_Threshold4 = 25;   	//Discriminator threshold (ch 4); mV
	ulong CFD_Threshold5 = 50;   	//Discriminator threshold (ch 5); mV
	ulong CFD_Threshold6 = 25;   	//Discriminator threshold (ch 6); mV
	ulong CFD_Threshold7 = 25;   	//Discriminator threshold (ch 7); mV
	ulong CFD_Threshold8 = 25;   	//Discriminator threshold (ch 8); mV
	ulong CFD_Threshold9 = 25;   	//Discriminator threshold (ch 9); mV
	ulong CFD_Threshold10 = 25;   	//Discriminator threshold (ch 10); mV
	ulong CFD_Threshold11 = 25;   	//Discriminator threshold (ch 11); mV
	ulong CFD_Threshold12 = 25;   	//Discriminator threshold (ch 12); mV
	ulong CFD_Threshold13 = 25;   	//Discriminator threshold (ch 13); mV
	ulong CFD_Threshold14 = 25;   	//Discriminator threshold (ch 14); mV
	ulong CFD_Threshold15 = 25;   	//Discriminator threshold (ch 15); mV
   
/*--------------------Scaler-----------------------------------------------------*/     
	
	uint32_t x=300;			//Dwell time of scaler, sec.
	uint32_t chenable=0x00000007;	//two channels enabled (Hex Value)
/*-------------------------------------------------------------------------------*/
x=x*2500000;

/*********************************************************************************/
/*                           open the Bus Adapter
*********************************************************************************/
    ret = CAENVME_Init(cvV1718, 0, 0, &handle);
    if (ret != cvSuccess) {
        printf("ERROR: can't open the BusAdapter\n");
        getch();
        return -1;
    }


//*************************************************************************
//                               CFD settings
// ************************************************************************
	
	if (VMEerror) {
		printf("Error during CFD programming: ");
//		printf(ErrorString);
		getch();
		goto QuitProgram;
	}
	

	// Set channel mask

	write_reg(0x004A, CFD_ChannelMask);

	// set output width
	write_reg(0x0040, CFD_OutputWidth);

	write_reg(0x0042, CFD_OutputWidth);

	// set CFD threshold
	write_reg(0, CFD_Threshold0);
	write_reg(2, CFD_Threshold1);
	write_reg(4, CFD_Threshold2);
	write_reg(6, CFD_Threshold3);
	write_reg(8, CFD_Threshold4);
	write_reg(10, CFD_Threshold5);
	write_reg(12, CFD_Threshold6);
	write_reg(14, CFD_Threshold7);
	write_reg(16, CFD_Threshold8);
	write_reg(18, CFD_Threshold9);
	write_reg(20, CFD_Threshold10);
	write_reg(22, CFD_Threshold11);
	write_reg(24, CFD_Threshold12);
	write_reg(26, CFD_Threshold13);
	write_reg(28, CFD_Threshold14);
	write_reg(30, CFD_Threshold15);

	printf("Discriminator programmed\n");

/*************************************************************************************************/
/*      SCALAR setting:read the firmware revision of the FPGA AMC (adc and memory controller)    */
/*************************************************************************************************/

    ret = ReadRegister16(V830_FW_REV, &data16);
    if (ret < 0) {
        printf("ERROR: can't read the V830\n");
        getch();
        return -1;
    }
    printf("V830 firmware revision: %02X\n", data16 & 0xFF);

    /* program the V830 */
    WriteRegister16(V830_SW_RESET, 0);  /* Reset the board */
    WriteRegister16(V830_CTRL_BITSET, 0x20 | USE_BERR << 4);  /* Enable header + berr (if used) */
    WriteRegister16(V830_INT_STATUSID, STATUS_ID);    /* Status ID (only for interrupts) */
    WriteRegister16(V830_INT_LEVEL, IRQ_LEVEL);  /* Interrupt req. level (only for interrupts) */
    WriteRegister16(V830_ALMOST_FULL, AF_LEVEL);  /* Almost Full level (only for interrupts) */
    WriteRegister16(V830_BLT_EVNUM, NUM_EVENTS_BLK);  /* Max. num of events for each Block Transfer*/
    WriteRegister32(V830_DWELL_TIME, x);  /* DWELL time = 1sec*/
    WriteRegister32(V830_CH_ENABLE, chenable);  /* channel enable mask */

    printf("Scaler programmed\n");

/***********************************************************************************************/
/*                                   start the acquisition                                     */
/***********************************************************************************************/
    printf("Press a key to start the acquisition\n");
    getch();

    WriteRegister16(V830_CTRL_BITSET, 2);  /* Enable periodic Triggers */

   while(!quit)  {
			c = 0;
		if (kbhit())
			c = getch();

		if(c == 'q') {
			quit = 1;
		}
	
	if (pnt == nw) {
        	ret = ReadEvents(buffer, &nw);
        	if (ret < 0)  /* readout error; quit the program */
            		break;
       		pnt=0;
		if (nw==0) {
			Sleep(100);
			continue;
		}
		//printf("nw=%d\n", nw);
	}

	if (IsHeader) {
		nch = ((buffer[pnt]>>18) & 0x3F); // num of words in the event
		printf("Event n.%d (%d channels enabled)\nPress 'q' to quit\n", wcnt/nch, nch);
		pnt++;
		IsHeader = 0;
		ch = 0;
		

	} else {
		if (chenable & (1<<ch)) {

			printf("CH%2d: %d\n", ch, buffer[pnt]);

        			if (ch==0)
        			fprintf(fout, "\n%d",buffer[pnt]);
        			else
        			fprintf(fout, "\t%d",buffer[pnt]);
			
			wcnt++;
			pnt++;

		}
		ch++;


		if (ch == 32){
			IsHeader = 1;
		system("clear");}
	}
        WriteRegister16(V830_CTRL_BITSET, 2);

    }
	fclose(fout);

    /* stop the acquisition */
    QuitProgram:
    WriteRegister16(V830_CTRL_BITCLEAR, 2);

    CAENVME_End(handle);
    free(buffer);
	
    return 0;

}    
