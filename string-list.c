/**************************************************************************
 * FILE: string-list.c
 * AUTHOR: William Grundy, Paul Pavlidis
 * CREATE DATE: 12-22-98, 2/2001
 * PROJECT: PHYLO
 * DESCRIPTION: Data structure for manipulating a list of strings. 
 **************************************************************************/
#include "string-list.h"
#include "utils.h"
#include <string.h>


/*************************************************************************
 * Primary data structure.
 *************************************************************************/
struct string_list_t {
  int     num_strings;    /* Number of strings in the array. */
  int     max_strings;    /* Total amount of memory allocated. */
  int     longest_string; /* Length of the longest allowed string. */
  char** strings;        /* The strings themselves. */
};

/*************************************************************************
 * Allocate dynamic memory for a string list.
 *************************************************************************/
#define DEFAULT_MAX_STRINGS 10
#define DEFAULT_STRING_LENGTH  10
#define DEFAULT_LINE_LENGTH 5000 /* max length of a line read from a file */
STRING_LIST_T* new_string_list
  ()
{
  STRING_LIST_T* new_list;  /* The list being created. */
  int             i_string;  /* Index into the list. */

  new_list = (STRING_LIST_T*)mycalloc(1, sizeof(STRING_LIST_T));
  new_list->num_strings = 0;
  new_list->max_strings = DEFAULT_MAX_STRINGS;
  new_list->longest_string = DEFAULT_STRING_LENGTH;
  new_list->strings = (char**)mycalloc(DEFAULT_MAX_STRINGS, sizeof(char*));
  for (i_string = 0; i_string < DEFAULT_MAX_STRINGS; i_string++) {
    new_list->strings[i_string] = (char*)mycalloc(DEFAULT_STRING_LENGTH + 1,
						   sizeof(char));
  }    
  return(new_list);
}

/*************************************************************************
 * Is the given list a null list?
 *************************************************************************/
static void check_null_list
  (STRING_LIST_T*  a_list)
{
  if (a_list == NULL) {
    die("Attempted to access null string list.\n");
  }
}
/*************************************************************************
 * Get the length of the longest string in a list.
 *************************************************************************/
int max_string_length
  (STRING_LIST_T*  a_list)
{
  check_null_list(a_list);

  return(a_list->longest_string);
}

/*************************************************************************
 * Get the total number of strings in a list.
 *************************************************************************/
int get_num_strings
  (STRING_LIST_T*  a_list)
{
  check_null_list(a_list);

  return(a_list->num_strings);
}

/*************************************************************************
 * Make sure each string in the list is long enough.
 *************************************************************************/
static void resize_string_list
  (int            new_length,
   STRING_LIST_T* a_list)
{
  int i_string;
      
  if (new_length > a_list->longest_string) {
    a_list->longest_string = new_length + 2; /* I don't know why, but the extra byte is needed on i86 solaris. -- PP */
    for (i_string = 0; i_string < a_list->max_strings; i_string++) {
      a_list->strings[i_string] 
	= (char*)myrealloc(a_list->strings[i_string], 
			   a_list->longest_string * sizeof(char) ); 
    }
  }
}						    

/*************************************************************************
 * Get the nth string from a list.
 *************************************************************************/
char* get_nth_string
  ( int             n,
   STRING_LIST_T*  a_list)
{
  check_null_list(a_list);

  if (n > a_list->max_strings) {
    die("Attempted to access string beyond end of list.\n");
  } else if (n > a_list->num_strings) {
    die("Attempted to access uninitialized string.\n");
  }

  return(a_list->strings[n]);
}

void set_nth_string
  (char*          new_string,            
   int            n,
   STRING_LIST_T* a_list)
{
  /* Check bounds. */
  get_nth_string(n, a_list);

  /* Make all the strings longer if this one is too long. */
  resize_string_list((int)strlen(new_string), a_list);

  /* Copy the string into the list. */
  strcpy(a_list->strings[n], new_string);
}


