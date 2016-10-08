# CAEN_V1190B_A2818
TDC V1190B Data acquisition program with VME optical bridge a2718

V1190B program is a basic program written for CAEN V1190B (Multi-Hit TDC module, mainly used in high precision particle physics experiments) module for data acquisition and store data in a text file which can be used later for analysis. This is application specific program is compatible with a2818 VME-optical Bridge.

Release date: Feb 27 2016 22:24 IST

Auth: Purnendu Kumar

Contents: V1190B.c, keyb.c, keyb.h.V1190B.h,V1190Bmicro.h, V1190Bsettings.h

prequities: CAEN a2818 Driver, CAEN VME Library, Linux Ubuntu 14.04LTS, GCC

Modules Compatiblity: VME-optical Bridge CAEN a2818, Multi-Hit TDC CAEN V1190B

To compile directly use: gcc V1190B.c keyb.c -DLINUX -lCAENVME -o V1190B

Acknowledgement:

1.CAEN Support Computing, Carlo Tintori(CAEN),

2.Indian Institute of Technology Madres, Chennai. http://iitm.ac.in,

3.Chris Giese geezer@execpc.com website: http://my.execpc.com/~geezer,

4.Tata Institute of Fundamental Research, Mumbai.
