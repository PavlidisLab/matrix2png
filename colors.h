/*****************************************************************************
 * FILE: colors.h
 * AUTHOR: Paul Pavlidis
 * CREATE DATE: 2/2001
 * PROJECT: PLOTKIT
 * DESCRIPTION: Color definitions and allocation
 *****************************************************************************/

#ifndef COLORS_H
#define COLORS_H

#include "gd.h"
#include "utils.h"
#include "debug.h"

#define MAXCOLORS (gdMaxColors-2) /* reserve one color out of 256 for
                                     the background color, and an
                                     extra for the 'middle' of the
                                     scale */

#define PASSTHROUGHBLACKMINCOLORS 2 
#define NOPASSTHROUGHBLACKMINCOLORS 2

/* color names - each corresponds to a case in color2rgb */
typedef enum {white, red, yellow, green, cyan, blue, magenta, black} color_T;

/* error codes for color allocation */
typedef enum {invalid, nocolor, norange, toofew, toomany} colorerrorcode_T;


/* define all the colors for our image */		     
void allocateColors (
		     gdImage* img,
		     color_T backgroundColor,
		     color_T  minColor,
		     color_T maxColor,
		     BOOLEAN_T passThroughBlack,
		     int numColors
		     );

/* report error in color allocation or definition */
void colorError(colorerrorcode_T colorerrorcode);

/* check that a valid color was allocated */
void checkColor(int color);

/* return the rgb components for a chosen color */
void color2rgb(color_T colorVal, int* red, int* green, int* blue);

/* how big steps between values have to be */
double getStepSize(int minColor, /* an r,g, or b value from 0 to 255 */
		int maxColor, /* an r,g, or b value from 0 to 255 */
		int numcolor /* total number of colors wanted */
		);

/* do actual color allocations across a range of values */
void makeColors (gdImagePtr img, 
		 double r, /* initial value of red */
		 double g, /* initial value of green */
		 double b, /* initial value of blue */
		 double redStepSize, /* advance each color by amounts */
		 double greenStepSize, 
		 double blueStepSize, 
		 int numColors /* total number of colors needed */
		 );

#endif
