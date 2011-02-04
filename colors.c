/*****************************************************************************
 * FILE: colors.c
 * AUTHOR: Paul Pavlidis
 * CREATE DATE: 2/2001
 * PROJECT: PLOTKIT
 * DESCRIPTION: Color definitions and allocation, using 'gd' library.
 * Copyright (c) Columbia University
 *****************************************************************************/

#include "gd.h"
#include "utils.h"
#include "colors.h"
#include "cmdparse.h"
#include <stdarg.h>

/**********************************************************
 * Allocate a color table for an image. The start and end colors can
 * be any of RGBCMYKW. One color is always reserved for the
 * background, and an extra color is used for the 'middle' value of
 * the scale.
 **********************************************************/
void allocateColors (
		     gdImagePtr img,
		     colorV_T* backgroundColor,
		     colorV_T* minColor,
		     colorV_T* midColor,
		     colorV_T* maxColor,
		     colorV_T* missingColor,
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
  int midRed = 0;
  int midGreen =0;
  int midBlue = 0;
  int missingRed = 0;
  int missingGreen = 0;
  int missingBlue = 0;

  double r, g, b; /* color components */

  int i; /* counter */

  /* step sizes for moving from low to high values in the scale */
  double redStepSize;
  double greenStepSize;
  double blueStepSize;
  
  /* check for valid color choices */
  if (minColor == maxColor) {
    colorError(norange);
  }

  if (passThroughBlack || midColor != NULL) {
    if (numColors < PASSTHROUGHBLACKMINCOLORS) {
      colorError(toofew);
    }
  }  else {
    if (numColors < NOPASSTHROUGHBLACKMINCOLORS) {
      colorError(toofew);
    }
  }
      
  if (numColors > MAXCOLORS) {
    colorError(toomany);
  }

  /* clear the entire color table for this image */
  for (i=0; i<gdMaxColors; i++) {
    gdImageColorDeallocate(img, i);
  }
    
  /* get rgb values for our key colors */
  color2rgb(minColor, &startRed, &startGreen, &startBlue);
  color2rgb(maxColor, &endRed, &endGreen, &endBlue);

  if (midColor != NULL) {
    color2rgb(midColor, &midRed, &midGreen, &midBlue);
  }
  color2rgb(backgroundColor, &backgroundRed, &backgroundGreen, &backgroundBlue);
  color2rgb(missingColor, &missingRed, &missingGreen, &missingBlue);
  
  /* the following calls take care of the 'reserved' colors that we
     need to have around. Some colors could get allocated twice this
     way. Todo: make the always-required colors allocated in a
     separate function call, because this is done in several places in the
     code. */

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

  /* allocate the rest of the colors */
  if (passThroughBlack) { //gets priority over other options.
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
  } else if (midColor != NULL) {
    /* first head to midcolor from start. */
    r = startRed;
    g = startGreen;
    b = startBlue;
    redStepSize = 2*getStepSize(startRed, midRed, numColors);
    greenStepSize = 2*getStepSize(startGreen, midGreen, numColors);
    blueStepSize = 2*getStepSize(startBlue, midBlue, numColors);
    DEBUG_CODE(1, fprintf(stderr, "Red step size: %f Green step size: %f Blue step size: %f\n", redStepSize, greenStepSize, blueStepSize); );
    makeColors(img, r, g, b, redStepSize, greenStepSize, blueStepSize, numColors/2);

    /* then head up from mid to the end color */
    r = midRed;
    g = midGreen;
    b = midBlue;
    redStepSize = 2*getStepSize(midRed, endRed, numColors);
    greenStepSize = 2*getStepSize(midGreen, endGreen, numColors);
    blueStepSize = 2*getStepSize(midBlue, endBlue, numColors);
    DEBUG_CODE(1, fprintf(stderr, "Red step size: %f Green step size: %f Blue step size: %f\n", redStepSize, greenStepSize, blueStepSize); );
    makeColors(img, r, g, b, redStepSize, greenStepSize, blueStepSize, numColors/2 + 1);
  } else { /* don't pass through black */
    /* smooth change between the two colors */
    r = startRed;
    g = startGreen;
    b = startBlue;
    redStepSize = getStepSize(startRed, endRed, numColors);
    greenStepSize = getStepSize(startGreen, endGreen, numColors);
    blueStepSize = getStepSize(startBlue, endBlue, numColors);    
    DEBUG_CODE(1, fprintf(stderr, "Red step size: %f Green step size: %f Blue step size: %f\n", redStepSize, greenStepSize, blueStepSize); );
    makeColors(img, r, g, b, redStepSize, greenStepSize, blueStepSize, numColors);
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
  return ((double)(maxColor - minColor)/(numColors == 1 ? 1 : numColors-1));
} /* getStepSize */


/**********************************************************
 * convert a string description of a color to a color_T 
 **********************************************************/
void string2color(char* string, colorV_T* colorVal)
{
  if (strstr(string, DIVIDER)) {
    parseValueSeries(string, DIVIDER, colorVal->rgb, 3);
    checkValidRGB(colorVal);
    colorVal->namedcolor = 0;
  } else if (!strcmp(string, "white")) {
    colorVal->namedcolor = white;
  } else if (!strcmp(string, "black")) {
    colorVal->namedcolor = black;
  } else if (!strcmp(string, "red")) {
    colorVal->namedcolor = red;
  } else if (!strcmp(string, "green")) {
    colorVal->namedcolor = green;
  } else if (!strcmp(string, "blue")) {
    colorVal->namedcolor = blue;
  } else if (!strcmp(string, "cyan")) {
    colorVal->namedcolor = cyan;
  } else if (!strcmp(string, "magenta")) {
    colorVal->namedcolor = magenta;
  } else if (!strcmp(string, "yellow")) {
    colorVal->namedcolor = yellow;
  } else if (!strcmp(string, "grey")) {
    colorVal->namedcolor = grey;
  } else if (!strcmp(string, "gray")) {
    colorVal->namedcolor = grey;
  } else if (!strcmp(string, "orange")) {
    colorVal->namedcolor = orange;
  } else if (!strcmp(string, "violet")) {
    colorVal->namedcolor = violet;
  } else if (!strcmp(string, "darkred")) {
    colorVal->namedcolor = darkred;
  } else if (!strcmp(string, "darkgreen")) {
    colorVal->namedcolor = darkgreen;
  } else if (!strcmp(string, "darkblue")) {
    colorVal->namedcolor = darkblue;
  } else if (!strcmp(string, "lightblue")) {
    colorVal->namedcolor = lightblue;
  } else if (!strcmp(string, "lightgreen")) {
    colorVal->namedcolor = lightgreen;
  } else if (!strcmp(string, "lightred")) {
    colorVal->namedcolor = lightred;
  } else {
    colorVal->namedcolor = 0;
    colorError(invalid, "(\"%s\" is not a valid choice)", string);
  }
  DEBUG_CODE(1, fprintf(stderr, "String \"%s\" to color: Got %d %d %d named color: %d\n", string, colorVal->rgb[0],  colorVal->rgb[1],  colorVal->rgb[2], (int)(colorVal->namedcolor)););

} /* string2color */



/**********************************************************
 * Return the rgb components of a color_T.
 **********************************************************/
void color2rgb(colorV_T* colorVal, int* r, int* g, int* b) {
  if (colorVal->namedcolor != (color_T)0) {
    switch ( colorVal->namedcolor ) {
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
    case grey:
      *r = 128;   *g = 128;   *b = 128;
      break;
    case gray:
      *r = 128;   *g = 128;   *b = 128;
      break;
    case orange:
      *r = 255;   *g = 128;   *b = 0;
      break;
    case violet:
      *r = 128;   *g = 0;   *b = 255;
      break;
    case darkred:
      *r = 128; *g = 0; *b = 0;
      break;
    case darkgreen:
      *r = 0; *g = 128; *b = 0;
      break;
    case darkblue:
      *r = 0; *g = 0; *b = 128;
      break;
    case lightred:
      *r = 255; *g = 128; *b = 128;
      break;
    case lightgreen:
      *r = 128; *g = 255; *b = 128;
      break;
    case lightblue:
      *r = 128; *g = 128; *b = 255;
      break;

    default:
      colorError(invalid);
      break;
    }
  } else {
    *r = colorVal->rgb[0];
    *g = colorVal->rgb[1];
    *b = colorVal->rgb[2];
  }
  DEBUG_CODE(1, fprintf(stderr, "Translated %d into %d %d %d\n", (int)(colorVal->namedcolor), *r, *g, *b););
} /* color2rgb */

void checkValidRGB(colorV_T* color) {
  int i;
  for (i=0; i<2; i++) {
    if (color->rgb[i] < MINRGB || color->rgb[i] > MAXRGB) {
      colorError(badrgb, "(%d - must be a value from %d to %d)", color->rgb[i], MINRGB, MAXRGB);
    }
  }
}

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
    /* clip */
    if (r > 255) {
      r = 255;
    } else if (r < 0) {
      r = 0;
    }
    if (g > 255) {
      g = 255;
    } else if (g < 0) {
      g = 0;
    }
    if (b > 255) {
      b = 255;
    } else if (b < 0) {
      b = 0;
    }

    colorReturn = gdImageColorAllocate(img, r, g, b);
    checkColor(colorReturn);
    DEBUG_CODE( 1, fprintf(stderr, "Allocated %f %f %f\n", r, g, b); );
    r+=redStepSize;
    g+=greenStepSize;
    b+=blueStepSize;
  }
} /* makeColors */



