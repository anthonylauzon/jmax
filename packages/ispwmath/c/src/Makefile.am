packagedir=$(pkglibdir)/packages/ispwmath/c
package_LTLIBRARIES=libispwmath.la
libispwmath_la_SOURCES= \
binop.c \
ispwmath.c 

noinst_HEADERS= \
ispwmath.h

libispwmath_la_LDFLAGS= -L$(top_builddir)/fts -lfts  -module -release @JMAX_VERSION@

INCLUDES=-I$(top_srcdir)/include
