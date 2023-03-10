/*****************************************************************************
 * FILE: rdb-matrix.c
 * AUTHOR: William Grundy, Paul Pavlidis
 * CREATE DATE: 4/19/99
 * PROJECT: GENEX -> PLOTKIT, etc.
 * DESCRIPTION: A matrix data structure with labels on rows and columns.
 * Copyright (c) Columbia University
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
  return(this_char == COMMENT_CHAR);
}

/***********************************************************************
 * Read one row from a file, checking for long lines.
 ***********************************************************************/
#define MAX_ROW 500000
static void read_one_row
  (FILE* infile,
   int   length,
   char* one_row)
{
  //  char*     fgets_result;       /* Error indicator for 'fgets' function. */
  int len = 0;
  if (length > 0) { ;} // avoid compiler warning...
  BOOLEAN_T format_notified = FALSE; // debug code;

  while(len <= MAX_ROW - 2) {
    char next = fgetc(infile);
    if (next == '\n' || next == EOF) {
      one_row[len++] = '\n'; 
      one_row[len++] = '\0';  
      break;
    } else if (next == '\r') {
      next = fgetc(infile);
      one_row[len++] = '\n'; 
      one_row[len++] = '\0'; 
      if (next != '\n') { 
	/* oops. Read to the next DOS line */
	if (!format_notified) {
	  DEBUG_CODE(1, fprintf(stderr, "Looks like DOS format");)
	  format_notified = 1;
	}
	ungetc(next, infile);
      } else {
	if (!format_notified) {
	  DEBUG_CODE(1, fprintf(stderr, "Looks like Macintosh format");)
	  format_notified = 1;
	}
      }

      break;
      /* Mac format, the \r was a line end.*/
    }
    one_row[len++] = next;
   }

  //  fprintf(stdout, one_row);
  
  //  fgets_result = fgets(one_row, length, infile);

  //  if (fgets_result == NULL) 
  //    die("Error during reading from the file.");

  len = strlen(one_row);

  DEBUG_CODE(1, fprintf(stderr, "Length %d, ends with 0%o\n", len, one_row[len - 1]););
 
  if (one_row[len - 1] != '\n' && one_row[len - 1] != '\r') {
    if (len == MAX_ROW) {
      die("Matrix row length too long!");
    } else {
      die("Error reading from file, did not find EOL %s", one_row);
    }
  }
  
}

