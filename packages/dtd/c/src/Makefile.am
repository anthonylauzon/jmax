packagedir=$(pkglibdir)/packages/dtd/c
package_LTLIBRARIES=libdtd.la
libdtd_la_SOURCES= \
dtd.c \
dtd_thread.c \
readsf.c \
writesf.c

libdtd_la_LDFLAGS= -L$(top_builddir)/fts -lfts -laudiofile -lpthread -release @JMAX_VERSION@

noinst_HEADERS=  \
dtd_buffer.h \
dtd_thread.h

INCLUDES=-I$(top_srcdir)/include
