packagedir=$(pkglibdir)/packages/aflib/c
package_LTLIBRARIES=libaflib.la
libaflib_la_SOURCES= aflib.c 
libaflib_la_LDFLAGS=-L$(top_builddir)/fts -lfts -laudiofile -release @JMAX_VERSION@

INCLUDES=-I$(top_srcdir)/include
