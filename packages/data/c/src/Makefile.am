packagedir=$(pkgdatadir)/packages/data/c
package_LTLIBRARIES=libdata.la
libdata_la_SOURCES= \
bpf.c \
col.c \
cvec.c \
data.c \
fmat.c \
fvec.c \
getinter.c \
getrange.c \
getsize.c \
getval.c \
ivec.c \
mat.c \
dict.c \
preset.c \
row.c \
value.c \
vec.c \
dumpfile.c

INCLUDES=-I$(top_srcdir)/packages -I$(top_srcdir)/include

noinst_HEADERS= \
col.h \
mat.h \
dict.h \
preset.h \
row.h \
value.h \
vec.h

libdata_la_LDFLAGS=-module

