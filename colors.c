/*****************************************************************************
 * FILE: colors.c
 * AUTHOR: Paul Pavlidis
 * CREATE DATE: 2/2001
 * PROJECT: PLOTKIT
 * DESCRIPTION: Color definitions and allocation, using 'gd' library.
 *****************************************************************************/

#include "gd.h"
#include "utils.h"
#include "colors.h"
#include "debug.h" /* definition of DEBUG */

/**********************************************************
 * Allocate a color table for an image. The start and end colors can
 * be any of RGBCMYKW. One color is always reserved for the
 * background, and an extra color is used for the 'middle' value of
 * the scale.
 **********************************************************/
void allocateColors (
		     gdImage* img,
		     color_T backgroundColor,
		     color_T minColor,
		     color_T maxColor,
		     BOOLEAN_T passThroughBlack,
		     int numColors
		     )
{

  /* the lowest values will be mapped to start*, the highest to end* */
  int startRed = 0;
  int startGreen = 0;
  int startBlue = 0;
  int backgroundRed = 0;
  int backgroundGreen = 0;
  int backgroundBlue = 0;
  int endRed = 0;
  int endGreen = 0;
  int endBlue = 0;

  double r, g, b; /* color components */

  int i; /* counter */

  /* step sizes for moving from low to high values in the scale */
  double redStepSize;
  double greenStepSize;
  double blueStepSize;
  
  int colorReturn= 0; /* the return value from gdAllocatecolor */

  /* check for valid color choices */
  if (minColor == maxColor) {
    colorError(norange);
  }

  if (passThroughBlack) {
    if (numColors < PASSTHROUGHBLACKMINCOLORS) {
      colorError(toofew);
    }
  }  else {
    if (numColors < NOPASSTHROUGHBLACKMINCOLORS) {
      colorError(toofew);
    }
  }
      
  if (numColors > MAXCOLORS) {
    colorError(toomany); /* note that if number of colors chosen is
                            even, the 'middle' color won't necessarily
                            be right */
  }

  /* clear the entire color table for this image */
  for (i=0; i<gdMaxColors; i++) {
    gdImageColorDeallocate(img, i);
  }
    
  /* get rgb values for our key colors */
  color2rgb(minColor, &startRed, &startGreen, &startBlue);
  color2rgb(maxColor, &endRed, &endGreen, &endBlue);
  color2rgb(backgroundColor, &backgroundRed, &backgroundGreen, &backgroundBlue);
  
  /* allocate the background color */
  colorReturn = gdImageColorAllocate(img, backgroundRed, backgroundGreen, backgroundBlue);
  checkColor(colorReturn);

  /* allocate the rest of the colors */
  if (passThroughBlack) {
    /* first head to black from the start color. */
    r = startRed;
    g = startGreen;
    b = startBlue;
    redStepSize = 2*getStepSize(startRed, 0, numColors);
    greenStepSize = 2*getStepSize(startGreen, 0, numColors);
    blueStepSize = 2*getStepSize(startBlue, 0, numColors);
    DEBUG_CODE(1, fprintf(stderr, "Red step size: %f Green step size: %f Blue step size: %f\n", redStepSize, greenStepSize, blueStepSize); );
    makeColors(img, r, g, b, redStepSize, greenStepSize, blueStepSize, numColors/2);

    /* then head up from black to the end color. Note that black is an
       'extra color' */
    r = 0.0;
    g = 0.0;
    b = 0.0;
    redStepSize = 2*getStepSize(0, endRed, numColors);
    greenStepSize = 2*getStepSize(0, endGreen, numColors);
    blueStepSize = 2*getStepSize(0, endBlue, numColors);
    DEBUG_CODE(1, fprintf(stderr, "Red step size: %f Green step size: %f Blue step size: %f\n", redStepSize, greenStepSize, blueStepSize); );
    makeColors(img, r, g, b, redStepSize, greenStepSize, blueStepSize, numColors/2 + 1);
    
  } else { /* don't pass through black */
    /* smooth change between the two colors, middle color is a
       mixture; the middle color is 'extra' */
    r = startRed;
    g = startGreen;
    b = startBlue;
    redStepSize = getStepSize(startRed, endRed, numColors);
    greenStepSize = getStepSize(startGreen, endGreen, numColors);
    blueStepSize = getStepSize(startBlue, endBlue, numColors);    
    DEBUG_CODE(1, fprintf(stderr, "Red step size: %f Green step size: %f Blue step size: %f\n", redStepSize, greenStepSize, blueStepSize); );
    makeColors(img, r, g, b, redStepSize, greenStepSize, blueStepSize, numColors + 1);
  }
} /* allocateColors */


/**********************************************************
 * Calculate how fast we have to change color components (see
 * colors.h) Assume min and max colors are different!
 **********************************************************/
double getStepSize(int minColor, 
		int maxColor, 
		int numColors) 
{
  return ((double)(maxColor - minColor)/numColors);
} /* getStepSize */


/**********************************************************
 * Return the rgb components of a color_T.
 **********************************************************/
void color2rgb(color_T colorVal, int* r, int* g, int* b) {

  switch ( colorVal ) {
  case white:
    *r = 255;    *g = 255;    *b = 255;
    break;
  case red:
    *r = 255;    *g = 0;    *b = 0;
    break;
  case yellow:
    *r = 255;    *g = 255;    *b = 0;
    break;
  case green:
    *r = 0;    *g = 255;    *b = 0;
    break;
  case cyan:
    *r = 0;    *g = 255;    *b = 255;
    break;
  case blue:
    *r = 0;    *g = 0;    *b = 255;
    break;
  case magenta:
    *r = 255;    *g = 0;    *b = 255;
    break;
  case black:
    *r = 0;    *g = 0;    *b = 0;
    break;
  default:
    colorError(invalid);
    break;
  }
} /* color2rgb */


/*********************************************************************
 * makeColors: do actual allocations across a range. Does _not_ clear
 * any previously allocated colors. Caller must make sure enough slots
 * exist.
 *********************************************************************/
void makeColors (gdImagePtr img, 
		 double r, /* initial value of red */
		 double g, /* initial value of green */
		 double b, /* initial value of blue */
		 double redStepSize, /* advance each color by amounts */
		 double greenStepSize, 
		 double blueStepSize, 
		 int numColors /* total number of colors needed */
		 )
{
  int colorReturn; /* return value from gdImageCollorAllocate */
  int i; /* counter */ 
  for (i=0; i<numColors;i++) {
    colorReturn = gdImageColorAllocate(img, r, g, b);
    checkColor(colorReturn);
    DEBUG_CODE( 1, fprintf(stderr, "Allocated %f %f %f\n", r, g, b); );
    
    r+=redStepSize;
    g+=greenStepSize;
    b+=blueStepSize;
  }
} /* makeColors */


/**********************************************************
 * Error checking
 **********************************************************/

/* respond to the return value from gdImageAllocateColor */
void checkColor(int colorReturn) 
{
  if (colorReturn < 0) {
    colorError(nocolor);
  }
} /* checkColor */


/* error codes */
void colorError(colorerrorcode_T colorerrorcode)
{
  switch (colorerrorcode) {
  case nocolor:
    die("Could not allocate any more colors\n");
    break;
  case invalid:
    die("Illegal color selected\n");
    break;
  case norange:
    die("Illegal color range: min and max colors must not be the same\n");
    break;
  case toomany:
    die("Illegal number of colors: Value must be <= %d (one color is reserved for background)\n", MAXCOLORS);
    break;
  case toofew:
    die("Illegal number of colors: too few\n");
    break;
  default:
    break;
  }
} /* color error */
