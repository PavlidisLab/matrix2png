/*****************************************************************************
 * FILE: colorscalebar.c
 * AUTHOR: Paul Pavlidis
 * CREATE DATE: 2/2001
 * PROJECT: PLOTKIT
 * DESCRIPTION: Functions to draw color scale bars on images.
 *****************************************************************************/

#include "gd.h"
#include "colorscalebar.h"
#include "colors.h"
#include "utils.h"
#include <string.h>
#include <stdio.h>
#include "matrix2png.h"

/*****************************************************************************
 * checkScaleBarDims: make sure a scale bar is legal
 *****************************************************************************/
void checkScaleBarDims (
			gdImagePtr img,
			BOOLEAN_T vertical,
			int xStart, /* all meas in pixels*/
			int yStart,
			int thickness,
			int length
			)
{
  int numColors = gdImageColorsTotal(img) - 1 - NUMRESERVEDCOLORS; /* ignore the background color etc. */
  int width, height;

  if (vertical) {
    width = thickness;
    height = length;
  } else {
    width = length;
    height = thickness;
  }
  
  if (numColors <= 0)  die("Attempt to draw scalebar with no colors allocated\n");
  if (width < 1 || height < 1)  die("Attempt to draw scalebar with insufficient thickness or length\n");
  if (xStart < 0 || yStart <0 || xStart > gdImageSX(img) || yStart > gdImageSY(img) ) die("Attempt to draw scalebar outside of image\n");
  if (xStart + width > gdImageSX(img) || yStart+ height > gdImageSY(img)) die("Scalebar will exceed image bounds\n");
  if (length < numColors) die("Scalebar is too narrow (%d) to show all %d values\n", length, numColors);
  
} /* checkScaleBarDims */


/*****************************************************************************
 *  Calculate the total dimensions of a finished scale bar
 *****************************************************************************/
void getTotalScaleBarDims(BOOLEAN_T addLabels,
			  BOOLEAN_T includeMidVal,
			  BOOLEAN_T vertical,
			  BOOLEAN_T rotatelabels,
			  int barLength,
			  int barThickness,
			  MATRIXINFO_T* matrixInfo,
			  int *width,
			  int *height,
			  int *widthoffset,
			  int *heightoffset)
{
  char leftLabel [MAXLABELLENGTH];
  char rightLabel [MAXLABELLENGTH];
  char middleLabel [MAXLABELLENGTH];
  int maxlength = 0;

  /* get the strings we need for labeling. The edge labels get special status. */
  if (matrixInfo->discreteMap == NULL) {
    double scaleMin = matrixInfo->minval;
    double scaleMax = matrixInfo->maxval;
    /* figure out maximum string length */
    sprintf(leftLabel, LABELFORMAT, scaleMin);
    sprintf(rightLabel, LABELFORMAT, scaleMax);
    sprintf(middleLabel, LABELFORMAT, (scaleMax - scaleMin )/ 2);
    if (includeMidVal) {
      if (strlen(leftLabel) >= strlen(rightLabel) && strlen(leftLabel) >=  strlen(middleLabel)) 
	maxlength = strlen(leftLabel);
      else if (strlen(rightLabel) >= strlen(leftLabel) && strlen(rightLabel) >=  strlen(middleLabel)) 
	maxlength = strlen(rightLabel);
      else maxlength = strlen(middleLabel);
    } else {
      if (strlen(leftLabel) >= strlen(rightLabel))
	maxlength = strlen(leftLabel);
      else
	maxlength = strlen(rightLabel);
    }
  } else {
    strcpy(leftLabel, get_nth_string(0, matrixInfo->discreteMap->labels));
    strcpy(rightLabel, get_nth_string(matrixInfo->discreteMap->count - 1, matrixInfo->discreteMap->labels));
    maxlength = max_string_length(matrixInfo->discreteMap->labels);
    if ((int)strlen(matrixInfo->discreteMap->defaultlabel) > maxlength)
      maxlength = strlen(matrixInfo->discreteMap->defaultlabel);
  }

  DEBUG_CODE(1, fprintf(stderr, "Longest string is %d, charwidth %d\n", maxlength, CHARWIDTH););

  if (vertical) {
    if (addLabels) {
      *width = barThickness + maxlength*CHARWIDTH + PADDING*2;
      *height = barLength + PADDING*2;
      *widthoffset = PADDING;
      *heightoffset = PADDING;
    } else {
      *width = barThickness + PADDING*2;
      *height = barLength + PADDING*2;
      *widthoffset = PADDING;
      *heightoffset = PADDING;
    }
  } else { /* horizontal */
    if (addLabels) {
      if (rotatelabels || matrixInfo->discreteMap != NULL) {
	*width = barLength;
	*height = barThickness + maxlength*CHARWIDTH + PADDING*3;
	*widthoffset = LABELHEIGHT;
	*heightoffset = maxlength*CHARWIDTH + PADDING*2;
      } else {
	*width = barLength + CHARWIDTH*strlen(leftLabel)/2 + CHARWIDTH*strlen(rightLabel)/2;
	*height = barThickness + LABELHEIGHT + PADDING*2;
	*widthoffset = CHARWIDTH*strlen(leftLabel)/2;
	*heightoffset = LABELHEIGHT + PADDING;
      }
    } else {
      *width = barLength + PADDING*2;
      *height = barThickness + PADDING*2;
      *widthoffset = PADDING;
      *heightoffset = PADDING;
    }
  }

  DEBUG_CODE(1, fprintf(stderr, "Total scale bar dimensions will be %d wide by %d high, labels is %d, vertical is %d, rotatelabels is %d\n", *width, *height, (int)addLabels, (int)vertical, (int)rotatelabels););
} /* getTotalScaleBarDims */