/*************************************************************************
 * Add a string to the end of a given list.
 *************************************************************************/
void add_string
  (char*     a_string,
   STRING_LIST_T* a_list)
{
  int i_string;
  check_null_list(a_list);

  /* Reallocate space if there isn't any. */
  if (a_list->num_strings >= a_list->max_strings) {
    a_list->strings = (char**)myrealloc(a_list->strings, 
					(a_list->max_strings 
					 + DEFAULT_MAX_STRINGS)
					* sizeof(char*));
    for (i_string = 0; i_string < DEFAULT_MAX_STRINGS; i_string++) {
	a_list->strings[a_list->max_strings + i_string] 
	  = (char*)mycalloc(a_list->longest_string + 1, sizeof(char));
    }
    a_list->max_strings += DEFAULT_MAX_STRINGS;
  }

  /* Make all the strings longer if this one is too long. */
  resize_string_list((int)strlen(a_string), a_list);
  /* Put the string in the list. */
  strcpy(a_list->strings[a_list->num_strings], a_string);
  (a_list->num_strings)++;
}

/*************************************************************************
 * Add a list of strings to the end of a given list.
 *************************************************************************/
void add_strings
  (STRING_LIST_T*  source_list,
   STRING_LIST_T*       target_list)
{
  int i_string;

  check_null_list(source_list);
  check_null_list(target_list);

  for (i_string = 0; i_string < get_num_strings(source_list); i_string++) {
    add_string(get_nth_string(i_string, source_list), target_list);
  }
}

/*************************************************************************
 * Remove a string from a given list.
 *
 * It is an error if the string does not appear in the list.
 *************************************************************************/
void remove_string
  (char*     a_string,
   STRING_LIST_T* a_list)
{
  BOOLEAN_T found_it;    /* Have we found the desired string? */
  char*    this_string; /* The current string. */
  int       i_string;    /* Index of the current string.*/

  check_null_list(a_list);

  /* Make sure the string exists in the list. */
  if (!have_string(a_string, a_list)) {
    die("Attempted to remove string %s from list that doesn't contain it.\n",
	a_string);
  }

  found_it = FALSE;
  for (i_string = 0; i_string < get_num_strings(a_list) - 1; i_string++) {
    this_string = get_nth_string(i_string, a_list);

    /* Is this the string we are looking for? */
    if (strcmp(this_string, a_string) == 0) {
      found_it = TRUE;
    }

    /* Copy the next string into the current location. */
    if (found_it) {
      strcpy(this_string, get_nth_string(i_string + 1, a_list));
    }
  }
  (a_list->num_strings)--;
}

/*************************************************************************
 * Remove a set of strings from a given list.
 *
 * It is an error for the source list to contain any strings that
 * don't appear in the target list.
 *************************************************************************/
void remove_strings
  (STRING_LIST_T*  source_list,
   STRING_LIST_T*       target_list)
{
  int i_string;

  for (i_string = 0; i_string < get_num_strings(source_list); i_string++) {
    remove_string(get_nth_string(i_string, source_list), target_list);
  }
}

/*************************************************************************
 * Does a given string appear in a given list?
 *************************************************************************/
BOOLEAN_T have_string
  (char*           a_string,
   STRING_LIST_T*  a_list)
{
  char* this_string; /* The current string in the list. */
  int    i_string;    /* Index of the current string. */
  
  check_null_list(a_list);

  for (i_string = 0; i_string < get_num_strings(a_list); i_string++) {
    this_string = get_nth_string(i_string, a_list);
    if (strcmp(this_string, a_string) == 0) {
      return(TRUE);
    }
  }
  return(FALSE);
}

/**************************************************************************
 * Make a copy of a string list.
 **************************************************************************/
