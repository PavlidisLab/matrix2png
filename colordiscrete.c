/*****************************************************************************
 * FILE: colordiscrete.c
 * AUTHOR: Paul Pavlidis
 * CREATE DATE: 6/2002
 * PROJECT: PLOTKIT
 * DESCRIPTION: Functions relating to using discretized color maps.
 *****************************************************************************/

#include "gd.h"
#include "utils.h"
#include "colordiscrete.h"
#include "colors.h"
#include <stdio.h>
#include <string.h>

DISCRETEMAP_T* readDiscreteMap(FILE* file) 
{
  char one_row[MAX_DROW];
  char* string_ptr;
  int num_scanned;
  int one_value;
  DISCRETEMAP_T* return_value;
  return_value = allocateDiscreteMap();

  while(1) {
    if (fgets(one_row, MAX_DROW, file) == NULL) {
      break;
    }

    if (return_value->maxcount <= return_value->count) {
      growDiscreteMap(return_value);
    }

    string_ptr = strtok(one_row, "\t");
    if (strcmp(string_ptr, DEFAULT_DISCRETE_STRING) == 0) {
      string_ptr = strtok(NULL, "\t\n\r");
      if (string_ptr == NULL)	die("No color read for discrete map \n");
      string2color(string_ptr, return_value->default_colorcode);
    } else {
      num_scanned = sscanf(string_ptr, "%d", &one_value);
      if (num_scanned == 0) {
	die("Missing or invalid value in the discrete map file");
      }
      //      return_value->values[return_value->count] = one_value; // not needed yet.
      string_ptr = (char*)strtok(NULL, "\t\n\r");
      if (string_ptr == NULL)	die("No color read for discrete map\n");
      string2color(string_ptr, return_value->colors[return_value->count]);
      return_value->count++;
    }
  }
  if (return_value->count > MAXCOLORS) {
    // because one color may correspond to the same color, this isn't really accurate, but it is simple.
    die("Too many colors chosen in discrete map"); // this isn't really possible since the number of colors in the palette is small.
  }
  return(return_value);
}


DISCRETEMAP_T* allocateDiscreteMap(void)
{
  int i;
  DISCRETEMAP_T* return_value = NULL;
  return_value = (DISCRETEMAP_T*)mymalloc(sizeof(DISCRETEMAP_T));
  return_value->colors = (colorV_T**)mymalloc(sizeof(colorV_T*)*DMAP_INITIAL_COUNT);
  for (i=0; i<DMAP_INITIAL_COUNT; i++) {
    return_value->colors[i] = initColorVByName((color_T)0);
  }
  return_value->default_colorcode = initColorVByName((color_T)0);
  string2color(DEFAULT_DISCRETE_COLOR, return_value->default_colorcode);
  return_value->count = 0;
  return_value->maxcount = DMAP_INITIAL_COUNT;
  return(return_value);
}

void freeDiscreteMap(DISCRETEMAP_T* dmap) {
  //  free(dmap->values);
  int i;
  for (i=0; i<dmap->count; i++) {
    free(dmap->colors[i]);
  }
  free(dmap->default_colorcode);
  free(dmap->colors);
  free(dmap);
}

void growDiscreteMap(DISCRETEMAP_T* dmap) {
  int newsize = dmap->count + DMAP_INITIAL_COUNT;
  int i;
  DEBUG_CODE(1, fprintf(stderr, "Growing map\n"););
  //  dmap->values = (int*)myrealloc(dmap->values, sizeof(int)*newsize);
  dmap->colors = (colorV_T**)myrealloc(dmap->colors, newsize*sizeof(colorV_T*));
  for (i=dmap->count; i<newsize; i++) {
    dmap->colors[i] = initColorVByName((color_T)0);
  }
  dmap->maxcount = newsize;
  DEBUG_CODE(1, fprintf(stderr, "Grew map\n"););
}

void allocateColorsDiscrete (gdImagePtr img, 
			     DISCRETEMAP_T* dmap,
			     colorV_T* backgroundColor, 	
			     colorV_T* missingColor
			     )
{
  int backgroundRed = 0;
  int backgroundGreen = 0;
  int backgroundBlue = 0;
  int missingRed = 0;
  int missingGreen = 0;
  int missingBlue = 0;
  int r,g,b, i;

  color2rgb(backgroundColor, &backgroundRed, &backgroundGreen, &backgroundBlue);
  color2rgb(missingColor, &missingRed, &missingGreen, &missingBlue);

  /* allocate the background color, index 0 */
  checkColor(gdImageColorAllocate(img, backgroundRed, backgroundGreen, backgroundBlue));

  /* Allocate white, index 1 */
  checkColor(gdImageColorAllocate(img, 255, 255, 255));

  /* Allocate black, index 2 */
  checkColor(gdImageColorAllocate(img, 0, 0 ,0));

  /* Allocate grey, index 3 */
  checkColor(gdImageColorAllocate(img, 128, 128, 128));

  /* allocate the missing value color, index 4 */
  checkColor(gdImageColorAllocate(img, missingRed, missingGreen, missingBlue));

  /* allocate the default color, index 5 */
  color2rgb(dmap->default_colorcode, &r, &g, &b);
  checkColor(gdImageColorAllocate(img, r, g, b));
  
  for (i=0; i<dmap->count; i++) {
    color2rgb(dmap->colors[i], &r, &g, &b);
    checkColor(gdImageColorAllocate(img, r, g, b));
  }
  
}


/* 
   colordiscrete.c 
*/
