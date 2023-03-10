/*****************************************************************************
 * FILE: cmdparse.c
 * AUTHOR: Paul Pavlidis
 * CREATE DATE: 2/2001
 * PROJECT: PLOTKIT
 * DESCRIPTION: Functions for command line parsing along with cmdline.h
 * Copyright (c) Columbia University
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
  int length = (int)strlen(command);
  char buf[length];
  strcpy(buf, command);
  token = strtok (buf, divider);

  *firstVal = atof(token);
  token = strtok (NULL, divider);
  if(token == NULL) {
    die("Command line parser: Failed to read a second value from %s.", command);
  } 

  *secondVal = atof(token);

} /* parseValuePair */

/*****************************************************************************
 * Deal with commands given in the r:g:b format etc (where : is defined in DIVIDER)
 *****************************************************************************/
void parseValueSeries (char* command, char* divider, int* values, int maxToRead)
{
  char *token;
  int length = (int)strlen(command);
  int i = 0;
  char buf[length];

  strcpy(buf, command);
  token = strtok(buf, divider);
  values[i] = atoi(token); // should use scanf?
  if (maxToRead > 1) {
    for (++i; i<maxToRead; i++) {
      token = strtok(NULL, divider);
      if(token == NULL) {
	die("Command line parser: Failed to read a value from %s.", command);
      }
      values[i] = atoi(token);
    }
  }
} /* parseValuePair */


/*
 * cmdparse.c
 */