STRING_LIST_T* copy_string_list
  (STRING_LIST_T* string_list)
{
  int i_string;
  char* this_string;
  STRING_LIST_T* new_list;

  /* Create the new list. */
  new_list = new_string_list();

  /* Add stuff to it. */
  for (i_string = 0; i_string < string_list->num_strings; i_string++) {
    this_string = get_nth_string(i_string, string_list);
    add_string(this_string, new_list);
  }

  /* Return it. */
  return(new_list);
}

/*************************************************************************
 * A version of 'strcmp' suitable for passing to 'qsort'.
 *************************************************************************/
static int string_compare
  (const void * elem1,
   const void * elem2)
{
  const char** string_ptr1 = (const char**)elem1;
  const char** string_ptr2 = (const char**)elem2;

  return(strcmp(*string_ptr1, *string_ptr2));
}

/*************************************************************************
 * Sort a list of strings in place.
 *************************************************************************/
void sort_string_list 
  (STRING_LIST_T* a_list)
{
  check_null_list(a_list);

  qsort((void *)(a_list->strings), a_list->num_strings, 
	sizeof(char*), string_compare);
}

/***************************************************************************
 * Given two sets, A and B, find 
 *  - A intersect B,
 *  - A minus B, and
 *  - B minus A.
 *
 * Allocates memory for the three new string lists, which must be
 * freed by the caller.
 *
 * If any of the return parameters is supplied as NULL, then that
 * parameter will not be filled in
 ***************************************************************************/
void overlap_string_lists
  (STRING_LIST_T*  list_a,
   STRING_LIST_T*  list_b,
   STRING_LIST_T**      intersection,
   STRING_LIST_T**      a_minus_b,
   STRING_LIST_T**      b_minus_a)
{
  int             i_all;         /* Index of current string. */
  char*          this_string;   /* The current string. */

  check_null_list(list_a);
  check_null_list(list_b);

  /* Allocate memory for the new list. */
  if (intersection != NULL) {
    *intersection = new_string_list();
  }
  if (a_minus_b != NULL) {
    *a_minus_b = new_string_list();
  }
  if (b_minus_a != NULL) {
    *b_minus_a = new_string_list();
  }

  /* Enumerate the first list. */
  for (i_all = 0; i_all < get_num_strings(list_a); i_all++) {
    this_string = get_nth_string(i_all, list_a);

    if (have_string(this_string, list_b)) {
      if (intersection != NULL) {
	add_string(this_string, *intersection);
      }
    } else {
      if (a_minus_b != NULL) {
	add_string(this_string, *a_minus_b);
      }
    }
  }

  /* Enumerate the second list. */
  for (i_all = 0; i_all < get_num_strings(list_b); i_all++) {
    this_string = get_nth_string(i_all, list_b);

    if (!have_string(this_string, list_a)) {
      if (b_minus_a != NULL) {
	add_string(this_string, *b_minus_a);
      }
    }
  }
}


/*************************************************************************
 * Combine all the strings in a given list into a single string
 * separated by a given separator.
 *
 * Memory allocated for return value must be freed by caller.
 *************************************************************************/
char* combine_string_list
  (STRING_LIST_T*  a_list,
    char*          separator)
{
  int    string_length;   /* Length of the combined string. */
  char* combined_string; /* String to be returned. */
  int    i_string;        /* Index of the current string. */

  check_null_list(a_list);

  /* Compute the length of the return string. */
  string_length = (get_num_strings(a_list) * 
		   (max_string_length(a_list) + strlen(separator))) + 2;

  /* Allocate memory for the string. */
  combined_string = (char*)mycalloc(string_length, sizeof(char));

  /* Accumulate all the strings, with spaces between. */
  for (i_string = 0; i_string < get_num_strings(a_list); i_string++) {
    strcat(combined_string, get_nth_string(i_string, a_list));
    strcat(combined_string, separator);
  }

  /* Remove the final space. */
  combined_string[strlen(combined_string)] = '\0';

  /* Return the string. */
  return(combined_string);
}


/*****************************************************************************
 * Read a list of strings from a given file. Paul fixed this to work with longer lines of text.
 *****************************************************************************/
