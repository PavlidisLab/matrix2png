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
  int numColors = gdImageColorsTotal(img) - 1; /* ignore the background color */
  int width, height;

  if (vertical) {
    width = thickness;
    height = length;
  } else {
    width = length;
    height = thickness;
  }

  if (numColors <= 0)  die("Attempt to draw scalebar with insufficient colors allocated\n");
  if (width < 1 || height < 1)  die("Attempt to draw scalebar with insufficient thickness or length\n");
  if (xStart < 0 || yStart <0 || xStart > gdImageSX(img) || yStart > gdImageSY(img) ) die("Attempt to draw scalebar outside of image\n");
  if (xStart + width > gdImageSX(img) || yStart+ height > gdImageSY(img)) die("Scalebar will exceed image bounds\n");
  if (length < numColors) die("Scalebar is too narrow to show all values\n");
  
} /* checkScaleBarDims */



/*****************************************************************************
 *  drawScaleBar: create just the image part of a scale bar.							      
 *****************************************************************************/
void drawScaleBar (
	       gdImagePtr img,
	       BOOLEAN_T vertical,
	       int xStart, /* all meas in pixels*/
	       int yStart,
	       int thickness,
	       int length
	       )
{
  double blockLength;
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
 
  blockLength = ((double)length/(numColors - NUMRESERVEDCOLORS));
  if (blockLength < 1.0) blockLength = 1.0;
  
  DEBUG_CODE(1, fprintf(stderr, "Block length is %f\n", blockLength););
  
  x = (double)xStart;
  y = (double)yStart;
  for (i = NUMRESERVEDCOLORS; i<numColors; i++) {
    if (vertical) { /* high values at the top */
      gdImageFilledRectangle(img, xStart, y, xStart + thickness, y + blockLength, numColors + NUMRESERVEDCOLORS - 1 - i);
      y+=blockLength;
    } else { /* high values at the right */
      gdImageFilledRectangle(img, x, yStart, x + blockLength, yStart + thickness, i);
      x+=blockLength;
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
		    int scaleBarxStart, /* all meas in pixels*/
		    int scaleBaryStart,
		    int scaleBarthickness,
		    int scaleBarlength,
		    double scaleMin,
		    double scaleMax
		    )
{
  char leftLabel [25];
  char rightLabel [25];
  char middleLabel [25];
  int r,g,b;
  int textIntensity;

  checkScaleBarDims(img, vertical, scaleBarxStart, scaleBaryStart, scaleBarthickness, scaleBarlength);

  /* make sure there is space next to the scale bar for the labels */
  if (scaleBarxStart < MINLABELWIDTH) die ("Insufficient room for left hand scale bar label\n");
  if (scaleBarxStart + scaleBarlength + MINLABELWIDTH > gdImageSX(img)) die("Insufficient room for right hand scale bar label\n");
  if (includemiddleval) {
    if (scaleBaryStart < LABELHEIGHT) die ("Insufficient room for middle scale bar label\n");
  }

  /* convert values to strings */
  sprintf(leftLabel,"%.2f", scaleMin);
  sprintf(rightLabel,"%.2f", scaleMax);
  sprintf(middleLabel,"%.2f", (scaleMax - scaleMin )/ 2);
  
  /* figure out if black or white text should be used, depending on
     the background color */
  r = gdImageRed(img, 0); /* Note that these are documented as gdImageColorRed etc - maybe it is so in the latest version of gd? */
  g = gdImageGreen(img, 0);
  b = gdImageBlue(img, 0);
  if (r < 128 && g < 128 && b < 128) {
    textIntensity = 255; /* use white */
  } else {
    textIntensity = 0; /* use black */
  }


  if (vertical) {
    /* bottom label */
    gdImageString(img, LABELFONT, scaleBarxStart, scaleBaryStart + scaleBarlength + PADDING, 
		  leftLabel, gdImageColorClosest(img, textIntensity,  textIntensity,  textIntensity) );

    /* top label */
    gdImageString(img, LABELFONT, scaleBarxStart, scaleBaryStart - LABELHEIGHT - PADDING,
		  rightLabel, gdImageColorClosest(img, textIntensity, textIntensity, textIntensity) );

    /* middle label */
    if (includemiddleval) {
      gdImageString(img, LABELFONT, scaleBarxStart - CHARWIDTH*strlen(middleLabel) - PADDING, scaleBaryStart + scaleBarlength/2 - LABELHEIGHT,
		    middleLabel, gdImageColorClosest(img, textIntensity, textIntensity, textIntensity) );
    }
  } else {
    /* left label */
    gdImageString(img, LABELFONT, scaleBarxStart -  CHARWIDTH*strlen(leftLabel)/2, scaleBaryStart - LABELHEIGHT, 
		  leftLabel, gdImageColorClosest(img, textIntensity,  textIntensity,  textIntensity) );
    /* right label */
    gdImageString(img, LABELFONT, scaleBarxStart + scaleBarlength -  CHARWIDTH*strlen(rightLabel)/2, scaleBaryStart - LABELHEIGHT,
		  rightLabel, gdImageColorClosest(img, textIntensity, textIntensity, textIntensity) );
    
    /* middle label */
    if (includemiddleval) {
      gdImageString(img, LABELFONT, scaleBarxStart + scaleBarlength/2 - CHARWIDTH*strlen(middleLabel)/2, scaleBaryStart - LABELHEIGHT,
		    middleLabel, gdImageColorClosest(img, textIntensity, textIntensity, textIntensity) );
    }
  }
  

} /* labelScaleBar */





/* 
 * colorscalebar.c
 */







