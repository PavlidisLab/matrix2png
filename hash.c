/*****************************************************************************
 * FILE: hash.c
 * AUTHOR: Paul Pavlidis
 * CREATE DATE: 3/2001
 * PROJECT: PLOTKIT
 *
 * DESCRIPTION: A quadratic probing hashtable. Keys are strings, but
 * contents can be anything. Largely based on code found in
 * M.A. Weiss's DS&AlgAnalysis, chapter 5. Keys are copied into the
 * hash table, but values are just pointers to the original data, so
 * it is up the user to maintain these.
 *****************************************************************************/
#include "string-list.h"
#include "hash.h"
#include "primes.h"
#include "utils.h"
#include <stdio.h>

/* create a filled table. The data can be any type; the keys are a
   string-list; the data are in an array. If the data is NULL: we
   assume that the indices of the stringlist items are what is needed,
   so the data is ints */
HASHTABLE_T* createHashTable(STRING_LIST_T* keys, 
			     void** data // data is a pointer to an array of data items we will insert in the table.
			     ) 
{
  HASHTABLE_T* return_val = NULL;
  int i;
  int num_keys;

  num_keys = get_num_strings(keys);
  if (num_keys == 0) {
    return(NULL);
  }
  return_val = (HASHTABLE_T*)inittable((int)DEFAULT_TABLE_SIZE);

  for (i = 0; i < num_keys; i++) {
    if (data == NULL) {
      insert(return_val, get_nth_string(i, keys), (void*)i);
    } else {
      DEBUG_CODE(1, fprintf(stderr, "Inserting %s\n", (char*)data[i]););
      insert(return_val, get_nth_string(i, keys), (void*)data[i]);
    }
  }
  return(return_val);
} /* createHashTable */



/* intialize a table. Keep the pointer to the allocationFunc - we will use this to make space later.  */
HASHTABLE_T* inittable(int size) {
  HASHTABLE_T* return_value;
  int i;
  return_value = (HASHTABLE_T*)mymalloc(sizeof(HASHTABLE_T));

  /* Note that if extra memory is needed for the hash table values,
     it's up to the user to allocate it. */
  return_value->table = (void**)mycalloc(size, sizeof(void*)); /* this does weiss' set to NULL */

  /* initialize the key list */
  return_value->keys = new_string_list();
  for (i=0; i<size; i++) {
    add_string("", return_value->keys);
  }
  return_value->usage = (BOOLEAN_T*)mycalloc(size,sizeof(BOOLEAN_T));
  return_value->table_size = size;
  return_value->num_items = 0;
  return(return_value);
}

/* return a pointer to the hash table. This should be done when
   allocating additional memory */
void** getTablePtr(HASHTABLE_T* hashtable) {
  return hashtable->table;
}


/*  return an array location in which to store a value. Basically  */
/*  copied from Weiss   */
int hash (char* key, int keylength, HASHTABLE_T* hashtable) { 
  int hashVal; 
  int i; 
  hashVal = 0; 
  for (i = 0; i < keylength; i++) { 
    hashVal = 37*hashVal + (int)key[i];  /*can I do that cast?   */
  }
  hashVal %= hashtable->table_size; 
  if (hashVal < 0) { 
    hashVal+= hashtable->table_size; 
  } 
  DEBUG_CODE(1, fprintf(stderr, "%s hashes to %d\n", key, hashVal););
  return hashVal; 
} /* hash */


/*  find where something lives */
int findPos(char* key, int keylength, HASHTABLE_T* hashtable) { 
  int collisionNum = 0;
  int current = hash(key, keylength, hashtable); 

  /* collision detection: when a collision occurs, check to see that
     the key for the location isn't the one we're using. If it is,
     then we've found it. */
  while (isActive(hashtable, current) && strcmp(key, get_nth_string(current, hashtable->keys)) ) {
    current+= 2*++collisionNum - 1;
    if (current >= hashtable->table_size) { 
      current -= hashtable->table_size;
    }
  }
  DEBUG_CODE(1,
	     //	     fprintf(stderr, "Findpos: %s -> %d\n", key, current);
	     );

  return current; 
} /* findpos */


