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
char *redgreen[] = {"red", "darkred", "black", "darkgreen", "green", NULL};
char *greenred[] = {"green", "darkgreen", "black", "darkred", "red", NULL};
char *twocolorarray[] = {"green", "yellow", "red", NULL};


/*****************************************************************************
 * 
 *****************************************************************************/
void allocateColorMap (
		       gdImagePtr img,
		       colorV_T* backgroundColor,
		       colorV_T* missingColor,
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
  int missingRed = 0;
  int missingGreen = 0;
  int missingBlue = 0;
  int endRed = 0;
  int endGreen = 0;
  int endBlue = 0;
  int numcolorsPerStep = 0;
  char** colorMapList = NULL;
  colorV_T* minColor = initColorVByName((color_T)0);
  colorV_T* maxColor = initColorVByName((color_T)0);
  char* maxColorString;
  char* minColorString;
  
  int i; /* counter */
  int nummapcols = 0;

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

  /* get background color and missing value color */
  color2rgb(backgroundColor, &backgroundRed, &backgroundGreen, &backgroundBlue);
  color2rgb(missingColor, &missingRed, &missingGreen, &missingBlue);  

  /* allocate the background color, index 0 */
  checkColor(gdImageColorAllocate(img, backgroundRed, backgroundGreen, backgroundBlue));

  /* Allocate white, index 1 */
  checkColor(gdImageColorAllocate(img, 255, 255, 255));

  /* Allocate black, index 2 */
  checkColor(gdImageColorAllocate(img, 0, 0 ,0));

  /* Allocate grey, index 3 */
  checkColor(gdImageColorAllocate(img, 128, 128, 128));

  /* allocate the missing value color, index 4 */
  checkColor(gdImageColorAllocate(img, missingRed, missingGreen, missingBlue));

  i=0;
  while (colorMapList[i] != NULL) {
    nummapcols++;
    i++;
  }

  numcolorsPerStep = (int)(numColors / (nummapcols - 1));
  DEBUG_CODE(1, fprintf(stderr, "Gonna allocate %d per step\n", numcolorsPerStep););
  for (i=0; colorMapList[i+1] != NULL; i++) {
    minColorString = colorMapList[i];
    maxColorString = colorMapList[i+1];
    DEBUG_CODE(1, fprintf(stderr, "Gonna allocate %s and %s\n", minColorString, maxColorString););
    string2color(minColorString, minColor);
    string2color(maxColorString, maxColor);
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
  case 3 :
    return redgreen;
  case 4 :
    return greenred;
  case 5 :
    return twocolorarray;
  default:
    colorError(invalid);
    break;
  }
  return(0);
} /* getColorMap */
