# $Id: Makefile.am,v 1.6 2004/08/24 10:01:53 diemo Exp $
#
# Makefile.am for building the dtd direct to disk package
#
# $Log: Makefile.am,v $

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
