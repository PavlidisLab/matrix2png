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
 *  Calculate the total dimensions of a finished scale bar
 *****************************************************************************/
void getTotalScaleBarDims(BOOLEAN_T addLabels,
			  BOOLEAN_T includeMidVal,
			  BOOLEAN_T vertical,
			  int barLength,
			  int barThickness,
			  double scaleMin,
			  double scaleMax,
			  int *width,
			  int *height,
			  int *widthoffset,
			  int *heightoffset)
{
  char leftLabel [MAXLABELLENGTH];
  char rightLabel [MAXLABELLENGTH];
  char middleLabel [MAXLABELLENGTH];

  sprintf(leftLabel, LABELFORMAT, scaleMin);
  sprintf(rightLabel, LABELFORMAT, scaleMax);
  sprintf(middleLabel, LABELFORMAT, (scaleMax - scaleMin )/ 2);

  if (vertical) {
    if (addLabels) {
      int maxlength;
      /* figure out maximum string length */
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
      *width = barThickness + maxlength + PADDING*2;
      *height = barLength + PADDING*2;
      *widthoffset = maxlength + PADDING;
      *heightoffset = PADDING;
    } else {
      *width = barThickness + PADDING*2;
      *height = barLength + PADDING*2;
      *widthoffset = PADDING;
      *heightoffset = PADDING;
    }
  } else { /* horizontal */
    if (addLabels) {
      *width = barLength + CHARWIDTH*strlen(leftLabel)/2 + CHARWIDTH*strlen(rightLabel)/2;
      *height = barThickness + LABELHEIGHT + PADDING*2;
      *widthoffset = CHARWIDTH*strlen(leftLabel)/2;
      *heightoffset = LABELHEIGHT + PADDING;
    } else {
      *width = barLength + PADDING*2;
      *height = barThickness + PADDING*2;
      *widthoffset = PADDING;
      *heightoffset = PADDING;
    }
  }

  DEBUG_CODE(1, fprintf(stderr, "Total scale bar dimensions will be %d by %d, labels is %d, vertical is %d\n", *width, *height, (int)addLabels, (int)vertical););
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
  
  char leftLabel [MAXLABELLENGTH];
  char rightLabel [MAXLABELLENGTH];
  char middleLabel [MAXLABELLENGTH];
  /*  int r,g,b; */
  int textIntensity;

  checkScaleBarDims(img, vertical, scaleBarxStart, scaleBaryStart, scaleBarthickness, scaleBarlength);

  /* make sure there is space next to the scale bar for the labels */
  //  if (scaleBarxStart < MINLABELWIDTH) die ("Insufficient room for left hand scale bar label\n");
  //  if (scaleBarxStart + scaleBarlength + MINLABELWIDTH > gdImageSX(img)) die("Insufficient room for right hand scale bar label\n");
  if (includemiddleval) {
    //    if (scaleBaryStart < LABELHEIGHT) die ("Insufficient room for middle scale bar label\n");
  }

  /* convert values to strings */
  sprintf(leftLabel, LABELFORMAT, scaleMin);
  sprintf(rightLabel, LABELFORMAT, scaleMax);
  sprintf(middleLabel, LABELFORMAT, (scaleMax - scaleMin )/ 2);

  textIntensity = chooseContrastingColor(img);

  /* note that the casts to unsigned char* are just to avoid compiler warnings */
  if (vertical) {
    /* bottom label */
    gdImageString(img, LABELFONT, scaleBarxStart, scaleBaryStart + scaleBarlength + PADDING, 
		  (unsigned char*)leftLabel, gdImageColorClosest(img, textIntensity,  textIntensity,  textIntensity) );

    /* top label */
    gdImageString(img, LABELFONT, scaleBarxStart, scaleBaryStart - LABELHEIGHT - PADDING,
		   (unsigned char*)rightLabel, gdImageColorClosest(img, textIntensity, textIntensity, textIntensity) );

    /* middle label */
    if (includemiddleval) {
      gdImageString(img, LABELFONT, scaleBarxStart - CHARWIDTH*strlen(middleLabel) - PADDING, scaleBaryStart + scaleBarlength/2 - LABELHEIGHT,
		     (unsigned char*)middleLabel, gdImageColorClosest(img, textIntensity, textIntensity, textIntensity) );
    }
  } else {
    /* left label */
    DEBUG_CODE(1, fprintf(stderr, "Adding left label to %d, %d\n",  (int)(scaleBarxStart -  CHARWIDTH*strlen(leftLabel)/2), (int)(scaleBaryStart - LABELHEIGHT) ););
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
  

} /* labelScaleBar */





/* 
 * colorscalebar.c
 */







