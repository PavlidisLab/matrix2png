/*****************************************************************************
 * FILE: locations.h
 * AUTHOR: Paul Pavlidis
 * CREATE DATE: 2/2001
 * PROJECT: PLOTKIT
 * DESCRIPTION: Locations in an image used to define placement of extra bits
 * Copyright (c) Columbia University
 *****************************************************************************/
 
#ifndef LOCATIONS_H
#define LOCATIONS_H

#include "utils.h"
#include "gd.h"

#define NUMSTANDARDLOCS 9
typedef enum {center=1, topmiddle, topleft, topright, 
	      bottommiddle, bottomleft, bottomright,
	      leftmiddle, rightmiddle} looseloc_T;


/* max length of description for location */
#define MAXDESCR 50

/* data structure to store location information */
typedef struct location_t LOCATION_T;

/* data structure to store the already-used parts of an image (just a rectangle) */
typedef struct usedspace_t USED_T;

/* data structure to store region coordinates */
typedef struct region_t {
  int ulx;
  int uly;
  int lrx;
  int lry;
} REGION_T;
#define newRegion() ((REGION_T*)mymalloc(sizeof(REGION_T)))

/* location definitions */
#define TOP 0.0
#define BOTTOM 1.0
#define LEFT 0.0
#define RIGHT 1.0
#define MIDDLE 0.5


/*****************************************************************************
 * Define a new location
 *****************************************************************************/
LOCATION_T* defineLocation (char* description, 
			    looseloc_T a_looseloc,
			    double xf, 
			    double yf);


/*****************************************************************************
 * Free a location
 *****************************************************************************/
void freeLocation (LOCATION_T* location);


/*****************************************************************************
 * Free standardlocs
 *****************************************************************************/
void freeStandardLocs (LOCATION_T** standardlocs);



/*****************************************************************************
 * Initialize a 'used' region struct
 *****************************************************************************/
USED_T*  initUsedRegion(void);

/*****************************************************************************
 * Update a 'used' region, define simple calcs on contents
 *****************************************************************************/
void updateUsedRegion(USED_T* usedRegion, int newulx, int newuly, int newlrx, int newlry);
#define USEDWIDTH(x) (x->lrx - x->ulx)
#define USEDHEIGHT(x) (x->lry - x->uly)

/**********************************************************
 * convert a string description of a location to a looseloc_T
 **********************************************************/
void string2looseloc(char* string, looseloc_T* loc);


/*****************************************************************************
 * Define standard locations as defined here
 *****************************************************************************/
LOCATION_T** defineStandardLocations(void);


/*****************************************************************************
 * Return upper left coordinate for an object given a location
 *****************************************************************************/
void findLocationCoords(LOCATION_T* location, 
			USED_T* usedRegion,
			int featureWidth, 
			int featureHeight, 
			int* x, 
			int* y);



/*****************************************************************************
 * Adjust coordinates to align with existing features.
 *****************************************************************************/
void align(gdImagePtr img,
	   char* locationAsString,
	   int *desiredupperLeftX, 
	   int *desiredupperLeftY, 
	   int featureWidth, 
	   USED_T* usedRegion);



/*****************************************************************************
 * Find space for a new feature to be added to an image at a given
 * location. Resize the image if necessary. This requires knowing what
 * parts of the image are "taken" by existing features.
 *****************************************************************************/
void placeFeature(gdImagePtr img, 
		  char* locationAsString,
		  BOOLEAN_T alignWithExisting, /* try to line it up with existing features */
		  int* x,  /* where we will put the new features */
		  int* y,
		  USED_T* usedRegion,
		  int featureWidth,
		  int featureHeight,
		  int *xtotaloffset, 
		  int *ytotaloffset);



/*****************************************************************************
 * Resize an image canvas to make it larger. Requires making a
 * copy. The old image is placed within the new image at Xplace,Yplace
 * (upper left). The old img is then destroyed.
 *****************************************************************************/
void enlargeCanvas(gdImagePtr img,
		   int newXsize,
		   int newYsize,
		   int Xplace,
		   int Yplace);
	       

#endif /* LOCATIONS_H */