/***********************************************************************
 * Read an RDB file into a matrix.
 ***********************************************************************/

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
  DEBUG_CODE(1, fprintf(stderr, "Reading first line from file.\n"););
  read_one_row(infile, MAX_ROW, one_row);

  /* Keep reading till we get past the comments. */
  while (is_comment(one_row)) {
    /* fprintf(stderr, "Skipping: %s", one_row); */
    DEBUG_CODE(1, fprintf(stderr, "Skipping comments.\n"););
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

    /* clean up any stray dos linefeed */
    if (string_ptr[strlen(string_ptr) - 1] == '\r') {
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
  DEBUG_CODE(1, fprintf(stderr, "Reading data matrix.\n"););
  /* Read the matrix. */
  for (i_row = 0; ; i_row++) {

    /* Read the next line, stopping if it's empty. */
    read_one_row(infile, MAX_ROW, one_row);

    if (strlen(one_row) == 0) {
      break;
    }

    /* Read the row name and store it. */
    string_ptr = strtok(one_row, "\t");

    /* clean up any stray dos linefeeds */
    if (string_ptr[strlen(string_ptr) - 1] == '\r') {
      string_ptr[strlen(string_ptr) - 1] = '\0';
    }
    add_string(string_ptr, row_names);

    /* Read the row. */
    for (i_column = 0; i_column < num_cols+1; i_column++) {
      
      /* This is an important check: if the number of
	 headings is too few, we won't read in all of the data, which
	 is almost not what is wanted. So check to see that there is
	 no data there. Typically this is caused by a missing corner string. (PP)*/
      if (i_column == num_cols) {
	string_ptr = strtok(NULL, "\t");
	if (string_ptr != NULL) {
	  die("Data unexpectedly found past end of headings: (%d,%d). Check that all columns in the file have a heading, including the 'corner'.\n", 
	      i_row, i_column);
	} else {
	  // good, we're done
	  break;
	}
      }

      /* Read the first value. */
      string_ptr = strtok(NULL, "\t");

      /* Make sure we got a valid string. */
      if (string_ptr == NULL) {
	die("No entry found at (%d,%d).", i_row, i_column);
      } else {

      /* Paul found this is quite a bit faster than sscanf (but not as
         safe, and ignores MSCAN): */
#ifdef QUICKBUTCARELESS
	one_value = strtod(string_ptr, NULL); 
	num_scanned = 1; /* avoid compiler warning */
#else
	/* Bill's original code. sscanf is slow but gives a useful return value */
	num_scanned = sscanf(string_ptr, MSCAN, &one_value);
	if (num_scanned == EOF) {
	  die("Premature end-of-file at (%d,%d).", i_row, i_column);
	}
	
	if (num_scanned == 0) {
	  if (strcmp(ATYPENAME, "double") != 0) {
	    die("Missing values are only permitted in arrays of type double.\n");
	  }
	  one_value = NaN();
	}
#endif
      }
      /* Store the value. */
      set_array_item(i_column, one_value, this_row);
    }
    /* Add this row to the matrix. */
    grow_matrix(this_row, matrix);
  }
  num_rows = i_row - 1;

  DEBUG_CODE(1, fprintf(stderr, "Read from file: %d rows, %d cols\n", num_rows, num_cols););

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

/* Read from an rdb file, allowing missing values, and permitting data
   storage to start from various points in the file. startrow and
   startcol are indexes (min 0) of the row and column to start reading
   from. 
 */

#define BUFSIZE 1000
RDB_MATRIX_T* read_rdb_matrix_wmissing
  (BOOLEAN_T format_line,
   FILE* infile,
   int rowstoread, 
   int colstoread,
   int startrow,
   int startcol
   )
{
  MATRIX_T* matrix;             /* The matrix to be read. */
  char*     corner_string;      /* Upper left corner of matrix. */
  STRING_LIST_T* row_names;     /* Row names in the matrix. */
  STRING_LIST_T* col_names;     /* Column names in the matrix. */
  //  char      one_row[MAX_ROW];   /* One row of the matrix. */
  int       i_row;              /* Index of the current row. */
  int       i_column;           /* Index of the current column. */
  int       num_rows;           /* Total number of rows. */
  int       num_cols;           /* Total number of rows. */
  int       num_scanned;        /* Error indicator for fscanf function. */
  char      string[BUFSIZE];
  char*     string_ptr;
  MTYPE     one_value;          /* One value read from the file. */
  ARRAY_T*  this_row = NULL;    /* One row of the matrix. */
  RDB_MATRIX_T* return_value;   /* The RDB matrix being created. */
  int i_char;
  int this_char;
  int num_missing = 0;
  int length;
  int count = -1;
  int i_read = 0;
  char* one_row = (char*)malloc(sizeof(char)*MAX_ROW);
  int foo = rowstoread; // todo, this is to avoid compiler complaints. Get rid of rowstoread.
    foo*=2;

  if (infile == NULL) {
    die("Attempted to read matrix from null file.");
  }

  /* Create the row names and column names lists. */
  row_names = new_string_list();
  col_names = new_string_list();

  /* Read the first row. */
  DEBUG_CODE(1, fprintf(stderr, "Reading first line from file.\n"););
  read_one_row(infile, MAX_ROW, one_row);

  /* Keep reading til we get past the comments. */
  while (is_comment(one_row)) {
    DEBUG_CODE(1, fprintf(stderr, "Skipping comments.\n"););
    read_one_row(infile, MAX_ROW, one_row);
  }
    
  /* Store the name of the first column. */
  string_ptr = strtok(one_row, "\t");
  copy_string(&corner_string, string_ptr);

  /* Store the names of the columns. */
  for (string_ptr = strtok(NULL, "\t"); string_ptr != NULL;
       string_ptr = strtok(NULL, "\t")) {

    count++;
    if (startcol > 0 && count < startcol)
      continue;

    /* Remove EOL. */
    if (string_ptr[strlen(string_ptr) - 1] == '\n') {
      string_ptr[strlen(string_ptr) - 1] = '\0';
    }

    /* clean up any stray dos linefeed */
    if (string_ptr[strlen(string_ptr) - 1] == '\r') {
      string_ptr[strlen(string_ptr) - 1] = '\0';
    }

    /* Store string if it is non-empty. (fixme: what if it is empty?) */
    if (strcmp(string_ptr, "") != 0) {
      add_string(string_ptr, col_names);
    }

    if (colstoread > 0 && get_num_strings(col_names) >= colstoread)
      break;
  }

  num_cols = get_num_strings(col_names);
  DEBUG_CODE(1, fprintf(stderr, "There are %d columns of data\n", num_cols););

  /* Allocate the matrix. */
  matrix = allocate_matrix(0, num_cols);

  /* Allocate one row. */
  this_row = allocate_array(num_cols);

  /* Skip the format line, if necessary. */
  if (format_line) {
    read_one_row(infile, MAX_ROW, one_row);
  }

  /* Read the matrix. */
  DEBUG_CODE(1, fprintf(stderr, "Reading data matrix.\n"););
  for (i_row = 0;  ; i_row++) {

    /* Read the next line, stopping if it's empty. Or if we've read enough rows. */
    //    if (fgets(one_row, MAX_ROW, infile) == NULL || (rowstoread >= 0 && i_row >= rowstoread)) {
    read_one_row(infile, MAX_ROW, one_row);

    if (strlen(one_row) == 0 || (strlen(one_row) == 1 && one_row[0] == '\n') ) {
      DEBUG_CODE(1, fprintf(stderr, "Line is empty.\n"););
      break;
    }

    if (startrow > 0 && i_row < startrow) {
      continue;
    }

    // chomp.
    if(one_row[strlen(one_row) - 1] == '\n') {
      one_row[strlen(one_row) - 1] = '\0';
    }

    // trim stray dos linefeed
    if(one_row[strlen(one_row) - 1] == '\r') {
      one_row[strlen(one_row) - 1] = '\0';
    }

    // read the row name
    i_char = 0;
    while ( one_row[i_char] > 0 && one_row[i_char] != '\t' && i_char < BUFSIZE - 1) {
      string[i_char] = one_row[i_char];
      i_char++;
    }
    if(i_char >= BUFSIZE - 1) {
      die("Row name too long (maximum is %d)\n", (int)BUFSIZE);
    }
    string[i_char] = '\0';
    i_char++; // go past the tab we just encountered.

    /* clean up any stray dos linefeed in the row name,  and add it to the list.*/
    if (string[strlen(string) - 1] == '\r') {
      string[strlen(string) - 1] = '\0';
    }
    add_string(string, row_names);

    this_char = 0;
    i_column = 0;
    length = strlen(one_row);
    i_read = 0;

    /*
      Step through the line one character at a time. When we find a
      field delimiter, we store the last set of tokens. Missing values
      are a special case. Line ends are treated specially to avoid
      fussing over trailing tabs.
     */
    while (i_char <= length) { // note we purposely read just past the end.
      if (one_row[i_char] == '\t' || i_char == length) { // check for missing value.
	if ((one_row[i_char] == '\t' && 
	    ((one_row[i_char-1] == '\t') || // tab tab
	    (one_row[i_char-1] == '-' && one_row[i_char-2] == '\t') || // tab - tab // means we went through a cell and didn't get a value.
	    (one_row[i_char-1] == ' ' && one_row[i_char-2] == '\t'))) // tab spc tab // means we went through a cell and didn't get a value.	    
	    || (i_char == length && (one_row[i_char-1] == '\t' || one_row[i_char-1] == ' ' || one_row[i_char-1] == '-'))	     // we're at the end but didn't read any numerical characters.
	    )
	  {
	    if (!(startcol >= 0 && i_column < startcol)) { // only if we've reached the required column selected by the user, if any.
	      if(i_read >= num_cols) { /* Don't do anything. Probably
					  a trailing tab character -
					  ignore it. This relaxes the
					  requirements for the end of
					  a line. If the entire column
					  is blank, then we needn't be
					  concerned. If there is any
					  data in this extrac column
					  it is caught by the
					  following case. */

		//		die("More data than column headings: Check data file format for correct header including 'corner string'.\nExpected %d columns, found at least %d", num_cols, i_read);
	      } else {
		set_array_item(i_read, NaN(), this_row);
		num_missing++;
		i_read++;
	      }
	    }
	  }
	else // not a missing dataum, store the value.
	  {
	    if (startcol < 0 || i_column >= startcol) { // only if we've reached the required column.
	      string[this_char] = '\0';
	      num_scanned = sscanf(string, MSCAN, &one_value);
	      if(i_read == num_cols && colstoread < 0) {
		die("Problem reading in row %d: Possible illegal character? Make sure the file is ASCII", i_row + 1);
	      } else {
		set_array_item(i_read, one_value, this_row);
		i_read++;
	      }
	    } else {
	      DEBUG_CODE(1, fprintf(stderr, "skipping column %d\n", i_column););
	    }
	  }
	this_char = 0;

	if (i_read >= num_cols) {
	  /* We've read enough. Stop. Note that if row has extra
	     fields, we silently ignore them. This didn't used to be
	     the behavior but the philosophy is "do something
	     reasonable". This is definitely the right thing to do if
	     num_cols is set. */
	  break;
	}

	i_column++;
      } else { // keep reading.
	string[this_char] = one_row[i_char];
	this_char++;
      }
      i_char++; // advance in string.
    }

    /* Make sure we got enough values (counting missing values */
    if (!(colstoread > 0 && i_read < colstoread) && i_read < get_num_strings(col_names)) {
      die("Line %d didn't have enough fields in it. Expected %d based on header, found only %d. If missing values were intended, it must be properly indicated (see documentation).",
	  i_row,
	  colstoread > 0 ? colstoread : get_num_strings(col_names), 
	  i_read );
    }

    /* Add this row to the matrix. */
    grow_matrix(this_row, matrix);
  }
  if (verbosity > NORMAL_VERBOSE) {
    fprintf(stderr, "%d missing values\n", num_missing);
  }

  num_rows = get_num_strings(row_names);

  DEBUG_CODE(1, fprintf(stderr, "Read from file: %d rows, %d cols\n", num_rows, num_cols););

  if (num_rows == 0) {
    die("No data rows were read from the file");
  }

  if (num_cols == 0) {
    die("No data columns were read from the file");
  }

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
  free(one_row);
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
