/*****************************************************************************
 * FILE: colormap.c
 * AUTHOR: Paul Pavlidis
 * CREATE DATE: 5/2001
 * PROJECT: PLOTKIT
 * DESCRIPTION: Allocation of colors according to predefined maps.
 *****************************************************************************/

#include "gd.h"
#include "utils.h"
#include "colors.h"
#include "colormap.h"
#include "matrix2png.h"

/* globals */
char *blackbody[] = {"black", "darkred", "orange", "yellow", "white", NULL};
char *spectrum[] = {"black", "red", "orange", "yellow", "violet", NULL};
char *redgreen[] = {"red", "darkred", "black", "darkgreen", "green", NULL};
char *greenred[] = {"green", "darkgreen", "black", "darkred", "red", NULL};
char *twocolorarray[] = {"green", "yellow", "red", NULL};

/* A selection of maps designed by Cynthia Brewer. For information on these color
   maps, see
   http://www.personal.psu.edu/faculty/c/a/cab38/ColorBrewerBeta2.html */
char *brewerseq1[] = {"255:255:217", "237:248:177", "199:233:180", "127:205:187", "65:182:196", "29:145:192", "56:108:176", "37:52:148", "8:29:88", NULL};
char *brewerseq2[] = {"", "", "", "", "", "", "", "", "", NULL};
char *brewerseq3[] = {"247:244:249", "231:225:239", "217:197:224", "206:161:206", "224:113:182", "231:29:133", "199:1:82", "152:0:67", "103:0:31", NULL};
char *brewerseq4[] = {"", "", "", "", "", "", "", "", "", NULL};
char *brewerseq5[] = {"", "", "", "", "", "", "", "", "", NULL};

char *brewerdiv1[] = {"179:66:3", "224:130:20", "253:184:99", "254:224:182", "247:247:247", "216:218:235", "178:171:210", "128:115:172", "73:29:130", NULL};
char *brewerdiv2[] = {"140:81:10", "191:129:45", "223:194:125", "246:232:195", "245:245:245", "199:234:229", "128:205:193", "53:151:143", "1:102:94", NULL};
char *brewerdiv3[] = {"99:27:121", "153:112:171", "194:165:207", "231:212:232", "247:247:247", "217:240:211", "168:216:183", "90:174:97", "27:120:55", NULL};
char *brewerdiv4[] = {"178:0:32", "215:109:80", "244:165:130", "253:219:199", "255:255:255", "224:224:224", "186:186:186", "135:135:135", "77:77:77", NULL};
char *brewerdiv5[] = {"215:48:39", "244:109:67", "253:174:97", "254:224:139", "255:255:191", "217:239:139", "166:217:106", "102:189:99", "17:147:82", NULL};

/*****************************************************************************
 * allocateColorMap
 *****************************************************************************/
void allocateColorMap (
		       gdImagePtr img,
		       colorV_T* backgroundColor,
		       colorV_T* missingColor,
		       int colorMap,
		       MATRIXINFO_T* matrixInfo
		       )
{
  double r,g,b; /* color components */
  int startRed = 0;
  int startGreen = 0;
  int startBlue = 0;
  int backgroundRed = 0;
  int backgroundGreen = 0;
  int backgroundBlue = 0;
  int missingRed = 0;
  int missingGreen = 0;
  int missingBlue = 0;
  int endRed = 0;
  int endGreen = 0;
  int endBlue = 0;
  int numcolorsPerStep = 0;

  char** colorMapList = NULL;
  colorV_T* minColor = initColorVByName((color_T)0);
  colorV_T* maxColor = initColorVByName((color_T)0);
  char* maxColorString;
  char* minColorString;
  
  int i; /* counter */
  int nummapcols = 0;

  /* step sizes for moving from low to high values in the scale */
  double redStepSize = 0.0;
  double greenStepSize = 0.0;
  double blueStepSize = 0.0;

  DEBUG_CODE(1, fprintf(stderr, "Allocating map\n"););
  colorMapList = getColorMap(colorMap);

  /* clear the entire color table for this image */
  for (i=0; i<gdMaxColors; i++) {
    gdImageColorDeallocate(img, i);
  }

  /* get background color and missing value color */
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

  i=0;
  while (colorMapList[i] != NULL) {
    nummapcols++;
    i++;
  }

  // if there aren't enough colors selected to properly do the map, adjust
  if (matrixInfo->numColors < nummapcols) 
    matrixInfo->numColors = nummapcols;

  numcolorsPerStep = ceil( (double)(matrixInfo->numColors) / (double)nummapcols);
  DEBUG_CODE(1, fprintf(stderr, "There are %d colors in the map; Gonna allocate %d per step\n", nummapcols, numcolorsPerStep););
  r=g=b=0;
  for (i=0; i < nummapcols-1 ; i++) { // we actually allocate colors in pairs going up the map, plus any colors needed in between.
    minColorString = colorMapList[i];
    maxColorString = colorMapList[i+1];
    DEBUG_CODE(1, fprintf(stderr, "Gonna allocate %s and %s\n", minColorString, maxColorString););
    string2color(minColorString, minColor);
    string2color(maxColorString, maxColor);
    color2rgb(minColor, &startRed, &startGreen, &startBlue);
    color2rgb(maxColor, &endRed, &endGreen, &endBlue);
    r = startRed;
    g = startGreen;
    b = startBlue;
    redStepSize = getStepSize(r, endRed, numcolorsPerStep+1);
    greenStepSize = getStepSize(g, endGreen, numcolorsPerStep+1);
    blueStepSize = getStepSize(b, endBlue, numcolorsPerStep+1);    
    DEBUG_CODE(1, fprintf(stderr, "Red step size: %f Green step size: %f Blue step size: %f\n", redStepSize, greenStepSize, blueStepSize); );
    DEBUG_CODE(1, fprintf(stderr, "Start Red: %f  Start Green: %f  Start Blue: %f\n", r,g,b););
    
    endRed-=redStepSize;
    endGreen-=greenStepSize;
    endBlue-=blueStepSize;
    DEBUG_CODE(1, fprintf(stderr, "End Red: %f  End Green: %f  End Blue: %f\n", (double)endRed, (double)endGreen, (double)endBlue););

    makeColors(img, r, g, b, redStepSize, greenStepSize, blueStepSize, i < nummapcols-2 ? numcolorsPerStep : numcolorsPerStep+1);
  }

} /* allocateColorMap */


/*****************************************************************************
 * 
 *****************************************************************************/
char** getColorMap (int colorMap) {
  DEBUG_CODE(1, fprintf(stderr, "Requested Color map is %d\n", colorMap););
  switch (colorMap) {
  case 1 :
    return blackbody;
  case 2 :
    return spectrum;
  case 3 :
    return redgreen;
  case 4 :
    return greenred;
  case 5 :
    return twocolorarray;
  case 6 :
    return brewerdiv1;
  case 7 :
    return brewerdiv2;
  case 8 :
    return brewerdiv3;
  case 9 :
    return brewerdiv4;
  case 10 :
    return brewerdiv5;
  case 11 :
    return brewerseq1;
  case 12 :
    return brewerseq2;
  case 13 :
    return brewerseq3;
  case 14 :
    return brewerseq4;
  case 15 :
    return brewerseq5;
  default:
    colorError(invalid);
    break;
  }
  return(0);
} /* getColorMap */
