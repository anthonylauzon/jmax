# $Id: Makefile.am,v 1.6 2004/08/24 10:01:53 diemo Exp $
#
# Makefile.am for building aflib, the audio file lib wrapper
#
# $Log: Makefile.am,v $
# Revision 1.6  2004/08/24 10:01:53  diemo
# The configuration option --with-audiofile-prefix wasn't actually used
# to build the aflib package to find the installed development version
# of the audiofile lib.
#

packagedir=$(pkglibdir)/packages/aflib/c
package_LTLIBRARIES=libaflib.la
libaflib_la_SOURCES= aflib.c 
libaflib_la_LDFLAGS= -L$(top_builddir)/fts -lfts @AUDIOFILE_LIBS@ -release @JMAX_VERSION@

# wreaks havoc on automake: CFLAGS  = @AUDIOFILE_CFLAGS@
INCLUDES=-I$(top_srcdir)/include @AUDIOFILE_CFLAGS@