/*    retrieve the a key's content */
void* find (HASHTABLE_T* hashtable, char* key) { 
  int current;
  void* returnval;
  current = findPos(key, strlen(key), hashtable); 
  returnval = isActive(hashtable, current) ? hashtable->table[current] : NULL;
  return returnval;
} /* find */


/*    retrieve the a key's content's index */
int findindex (HASHTABLE_T* hashtable, char* key) { 
  int current;
  int returnval;
  current = findPos(key, strlen(key), hashtable); 
  returnval = isActive(hashtable, current) ? current : -1;
  return returnval;
}


/* insert a value into the table.. */
BOOLEAN_T insert (HASHTABLE_T* hashtable, char* key, void *value) { 
  static int numStored = 0; 
  int current;
  
  // check if this key is already there.

  if (find(hashtable, key) == NULL) {
    //fprintf(stderr, "Warning: Duplicate key: A value for key \"%s\" is already in the table.\n", key);
    //return FALSE; /* ??? overwrite or not ??? */
    hashtable->num_items++;
  }

  /* find a place to put this */
  current = findPos(key, strlen(key), hashtable); 

  /* store: 1. the value 2. that the position is taken and 3. the
     key. */
  hashtable->table[current] = value;
  hashtable->usage[current] = TRUE;
  set_nth_string(key, current, hashtable->keys);

  DEBUG_CODE(1,
	     fprintf(stderr, "Insert: key %s\n", key);
	     );

  /* rehash if the table is more than half full */
  if (++numStored > hashtable->table_size / 2) { 
    rehash(hashtable); 
  }
  return TRUE; /* ??? overwrite or not ??? */
} /* insert */


/*   check if a position is being used  */
BOOLEAN_T isActive(HASHTABLE_T* hashtable, int position){
  return (BOOLEAN_T)(hashtable->table[position] && hashtable->usage[position]);
}


/* expand a crowded hashtable */
void rehash(HASHTABLE_T* hashtable) {
  HASHTABLE_T* newtable;
  int i;
  int newsize;
  char* key;
  void* value;

  DEBUG_CODE(1,
	     fprintf(stderr, "Rehashing\n");
	     );

  newsize = nextPrime(2*(hashtable->table_size));
  newtable = inittable(newsize);

  for (i = 0; i < hashtable->table_size; i++) {
    key = get_nth_string(i, hashtable->keys);
    if (strlen(key) > 0) {
      value = find(hashtable, key);
      if (hashtable->table[i] != NULL && hashtable->usage[i]) { /* this if clause isn't strictly necessary; the key presence tells us we need to insert. */
	insert(newtable, key, value);
      }
    }
  }
  
  /* is there a nicer way of doing the following? */
  free(hashtable->table);
  free(hashtable->usage);
  free_string_list(hashtable->keys);
  hashtable->keys = newtable->keys;
  hashtable->table = newtable->table;
  hashtable->usage = newtable->usage;
  hashtable->table_size = newtable->table_size;
  free(newtable);
} /* rehash */


/* Free the memory for a table, NOT including all the data it contains */
void freetable(HASHTABLE_T* hashtable) {
  //  int size;
  //  size = hashtable->table_size;
  free(hashtable->table);
  free(hashtable->usage);
  free_string_list(hashtable->keys);
  free(hashtable);
} /* freetable */

/* this is really for debugging - only will work for string as data types !*/
void printHash(HASHTABLE_T* hashtable) {
  int i;
  int j;
  char* k;
  int m;
  j = hashtable->table_size;
  for (i=0; i<j; i++) {
    if (hashtable->usage[i] == TRUE) {
      k = get_nth_string(i, hashtable->keys);
      DEBUG_CODE(1, fprintf(stderr, "Looking for %s\n", k););
      if (strlen(k) != 0) {
	m = findindex(hashtable, k);
	if (m < 0) {
	  die("Could not find value for %s\n", k);
	} else {
	  fprintf(stderr, "%s\t%s\n", k, (char*)hashtable->table[m]);
	  //	  fprintf(stderr, "%s\t%d\n", k, *(int*)hashtable->table[m]);
	}
      }
    }
  }
} /* printhash */

/*
 * hash.c
 */
