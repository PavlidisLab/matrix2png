/*****************************************************************************
 * FILE: matrix2png.h
 * AUTHOR: Paul Pavlidis
 * CREATE DATE: 2/2001
 * PROJECT: PLOTKIT
 * DESCRIPTION: Functions to make images from matrices
 *****************************************************************************/

#ifndef MATRIXPNG_H
#define MATRIXPNG_H

#include "gd.h"
#include "colors.h"
#include "locations.h"
#include "rdb-matrix.h"
#include "string-list.h"
#include "utils.h"
#include "matrix.h"
#include "colordiscrete.h"

#define DEFAULTCONTRAST 1.0
#define DEFAULTBKGCOLOR white
#define DEFAULTMINCOLOR blue
#define DEFAULTMAXCOLOR red
#define DEFAULTDIVIDERCOLOR grey
#define DEFAULTCOLORMAP 0
#define DEFAULTSCALEBARLOC (defineLocation("topleft", topleft, 0.0, 0.0))
#define DEFAULTROWNAMESLOC (defineLocation("rightmiddle", rightmiddle, 1.0, 0.5))
#define DEFAULTCOLNAMESLOC (defineLocation("topmiddle", topmiddle, 0.5, 0.0))
#define DEFAULTDESCTEXTLOC (defineLocation("rightmiddle", rightmiddle, 1.0, 0.5))
#define DEFAULTXPIXSIZE 2
#define DEFAULTYPIXSIZE 2

/* data structure for storing the details of the matrix image */
typedef struct matrixinfo_t {
  int ulx; /* upper left x etc */
  int uly;
  int lrx;
  int lry;
  int numrows; 
  int numcols; 
  int xblocksize;
  int yblocksize;
  double minval;
  double maxval;
  double outliers;
  int xminSize; // todo: not needed
  int yminSize; // todo: not needed
  USED_T* usedRegion;
  BOOLEAN_T dividers;
  BOOLEAN_T circles;
} MATRIXINFO_T;

/* create a new matrixinfo struct */
MATRIXINFO_T* newMatrixInfo(void);

/* function which: given a matrix, and some other information, returns
 * a pointer to a gdimage object */
gdImagePtr matrix2img (
		     MATRIX_T* matrix,
		     double contrast,
		     BOOLEAN_T useDataRange, /* let the data define the range of values depicted. If false, must set minVal and maxVal */
		     BOOLEAN_T includeDividers, /* add a 1-pixel grey border between each block */
		     BOOLEAN_T passThroughBlack, /* use black as the middle value in the map? */
		     double minVal, /* the minimum value to be represented in the image. Lower values will be clipped. Only used if useDataRange is false */
		     double maxVal, /* the max value to be represented in the image. Higher values will be clipped Only used if useDataRange is false */
		     color_T minColor,
		     color_T maxColor,
		     color_T backgroundColor, /* used for extra parts of the image - try white or black */
		     color_T missingColor, /* used for extra parts of the image - try white or black */
		     int colorMap, /* optional color mapping */
		     DISCRETEMAP_T* discreteMap,
		     int numcolors,
		     MATRIXINFO_T* matrixInfo,
		     MTYPE** rawmatrix
		     );


/* Given a raw 2-d array structure, do the same as the above (see
   there for descriptions of inputs */
gdImagePtr rawmatrix2img (
		     MTYPE** matrix,
		     //		     int numrows,
		     //		     int numcols,
		     double contrast,
		     BOOLEAN_T useDataRange, /* let the data define the range of values depicted. If false, must set minVal and maxVal */
		     BOOLEAN_T includeDividers, /* add a 1-pixel grey border between each block */
		     BOOLEAN_T passThroughBlack, /* use black as the middle value in the map? */
		     //		     int xSize, /* x dimension of each block */
		     //		     int ySize, /* y dimension of each block */
		     double minVal, /* the minimum value to be represented in the image. Lower values will be clipped. Only used if useDataRange is false */
		     double maxVal, /* the max value to be represented in the image. Higher values will be clipped  Only used if useDataRange is false*/
		     color_T minColor,
		     color_T maxColor,
		     color_T backgroundColor, /* used for extra parts of the image - try white or black */
		     color_T missingColor, /* used for extra parts of the image - try white or black */
		     int colorMap, /* optional color mapping */
		     DISCRETEMAP_T* discreteMap,
		     int numcolors,
		     MATRIXINFO_T* matrixInfo
		     );


#endif /* matrix2png.h */
