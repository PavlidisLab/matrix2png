/*****************************************************************************
 * FILE: hash.h
 * AUTHOR: Paul Pavlidis
 * CREATE DATE: 3/2001
 * PROJECT: PLOTKIT
 * DESCRIPTION: A quadratic probing hashtable. Keys are strings, 
 * but contents can be anything (define HTYPE)
 *****************************************************************************/

#ifndef HASH_D
#define HASH_D

#include "utils.h"

#ifdef STRINGHASH
#define HTYPE char*
#else
#define HTYPE double
#endif

#ifndef KEYLENGTH
#define KEYLENGTH 25 /* Max length of a key. */
#endif

#define DEFAULT_TABLE_SIZE 17389

//typedef struct hashtable_t HASHTABLE_T;

/* define the structure for the hash table */
typedef struct hashtable_t HASHTABLE_T;

struct hashtable_t {
  int table_size; /* the size of the table */
  char** keys; /* keys which have been used. Needed for rehashing and also iterating over the table perl-style */
  HTYPE* table; /* the actual table (array) of values to be stored. */
  BOOLEAN_T* usage; /* which positions are used. increases storage but so what. (do we absolutely need this? could use the keys themselves) */
  void(*insertFunc)(HASHTABLE_T*, int, HTYPE); /* callback for value memory allocation */
  int num_items;
};

HASHTABLE_T* createHashTable(STRING_LIST_T* keys, 
			     void* data, 
			     void(*al)(HASHTABLE_T* hashtable, int index, HTYPE value));


/* intialize a table */
HASHTABLE_T* inittable(int size, void(*al)(HASHTABLE_T* hashtable, int index, HTYPE value) );

/* return an array location in which to store a value */
int hash (char* key, int keylength, HASHTABLE_T* hashtable);

/* retrieve the contents */
HTYPE find (HASHTABLE_T* hashtable, char* key);

/*    retrieve the a key's content's index */
int findindex (HASHTABLE_T* hashtable, char* key);

/* insert a value into the table */
void insert (HASHTABLE_T* hashtable, char* key, HTYPE value);

/* check if a position is being used */
BOOLEAN_T isActive(HASHTABLE_T* hashtable, int position);

/* find a new position for an item (for placement*/
int findPos(char* key, int keylength, HASHTABLE_T* hashtable);

/* expand when the table gets too full */
void rehash(HASHTABLE_T * table);

/* free memory for a table */
void freetable(HASHTABLE_T * table);

/* get pointer to the table itself. Should be used during memory
   allocation */
HTYPE* getTablePtr(HASHTABLE_T* hashtable);

void printHash(HASHTABLE_T* hashtable);

#endif
