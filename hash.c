/*****************************************************************************
 * FILE: hash.c
 * AUTHOR: Paul Pavlidis
 * CREATE DATE: 3/2001
 * PROJECT: PLOTKIT
 * DESCRIPTION: A quadratic probing hashtable. Keys are strings, 
 * but contents can be anything (define HTYPE in hash.h)
 * Ported from Java code found in M.A. Weiss's DS&AlgAnalysis in Java, chapter 5.
 *****************************************************************************/

#include "string-list.h"
#include "hash.h"
#include "primes.h"
#include "utils.h"
#include <stdio.h>

/* create a filled table. The data can be any type; the keys are a
   string-list; the data are in an array. The caller provides the
   insert callback, and assures us that the data and the keys have the
   same number of items.  An exception is if the data is NULL: we
   assume that the indices of the stringlist items are what is needed,
   so the data is ints */
HASHTABLE_T* createHashTable(STRING_LIST_T* keys, 
			     void* data, 
			     void(*al)(HASHTABLE_T* hashtable, int index, HTYPE value)) 
{
  HASHTABLE_T* return_val = NULL;
  int i;
  int num_keys;

  num_keys = get_num_strings(keys);
  if (num_keys == 0) {
    return(NULL);
  }
  return_val = inittable(DEFAULT_TABLE_SIZE, al);

  for (i = 0; i < num_keys; i++) {
    if (data == NULL) {
      insert(return_val, get_nth_string(i, keys), (HTYPE)i);
    } else {
      insert(return_val, get_nth_string(i, keys), (HTYPE)data[i]);
    }
  }
  return(return_val);
} /* createHashTable */



/* intialize a table. Keep the pointer to the allocationFunc - we will use this to make space later.  */
HASHTABLE_T* buildtable(int size, void(*al)(HASHTABLE_T* hashtable, int index, HTYPE value)) {
  HASHTABLE_T* return_value;
  int i;
  return_value = (HASHTABLE_T*)mymalloc(sizeof(HASHTABLE_T));

  /* Note that if extra memory is needed for the hash table values,
     it's up to the user to allocate it. */
  return_value->table = (HTYPE*)mycalloc(size, sizeof(HTYPE)); /* this does weiss' set to NULL */
  return_value->keys = (char**)mycalloc(size, sizeof(char*));
  for (i=0; i<size;i++){
    return_value->keys[i] = (char*)mymalloc(KEYLENGTH*sizeof(char));
  }
  return_value->usage = (BOOLEAN_T*)mycalloc(size,sizeof(BOOLEAN_T));
  return_value->table_size = size;
  return_value->insertFunc = al;
  return_value->num_items = 0;
  return(return_value);
}

/* return a pointer to the hash table. This should be done when
   allocating additional memory */
HTYPE* getTablePtr(HASHTABLE_T* hashtable) {
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
  return hashVal; 
} 


/*  find where something lives */
int findPos(char* key, int keylength, HASHTABLE_T* hashtable) { 
  int collisionNum = 0;
  int current = hash(key, keylength, hashtable); 

  /* collision detection. If the position isn't empty, and isn't
     associated with the key we are looking for */
  while (isActive(hashtable, current) && strcmp(hashtable->keys[current], key)) {
    current+= 2*++collisionNum - 1;
    if (current >= hashtable->table_size) { 
      current -= hashtable->table_size;
    } 
  }
  DEBUG_CODE(1,
	     //	     fprintf(stderr, "%s -> %d\n", key, current);
	     );

  return current; 
}


/*    retrieve the a key's content */
HTYPE find (HASHTABLE_T* hashtable, char* key) { 
  int current;
  HTYPE returnval;
  current = findPos(key, strlen(key), hashtable); 
  returnval = isActive(hashtable, current) ? hashtable->table[current] : NULL;
  return returnval;
}


/*    retrieve the a key's content's index */
int findindex (HASHTABLE_T* hashtable, char* key) { 
  int current;
  int returnval;
  current = findPos(key, strlen(key), hashtable); 
  returnval = isActive(hashtable, current) ? current : -1;
  return returnval;
}


/* insert a value into the table. Give the size of the value if we
   need to allocate memory for it.  */
void insert (HASHTABLE_T* hashtable, char* key, HTYPE value) { 
  static int numStored = 0; 
  int current;
  current = findPos(key, strlen(key), hashtable); 

  if (isActive(hashtable, current)) {
   return;
  }

  /* store: 1. the value 2. that the position is taken and 3. the
     key. The key could substitute for 2. First make space for the
     value if need be.*/
  if (hashtable->insertFunc != NULL) {
    hashtable->insertFunc(hashtable, current, value);
  } else {
    hashtable->table[current] = value;
  }
  hashtable->usage[current] = TRUE;
  hashtable->num_items++;
  strcpy(hashtable->keys[current], key);

  DEBUG_CODE(1,
	     //	     fprintf(stderr, "Insert: key %s value %s ---> readkey %s readval %s\n", key, value, hashtable->keys[current], find(hashtable, key));
	     );

  /* rehash if the table is more than half full */
  if (++numStored > hashtable->table_size / 2) { 
    rehash(hashtable); 
  }
} 


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
  HTYPE value;

  DEBUG_CODE(1,
	     fprintf(stderr, "Rehashing\n");
	     );

  newsize = nextPrime(2*(hashtable->table_size));
  newtable = inittable(newsize, hashtable->insertFunc);

  for (i = 0; i < hashtable->table_size; i++) {
    key = hashtable->keys[i];
    if (key != NULL) {
      value = find(hashtable, key);
      if (hashtable->table[i] != NULL && hashtable->usage[i]) { /* this if clause isn't strictly necessary; the key presence tells us we need to insert. */
	insert(newtable, key, value);
      }
    }
  }
  
  /* is there a nicer way of doing the following? */
  free(hashtable->table);
  free(hashtable->usage);
  free(hashtable->keys);
  
  hashtable->table = newtable->table;
  hashtable->usage = newtable->usage;
  hashtable->keys = newtable->keys;
  hashtable->table_size = newtable->table_size;
  free(newtable);
}


/* Free the memory for a table, including all the data it contains, if
   allocation was done by the table */
void freetable(HASHTABLE_T* hashtable) {
  int i;
  int size;
  size = hashtable->table_size;
  if (hashtable->insertFunc != NULL) {
    int m;
    for (i = 0; i < size; i++) { /* this seems quite inefficient */
      if (strlen(hashtable->keys[i]) != NULL) {
	/* find where in the table key[i] is */
	m = findindex(hashtable, hashtable->keys[i]);
	if (m >= 0) {
	  free(hashtable->table[m]);
	}
      }
    }
  }
  free(hashtable->table);
  free(hashtable->usage);
  for (i = 0; i< size; i++) {
    free(hashtable->keys[i]);
  }
  free(hashtable->keys);
  free(hashtable);
}

/* this is really for debuggin */
void printHash(HASHTABLE_T* hashtable) {
  int i;
  int j;
  char* k;
  int m;
  j = hashtable->table_size;
  for (i=0; i<j; i++) {
    k = hashtable->keys[i];
    if (strlen(k) != 0) {
      m = findindex(hashtable, k);
      if (m < 0) {
      	die("Could not find value for %s\n", k);
      } else {
	fprintf(stderr, "%s\t%s\n", k, hashtable->table[m]);
      }
    }
  }
}

/*
 * hash.c
 */
