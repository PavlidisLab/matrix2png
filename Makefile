#********************************************************************
# FILE: Makefile
# AUTHOR: Paul Pavlidis
# PROJECT: plotkit
# COPYRIGHT: 2001, Columbia University
# VERSION: 
#********************************************************************

COMPILER=gcc

CFLAGS=-O3 -Werror

DEFINES=-DDEBUG -DTINYTEXT

LIBS=-lgd -lm -lpng -lz

CC=$(COMPILER)  $(CFLAGS) 

EXTRAFLAGS=$(LIBGD_DIR) $(LIBS) 

PROGRAMS=text2png matrix2png test

GDSOURCE= /space/users/pavlidis/data/download/gd-1.8.3/*.c  /space/users/pavlidis/data/download/gd-1.8.3/*.h

all: $(PROGRAMS)

tags: 
	rm -f TAGS
	etags ./*.h ./*.c /usr/local/include/gd.h $(GDSOURCE)

clean:
	rm -f *.o

TEST = utils.SUF colors.SUF colorscalebar.SUF locations.SUF
TEST_OBJ = $(TEST:SUF=o)
test: $(TEST_OBJ)
	$(CC) -o testout test.c $(TEST_OBJ) $(EXTRAFLAGS) $(DEFINES)
	rm -f test.o colors.o colorscalebar.o utils.o locations.o

MATRIX2PNG = colors.SUF colorscalebar.SUF locations.SUF utils.SUF \
	rdb-matrix.SUF string-list.SUF matrix.SUF array.SUF cmdparse.SUF \
	 addextras.SUF text2png.SUF
MATRIX2PNG_OBJ = $(MATRIX2PNG:SUF=o)
matrix2png: $(MATRIX2PNG_OBJ)
	$(CC) -o matrix2png matrix2png.c $(MATRIX2PNG_OBJ) $(EXTRAFLAGS) $(DEFINES) -DMATRIXMAIN
	rm -f matrix2png.o colors.o colorscalebar.o locations.o

TEXT2PNG = utils.SUF string-list.SUF cmdparse.SUF
TEXT2PNG_OBJ = $(TEXT2PNG:SUF=o)
text2png: $(TEXT2PNG_OBJ)
	$(CC) -o text2png text2png.c $(TEXT2PNG_OBJ) $(EXTRAFLAGS) $(DEFINES) -DTEXTMAIN
	rm -f text2png.o utils.o


#--------------------------------------------------------------------

utils.o: utils.h utils.c
	$(CC) -c utils.c $(DEFINES)

colors.o: colors.h colors.c
	$(CC) -c colors.c $(DEFINES)

colorscalebar.o: colorscalebar.h colorscalebar.c
	$(CC) -c colorscalebar.c $(DEFINES)

locations.o: locations.h locations.c
	$(CC) -c locations.c $(DEFINES)

rdb-matrix.o: utils.o array.o matrix.o string-list.c rdb-matrix.h rdb-matrix.c
	$(CC) -c rdb-matrix.c $(DEFINES)

matrix.o: matrix.h matrix.c
	$(CC) -c matrix.c $(DEFINES)

array.o: array.h array.c utils.h utils.c
	$(CC) -c array.c $(DEFINES)

string-list.o: string-list.h string-list.c utils.o \
	string-list.c
	$(CC) -c string-list.c $(DEFINES)

cmdparse.o: cmdparse.h cmdparse.c
	$(CC) -c cmdparse.c $(DEFINES)

addextras.o: addextras.h addextras.c
	$(CC) -c addextras.c $(DEFINES)

