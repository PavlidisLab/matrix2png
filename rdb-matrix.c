/*****************************************************************************
 * FILE: rdb-matrix.c
 * AUTHOR: William Grundy, Paul Pavlidis
 * CREATE DATE: 4/19/99, 2/2001
 * PROJECT: GENEX -> PLOTKIT
 * DESCRIPTION: A matrix data structure with labels on rows and columns.
 *****************************************************************************/
#include "rdb-matrix.h"
#include "string-list.h"
#include "matrix.h"
#include "array.h"
#include "utils.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>

/***************************************************************************
 * Define the RDB matrix type.
 ***************************************************************************/
struct rdb_matrix_t {
  char*          corner_string; /* Upper left corner. */
  STRING_LIST_T* row_names;
  STRING_LIST_T* col_names;
  MATRIX_T*      matrix;
};

/***********************************************************************
 * Allocate memory for an RDB matrix.
 *
 * Optional third argument is a raw matrix to be stored in the new RDB
 * matrix.
 ***********************************************************************/
RDB_MATRIX_T* allocate_rdb_matrix
  (int num_rows,
   int num_cols,
   MATRIX_T* matrix)
{
  RDB_MATRIX_T* return_value;

  /* Allocate the new RDB matrix. */
  return_value = (RDB_MATRIX_T*)mymalloc(sizeof(RDB_MATRIX_T));

  return_value->row_names = new_string_list();
  return_value->col_names = new_string_list();
  if (matrix == NULL) {
    return_value->matrix = allocate_matrix(num_rows, num_cols);
  } else {
    return_value->matrix = matrix;
  }

  return(return_value);
}


/***********************************************************************
 * Get or set the various pieces of the RDB matrix.
 ***********************************************************************/
STRING_LIST_T* get_row_names
  (RDB_MATRIX_T* rdb_matrix)
{
  if (rdb_matrix == NULL) {
    die("Attempted to access null matrix.");
  }
  return(rdb_matrix->row_names);
}

STRING_LIST_T* get_col_names
  (RDB_MATRIX_T* rdb_matrix)
{
  if (rdb_matrix == NULL) {
    die("Attempted to access null matrix.");
  }
  return(rdb_matrix->col_names);
}

void set_row_names
  (STRING_LIST_T* row_names,
   RDB_MATRIX_T* rdb_matrix)
{
  int num_rows;
  int i_row;

  if (rdb_matrix == NULL) {
    die("Attempted to access null matrix.");
  }

  num_rows = get_num_rows(get_raw_matrix(rdb_matrix));
  if (get_num_strings(row_names) != num_rows) {
    die("Adding %d row names to a matrix with %d rows.",
	get_num_strings(row_names), num_rows);
  }

  /* Add the strings. */
  for (i_row = 0; i_row < num_rows; i_row++) {
    add_string(get_nth_string(i_row, row_names), rdb_matrix->row_names);
  }
}

void set_col_names
  (STRING_LIST_T* col_names,
   RDB_MATRIX_T*  rdb_matrix)
{
  int i_col;
  int num_cols;

  if (rdb_matrix == NULL) {
    die("Attempted to access null matrix.");
  }

  num_cols = get_num_cols(get_raw_matrix(rdb_matrix));
  if (get_num_strings(col_names) != num_cols) {
    die("Adding %d column names to a matrix with %d columns.",
	get_num_strings(col_names), get_num_cols(get_raw_matrix(rdb_matrix)));
  }

  /* Add the strings. */
  for (i_col = 0; i_col < num_cols; i_col++) {
    add_string(get_nth_string(i_col, col_names), rdb_matrix->col_names);
  }
}

char* get_corner_string
  (RDB_MATRIX_T* rdb_matrix)

{
  if (rdb_matrix == NULL) {
    die("Attempted to access null matrix.");
  }
  return(rdb_matrix->corner_string);
}

void set_corner_string
  (char* corner_string,
   RDB_MATRIX_T* rdb_matrix)
{
  if (rdb_matrix == NULL) {
    die("Attempted to access null matrix.");
  }
  copy_string(&(rdb_matrix->corner_string), corner_string);
}


MATRIX_T* get_raw_matrix
  (RDB_MATRIX_T* rdb_matrix)
{
  if (rdb_matrix == NULL) {
    die("Attempted to access null matrix.");
  }
  return(rdb_matrix->matrix);
}

void set_raw_matrix
  (MATRIX_T*     matrix,
   RDB_MATRIX_T* rdb_matrix)
{
  if (rdb_matrix == NULL) {
    die("Attempted to access null matrix.");
  }
  free_matrix(rdb_matrix->matrix);
  rdb_matrix->matrix = matrix;
}

