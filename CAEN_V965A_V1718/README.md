# CAEN_V965A_V1718
QDC CAEN V965A Data acquisition program with VME USB bridge V1718
V965A program is a basic program written for CAEN V965A module for data acquisition and store data in a text file which can be used later for analysis. This is application specific program is compatible with V1718 VME-USB Bridge.

Release date: Jan 18 2014 16:24 IST

Auth: Carlo Tintori (CAEN)

Modified and Optimized by: Purnendu Kumar (JRF)purnenduk90@gmail.com, University of Delhi.

Contents: V7965A.c, keyb.c, keyb.h

prequities: CAEN USB Driver, CAEN VME Library, Linux Ubuntu 12.04LTS, GCC

Modules Compatiblity: VME-USB Bridge CAEN V1718, Scaler CAEN V965A

To compile directly use: gcc V965A.c keyb.c -DLINUX -lCAENVME -o V965A

Acknowledgement:

1.CAEN Support Computing, Carlo Tintori(CAEN),

2.University of Delhi (CDRST)http://cdrstdu.edu.in/index.html

3.Chris Giese geezer@execpc.com website: http://my.execpc.com/~geezer
