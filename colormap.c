/*****************************************************************************
 * FILE: colormap.c
 * AUTHOR: Paul Pavlidis
 * CREATE DATE: 5/2001
 * PROJECT: PLOTKIT
 * DESCRIPTION: Allocation of colors according to predefined maps.
 *****************************************************************************/

#include "gd.h"
#include "utils.h"
#include "colors.h"
#include "colormap.h"

/* globals */
char *blackbody[] = {"black", "darkred", "orange", "yellow", "white", NULL};
char *spectrum[] = {"black", "red", "orange", "yellow", "violet", NULL};

/*****************************************************************************
 * 
 *****************************************************************************/
void allocateColorMap (
		       gdImagePtr img,
		       color_T backgroundColor,
		       int colorMap,
		       int numColors
		       )
{
  double r,g,b; /* color components */
  int startRed = 0;
  int startGreen = 0;
  int startBlue = 0;
  int backgroundRed = 0;
  int backgroundGreen = 0;
  int backgroundBlue = 0;
  int endRed = 0;
  int endGreen = 0;
  int endBlue = 0;
  int numcolorsPerStep = 0;
  char** colorMapList = NULL;
  color_T minColor = 0;
  color_T maxColor = 0;
  char* maxColorString;
  char* minColorString;
  
  int i; /* counter */

  /* step sizes for moving from low to high values in the scale */
  double redStepSize = 0.0;
  double greenStepSize = 0.0;
  double blueStepSize = 0.0;

  DEBUG_CODE(1, fprintf(stderr, "Allocating map\n"););
  colorMapList = getColorMap(colorMap);

  /* clear the entire color table for this image */
  for (i=0; i<gdMaxColors; i++) {
    gdImageColorDeallocate(img, i);
  }

  /* get background color */
  color2rgb(backgroundColor, &backgroundRed, &backgroundGreen, &backgroundBlue);  
  
  /* allocate the background color, index 0 */
  checkColor(gdImageColorAllocate(img, backgroundRed, backgroundGreen, backgroundBlue));

  /* Allocate white, index 1 */
  checkColor(gdImageColorAllocate(img, 255, 255, 255));

  /* Allocate black, index 2 */
  checkColor(gdImageColorAllocate(img, 0, 0 ,0));

  /* Allocate grey, index 3 */
  checkColor(gdImageColorAllocate(img, 128, 128, 128));
  
  numcolorsPerStep = (int)(numColors / (NUMMAPCOLS  - 1));
  DEBUG_CODE(1, fprintf(stderr, "Gonna allocate %d per step\n", numcolorsPerStep););
  for (i=0; colorMapList[i+1] != NULL; i++) {
    minColorString = colorMapList[i];
    maxColorString = colorMapList[i+1];
    DEBUG_CODE(1, fprintf(stderr, "Gonna allocate %s and %s\n", minColorString, maxColorString););
    string2color(minColorString, &minColor);
    string2color(maxColorString, &maxColor);
    color2rgb(minColor, &startRed, &startGreen, &startBlue);
    color2rgb(maxColor, &endRed, &endGreen, &endBlue);
    r = startRed;
    g = startGreen;
    b = startBlue;
    if (i>0) {
      r+=redStepSize;
      g+=greenStepSize;
      b+=blueStepSize;
    }
    redStepSize = getStepSize(startRed, endRed, numcolorsPerStep);
    greenStepSize = getStepSize(startGreen, endGreen, numcolorsPerStep);
    blueStepSize = getStepSize(startBlue, endBlue, numcolorsPerStep);    
    DEBUG_CODE(1, fprintf(stderr, "Red step size: %f Green step size: %f Blue step size: %f\n", redStepSize, greenStepSize, blueStepSize); );
    makeColors(img, r, g, b, redStepSize, greenStepSize, blueStepSize, numcolorsPerStep+1);
  }
} /* allocateColorMap */


/*****************************************************************************
 * 
 *****************************************************************************/
char** getColorMap (int colorMap) {
  DEBUG_CODE(1, fprintf(stderr, "Requested Color map is %d\n", colorMap););
  switch (colorMap) {
  case 1 :
    return blackbody;
  case 2 :
    return spectrum;
  default:
    colorError(invalid);
    break;
  }
} /* getColorMap */