/*****************************************************************************
 * Figure out if black or white text should be used, depending on the
 * background color. This could be better.
 *****************************************************************************/
int chooseContrastingColor(gdImagePtr img)
{
  int r, g, b;
  r = gdImageRed(img, 0); /* Note that these are documented as gdImageColorRed etc - maybe it is so in the latest version of gd? */
  g = gdImageGreen(img, 0);
  b = gdImageBlue(img, 0);
  if (r + g + b < 325) { /* if color is dark, use white. The value 325 was arrived at by experientation. */
    return 255;/* use white */
  } else {
    return  0;  /* use black */
  }
} /* choosecontrastingColor */


/**********************************************************
 * Error checking
 **********************************************************/

/* respond to the return value from gdImageAllocateColor */
void checkColor(int colorReturn) 
{
  if (colorReturn < 0) {
    colorError(nocolor, "(%d)", colorReturn);
  }
} /* checkColor */


/* copy a palette between two gd images - this does not do the
   remapping and can be used only on blank target images with no
   colors allocated */
void copyPaletteToNew(gdImagePtr dst, gdImagePtr src)
{
  int i;
  if (dst->colorsTotal != 0) die("Can't call copyPaletteToNew on image with allocated colors\n");
  dst->colorsTotal = src->colorsTotal;
  for (i=0; i < src->colorsTotal; i++) {
    if (src->open[i] == 0) {
      dst->open[i] = 0;
      dst->red[i] = src->red[i];
      dst->green[i] = src->green[i];
      dst->blue[i] = src->blue[i];
    }
  }
  DEBUG_CODE(1, fprintf(stderr, "Target image has %d colors allocated\n", dst->colorsTotal););
} /* copyPaletteToNew */


