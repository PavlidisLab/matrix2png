/*****************************************************************************
 * FILE: genehash.c
 * AUTHOR: Paul Pavlidis
 * CREATE DATE: 3/2001
 * PROJECT: PLOTKIT
 * DESCRIPTION: Make a hash table of string information. Keys are string ids (strings), data  are strings.
 * Takes a file that has 1 header line (optional), one column of string names, and one column of description (or whatever it is)
 * Some snips of code borrowed from WNG's string-list.c
 *****************************************************************************/
#include "stringhash.h"
#include "hash.h"
#include "utils.h"

/*****************************************************************************
 * Build the table from a file.
 * Input: open file.
 *****************************************************************************/
HASHTABLE_T* buildStringHash(FILE* infile) {
  char this_line[DEFAULT_LINE_LENGTH];
  char* fgets_result;
  char* stringName;
  char* stringDesc;
  char* token;
  int m;
  HASHTABLE_T* return_value;
  HTYPE* tablePtr = NULL;

  /* set up the initial table */
  return_value = (HASHTABLE_T*)buildtable(DEFAULT_TABLE_SIZE, &ALLOCATIONFUNC);

  /* Read the first name. */
  fgets_result = fgets(this_line,  DEFAULT_LINE_LENGTH, infile);
  this_line[strlen(this_line)-1] = 0; // chop

  /* Make sure we got at least one name. */
  if (fgets_result == NULL) {
      die("Couldn't read a single string from the given file.");
  }
  
  while (fgets_result != NULL) {
    
    /* split into string name and description */
    token = (char*)strtok (this_line, STRINGTABLE_DIVIDER);
    stringName = token;
    token = (char*)strtok (NULL, STRINGTABLE_DIVIDER);
    stringDesc = token;

    /* allocate space for the string description */
    insert(return_value, stringName, stringDesc);
    
    /* Read the next name. */
    fgets_result = fgets(this_line,  DEFAULT_LINE_LENGTH, infile);
    this_line[strlen(this_line)-1] = 0; // chop
  }
  return(return_value);
}

/* callback for  buildtable */
void insertString(HASHTABLE_T* hashtable, int index, HTYPE value) {
  int j = 0;
  char* m;
  j = strlen(value);
  j++;
  hashtable->table[index] = (char*)mymalloc(j*sizeof(char));
  strcpy(hashtable->table[index], value);
}
