/*****************************************************************************
 * FILE: colorscalebar.h
 * AUTHOR: Paul Pavlidis
 * CREATE DATE: 2/2001
 * PROJECT: PLOTKIT
 * DESCRIPTION: Functions to draw color scale bars on images.
 *****************************************************************************/
#ifndef CSB_H
#define CSB_H

#include "gd.h"
#include "utils.h"

#ifdef TINYTEXT
#define LABELFONT gdFontTiny
#include "gdfontt.h"
#else
#ifdef SMALLTEXT
#define LABELFONT gdFontSmall
#include "gdfonts.h"
#else
#ifdef MEDIUMTEXT
#define LABELFONT gdFontMediumBold
#include "gdfontmb.h"
#else
#ifdef LARGETEXT
#define LABELFONT gdFontLarge
#include "gdfontl.h"
#endif
#endif
#endif
#endif

#define MAXLABELLENGTH 25
#define LABELFORMAT "%.2f"
#define CHARWIDTH (LABELFONT->w)
#define LABELHEIGHT (LABELFONT->h)
#define MINLABELWIDTH (CHARWIDTH*5)
#define DEFAULTSCALEBARLENGTH 150
#define DEFAULTSCALEBARHEIGHT 8
#define PADDING 3 /* pixel padding for some text features */

/* add a scale bar to an image. Must designate where to put it. If
 * space was not allotted in the image for the scale bar. It uses the
 * colors which have been allocated for the image.
 */
void drawScaleBar (
	       gdImagePtr img,
	       BOOLEAN_T vertical,
	       int xStart, /* all meas in pixels*/
	       int yStart,
	       int thickness,
	       int length
	       );

void checkScaleBarDims (
			gdImagePtr img,
			BOOLEAN_T vertical,
			int xStart, /* all meas in pixels*/
			int yStart,
			int thickness,
			int length
			);

void getTotalScaleBarDims(BOOLEAN_T addLabels,
			  BOOLEAN_T includeMidVal,
			  BOOLEAN_T vertical,
			  BOOLEAN_T rotatelabels,
			  int barLength,
			  int barThickness,
			  double scaleMin,
			  double scaleMax,
			  int *width,
			  int *height,
			  int *widthoffset,
			  int *heightoffset);


void labelScaleBar (
		    gdImagePtr img,
		    BOOLEAN_T includemiddleval,
		    BOOLEAN_T vertical,
		    BOOLEAN_T rotatelabels,
		    int scaleBarxStart, /* all meas in pixels*/
		    int scaleBaryStart,
		    int scaleBarthickness,
		    int scaleBarlength,
		    double scaleMin,
		    double scaleMax
		    );

#endif









