/*****************************************************************************
 * FILE: matrix2png.c
 * AUTHOR: Paul Pavlidis
 * CREATE DATE: 2/2001
 * PROJECT: PLOTKIT
 * DESCRIPTION: Functions to make images from matrices
 *****************************************************************************/

#include <stdio.h>
#include <string.h>
#include "matrix2png.h"
#include "gd.h"
#include "locations.h"
#include "colordiscrete.h"
#include "colors.h"
#include "colormap.h"
#include "string-list.h"
#include "utils.h"
#include "matrix.h"
#include "array.h"
#include "rdb-matrix.h"
#include "text2png.h"
#include "cmdline.h"
#include "cmdparse.h"
#include "addextras.h"
#include <float.h>

/* create a new matrixinfo struct */
MATRIXINFO_T* newMatrixInfo(void) {
  return (MATRIXINFO_T*)mymalloc(sizeof(MATRIXINFO_T));
} /* newMatrixInfo */

/* dump the info about a matrix */
void dumpMatrixInfo(MATRIXINFO_T* matrixInfo) {
  //  fprintf(stderr, "Acutal rows: %d   Actual columns: %d\n", matrixInfo->numactualrows,matrixInfo->numactualcols);
  fprintf(stderr, "Rows: %d   Columns: %d\n", matrixInfo->numrows,matrixInfo->numcols);
}

/* hands raw matrix to rawmatrix2img for processing */
gdImagePtr matrix2img (
		     MATRIX_T* matrix,
		     double contrast,
		     BOOLEAN_T useDataRange, /* let the data define the range of values depicted. If false, must set minVal and maxVal */
		     BOOLEAN_T includeDividers, /* add a 1-pixel grey border between each block */
		     BOOLEAN_T passThroughBlack, /* use black as the middle value in the map? */
		     double minVal, /* the minimum value to be represented in the image. Lower values will be clipped. Only used if useDataRange is false */
		     double maxVal, /* the max value to be represented in the image. Higher values will be clipped Only used if useDataRange is false */
		     colorV_T* minColor,
		     colorV_T* maxColor,
		     colorV_T* backgroundColor, /* used for extra parts of the image - try white or black */
		     colorV_T* missingColor, /* used for extra parts of the image - try white or black */
		     int colorMap,
		     DISCRETEMAP_T* discreteMap,
		     int numcolors,
		     MATRIXINFO_T* matrixInfo,
		     MTYPE** rawmatrix
		     )
{
  MTYPE* rawrow;
  ARRAY_T* row;
  int i;
  if (rawmatrix != NULL) {
    die("Attempt to pass raw matrix to matrix2img with  non-NULL value\n");
  } else {
    rawmatrix = (MTYPE**)mymalloc(matrixInfo->numrows*sizeof(MTYPE));
  }

  /* decant the matrix into a regular matrix */
  for (i=0;i<matrixInfo->numrows;i++) {
    row = get_matrix_row(i, matrix);
    rawrow = row->items;
    rawmatrix[i] = rawrow;
  }
  
  return rawmatrix2img(rawmatrix, contrast, useDataRange, 
		       includeDividers, passThroughBlack,
		       minVal, maxVal,
		       minColor, maxColor, backgroundColor, missingColor, colorMap, discreteMap, numcolors, matrixInfo);
  
} /* matrix2img */



