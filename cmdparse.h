/*****************************************************************************
 * FILE: cmdparse.h
 * AUTHOR: Paul Pavlidis
 * CREATE DATE: 2/2001
 * PROJECT: PLOTKIT
 * DESCRIPTION: Functions for command line parsing along with cmdline.h
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

#endif /* CMDPARSE_H */

