/********************************************************************
 * FILE: array.c
 * AUTHOR: William Noble Grundy, Paul Pavlidis
 * PROJECT: MHMM, prob-exp, matrix2png....etc.
 * COPYRIGHT: 1997-1999, Regents of the University of California

 * DESCRIPTION: Some simple array-handling routines. This version
 * (modified by Paul) handles missing values. In some cases it relies
 * on the IEEE standard specification which states that all operations
 * involving NaN result in NaN; in other cases, this result must be
 * avoided to prevent entire arrays being assigned NaN. This extra
 * checking will make this code somewhat slower than the
 * non-missingvalue-checking version.
 ********************************************************************/
#include "array.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


/***********************************************************************
 * Allocate one array.
 ***********************************************************************/
ARRAY_T* allocate_array
  (const int num_items)
{
  ARRAY_T* new_array = (ARRAY_T*)mymalloc(sizeof(ARRAY_T));
  new_array->items = (ATYPE*)mycalloc(num_items, sizeof(ATYPE));
  new_array->num_items = num_items;
  return(new_array);
}



/**************************************************************************
 * Error checking routine called by all access functions to avoid core
 * dump when attempting to access a null pointer.
 **************************************************************************/
static void check_null_array
  (ARRAY_T* array)
{
#ifdef BOUNDS_CHECK
  if (array == NULL) {
    die("Attempted to access a null array.\n");
  }
#else
  /* Avoid compiler warning. */
  array = NULL;
#endif
}

/**************************************************************************
 * Implement bounds checking.
 **************************************************************************/
static void array_bounds_check
  (int      index,
   ARRAY_T* array)
{

#ifdef BOUNDS_CHECK
  if (index < 0) {
    die("Invalid array index (%d).\n", index);
  } else if (index >= get_array_length(array)) {
    die("Array index out of bounds (%d >= %d).\n", 
	index, get_array_length(array));
  }
#else
  /* Avoid compiler warning. */
  array += index;
#endif
}


/***********************************************************************
 * Check to be sure that two arrays have the same length.
 ***********************************************************************/
static BOOLEAN_T check_array_dimensions
  (BOOLEAN_T die_on_mismatch,
   ARRAY_T*  array1,
   ARRAY_T*  array2)
{
  /* Check to see that the two arrays are of the same length. */
  if (get_array_length(array1) != get_array_length(array2)) {
    if (die_on_mismatch) {
      die("Arrays have differing lengths (%d != %d).\n", 
	  get_array_length(array1), get_array_length(array2));
    }
    return(FALSE);
  }
  return(TRUE);
}


/***********************************************************************
 * Basic access routines.
 ***********************************************************************/
int get_array_length
  (ARRAY_T* array)
{
  check_null_array(array);
  return(array->num_items);
}

/* get array length, NOT counting missing values */
int get_array_length_m 
(ARRAY_T* array)
{
  int i;
  int return_value = 0;
  check_null_array(array);
  for (i=0; i<array->num_items; i++) {
    if(!isnan(get_array_item(i, array)))
      return_value++;
  }
  return(return_value);
}


/* The following two functions are only used when array bounds
   checking is turned on. Otherwise, they are replaced by macros in
   array.h. */
ATYPE get_array_item_defcheck
  (int      index,
   ARRAY_T* array)
{
  check_null_array(array);
  array_bounds_check(index, array);
  return(array->items[index]);
}

void set_array_item_defcheck
  (int      index,
   ATYPE    value,
   ARRAY_T* array)
{
  check_null_array(array);
  array_bounds_check(index, array);
  array->items[index] = value;
}

void incr_array_item
  (int      index,
   ATYPE    value,
   ARRAY_T* array)
{
  check_null_array(array);
  array_bounds_check(index, array);
  array->items[index] += value;
}

/***********************************************************************
 * Give out the innards of the array.  This function is included only
 * to allow optimizations.  It should be used sparingly.
 ***********************************************************************/
ATYPE* raw_array
  (ARRAY_T* array)
{
  return(array->items);
}