/* Given a raw 2-d array structure make image */
gdImagePtr rawmatrix2img (
		     MTYPE** matrix,
		     double contrast,
		     BOOLEAN_T useDataRange, /* let the data define the range of values depicted. If false, must set minVal and maxVal */
		     BOOLEAN_T includeDividers, /* add a 1-pixel grey border between each block */
		     BOOLEAN_T passThroughBlack, /* use black as the middle value in the map? */
		     double minVal, /* the minimum value to be represented in the image. Lower values will be clipped. Only used if useDataRange is false */
		     double maxVal, /* the max value to be represented in the image. Higher values will be clipped  Only used if useDataRange is false*/
		     colorV_T* minColor,
		     colorV_T* maxColor,
		     colorV_T* backgroundColor, /* used for extra parts of the image - try white or black */
		     colorV_T* missingColor, /* used for extra parts of the image - try white or black */
		     int colorMap,
		     DISCRETEMAP_T* discreteMap,
		     int numColors,
		     MATRIXINFO_T* matrixInfo
		     )
{
  gdImagePtr img; /* the image */
  int i, j; /* counters */
  int x, y; /* locations in the image */
  int colorcode; /* current color */
  double min, max; /* range values */
  double range, stepsize; /* value to color mapping info */
  int width, height; /* size of image */
  double value; /* value to be graphed */
  int initX, initY; /* where we should start drawing the matrix */
  int xoffset, yoffset;
  int featureWidth, featureHeight;
  int dividerColor = 0;
  int xSize = matrixInfo->xblocksize;
  int ySize = matrixInfo->yblocksize;

  /* create image to fit (1 pixel dividers)*/
  if (includeDividers) {
    xSize++;
    height = matrixInfo->numrows * (ySize+1);
  } else {
    height = matrixInfo->numrows * ySize;
  }
  width = matrixInfo->numcols * xSize;

  featureWidth = width;
  featureHeight = height;
  
  //  if (matrixInfo->xminSize > width) width = matrixInfo->xminSize;
  //  if (matrixInfo->yminSize > height) height = matrixInfo->yminSize;
  DEBUG_CODE(1, fprintf(stderr, "Set image size to %d by %d\n", width, height););

  img = gdImageCreate(width, height);

  /* allocate the colors */
  if (colorMap) {
    allocateColorMap(img, backgroundColor, missingColor, colorMap, numColors);
  } else if (discreteMap != NULL) {
    allocateColorsDiscrete(img, discreteMap, backgroundColor, missingColor);
  } else {
    allocateColors(img, backgroundColor, minColor, maxColor, missingColor, passThroughBlack, numColors);
  }

  if (includeDividers) {
    int r,g,b;
    color2rgb((colorV_T*)DEFAULTDIVIDERCOLOR, &r, &g, &b);
    dividerColor = gdImageColorClosest(img, r, g, b);
    DEBUG_CODE(1, fprintf(stderr, "Including dividers %d %d %d %d\n", r, g, b, dividerColor););
  }

  /* place the image (which is empty at this point)*/
  placeFeature(img,
	       "center",
	       FALSE,
	       &initX,
	       &initY,
	       matrixInfo->usedRegion,
	       featureWidth,
	       featureHeight, 
	       &xoffset, &yoffset);

  DEBUG_CODE(1, 
	     if(img == NULL) die ("null image\n");
	     );

  DEBUG_CODE(1, fprintf(stderr, "Image is %d by %d pixels; starting from %d, %d\n", gdImageSX(img), gdImageSY(img), initX, initY););
  
  /* figure out the value-to-color mapping */
  if (useDataRange && matrix[0] != NULL) {
    //    int minindx, maxindx, minindy, maxindy; /* caution: these are not used now  */
    if (matrixInfo->numrows > 0)
      find_rawmatrix_min_and_max(matrix, matrixInfo->numrows, matrixInfo->numcols, matrixInfo->outliers, &min, &max);
    min/=contrast;
    max/=contrast;
  } else {
    max = maxVal;
    min = minVal;
  }

  range = max - min;
  stepsize = range / numColors;
  DEBUG_CODE(1, fprintf(stderr, "Min is %f, max is %f, Step size is %f\n", min, max, stepsize););

  /* draw the image */
  y = initY;
  for (i=0; i<matrixInfo->numrows; i++) {
    x = initX;
    if(includeDividers && i>0) {
      gdImageLine(img, initX, y, initX + width, y, dividerColor);
      y++;
    }

    for (j=0; j<matrixInfo->numcols; j++) {

      value = matrix[i][j];

      if (isnan(value)) {
	colorcode = MISSING;
      } else if (discreteMap != NULL) {
	if (value > discreteMap->count || value < 0) {
	  colorcode = DEFAULT_DISCRETE_COLOR_INDEX;
	} else {
	  colorcode = (int)value+NUMRESERVEDCOLORS;
	}
      } else {
 	/* clip color if necessary */
	if (!useDataRange || contrast != 1.0 || matrixInfo->outliers) {
	  if (value > max) {
 	    value = max;
 	  } else if (value < min) {
 	    value = min;
 	  }
 	}
 	colorcode = (int)( (value - min) / stepsize) + NUMRESERVEDCOLORS;
      }

      /* draw rectangle and advance to the next position */
      if (matrixInfo->circles) {
	int yRad = (int)ySize/2;
	int xRad = (int)xSize/2;
	if (includeDividers) {
	  gdImageArc(img, x-1+xRad, y-1+yRad, xSize, ySize, 0, 360, colorcode);
	  gdImageFill(img, x-1+xRad, y-1+yRad, colorcode);
	  gdImageLine(img, x+xSize-1, y-1, x+xSize-1, y+ySize-1, dividerColor);
	} else {
	  gdImageArc(img, x+xRad, y+yRad, xSize, ySize, 0, 360, colorcode);
	  gdImageFill(img, x+xRad, y+yRad, colorcode);
	}
      } else {
	if (includeDividers) {
	  gdImageFilledRectangle(img, x, y, x+xSize, y+ySize-1, colorcode);
	  gdImageLine(img, x+xSize-1, y-1, x+xSize-1, y+ySize-1, dividerColor);
	} else {
	  gdImageFilledRectangle(img, x, y, x+xSize, y+ySize, colorcode);
	}
      }
      x+=xSize;
    }
    y+=ySize;
  }
  
  matrixInfo->ulx = xoffset;
  matrixInfo->uly = yoffset;
  matrixInfo->lrx = featureWidth + xoffset;
  matrixInfo->lry = featureHeight + yoffset;
  matrixInfo->minval = min;
  matrixInfo->maxval = max;
  matrixInfo->dividers = includeDividers;

  return img;
} /* rawmatrix2img */



