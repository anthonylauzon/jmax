packagedir=$(pkgdatadir)/packages/dtd/c
package_LTLIBRARIES=libdtd.la
libdtd_la_SOURCES= \
dtdfifo.c \
dtdmain.c \
dtdmodule.c \
dtdobjs.c \
dtdserver.c \
dummydtd.c \
libtime.c 

libdtd_la_LDFLAGS=-laudiofile -lpthread

noinst_HEADERS=  \
dtddefs.h \
dtdfifo.h \
dtdserver.h \
libtime.h

INCLUDES=-I$(top_srcdir)/include
