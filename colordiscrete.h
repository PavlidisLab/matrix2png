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
#include "utils.h"
#include "colors.h"


/* struct defining a discrete value to color mapping */
typedef struct discretemap_t 
{
  color_T*   colors;
  //  int*       values; // not needed in current implementation (integers 1,2,...)
  color_T    default_colorcode;
  int        count;
  int        maxcount;
} DISCRETEMAP_T;


#define DMAP_INITIAL_COUNT 10
#define DEFAULT_DISCRETE_STRING "default"
#define DEFAULT_DISCRETE_COLOR_INDEX 5
#define MAX_DROW 1000

DISCRETEMAP_T* readDiscreteMap(FILE* file);
DISCRETEMAP_T* allocateDiscreteMap(void);
void freeDiscreteMap(DISCRETEMAP_T* dmap);
void growDiscreteMap(DISCRETEMAP_T* dmap);
void allocateColorsDiscrete (gdImagePtr img, 
			     DISCRETEMAP_T* dmap, 
			     color_T backgroundColor, 
			     color_T missingColor);

#endif /* COLORDISCRETE_H */
