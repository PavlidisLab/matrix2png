## Process this file with automake to produce Makefile.in

bin_PROGRAMS = matrix2png
matrix2png_SOURCES = matrix2png.c string-list.c matrix.c array.c \
	utils.c text2png.c rdb-matrix.c addextras.c colors.c \
	colormap.c colordiscrete.c \
	colorscalebar.c locations.c cmdparse.c hash.c primes.c


#INCLUDES = -DTINYTEXT -DQUICKBUTCARELESS -DMATRIXMAIN  -Wall -W -Werror
#INCLUDES = -DTINYTEXT -DMATRIXMAIN  -DDEBUG -DBOUNDS_CHECK -Wall -W -Werror
#INCLUDES = -DSMALLTEXT -DMATRIXMAIN  -DDEBUG -DBOUNDS_CHECK -Wall -W -Werror
INCLUDES = -DSMALLTEXT -DMATRIXMAIN -Wall -W -Werror
#INCLUDES = -DTINYTEXT -DMATRIXMAIN -Wall -W -Werror
ETAGS_ARGS = ./*.h ./*.c 