/***********************************************************************
 * Add a row or column name to a matrix.
 ***********************************************************************/
void add_row_name
  (char*         row_name,
   RDB_MATRIX_T* rdb_matrix)
{
  if (rdb_matrix == NULL) {
    die("Attempted to access null matrix.");
  }
  add_string(row_name, rdb_matrix->row_names);
}

void add_col_name
  (char*         col_name,
   RDB_MATRIX_T* rdb_matrix)
{
  if (rdb_matrix == NULL) {
    die("Attempted to access null matrix.");
  }
  add_string(col_name, rdb_matrix->col_names);
}

/***********************************************************************
 * Is the given line a comment?
 ***********************************************************************/
static BOOLEAN_T is_comment
  (char* one_line)
{
  int  i_char;
  char this_char;

  /* Find the first non-space character. */
  i_char = 0;
  this_char = one_line[i_char];
  while (this_char == ' ') {
    this_char = one_line[i_char++];
  }

  /* Is it a hash mark? */
  return(this_char == '#');
}

/***********************************************************************
 * Read one row from a file, checking for long lines.
 ***********************************************************************/
static void read_one_row
  (FILE* infile,
   int   length,
   char* one_row)
{
  char*     fgets_result;       /* Error indicator for 'fgets' function. */

  fgets_result = fgets(one_row, length, infile);

  if (one_row[strlen(one_row) - 1] != '\n') {
    die("Matrix lines too long.  Increase length.");
  }
}

/***********************************************************************
 * Read an RDB file into a matrix.
 ***********************************************************************/
#define MAX_ROW 100000
RDB_MATRIX_T* read_rdb_matrix
  (BOOLEAN_T format_line,
   FILE* infile)
{
  MATRIX_T* matrix;             /* The matrix to be read. */
  char*     corner_string;      /* Upper left corner of matrix. */
  STRING_LIST_T* row_names;     /* Row names in the matrix. */
  STRING_LIST_T* col_names;     /* Column names in the matrix. */
  char      one_row[MAX_ROW];   /* One row of the matrix. */
  int       i_row;              /* Index of the current row. */
  int       i_column;           /* Index of the current column. */
  int       num_rows;           /* Total number of rows. */
  int       num_cols;           /* Total number of rows. */
  int       num_scanned;        /* Error indicator for fscanf function. */
  char*     string_ptr = NULL;  /* Storage for strtok function. */
  MTYPE     one_value;          /* One value read from the file. */
  ARRAY_T*  this_row = NULL;    /* One row of the matrix. */
  RDB_MATRIX_T* return_value;   /* The RDB matrix being created. */

  /* Make sure the file is valid. */
  if (infile == NULL) {
    die("Attempted to read matrix from null file.");
  }

  /* Create the row names and column names lists. */
  row_names = new_string_list();
  col_names = new_string_list();

  /* Read the first row. */
  read_one_row(infile, MAX_ROW, one_row);

  /* Keep reading till we get past the comments. */
  while (is_comment(one_row)) {
    /* fprintf(stderr, "Skipping: %s", one_row); */
    read_one_row(infile, MAX_ROW, one_row);
  }
    
  /* Store the name of the first column. */
  string_ptr = strtok(one_row, "\t");

  copy_string(&corner_string, string_ptr);

  /* Store the names of the columns. */
  for (string_ptr = strtok(NULL, "\t"); string_ptr != NULL;
       string_ptr = strtok(NULL, "\t")) {

    /* Remove EOL. */
    if (string_ptr[strlen(string_ptr) - 1] == '\n') {
      string_ptr[strlen(string_ptr) - 1] = '\0';
    }

    /* Store string if it is non-empty. */
    if (strcmp(string_ptr, "") != 0) {
      add_string(string_ptr, col_names);
    }
  }

  num_cols = get_num_strings(col_names);

  /* Allocate the matrix. */
  matrix = allocate_matrix(0, num_cols);

  /* Allocate one row. */
  this_row = allocate_array(num_cols);

  /* Skip the format line, if necessary. */
  if (format_line) {
    read_one_row(infile, MAX_ROW, one_row);
  }

  /* Read the matrix. */
  for (i_row = 0; ; i_row++) {

    /* Read the next line, stopping if it's empty. */
    if (fgets(one_row, MAX_ROW, infile) == NULL) {
      break;
    }

    /* Read the row name and store it. */
    string_ptr = strtok(one_row, "\t");
    add_string(string_ptr, row_names);

    /* Read the row. */
    for (i_column = 0; i_column < num_cols; i_column++) {

      /* Read the first value. */
      string_ptr = strtok(NULL, "\t");

      /* Make sure we read the number properly. */
      if (string_ptr == NULL) {
	die("Error reading rdb matrix at position (%d,%d). ", i_row, i_column);
      }

      /* Paul found this is quite a bit faster than sscanf (but not as
         safe, and ignores MSCAN): */
#ifdef QUICKBUTCARELESS
      one_value = strtod(string_ptr, NULL); 
      num_scanned = 1; /* avoid compiler warning */
#else
      /* Bill's original code. sscanf is slow but gives a useful return value */
      num_scanned = sscanf(string_ptr, MSCAN, &one_value);
      if ((num_scanned == 0) || (num_scanned == EOF)) {
	die("Error reading rdb matrix at position (%d,%d).", i_row, i_column);
      }
#endif

      /* Store the value. */
      set_array_item(i_column, one_value, this_row);
    }

    /* Add this row to the matrix. */
    grow_matrix(this_row, matrix);
  }
  num_rows = i_row - 1;

  /* Assemble it all into an RDB matrix. */
  return_value = allocate_rdb_matrix(num_rows, num_cols, matrix);
  set_corner_string(corner_string, return_value);
  set_row_names(row_names, return_value);
  set_col_names(col_names, return_value);

  /* Free local dynamic memory. */
  myfree(corner_string);
  free_array(this_row);
  free_string_list(row_names);
  free_string_list(col_names);

  return(return_value);
}

