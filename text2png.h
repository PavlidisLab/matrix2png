/*****************************************************************************
 * FILE: text2image.h
 * AUTHOR: Paul Pavlidis
 * CREATE DATE: 2/2001
 * PROJECT: PLOTKIT
 * DESCRIPTION: Turn a text file into a png image
 * Copyright (c) Columbia University
 *****************************************************************************/

#ifndef TEXT2IMAGE_H
#define TEXT2IMAGE_H
#include "gd.h"
#include "utils.h"
#include "matrix2png.h"

#define WARNINGSIZE 40000 /* images larger than this many pixels
                             (wide,high) may not display properly in
                             some browsers */

#define DEFAULTFONT gdFontSmall
#define DEFAULTLINESPACE 2
#define TEXTPADDING 2  /* extra space, used at the beginning of strings */
#define DIVIDERCHARS  "\t" /* characters which don't render properly with gd fonts */
#define DIVIDERWIDTH 1 /* how much blanks we turn DIVIDERCHARS into */

/*****************************************************************************
 * calcTextDimensions - calculate the space needed for the text
 *****************************************************************************/
void calcTextDimensions (STRING_LIST_T* strings,
			 int numtodo,
			 BOOLEAN_T vertical,
			 int padding,
			 int linespacing,
			 gdFontPtr font,
			 int *width,
			 int *height);


/*****************************************************************************
 * stringlist2image - takes a string list structure							      
 *****************************************************************************/
void stringlist2image (gdImagePtr img,
		       STRING_LIST_T* strings,
		       int numtodo,
		       BOOLEAN_T rightJustified, /* this means top justified if vertical is true */
		       BOOLEAN_T vertical,
		       int padding, /* extra pixels at edge of image */
		       int linespacing,
		       int initX,
		       int initY,
		       gdFontPtr font /* 0,1,2,3,4 for tiny, small, medium, large, giant */
		       );


/*****************************************************************************
 * chooseFont - convert command line flag into font name
 *****************************************************************************/
gdFontPtr chooseFont (char* cmdlineflag);

#endif /*TEXT2IMAGE_H*/





