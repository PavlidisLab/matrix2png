#include <stdio.h>  /* Bring in standard I/O so we can output the PNG to a file */
#include "gd.h"
#include "colors.h"
#include <math.h>
#include "debug.h"

int main() {
  gdImagePtr im;
  FILE *pngout;
  int imagesize = 1000;
  int numcolors = 2;
  int i;
  int place = (int)ceil( (double) ((imagesize-1)/numcolors) );
  
  if (DEBUG) {
    fprintf(stderr, "Debugging is on\n");
  }

  place = 1000/(numcolors+2);
  
  im = gdImageCreate(imagesize, imagesize);
  allocateColors(im, black, red, green, 0, numcolors);

  for (i = 0; i< numcolors + 2;i++) {
    gdImageLine(im, i*place, i*place, (i+1)*place, (i+1)*place, i);
  }

  pngout = fopen("test.png", "wb");  
  gdImagePng(im, pngout);
  fclose(pngout);
  gdImageDestroy(im);
  return 1; 
}









