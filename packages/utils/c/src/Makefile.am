packagedir=$(pkgdatadir)/packages/utils/c
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

libutils_la_LDFLAGS= -no-undefined -module
libutils_la_LIBADD= -L$(top_srcdir)/fts/.libs -lfts

noinst_HEADERS= \
cfft.h \
rfft_shuffle.h \
utils.h
