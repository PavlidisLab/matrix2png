/*****************************************************************************
 * FILE: colordiscrete.h
 * AUTHOR: Paul Pavlidis
 * CREATE DATE: 6/2002
 * PROJECT: PLOTKIT
 * DESCRIPTION: Functions relating to using discretized color maps.
 *****************************************************************************/
#ifndef COLORDISCRETE_H
#define COLORDISCRETE_H

#include "gd.h"
#include "matrixinfo.h"
#include "utils.h"
#include "colors.h"

#define DMAP_INITIAL_COUNT 15
#define DEFAULT_DISCRETE_STRING "default"
#define DEFAULT_DISCRETE_LABEL "Other"
#define DEFAULT_DISCRETE_COLOR_INDEX 5
#define DEFAULT_DISCRETE_COLOR "grey"
#define MAX_DROW 1000

/* Default discrete mapping, using as many of the predefined colors as
 *  possible. Designed so there is a reasonable chance that this will
 *  look okay. Don't use black and white because these are reasonbly
 *  likely to be background values. Grey is also not used.
 */
#define DEFAULT_DISCRETE_MAPARY {red, blue, green, magenta, cyan, yellow, violet, orange, darkred, darkblue, darkgreen, (color_T)0}

DISCRETEMAP_T* readDiscreteMap(FILE* file);
DISCRETEMAP_T* allocateDiscreteMap(void);
void freeDiscreteMap(DISCRETEMAP_T* dmap);
void growDiscreteMap(DISCRETEMAP_T* dmap);
void checkDiscreteUsedValues(MATRIXINFO_T* matrixInfo);
void allocateColorsDiscrete (gdImagePtr img, 
			     DISCRETEMAP_T* dmap, 
			     colorV_T* backgroundColor, 
			     colorV_T* missingColor);


#endif /* COLORDISCRETE_H */
