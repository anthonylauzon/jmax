packagedir=$(pkglibdir)/packages/midishare/c
package_LTLIBRARIES=libmidishare.la
libmidishare_la_SOURCES= \
midishareport.c 

libmidishare_la_CFLAGS = @MIDISHARE_CFLAGS@
libmidishare_la_LIBADD = -L$(top_builddir)/fts -lfts @MIDISHARE_LDFLAGS@

