## Process this file with automake to produce Makefile.in

bin_PROGRAMS = matrix2png
matrix2png_SOURCES = matrix2png.c string-list.c matrix.c array.c \
	utils.c text2png.c rdb-matrix.c addextras.c colors.c \
	colormap.c colordiscrete.c \
	colorscalebar.c locations.c cmdparse.c 


#INCLUDES = -DTINYTEXT -DQUICKBUTCARELESS -DMATRIXMAIN -DSTRINGHASH
#INCLUDES = -DTINYTEXT -DMATRIXMAIN -DSTRINGHASH -DDEBUG
INCLUDES = -DTINYTEXT -DMATRIXMAIN
ETAGS_ARGS = ./*.h ./*.c /usr/local/include/gd.h 
TAGS_DEPENDENCIES =  /usr/local/include/gd.h
