/*****************************************************************************
 * FILE: locations.c
 * AUTHOR: Paul Pavlidis
 * CREATE DATE: 2/2001
 * PROJECT: PLOTKIT
 * DESCRIPTION: Locations in an image used to define placement of extra bits
 * Copyright (c) Columbia University
 *****************************************************************************/

#include "locations.h"
#include "utils.h"
#include "gd.h"
#include <string.h> /* memset, memmove */

/*****************************************************************************
 * Data structure definition for storing location information
 *****************************************************************************/
struct location_t {
  char* description;
  looseloc_T looseloc; /* a standard location id  (enum) */
  double xf; /* upper left corner fractional distance x*/
  double yf; /* upper left corner fractional distance y*/
};


/*****************************************************************************
 * data structure to store the already-used parts of an image (just a rectangle)
 *****************************************************************************/
struct usedspace_t {
  int ulx; /* upper left x etc */
  int uly;
  int lrx;
  int lry; 
};


/*****************************************************************************
 * Define a new location
 *****************************************************************************/
LOCATION_T* defineLocation (char* desc, 
			    looseloc_T a_looseloc,
			    double xf, 
			    double yf)
{
  LOCATION_T* newloc;

  if (xf < 0.0 || xf > 1.0) die("Illegal fractional x location\n");
  if (yf < 0.0 || yf > 1.0) die("Illegal fractional y location\n");

  newloc = (LOCATION_T*)mymalloc(sizeof(LOCATION_T));
  newloc->description = (char*)mymalloc((MAXDESCR + 1)*sizeof(char));
  newloc->looseloc = a_looseloc;
  strcpy(newloc->description, desc);
  newloc->xf = xf;
  newloc->yf = yf;

  return(newloc);

} /* defineLocation */



/*****************************************************************************
 * Free a location
 *****************************************************************************/
void freeLocation (LOCATION_T* location)
{
  if (location == NULL) return;
  free(location->description);
  free(location);
} /* freeLocation */


/*****************************************************************************
 * Free standardlocs
 *****************************************************************************/
void freeStandardLocs (LOCATION_T** standardlocs)
{
  int i;
  for (i = 0; i< NUMSTANDARDLOCS; i++) {
    freeLocation(standardlocs[i]);
  }
  free(standardlocs);
}


/*****************************************************************************
 * Define standard locations as defined in locations.h
 *****************************************************************************/
LOCATION_T** defineStandardLocations(void)
{ 
  LOCATION_T** standardlocs;
  standardlocs = (LOCATION_T**)mymalloc(NUMSTANDARDLOCS*sizeof(LOCATION_T*));

  standardlocs[0] = defineLocation("center", center, MIDDLE, MIDDLE);
  standardlocs[1] = defineLocation("topmiddle", topmiddle, MIDDLE, TOP);
  standardlocs[2] = defineLocation("topleft", topleft, LEFT, TOP);
  standardlocs[3] = defineLocation("topright", topright, RIGHT, TOP);
  standardlocs[4] = defineLocation("bottommiddle", bottommiddle, MIDDLE, BOTTOM);
  standardlocs[5] = defineLocation("bottomleft", bottomleft, LEFT, BOTTOM);
  standardlocs[6] = defineLocation("bottomright", bottomright, RIGHT, BOTTOM);
  standardlocs[7] = defineLocation("leftmiddle", leftmiddle, LEFT, MIDDLE);
  standardlocs[8] = defineLocation("rightmiddle", rightmiddle, RIGHT, MIDDLE);

  return standardlocs;
  
} /* defineStandardLocations */



/**********************************************************
 * convert a string description of a location to a looseloc_T
 **********************************************************/
