/*****************************************************************************
 * FILE: locations.h
 * AUTHOR: Paul Pavlidis
 * CREATE DATE: 2/2001
 * PROJECT: PLOTKIT
 * DESCRIPTION: Locations in an image used to define placement of extra bits
 *****************************************************************************/
 
#ifndef LOCATIONS_H
#define LOcATIONS_H

#include "utils.h"

typedef enum {topmiddle, topleft, topright, 
	     bottommiddle, bottomleft, bottomright, 
	     lefttop, leftmiddle, leftbottom, 
	     righttop, rightmiddle, rightbottom} looseloc_T;

typedef struct {
  double xf; /* upper left corner fractional distance */
  double yf; /* upper right corner fractional distance*/
  BOOLEAN_T vertical;
} location_T;

/* size of extra bits in pixels */
#define SMALL 15
#define MEDIUM 25
#define LARGE 35

const looseloc_T standardLocations[12];

#endif
