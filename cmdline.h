/********************************************************************
 * FILE: cmdline.h
 * AUTHOR: Timothy Bailey; Modified by Paul Pavlidis, Columbia University
 * PROJECT: Originally for MHMM (MEME)
 * COPYRIGHT: 1997, Regents of the University of California
 * DESCRIPTION: Generic command-line parsing macros.
 ********************************************************************/
#ifndef CMDLINE_H
#define CMDLINE_H

#include <stdio.h>
#include <string.h> /* Needed for strncmp. */

/* 
	The macro DO_STANDARD_COMMAND_LINE(n, stmts) processes argv[1..argc-1]
	according to the (semicolon-separated) statements in stmts.
	In the following, if X=0, the statement is ignored.
	The possible statements are as follows.
	  SIMPLE_FLAG_OPTN(X,s,var), which sets var to TRUE on
	    encountering the flag -s (s is an unquoted string of
	    letters and digits).
	  FLAG_OPTN(X,s,stmt), which executes stmt on encountering -s.
	  SIMPLE_CFLAG_OPTN(X,s,var) and CFLAG_OPTN(X,s,stmt), which are like
	    SIMPLE_FLAG_OPTN or FLAG_OPTN but require s to be a single
	    character and allow multiple flags to be concatenated (as in
	    ls -AF).
	  DATA_OPTN(X,s,desc,stmt), which assumes a switch of the form
	    -sdata or -s data, and executes stmt with _OPTION_ set to 
	    the start of the data string.  Desc is used as descriptive
	    text in a "Usage" message.
	  NON_SWITCH(X,desc,stmt), which executes stmt for any argument
	    that does not begin with "-".  Here, desc is used as descriptive 
	    text in a "Usage" message.  _OPTION_ is set to the
	    argument.
	  ANY_OPTION(X,desc,stmt), which executes stmt for any argument.
	    Desc is as for NON_SWITCH.  _OPTION_ is set to the argument.
	If there are fewer than <n> arguments on the command line, an
	error occurs.
	In case an error is detected, an error message is constructed from
	the flag names and descriptive text, and the program exits
	abnormally after printing the message.  Within one of the statements,
	the programmer can cause such an error message and termination with
	the macro COMMAND_LINE_ERROR.
*/

#define SIMPLE_FLAG_OPTN(X,string, desc, var) \
  FLAG_OPTN(X, string, desc, (var) = TRUE)



#define FLAG_OPTN(X,string,desc,stmt) \
  if (X) { \
    if (__ACTION__ == 0) { \
      if (strcmp(_OPTION_, "-" #string) == 0) { \
	{stmt;} \
	continue; \
      } \
    } \
    else if (__ACTION__ == 1 || __ACTION__ == 4) { \
      (void) fputs(" -" #string "" #desc "\n", stderr); \
    } \
  }


/* Paul changed this so you can have a description for cflags*/
#define CFLAG_OPTN(X,char, desc, stmt) \
  if (X) { \
    if (__ACTION__ == 0 && _OPTION_[1] == (#char)[0]) { \
      __ACTION__ = 2; \
      _OPTION_ += 1; \
    }  \
    if (__ACTION__ == 2 && _OPTION_[0] == (#char)[0]) { \
      {stmt;} \
      continue; \
    } else if (__ACTION__ == 1 || __ACTION__ == 3) { \
      (void) fputs(" -" #char " : " #desc "\n", stderr); \
      __ACTION__ = 3; \
    } else if (__ACTION__ == 5) { \
      __ACTION__ = 4; \
    } \
  }
  
#define SIMPLE_CFLAG_OPTN(X,char,var) \
  CFLAG_OPTN(X, char, (var) = 1)

#define DATA_OPTN(X,string,desc,stmt) \
  if (X) { \
    if (__ACTION__ == 0 \
	&& strncmp(_OPTION_, "-" #string, sizeof("-" #string) - 1) == 0) { \
      if (_OPTION_[sizeof("-" #string)-1] == '\0') { \
	__i__ += 1; \
	if (__i__ >= argc) __ACTION__ = 1; \
	_OPTION_ = argv[__i__]; \
      } \
      else _OPTION_ += sizeof("-" #string); \
      if (__ACTION__ != 1) {stmt;} \
      continue; \
    } else if (__ACTION__ == 1 || __ACTION__ == 4) { \
      (void) fputs(" -" #string " " #desc "\n", stderr); \
    } \
  }

#define NON_SWITCH(X,desc,stmt) \
  if (X) { \
    if (__ACTION__ == 0 && _OPTION_[0] != '-') { \
      stmt; \
      continue; \
    } \
    else if (__ACTION__ == 1 || __ACTION__ == 4) \
      (void) fputs(" " #desc, stderr); \
  }

#define ANY_OPTION(X,desc,stmt) \
  if (X) { \
    if (__ACTION__ == 0) { \
      stmt; \
      continue; \
    } \
    else if (__ACTION__ == 1 || __ACTION__ == 4) \
      (void) fputs(" " #desc, stderr); \
  }

#define COMMAND_LINE_ERROR { __ACTION__ = 1; continue; }

#define DO_STANDARD_COMMAND_LINE(n, stmts) \
  { \
    int __i__; \
    int __ACTION__ = 0; \
    char *_OPTION_ = NULL; \
    __i__ = 0; \
    while (1) { \
      if (__ACTION__ == 0 && argc < n+1) COMMAND_LINE_ERROR; \
      if (__ACTION__ == 2) { \
        _OPTION_ += 1; \
        if (_OPTION_[0] == '\0') __ACTION__ = 0; \
      } \
      if (__ACTION__ == 0) { \
        __i__ += 1; \
        if (__i__ >= argc) break; \
        _OPTION_ = argv[__i__]; \
      } \
      if (__ACTION__ == 1)  (void) fprintf(stderr, "Version: %s\nUsage:\n %s\n", VERSION, argv[0]); \
      stmts;  \
      if (__ACTION__ == 1 || __ACTION__ == 4) { \
        (void) putc('\n', stderr); \
        exit(1); \
      } \
      if (__ACTION__ == 3) { \
        (void) putc(' ', stderr); \
        __ACTION__ = 5; \
      } else { \
         __ACTION__ = 1; \
      } \
    } \
  }


#endif /* CMDLINE_H */
