/*****************************************************************************
 * FILE: addextras.h
 * AUTHOR: Paul Pavlidis
 * CREATE DATE: 2/2001
 * PROJECT: PLOTKIT
 * DESCRIPTION: Add annotations etc to a matrix image.
 *****************************************************************************/

#ifndef ADDEXTRAS_H
#define ADDEXTRAS_H
#include "string-list.h"
#include "gd.h"
#include "locations.h"
#include "matrix2png.h"

/*****************************************************************************
 * addScaleBar
 *****************************************************************************/
void addScaleBar(gdImagePtr img, 
		 USED_T* usedRegion, 
		 MATRIXINFO_T* matrixInfo,
		 int numcolors);


/*****************************************************************************
 * addRowLabels
 *****************************************************************************/
void addRowLabels(gdImagePtr img, 
		  STRING_LIST_T* rowLabels, 
		  USED_T* usedRegion, 
		  int yBlockSize, 
		  MATRIXINFO_T* matrixInfo);


/*****************************************************************************
 * addColLabels
 *****************************************************************************/
void addColLabels(gdImagePtr img, 
		  STRING_LIST_T* colLabels, 
		  USED_T* usedRegion, 
		  int xBlockSize, 
		  MATRIXINFO_T* matrixInfo);


/*****************************************************************************
 * addHighlight - highlight a region of the matrix, return the
 * original highlighted region so it can be put back if desired.
 *****************************************************************************/
gdImagePtr addHighlight(gdImagePtr img, 
			MATRIXINFO_T* matrixInfo,
			int firstx, /* these are the coordinates _in the matrix_ of the region to be highlighted */
			int lastx, 
			int firsty, 
			int lasty,
			REGION_T *region);



/*****************************************************************************
 * restoreRegion: used after highlighting a region, and we want to
 * restore it.
 *****************************************************************************/
void restoreRegion(gdImagePtr img, gdImagePtr cachedImg, REGION_T* regionInfo);


#endif /*ADDEXTRAS_H*/
