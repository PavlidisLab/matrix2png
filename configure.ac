dnl Process this file with autoconf to produce a configure script.
AC_INIT(matrix2png.c)
AM_INIT_AUTOMAKE(matrix2png, 1.2.3)

#AC_INIT(text2png.c)
#AM_INIT_AUTOMAKE(text2png, 0.9)

dnl Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL

dnl Checks for libraries.
AC_CHECK_LIB(m, sin, , AC_MSG_FAILURE([You need to have libm installed and findable by the configure script]))
AC_CHECK_LIB(png, main, , AC_MSG_FAILURE([You need to have libpng installed and findable by the configure script]))
AC_CHECK_LIB(z, main, , AC_MSG_FAILURE([You need to have zlib installed and findable by the configure script]))
AC_CHECK_LIB(gd, gdImageCreate, , AC_MSG_FAILURE([You need to have libgd installed and findable by the configure script]))
AC_CHECK_LIB(freetype, FT_Load_Glyph, , AC_MSG_WARN([You might need to have libfreetype installed and findable by the configure script]))

dnl Messes up enlargeCanvas...
AC_CHECK_FUNCS(gdImageSetClip)

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(sys/time.h)
AC_CHECK_HEADERS(stdio.h)
AC_CHECK_HEADERS(assert.h)
AC_CHECK_HEADERS(string.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

dnl Checks for library functions.
AC_FUNC_VPRINTF
AC_CHECK_FUNCS(strstr strtod)

AC_OUTPUT(Makefile)
