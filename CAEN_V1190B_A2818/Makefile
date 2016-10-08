###########################################################################
# Program:		TDC V1190B Data Acquisition Program
# OS:			Linux Ubuntu 14.04
# VME-USB Bridge:	CAEN V2718
# Prequities:		CAENVME Lib, CAEN a2818 Driver
# Auth:			Purnendu Kumar 
# Date:			February 26, 2016.
# Acknowledgement:	caen.it, TIFR, my.execpc.com/~geezer
###########################################################################
# Copyright (C) {2016} {Purnendu Kumar} http://engineeringxd.com

EXE	=	V1190B

CC	=	gcc

COPTS	=	-fPIC -DLINUX -Wall  

FLAGS	=	-Wall -s

DEPLIBS	=       -l CAENVME  -lc -lm

LIBS	=	

INCLUDEDIR =	-I.

OBJS	=	V1190B.o keyb.o

INCLUDES =	keyb.h V1190B.h V1190Bmicro.h V1190Bsettings.h
#########################################################################

all	:	$(EXE)

clean	:
		/bin/rm -f $(OBJS) $(EXE)

$(EXE)	:	$(OBJS)
		/bin/rm -f $(EXE)
		$(CC) $(FLAGS) -o $(EXE) $(OBJS) $(DEPLIBS)

$(OBJS)	:	$(INCLUDES) Makefile

%.o	:	%.c
		$(CC) $(COPTS) $(INCLUDEDIR) -c -o $@ $<
