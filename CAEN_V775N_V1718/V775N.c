/*******************************************************************************/
/* Programma per rileggere un canale del V792 e scrivere un file di istogramma
Auth: Carlo Tintori
Edited By: Purnendu Kumar     (09-JAN-2014)      University of Delhi           */
/*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifdef WIN32
    #include <time.h>
    #include <sys/timeb.h>
    #include <conio.h>
	#define kbhit _kbhit
	#define getch _getch
#else
    #include <unistd.h>
    #include <sys/time.h>
	//#define Sleep(x) usleep((x)*1000)
#endif

#include <CAENVMElib.h>
#include <CAENVMEtypes.h>


#define ulong  unsigned long
#define ushort unsigned short
#define uchar  unsigned char



#define MAX_BLT_SIZE  (256*1024)

#define DATATYPE_MASK	    0x06000000
#define DATATYPE_HEADER     0x02000000
#define DATATYPE_CHDATA     0x00000000
#define DATATYPE_EOB        0x04000000
#define DATATYPE_FILLER     0x06000000

#define LSB2PHY             35   // LSB (= ADC count) to Physical Quantity (time in ps, charge in fC, amplitude in mV)

#define LOGMEAS_NPTS       1000


#define ENABLE_LOG  0

// --------------------------
// Variabili Globali
// --------------------------
// V792 Base Address 
ulong base_addr = 0xEE000000;

// handle per il V1718/V2718 
ulong handle; 

int VMEerror = 0;
char ErrorString[100];
FILE *logfile;


// ---------------------------------------------------------------------------
// get time in milliseconds
// ---------------------------------------------------------------------------
long get_time()
{
long time_ms;

#ifdef WIN32
    struct _timeb timebuffer;

    _ftime( &timebuffer );
    time_ms = (long)timebuffer.time * 1000 + (long)timebuffer.millitm;
#else
    struct timeval t1;
    struct timezone tz;

    gettimeofday(&t1, &tz);
    time_ms = (t1.tv_sec) * 1000 + t1.tv_usec / 1000;
#endif

    return time_ms;
}

/*******************************************************************************/
/*                               READ_REG                                      */
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
	if (ENABLE_LOG)
		fprintf(logfile, " Reading register at address %08X; data=%04X; ret=%d\n", (uint32_t)(base_addr + reg_addr), data, (int)ret);
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
	if (ENABLE_LOG)
		fprintf(logfile, " Writing register at address %08X; data=%04X; ret=%d\n", (uint32_t)(base_addr + reg_addr), data, (int)ret);
}
      

