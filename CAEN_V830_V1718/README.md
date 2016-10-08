# CAEN_V830_V1718
SCALAR CAEN V830 Data Acquisition Program with VME USB bridge CAEN V1718
V830 program is a basic program written for CAEN V830 module for data acquisition and store data in a text file which can be used later for analysis. This is application specific programing which also involves the threshold and width setting of Discriminator V814 and is compatible with V1718 VME-USB Bridge.

Release date: Jan 16 2014 21:58 IST

Auth: Purnendu Kumar (JRF)<purnenduk90@gmail.com>, University of Delhi. 

Contents: Makefile, V830.c, V830.h, keyb.c, keyb.h

prequities: CAEN USB Driver, CAEN VME Library, Linux Ubuntu 12.04LTS, GCC

Modules Compatiblity: VME-USB Bridge CAEN V1718, Discriminator CAEN V814, Scaler CAEN V830

Changing the settings: Setting parameters are highlighted in "V830.c" once you change the desired setting save the file and make again.

To compile directly use: gcc V830.c keyb.c -DLINUX -lCAENVME -o V830

Acknowledgement: 

1.CAEN Support Computing, Carlo Tintori(CAEN),<caen.it>

2.University of Delhi (CDRST)<http://cdrstdu.edu.in/index.html>

3.Chris Giese <geezer@execpc.com> website: http://my.execpc.com/~geezer
