/*****************************************************************************
 * FILE: cmdparse.h
 * AUTHOR: Paul Pavlidis
 * CREATE DATE: 2/2001
 * PROJECT: PLOTKIT
 * DESCRIPTION: Functions for command line parsing along with cmdline.h
 * Copyright (c) Columbia University
 *****************************************************************************/

#ifndef CMDPARSE_H
#define CMDPARSE_H

#include <string.h>
#include "cmdparse.h"

#define DIVIDER ":"

/*****************************************************************************
 * Deal with commands given in the x:y format  (where : is defined in DIVIDER)
 *****************************************************************************/
void parseValuePair (char* command, char* divider, double* firstVal, double* secondVal);


/*****************************************************************************
 * Deal with commands given in the r:g:b format  (where : is defined in DIVIDER)
 *****************************************************************************/
void parseValueSeries (char* command, char* divider, int* values, int maxToRead);

#endif /* CMDPARSE_H */

