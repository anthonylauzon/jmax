packagedir=$(pkgdatadir)/packages/numeric/c
package_LTLIBRARIES=libnumeric.la
libnumeric_la_SOURCES= \
numeric.c \
abs.c \
clip.c \
drunk.c \
exscale.c \
fit.c \
logscale.c \
random.c \
range.c \
rewrap.c \
wrap.c 

INCLUDES=-I$(top_srcdir)/include -I$(top_srcdir)/packages

noinst_HEADERS= \
numeric.h

libnumeric_la_LDFLAGS= -module

