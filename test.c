#include <stdio.h>
#include "gd.h"
#include "colors.h"
#include <math.h>
#include "colorscalebar.h"

int main() {
  gdImagePtr im;
  FILE *pngout;
  int imagesize = 1000;
  int numcolors = 4;
  int i;
  int place = (int)ceil( (double) ((imagesize-1)/numcolors) );
  int sbwidth;
  int sbheight;

  if (DEBUG) {
    fprintf(stderr, "Debugging is on\n");
  }

  place = 1000/(numcolors+2);
  
  sbwidth = 275;
  sbheight = 20;

  im = gdImageCreate(imagesize, imagesize);
  allocateColors(im, cyan, red, green, 0, numcolors);
  
  drawScaleBar(im, 0, 100, 40, sbheight, sbwidth);
  labelScaleBar(im, 1, 0, 100, 40, sbheight, sbwidth, 0.0, 10.0);

  pngout = fopen("test.png", "wb");  
  gdImagePng(im, pngout);
  fclose(pngout);
  gdImageDestroy(im);
  return 1; 
}

