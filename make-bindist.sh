#!/bin/sh

# Build binary distributions for matrix2png. Makes a version which
# uses shared libpng/gd and one that has the libraries statically
# linked. Creates a matrix2png-*-static.tar.gz and
# matrix2png-*-shared.tar.gz where * is the architecture name.

# Set these to be the source directories of the required libraries.
#LIBPNGDIR=/netapps/pavlidis/grp/downloads/libpng-1.2.2/
#LIBGDDIR=/netapps/pavlidis/grp/downloads/gd-2.0.1/
LIBPNGDIR=/usr/lib/
LIBGDDIR=/usr/lib/
#TAR=/usr/local/bin/tar
TAR=tar

# hopefully you won't have to make any changes below this line

HERE=`pwd`
ARCH=`uname -ms | tr  ' ' '-'`

#rm matrix2png;
./configure
make clean
make
#echo "$TAR -c matrix2png | gzip -c > matrix2png-${ARCH}-shared.tar.gz"
#$TAR -c matrix2png | gzip -c > matrix2png-${ARCH}-shared.tar.gz
#mv matrix2png matrix2png.shared
#cd $LIBPNGDIR
#echo `pwd`
#make clean
#make libpng.a
#echo `pwd`
#cd $LIBGDDIR
#make clean
#make libgd.a
#cd $HERE;
#echo `pwd`
gcc  -g -O2  -o matrix2png  matrix2png.o string-list.o matrix.o array.o utils.o text2png.o rdb-matrix.o addextras.o colors.o colormap.o colordiscrete.o colorscalebar.o locations.o cmdparse.o hash.o primes.o ${LIBGDDIR}libgd.a ${LIBPNGDIR}libpng.a  -lz -lm
echo "$TAR -c $HERE/matrix2png | gzip -c > $HERE/matrix2png-${ARCH}-static.tar.gz"
$TAR -c matrix2png | gzip -c > matrix2png-${ARCH}-static.tar.gz
#mv matrix2png matrix2png.static