/***********************************************************************
 * Initialize a given array with a given value.
 ***********************************************************************/
void init_array
  (ATYPE    value,
   ARRAY_T* array)
{
  int i_item;
  int num_items;

  num_items = get_array_length(array);
  for (i_item = 0; i_item < num_items; i_item++) {
    set_array_item(i_item, value, array);
  }
}

/***********************************************************************
 * Fill an array with a given raw array of values.
 ***********************************************************************/
void fill_array
  (ATYPE*   raw_array,
   ARRAY_T* array)
{
  int i_item;
  int num_items;

  num_items = get_array_length(array);
  for (i_item = 0; i_item < num_items; i_item++) {
    set_array_item(i_item, raw_array[i_item], array);
  }
}


/***********************************************************************
 * Copy an array into another array of the same length.
 ***********************************************************************/
void copy_array
  (ARRAY_T* array1,
   ARRAY_T* array2)
{
  int num_items;

  check_null_array(array1);
  check_null_array(array2);
  check_array_dimensions(TRUE, array1, array2);

  num_items = get_array_length(array1);
  if (num_items != 0) {
    memcpy(array2->items, array1->items, sizeof(ATYPE) * num_items);
  }
}

/***********************************************************************
 * Determine whether two arrays are equal, within a given bound.
 ***********************************************************************/
BOOLEAN_T equal_arrays
  (ATYPE    close_enough,
   ARRAY_T* array1,
   ARRAY_T* array2)
{
  int i_item;
  int num_items;

  check_null_array(array1);
  check_null_array(array2);

  /* Verify that the arrays are of the same length. */
  if (!check_array_dimensions(FALSE, array1, array2)) {
    return(FALSE);
  }

  /* Check to be sure that each value is the same. */
  num_items = get_array_length(array1);
  for (i_item = 0; i_item < num_items; i_item++) {
    if (!almost_equal(get_array_item(i_item, array1)
		     - get_array_item(i_item, array2), 0.0, close_enough)) {
      return(FALSE);
    }
  }

  return(TRUE);
}

/***********************************************************************
 * Add two arrays, total in array2. If either array is missing a
 * value, the sum is nan.
 ***********************************************************************/
void sum_array
  (ARRAY_T* array1,
   ARRAY_T* array2)
{
  int i_item;
  int num_items;
  
  check_null_array(array1);
  check_null_array(array2);
  check_array_dimensions(TRUE, array1, array2);

  num_items = get_array_length(array1);
  for (i_item = 0; i_item < num_items; i_item++) {
    incr_array_item(i_item, get_array_item(i_item, array1), array2);
  }
}

/***********************************************************************
 * Compute the element-by-element product of two arrays. If either
 * value is missing, the product is nan.
 ***********************************************************************/
void element_product
  (ARRAY_T* array1,
   ARRAY_T* array2)
{
  int i_item;
  int num_items;
  
  check_null_array(array1);
  check_null_array(array2);
  check_array_dimensions(TRUE, array1, array2);

  num_items = get_array_length(array1);
  for (i_item = 0; i_item < num_items; i_item++) {
    set_array_item(i_item, get_array_item(i_item, array1) *
		   get_array_item(i_item, array2), array2);
  }
}

/***********************************************************************
 * Compute the dot product of two arrays. If either corresponding
 * value is nan, the values are ignored.
 ***********************************************************************/
double dot_product
  (ARRAY_T* array1,
   ARRAY_T* array2)
{
  int i_item;
  int num_items;
  double return_value;
  
  check_array_dimensions(TRUE, array1, array2);

  num_items = get_array_length(array1);
  return_value = 0.0;
  for (i_item = 0; i_item < num_items; i_item++) {
    if(isnan(get_array_item(i_item, array1)) || isnan(get_array_item(i_item, array2))) {
      continue; 
    } else {
      return_value += get_array_item(i_item, array1) *
	get_array_item(i_item, array2);
    }
  }
  return(return_value);
}

/***********************************************************************
 * Add a scalar value to each element of an array, unless it is
 * missing.
 ***********************************************************************/
