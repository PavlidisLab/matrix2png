## Process this file with automake to produce Makefile.in

bin_PROGRAMS = matrix2png

matrix2png_SOURCES = matrix2png.c string-list.c matrix.c array.c \
	utils.c text2png.c rdb-matrix.c addextras.c colors.c \
	colormap.c colordiscrete.c \
	colorscalebar.c locations.c cmdparse.c hash.c primes.c \
	matrix2png.h string-list.h matrix.h array.h \
	utils.h text2png.h rdb-matrix.h addextras.h colors.h \
	colormap.h colordiscrete.h \
	colorscalebar.h locations.h cmdparse.h hash.h primes.h \
	cmdline.h matrixinfo.h

#INCLUDES = -DTINYTEXT -DQUICKBUTCARELESS -DMATRIXMAIN  -Wall -W -Werror
#INCLUDES = -DTINYTEXT -DMATRIXMAIN  -DDEBUG -DBOUNDS_CHECK -Wall -W -Werror
#INCLUDES = -DSMALLTEXT -DMATRIXMAIN  -DDEBUG -DBOUNDS_CHECK -Wall -W -Werror
#INCLUDES = -DSMALLTEXT -DMATRIXMAIN
INCLUDES = -DSMALLTEXT -DMATRIXMAIN -Wall -W -Werror
ETAGS_ARGS = ./*.h ./*.c 