void string2looseloc(char* string, looseloc_T* loc)
{
  if (!strcmp(string, "center")) {
    *loc = center;
  } else if (!strcmp(string, "topmiddle")) {
    *loc = topmiddle;
  } else if (!strcmp(string, "topleft")) {
    *loc = topleft;
  } else if (!strcmp(string, "topright")) {
    *loc = topright;
  } else if (!strcmp(string, "bottommiddle")) {
    *loc = bottommiddle;
  } else if (!strcmp(string, "bottomleft")) {
    *loc = bottomleft;
  } else if (!strcmp(string, "bottomright")) {
    *loc = bottomright;
  } else if (!strcmp(string, "leftmiddle")) {
    *loc = leftmiddle;
  } else if (!strcmp(string, "rightmiddle")) {
    *loc = rightmiddle;
  } else {
    *loc = 0;
  }

} /* string2looseloc */



/*****************************************************************************
 * Initialize a 'used' region struct
 *****************************************************************************/
USED_T*  initUsedRegion(void) {
  USED_T* newUsedRegion;
  newUsedRegion = (USED_T*)mymalloc(sizeof(USED_T));
  newUsedRegion->ulx = -1;
  newUsedRegion->uly = -1;
  newUsedRegion->lrx = -1;
  newUsedRegion->lry = -1;
  return newUsedRegion;
} /* initUsedRegion */



/*****************************************************************************
 * Update a 'used' region
 *****************************************************************************/
void updateUsedRegion(USED_T* usedRegion, int newulx, int newuly, int newlrx, int newlry)
{
  usedRegion->ulx = newulx;
  usedRegion->uly = newuly;
  usedRegion->lrx = newlrx;
  usedRegion->lry = newlry;
} /* updateUsedRegion */



/*****************************************************************************
 * Return upper left coordinate for an object, allowing for the used
 * region. The coordinates might be negative numbers.
 *****************************************************************************/
void findLocationCoords(LOCATION_T* location, 
			USED_T* usedRegion,
			int featureWidth, 
			int featureHeight, 
			int* x, 
			int* y)
{
  int tempx, tempy;

  if (strstr(location->description, "right")) {
    tempx = (int)(usedRegion->lrx * location->xf);
  } else {
    tempx = (int)(usedRegion->ulx * location->xf - featureWidth);
  }

  if (strstr(location->description, "bottom")) {
    tempy = (int)(usedRegion->lry * location->yf);
    //    tempy = (int)(height * location->yf - featureHeight);    
  } else {
    tempy = (int)(usedRegion->uly * location->yf - featureHeight);
    //  tempy = (int)(height * location->yf);
  }

  *x = tempx;
  *y = tempy;

} /* findLocationCoords */


/*****************************************************************************
 * Adjust coordinates to align with existing features.
 *****************************************************************************/
void align(gdImagePtr img,
	   char* locationAsString,
	   int *desiredupperLeftX, 
	   int *desiredupperLeftY, 
	   int featureWidth,  // we only need the width for justification
	   USED_T* usedRegion)

{
  int imageXSize, imageYSize;

  imageXSize = gdImageSX(img);
  imageYSize = gdImageSY(img);

  /* the possibilities are: 
   * topmiddle, topleft, bottomleft, bottommiddle: move X only to the right by upperLeftXTaken
   * topright, bottomright: move x left by imageXsize - lowerRightXtaken
   * leftmiddle, rightmiddle: move y down
   */
  if (strstr(locationAsString, "top") != NULL || strstr(locationAsString, "bottom") != NULL) {
    /* then move X, but not Y  unless the image is in the way */
    if (strstr(locationAsString, "right") != NULL) { /*topright or bottom right, bottommiddle or topmiddle */
      *desiredupperLeftX = usedRegion->lrx - featureWidth;
    } else if (strstr(locationAsString, "left") != NULL || strstr(locationAsString, "right") != NULL)  { /*  topleft or bottom left */
      *desiredupperLeftX = usedRegion->ulx;
    } else { /* middle */
      if (featureWidth > (usedRegion->lrx - usedRegion->ulx)) {
	*desiredupperLeftX = -(featureWidth - usedRegion->lrx - usedRegion->ulx)/2;
      } else { 
	*desiredupperLeftX = usedRegion->ulx + (usedRegion->lrx - usedRegion->ulx - featureWidth)/2;
      }
    }
  } else { /* leftmiddle, rightmiddle */
    /* we align with the top, but not the left */
    *desiredupperLeftY = usedRegion->uly;
  }
} /* align */


