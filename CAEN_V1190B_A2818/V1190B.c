//---------------------------------------------------------------------------------------------------
// Design Name: V1190B Data acquisition program
// Engineer: Purnendu Kumar
// Copyright (C) {2016} {Purnendu Kumar} http://engineeringxd.com
// Create Date: 02/26/2016 05:57:11 PM
//
// Target Devices: CAEN V1190B (Multihit TDC), A2818 as communication 
// Tool Versions: 1
// Description: This header file is written in c which contains main program for data acquisition from TDC 1190B.
// Dependencies: GCC, CAENVMELIB, LINUX  
// 
// Revision: 1
// Additional Comments:
// 
//-----------------------------------------------------------------------------------------------------

//include files
#include<stdio.h>
#include<stdlib.h>
#include<CAENVMElib.h>
#include<CAENVMEtypes.h>
#include<CAENVMEoslib.h>
#include<ctype.h>
#include<string.h>
#include<stdint.h>
#include<sys/time.h>
#include<sys/stat.h>
#include<time.h>
#include<math.h>
#include<unistd.h>
#include <stdlib.h>
#include "keyb.h"
#include "V1190B.h"
#include "V1190Bmicro.h"
#include "V1190Bsettings.h"
//--------------------------------------------------------------------------------------------------

int main(int argc,char* argv[])
{
//Variables
	int TDC_Ch, TDC_EVENT_COUNT=0,quit=0;
	unsigned int irqVector,j,k,i,temp1;
	uint16_t data,ret;
	//unsigned TDC_Word;
	unsigned long Current_Data, TDC_Data[NOOFCHANNEL], TDC_OUT[4096][NOOFCHANNEL], temp, TDC_Word;
	enum CVErrorCodes error;
	char c, fname[100], out[100];
	FILE *fout;
	struct tm *tmp;
	time_t t;
	CAEN_BYTE activeIRQ;
//---------------------------------------------------------------------------------------------------

//Creating file with date and time as its name	
t = time(NULL);
tmp = localtime(&t);
strftime(out, sizeof(out), "%F_%T", tmp);
mkdir("DATA", 0777);
snprintf(fname, sizeof(fname), "DATA/%s.txt", out);
//---------------------------------------------------------------------------------------------------

//initializing output storage and other values
Current_Data=0;
temp=0;
TDC_Word=0;
for (k=0;k<NOOFCHANNEL;k++){
	TDC_Data[k]=0;
	}
for (i=0;i<4096;i++){
	for (k=0;k<NOOFCHANNEL;k++){
		TDC_OUT[i][k]=0;
	}
}
//---------------------------------------------------------------------------------------------------

//initialize
init();
//---------------------------------------------------------------------------------------------------

//manufacturer identifier data read test (ROM D16)
ret = read_reg16(OUI, &data);
   	if (ret < 0)	 {
        	printf("ERROR: can't read OUI\n");
        	getch();
        	return -1;
	}
	else
     	{
         if(data!=0xE6) //checking if output is valid
             printf("ERROR: OUI Error");
         else
             printf("module detected, OUI:%02x\n",data & 0xFF);
     	}
//----------------------------------------------------------------------------------------------------

//firmware revision read test (ROM D16)
ret = read_reg16(V1190B_FW_VER, &data);
   	if (ret < 0)	 {
        	printf("ERROR: can't read the V1190B\n");
        	getch();
        	return -1;
  	 		 }
    	printf("V1190B firmware revision: %02X\n\n", data & 0xFF);
//----------------------------------------------------------------------------------------------------

//program the TDC for data acquisition
write_reg16(V1190B_RESET, 0); //reset the module
sleep(1);

write_reg16(V1190B_SWRESET, 0); //reset the software
sleep(1);

write_reg16(INTLVL, cvIRQ1);//setting interrupt level 1 (CAENVMEtypes.h)
sleep(1);

write_reg16(INTVEC, STATUS);  //setting interrupt vector (STATUS/ID) 
sleep(1);

write_reg16(ALFULLLVL, ALL_FL); //setting almost full level
sleep(1);

write_reg16(CTRLREG, CTRL_REG); //setting control register
sleep(1);

micro_trigger(); // writing proper opcodes for TDC trigger setting
sleep(1);

micro_tdc_confg(); //Configuring TDC for operation
sleep(1);

error=CAENVME_IRQEnable(handle, cvIRQ1);
if(error!=cvSuccess)
	printf("Error in enabling IRQ\n");
else
	printf("IRQ level %d enabled succussfully\n",(int)cvIRQ1);

sleep(1);

//Data acquisition Program
printf("Press any key to start the acquisition\n");
getch();

while(!quit)  	{
	c = 0;
	if (kbhit()) c = getch();
	if(c == 'q') 	{
		quit = 1;
			}


j=1;
while(j){
	if((CAENVME_IRQWait(handle, cvIRQ1, 50) == cvSuccess)){ //IRQ wait timeout .05 sec
		CAENVME_IRQCheck(handle, &activeIRQ); // Check active IRQ lines
                //printf("iterrupt level %x has occured\n",activeIRQ);  //Debug
                CAENVME_IRQDisable(handle, activeIRQ); // Disable those lines
		//printf("irq vextor=%x\t STATUS=%x\n",irqVector,STATUS); //Debug
                //printf("active IRQ=%x\n",activeIRQ);  //Debug
                if((activeIRQ&STATUS)==STATUS){
                	error=CAENVME_IACKCycle(handle, cvIRQ1, &irqVector, cvD16); // acknowledge cycle
                    	if(error!=cvSuccess){
                        	printf("Error in IACK cycle with error code %d\n",error); 	}
                  	//printf("irqVector=%x\t STATUS=%x\n",irqVector,STATUS); //Debug
                    	if((irqVector & 0x00FF) == (STATUS & 0x00FF)){
                        //printf("Irq vector matched with TDC Irq vector\n"); //Debug
			while(1){
				temp1=0;
                            	error = CAENVME_ReadCycle(handle,V1190B_BA,&temp1,cvA32_U_DATA,cvD32);
				//printf("%d",(int)error);				//Debug
                            	if(error!=cvSuccess){
                                	printf("Error while reading from TDC %d\n",error);	 }
				Current_Data=temp1;
				//printf("Current_Data=%8x\n",Current_Data);		//Debug
                            	TDC_Word = (Current_Data & 0xF8000000);
				//printf("TDC_Word=%8x\n",TDC_Word);
                            	/*if(TDC_Word==0xC0000000) {  				//Debug
                                	printf("No data in buffer or filler data\n");
                                	usleep(100);	}
                            	if(TDC_Word==0x08000000){      // TDC Header
                                	printf("TDC Header\t%x\n",(Current_Data>>4)&0x7FFFF);	}
                            	if(TDC_Word==0x18000000){      // TDC trailer
                                	printf("TDC trailer\n");	}
                            	if(TDC_Word==0x20000000){      // TDC Error
                                	printf("TDC error\n");	}
                            	if(TDC_Word==0x81000000){      // TDC Time Tag
                                	printf("TDC Time Tag\n");	}*/
                            	if(TDC_Word==0x40000000){      // global header
					//printf("Global header\n");    //Debug                                
					TDC_EVENT_COUNT++;       	}
                            	if(TDC_Word==0x80000000){      // global trailer
		                        //printf("Global trailer\n");   //Debug
					fout = fopen(fname, "w");
					for(i=0; i<4096; i++) { 
						for(k=0; k<NOOFCHANNEL; k++) {
							fprintf(fout, "%d\t", (int)TDC_OUT[i][k]);}
						fprintf(fout,"\n");
					}
                			fclose(fout);
					break;	}
					
                            	else if(TDC_Word==0x00000000){    // data
                            		//TDC_Ch = (Current_Data & 0x03F80000)>>19;
                                	TDC_Ch = (Current_Data >>19)&0x7F;
                                	printf("TDC_Ch=%d\t",TDC_Ch);				
                        		TDC_Data[TDC_Ch] = Current_Data & 0x0007FFFF;
					printf("TDC measurement (%d)=%ld\n",TDC_Ch,TDC_Data[TDC_Ch]);
					temp=TDC_Data[TDC_Ch];
					TDC_OUT[temp][TDC_Ch]=TDC_OUT[temp][TDC_Ch]+1;	
				}

                        }
                        sleep(1);
                    	}

                }
		CAENVME_IRQEnable(handle, cvIRQ1);
	}
        j--;
}
}
printf("Total number of event recorded is %d\n",TDC_EVENT_COUNT);
printf("Please see \"%s\" for result from current run\n",fname);
return 0;
}
