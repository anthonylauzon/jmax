packagedir=$(pkglibdir)/packages/midishare/c
package_LTLIBRARIES=libmidishare.la
libmidishare_la_SOURCES= \
midishareport.c 

libmidishare_la_CFLAGS = @MIDISHARE_CFLAGS@
libmidishare_la_LDFLAGS = -L$(top_builddir)/fts -lfts @MIDISHARE_LDFLAGS@ -release @JMAX_VERSION@

INCLUDES=-I$(top_srcdir)/include

