/*****************************************************************************
 * FILE: text2image.c
 * AUTHOR: Paul Pavlidis
 * CREATE DATE: 2/2001
 * PROJECT: PLOTKIT
 * DESCRIPTION: Turn a text file into a png image
 *****************************************************************************/

#include "gd.h"
#include <stdio.h>
#include <string.h>
#include "string-list.h"
#include "utils.h"
#include "text2png.h"
#include "gdfontt.h"
#include "gdfonts.h"
#include "gdfontmb.h"
#include "gdfontl.h"
#include "gdfontg.h"
#include "cmdline.h"
#include "cmdparse.h"
#include "colors.h"

/*****************************************************************************
 * chooseFont - convert command line flag into font name (gdFontPtr)
 *****************************************************************************/
gdFontPtr chooseFont (char* cmdlineflag) 
{
  if (! strcmp (cmdlineflag, "tiny")) {
    return gdFontTiny;
  } else if (! strcmp (cmdlineflag, "small")) {
    return gdFontSmall;
  } else if (! strcmp (cmdlineflag, "medium")) {
    return gdFontMediumBold;
  } else if (! strcmp (cmdlineflag, "large")) {
    return gdFontLarge;
  } else if (! strcmp (cmdlineflag, "giant")) {
    return gdFontGiant;
  } else {
    return NULL;
  }
} /* chooseFont */



/*****************************************************************************
 * calcTextDimensions - calculate the space needed for the text
 *****************************************************************************/
void calcTextDimensions (STRING_LIST_T* strings,
			 int numtodo,
			 BOOLEAN_T vertical,
			 int padding,
			 int linespacing, /* extra linespacing */
			 gdFontPtr font,
			 int *width,
			 int *height)
{
  int numstrings;
  int maxstring;

  maxstring = max_string_length(strings);
  numstrings = get_num_strings(strings);
  if (numstrings < numtodo) {
    //    die ("Request to calculate space for more strings than are available (%d requested, %d available)\n", numtodo, numstrings);
  } else if (numstrings > numtodo) {
    numstrings = numtodo;
  }
  DEBUG_CODE(1, fprintf(stderr, "Max string is %d; there are %d strings in the list, we are using %d\n", maxstring, get_num_strings(strings), numstrings););

  /* calculate width and height of the image. If vertical, the
     gdImagedimensions are reversed from the usual */
  *width = maxstring*font->w + 2*padding;
  *height = numstrings*font->h + 2* padding + numstrings*linespacing;

  if (*width > WARNINGSIZE || *height > WARNINGSIZE) {
    if (verbosity >= NORMAL_VERBOSE)
      fprintf(stderr, "Warning: Image is larger than %d pixels wide and/or high. It may not display properly in some web browsers\n", WARNINGSIZE);
  }

  if (vertical) {
    int temp;
    temp = *width;
    *width = *height;
    *height = temp;
  }

  DEBUG_CODE(1,	fprintf(stderr, "%d is the longest string, so width is %d, %d strings\n", maxstring, *width, numstrings););

  
} /*calcTextDimensions */


/******************************************************************************
 * stringlist2image - takes a string list structure							      
 *****************************************************************************/
