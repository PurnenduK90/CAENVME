###########################################################################
# Program:		SCALAR CAEN V830 Data Acquisition Program
# Discriminator used:	V814
# OS:			Linux Ubuntu 12.04
# VME-USB Bridge:	CAEN V1718
# Prequities:		CAENVME Lib, CAEN USB Driver
# Auth:			Purnendu Kumar(JRF, University of Delhi).
# Date:			January 15, 2014.
# Acknowledgement:	Carlo Tintori(CAEN)
###########################################################################

EXE	=	V830

CC	=	gcc

COPTS	=	-fPIC -DLINUX -Wall 
#COPTS	=	-g -fPIC -DLINUX -Wall 

FLAGS	=	-Wall -s
#FLAGS	=	-Wall

DEPLIBS	=       -l CAENVME  -lc -lm

LIBS	=	

INCLUDEDIR =	-I.

OBJS	=	V830.o keyb.o

INCLUDES =	keyb.h V830.h
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

