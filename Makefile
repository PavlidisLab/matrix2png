#********************************************************************
# FILE: Makefile
# AUTHOR: Paul Pavlidis
# PROJECT: plotkit
# COPYRIGHT: 2001, Columbia University
# VERSION: 
#********************************************************************

COMPILER=gcc

#AR=ar

CFLAGS=-O3 -Werror

DEFINES=-DDEBUG -DTINYTEXT

LIBS=-lgd -lm -lpng -lz

CC=$(COMPILER)  $(CFLAGS) 

EXTRAFLAGS=$(LIBGD_DIR) $(LIBS) 

all: $(PROGRAMS)

tags: 
	etags ./*.h ./*.c


TEST = utils.SUF colors.SUF colorscalebar.SUF
TEST_OBJ = $(TEST:SUF=o)
test: $(TEST_OBJ)
	$(CC) -o testout test.c $(TEST_OBJ) $(EXTRAFLAGS) $(DEFINES)
	rm -f test.o colors.o colorscalebar.o utils.o

#--------------------------------------------------------------------

utils.o: utils.h utils.c
	$(CC) -c utils.c $(DEFINES)

colors.o: colors.h colors.c
	$(CC) -c colors.c $(DEFINES)

colorscalebar.o: colorscalebar.h colorscalebar.c
	$(CC) -c colorscalebar.c $(DEFINES)