void scalar_add
  (ATYPE    value,
   ARRAY_T* array)
{
  int i_item;
  int num_items;
  
  num_items = get_array_length(array);
  for (i_item = 0; i_item < num_items; i_item++) {
    if(isnan(get_array_item(i_item, array)))
      continue;
    incr_array_item(i_item, value, array);
  }
}

/***********************************************************************
 * Multiply each element of an array by a scalar value,unless the
 * element is missing in which case it remains nan.
 ***********************************************************************/
void scalar_mult
  (ATYPE    value,
   ARRAY_T* array)
{
  int i_item;
  int num_items;
  
  check_null_array(array);
  num_items = get_array_length(array);

  for (i_item = 0; i_item < num_items; i_item++) {
    if(isnan(get_array_item(i_item, array)))
      continue;
    set_array_item(i_item, get_array_item(i_item, array) * value, array);
  }
}

/***********************************************************************
 * Compute the sum of the elements in an array, not including missing
 * values.
 ***********************************************************************/
ATYPE array_total
  (ARRAY_T* array)
{
  int i_item;
  int num_items;
  ATYPE total = 0;
  
  check_null_array(array);
  num_items = get_array_length(array);

  for (i_item = 0; i_item < num_items; i_item++) {
    if (isnan(get_array_item(i_item, array)))
	continue;
    total += get_array_item(i_item, array);
  }
  return(total);
}

/***********************************************************************
 * Sort the elements in an array. 
 ***********************************************************************/
static int sort_compare
  (const void* elem1,
   const void* elem2)
{
  ATYPE num1 = *((ATYPE*)elem1);
  ATYPE num2 = *((ATYPE*)elem2);

  if (num1 < num2) {
    return(-1);
  } else if (num1 > num2) {
    return(1);
  }
  return(0);
}

static int reverse_sort_compare
  (const void* elem1,
   const void* elem2)
{
  ATYPE num1 = *((ATYPE*)elem1);
  ATYPE num2 = *((ATYPE*)elem2);

  if (num1 > num2) {
    return(-1);
  } else if (num1 < num2) {
    return(1);
  }
  return(0);
}

void sort_array
  (BOOLEAN_T reverse_sort,
   ARRAY_T* array)
{
  if (reverse_sort) {
    qsort(array->items, array->num_items, sizeof(ATYPE), reverse_sort_compare);
  } else {
    qsort(array->items, array->num_items, sizeof(ATYPE), sort_compare);
  }
}

/***********************************************************************
 * Set and get the key used in sorting multiple arrays.
 ***********************************************************************/
void set_array_key
  (ATYPE key,
   ARRAY_T* array)
{
  check_null_array(array);
  array->key = key;
}
ATYPE get_array_key
  (ARRAY_T* array)
{
  check_null_array(array);
  return(array->key);
}

/***********************************************************************
 * Compute the sum of the squares of the elements in an array, not including missing values.
 ***********************************************************************/
ATYPE sum_of_squares
  (ARRAY_T* array)
{
  int i_item;
  int num_items;
  ATYPE value;     /* One value from the array. */
  ATYPE total = 0; /* The sum of the squares. */
  
  check_null_array(array);
  num_items = get_array_length(array);

  for (i_item = 0; i_item < num_items; i_item++) {
    if(isnan(get_array_item(i_item,array)))
      continue;
    value = get_array_item(i_item, array);
    total += value * value;
  }
  return(total);
}

/***********************************************************************
 * Compute the sum of the squares of the differences between two
 * arrays, exluding elements which are missing in either array.
 ***********************************************************************/
ATYPE sum_of_square_diffs
  (ARRAY_T* array1,
   ARRAY_T* array2) 
{
  int i_item;
  int num_items;
  ATYPE diff;      /* The difference between two corresponding array values. */
  ATYPE total = 0; /* The sum of the squares. */
  
  check_null_array(array1);
  check_null_array(array2);
  check_array_dimensions(TRUE, array1, array2);

  num_items = get_array_length(array1);
  for (i_item = 0; i_item < num_items; i_item++) {
    if (isnan(get_array_item(i_item, array1)) || isnan(get_array_item(i_item, array2)))
      continue;
    diff = get_array_item(i_item, array1) - get_array_item(i_item, array2);
    total += diff * diff;
  }
  return(total);
}

