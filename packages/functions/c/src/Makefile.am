packagedir=$(pkglibdir)/packages/functions/c
package_LTLIBRARIES=libfunctions.la
libfunctions_la_SOURCES= \
functions.c 

noinst_HEADERS= \
mathfuns.h

libfunctions_la_LDFLAGS= -L$(top_builddir)/fts -lfts  -module

INCLUDES=-I$(top_srcdir)/include
