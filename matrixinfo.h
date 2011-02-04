/* Copyright (c) Columbia University */
#ifndef MATRIXINFO_H
#define MATRIXINFO_H

#include "utils.h"
#include "matrix.h"
#include "hash.h"
#include "colors.h"
#include "string-list.h"
#include "locations.h"

#define DEFAULT_DISCRETE_LABEL_BUFSIZE 25

/* struct defining a discrete value to color mapping */
typedef struct discretemap_t 
{
  colorV_T**     colors;
  HASHTABLE_T*   mapping; // mapping of integer values to
			  // colors. (keys are stringified values),
			  // read from the file. This is a map of
			  // "values" to "consecints". Could be
			  // better!
  int*           values; // the Values that are expected to be present
			 // in the data.
  int*           consecints; // We need this as data for the hashes.
  HASHTABLE_T*   usedValues; // Which values are actually used -
			     // needed to make the scale bar
			     // correctly. This is a map of 'value' to a boolean, basically.
  BOOLEAN_T      default_used;  // whether the default is needed (in scale bar mostly)
  STRING_LIST_T* labels; // text labels for the scale bar.
  int            count;
  int            maxcount;
  colorV_T*      default_colorcode; // the color for 'default' values.
  char           defaultlabel[DEFAULT_DISCRETE_LABEL_BUFSIZE];
} DISCRETEMAP_T;

/* data structure for storing the details of the matrix image */
typedef struct matrixinfo_t  {
  int ulx; /* upper left x etc */
  int uly;
  int lrx;
  int lry;
  int numrows; 
  int numcols; 
  int rowsToUse;
  int colsToUse;
  int numColors;
  int xblocksize;
  int yblocksize;
  double minval;
  double maxval;
  double outliers;
  int xminSize; // todo: not needed
  int yminSize; // todo: not needed
  char* fontName; // if supported
  DISCRETEMAP_T* discreteMap;
  USED_T* usedRegion;
  BOOLEAN_T dividers;
  BOOLEAN_T circles;
  BOOLEAN_T rowLabelsLeft;
  BOOLEAN_T colLabelsBottom;
  BOOLEAN_T reverseJustification; // row label text alignment opposite of default?
  MATRIX_T* matrix; /* pointer to the matrix itself */
} MATRIXINFO_T;



#endif /* MATRIXINFO_H */
