/*****************************************************************************
 * FILE: cmdparse.c
 * AUTHOR: Paul Pavlidis
 * CREATE DATE: 2/2001
 * PROJECT: PLOTKIT
 * DESCRIPTION: Functions for command line parsing along with cmdline.h
 *****************************************************************************/

#include <string.h>
#include "cmdparse.h"
#include "utils.h"

/*****************************************************************************
 * Deal with commands given in the x:y format							      
 *****************************************************************************/
void parseValuePair (char* command, char* divider, double* firstVal, double* secondVal)
{
  char *token;
  token = strtok (command, divider);
  *firstVal = atof(token);
  token = strtok (NULL, divider);
  *secondVal = atof(token);
} /* parseValuePair */


/*
 * cmdparse.c
 */

