/*****************************************************************************
 * FILE: rdb-matrix.h
 * AUTHOR: William Grundy, Paul Pavlidis
 * CREATE DATE: 4/19/99, 2/2001
 * PROJECT: GENEX -> PLOTKIT
 * DESCRIPTION: A matrix data structure with labels on rows and columns.
 * Copyright (c) Columbia University
 *****************************************************************************/
#ifndef RDB_MATRIX_H
#define RDB_MATRIX_H

#include "string-list.h"
#include "matrix.h"
#include "array.h"
#include "utils.h"

/* lines beginning with this are taken to be comments */
#define COMMENT_CHAR '#'

/***************************************************************************
 * Define the RDB matrix type.
 ***************************************************************************/
typedef struct rdb_matrix_t RDB_MATRIX_T;

/***********************************************************************
 * Allocate memory for an RDB matrix.
 ***********************************************************************/
RDB_MATRIX_T* allocate_rdb_matrix
  (int num_rows,
   int num_cols,
   MATRIX_T* matrix);

/***********************************************************************
 * Get or set the various pieces of the RDB matrix.
 ***********************************************************************/
STRING_LIST_T* get_row_names
  (RDB_MATRIX_T* rdb_matrix);

STRING_LIST_T* get_col_names
  (RDB_MATRIX_T* rdb_matrix);

void set_row_names
  (STRING_LIST_T* row_names,
   RDB_MATRIX_T* rdb_matrix);

void set_col_names
  (STRING_LIST_T* col_names,
   RDB_MATRIX_T*  rdb_matrix);

char* get_corner_string
  (RDB_MATRIX_T* rdb_matrix);

void set_corner_string
  (char* corner_string,
   RDB_MATRIX_T* rdb_matrix);

MATRIX_T* get_raw_matrix
  (RDB_MATRIX_T* rdb_matrix);

void set_raw_matrix
  (MATRIX_T*     matrix,
   RDB_MATRIX_T* rdb_matrix);

/***********************************************************************
 * Add a column name to a matrix.
 ***********************************************************************/
void add_col_name
  (char*         col_name,
   RDB_MATRIX_T* rdb_matrix);

/***********************************************************************
 * Read an RDB file into a matrix.
 ***********************************************************************/
RDB_MATRIX_T* read_rdb_matrix
  (BOOLEAN_T format_line,
   FILE* infile);


/***********************************************************************
 * Read an RDB file into a matrix, allowing missing values.
 ***********************************************************************/
RDB_MATRIX_T* read_rdb_matrix_wmissing
  (BOOLEAN_T format_line,
   FILE* infile,
   int rowstoread, 
   int colstoread,
   int startrow,
   int startcol);


/***********************************************************************
 * Write a labeled matrix in RDB format.
 ***********************************************************************/
void print_rdb_matrix
  (RDB_MATRIX_T*  rdb_matrix,    /* Matrix to be printed. */
   BOOLEAN_T      format_line,   /* Print format line? */
   int            width,         /* Width of each cell. */
   int            precision,     /* Precision of each cell. */
   FILE*          outfile);      /* File to which to write. */

/***********************************************************************
 * Free an RDB matrix.
 ***********************************************************************/
void free_rdb_matrix
  (RDB_MATRIX_T* rdb_matrix);

#endif

/*
 * Local Variables:
 * mode: c
 * c-basic-offset: 2
 * End:
 */
