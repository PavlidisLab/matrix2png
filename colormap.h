/*****************************************************************************
 * FILE: colormap.h
 * AUTHOR: Paul Pavlidis
 * CREATE DATE: 5/2001
 * PROJECT: PLOTKIT
 * DESCRIPTION: Allocation of colors according to predefined maps.
 *****************************************************************************/
#ifndef COLORMAP_H
#define COLORMAP_H
#include "gd.h"
#include "utils.h"
#include "colors.h"
#include "matrix2png.h"

/* color map definitions: these are the colors which we pass through, evenly spaced. */
/* black body color map (black, darkred, orange, yellow, white) */
/* spectrum (red, orange, yellow, green, blue, violet) */
//#define blackbody {"black", "darkred", "orange", "yellow", "white"}
//#define spectrum {"black", "red", "orange", "yellow", "violet"};
//#define NUMMAPCOLS 5

/*****************************************************************************
 * 
 *****************************************************************************/
char** getColorMap (int colorMap);

void allocateColorMap (
		       gdImagePtr img,
		       colorV_T* backgroundColor,
		       colorV_T* missingColor,
		       int colorMap,
		       MATRIXINFO_T* matrixInfo
		       );

#endif
