packagedir=$(pkglibdir)/packages/functions/c
package_LTLIBRARIES=libfunctions.la
libfunctions_la_SOURCES= \
functions.c 

noinst_HEADERS= \
mathfuns.h

libfunctions_la_LDFLAGS= -L$(top_builddir)/fts -lfts  -module -release @JMAX_VERSION@

INCLUDES=-I$(top_srcdir)/include
