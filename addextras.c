/*****************************************************************************
 * FILE: addextras.c
 * AUTHOR: Paul Pavlidis
 * CREATE DATE: 2/2001
 * PROJECT: PLOTKIT
 * DESCRIPTION: Add annotations etc to a matrix image.
 *****************************************************************************/

#include "addextras.h"
#include <stdio.h>
#include "locations.h"
#include "string-list.h"
#include "colorscalebar.h"
#include "gd.h"
#include "gdfontt.h"
#include "gdfonts.h"
#include "gdfontl.h"
#include "utils.h"
#include "text2png.h"
#include "matrix2png.h"


/*****************************************************************************
 * addScaleBar
 *****************************************************************************/
void addScaleBar(gdImagePtr img, MATRIXINFO_T* matrixInfo, int numcolors)
{
  int x, y, xoffset, yoffset; /* the offsets are where the actual scale bar starts, exclusive of labels */
  int featureWidth, featureHeight;
  int barWidth, barHeight;
  int xtotaloffset, ytotaloffset;

  BOOLEAN_T vertical = FALSE;
  BOOLEAN_T includeMidVal = FALSE;
  
  if (numcolors > DEFAULTSCALEBARLENGTH) {
    barWidth  = numcolors;
  } else {
    barWidth  = DEFAULTSCALEBARLENGTH;
  }
  barHeight = DEFAULTSCALEBARHEIGHT;

  /* should check to see that the default width is enough. */
  //


  getTotalScaleBarDims(TRUE, includeMidVal, vertical, barWidth, barHeight, 
		       matrixInfo->minval, matrixInfo->maxval, &featureWidth, &featureHeight, &xoffset, &yoffset);

  placeFeature(img,
	       "topleft", TRUE,
	       &x, &y, /* these will contain the positions for the _entire_ scale bar, including labels */
	       matrixInfo->usedRegion,
	       featureWidth, featureHeight,
	       &xtotaloffset, &ytotaloffset
	       );

  matrixInfo->ulx += xtotaloffset;
  matrixInfo->uly += ytotaloffset;
  matrixInfo->lrx += xtotaloffset;
  matrixInfo->lry += ytotaloffset;

  DEBUG_CODE(1, fprintf(stderr, "Adding scale bar at %d %d, %d by %d\n", x, y, barWidth, barHeight););
  drawScaleBar(img, vertical, x+xoffset, y+yoffset, barHeight, barWidth );
  labelScaleBar(img, includeMidVal, vertical, x+xoffset, y+yoffset,  barHeight, barWidth, matrixInfo->minval, matrixInfo->maxval);

} /* addScaleBar */


/*****************************************************************************
 * addRowLabels
 *****************************************************************************/
void addRowLabels(gdImagePtr img, STRING_LIST_T* rowLabels, 
		  MATRIXINFO_T* matrixInfo)
{
  int textWidth;
  int textHeight;
  int linespacing, initX, initY;
  int xoffset, yoffset;
  gdFontPtr font = NULL;
  /* make sure the text and the ysize are _exactly_ the same. Choose the appropriate font, up to large */
  int yBlockSize = matrixInfo->yblocksize;
  if (matrixInfo->dividers) {
    yBlockSize++;
  }
  if (yBlockSize < gdFontTiny->h) die("Can't fit the row label text with this block size (must be at least %d)", (int)gdFontTiny->h);
  else if (yBlockSize < gdFontSmall->h) font = gdFontTiny;
  else if (yBlockSize < gdFontLarge->h) font = gdFontSmall;
  else font = gdFontLarge;
  
  DEBUG_CODE(1, fprintf(stderr, "Max string is %d\n", max_string_length(rowLabels)););
  linespacing = yBlockSize - font->h;
  DEBUG_CODE(1, if(linespacing<0) die("Linespacing is < 0"););

  calcTextDimensions(rowLabels, matrixInfo->numrows, FALSE, 0, linespacing, font, &textWidth, &textHeight); /* we do this again, in stringlist2image */
  DEBUG_CODE(1, fprintf(stderr, "Adding row labels %d %d\n", textWidth, textHeight););

  placeFeature(img, "rightmiddle", TRUE, &initX, &initY, matrixInfo->usedRegion, textWidth + TEXTPADDING, textHeight, &xoffset, &yoffset);

  matrixInfo->ulx += xoffset;
  matrixInfo->uly += yoffset;
  matrixInfo->lrx += xoffset;
  matrixInfo->lry += yoffset;

  stringlist2image(img, rowLabels, matrixInfo->numrows, FALSE, FALSE, TEXTPADDING, linespacing, initX, initY, font);

} /* addRowLabels */


/*****************************************************************************
 * addColLabels
 *****************************************************************************/
