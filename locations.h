/*****************************************************************************
 * FILE: locations.h
 * AUTHOR: Paul Pavlidis
 * CREATE DATE: 2/2001
 * PROJECT: PLOTKIT
 * DESCRIPTION: Locations in an image used to define placement of extra bits
 *****************************************************************************/
 
#ifndef LOCATIONS_H
#define LOcATIONS_H

enum location {topmiddle, topleft, topright, 
	       bottommiddle, bottomleft, bottomright, 
	       lefttop, leftmiddle, leftbottom, 
	       righttop, rightmiddle, rightbottom};

/* size of extra bits in pixels */
#define SMALL 15
#define MEDIUM 25
#define LARGE 35

#endif
