packagedir=$(pkgdatadir)/packages/numeric/c
package_LTLIBRARIES=libnumeric.la
libnumeric_la_SOURCES= \
numeric.c \
fft.c \
rewrap.c \
wrap.c 

INCLUDES=-I@top_srcdir@/packages/data/c/src -I@top_srcdir@/packages/utils/c/src

noinst_HEADERS= \
numeric.h

libnumeric_la_LDFLAGS= -module