/***********************************************************************
 * Compute the Euclidean distance between two arrays.
 ***********************************************************************/
double euclidean_distance
  (ARRAY_T* array1,
   ARRAY_T* array2)
{
  return(sqrt(sum_of_square_diffs(array1, array2)));
}


/***********************************************************************
 * Compute the correlation r between two arrays. (Paul) If a value is
 * missing in either array, that index is ignored.
 ***********************************************************************/
double correlation_coeff 
  (ARRAY_T* array1,
   ARRAY_T* array2)
{
  int i;
  int size = 0;
  ATYPE meanarray1;
  ATYPE meanarray2;
  ATYPE stdevarray1;
  ATYPE stdevarray2;
  ATYPE sum = 0;

  check_null_array(array1);
  check_null_array(array2);
  check_array_dimensions(TRUE, array1, array2);
  //  size = array1->num_items;

  meanarray1 = ave_array(array1);
  meanarray2 = ave_array(array2);
  stdevarray1 = sqrt(array_variance(array1));
  stdevarray2 = sqrt(array_variance(array2));

  for (i=0; i < size; i++) {
    if (isnan(get_array_item(i, array1)) || isnan(get_array_item(i, array2))) // only if both aren't missing.
      continue;
    sum += ((get_array_item(i, array1) - meanarray1)  *  (get_array_item(i, array2) - meanarray2))/(stdevarray1 * stdevarray2);
    size++;
  }
  return (double)(sum/(size - 1));
}

/***********************************************************************
 * Compute the median value in an array.
 *
 * Sorts the array as a side effect. TODO: make this behave correctly
 * with missing values.
 ***********************************************************************/
ATYPE compute_median
  (ARRAY_T* array)
{
  int   num_items;
  ATYPE return_value;

  /* Sort the array. */
  sort_array(FALSE, array);

  /* If there are an odd number of elements, return the middle one. */
  num_items = get_array_length(array);
  if (num_items % 2 == 1) {
    return_value = get_array_item(num_items / 2, array);
  }

  /* Otherwise, return the average of the two middle ones. */
  else {
    return_value = get_array_item((num_items / 2) - 1, array);
    return_value += get_array_item(num_items / 2, array);
    return_value /= 2.0;
  }

  return(return_value);
}

/***********************************************************************
 * Read an array of known length from a file.
 ***********************************************************************/
void read_array
  (FILE *   infile,
   ARRAY_T* array)
{
  int   i_item;
  int   num_items;
  ATYPE value;
  
  check_null_array(array);

  num_items = get_array_length(array);
  for (i_item = 0; i_item < num_items; i_item++) {
    if (fscanf((FILE *)infile, ASCAN, &value) != 1) {
      die("Error reading array at position %d.\n", i_item);
    }
    set_array_item(i_item, value, array);
  }
}

/***********************************************************************
 * Write an array to a file.
 ***********************************************************************/
void print_array
  (ARRAY_T*  array,         /* The array to be printed. */
   int       width,         /* Width of each cell. */
   int       precision,     /* Precision of each cell. */
   BOOLEAN_T eol,           /* Include an EOL char at the end? */
   FILE*     outfile)       /* File to which to write. */
{
  int   i_item;
  int   num_items;
  ATYPE item;
  
  check_null_array(array);

  num_items = get_array_length(array);
  for (i_item = 0; i_item < num_items; i_item++) {
    item = get_array_item(i_item, array);
    fprintf(outfile, APRINT, width, precision, item);
  }
  if (eol) {
    fprintf(outfile, "\n");
  }
}

/***********************************************************************
 * Free memory used by an array.
 ***********************************************************************/
void free_array
  (ARRAY_T* array)
{
  if (array == NULL) {
    return;
  }

  myfree(array->items);
  myfree(array);
}

