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
void addScaleBar(gdImagePtr img, MATRIXINFO_T* matrixInfo)
{
  int x, y, xoffset, yoffset; /* the offsets are where the actual scale bar starts, exclusive of labels */
  int featureWidth, featureHeight;
  int barWidth, barHeight;
  int xtotaloffset, ytotaloffset;
  double blocksize;

  
  // The following are 'reasonable' settings. For discrete mappings,
  // the text is always vertical for horizontal scale bars. (but here
  // we set the scale bar to be vertical for discrete.
  BOOLEAN_T vertical = FALSE || matrixInfo->discreteMap != NULL; // todo: make this user-settable
  BOOLEAN_T includeMidVal = FALSE; // todo: make this user-settable (?)
  BOOLEAN_T rotateLabels = FALSE; // option only applies if scalebar is horizontal : todo: make this user-settable.

  DEBUG_CODE(1, fprintf(stderr, "--Adding scale bar\n"););

  /* calclute the space needed for the color bar */
  barWidth  = DEFAULTSCALEBARLENGTH;
  if (matrixInfo->numColors > DEFAULTSCALEBARLENGTH) {
    barWidth  = matrixInfo->numColors; // allot one pixel per color, please.
  } else if (matrixInfo->discreteMap) { // allot one font width/height per value please.
    int SKIPDEFAULT = 1;
    checkDiscreteUsedValues(matrixInfo); 
    if (matrixInfo->discreteMap->default_used) {
      SKIPDEFAULT = 0;
    }
    if (!vertical) {
      barWidth = (matrixInfo->discreteMap->count - SKIPDEFAULT) * (CHARWIDTH + 1);
    } else if (vertical) {
      barWidth = (matrixInfo->discreteMap->count - SKIPDEFAULT)  * (LABELHEIGHT + 1) ;
    }
  }
  barHeight = DEFAULTSCALEBARHEIGHT;

  /* get the total size including labels and padding */
  getTotalScaleBarDims(TRUE, includeMidVal, vertical, rotateLabels, barWidth, barHeight, 
			 matrixInfo, &featureWidth, &featureHeight, &xoffset, &yoffset);

  placeFeature(img,
	       "topleft", // todo: make this user-settable.
	       TRUE,  // align.
	       &x, &y, /* these will contain the positions for the _entire_ scale bar, including labels */
	       matrixInfo->usedRegion,
	       featureWidth+xoffset, featureHeight,
	       &xtotaloffset, &ytotaloffset // this is how much we had to move everything
	       );

  matrixInfo->ulx += xtotaloffset;
  matrixInfo->uly += ytotaloffset;
  matrixInfo->lrx += xtotaloffset;
  matrixInfo->lry += ytotaloffset;

  DEBUG_CODE(1, fprintf(stderr, "Adding scale bar at %d %d, %d by %d, offset %d %d\n", x, y, featureWidth, featureHeight, xoffset, yoffset););
  drawScaleBar(img, vertical, x+xoffset, y+yoffset, barHeight, barWidth, &blocksize, matrixInfo);
  labelScaleBar(img, includeMidVal, vertical, rotateLabels, x+xoffset, y+yoffset, barHeight, barWidth, blocksize, matrixInfo);

} /* addScaleBar */


/*****************************************************************************
 * addRowLabels - put row label text on a picture.
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

  // todo: make location user-settable.
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

  // todo: make location user-settable.
  placeFeature(img, "topleft", TRUE, &initX, &initY, matrixInfo->usedRegion, textWidth, textHeight + TEXTPADDING*2, &xoffset, &yoffset); /* what's with the x2? */
  DEBUG_CODE(1, fprintf(stderr, "Adding col labels %d %d %d %d\n", textWidth, textHeight, initX, initY););

  matrixInfo->ulx += xoffset;
  matrixInfo->uly += yoffset;
  matrixInfo->lrx += xoffset;
  matrixInfo->lry += yoffset;

  stringlist2image(img, colLabels, matrixInfo->numcols, FALSE, TRUE, TEXTPADDING, linespacing, initX, initY, font);
  
} /* addColLabels */


/*****************************************************************************
 * addHighlight - highlight a region of the matrix, given matrix
 * coords. (This was experimental and is not accessible to the
 * user. todo: implement this as an option if anybody cares.
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
 * restore it. See addHighlight.
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









