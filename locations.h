/*****************************************************************************
 * FILE: locations.h
 * AUTHOR: Paul Pavlidis
 * CREATE DATE: 2/2001
 * PROJECT: PLOTKIT
 * DESCRIPTION: Locations in an image used to define placement of extra bits
 *****************************************************************************/
 
#ifndef LOCATIONS_H
#define LOcATIONS_H

typdef enum {topmiddle, topleft, topright, 
	     bottommiddle, bottomleft, bottomright, 
	     lefttop, leftmiddle, leftbottom, 
	     righttop, rightmiddle, rightbottom} location_T;

/* size of extra bits in pixels */
#define SMALL 15
#define MEDIUM 25
#define LARGE 35

#endif