/***********************************************************************
 * Swap two arrays (added by paul)
 ***********************************************************************/
void swap_arrays (ARRAY_T* array1, ARRAY_T* array2)
{
  ARRAY_T* temp = NULL;
  ATYPE* temparray = NULL;
  int tempnum_items;
  ATYPE tempkey;

  temparray = array1->items;
  tempnum_items = array1->num_items;
  tempkey = array1->key;
  temp = array1;

  array1->items = array2->items;
  array1->num_items = array2->num_items;
  array1->key = array2->key;
  array1 = array2;

  array2->key = tempkey;
  array2->num_items = tempnum_items;
  array2->items = temparray;
  array2 = temp;
}


/* The following functions require non-integral array values. */
#ifdef NOT_INT

/***********************************************************************
 * Divide corresponding elements in two arrays
 ***********************************************************************/
void dot_divide
  (ARRAY_T* array1,
   ARRAY_T* array2)
{
  int i_item;
  int num_items;
  
  check_null_array(array1);
  check_null_array(array2);
  check_array_dimensions(TRUE, array1, array2);

  num_items = get_array_length(array1);
  for (i_item = 0; i_item < num_items; i_item++) {
    set_array_item(i_item, get_array_item(i_item, array1) /
		   get_array_item(i_item, array2), array2);
  }
}


/***********************************************************************
 * Compute the average of an array (non-missing values only)
 ***********************************************************************/
ATYPE ave_array
  (ARRAY_T* array)
{
  int num_items;

  check_null_array(array);  

  /* Check for zero length. */
  num_items = get_array_length_m(array);
  if (num_items == 0) {
    die("Attempting to average the elements of an empty array.\n");
  }

  /* Compute the average. */
  return(array_total(array) / (ATYPE)num_items);
}

/***********************************************************************
 * Compute the variance of the elements in an array.
 ***********************************************************************/
ATYPE array_variance
  (ARRAY_T* array)
{
  int num_items, num_nonmissing;
  int i_item;
  ATYPE average;
  ATYPE error;
  ATYPE total_error;

  /* Compute the average. */
  average = ave_array(array);

  total_error = 0.0;
  num_items = get_array_length(array);
  num_nonmissing = get_array_length_m(array);

  if (num_nonmissing == 0)    die("Attempting to get the variance of an empty array");

  for (i_item = 0; i_item < num_items; i_item++) {
    if(isnan(get_array_item(i_item, array)))
      continue;
    error = get_array_item(i_item, array) - average;
    total_error += error * error;
  }
  return(total_error / (ATYPE)(num_nonmissing - 1));
}
  

/***********************************************************************
 * Make an array sum to zero by subtracting the mean from each element.
 ***********************************************************************/
void sum_to_zero
  (ARRAY_T* array)
{
  int num_items;
  int i_item;
  ATYPE ave;

  ave = ave_array(array);

  num_items = get_array_length(array);
  for (i_item = 0; i_item < num_items; i_item++) {
    if(isnan(get_array_item(i_item, array)))
      continue;
    incr_array_item(i_item, -ave, array);
  }
}

/***********************************************************************
 * Make an array have variance one by dividing by the standard
 * deviation.
 ***********************************************************************/
void variance_one_array
  (ARRAY_T* array)
{
  ATYPE variance;
  variance = array_variance(array);
  
  /* Avoid divide by zero. */
  if (variance == 0.0) {
    fprintf(stderr, "Warning: variance of zero.\n");
  } else {
    scalar_mult(1.0 / sqrt(array_variance(array)), array);
  }
}


/***********************************************************************
 * Mean center an array
 ***********************************************************************/
void mean_center_array
  (ARRAY_T* array)
{
  ATYPE mean;
  mean =  - ave_array(array);
  scalar_add(mean, array);
}


/***********************************************************************
 * Normalize the elements of an array to sum to 1.0.
 ***********************************************************************/
