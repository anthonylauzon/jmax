packagedir=$(pkglibdir)/packages/numeric/c
package_LTLIBRARIES=libnumeric.la
libnumeric_la_SOURCES= \
numeric.c \
abs.c \
clip.c \
count.c \
drunk.c \
exscale.c \
fit.c \
for.c \
logscale.c \
ramp.c \
random.c \
range.c \
rewrap.c \
wrap.c 

INCLUDES=-I$(top_srcdir)/include -I$(top_srcdir)/packages/utils/c/include 

noinst_HEADERS= \
numeric.h

libnumeric_la_LDFLAGS= -L$(top_builddir)/fts -lfts  -module -release @JMAX_VERSION@

