# $Id: Makefile.am,v 1.7 2004/08/24 10:03:21 diemo Exp $
#
# Makefile.am for building the dtd direct to disk package
#
# $Log: Makefile.am,v $
# Revision 1.7  2004/08/24 10:03:21  diemo
# The configuration option --with-audiofile-prefix wasn't actually used
# to find the installed development version of the audiofile lib.
#

packagedir=$(pkglibdir)/packages/dtd/c
package_LTLIBRARIES=libdtd.la
libdtd_la_SOURCES= \
dtd.c \
dtd_thread.c \
readsf.c \
writesf.c

libdtd_la_LDFLAGS = -L$(top_builddir)/fts -lfts @AUDIOFILE_LIBS@ -lpthread -release @JMAX_VERSION@

noinst_HEADERS=  \
dtd_buffer.h \
dtd_thread.h

INCLUDES=-I$(top_srcdir)/include
