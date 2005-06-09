packagedir=$(pkglibdir)/packages/functions/c
package_LTLIBRARIES=libfunctions.la
libfunctions_la_SOURCES= \
functions.c 

# info function needs mat from data package, data needs utils package
INCLUDES=-I$(top_srcdir)/packages/utils/c/include -I$(top_srcdir)/packages/data/c/include -I$(top_srcdir)/include

noinst_HEADERS= \
mathfuns.h

libfunctions_la_LDFLAGS= -L$(top_builddir)/fts -lfts  -module -release @JMAX_VERSION@
