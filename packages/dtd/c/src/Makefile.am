packagedir=$(pkgdatadir)/packages/dtd/c
package_LTLIBRARIES=libdtd.la
libdtd_la_SOURCES= \
dtd.c \
readsf.c \
writesf.c \
libtime.c 

libdtd_la_LDFLAGS=-laudiofile -lpthread

noinst_HEADERS=  \
dtddefs.h \
dtdfifo.h \
dtdserver.h \
libtime.h

INCLUDES=-I$(top_srcdir)/include
