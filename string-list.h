/**************************************************************************
 * FILE: string-list.h
 * AUTHOR: William Grundy
 * CREATE DATE: 12-22-98
 * PROJECT: PHYLO
 * DESCRIPTION: Data structure for manipulating a list of strings. 
 **************************************************************************/
#ifndef STRING_LIST_H
#define STRING_LIST_H

#include "utils.h"

/*************************************************************************
 * Primary data structure.
 *************************************************************************/
typedef struct string_list_t STRING_LIST_T;

/*************************************************************************
 * Allocate dynamic memory for a string list.
 *************************************************************************/
STRING_LIST_T* new_string_list
  ();

/*************************************************************************
 * Get the length of the longest string in a list.
 *************************************************************************/
int max_string_length
  (STRING_LIST_T* a_list);

/*************************************************************************
 * Get the total number of strings in a list.
 *************************************************************************/
int get_num_strings
  (STRING_LIST_T* a_list);

/*************************************************************************
 * Get or set the nth string in a list.
 *************************************************************************/
char* get_nth_string
  (int            n,
   STRING_LIST_T* a_list);

void set_nth_string
  (char*          new_string,            
   int            n,
   STRING_LIST_T* a_list);

/*************************************************************************
 * Does the given string list have any space left in it?
 *************************************************************************/
BOOLEAN_T have_space
  (STRING_LIST_T* a_list);

/*************************************************************************
 * Add a string to the end of a given list.
 *************************************************************************/
void add_string
  (char*    a_string,
   STRING_LIST_T* a_list);

/*************************************************************************
 * Add a list of strings to the end of a given list.
 *************************************************************************/
void add_strings
  (STRING_LIST_T* source_list,
   STRING_LIST_T*       target_list);

/*************************************************************************
 * Remove a string from a given list.
 *
 * It is an error if the string does not appear in the list.
 *************************************************************************/
void remove_string
  (char*    a_string,
   STRING_LIST_T* a_list);

/*************************************************************************
 * Remove a set of strings from a given list.
 *
 * It is an error for the source list to contain any strings that
 * don't appear in the target list.
 *************************************************************************/
void remove_strings
  (STRING_LIST_T* source_list,
   STRING_LIST_T*       target_list);

/*************************************************************************
 * Does a given string appear in a given list?
 *************************************************************************/
BOOLEAN_T have_string
  (char*          a_string,
   STRING_LIST_T* a_list);

/*************************************************************************
 * Return a copy of the given list.
 *************************************************************************/
STRING_LIST_T* copy_string_list
  (STRING_LIST_T* string_list);

/*************************************************************************
 * Sort a list of strings in place.
 *************************************************************************/
void sort_string_list 
  (STRING_LIST_T* a_list);

/***************************************************************************
 * Given two sets, A and B, find 
 *  - A union B,
 *  - A minus B, and
 *  - B minus A.
 ***************************************************************************/
void overlap_string_lists
  (STRING_LIST_T* list_a,
   STRING_LIST_T* list_b,
   STRING_LIST_T**      intersection,
   STRING_LIST_T**      a_minus_b,
   STRING_LIST_T**      b_minus_a);

/*************************************************************************
 * Combine all the strings in a given list into a single string
 * separated by a given separator.
 *************************************************************************/
char* combine_string_list
  (STRING_LIST_T* a_list,
   char*          separator);

/*****************************************************************************
 * Read a list of strings from a given file.
 *****************************************************************************/
STRING_LIST_T* read_string_list
  (FILE* infile);

/*************************************************************************
 * Write out a list of strings.
 *************************************************************************/
void write_string_list
  (char*          separator, /* String to separate strings in list. */
   STRING_LIST_T* a_list,    /* The list to be printed. */
   FILE*          outfile);

/*************************************************************************
 * Free dynamic memory used by a list of strings.
 *************************************************************************/
void free_string_list
  (STRING_LIST_T* a_list);

#endif
