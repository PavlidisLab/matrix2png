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
#include "string-list.h"
#include <stdio.h>
#include <string.h>
#include "hash.h"
#include "matrix2png.h"

/*****************************************************************************
 * Read a discrete map from a file, or assign the default map.
 *****************************************************************************/
DISCRETEMAP_T* readDiscreteMap(FILE* file) 
{
  char one_row[MAX_DROW];
  char* string_ptr;
  int num_scanned;
  int one_value;
  char colorbuf[MAX_DROW];
  char buf[10];

  DISCRETEMAP_T* return_value;
  return_value = allocateDiscreteMap();

  if (file == NULL) { // define the default map.
    int i;
    color_T (colorary)[] = DEFAULT_DISCRETE_MAPARY; // see colordiscrete.h for definition.

    for (i=0; colorary[i] > 0; i++) {
      if (return_value->maxcount <= return_value->count) {
	growDiscreteMap(return_value);
      }
      return_value->colors[return_value->count]->namedcolor = colorary[i];
      return_value->values[return_value->count] = i;
      sprintf(buf, "%d", return_value->count + 1);
      DEBUG_CODE(1, fprintf(stderr, "No label so got %s\n", buf););
      add_string(buf, return_value->labels); // use the integer value
      return_value->count++;
    }
  } else {

    /* Read the first row, which is a header.*/
    fgets(one_row, MAX_DROW, file);

    while(1) {
      if (fgets(one_row, MAX_DROW, file) == NULL) {
	break;
      }
      
      if (return_value->maxcount <= return_value->count) {
	growDiscreteMap(return_value);
      }
      
      string_ptr = strtok(one_row, "\t"); // the value

      if (string_ptr == NULL) die("Failed to read a value from discrete map file\n");
      
      if (strcmp(string_ptr, DEFAULT_DISCRETE_STRING) == 0) { // default value.
	string_ptr = strtok(NULL, "\t\n\r"); // the color

	if (string_ptr == NULL)	
	  die("No color read for discrete map \n");

	strcpy(colorbuf, string_ptr);
	string_ptr = strtok(NULL, "\t\n\r"); // the label, if any

	if (string_ptr != NULL) {
	  DEBUG_CODE(1, fprintf(stderr, "Got %s\n", string_ptr););
	  if (strlen(string_ptr) > DEFAULT_DISCRETE_LABEL_BUFSIZE)
	    die("Default discrete label is too long. Need to increase defined DEFAULT_DISCRETE_LABEL_BUFSIZE");

	  strcpy(return_value->defaultlabel, string_ptr);
	}

	string2color(colorbuf, return_value->default_colorcode);
      } else { // regular value.
	num_scanned = sscanf(string_ptr, "%d", &one_value);

	if (num_scanned == 0) {
	  die("Missing or invalid value in the discrete map file");
	}

	string_ptr = strtok(NULL, "\t\n\r"); // the color
	if (string_ptr == NULL)	die("No color read for discrete map\n");
	DEBUG_CODE(1, fprintf(stderr, "Got %s\n", string_ptr););
	strcpy(colorbuf, string_ptr);
	string_ptr = strtok(NULL, "\t\n\r"); // the label

	if (string_ptr != NULL) {
	  add_string(string_ptr, return_value->labels); // use the label
	  DEBUG_CODE(1, fprintf(stderr, "got %s\n", string_ptr););
	} else { // make up a label.
	  sprintf(buf, "%d", one_value);
	  DEBUG_CODE(1, fprintf(stderr, "No label so got %s\n", buf););
	  add_string(buf, return_value->labels); // use the integer value
	}
	return_value->values[return_value->count] = one_value; 
	return_value->consecints[return_value->count] = return_value->count; // we need this...
	string2color(colorbuf, return_value->colors[return_value->count]);
	sprintf(buf, "%d", return_value->values[return_value->count]);
	insert(return_value->mapping, buf, &(return_value->consecints[return_value->count]));
	return_value->count++;
      }
    }
    if (return_value->count > MAXCOLORS) {
      // because one color may correspond to the same color, this isn't really accurate, but it is simple.
      die("Too many colors chosen in discrete map"); // this isn't really possible since the number of colors in the palette is small.
    }
  }
  return(return_value);
}


/*****************************************************************************
 * allocate memory and initialize a discrete mapping data structure
 *****************************************************************************/
