packagedir=$(pkgdatadir)/packages/data/c
package_LTLIBRARIES=libdata.la
libdata_la_SOURCES= \
bpf.c \
col.c \
cvec.c \
data.c \
fmat.c \
fvec.c \
getrange.c \
ivec.c \
mat.c \
dict.c \
row.c \
tabeditor.c \
vec.c \
dumpfile.c

INCLUDES=-I$(top_srcdir)/packages/utils/c/include -I$(top_srcdir)/packages/data/c/include -I$(top_srcdir)/include

noinst_HEADERS= \
col.h \
row.h \
tabeditor.h

libdata_la_LDFLAGS=-module

