/*****************************************************************************
 * FILE: hash.h
 * AUTHOR: Paul Pavlidis
 * CREATE DATE: 3/2001
 * PROJECT: PLOTKIT
 * DESCRIPTION: A quadratic probing hashtable. Keys are strings, 
 * but contents can be anything.
 *****************************************************************************/

#ifndef HASH_D
#define HASH_D

#include "utils.h"
#include "string-list.h"

#ifndef KEYLENGTH
#define KEYLENGTH 25 /* Max length of a key. */
#endif

#ifndef DEBUG
#define DEFAULT_TABLE_SIZE 17389
#else /* make it very small so we get a lot of collisions and rehashing */
#define DEFAULT_TABLE_SIZE 7
#endif

/* define the structure for the hash table */
typedef struct hashtable_t HASHTABLE_T;

struct hashtable_t {
  int table_size; /* the size of the table */
  void** table; /* the actual table (array) of values to be stored. */
  STRING_LIST_T* keys; /* keys which have been used. Needed for rehashing and also iterating over the table perl-style */
  BOOLEAN_T* usage; /* which positions are used. increases storage but so what. (do we absolutely need this? could use the keys themselves) */
  int num_items; /* how many items are actually stored. */
};

/* build table given data */
HASHTABLE_T* createHashTable(STRING_LIST_T* keys, 
			     void** data
			     );

HASHTABLE_T* inittable(int size);

/* return an array location in which to store a value */
int hash (char* key, int keylength, HASHTABLE_T* hashtable);

/* retrieve the contents */
void* find (HASHTABLE_T* hashtable, char* key);

/*    retrieve the a key's content's index */
int findindex (HASHTABLE_T* hashtable, char* key);

/* insert a value into the table */
BOOLEAN_T insert (HASHTABLE_T* hashtable, char* key, void* value);

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
void** getTablePtr(HASHTABLE_T* hashtable);

/* debugging tool at the moment: data must be char* */
void printHash(HASHTABLE_T* hashtable);

#endif
