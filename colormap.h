/*****************************************************************************
 * FILE: colormap.h
 * AUTHOR: Paul Pavlidis
 * CREATE DATE: 5/2001
 * PROJECT: PLOTKIT
 * DESCRIPTION: Allocation of colors according to predefined maps.
 * Copyright (c) Columbia University
 *****************************************************************************/
#ifndef COLORMAP_H
#define COLORMAP_H
#include "gd.h"
#include "utils.h"
#include "colors.h"
#include "matrix2png.h"

#define MAXCOLORMAP 18 // how may color maps we have defined.

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