STRING_LIST_T* read_string_list
  (FILE* infile)
{
  char           this_line[DEFAULT_LINE_LENGTH];
  char*          fgets_result;
  STRING_LIST_T* return_value;
  
  /* Allocate dynamic memory for a maximal list of names. */
  return_value = new_string_list();

  /* Read the first name. */
  fgets_result = fgets(this_line,  DEFAULT_LINE_LENGTH, infile);
  this_line[strlen(this_line)-1] = 0; // chop

  /* Make sure we got at least one name. */
  if (fgets_result == NULL) {
      die("Couldn't read a single name from the given file.");
  }

  while (fgets_result != NULL) {

     /* Store the name in the name list. */
    add_string(this_line, return_value);

     /* Read the next name. */
     fgets_result = fgets(this_line,  DEFAULT_LINE_LENGTH, infile);
     this_line[strlen(this_line)-1] = 0; // chop
  }

  /* Die if we didn't read anything. */
  if (get_num_strings(return_value) == 0) {
    die("Failed to read any names.");
  }

  /* Tell the user what's up. */
  if (verbosity > NORMAL_VERBOSE) {
    fprintf(stderr, "Read %d names.\n", get_num_strings(return_value));
  }
  
  return(return_value);
}

/*************************************************************************
 * Write out a list of strings.
 *************************************************************************/
void write_string_list
  (char*          separator, /* String to separate strings in list. */
   STRING_LIST_T*  a_list,    /* The list to be printed. */
   FILE *                outfile)
{
  int i_string;

  check_null_list(a_list);

  /* Don't do anything if we got no strings. */
  if (get_num_strings(a_list) == 0) {
      return;
  }

  for (i_string = 0; i_string < get_num_strings(a_list) - 1; i_string++) {
    fprintf(outfile, "%s%s", get_nth_string(i_string, a_list), separator);
  }
  fprintf(outfile, "%s\n", get_nth_string(i_string, a_list));
}


/*************************************************************************
 * Free dynamic memory used by a list of strings.
 *************************************************************************/
void free_string_list
  (STRING_LIST_T* a_list)
{
  int i_string;

  if (a_list == NULL) {
    return;
  }

  for (i_string = 0; i_string < a_list->max_strings; i_string++) {
    myfree(a_list->strings[i_string]);
  }
  myfree(a_list->strings);
  myfree(a_list);
}



/*************************************************************************
 * Right justify a string list by adding white space to the beginning
 * of short strings. We don't have to worry about making more space,
 * since the list building makes sure all strings are at least as long
 * as the longest string (Paul)
 *************************************************************************/
void right_justify_string_list
  (STRING_LIST_T* a_list)
{
  int numstrings, maxstring;
  int i,j;
  char* thisString;
  char* addedSpaces;

  int numSpacesToAdd;
  int lastaddedSpaces = 0;

  numstrings = get_num_strings(a_list);
  maxstring = max_string_length(a_list);
  thisString = (char*)mymalloc(maxstring *sizeof(char)+1);
  addedSpaces = (char*)mymalloc(maxstring*sizeof(char)+1);
  
  for (i=0; i<numstrings;i++) {
    thisString = get_nth_string(i, a_list);
    numSpacesToAdd = maxstring - strlen(thisString);
    if (numSpacesToAdd > 0) {
      if (numSpacesToAdd != lastaddedSpaces) {
	for(j=0; j<maxstring;j++) {
	  if (j<numSpacesToAdd) {
	    strcpy(addedSpaces+j, " ") ;
	  } else {
	    addedSpaces[j] = 0;
	  }
	}
      }
      thisString = strcat(addedSpaces, thisString);
      set_nth_string(thisString, i, a_list); /* makes a copy */
      lastaddedSpaces = numSpacesToAdd;
    }
  }
  free(addedSpaces);
  free(thisString);
}

/*
 * Local Variables:
 * mode: c
 * c-basic-offset: 2
 * End:
 */