/******************************************************************************/
/*                                   MAIN                                     */
/******************************************************************************/
void main(int argc,char *argv[])
{
	int i, j, ch, chindex, wcnt, nch, pnt, ns[32], bcnt;
	int quit=0, totnb=0, nev=0, DataError=0, LogMeas=0, lognum=0;
	int DataType = DATATYPE_HEADER;
	int QDCba, CFDba;
	char c;
	ushort fwrev;
	ulong histo[32][4096];
	uint32_t buffer[MAX_BLT_SIZE/4];
	ushort CFD_OutputWidth;
	ushort CFD_Threshold;
        ushort CFD_ChannelMask;
    	long CurrentTime, PreviousTime, ElapsedTime;
	float rate = 0.0;
	FILE *fh, *gnuplot;

	if (argc < 4)  {
		printf("Syntax: v792_histo QDC_BaseAddress CFD_BaseAddress Channel\n");
		getch();
		exit (-1);
	}

	sscanf(argv[1], "%x", &QDCba);
	sscanf(argv[2], "%x", &CFDba);
	sscanf(argv[3], "%d", &ch);
	printf("QDC-BA = 0x%08X, CFD-BA = 0x%08X, Channel = %d\n", QDCba, CFDba, ch);

	printf("Log file = %d\n", ENABLE_LOG);

	if (ENABLE_LOG) {

		printf("Log file is enabled\n");
		logfile = fopen("v792_log","w");
	}
	if(CAENVME_Init(cvV1718, 0, 0, &handle) != cvSuccess ) {
		if( CAENVME_Init(cvV2718, 0, 0, &handle) != cvSuccess ) {
			printf("Error opening VME bridge V1718/V2718\n");
			getch();
			exit (-1);
		}
	}

	
#ifdef LINUX
	gnuplot = popen("/usr/bin/gnuplot", "w");
#else
	gnuplot = _popen("pgnuplot.exe", "w");
#endif
	if (gnuplot == NULL) {
		printf("Can't open gnuplot\n\n");
		exit (0);
	}

	for(j=0; j<32; j++) {
                ns[j]=0;	
		for(i=0; i<4096; i++)
			histo[j][i]=0;
	}

	// ************************************************************************
	// CFD settings
	// ************************************************************************
	base_addr = CFDba;
	if (VMEerror) {
		printf("Error during CFD programming: ");
		printf(ErrorString);
		getch();
		goto QuitProgram;
	}
	CFD_OutputWidth = 255;
	//CFD_Threshold = 25;  // mV
ulong CFD_Threshold0 = 50;   	//Discriminator threshold (ch 0); mV
	ulong CFD_Threshold1 = 20;   	//Discriminator threshold (ch 1); mV
	ulong CFD_Threshold2 = 50;   	//Discriminator threshold (ch 2); mV
        CFD_ChannelMask = 0x00; // ch 0and 1 only

	// Set channel mask
	write_reg(0x004A, CFD_ChannelMask);

	// set output width
	write_reg(0x0040, CFD_OutputWidth);
	write_reg(0x0042, CFD_OutputWidth);

	// set CFD threshold
	//for(i=0; i<16; i++)
		//write_reg(i*2, CFD_Threshold);
write_reg(0, CFD_Threshold0);
	write_reg(2, CFD_Threshold1);
	write_reg(4, CFD_Threshold2);
	printf("Discriminator programmed\n");


	// ************************************************************************
	// QDC settings
	// ************************************************************************
	base_addr = QDCba;
	// Reset V792
	write_reg(0x1016, 0);
	if (VMEerror) {
		printf("Error during CFD programming: ");
		printf(ErrorString);
		getch();
		goto QuitProgram;
	}

	// Read FW revision
	fwrev = read_reg(0x1000);
	if (VMEerror) {
		printf(ErrorString);
		getch();
		goto QuitProgram;
	}
	printf("FW Revision = %d.%d\n", (fwrev >> 8) & 0xFF, fwrev & 0xFF);

	write_reg(0x1060, 100);  // Set pedestal
	write_reg(0x1010, 0x20);  // enable BERR to close BLT at and of block
#if 0
	write_reg(0x1032, 0x0010);  // disable zero suppression
	write_reg(0x1032, 0x0008);  // disable overrange suppression
	write_reg(0x1032, 0x1000);  // enable empty events
#endif

	printf("V792 programmed\n");

	printf("Starting Acquisition: reading data from channel %d\n",ch);
	Sleep(1000);

	// ------------------------------------------------------------------------------------
	// Acquisition loop
	// ------------------------------------------------------------------------------------
	pnt = 0;  // word pointer
	wcnt = 0; // num of lword read in the MBLT cycle
	buffer[0] = DATATYPE_FILLER;

	// clear V792
	write_reg(0x1032, 0x4);
	write_reg(0x1034, 0x4);

    	PreviousTime = get_time();
	while(!quit)  {

		c = 0;
		if (kbhit())
			c = getch();

		if (c == 'r') {
			for(j=0; j<32; j++) {
				ns[j]=0;	
				for(i=0; i<4096; i++)
					histo[j][i]=0;
			}
		}
		if(c == 'q') {
			quit = 1;
		}
		if(c == 'c') {
			printf("Enter new channel : ");
			scanf("%d", &ch);
		}
                

		// Log statistics on the screen
		CurrentTime = get_time(); // Time in milliseconds
		ElapsedTime = CurrentTime - PreviousTime;
		if (ElapsedTime > 1000) {
			system("clear");
			rate = (float)nev / ElapsedTime;
			printf("Acquired %d events on channel %d\n", ns[ch], ch);
			if (nev > 1000)
				printf("Trigger Rate = %.2f KHz\n", (float)nev / ElapsedTime);
			else
				printf("Trigger Rate = %.2f Hz\n", (float)nev * 1000 / ElapsedTime);
			if (totnb > (1024*1024))
				printf("Readout Rate = %.2f MB/s\n", ((float)totnb / (1024*1024)) / ((float)ElapsedTime / 1000));
			else
				printf("Readout Rate = %.2f KB/s\n", ((float)totnb / 1024) / ((float)ElapsedTime / 1000));
			nev = 0;
			totnb = 0;
			fh = fopen("histo.txt","w");
			for(i=0; i<4096; i++) {
				fprintf(fh, "%d\n", (int)histo[ch][i]);
			}
			fclose(fh);
			fh = fopen("histo1.txt","w");
			for(i=0; i<4096; i++) {
				fprintf(fh, "%d\n", (int)histo[ch+1][i]);
			}
			fclose(fh);
			fprintf(gnuplot, "set ylabel 'Counts'\n");			
			fprintf(gnuplot, "set xlabel 'ADC channels'\n");
			fprintf(gnuplot, "set yrange [0:]\n");
			fprintf(gnuplot, "set grid\n");
			fprintf(gnuplot, "set title 'Ch. %d (Rate = %.3fKHz, counts = %d)'\n", ch, rate, ns[ch]);
			fprintf(gnuplot, "plot 'histo.txt' with step, 'histo1.txt' with step\n");
			fflush(gnuplot);
			printf("[q] quit  [r] reset statistics  [c] change channel\n");
			PreviousTime = CurrentTime;
		}

		// if needed, read a new block of data from the board 
		if ((pnt == wcnt) || ((buffer[pnt] & DATATYPE_MASK) == DATATYPE_FILLER)) {
			CAENVME_MBLTReadCycle(handle, base_addr, (uchar *)buffer, MAX_BLT_SIZE, cvA32_U_MBLT, &bcnt);
			if (ENABLE_LOG && (bcnt>0)) {
				int b;
				fprintf(logfile, "Read Data Block: size = %d bytes\n", bcnt);
				for(b=0; b<(bcnt/4); b++)
					fprintf(logfile, "%2d: %08X\n", b, buffer[b]);
			}

			wcnt = bcnt/4;
			totnb += bcnt;
			pnt = 0;
		}
		if (wcnt == 0)  // no data available
			continue;

		/* header */
		switch (DataType) {
		case DATATYPE_HEADER :
			if((buffer[pnt] & DATATYPE_MASK) != DATATYPE_HEADER) {
				printf("Header not found: %08X (pnt=%d)\n", buffer[pnt], pnt);
				DataError = 1;
			} else {
				nch = (buffer[pnt] >> 8) & 0x3F;
				chindex = 0;
				nev++;
				if (nch>0)
					DataType = DATATYPE_CHDATA;
				else
					DataType = DATATYPE_EOB;
			}
			break;

		/* Channel data */
		case DATATYPE_CHDATA :
			if((buffer[pnt] & DATATYPE_MASK) != DATATYPE_CHDATA) {
				printf("Wrong Channel Data: %08X (pnt=%d)\n", buffer[pnt], pnt);
				DataError = 1;
			} else {
				j = (int)((buffer[pnt] >> 17) & 0x3F);
				histo[j][buffer[pnt] & 0xFFF]++;
				ns[j]++;
				if (chindex == (nch-1))
					DataType = DATATYPE_EOB;
				chindex++;
			}
			break;

		/* EOB */
		case DATATYPE_EOB :
			if((buffer[pnt] & DATATYPE_MASK) != DATATYPE_EOB) {
				printf("EOB not found: %08X (pnt=%d)\n", buffer[pnt], pnt);
				DataError = 1;
			} else {
				DataType = DATATYPE_HEADER;
			}
			break;
		}
		pnt++;

		if (DataError) {
			pnt = wcnt;
			write_reg(0x1032, 0x4);
			write_reg(0x1034, 0x4);
			DataType = DATATYPE_HEADER;
			DataError=0;
		}

	}

// ------------------------------------------------------------------------------------

	QuitProgram:
	fclose(gnuplot);
	CAENVME_End(handle);
	for(j=0; j<32; j++) {
                                FILE *fout;
                                char fname[100];
                                sprintf(fname, "Histo_%d.txt", j);
                                fout = fopen(fname, "w"); 
				for(i=0; i<4096; i++) 
					fprintf(fout, "%d\n", (int)histo[ch][i]);
                                fclose(fout);
				}
				printf("Saved histograms to output files\n");
}
