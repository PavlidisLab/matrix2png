/*****************************************************************************
 * FILE: colors.h
 * AUTHOR: Paul Pavlidis
 * CREATE DATE: 2/2001
 * PROJECT: PLOTKIT
 * DESCRIPTION: Color definitions and allocation
 * Copyright (c) Columbia University
 *****************************************************************************/

#ifndef COLORS_H
#define COLORS_H

#include "gd.h"
#include "utils.h"

/* how many colors are set aside - includes the background color and
   white and black and grey, and the missing value color. */
#define NUMRESERVEDCOLORS 5
#define MISSING 4 // index of the color to use for missing values

/* reserve one color out of 256 for the background color, one for
 * white, and an extra for the 'middle' of the scale */
#define MAXCOLORS (gdMaxColors-(1+NUMRESERVEDCOLORS) )
#define MINCOLORS 2

#define PASSTHROUGHBLACKMINCOLORS 2 
#define NOPASSTHROUGHBLACKMINCOLORS 2

#define DEFAULTNUMCOLORS 64
#define MINRGB 0
#define MAXRGB 255

/* color names - each corresponds to a case in color2rgb */
typedef enum {white=1, 
	      red, 
	      yellow, 
	      green, 
	      cyan, 
	      blue, 
	      magenta, 
	      black, 
	      grey, 
	      gray,
	      orange,
	      violet,
	      darkred,
	      darkgreen,
	      darkblue
} color_T;


/* this struct can hold either or both a named color and a rgb color vector */
typedef struct colorv_t {
  color_T namedcolor;
  int rgb[3];
} colorV_T;

colorV_T* initColorVByName (color_T named);

/* error codes for color allocation */
typedef enum {invalid, nocolor, norange, toofew, toomany, badrgb} colorerrorcode_T;


/* define all the colors for our image */		     
void allocateColors (
		     gdImage* img,
		     colorV_T* backgroundColor,
		     colorV_T*  minColor,
		     colorV_T* maxColor,
		     colorV_T* missingColor,
		     BOOLEAN_T passThroughBlack,
		     int numColors
		     );

/* report error in color allocation or definition */
void colorError(colorerrorcode_T colorerrorcode, ...);

/* check that a valid color was allocated */
void checkColor(int color);

/* check that a valid rgb values are selected*/
void checkValidRGB(colorV_T* color);

/* convert a string description of a color to a color_T */
void string2color(char* string, colorV_T* colorVal);

/* return the rgb components for a chosen color */
void color2rgb(colorV_T* colorVal, int* red, int* green, int* blue);

/* copy a palette between two gd images - this does not do the
   remapping and should be used only on blank images - this
   manipulates the gdimage data structure directly */
void copyPaletteToNew(gdImagePtr dst, gdImagePtr src);

/* how big steps between values have to be */
double getStepSize(int minColor, /* an r,g, or b value from 0 to 255 */
		int maxColor, /* an r,g, or b value from 0 to 255 */
		int numcolor /* total number of colors wanted */
		);

/* choose a color (intensity actually) that contrasts with the background (for text for example) */
int chooseContrastingColor(gdImagePtr img);

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
