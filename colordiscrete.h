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
#include "string-list.h"


#define DMAP_INITIAL_COUNT 4
#define DEFAULT_DISCRETE_STRING "default"
#define DEFAULT_DISCRETE_LABEL "Other"
#define DEFAULT_DISCRETE_LABEL_BUFSIZE 25
#define DEFAULT_DISCRETE_COLOR_INDEX 5
#define DEFAULT_DISCRETE_COLOR "grey"
#define MAX_DROW 1000
#define DEFAULT_DISCRETE_MAPSIZE 8 // how many colors to use when a map file is not specified. Used by readdiscretemap

/* struct defining a discrete value to color mapping */
typedef struct discretemap_t 
{
  colorV_T**     colors;
  //  int*       values; // not needed in current implementation (integers 1,2,...)
  STRING_LIST_T* labels;
  int            count;
  int            maxcount;
  colorV_T*      default_colorcode;
  char           defaultlabel[DEFAULT_DISCRETE_LABEL_BUFSIZE];
} DISCRETEMAP_T;

DISCRETEMAP_T* readDiscreteMap(FILE* file);
DISCRETEMAP_T* allocateDiscreteMap(void);
void freeDiscreteMap(DISCRETEMAP_T* dmap);
void growDiscreteMap(DISCRETEMAP_T* dmap);
void allocateColorsDiscrete (gdImagePtr img, 
			     DISCRETEMAP_T* dmap, 
			     colorV_T* backgroundColor, 
			     colorV_T* missingColor);

#endif /* COLORDISCRETE_H */