void stringlist2image (gdImagePtr img,
		       STRING_LIST_T* strings,
		       int numtodo,
		       BOOLEAN_T rightJustify,
		       BOOLEAN_T vertical,		       
		       int padding, /* extra pixels at start of text */
		       int linespacing,
		       int initX,
		       int initY,
		       gdFontPtr font)
{
  int numstrings;
  int i;
  char* string;
  int backgroundColor, textColor;
  int width, height;

  numstrings = get_num_strings(strings);
  if (numstrings < numtodo) {
    //    die ("Request to draw more strings than are available\n");
  } else if (numstrings > numtodo) {
    numstrings = numtodo;
  }
  if (rightJustify){;}  /* avoid compiler warning. Not using this right now, and might be able to remove it */

  /* this is already done in some situations */
  calcTextDimensions(strings, numtodo, vertical, padding, linespacing, font, &width, &height);
  
  /* allocate image and colors if image is null; otherwise add to existing image */
  if (img == NULL) {
    img = gdImageCreate(width, height);
    backgroundColor = gdImageColorAllocate(img, 255, 255, 255);
    textColor = gdImageColorAllocate(img, 0, 0, 0);
  } else {
    int textIntensity;
    textIntensity = chooseContrastingColor(img);
    textColor = gdImageColorClosest(img, textIntensity, textIntensity, textIntensity);
  }

  if (vertical) {
    width+=initX; 
    height+=initY;
  } else {
    width+=initX; 
    height+=initY;
  }

  /* draw the text. All the tokenizing business is because the gd
     fonts don't render tabs as blanks */
  for (i=0; i< numstrings; i++) {
    char *word;
    int currentpos;
    if (vertical) {
      currentpos = initY;
    } else {
      currentpos = initX;
    }
    string = get_nth_string(i, strings);
    word = strtok(string, DIVIDERCHARS);
    while (word != NULL) {
      if (vertical) {
	gdImageStringUp(img, font, initX + i*(linespacing + font->h), currentpos + height - padding, (unsigned char*)word, textColor);
	currentpos += (strlen(word)+DIVIDERWIDTH) * font->h;
      } else {
	gdImageString(img, font, currentpos + padding, initY + i*(linespacing + font->h), (unsigned char*)word, textColor);
	currentpos += (strlen(word)+DIVIDERWIDTH) * font->w;
      }
      word = strtok(NULL, DIVIDERCHARS);
    }
  }
} /* stringlist2image */


/*
 * main
 *
 */
#ifdef TEXTMAIN
VERBOSE_T verbosity = NORMAL_VERBOSE;

int main (int argc, char **argv) {
  
  char* textFilename;
  char* fontSize = NULL;
  FILE* textFile;
  STRING_LIST_T* stringlist;
  char** rawstrings;
  int numstrings;
  BOOLEAN_T vertical = FALSE;
  BOOLEAN_T rightJustify = FALSE;
  gdFontPtr font;
  gdImagePtr img;
  FILE* pngout;
  int padding = TEXTPADDING;
  int linespacing = DEFAULTLINESPACE;
  int initX, initY;

  DO_STANDARD_COMMAND_LINE
    (1,
     SIMPLE_CFLAG_OPTN(1, r, rightJustify);
     SIMPLE_CFLAG_OPTN(1, v, vertical);
     DATA_OPTN(1, text, <file> [required],
	       textFilename = _OPTION_);
     DATA_OPTN(1, font, <font: default small>,
	       fontSize = _OPTION_);
     DATA_OPTN(1, pad, <pixels: default 5>,
	       padding = atoi(_OPTION_));
     DATA_OPTN(1, linespace, <pixels: default 2>,
	       linespacing = atoi(_OPTION_));
     );
  
  if (open_file(textFilename, "r", FALSE, "text", "the text", &textFile) == 0) exit(1);

  /* read the list and justify if necessary */
  stringlist = read_string_list(textFile);
  if (rightJustify) {
    right_justify_string_list(stringlist);
  }
  
  /* choose font */
  if (fontSize != NULL) {
    font = chooseFont(fontSize);
    if (font == NULL) die("Illegal font choice: must be tiny,small,medium,large or giant");
  } else {
    font = DEFAULTFONT;
  }

  /* write the image from the string list */
  initX = 20;
  initY = 110;
  stringlist2image(img, stringlist, rightJustify, vertical, padding, linespacing, initX, initY, font);
  if (img != NULL) {
    pngout = fopen("testfromstringlist.png", "wb");
    gdImagePng(img, pngout);
    fclose(pngout);
    gdImageDestroy(img);
  } else {
    free_string_list(stringlist);
    die("Could not make image\n");
  }

  free_string_list(stringlist);

  return 0;
}
#endif
/*
 * text2image.c
 */