void addColLabels(gdImagePtr img, STRING_LIST_T* colLabels, 
		  MATRIXINFO_T* matrixInfo)
{
  int textWidth;
  int textHeight;
  int linespacing, initX, initY;
  int xoffset, yoffset;
  gdFontPtr font = NULL;
  /* make sure the text and the ysize are _exactly_ the same. Choose the appropriate font, up to large */
  int xBlockSize = matrixInfo->xblocksize;
  if (matrixInfo->dividers) {
    xBlockSize++;
  }
  if (xBlockSize < gdFontTiny->h) die("Can't fit the column label text with this block size (must be at least %d)", (int)gdFontTiny->h);
  else if (xBlockSize < gdFontSmall->h) font = gdFontTiny;
  else if (xBlockSize < gdFontLarge->h) font = gdFontSmall;
  else font = gdFontLarge;
  
  DEBUG_CODE(1, fprintf(stderr, "Max string is %d\n", max_string_length(colLabels)););
  linespacing = xBlockSize - font->h;
  DEBUG_CODE(1, if(linespacing<0) die("Linespacing is < 0"););

  calcTextDimensions(colLabels, matrixInfo->numcols, TRUE, 0, linespacing, font, &textWidth, &textHeight); /* we do this again, in stringlist2image */
  placeFeature(img, "topleft", TRUE, &initX, &initY, matrixInfo->usedRegion, textWidth, textHeight + TEXTPADDING*2, &xoffset, &yoffset); /* what's with the x2? */
  DEBUG_CODE(1, fprintf(stderr, "Adding col labels %d %d %d %d\n", textWidth, textHeight, initX, initY););

  matrixInfo->ulx += xoffset;
  matrixInfo->uly += yoffset;
  matrixInfo->lrx += xoffset;
  matrixInfo->lry += yoffset;

  stringlist2image(img, colLabels, matrixInfo->numcols, FALSE, TRUE, TEXTPADDING, linespacing, initX, initY, font);
  
} /* addColLabels */


/*****************************************************************************
 * addHighlight - highlight a region of the matrix, given matrix coords.
 *****************************************************************************/
gdImagePtr addHighlight(gdImagePtr img, 
			MATRIXINFO_T* matrixInfo, 
			int firstx, 
			int lastx, 
			int firsty, 
			int lasty, 
			REGION_T* region)
{
  int highulx, highuly, highlrx, highlry;
  gdImagePtr highlight, highlightedRegion;
  int highlightColor;
  int highlightBorderColorIntensity;
  int divideradd = 0;
  /* try to recover from insane coordinates */
  if (lastx < firstx) {
    int temp;
    temp = lastx;
    lastx = firstx;
    firstx = temp;
  }

  if (lasty < firsty) {
    int temp;
    temp = lasty;
    lastx = firsty;
    firsty = temp;
  }
  
  if (lastx == firstx) lastx++;
  if (lasty == firsty) lasty++;

  if (lastx - firstx > matrixInfo->numcols) {
    firstx = 0;
    lastx = matrixInfo->numcols - 1;
  } 

  if (lasty - firsty > matrixInfo->numrows) {
    firsty = 0;
    lasty = matrixInfo->numrows - 1;
  }
      
  /* calculate coordinates for the region vis the image */
  if (matrixInfo->dividers)  divideradd = 1;
  
  highulx = matrixInfo->ulx + firstx*(matrixInfo->xblocksize + divideradd) - divideradd;
  highuly = matrixInfo->uly + firsty*(matrixInfo->yblocksize + divideradd) - 2*divideradd + 1;
  highlrx = matrixInfo->ulx + lastx*(matrixInfo->xblocksize + divideradd) - 1;
  highlry = matrixInfo->uly + lasty*(matrixInfo->yblocksize + divideradd) - divideradd;
  region->ulx = highulx;
  region->uly = highuly;
  region->lrx = highlrx;
  region->lry = highlry;
  
  /* implement highlight as a blend of white with the region, with a contrasting border */
  highlight = gdImageCreate(highlrx - highulx, highlry - highuly);
  highlightColor = gdImageColorAllocate(highlight, 255, 255, 255); /* highlight color is white */

  /* first copy the original part of the image, so we can put it back */
  highlightedRegion = gdImageCreate(highlrx - highulx, highlry - highuly);
  gdImageCopy(highlightedRegion, img, 0, 0, highulx, highuly, highlrx, highlry);

  gdImageFilledRectangle(highlight, 0, 0, gdImageSX(highlight) - 1, gdImageSY(highlight) - 1, highlightColor);
  gdImageCopyMerge(img, highlight, highulx, highuly, 0, 0, gdImageSX(highlight), gdImageSY(highlight), 50);
  highlightBorderColorIntensity =  chooseContrastingColor(img);
  gdImageRectangle(img, highulx, highuly, highlrx, highlry, gdImageColorClosest(img, highlightBorderColorIntensity, highlightBorderColorIntensity, highlightBorderColorIntensity));
  gdImageDestroy(highlight);

  return highlightedRegion;
} /* addHighlight */



/*****************************************************************************
 * restoreRegion: used after highlighting a region, and we want to
 * restore it.
 *****************************************************************************/
void restoreRegion(gdImagePtr img, gdImagePtr cachedImg, REGION_T* regionInfo) 
{
  if (img == NULL || cachedImg == NULL) die("Can't restore: null pointer");
  gdImageCopy(img, cachedImg, regionInfo->ulx, regionInfo->uly, 0, 0,
	      regionInfo->lrx - regionInfo->ulx, regionInfo->lry - regionInfo->uly);
} /* restoreRegion */


/*
 * addextras.c
 */









