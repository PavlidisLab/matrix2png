#********************************************************************
# FILE: Makefile
# AUTHOR: Paul Pavlidis
# PROJECT: plotkit
# COPYRIGHT: 2001, Columbia University
# VERSION: $Revision: 1.1 $
#********************************************************************

COMPILER=gcc

AR=ar

CFLAGS=-O3

GD_INCLUDEDIR=-I/usr/local/include

INCLUDEDIRS=-I. $(GD_INCLUDEDIR)

LIBS=-lgd -lm -lpng -lz

#Location where libgd.a is installed
LIBGD_DIR=-L/usr/local/lib

CC=$(COMPILER) $(INCLUDEDIRS)

all: $(PROGRAMS)

tags: 
	etags ./*.h ./*.c



test: test.o 
	$(CC) colors.c test.o utils.c -o testout $(LIBGD_DIR) $(LIBS) 

