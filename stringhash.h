/*****************************************************************************
 * FILE: stringhash.h
 * AUTHOR: Paul Pavlidis
 * CREATE DATE: 3/2001
 * PROJECT: PLOTKIT
 * DESCRIPTION: Make a hash table of string information. Keys are string ids (strings), data are strings.
 * Takes a file that has 1 header line (optional), one column of string names, and one column of description.
 * The keys are stored in the hash table only. Access to the table is through wrappers to the usual hash functions.
 *****************************************************************************/

#ifndef STRINGHASH_H
#include "hash.h"

#ifndef STRINGHASH
#define STRINGHASH
#endif


#define STRINGTABLE_DIVIDER  "\t"
#define DEFAULT_STRING_NAME_LENGTH 25

#ifndef KEYLENGTH
#define KEYLENGTH DEFAULT_STRING_NAME_LENGTH
#endif

#ifndef DEFAULT_LINE_LENGTH
#define DEFAULT_LINE_LENGTH 1000
#endif

#define ALLOCATIONFUNC insertString

/*****************************************************************************
 * Build the table from a file. We need to allocate extra space for the strings as we go.
 *****************************************************************************/
HASHTABLE_T* buildStringHash(FILE* infile);



/*****************************************************************************
 * Free the table.
 *****************************************************************************/



/*****************************************************************************
 * Insert a value
 *****************************************************************************/


/*****************************************************************************
 * Store a value
 *****************************************************************************/



/*****************************************************************************
 * Allocate a string
 *****************************************************************************/
void insertString(HASHTABLE_T* hashtable, int index, HTYPE value);


#endif /* stringhash_h */