/*
 * Main
 */
#ifdef MATRIXMAIN
VERBOSE_T verbosity = NORMAL_VERBOSE;
int main (int argc, char **argv) {


  /* Main data structures and corresponding command line inputs */
  gdImagePtr img;
  char* dataFilename = NULL;
  FILE* dataFile = NULL;
  char* discreteMappingFileName = NULL;
  FILE* discreteMappingFile = NULL;
  MATRIX_T* dataMatrix;
  RDB_MATRIX_T* rdbdataMatrix;
  USED_T* usedRegion; /* keep track of free space on the image canvas */
  MTYPE** rawmatrix = NULL;

  int numactualrows = 0;
  int numactualcols = 0;

  /* command line options */
  BOOLEAN_T discrete = FALSE;
  BOOLEAN_T doscalebar = FALSE;
  BOOLEAN_T dorownames = FALSE;
  BOOLEAN_T docolnames = FALSE;
  BOOLEAN_T dodividers = FALSE;
  BOOLEAN_T dodesctext = FALSE;
  BOOLEAN_T useDataRange = TRUE;
  BOOLEAN_T passThroughBlack = FALSE;
  BOOLEAN_T skipformatline = FALSE; /* if selected, assumes that we ARE using RDB format */
  BOOLEAN_T ellipses = FALSE; /* draw ellipses or circles instead of rectangles */
  BOOLEAN_T normalize = FALSE; /* normalize the rows N(0,1) */
  double contrast = DEFAULTCONTRAST;
  int numcolors = DEFAULTNUMCOLORS;
  int colorMap = DEFAULTCOLORMAP;
  int numr = -1; /* number of rows to process */
  int numc = -1; /* number of columsn to process */
  double outliers = 0.0;
  int startr = -1;
  int startc = -1;

  /* the following are given in the format xDIVIDERy */
  char* rangeInput = NULL;
  char* pixsizeInput = NULL;
  char* minsizeInput = NULL;

  /* user-specified range for values represented in the image */
  double min = (double)FLT_MAX;
  double max = (double)FLT_MIN;

  /* user-defined colors */
  char* minColorInput = NULL;
  char* maxColorInput = NULL;
  char* bkgColorInput = NULL;
  char* missingColorInput = NULL;
  colorV_T* minColor = initColorVByName(blue);
  colorV_T* maxColor = initColorVByName(red);
  colorV_T* bkgColor = initColorVByName(white);
  colorV_T* missingColor = initColorVByName(grey);
  DISCRETEMAP_T* discreteMap = NULL;

  /* how big each square in the image is */
  int xpixSize = DEFAULTXPIXSIZE;
  int ypixSize = DEFAULTYPIXSIZE;

  /* minimum image size; default of -1 means 'ignore' */
  int xminSize = -1; 
  int yminSize = -1;

  /* optional additional text information added to image */
  char* descFilename = NULL;
  FILE* descFile;
  MATRIXINFO_T* matrixInfo;
  STRING_LIST_T* rownames = NULL;
  STRING_LIST_T* colnames = NULL;
  STRING_LIST_T* desctext = NULL;

  verbosity = NORMAL_VERBOSE;

  DEBUG_CODE(1, fprintf(stderr, "Done initializing\n"););
  
  /* process command line */
  DO_STANDARD_COMMAND_LINE
    (1,
     DATA_OPTN(1, data, <file> (required),
     	       dataFilename = _OPTION_);
     DATA_OPTN(1, desctext, <file>,
     	       descFilename = _OPTION_);
     DATA_OPTN(1, range, : values assigned to mincolor and maxcolor as min:max (default is data range),
     	       rangeInput = _OPTION_);
     DATA_OPTN(1, con, : contrast (default = 1.0; applies only when not using -range option),
     	       contrast = atof(_OPTION_));
     DATA_OPTN(1, size, : pixel dimensions per value as  x:y (default = 2x2),
	       pixsizeInput = _OPTION_);
     DATA_OPTN(1, numcols, : number of colors (default = 16),
	       numcolors = atoi(_OPTION_));
     DATA_OPTN(1, minsize, : minimum image size as x:y pixels,
	       minsizeInput = _OPTION_);
     DATA_OPTN(1, mincolor, : color used at lowest value (name or r:g:b triplet) (default = blue),
	       minColorInput = _OPTION_);
     DATA_OPTN(1, maxcolor, : color used at highest value (name or r:g:b triplet) (default = red),
	       maxColorInput = _OPTION_);
     DATA_OPTN(1, bkgcolor, : color used as background (name or r:g:b triplet) (default = white),
	       bkgColorInput = _OPTION_);
     DATA_OPTN(1, missingcolor, : color used for missing values (name or r:g:b triplet) (default = grey),
	       missingColorInput = _OPTION_);
     DATA_OPTN(1, map, : color choices from preset maps: overrides min/max colors and -b (default = 0 (none) ),
	       colorMap = atoi(_OPTION_));
     SIMPLE_FLAG_OPTN(1, discrete, : Use discretized mapping of values to colors; use -dmap to assign a mapping file,
     	       discrete);
     DATA_OPTN(1, dmap, <mapping file> : Discrete color mapping file to use for discrete mapping (default = preset),
     	       discreteMappingFileName = _OPTION_);
     DATA_OPTN(1, numr, : Number of rows to process starting from the top of the matrix by default,
	       numr = atoi(_OPTION_));
     DATA_OPTN(1, numc, : Number of columns to process starting from the left edge of the matrix by default,
	       numc = atoi(_OPTION_));
     DATA_OPTN(1, startrow, : Index of the first row to be processed; can combine with numr (default=1),
     	       startr = atoi(_OPTION_));
     DATA_OPTN(1, startcol, : Index of the first column to be processed; can combine with numc (default=1),
     	       startc = atoi(_OPTION_));
     DATA_OPTN(1, trim, : Trim this percent of data extremes when determining data range (only without the -range option),
	       outliers = atof(_OPTION_));
     DATA_OPTN(1, verbose, : Verbosity of the output 1|2|3|4|5 (default=2),
	       verbosity = (VERBOSE_T)atoi(_OPTION_));
     CFLAG_OPTN(1, z, Row-normalize the data to mean 0 and variance 1, normalize = TRUE); 
     CFLAG_OPTN(1, b, Middle of color range is black, passThroughBlack = TRUE);
     CFLAG_OPTN(1, d, Add cell dividers, dodividers = TRUE);
     CFLAG_OPTN(1, s, Add scale bar, doscalebar = TRUE);
     CFLAG_OPTN(1, r, Add row names, dorownames = TRUE);
     CFLAG_OPTN(1, c, Add column names, docolnames = TRUE);
     CFLAG_OPTN(1, f, Data file has a format line, skipformatline = TRUE); 
     CFLAG_OPTN(1, e, Draw ellipses instead of rectangles, ellipses = TRUE); 
     //     NON_SWITCH(1, \n datafile, dataFilename = _OPTION_);
     );


  if (numcolors < MINCOLORS || numcolors > MAXCOLORS) 
    die("Illegal number of colors, must be between %d and %d", MINCOLORS, MAXCOLORS);

  if (minsizeInput != NULL) {
    double parseval1, parseval2;
    parseValuePair(minsizeInput, DIVIDER, &parseval1, &parseval2);
    xminSize = (int)parseval1;
    yminSize = (int)parseval2;
    if (xminSize <= 0 || yminSize <= 0) die ("Illegal values for minimum image size");
  }

  if (rangeInput != NULL) {
    double parseval1, parseval2;
    parseValuePair(rangeInput, DIVIDER, &parseval1, &parseval2);
    min = (double)parseval1;
    max = (double)parseval2;
    DEBUG_CODE(1, fprintf(stderr, "Using %f and %f as min and max values\n", min, max););
    if (min >= max) die("Illegal values for min and max range");
    useDataRange = FALSE;
  }

  if (pixsizeInput != NULL) {
    double parseval1, parseval2;
    parseValuePair(pixsizeInput, DIVIDER, &parseval1, &parseval2);
    xpixSize = (int)parseval1;
    ypixSize = (int)parseval2;
    if (xpixSize <= 0 || ypixSize <= 0) die("Illegal values for x and/or y pixel sizes range %d %d", xpixSize, ypixSize);
    DEBUG_CODE(1, fprintf(stderr, "Pixels set to %d by %d\n", xpixSize, ypixSize););
  }

  if (outliers && rangeInput) {
    die("Cannot specifiy outlier trimming as well as the -range option\n");
  }

  if (outliers < 0.0 || outliers > 100.0) {
    die("Please select an outlier trimming value that is a valid percentage\n");
  }

  if (bkgColorInput != NULL) {
    string2color(bkgColorInput, bkgColor);
    if (bkgColor == 0) die("Illegal background color chosen");
  }
  if (missingColorInput != NULL) {
    string2color(missingColorInput, missingColor);
    if (missingColor == 0) die("Illegal missing color chosen");
  }


  if (startr >= 1) {
    startr--;
  }

  if (startc >= 1) {
    startc--;
  }

  /* read data */
  DEBUG_CODE(1, fprintf(stderr, "Reading data\n"););
  if (!strcmp(dataFilename, "-")) { /* read from stdin */
    rdbdataMatrix = read_rdb_matrix_wmissing(skipformatline, stdin, numr, numc, startr, startc);
  } else {
    if (open_file(dataFilename, "r", FALSE, "data", "the data", &dataFile) == 0) exit(1);
    rdbdataMatrix = read_rdb_matrix_wmissing(skipformatline, dataFile, numr, numc, startr, startc);
    fclose(dataFile);
  }
  dataMatrix = get_raw_matrix(rdbdataMatrix);
  DEBUG_CODE(1, fprintf(stderr, "Done reading\n"););
  numactualrows = get_num_rows(dataMatrix);
  numactualcols = get_num_cols(dataMatrix);
  
  // we allow the user to read zero rows or columns (side effects can be useful)
  // but not if they asked for more than 0.
  if (numactualrows == 0 && numr > 0)
    die("No rows read\n");
  if (numactualcols == 0 && numc > 0)
    die("No columns read\n");

  if (dorownames)
    rownames = get_row_names(rdbdataMatrix);

  if (docolnames)
    colnames = get_col_names(rdbdataMatrix);

  //  if (numr < 0 || numr > numactualrows)
  //    numr = numactualrows;    

  //  if (numc < 0 || numc > numactualcols)
  //    numc = numactualcols;


  /* convert user-defined colors into corresponding colorV_T */
  if (colorMap > 0) { 
    DEBUG_CODE(1, fprintf(stderr, "Using color map %d\n", colorMap););
  } else if (discrete == TRUE) {
    // read the mapping file
    if (discreteMappingFileName != NULL) {
      if (open_file(discreteMappingFileName, "r", FALSE, "discrete", "discrete", &discreteMappingFile) == 0) exit(1);
      discreteMap = readDiscreteMap(discreteMappingFile);
    } else {
      discreteMap = readDiscreteMap(NULL);
    }
  } else {
    if (minColorInput != NULL) {
      string2color(minColorInput, minColor);
      if (minColor == 0) die("Illegal mincolor chosen");
    }
    if (maxColorInput != NULL) {
      string2color(maxColorInput, maxColor);
      if (maxColor == 0) die("Illegal maxcolor chosen");
    }
  }

  if (normalize) {
    zero_mean_matrix_rows(dataMatrix);
    variance_one_matrix_rows(dataMatrix);
  }

  /* read descriptive text if needed */
  if (descFilename != NULL) {
    dodesctext = TRUE;
    if (open_file(descFilename, "r", FALSE, "descriptions", "row descriptions", &descFile) == 0) exit (1);
    desctext = read_string_list(descFile);
    fclose(descFile);
  }
  
  /*******************************************************************
   * starting here is the core code required for drawing an annotated
   * matrix image - the preceding just deals with the command line and
   * data sources
   ******************************************************************/
  usedRegion = initUsedRegion();
  matrixInfo = newMatrixInfo();

  matrixInfo->xblocksize = xpixSize;
  matrixInfo->yblocksize = ypixSize;
  matrixInfo->outliers = outliers;
  matrixInfo->circles = ellipses;
  matrixInfo->numrows = numactualrows;
  matrixInfo->numcols = numactualcols;
  matrixInfo->xminSize = xminSize;
  matrixInfo->yminSize = yminSize;
  matrixInfo->usedRegion = usedRegion;

  DEBUG_CODE(1, dumpMatrixInfo(matrixInfo););
  
  DEBUG_CODE(1, fprintf(stderr, "Building image\n"););
  /* make the image as specified */
  img = matrix2img(dataMatrix, contrast, useDataRange, dodividers, passThroughBlack,
		   min, max,
		   minColor,
		   maxColor,
		   bkgColor,
		   missingColor,
		   colorMap,
		   discreteMap,
		   numcolors, matrixInfo, rawmatrix);
  
  /* add extra goodies: (the order matters because of primitive
     feature placement routine) */
  if (dorownames) addRowLabels(img, rownames, matrixInfo);
  if (dodesctext) addRowLabels(img, desctext, matrixInfo);
  if (docolnames) addColLabels(img, colnames, matrixInfo);
  if (doscalebar) addScaleBar(img, matrixInfo, numcolors);

  // enlarge the canvas if requested (todo: make this a function call)
  if (matrixInfo->xminSize > gdImageSX(img) || matrixInfo->yminSize > gdImageSY(img)) {
    int newxsize = matrixInfo->xminSize > gdImageSX(img) ? matrixInfo->xminSize :  gdImageSX(img);
    int newxplace = matrixInfo->xminSize > gdImageSX(img) ? floor((matrixInfo->xminSize -  gdImageSX(img))/2) :  0;
    int newysize = matrixInfo->yminSize > gdImageSY(img) ? matrixInfo->yminSize :  gdImageSY(img);
    int newyplace = matrixInfo->yminSize > gdImageSY(img) ? floor((matrixInfo->yminSize -  gdImageSY(img))/2) :  0;
    enlargeCanvas(img, newxsize, newysize, newxplace, newyplace);
  }

  /* test: add highlight to some of the image */
  DEBUG_CODE(0, 
  {
    int r = matrixInfo->numrows;
    int c = matrixInfo->numcols;
    gdImagePtr replacedRegion;
    REGION_T* region = newRegion();
    replacedRegion = addHighlight(img, matrixInfo, c/2, c/2+20, r/2, r/2+30, region);
    
    /* replace the highlighted region */
    //    restoreRegion(img, replacedRegion, region);
    gdImageDestroy(replacedRegion);
    free(region);
  }
	     );

  /* output */
  gdImagePng(img, stdout);

  /* clean up */
  gdImageDestroy(img);
  /*  free_rdb_matrix(rdbdataMatrix); */
  free_matrix(dataMatrix);
  free(usedRegion);
  free(matrixInfo);
  free(rawmatrix);

  // clean up color structs.
  free(minColor);
  free(maxColor);
  free(bkgColor); 
  free(missingColor);
  if (discreteMap != NULL) {
    freeDiscreteMap(discreteMap);
  }

  return 0;
}
#endif /* MATRIXMAIN */
/*
 * matrix2png.c
 */
