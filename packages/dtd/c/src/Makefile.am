packagedir=$(pkgdatadir)/packages/dtd/c
package_LTLIBRARIES=libdtd.la
libdtd_la_SOURCES= \
dtd.c \
dtd_thread.c \
readsf.c \
writesf.c

libdtd_la_LDFLAGS=-laudiofile -lpthread

noinst_HEADERS=  \
dtd_buffer.h \
dtd_thread.h

INCLUDES=-I$(top_srcdir)/include