DISCRETEMAP_T* allocateDiscreteMap(void)
{
  int i;
  DISCRETEMAP_T* return_value = NULL;

  return_value = (DISCRETEMAP_T*)mymalloc(sizeof(DISCRETEMAP_T));
  return_value->colors = (colorV_T**)mymalloc(sizeof(colorV_T*)*DMAP_INITIAL_COUNT);
  return_value->values = (int*)mymalloc(sizeof(int)*DMAP_INITIAL_COUNT);
  return_value->consecints = (int*)mymalloc(sizeof(int)*DMAP_INITIAL_COUNT);

  for (i=0; i<DMAP_INITIAL_COUNT; i++) {
    return_value->colors[i] = initColorVByName((color_T)0);
  }

  return_value->labels = new_string_list();

  return_value->default_used = FALSE;
  return_value->default_colorcode = initColorVByName((color_T)0);
  string2color(DEFAULT_DISCRETE_COLOR, return_value->default_colorcode);

  return_value->count = 0;
  return_value->maxcount = DMAP_INITIAL_COUNT;
  return_value->usedValues = inittable(DMAP_INITIAL_COUNT);
  return_value->mapping = inittable(DMAP_INITIAL_COUNT);
  if (strlen(DEFAULT_DISCRETE_LABEL) > DEFAULT_DISCRETE_LABEL_BUFSIZE)
    die("Default discrete label is too long. Need to increase defined DEFAULT_DISCRETE_LABEL_BUFSIZE");
  strcpy(return_value->defaultlabel, DEFAULT_DISCRETE_LABEL);

  return(return_value);
}

/*****************************************************************************
 *Free a discrete map 
 *****************************************************************************/
void freeDiscreteMap(DISCRETEMAP_T* dmap) {
  int i;
  for (i=0; i<dmap->count; i++) {
    free(dmap->colors[i]);
  }
  free_string_list(dmap->labels);
  free(dmap->default_colorcode);
  free(dmap->colors);
  freetable(dmap->usedValues);
  freetable(dmap->mapping);
  free(dmap->values);
  free(dmap->consecints);
  free(dmap);
}

/*****************************************************************************
 * Make more room in a discrete map.
 *****************************************************************************/
void growDiscreteMap(DISCRETEMAP_T* dmap) {
  int newsize = dmap->count + DMAP_INITIAL_COUNT;
  int i;
  DEBUG_CODE(1, fprintf(stderr, "Growing map\n"););
  dmap->values = (int*)myrealloc(dmap->values, sizeof(int)*newsize);
  dmap->consecints = (int*)myrealloc(dmap->consecints, sizeof(int)*newsize);
  dmap->colors = (colorV_T**)myrealloc(dmap->colors, newsize*sizeof(colorV_T*));

  for (i=dmap->count; i<newsize; i++) {
    dmap->colors[i] = initColorVByName((color_T)0);
  }

  dmap->maxcount = newsize;
  DEBUG_CODE(1, fprintf(stderr, "Grew map\n"););
}

/*****************************************************************************
 * Check the data for actual use of the values. Used for scale bar
 *****************************************************************************/
void checkDiscreteUsedValues(MATRIXINFO_T* matrixInfo)
{
  int i,j;
  static int notnull = 1;
  int* k;
  char buf[100];
  for (i=0; i<matrixInfo->numrows; i++) {
    for(j=0; j<matrixInfo->numcols; j++) {

      if (isnan(get_matrix_cell(i,j,matrixInfo->matrix)))
	continue;
      
      sprintf(buf, "%d", (int)get_matrix_cell(i,j,matrixInfo->matrix));
      k = (int*)find(matrixInfo->discreteMap->mapping, buf);
      if (k == NULL) {
	matrixInfo->discreteMap->default_used = TRUE;
	//	fprintf(stderr, "Default is needed for %s\n", buf);
      } else {
	insert(matrixInfo->discreteMap->usedValues, buf, &notnull);
	//	fprintf(stderr, "Found usage of %s\n", buf);
      }
    }
  }
  DEBUG_CODE(1, fprintf(stderr, "There are %d values used (not including the default)\n", matrixInfo->discreteMap->usedValues->num_items););
}


/*****************************************************************************
 * Add colors to gdimage pallete for discete map.
 *****************************************************************************/
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
    DEBUG_CODE(1, fprintf(stderr, "Discrete: Allocating %d %d %d\n", r,g,b););
    checkColor(gdImageColorAllocate(img, r, g, b));
  }
}


/* 
   colordiscrete.c 
*/
