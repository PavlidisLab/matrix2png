/*****************************************************************************
 * FILE: matrix2png.h
 * AUTHOR: Paul Pavlidis
 * CREATE DATE: 2/2001
 * PROJECT: PLOTKIT
 * DESCRIPTION: Functions to make images from matrices
 *****************************************************************************/

#include "gd.h"
#include "colors.h"
#include "locations.h"
#include "rdb-matrix.h"

/* function which: given a matrix, and some other information, returns
 * a pointer to a gdimage object */
gdImage* matrix2img (
		     *MATRIX_T matrix,
		     double contrast,
		     BOOLEAN useDataRange, /* let the data define the range of values depicted. If false, must set minVal and maxVal */
		     BOOLEAN includeRowNames,
		     BOOLEAN includeColNames,
		     BOOLEAN includeDescriptiveText, /* include additional text descriptin for each row */
		     BOOLEAN includeDividers, /* add a 1-pixel grey border between each block */
		     BOOLEAN includeScaleBar,
		     *STRING_LIST rowNames, /* set these to null if not needed */
		     *STRING_LIST colNames,
		     *STRING_LIST descriptiveText,
		     int xSize, /* x dimension of each block */
		     int ySize, /* y dimensin of each block */
		     double minVal, /* the minimum value to be represented in the image. Lower values will be clipped. */
		     double maxVal, /* the max value to be represented in the image. Higher values will be clipped */
		     location scaleBarLoc,
		     location rowNamesLoc,
		     location colNamesLoc,
		     location descriptiveTextLoc,
		     color backgroundColor /* used for extra parts of the image - try white or black */
		     );


/* Given a raw 2-d array structure, do the same as the above (see
   there for descriptions of inputs */
gdImage* rawmatrix2img (
		     **ATYPE matrix,
		     BOOLEAN includeRowNames,
		     BOOLEAN includeColNames,
		     BOOLEAN includeDescriptiveText,
		     BOOLEAN includeDividers,
		     BOOLEAN includeScaleBar,
		     *STRING_LIST rowNames,
		     *STRING_LIST colNames,
		     *STRING_LIST descriptiveText,
		     int numRows, /* number of rows in the matrix */
		     int numCols, /* number of cols in the matrix */
		     int xSize,
		     int ySize,
		     double minVal, /* the minimum value to be represented in the image. Lower values will be clipped. */
		     double maxVal, /* the max value to be represented in the image. Higher values will be clipped */
		     location scaleBarLoc,
		     location rowNamesLoc,
		     location colNamesLoc,
		     location descriptiveTextLoc,
		     color backgroundColor
		     );


/* add a scale bar to an image. Must designate where to put it. If
   space was not allotted in the image for the scale bar */
void scaleBar (
	       gdImage* img,
	       int xStart,
	       int yStart,
	       int minColor,
	       int maxColor,
	       int height,
	       int width,
	       int numSteps
	       );