/*****************************************************************************
 * Find space for a new feature to be added to an image at a given
 * location. Resize the image if necessary. This requires knowing what
 * parts of the image are "taken" by existing features. This function
 * should be called before adding anything to an image that uses the
 * location structs to place items.
 *****************************************************************************/
void placeFeature(gdImagePtr img, 
		  char* locationAsString,
		  BOOLEAN_T alignWithExisting, /* try to line it up with existing features */
		  int* x,  /* where we will put the new features */
		  int* y,
		  USED_T* usedRegion,
		  int featureWidth,
		  int featureHeight,
		  int *xoffset, int *yoffset)
{
  int currentXSize, currentYSize;
  int desiredupperLeftX, desiredupperLeftY, desiredlowerRightX, desiredlowerRightY;
  int newwidth, newheight, newstartX, newstartY;
  LOCATION_T** standardlocs;
  looseloc_T desiredlooseloc;
  LOCATION_T* desiredlocation;
  BOOLEAN_T naive = TRUE;
  BOOLEAN_T resize = FALSE;
  
  /* get the locations set up */
  standardlocs = defineStandardLocations();
  string2looseloc(locationAsString, &desiredlooseloc);
  if (desiredlooseloc == 0) die("Illegal location %s\n", locationAsString);
  desiredlocation = standardlocs[desiredlooseloc - 1];

  DEBUG_CODE(1, fprintf(stderr, "Desired location is %s, alignWithExisting=%d\n", desiredlocation->description, (int)alignWithExisting););

  /* figure out the size of the image */
  currentXSize = gdImageSX(img);
  currentYSize = gdImageSY(img);
  if (usedRegion->ulx >= 0) {
    naive = FALSE;
  }

  /* special case: center is only available if the image is naive */
  if (desiredlooseloc == center) {
    if (naive) { // we are adding the first thing to the image.
      DEBUG_CODE(1, fprintf(stderr, "Placing feature in the center\n"););
      /* go ahead, but don't do any aligning. Resize if necessary and
         update the taken coordinates */
      if (featureWidth > currentXSize || featureHeight > currentYSize) {
	/* resize the image */

	newwidth = featureWidth > currentXSize ? featureWidth : currentXSize;
	newheight = featureHeight > currentYSize ? featureHeight : currentYSize;
	newstartX = featureWidth == newwidth ? 0 : (newwidth - featureWidth )/2;
	newstartY = featureHeight == newheight ? 0 : (newheight - featureHeight)/2;
	DEBUG_CODE(1, fprintf(stderr, "Resizing image to %d by %d\n", newwidth, newheight););

	enlargeCanvas(img, newwidth, newheight, newstartX, newstartY);

	*x = newstartX;
	*y = newstartY;

      } else {
	*x = (currentXSize - featureWidth)/2;
	*y = (currentYSize - featureHeight)/2;
      }
      DEBUG_CODE(1, fprintf(stderr, "Starting from %d, %d\n", *x, *y););
      updateUsedRegion(usedRegion, *x, *y, *x + featureWidth, *y + featureHeight);
      *xoffset = *x;
      *yoffset = *y;
      return;
    } else {
      die("Cannot use center on non-naive image");
    }
  }   /* dealt with center case */

  /* figure out where we would put it if we could put it anywhere we
     wanted, then move existing stuff around and finally resize the image as needed */
  newstartX = usedRegion->ulx;
  newstartY = usedRegion->uly;

  findLocationCoords(desiredlocation, 
		     usedRegion,
		     featureWidth, 
		     featureHeight, 
		     &desiredupperLeftX, 
		     &desiredupperLeftY);
  
  /* usually the desired coordinates will be negative. */
  DEBUG_CODE(1, fprintf(stderr, "Before alignment, want to place object at: (upper left x,y coordinates) %d %d \n", desiredupperLeftX,  desiredupperLeftY););

  // this step just alters the dominant coordinate (x or y) to line up with what is there already.
  if (alignWithExisting && !naive) {
    align(img,
	  locationAsString,
	  &desiredupperLeftX, 
	  &desiredupperLeftY, 
	  featureWidth,
	  usedRegion);
  }
  desiredlowerRightX = desiredupperLeftX + featureWidth;
  desiredlowerRightY = desiredupperLeftY + featureHeight;
  DEBUG_CODE(1, fprintf(stderr, "After alignment want to place at (upperleft)  %d, %d, (lowerright) %d, %d\n", desiredupperLeftX, desiredupperLeftY, desiredlowerRightX, desiredlowerRightY););

  /* calculate the new image size and placement of the old image
     within it. It only needs to be resized if any of the coordinates
     are negative or greater than the image size */
  if (desiredupperLeftX < 0 || desiredlowerRightX > currentXSize) {
    DEBUG_CODE(1, fprintf(stderr, "Adjusting width\n"););
    newwidth = (desiredupperLeftX < 0 ? currentXSize - desiredupperLeftX : currentXSize) + (desiredlowerRightX > currentXSize ? desiredlowerRightX - currentXSize : 0);

    if (desiredupperLeftX < 0) {
      /* adjust the coordinate by the difference */
      newstartX -= desiredupperLeftX;
      desiredupperLeftX -= desiredupperLeftX;

    } else {
      //      newstartX = 0;
      /* don't need to move the existing image anywhere */
    }
    resize = TRUE;
  } else {
    newwidth = currentXSize;
    //    newstartX = 0;
  }

  if (desiredupperLeftY < 0 || desiredlowerRightY > currentYSize) {
    DEBUG_CODE(1, fprintf(stderr, "Adjusting height\n"););
    newheight = (desiredupperLeftY < 0 ? currentYSize - desiredupperLeftY : currentYSize) + (desiredlowerRightY > currentYSize ? desiredlowerRightY - currentYSize : 0);

    if (desiredupperLeftY < 0) {
      newstartY -= desiredupperLeftY;
      desiredupperLeftY -= desiredupperLeftY;

    } else {
      //      newstartY = 0;
      /* no adjustment is necessary */
    }
    resize = TRUE;
  } else {
    newheight = currentYSize;
    //    newstartY = 0;
  }

  DEBUG_CODE(1, fprintf(stderr, "New size will be %d x %d\n", newwidth, newheight););
  DEBUG_CODE(1, fprintf(stderr, "Moving existing image to %d %d in the new image\n", newstartX, newstartY););

  if (resize) {
    DEBUG_CODE(1, fprintf(stderr, "Resizing\n"););
    enlargeCanvas(img, newwidth, newheight, newstartX, newstartY);
    *xoffset = newstartX;
    *yoffset = newstartY;
  } else {
    *xoffset = 0;
    *yoffset = 0;
  }

  *x = desiredupperLeftX;
  *y = desiredupperLeftY;

  if (*x < usedRegion->ulx) usedRegion->ulx = *x;
  if (*y < usedRegion->uly) usedRegion->uly = *y;
  if (*x + featureWidth > usedRegion->lrx) usedRegion->lrx = *x + featureWidth;
  if (*y + featureWidth > usedRegion->lry) usedRegion->lry = *y + featureWidth;
  
  /* clean up */
  freeStandardLocs(standardlocs);
} /* placeFeature */