void normalize
  (ATYPE    close_enough, /* If the total is close to 1.0, don't bother. */
   ARRAY_T* array)
{
  ATYPE total;

  /* Compute the sum of the elements in the array. */
  total = array_total(array);

  /* Only normalize if we're relatively far from 1.0. */
  if (almost_equal(total, 1.0, close_enough)) {
    return;
  }

  /* If there's nothing in the array, then return a uniform distribution. */
  if (total == 0.0) {
    init_array(1.0 / (ATYPE)get_array_length(array), array);
    return;
  }

  /* Divide by the total. */
  scalar_mult(1.0 / total, array);
}

/***********************************************************************
 * Convert an array to and from logs (base 2).
 ***********************************************************************/
void log_array
  (ARRAY_T* array)
{
  int i_item;
  int num_items;

  check_null_array(array);

  num_items = get_array_length(array);
  for (i_item = 0; i_item < num_items; i_item++) {
    set_array_item(i_item, my_log2(get_array_item(i_item, array)), array);
  }
}

void unlog_array
  (ARRAY_T* array)
{
  int i_item;
  int num_items;

  check_null_array(array);

  num_items = get_array_length(array);
  for (i_item = 0; i_item < num_items; i_item++) {
    set_array_item(i_item, EXP2(get_array_item(i_item, array)), array);
  }
}

/***********************************************************************
 * Compute the sum of an array in log space.
 ***********************************************************************/
ATYPE log_array_total
  (ARRAY_T* array)
{
  int   i_item;
  int   num_items;
  ATYPE total = LOG_ZERO;

  check_null_array(array);

  num_items = get_array_length(array);
  for (i_item = 0; i_item < num_items; i_item++) {
    total = LOG_SUM(total, get_array_item(i_item, array));
  }

  return(total);
}

/***********************************************************************
 * Normalize an array in log space.
 ***********************************************************************/
void log_normalize
  (ATYPE    close_enough,
   ARRAY_T* array)
{
  int i_item;
  int num_items;
  ATYPE total;
  ATYPE this_value;

  /* Get the sum of the elements. */
  total = log_array_total(array);

  /* If the array already sums to zero, don't bother. */
  if (almost_equal(total, 0.0, close_enough)) {
    return;
  }

  /* If there's nothing in the array, then return all zeroes. */
  if (total < LOG_SMALL) {
    init_array(LOG_ZERO, array);
    return;
  }

  num_items = get_array_length(array);
  for (i_item = 0; i_item < num_items; i_item++) {
    this_value = get_array_item(i_item, array) - total;

    /* If this value is small enough, just make it zero. */
    if (this_value < LOG_SMALL) {
      set_array_item(i_item, LOG_ZERO, array);
    } else {
      set_array_item(i_item, this_value, array);
    }
  }
}

/***********************************************************************
 * Mix two arrays in log space.
 ***********************************************************************/
void mix_log_arrays
  (float    mixing, /* Percent of array2 that will be retained. */
   ARRAY_T* array1,
   ARRAY_T* array2)
{
  int   i_item;
  int   num_items;
  ATYPE mixed_value;

  check_null_array(array1);
  check_null_array(array2);

  /* Verify that the arrays are of the same length. */
  check_array_dimensions(TRUE, array1, array2);

  /* Verify that we've got a reasonable mixing parameter. */
  if ((mixing > 1.0) || (mixing < 0.0)) {
    die("Invalid mixing parameter (%g).\n", mixing);
  }

  num_items = get_array_length(array1);
  for (i_item = 0; i_item < num_items; i_item++) {
    mixed_value
      = LOG_SUM(my_log2(1.0 - mixing) + get_array_item(i_item, array1),
		my_log2(mixing) + get_array_item(i_item, array2));
    set_array_item(i_item, mixed_value, array2);
  }
}
  
/***********************************************************************
 * Fill an array with random values between 0 and a given maximum.
 *
 * Assumes that the random number generator is initialized. 
 ***********************************************************************/
