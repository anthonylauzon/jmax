packagedir=$(pkglibdir)/packages/utils/c
package_LTLIBRARIES=libutils.la
libutils_la_SOURCES= \
cfft.c \
complex.c \
cubic.c \
floatfuns.c \
ramp_vectors.c \
ramps.c \
random.c \
rfft_shuffle.c \
utils.c \
vec_fft.c 

noinst_HEADERS= \
cfft.h \
rfft_shuffle.h

libutils_la_LDFLAGS= -L$(top_builddir)/fts -lfts  -module -release @JMAX_VERSION@

INCLUDES=-I$(top_srcdir)/packages/utils/c/include -I$(top_srcdir)/include 