/*****************************************************************************
 * Resize an image canvas to make it larger. The old image is placed
 * within the new image at Xplace,Yplace (upper left). This directly
 * manipulates the gd image structure!!
 *****************************************************************************/
void enlargeCanvas(gdImagePtr img,
		   int newXSize,
		   int newYSize,
		   int Xplace, /* where to move the old image to in the new image */
		   int Yplace)
{

//#define OLDWAY
#ifndef OLDWAY

  int i;
  int oldXSize = gdImageSX(img);
  int oldYSize = gdImageSY(img);

  if (newXSize < oldXSize || newYSize < oldYSize) die("enlargeCanvas: Can't make image smaller");
  if (Xplace > newXSize || Yplace > newYSize) die("enlargeCanvas: Can't place old image in resized version at given coordinates - off the canvas");
  if (Xplace + oldXSize > newXSize || Yplace + oldYSize > newYSize)
    die ("enlargeCanvas: New image (%d x %d) isn't going to be big enough to place old image at (%d, %d): current size is %d x %d.", 
										     newXSize, newYSize, Xplace, Yplace, gdImageSX(img), gdImageSY(img) );
  DEBUG_CODE(1, fprintf(stderr, "enlargeCanvas: Increase size from %d by %d to %d by %d, move to %d %d\n", gdImageSX(img), gdImageSY(img), newXSize, newYSize, Xplace, Yplace););

  // realloc, memmove, memcopy as needed
  if (newYSize > oldYSize) {
    // add rows 
    DEBUG_CODE(1, fprintf(stderr, "Realloc Y to new size by adding rows %d\n", newYSize););
    img->pixels = (unsigned char**)myrealloc(img->pixels, newYSize*sizeof(unsigned char*));
    DEBUG_CODE(1, if (img->pixels == NULL) die("Null pointer\n"););
    for (i=gdImageSY(img); i<newYSize; i++) { // assign new rows we need 
       img->pixels[i] = (unsigned char*)mycalloc(newXSize, sizeof(unsigned char));
    }
  }
  img->sy = newYSize;

  if (newXSize > oldXSize) {
    DEBUG_CODE(1, fprintf(stderr, "Realloc X from %d to new size %d by adding columns\n", gdImageSX(img), newXSize););
    for (i=0; i< gdImageSY(img); i++) {
      img->pixels[i] = (unsigned char*)myrealloc(img->pixels[i], newXSize*sizeof(unsigned char));
    }
  }
  img->sx = newXSize;

#ifdef HAVE_GDIMAGESETCLIP
  gdImageSetClip(img, 0, 0, newXSize, newYSize);
#endif

  // move existing data to the right as necessary 
  if (Xplace != 0 || newXSize > Xplace + oldXSize) {
    DEBUG_CODE(1, fprintf(stderr, "Memmove X to new place %d\n", Xplace););
    for (i=0; i<oldYSize; i++) {
      if (Xplace != 0) {
	memmove(&img->pixels[i][Xplace], &img->pixels[i][0], oldXSize);
	memset(&img->pixels[i][0], 0, Xplace-1); /* set beginning to zero */
      }
      if (newXSize > Xplace + oldXSize) { /* set end to zero */
	memset(&img->pixels[i][Xplace + oldXSize], 0, newXSize - (Xplace + oldXSize));
      }
    }
  }

  /* move existing row pointers down as necessary */
  if (Yplace != 0) {
    unsigned char** temp;
    DEBUG_CODE(1, fprintf(stderr, "Memmove Y to new place %d\n", Yplace););
    temp = (unsigned char**)mymalloc(sizeof(unsigned char*)*Yplace);
    for (i=0; i<Yplace; i++) {
      temp[i] = img->pixels[newYSize -1 - i];
    }
    for (i=newYSize - 1; i - Yplace >= 0; i--) {
      img->pixels[i] = img->pixels[i-Yplace];
    }
    // erase the front rows (replace with previously allocated blank rows)
    for (i=0; i<Yplace; i++) {
      img->pixels[i] = temp[i];
    }
    free(temp);
  }




#else

  /* Old way, about half as fast, and more memory, of resizing  -- color problems? */
  gdImagePtr newimage = gdImageCreate(newXSize, newYSize);
  gdImagePaletteCopy(newimage, img);
  gdImageCopy(newimage, img, Xplace, Yplace, 0, 0, gdImageSX(img), gdImageSY(img));
  *img = *newimage;

#endif
  
} /* enlargeCanvas */

