# $Id$
#
# Makefile.am for building aflib, the audio file lib wrapper
#
# $Log$

packagedir=$(pkglibdir)/packages/aflib/c
package_LTLIBRARIES=libaflib.la
libaflib_la_SOURCES= aflib.c 
libaflib_la_LDFLAGS= -L$(top_builddir)/fts -lfts @AUDIOFILE_LIBS@ -release @JMAX_VERSION@

# wreaks havoc on automake: CFLAGS  = @AUDIOFILE_CFLAGS@
INCLUDES=-I$(top_srcdir)/include @AUDIOFILE_CFLAGS@