void randomize_array
  (ATYPE    magnitude,
   ARRAY_T* array)
{
  int num_items;
  int i_item;

  check_null_array(array);

  num_items = get_array_length(array);
  for (i_item = 0; i_item < num_items; i_item++) {
    set_array_item(i_item, my_drand() * magnitude, array);
  }
}


/***********************************************************************
 * scramble the elements of an array in place (Paul)
 ***********************************************************************/
void scramble (ARRAY_T* array) {
  int array_size = get_array_length(array);
  int rand1;
  int rand2;
  ATYPE temp;
  int SCRAMBLERUNS = 7; /* could probably be fewer than 7, but 1 is not enough! */
  int i;
  for (i = 0; i < array_size * SCRAMBLERUNS ; i++) {
    rand1 = rand() % array_size;
    rand2 = rand() % array_size;
    temp = get_array_item(rand1, array);
    set_array_item(rand1, get_array_item(rand2, array), array);
    set_array_item(rand2, temp, array);
  }
}




/***********************************************************************
 * Add random noise to an array.
 ***********************************************************************/
void add_noise
  (ATYPE    magnitude,  /* Magnitude of the noise. */
   ARRAY_T* array)
{
  int   i_item;
  int   num_items;
  ATYPE noise;

  check_null_array(array);  

  num_items = get_array_length(array);
  for (i_item = 0; i_item < num_items; i_item++) {
    noise = magnitude * (2 * my_drand() - 1);
    incr_array_item(i_item, noise, array);
  }
}

/***********************************************************************
 * Make all the elements of an array positive by adding a constant to
 * each.
 ***********************************************************************/
void all_positive
  (ARRAY_T* array)
{
  int   i_item;
  int   num_items;
  ATYPE min;

  check_null_array(array);  

  /* Find the minimum value. */
  min = get_array_item(0, array);
  num_items = get_array_length(array);
  for (i_item = 0; i_item < num_items; i_item++) {
    if (get_array_item(i_item, array) < min) {
      min = get_array_item(i_item, array);
    }
  }

  /* If there are negative elements ... */
  if (min < 0.0) {
    /* ... then subtract the minimum from all elements. */
    for (i_item = 0; i_item < num_items; i_item++) {
      incr_array_item(i_item, -min, array);
    }
  }
}
#endif /* NOT_INT */

#ifdef MAIN

VERBOSE_T verbosity = INVALID_VERBOSE;

int main()
{
  ARRAY_T* array1;
  ARRAY_T* array2;

  /* Set up two arrays containing 1,2,3,4,5. */
  array1 = allocate_array(5);
  set_array_item(0, 1, array1);
  set_array_item(1, 2, array1);
  set_array_item(2, 3, array1);
  set_array_item(3, 4, array1);
  set_array_item(4, 5, array1);

  array2 = allocate_array(5);
  set_array_item(0, 1, array2);
  set_array_item(1, 2, array2);
  set_array_item(2, 3, array2);
  set_array_item(3, 4, array2);
  set_array_item(4, 5, array2);


  printf("Before: ");
  print_array(array1, 3, 3, TRUE, stdout);

  printf("Initialize to 1: ");
  init_array(1, array1);
  print_array(array1, 3, 3, TRUE, stdout);

  printf("Multiply by 2: ");
  scalar_mult(2, array1);
  print_array(array1, 3, 3, TRUE, stdout);

  printf("Array2: ");
  print_array(array2, 3, 3, TRUE, stdout);
  printf("Element product: ");
  element_product(array1, array2);
  print_array(array2, 3, 3, TRUE, stdout);

#ifdef NOT_INT
  printf("Normalize: ");
  normalize(0.0, array2);
  print_array(array2, 3, 3, TRUE, stdout);

  printf("Sum to zero: ");
  sum_to_zero(array2);
  print_array(array2, 3, 3, TRUE, stdout);
#endif

  printf("Copy: ");
  copy_array(array2, array1);
  print_array(array1, 3, 3, TRUE, stdout);

  free_array(array1);
  free_array(array2);
  return(0);
}
#endif /* MAIN */

/*
 * Local Variables:
 * mode: c
 * c-basic-offset: 2
 * End:
 */