/*****************************************************************************
 *  drawScaleBar: create just the image part of a scale bar.							      
 *****************************************************************************/
void drawScaleBar (
	       gdImagePtr img,
	       BOOLEAN_T vertical,
	       int xStart, /* all meas in pixels*/
	       int yStart,
	       int thickness,
	       int length,
	       double* blockLength
	       )
{
  int i;
  double x, y;
  int numColors = gdImageColorsTotal(img);
  int width, height;

  if (vertical) {
    width = thickness;
    height = length;
  } else {
    width = length;
    height = thickness;
  }
  
  checkScaleBarDims(img, vertical, xStart, yStart, thickness, length);
 
  *blockLength = ((double)length/(numColors - NUMRESERVEDCOLORS));
  if (*blockLength < 1.0) *blockLength = 1.0;
  
  DEBUG_CODE(1, fprintf(stderr, "Block length is %f\n", *blockLength););
  
  x = (double)xStart;
  y = (double)yStart;

  for (i = NUMRESERVEDCOLORS; i<numColors; i++) {
    if (vertical) { /* high values at the top */
      gdImageFilledRectangle(img, xStart, y, xStart + thickness, y + *blockLength, numColors + NUMRESERVEDCOLORS - 1 - i);
      y+= *blockLength;
    } else { /* high values at the right */
      gdImageFilledRectangle(img, x, yStart, x + *blockLength, yStart + thickness, i);
      x+= *blockLength;
    }
  }

} /* drawScaleBar */


/*****************************************************************************
 * labelScaleBar: add text numerical labels to a scale bar							      
 *****************************************************************************/