/***********************************************************************
 * Write a named array to a file.
 ***********************************************************************/
void print_rdb_array
  (STRING_LIST_T* names,         /* Structure storing names */ 
   char*          label1,        /* Label for column 1. */
   char*          label2,        /* Label for column 2. */
   ARRAY_T*       array,         /* The array to be printed. */
   int            width,         /* Width of each cell. */
   int            precision,     /* Precision of each cell. */
   FILE*          outfile)       /* File to which to write. */
{
  
  int   i_item;
  int   num_items;

  fprintf(outfile, "%s\t%s\n", label1, label2);
  fprintf(outfile, "5S\t5N\n");

  num_items = get_array_length(array);
  for (i_item = 0; i_item < num_items; i_item++) {
    fprintf(outfile, "%s\t%*.*g\n", get_nth_string(i_item, names), width, 
	    precision, get_array_item(i_item, array));
  }
}

/***********************************************************************
 * Write a labeled matrix in RDB format.
 ***********************************************************************/
void print_rdb_matrix
  (RDB_MATRIX_T*  rdb_matrix,    /* Matrix to be printed. */
   BOOLEAN_T      format_line,   /* Print format line? */
   int            width,         /* Width of each cell. */
   int            precision,     /* Precision of each cell. */
   FILE*          outfile)       /* File to which to write. */
{
  int num_cols;
  int num_rows;
  int i_col;
  int i_row;

  /* Print the column names. */
  fprintf(outfile, "%s", rdb_matrix->corner_string);
  num_cols = get_num_cols(rdb_matrix->matrix);
  for (i_col = 0; i_col < num_cols; i_col++) {
    fprintf(outfile, "\t%s", get_nth_string(i_col, rdb_matrix->col_names));
  }
  fprintf(outfile, "\n");

  /* If requested, print the column widths. */
  if (format_line) {
    fprintf(outfile, "10S");
    for (i_col = 0; i_col < num_cols; i_col++) {
      fprintf(outfile, "\t%dN", width);
    }
    fprintf(outfile, "\n");
  }

  /* Print the matrix. */
  num_rows = get_num_rows(rdb_matrix->matrix);
  for (i_row = 0; i_row < num_rows; i_row++) {
    fprintf(outfile, "%s", get_nth_string(i_row, rdb_matrix->row_names));
    for (i_col = 0; i_col < num_cols; i_col++) {
      fprintf(outfile, "\t%*.*g", width, precision, 
	      get_matrix_cell(i_row, i_col, rdb_matrix->matrix));
    }
    fprintf(outfile, "\n");    
  }
}


/***********************************************************************
 * Free an RDB matrix.
 ***********************************************************************/
void free_rdb_matrix
  (RDB_MATRIX_T* rdb_matrix)
{
  if (rdb_matrix != NULL) {
    myfree(rdb_matrix->corner_string);
    free_string_list(rdb_matrix->row_names);
    free_string_list(rdb_matrix->col_names);
    free_matrix(rdb_matrix->matrix);

    myfree(rdb_matrix);
  }
}

/*
 * Local Variables:
 * mode: c
 * c-basic-offset: 2
 * End:
 */
