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

if MACOSX
XLDFLAGS=  -no-undefined -module
XLIBADD= -L$(top_srcdir)/fts/.libs -lfts
endif
libutils_la_LDFLAGS=$(XLDFLAGS)
libutils_la_LIBADD=$(XLIBADD)

noinst_HEADERS= \
cfft.h \
rfft_shuffle.h \
utils.h