void labelScaleBar (
		    gdImagePtr img,
		    BOOLEAN_T includemiddleval,
		    BOOLEAN_T vertical,
		    BOOLEAN_T rotatelabels,
		    int scaleBarxStart, /* all meas in pixels*/
		    int scaleBaryStart,
		    int scaleBarthickness,
		    int scaleBarlength,
		    double blocksize,
		    MATRIXINFO_T* matrixInfo
		    )
{
  
  char leftLabel [MAXLABELLENGTH];
  char rightLabel [MAXLABELLENGTH];
  char middleLabel [MAXLABELLENGTH];
  /*  int r,g,b; */
  int textIntensity;
  double scaleMin = matrixInfo->minval;
  double scaleMax = matrixInfo->maxval;

  checkScaleBarDims(img, vertical, scaleBarxStart, scaleBaryStart, scaleBarthickness, scaleBarlength);

  /* make sure there is space next to the scale bar for the labels */
  //  if (scaleBarxStart < MINLABELWIDTH) die ("Insufficient room for left hand scale bar label\n");
  //  if (scaleBarxStart + scaleBarlength + MINLABELWIDTH > gdImageSX(img)) die("Insufficient room for right hand scale bar label\n");
  if (includemiddleval) {
    //    if (scaleBaryStart < LABELHEIGHT) die ("Insufficient room for middle scale bar label\n");
  }

  textIntensity = chooseContrastingColor(img);
  
  if (matrixInfo->discreteMap != NULL) {
    int i;
    int numvals = matrixInfo->discreteMap->count;
    if (vertical) {
      // add the bottom label for default values
      gdImageString(img, LABELFONT, scaleBarxStart + scaleBarthickness + PADDING, scaleBaryStart + scaleBarlength - LABELHEIGHT*1.5, 
		    (unsigned char*)matrixInfo->discreteMap->defaultlabel,  gdImageColorClosest(img, textIntensity,  textIntensity,  textIntensity) );
      for (i=0; i<numvals; i++) {
	gdImageString(img, LABELFONT, scaleBarxStart + scaleBarthickness + PADDING, scaleBaryStart + LABELHEIGHT/2 + blocksize*i, 
		      (unsigned char*)get_nth_string(numvals - i - 1, matrixInfo->discreteMap->labels), gdImageColorClosest(img, textIntensity,  textIntensity,  textIntensity) );
      }
    } else { // horizontal
	gdImageStringUp(img, LABELFONT, scaleBarxStart +  LABELHEIGHT/2, scaleBaryStart - PADDING, 
			(unsigned char*)matrixInfo->discreteMap->defaultlabel, gdImageColorClosest(img, textIntensity,  textIntensity,  textIntensity) );
      for (i=0; i<numvals; i++) {
	gdImageStringUp(img, LABELFONT, scaleBarxStart +  LABELHEIGHT/2 + blocksize*(i+1), scaleBaryStart - PADDING, 
		      (unsigned char*)get_nth_string(i, matrixInfo->discreteMap->labels), gdImageColorClosest(img, textIntensity,  textIntensity,  textIntensity) );
      }
    }
  } else {
    /* convert values to strings */
    sprintf(leftLabel, LABELFORMAT, scaleMin);
    sprintf(rightLabel, LABELFORMAT, scaleMax);
    sprintf(middleLabel, LABELFORMAT, (scaleMax - scaleMin )/ 2);
    
    /* note that the casts to unsigned char* are just to avoid compiler warnings */
    if (vertical) {
      /* bottom label */
      gdImageString(img, LABELFONT, scaleBarxStart + scaleBarthickness + PADDING, scaleBaryStart + scaleBarlength - LABELHEIGHT, 
		    (unsigned char*)leftLabel, gdImageColorClosest(img, textIntensity,  textIntensity,  textIntensity) );
      
      /* top label */
      gdImageString(img, LABELFONT, scaleBarxStart + scaleBarthickness + PADDING, scaleBaryStart,
		    (unsigned char*)rightLabel, gdImageColorClosest(img, textIntensity, textIntensity, textIntensity) );
      
      /* middle label */
      if (includemiddleval) {
	gdImageString(img, LABELFONT, scaleBarxStart + scaleBarthickness + PADDING, scaleBaryStart + scaleBarlength/2 - LABELHEIGHT,
		      (unsigned char*)middleLabel, gdImageColorClosest(img, textIntensity, textIntensity, textIntensity) );
      }
    } else { // horizontal
      /* left label */
      if (rotatelabels) {
	gdImageStringUp(img, LABELFONT, scaleBarxStart, scaleBaryStart - LABELHEIGHT + PADDING*2, 
			(unsigned char*)leftLabel, gdImageColorClosest(img, textIntensity,  textIntensity,  textIntensity) );
	/* right label */
	gdImageStringUp(img, LABELFONT, scaleBarxStart + scaleBarlength - LABELHEIGHT, scaleBaryStart - LABELHEIGHT + PADDING*2,
			(unsigned char*)rightLabel, gdImageColorClosest(img, textIntensity, textIntensity, textIntensity) );
	
	/* middle label */
	if (includemiddleval) {
	  gdImageStringUp(img, LABELFONT, scaleBarxStart + scaleBarlength/2 - LABELHEIGHT/2, scaleBaryStart - LABELHEIGHT + PADDING*2,
			  (unsigned char*)middleLabel, gdImageColorClosest(img, textIntensity, textIntensity, textIntensity) );
	}
      } else {
	gdImageString(img, LABELFONT, scaleBarxStart -  CHARWIDTH*strlen(leftLabel)/2, scaleBaryStart - LABELHEIGHT, 
		      (unsigned char*)leftLabel, gdImageColorClosest(img, textIntensity,  textIntensity,  textIntensity) );
	/* right label */
	gdImageString(img, LABELFONT, scaleBarxStart + scaleBarlength -  CHARWIDTH*strlen(rightLabel)/2, scaleBaryStart - LABELHEIGHT,
		      (unsigned char*)rightLabel, gdImageColorClosest(img, textIntensity, textIntensity, textIntensity) );
	
	/* middle label */
	if (includemiddleval) {
	  gdImageString(img, LABELFONT, scaleBarxStart + scaleBarlength/2 - CHARWIDTH*strlen(middleLabel)/2, scaleBaryStart - LABELHEIGHT,
			(unsigned char*)middleLabel, gdImageColorClosest(img, textIntensity, textIntensity, textIntensity) );
	}
      }
    }
  }
} /* labelScaleBar */
  


/* 
 * colorscalebar.c
 */



