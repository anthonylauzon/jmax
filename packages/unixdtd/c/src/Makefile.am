packagedir=$(pkgdatadir)/packages/unixdtd/c
package_LTLIBRARIES=libunixdtd.la
libunixdtd_la_SOURCES= \
dtdfifo.c \
dtdmain.c \
dtdmodule.c \
dtdobjs.c \
dtdserver.c \
libtime.c 

package_PROGRAMS=dtd
dtd_SOURCES= \
dtdmain.c \
dtdfifo.c
dtd_LDADD=-laudiofile

noinst_HEADERS=  \
dtddefs.h \
dtdfifo.h \
dtdserver.h \
libtime.h

INCLUDES=-I$(top_srcdir)/include