/* initialize an empty colorV_T struct */
colorV_T* initColorVByName (color_T named) {
  colorV_T* return_value;
  return_value = (colorV_T*)mymalloc(sizeof(colorV_T));
  if (named != (color_T)NULL) { 
    return_value->namedcolor = named;
  } else {
    return_value->namedcolor = 0;
  }
  return_value->rgb[0] = -1;
  return_value->rgb[1] = -1;
  return_value->rgb[2] = -1;
  //  DEBUG_CODE(1, fprintf(stderr, "Initialized empty colorv_t %d\n", (int)(return_value->namedcolor)););
  return(return_value);
}

/* error codes. This is not really that useful any more, should probably just use 'die' */
void colorError(colorerrorcode_T colorerrorcode, ...)
{
  va_list  argp;
  char* format = NULL;

  fprintf(stderr, "Color selection error: ");
  switch (colorerrorcode) {
  case nocolor:
    fprintf(stderr, "Could not allocate any more colors ");
    break;
  case invalid:
    fprintf(stderr, "Illegal color selected ");
    break;
  case norange:
    fprintf(stderr, "Illegal color range: min and max colors must not be the same.");
    break;
  case toomany:
    fprintf(stderr, "Illegal number of colors: Value must be <= %d (one color is reserved for background) ", MAXCOLORS);
    break;
  case toofew:
    fprintf(stderr, "Illegal number of colors: too few.");
    break;
  case badrgb:
    fprintf(stderr, "Invalid RGB value ");
  default:
    break;
  }
  va_start(argp, colorerrorcode);
  format = va_arg(argp, char*);
  if (format != NULL) {
    vfprintf(stderr, format, argp);
  }
  va_end(argp);
  fprintf(stderr, "\n");
  fflush(stderr);

#ifdef DEBUG
  abort();
#else
  exit(1);
#endif

} /* color error */
